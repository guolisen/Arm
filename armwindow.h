#ifndef ARMWINDOW_H
#define ARMWINDOW_H

#include <QMainWindow>

namespace Ui {
class ArmWindow;
}

class QFileSystemModel;
class ArmWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArmWindow(QWidget *parent = nullptr);
    ~ArmWindow();

    void init();

private:
    Ui::ArmWindow *ui;
    QFileSystemModel* model_;
};

#endif // ARMWINDOW_H
