#ifndef QUPD_DOWNLOADUPDATE_H
#define QUPD_DOWNLOADUPDATE_H

#include <QDialog>

namespace Ui {
class QUPDDownloadUpdateDialog;
}

class QUPDDownloadUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QUPDDownloadUpdateDialog(QWidget *parent = 0);
    ~QUPDDownloadUpdateDialog();

public slots:
    void updateProgressBar(qint64 bytesRead, qint64 totalBytes);
    void downloadComplete();

signals:
    void installAndRelaunch();

private:
    Ui::QUPDDownloadUpdateDialog *ui;
};

#endif // QUPD_DOWNLOADUPDATE_H
