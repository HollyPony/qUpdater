#ifndef QUPD_UPDATERDIALOGLIGHT_H
#define QUPD_UPDATERDIALOGLIGHT_H

#include <QDialog>

namespace Ui {
class QUPDUpdaterDialogLight;
}

class QUPDUpdaterDialogLight : public QDialog
{
    Q_OBJECT

public:
    explicit QUPDUpdaterDialogLight(QWidget *parent = 0);
    ~QUPDUpdaterDialogLight();

private:
    Ui::QUPDUpdaterDialogLight *ui;
};

#endif // QUPD_UPDATERDIALOGLIGHT_H
