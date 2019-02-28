#ifndef ARMDIALOG_H
#define ARMDIALOG_H

#include <QDialog>

namespace Ui {
class ArmDialog;
}

class ArmDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArmDialog(QWidget *parent = nullptr);
    ~ArmDialog();

private:
    Ui::ArmDialog *ui;
};

#endif // ARMDIALOG_H
