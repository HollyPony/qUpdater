#ifndef QUPD_UPDATER_H
#define QUPD_UPDATER_H

#include <QObject>

class QXmlStreamReader;
class QNetworkAccessManager;
class QNetworkReply;
class QUPDAvailableUpdate;
class QFile;

class QUPDDownloadUpdateDialog;
class QDialog;

#if defined QTUPDATER_LIBRARY
#define QTUPDATER_LIBRARY_COMMON_DLLSPEC  Q_DECL_EXPORT
#else
#define QTUPDATER_LIBRARY_COMMON_DLLSPEC Q_DECL_IMPORT
#endif

const QString QUPD_APPLICATION_UPDATE_NAME_KEY = "QUPDApplicationUpdateName";

class QTUPDATER_LIBRARY_COMMON_DLLSPEC QUPDUpdater : public QObject
{
    Q_OBJECT

public:
    QUPDAvailableUpdate* proposedUpdate();

    // Accessors
    void setParentWidget(QWidget *parent) { _parentWidget = parent; }
    QWidget *parentWidget() { return _parentWidget; }

    bool isSilentMode() { return _silentMode; }
    bool isUpdateRequested();

public slots:
    bool checkForUpdates(bool silentMode = true);

private slots:
    // Update window button slots
    void installUpdate();
    void skipUpdate();
    void remindMeLater();
    void installAndRelaunch();

private:
    QWidget              *_parentWidget;
    QUPDAvailableUpdate  *_proposedUpdate;

    // Windows instance
    QUPDDownloadUpdateDialog *_downloadUpdateDialog;
    QDialog                  *_updaterDialog;

    // If true, don't show the error dialogs and the "no updates." dialog
    // (silentAsMuchAsItCouldGet from CheckForUpdates() goes here)
    // Useful for automatic update checking upon application startup.
    bool _silentMode;

    // Download management
    QNetworkAccessManager *_qnam;

//****************************************************************************//
// **                         Singleton Buisness                           ** \\
//****************************************************************************//
public:
    static QUPDUpdater* instance();
    static void drop();

private:
    QUPDUpdater();                              // Hide main constructor
    ~QUPDUpdater();                             // Hide main destructor
    QUPDUpdater(const QUPDUpdater&);            // Hide copy constructor
    QUPDUpdater& operator=(const QUPDUpdater&); // Hide assign op

    static QUPDUpdater* _instance;             // Singleton instance

//****************************************************************************//
// **                           Private methods                            ** \\
//****************************************************************************//
private:
    void installTranslator(); // Initialize translation mechanism

    // DownlodUpdateDialog Control
    void showDownloadUpdateDialogWithCurrentProposal();
    void hideDownloadUpdateDialog();

    // UpdateWindow Control
    void showUpdaterWindowUpdatedWithCurrentUpdateProposal();
    void hideUpdaterWindow();

    // Dialogs (notifications)
    void showErrorDialog(QString message, bool showEvenInSilentMode = false);       // Show an error message
    void showInformationDialog(QString message, bool showEvenInSilentMode = false); // Show an informational message

//****************************************************************************//
// **                           Feed Management                            ** \\
//****************************************************************************//
public: void    setFeedURL(QString feedURL) { _feedURL = feedURL; }
public: QString feedURL()                   { return _feedURL; }

signals:
    void downloadFeedAborted();
    void downloadFeedError();
    void downloadFeedSuccess();

private slots:
    void feedHttpReadyRead();
    void feedHttpDownloadFinished();

private:
    void startDownloadFeed();
    void cancelDownloadFeed();

    // Xml methods
    bool xmlParseFeed();   // Parse feed in m_xml
    bool searchDownloadedFeedForUpdates(QString xmlTitle,
                                        QString xmlLink,
                                        QString xmlPubDate,
                                        QString xmlEnclosureUrl,
                                        QString xmlEnclosureVersion,
                                        QString xmlEnclosurePlatform,
                                        unsigned long xmlEnclosureLength,
                                        QString xmlEnclosureType);

    QString           _feedURL; // Feed URL that will be fetched
    QNetworkReply    *_feedReply;
    bool              _feedHttpRequestAborted;

    // XML parser
    QXmlStreamReader *_xml; // XML data collector and parser

//****************************************************************************//
// **                        Changelog Management                          ** \\
//****************************************************************************//
public:
    const QString changelog() { return _changelog; }

signals:
    void downloadChangelogAborted();
    void downloadChangelogError();
    void downloadChangelogSuccess();

private slots:
    void changelogHttpReadyRead();
    void changelogHttpDownloadFinished();

private:
    void startDownloadChangelog();  // Start download changelog
    void cancelDownloadChangelog(); // Stop downloading

    QNetworkReply *_changelogReply;
    bool           _changelogRequestAborted;

    QString        _changelog;

//****************************************************************************//
// **                       NewVersion Management                          ** \\
//****************************************************************************//
signals:
    void newVersionHttpUpdateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

    void downloadNewVersionAborted();
    void downloadNewVersionError();
    void downloadNewVersionSuccess();

private slots:
    void newVersionHttpReadyRead();
    void newVersionHttpDownloadFinished();

private:
    void startDownloadNewVersion();  // Start download new version
    void cancelDownloadNewVersion(); // Stop downloading

    QNetworkReply *_newVersionReply;
    bool           _newVersionRequestAborted;

    QFile         *_newVersion;
};

#endif // QUPD_UPDATER_H
