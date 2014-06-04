#include "qupd_updaterdialoglight.h"
#include "ui_qupd_updaterdialoglight.h"

QUPDUpdaterDialogLight::QUPDUpdaterDialogLight(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QUPDUpdaterDialogLight)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, true);
}

QUPDUpdaterDialogLight::~QUPDUpdaterDialogLight()
{
    delete ui;
}
