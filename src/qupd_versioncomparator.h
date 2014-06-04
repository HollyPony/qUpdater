#ifndef QUPD_VERSIONCOMPARATOR_H
#define QUPD_VERSIONCOMPARATOR_H

#include <QString>

class QUPDVersionComparator
{
public:

    enum ComparatorResult {
        EQUAL,
        GREATER,
        SMALLER
    };

    // Return GREATER if remoteVersion is Greater.
    static ComparatorResult compareVersions(QString currentVersion,
                                            QString remoteVersion);

};

#endif // QUPD_VERSIONCOMPARATOR_H
