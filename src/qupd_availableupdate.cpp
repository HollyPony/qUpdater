#include "qupd_availableupdate.h"

QString QUPDAvailableUpdate::title()
{
    return _title;
}

void QUPDAvailableUpdate::setTitle(QString title)
{
    _title = title;
}

QString QUPDAvailableUpdate::pubDate()
{
    return _pubDate;
}

void QUPDAvailableUpdate::setPubDate(QString pubDate)
{
    _pubDate = pubDate;
}

QUrl QUPDAvailableUpdate::enclosureUrl()
{
    return _enclosureUrl;
}

void QUPDAvailableUpdate::setEnclosureUrl(QUrl enclosureUrl)
{
    _enclosureUrl = enclosureUrl;
}

void QUPDAvailableUpdate::setEnclosureUrl(QString enclosureUrl)
{
    setEnclosureUrl(QUrl(enclosureUrl));
}

QString QUPDAvailableUpdate::enclosureVersion()
{
    return _enclosureVersion;
}

void QUPDAvailableUpdate::setEnclosureVersion(QString enclosureVersion)
{
    _enclosureVersion = enclosureVersion;
}

QString QUPDAvailableUpdate::enclosurePlatform()
{
    return _enclosurePlatform;
}

void QUPDAvailableUpdate::setEnclosurePlatform(QString enclosurePlatform)
{
    _enclosurePlatform = enclosurePlatform;
}

ulong QUPDAvailableUpdate::enclosureLength()
{
    return _enclosureLength;
}

void QUPDAvailableUpdate::setEnclosureLength(ulong enclosureLength)
{
    _enclosureLength = enclosureLength;
}

QString QUPDAvailableUpdate::enclosureType()
{
    return _enclosureType;
}

void QUPDAvailableUpdate::setEnclosureType(QString enclosureType)
{
    _enclosureType = enclosureType;
}
