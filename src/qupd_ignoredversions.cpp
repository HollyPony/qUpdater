#include "qupd_ignoredversions.h"

#include <QApplication>
#include <QSettings>

#include "qupd_versioncomparator.h"

bool QUPDIgnoredVersions::versionIsIgnored(QString remoteVersion)
{
    QString currentAppVersion = qApp->applicationVersion();
    QString lastSkippedVersion ="";

    if (QSettings().contains(QUPD_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY))
        lastSkippedVersion = QSettings().value(QUPD_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY, "").toString();

    bool isGreater  = (QUPDVersionComparator::compareVersions(currentAppVersion, remoteVersion) == QUPDVersionComparator::GREATER);

    // The version is ignored if ...
    return  (remoteVersion == lastSkippedVersion) || !isGreater;
}

void QUPDIgnoredVersions::ignoreVersion(QString version)
{
    if (!version.isEmpty() || version != qApp->applicationVersion())
        QSettings().setValue(QUPD_IGNORED_VERSIONS_LATEST_SKIPPED_VERSION_KEY, version);
}
