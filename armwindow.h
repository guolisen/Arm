#ifndef ARMWINDOW_H
#define ARMWINDOW_H

#include <QMainWindow>

namespace Ui {
class ArmWindow;
}

class LogFileSystemModel;
class ArmWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArmWindow(QWidget *parent = nullptr);
    ~ArmWindow();

    void init();

private slots:
    void open();
private:
    void createMenu();
    Ui::ArmWindow *ui;
    LogFileSystemModel* model_;

};

#endif // ARMWINDOW_H
