#include "qupd_versioncomparator.h"

#include <QString>

QUPDVersionComparator::ComparatorResult QUPDVersionComparator::compareVersions(QString currentVersion,
                                                                               QString remoteVersion)
{
//    versionA.contains("alpha", Qt::CaseInsensitive);
//    versionA.contains("beta", Qt::CaseInsensitive);
//    versionB.contains("alpha", Qt::CaseInsensitive);
//    versionB.contains("beta", Qt::CaseInsensitive);

    ComparatorResult result;

    bool isRegularUpdate = true;

    bool test;
    double dA = currentVersion.toDouble(&test);
    if (!test) isRegularUpdate = false;
    double dB = remoteVersion.toDouble(&test);
    if (!test) isRegularUpdate = false;

    if (isRegularUpdate) {
        if (dA == dB) result = EQUAL;
        else if (dA > dB) result = SMALLER;
        else result = GREATER;
    }

    return result;
}
