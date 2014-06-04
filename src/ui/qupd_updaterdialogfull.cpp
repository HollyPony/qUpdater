#include "qupd_updaterdialogfull.h"
#include "ui_qupd_updaterdialogfull.h"

#include <QApplication>

#include "qupd_updater.h"
#include "qupd_availableupdate.h"

QUPDUpdaterDialogFull::QUPDUpdaterDialogFull(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QUPDUpdaterDialogFull)
{
    ui->setupUi(this);

    // TODO
    setWindowFlags(windowFlags() | Qt::Popup);
    setAttribute(Qt::WA_DeleteOnClose, true);

    QString newVersString = ui->newVersionIsAvailableLabel->text().arg(qApp->applicationName());
    ui->newVersionIsAvailableLabel->setText(newVersString);


    QUPDAvailableUpdate *proposedUpdate = QUPDUpdater::instance()->proposedUpdate();
    if (proposedUpdate) {
        QString downloadString = ui->wouldYouLikeToDownloadLabel->text().arg(
                    qApp->applicationName(),
                    proposedUpdate->enclosureVersion(),
                    qApp->applicationVersion());
        ui->wouldYouLikeToDownloadLabel->setText(downloadString);
    }

    // Connect buttons
    connect(ui->installUpdateButton, SIGNAL(clicked()),
            QUPDUpdater::instance(), SLOT(installUpdate()));
    connect(ui->skipThisVersionButton, SIGNAL(clicked()),
            QUPDUpdater::instance(),   SLOT(skipUpdate()));
    connect(ui->remindMeLaterButton, SIGNAL(clicked()),
            QUPDUpdater::instance(), SLOT(remindMeLater()));
}

QUPDUpdaterDialogFull::~QUPDUpdaterDialogFull()
{
    delete ui;
}

void QUPDUpdaterDialogFull::changelogDownloaded()
{
    ui->releaseNotes->setHtml(QUPDUpdater::instance()->changelog());
}
