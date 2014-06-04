#ifndef QUPD_IGNOREDVERSIONS_H
#define QUPD_IGNOREDVERSIONS_H

#include <QObject>

const QString QUPD_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY = "QUPDLatestSkippedVersion"; // QSettings key for the latest skipped version

class QUPDIgnoredVersions : public QObject
{
    Q_OBJECT

public:

    static bool versionIsIgnored(QString version);
    static void ignoreVersion(QString version);

};

#endif // QUPD_IGNOREDVERSIONS_H
