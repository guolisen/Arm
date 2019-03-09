#ifndef ARMWINDOW_H
#define ARMWINDOW_H

#include <QMainWindow>

namespace Ui {
class ArmWindow;
}

class QProcess;
class LogFileSystemModel;
class ArmWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ArmWindow(QWidget *parent = nullptr);
    ~ArmWindow();

    void init();

public slots:
    void notefinished(int exitCode);
private slots:
    void open();
    void findStringProcess(const QString &s);
    void resizeColumn(const QString &path);
    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    void createMenu();

    Ui::ArmWindow *ui;
    LogFileSystemModel* model_;
    QProcess* process_;
};

#endif // ARMWINDOW_H
