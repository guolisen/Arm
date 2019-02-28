#include "armdialog.h"
#include "ui_armdialog.h"

ArmDialog::ArmDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArmDialog)
{
    ui->setupUi(this);
}

ArmDialog::~ArmDialog()
{
    delete ui;
}
