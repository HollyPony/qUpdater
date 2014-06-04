#include "qupd_updater.h"

#include <QApplication>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamReader>

#include "qupd_platform.h"
#include "qupd_availableupdate.h"
#include "qupd_ignoredversions.h"

void QUPDUpdater::startDownloadFeed()
{
    if (_xml) {
        delete _xml;
        _xml = 0;
    }

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/xml");
    request.setHeader(QNetworkRequest::UserAgentHeader, QApplication::applicationName());
    request.setUrl(feedURL());

    _feedReply = _qnam->get(request);

    connect(_feedReply, SIGNAL(finished()), this, SLOT(feedHttpDownloadFinished()));
    connect(_feedReply, SIGNAL(readyRead()), this, SLOT(feedHttpReadyRead()));
}

void QUPDUpdater::cancelDownloadFeed()
{
    if (_feedReply) {
        _feedHttpRequestAborted = true;
        _feedReply->abort();
        _feedReply->deleteLater();
        _feedReply = 0;
    }

    if (_xml) {
        delete _xml;
        _xml = 0;
    }
}

void QUPDUpdater::feedHttpReadyRead()
{
    if (_feedReply) {
        if (!_xml && _feedReply->attribute(QNetworkRequest::RedirectionTargetAttribute).isNull()) {
            _xml = new QXmlStreamReader;
        }
        if (_xml)
            _xml->addData(_feedReply->readAll());
    }
}

void QUPDUpdater::feedHttpDownloadFinished()
{
    if (_feedReply ) {
        QVariant redirectionTarget = _feedReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
        if (_feedHttpRequestAborted) {
            _feedReply->deleteLater();
            _feedReply = 0;
            if (_xml) {
                delete _xml;
                _xml = 0;
            }
        }
        else if (_feedReply->error()) {
            showErrorDialog(tr("Feed download failed: %1.").arg(_feedReply->errorString()), false);
            _feedReply->deleteLater();
            _feedReply = 0;
            if (_xml) {
                delete _xml;
                _xml = 0;
            }
        } else if (! redirectionTarget.isNull()) {
            setFeedURL(_feedReply->url().resolved(redirectionTarget.toUrl()).toString());
            _feedReply->deleteLater();
            startDownloadFeed();
        } else {
            xmlParseFeed();
            _feedReply->deleteLater();
            _feedReply = 0;
            if (_xml) {
                delete _xml;
                _xml = 0;
            }
        }
    }
}

// TODO rework
bool QUPDUpdater::xmlParseFeed()
{
    if (_xml) {
        QString currentTag;
        QString currentQualifiedTag;

        QString xmlTitle;
        QString xmlLink;
        QString xmlPubDate;
        QString xmlEnclosureUrl;
        QString xmlEnclosureVersion;
        QString xmlEnclosurePlatform;
        QString xmlEnclosureType;

        ulong   xmlEnclosureLength = 0;

        // Parse
        while (! _xml->atEnd()) {
            _xml->readNext();
            if (_xml->isStartElement()) {
                currentTag = _xml->name().toString();
                currentQualifiedTag = _xml->qualifiedName().toString();

                if (_xml->name() == "item") {
                    xmlTitle.clear();
                    xmlLink.clear();
                    xmlPubDate.clear();
                    xmlEnclosureUrl.clear();
                    xmlEnclosureVersion.clear();
                    xmlEnclosurePlatform.clear();
                    xmlEnclosureLength = 0;
                    xmlEnclosureType.clear();
                } else if (_xml->name() == "enclosure") {
                    QXmlStreamAttributes attribs = _xml->attributes();
                    if (attribs.hasAttribute("qtupdater:platform")) {

                        if (QUPDPlatform::CurrentlyRunningOnPlatform(attribs.value("qtupdater:platform").toString().trimmed())) {
                            xmlEnclosurePlatform = attribs.value("qtupdater:platform").toString().trimmed();

                            if (attribs.hasAttribute("url")) {
                                xmlEnclosureUrl = attribs.value("url").toString().trimmed();
                            } else {
                                xmlEnclosureUrl = "";
                            }

                            if (attribs.hasAttribute("qtupdater:version")) {
                                QString candidateVersion = attribs.value("qtupdater:version").toString().trimmed();
                                if (! candidateVersion.isEmpty()) {
                                    xmlEnclosureVersion = candidateVersion;
                                }
                            }

                            if (attribs.hasAttribute("length")) {
                                xmlEnclosureLength = attribs.value("length").toString().toLong();
                            } else {
                                xmlEnclosureLength = 0;
                            }

                            if (attribs.hasAttribute("type")) {
                                xmlEnclosureType = attribs.value("type").toString().trimmed();
                            } else {
                                xmlEnclosureType = "";
                            }
                        }
                    }
                }
            } else if (_xml->isEndElement()) {
                if (_xml->name() == "item") {
                    // That's it - we have analyzed a single <item> and we'll stop
                    // here (because the topmost is the most recent one, and thus
                    // the newest version.
                    return searchDownloadedFeedForUpdates(xmlTitle,
                                                          xmlLink,
                                                          xmlPubDate,
                                                          xmlEnclosureUrl,
                                                          xmlEnclosureVersion,
                                                          xmlEnclosurePlatform,
                                                          xmlEnclosureLength,
                                                          xmlEnclosureType);
                }
            } else if (_xml->isCharacters() && !_xml->isWhitespace()) {
                if (currentTag == "title")
                    xmlTitle += _xml->text().toString().trimmed();
                else if (currentTag == "link")
                    xmlLink += _xml->text().toString().trimmed();
                else if (currentTag == "pubDate")
                    xmlPubDate += _xml->text().toString().trimmed();
            }

            if (_xml->error() && _xml->error() != QXmlStreamReader::PrematureEndOfDocumentError) {
                showErrorDialog(tr("Feed parsing failed: %1 %2.").arg(QString::number(_xml->lineNumber()), _xml->errorString()), false);
                return false;
            }
        }

        // No updates were found if we're at this point
        // (not a single <item> element found)
        showInformationDialog(tr("No updates were found."), false);
    }
    return false;
}

// TODO !!!!
bool QUPDUpdater::searchDownloadedFeedForUpdates(QString xmlTitle,
                                               QString xmlLink,
                                               QString xmlPubDate,
                                               QString xmlEnclosureUrl,
                                               QString xmlEnclosureVersion,
                                               QString xmlEnclosurePlatform,
                                               unsigned long xmlEnclosureLength,
                                               QString xmlEnclosureType)
{
    // Validate
    if (xmlLink.isEmpty()) {
        showErrorDialog(tr("Feed error: \"release notes\" link is empty"), false);
        return false;
    }
    if (! (xmlLink.startsWith("http://") || xmlLink.startsWith("https://"))) {
        showErrorDialog(tr("Feed error: invalid \"release notes\" link"), false);
        return false;
    }
    if (xmlEnclosureUrl.isEmpty() || xmlEnclosureVersion.isEmpty() || xmlEnclosurePlatform.isEmpty()) {
        showErrorDialog(tr("Feed error: invalid \"enclosure\" with the download link"), false);
        return false;
    }

    // Relevant version?
    if (QUPDIgnoredVersions::versionIsIgnored(xmlEnclosureVersion)) {
        showInformationDialog(tr("No updates were found."), false);

        return true; // Things have succeeded when you think of it.
    }

    if (_proposedUpdate) {
        delete _proposedUpdate;
        _proposedUpdate = 0;
    }
    _proposedUpdate = new QUPDAvailableUpdate();
    _proposedUpdate->setTitle(xmlTitle);
    _proposedUpdate->setReleaseNoteLink(xmlLink);
    _proposedUpdate->setPubDate(xmlPubDate);
    _proposedUpdate->setEnclosureUrl(xmlEnclosureUrl);
    _proposedUpdate->setEnclosureVersion(xmlEnclosureVersion);
    _proposedUpdate->setEnclosurePlatform(xmlEnclosurePlatform);
    _proposedUpdate->setEnclosureLength(xmlEnclosureLength);
    _proposedUpdate->setEnclosureType(xmlEnclosureType);

    // Show "look, there's an update" window
    showUpdaterWindowUpdatedWithCurrentUpdateProposal();

    return true;
}
