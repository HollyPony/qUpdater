#ifndef QUPD_AVAILABLEUPDATE_H
#define QUPD_AVAILABLEUPDATE_H

#include <QObject>
#include <QUrl>

class QUPDAvailableUpdate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title             READ title             WRITE setTitle)
    Q_PROPERTY(QUrl releaseNoteLink      READ releaseNoteLink   WRITE setReleaseNoteLink)
    Q_PROPERTY(QString pubDate           READ pubDate           WRITE setPubDate)
    Q_PROPERTY(QUrl    enclosureUrl      READ enclosureUrl      WRITE setEnclosureUrl)
    Q_PROPERTY(QString enclosureVersion  READ enclosureVersion  WRITE setEnclosureVersion)
    Q_PROPERTY(QString enclosurePlatform READ enclosurePlatform WRITE setEnclosurePlatform)
    Q_PROPERTY(ulong   enclosureLength   READ enclosureLength   WRITE setEnclosureLength)
    Q_PROPERTY(QString enclosureType     READ enclosureType     WRITE setEnclosureType)

public:
    QString title();
    void    setTitle(QString title);

    QUrl releaseNoteLink() const { return _releaseNoteLink; }
    void setReleaseNoteLink(QUrl arg) { _releaseNoteLink = arg; }

    QString pubDate();
    void    setPubDate(QString pubDate);

    QUrl enclosureUrl();
    void setEnclosureUrl(QUrl    enclosureUrl);
    void setEnclosureUrl(QString enclosureUrl);

    QString enclosureVersion();
    void    setEnclosureVersion(QString enclosureVersion);

    QString enclosurePlatform();
    void    setEnclosurePlatform(QString enclosurePlatform);

    ulong enclosureLength();
    void  setEnclosureLength(ulong enclosureLength);

    QString enclosureType();
    void    setEnclosureType(QString enclosureType);


private:
    QString _title;
    QUrl _releaseNoteLink;
    QString _pubDate;
    QUrl    _enclosureUrl;
    QString _enclosureVersion;
    QString _enclosurePlatform;
    ulong   _enclosureLength;
    QString _enclosureType;

};

#endif // QUPD_AVAILABLEUPDATE_H
