#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include <Core/iconfigmgr.h>

AboutDialog::AboutDialog(core::ContextPtr context, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog),
    context_(context)
{
    ui->setupUi(this);
    setWindowTitle("About");

    Qt::WindowFlags flag = windowFlags();
    flag = flag & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flag);
    core::ConfigMgrPtr configMgrPtr_ = context_->getComponent<core::IConfigMgr>(nullptr);
    QString version = "Version: v" + configMgrPtr_->getCurrentVersion();
    ui->versionLabel->setText(version);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
