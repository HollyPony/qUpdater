#include "qupd_downloadupdatedialog.h"
#include "ui_qupd_downloadupdatedialog.h"

#include <QPushButton>

// TODO Change to QProgressDialog

QUPDDownloadUpdateDialog::QUPDDownloadUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QUPDDownloadUpdateDialog)
{
    ui->setupUi(this);
    if (parent && parent->isVisible())
        setWindowFlags(windowFlags() | Qt::Sheet);
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(0);
}

QUPDDownloadUpdateDialog::~QUPDDownloadUpdateDialog()
{
    delete ui;
}

void QUPDDownloadUpdateDialog::updateProgressBar(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}

void QUPDDownloadUpdateDialog::downloadComplete()
{
    QPushButton *installAndRelaunchButton = new QPushButton(this);
    installAndRelaunchButton->setText(tr("Install and relaunch"));
    connect(installAndRelaunchButton, SIGNAL(clicked()),
            this, SIGNAL(installAndRelaunch()));
    ui->buttonBox->addButton(installAndRelaunchButton, QDialogButtonBox::AcceptRole);
}
