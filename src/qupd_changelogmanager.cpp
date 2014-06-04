#include "qupd_updater.h"

#include <QApplication>
#include <QNetworkReply>
#include <QNetworkRequest>

#include "qupd_availableupdate.h"

void QUPDUpdater::startDownloadChangelog()
{
    _changelog = "";

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/html");
    request.setHeader(QNetworkRequest::UserAgentHeader, QApplication::applicationName());
    request.setUrl(proposedUpdate()->releaseNoteLink());

    _changelogReply = _qnam->get(request);

    connect(_changelogReply, SIGNAL(finished()),
            this, SLOT(changelogHttpDownloadFinished()));
    connect(_changelogReply, SIGNAL(readyRead()),
            this, SLOT(changelogHttpReadyRead()));
}

void QUPDUpdater::cancelDownloadChangelog()
{
    if (_changelogReply) {
        _changelogRequestAborted = true;
        _changelogReply->abort();
        _changelogReply->deleteLater();
        _changelogReply = 0;
    }

    _changelog = "";

    emit downloadChangelogAborted();
}

void QUPDUpdater::changelogHttpReadyRead()
{
    if (_changelogReply && !_changelogRequestAborted) {
        if (_changelogReply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull()) {
            _changelog = "";
        }

        _changelog += _changelogReply->readAll();
    }
}

void QUPDUpdater::changelogHttpDownloadFinished()
{
    if (_changelogReply ) {
        QVariant redirectionTarget = _changelogReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (_changelogRequestAborted) {
            _changelogReply->deleteLater();
            _changelogReply = 0;
        }
        else if (_changelogReply->error()) {
            showErrorDialog(tr("New Version download failed: %1.").arg(_changelogReply->errorString()), false);
            emit downloadChangelogError();
            _changelogReply->deleteLater();
            _changelogReply = 0;
        } else if (! redirectionTarget.isNull()) {
            proposedUpdate()->setReleaseNoteLink(_changelogReply->url().resolved(redirectionTarget.toUrl()).toString());
            _changelogReply->deleteLater();
            startDownloadChangelog();
        } else {
            emit downloadChangelogSuccess();

            _changelogReply->deleteLater();
            _changelogReply = 0;
        }
    }
}
