#ifndef ARMWINDOW_H
#define ARMWINDOW_H

#include <QDebug>
#include <QMainWindow>

namespace Ui {
class ArmWindow;
}

class QProcess;
class QSettings;
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
    void findStringProcess(const QString &s);
    void resizeColumn(const QString &path);
    void on_treeView_doubleClicked(const QModelIndex &index);
    void setting();
private:
    void createMenu();

    Ui::ArmWindow *ui;
    LogFileSystemModel* model_;
    QString editorPath_;
    QSettings* setting_;
};

#endif // ARMWINDOW_H
