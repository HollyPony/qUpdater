#ifndef QUPD_PLATFORM_H
#define QUPD_PLATFORM_H

#include <QObject>

class QUPDPlatform : public QObject
{
    Q_OBJECT

public:
    static bool CurrentlyRunningOnPlatform(QString platform);

private:
    explicit QUPDPlatform(QObject *parent = 0);

};

#endif // QUPD_PLATFORM_H
