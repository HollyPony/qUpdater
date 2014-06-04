#include "qupd_updater.h"

#include <QApplication>
#include <QFile>
#include  <QSettings>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "qupd_availableupdate.h"

// Ui
#include "ui/qupd_downloadupdatedialog.h"

void QUPDUpdater::startDownloadNewVersion()
{
    if (_newVersion) {
        _newVersionReply->abort();
        _newVersion->deleteLater();
        _newVersion = 0;
    }

    if (_newVersionReply) {
        _newVersionReply->deleteLater();
        _newVersionReply = 0;
    }

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, proposedUpdate()->enclosureType());
    request.setHeader(QNetworkRequest::UserAgentHeader, QApplication::applicationName());
    request.setUrl(proposedUpdate()->enclosureUrl());

    _newVersionRequestAborted = false;
    _newVersionReply = _qnam->get(request);

    connect(_newVersionReply, SIGNAL(finished()),
            this, SLOT(newVersionHttpDownloadFinished()));
    connect(_newVersionReply, SIGNAL(readyRead()),
            this, SLOT(newVersionHttpReadyRead()));
}

void QUPDUpdater::cancelDownloadNewVersion()
{
    if (_newVersionReply) {
        _newVersionRequestAborted = true;
        _newVersionReply->abort();
        _newVersionReply->deleteLater();
        _newVersionReply = 0;
    }

    if (_newVersion) {
        _newVersion->deleteLater();
        _newVersion = 0;
    }

    if (_newVersionRequestAborted)
        emit downloadNewVersionAborted();
}

void QUPDUpdater::newVersionHttpReadyRead()
{
    if (_newVersionReply && !_newVersionRequestAborted) {
        if (!_newVersion && _newVersionReply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull()) {

            QString url = _newVersionReply->url().toString(QUrl::RemoveScheme | QUrl::RemoveAuthority | QUrl::RemoveQuery);
            QString fileName = url.split("/").last();

            QString applicationPath = QCoreApplication::applicationFilePath();

#if defined Q_OS_MAC
            int index = applicationPath.lastIndexOf(".app");
            applicationPath = applicationPath.left(index);
            index = applicationPath.lastIndexOf("/");
            applicationPath = applicationPath.left(index);
            //    QFile::rename(bundlePath, bundlePath + "something.app");
#endif

            _newVersion = new QFile(applicationPath +"/"+ fileName);
            _newVersion->open(QIODevice::WriteOnly);

            if (_downloadUpdateDialog)
                connect(_newVersionReply, SIGNAL(downloadProgress(qint64,qint64)),
                        _downloadUpdateDialog, SLOT(updateProgressBar(qint64,qint64)));
        }

        if (_newVersion)
            _newVersion->write(_newVersionReply->readAll());
    }
}

void QUPDUpdater::newVersionHttpDownloadFinished()
{
    if (_newVersionReply ) {
        QVariant redirectionTarget = _newVersionReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (_newVersionRequestAborted) {
            _newVersionReply->deleteLater();
            _newVersionReply = 0;
        }
        else if (_newVersionReply->error()) {
            showErrorDialog(tr("New Version download failed: %1.").arg(_newVersionReply->errorString()), false);
            emit downloadNewVersionError();
            _newVersionReply->deleteLater();
            _newVersionReply = 0;
        } else if (! redirectionTarget.isNull()) {
            proposedUpdate()->setEnclosureUrl(_newVersionReply->url().resolved(redirectionTarget.toUrl()).toString());
            _newVersionReply->deleteLater();
            startDownloadNewVersion();
        } else {
            emit downloadNewVersionSuccess();
            _newVersionReply->deleteLater();
            _newVersionReply = 0;
            if (_newVersion && _newVersion->isOpen()) {
                _newVersion->flush();
                _newVersion->close();
            }
        }
    }
}
