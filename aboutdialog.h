#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include <Core/appcontext.h>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(core::ContextPtr context, QWidget *parent = nullptr);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
    core::ContextPtr context_;
};

#endif // ABOUTDIALOG_H
