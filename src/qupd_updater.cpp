#include "qupd_updater.h"

#include <QDebug>

#include <QApplication>
#include <QProcess>
#include <QNetworkAccessManager>
#include <QMessageBox>
#include <QMutex>
#include <QTranslator>
#include <QLibraryInfo>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>

#include "qupd_ignoredversions.h"
#include "qupd_availableupdate.h"

// Ui
#include "ui/qupd_downloadupdatedialog.h"
#include "ui/qupd_updaterdialogfull.h"

#if defined Q_OS_WIN
extern Q_CORE_EXPORT int qt_ntfs_permission_lookup; // Allow permissions verification os NTFS file system
#endif

const QString QUPD_UPDATED_VERSION_KEY = "QUPDUpdatedVersionKey"; // QSettings key for the path of the updated version

QUPDUpdater* QUPDUpdater::_instance = 0;

QUPDUpdater::QUPDUpdater()
    : QObject(0)
    , _parentWidget(0)
    , _proposedUpdate(0)
    , _downloadUpdateDialog(0)
    , _updaterDialog(0)
    , _qnam(new QNetworkAccessManager(this))
    , _feedReply(0)
    , _xml(0)
    , _changelogReply(0)
    , _newVersionReply(0)
    , _newVersion(0)
{
    installTranslator();
}

QUPDUpdater::~QUPDUpdater()
{
    if (_proposedUpdate) {
        delete _proposedUpdate;
        _proposedUpdate = 0;
    }

    _qnam->deleteLater();

    hideDownloadUpdateDialog();
    hideUpdaterWindow();
}

QUPDUpdater* QUPDUpdater::instance()
{
    if (!_instance) {
        static QMutex mutex;
        mutex.lock();
        _instance = new QUPDUpdater;
        mutex.unlock();
    }

    return _instance;
}

void QUPDUpdater::drop()
{
    static QMutex mutex;
    mutex.lock();
    delete _instance;
    _instance = 0;
    mutex.unlock();
}

void QUPDUpdater::installTranslator()
{
    QString locale = QLocale::system().name();

    QTranslator qtTranslator;
    if (qtTranslator.load("qt_"+ locale,
                          QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
        qApp->installTranslator(&qtTranslator);

    QTranslator appTranslator;
    if (appTranslator.load(":/tr/qtupdater_" + locale))
        qDebug() << "Install translator for" << locale << qApp->installTranslator(&appTranslator);
}

// DownloadUpdateDialog
void QUPDUpdater::showDownloadUpdateDialogWithCurrentProposal()
{
    hideDownloadUpdateDialog();

    QWidget *parent = 0;
    if (_updaterDialog) parent = _updaterDialog;
    else                parent = parentWidget();

    _downloadUpdateDialog = new QUPDDownloadUpdateDialog(parent);
    startDownloadNewVersion();
    connect(this, SIGNAL(downloadNewVersionSuccess()),
            _downloadUpdateDialog, SLOT(downloadComplete()));
    connect(_downloadUpdateDialog, SIGNAL(installAndRelaunch()),
            this, SLOT(installAndRelaunch()));
    _downloadUpdateDialog->show();
}

void QUPDUpdater::hideDownloadUpdateDialog()
{
    if (_downloadUpdateDialog) {
        if (!_downloadUpdateDialog->close())
            qWarning() << "Update window didn't close, leaking memory from now on";
        _downloadUpdateDialog = 0;
    }
}

// UpdaterWindow
void QUPDUpdater::showUpdaterWindowUpdatedWithCurrentUpdateProposal()
{
    hideUpdaterWindow();

    _updaterDialog = new QUPDUpdaterDialogFull(parentWidget());
    connect(this, SIGNAL(downloadChangelogSuccess()),
            _updaterDialog, SLOT(changelogDownloaded()));
    startDownloadChangelog();
    _updaterDialog->show();
}

void QUPDUpdater::hideUpdaterWindow()
{
    if (_updaterDialog) {
        _updaterDialog->close();
        _updaterDialog = 0;
    }
}

bool QUPDUpdater::isUpdateRequested()
{
    qDebug() << "isUpdateRequested";

    if (QSettings().contains(QUPD_UPDATED_VERSION_KEY)) {

        QFileInfo updatedFile (QSettings().value(QUPD_UPDATED_VERSION_KEY).toString());
        QFileInfo applicationFile ( qApp->applicationFilePath() );

#if defined Q_OS_MAC
        QDir dir = applicationFile.dir();
        dir.cdUp(); dir.cdUp();
        applicationFile = QFileInfo( dir.absolutePath() );

#elif defined Q_OS_WIN
        qt_ntfs_permission_lookup++; // Allow permissions verification os NTFS file system

#endif

        if (!updatedFile.exists()) {
            ; // TODO Error
        } else if (!applicationFile.isWritable()) {
            qDebug() << "Unwritable file, update aborted";
        } else {

#if defined Q_OS_WIN
            ;

#elif defined Q_OS_MAC
            QProcess process;
            process.setProgram("hdiutil");
            process.setArguments(QStringList() << "verify" << updatedFile.absoluteFilePath());
            process.start();
            process.waitForFinished();
            if (process.exitStatus() == QProcess::NormalExit) {
                QString mountPoint = applicationFile.absolutePath() + "/" + applicationFile.baseName();

                // Mount the downloaded dmg
                process.setProgram("hdiutil");
                process.setArguments(QStringList() << "attach" << "-mountpoint" << mountPoint << updatedFile.absoluteFilePath());
                process.start();
                process.waitForFinished(180000);
                qDebug() << process.exitStatus();
                qDebug() << process.readAllStandardError();

                // Replace the Application
                QString newFile(mountPoint + "/" +
                              qApp->applicationName() +".app");
                QString oldFile(applicationFile.absoluteFilePath());

                process.setProgram("rm");
                process.setArguments(QStringList() << "-rf" << oldFile);
                process.start();
                process.waitForFinished();
                process.exitStatus();

                process.setProgram("cp");
                process.setArguments(QStringList() << "-r" << newFile << oldFile);
                process.start();
                process.waitForFinished();

                // Unmount it
                process.setProgram("hdiutil");
                process.setArguments(QStringList() << "detach" << applicationFile.absolutePath() + "/" + applicationFile.baseName());
                process.start();
                process.waitForFinished();
                process.exitCode();
                process.exitStatus();
            }
#endif

        }

#if defined Q_OS_WIN
        qt_ntfs_permission_lookup--;
#endif

        QSettings().remove(QUPD_UPDATED_VERSION_KEY);
    }

    return false;
}

// Accessors
QUPDAvailableUpdate *QUPDUpdater::proposedUpdate() { return _proposedUpdate; }

// SLOTS
void QUPDUpdater::installUpdate() // Rename -> DownloadUpdate
{
    qDebug() << "Download update";
    showDownloadUpdateDialogWithCurrentProposal();
}

void QUPDUpdater::skipUpdate()
{
    qDebug() << "Skip update";
    if (proposedUpdate()) {
        QUPDIgnoredVersions::ignoreVersion(proposedUpdate()->enclosureVersion());
        hideUpdaterWindow();
    }
}

void QUPDUpdater::remindMeLater()
{
    qDebug() << "Remind me later";
    hideUpdaterWindow();
}

void QUPDUpdater::installAndRelaunch() // TODO Rename method
{
    qDebug() << "Install and Relaunch";

    if (_newVersion) {
        qDebug() << _newVersion->fileName();
        QSettings().setValue(QUPD_UPDATED_VERSION_KEY, _newVersion->fileName());
    }

    QProcess::startDetached(qApp->applicationFilePath());
    qApp->quit();
}

bool QUPDUpdater::checkForUpdates(bool silentMode)
{
    bool canCheck = true;
    if (_feedURL.isEmpty()) {
        qCritical() << "Please set feed URL via setFeedURL() before calling CheckForUpdates().";
        canCheck = false;
    } else if (QApplication::organizationName().isEmpty()) {
        qCritical() << "QApplication::organizationName is not set. Please do that.";
        canCheck = false;
    } else if (QApplication::organizationDomain().isEmpty()) {
        qCritical() << "QApplication::organizationDomain is not set. Please do that.";
        canCheck = false;
    }

    if (canCheck) {
        _silentMode = silentMode;
        // Set application name / version is not set yet
        if (QApplication::applicationName().isEmpty()) {
            QString appName = qApp->applicationName();
            qWarning() << "QApplication::applicationName is not set, setting it to '" << appName << "'";
            QApplication::setApplicationName(appName);
        }

        if (QApplication::applicationVersion().isEmpty()) {
            QString appVersion = qApp->applicationVersion();
            qWarning() << "QApplication::applicationVersion is not set, setting it to '" << appVersion << "'";
            QApplication::setApplicationVersion(appVersion);
        }

        cancelDownloadFeed();
        _feedHttpRequestAborted = false;
        startDownloadFeed();
        canCheck = true;
    }

    return canCheck;
}

void QUPDUpdater::showErrorDialog(QString message, bool showEvenInSilentMode)
{
    // Don't show errors in the silent mode
    if (!_silentMode || showEvenInSilentMode) {
        QMessageBox dlFailedMsgBox;
        dlFailedMsgBox.setIcon(QMessageBox::Critical);
        dlFailedMsgBox.setText(tr("Error"));
        dlFailedMsgBox.setInformativeText(message);
        dlFailedMsgBox.exec();
    }
}

void QUPDUpdater::showInformationDialog(QString message, bool showEvenInSilentMode)
{
    // Don't show information dialogs in the silent mode
    if (!_silentMode || showEvenInSilentMode) {
        QMessageBox dlInformationMsgBox;
        dlInformationMsgBox.setIcon(QMessageBox::Information);
        dlInformationMsgBox.setText(tr("Information"));
        dlInformationMsgBox.setInformativeText(message);
        dlInformationMsgBox.exec();
    }
}

int main (int argc, char *argv[]) {
    return 0;
}
