#ifndef QUPD_UPDATERFULL_H
#define QUPD_UPDATERFULL_H

#include <QDialog>

namespace Ui {
class QUPDUpdaterDialogFull;
}

class QUPDUpdaterDialogFull : public QDialog
{
    Q_OBJECT

public:
    explicit QUPDUpdaterDialogFull(QWidget *parent = 0);
    ~QUPDUpdaterDialogFull();

public slots:
    void changelogDownloaded();

private:
    Ui::QUPDUpdaterDialogFull *ui;
};

#endif // QUPD_UPDATERFULL_H
