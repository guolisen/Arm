#include <QFileDialog>
#include "armwindow.h"
#include "ui_armwindow.h"
#include "logfilesystemmodel.h"
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include "aboutdialog.h"
#include "uncompressfilecache.h"
#include "folderopendialog.h"
#include "settingdialog.h"

ArmWindow::ArmWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmWindow)
{
    ui->setupUi(this);
    setting_ = new QSettings;
}

ArmWindow::~ArmWindow()
{
    delete ui;
    ::system("del cache*");
}

void ArmWindow::resizeColumn(const QString &path)
{
    for (int column = 0; column < model_->columnCount(); ++column)
        ui->treeView->resizeColumnToContents(column);
}

void ArmWindow::init()
{
    setWindowTitle("Arm v0.1");
    model_ = new LogFileSystemModel(this);
    model_->init();

    ui->treeView->setModel(model_);
    ui->treeView->setAnimated(false);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(false);
    ui->treeView->setWindowTitle(QObject::tr("Arm"));

    connect(ui->comboBox, &QComboBox::editTextChanged, this, &ArmWindow::findStringProcess);
    connect(model_, &LogFileSystemModel::directoryLoaded, this, &ArmWindow::resizeColumn);

    createMenu();

    editorPath_ = setting_->value("Arm/Setting/editorPath").toString();
    if (editorPath_.isEmpty())
        editorPath_ = QDir::cleanPath("C:/Program Files (x86)/Notepad++/notepad++.exe");
}

void ArmWindow::findStringProcess(const QString& s)
{
    model_->setFilter(QDir::Files | QDir::Dirs);
    model_->setNameFilters({s});
}

void ArmWindow::open()
{
    FolderOpenDialog folderDialog(this);
    if (folderDialog.exec() == QDialog::Accepted)
    {
        QString fileName;
        fileName = folderDialog.getFileName();
        if (fileName.isEmpty())
            return;
        model_->setRootPath(QDir::cleanPath(fileName));
        const QModelIndex rootIndex = model_->index(QDir::cleanPath(fileName));
        ui->treeView->setRootIndex(rootIndex);
        ui->treeView->update();
    }
}

void ArmWindow::setting()
{
    SettingDialog settingDialog(this);
    if (settingDialog.exec() == QDialog::Accepted)
    {
        editorPath_ = setting_->value("Arm/Setting/editorPath").toString();
        if (editorPath_.isEmpty())
            editorPath_ = QDir::cleanPath("C:/Program Files (x86)/Notepad++/notepad++.exe");
    }
}

void ArmWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    const QIcon openIcon = QIcon::fromTheme("open", QIcon(":/folder.ico"));
    QAction *newAct = new QAction(openIcon, tr("&Open Log Folder"), this);
    newAct->setShortcuts(QKeySequence::Open);
    newAct->setStatusTip(tr("Open a new folder"));
    connect(newAct, &QAction::triggered, this, &ArmWindow::open);
    fileMenu->addAction(newAct);
    //-----
    const QIcon settingIcon = QIcon::fromTheme("setting", QIcon(":/setting.ico"));
    QAction *settingAct = new QAction(settingIcon, tr("&Preferences..."), this);
    settingAct->setShortcuts(QKeySequence::Preferences);
    settingAct->setStatusTip(tr("Setting..."));
    connect(settingAct, &QAction::triggered, this, &ArmWindow::setting);
    fileMenu->addAction(settingAct);
    fileMenu->addSeparator();
    //-----
    const QIcon exitIcon = QIcon::fromTheme("exit", QIcon(":/exit.ico"));
    QAction *quitAct = new QAction(exitIcon, tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit"));
    connect(quitAct, &QAction::triggered, this, [this](){close();});
    fileMenu->addAction(quitAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon infoIcon = QIcon::fromTheme("info", QIcon(":/info.ico"));
    QAction *aboutAct = new QAction(infoIcon, tr("&About"), this);
    aboutAct->setStatusTip(tr("Open About"));
    connect(aboutAct, &QAction::triggered, this, [this](){
        AboutDialog dialog(this);
        if (dialog.exec() != QDialog::Accepted)
            return;
    });
    helpMenu->addAction(aboutAct);
}

void ArmWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    LogFileSystemModel* m = (LogFileSystemModel*)index.model();
    QString activeFileName = m->filePath(index);
    UncompressFileCache fileCache;
    QString cacheFileName = fileCache.createUncompressCacheFile(activeFileName);
    if(cacheFileName.isEmpty())
        return;

    QProcess* proc = new QProcess(this);
    connect(proc, static_cast<void(QProcess::*)(int)>(&QProcess::finished),
          [proc, cacheFileName, this](int){
          // QString command = "del " + QDir::cleanPath(cacheFileName);
          //::system(command.toStdString().c_str());
          // qWarning()<< command << " Clean cache"<< proc.use_count() << " " << test.use_count();
          proc->close();
    });

    qDebug() << "cacheFileName: " << cacheFileName;
    if (editorPath_.isEmpty())
    {
        QMessageBox::information(this, tr("Warning"), tr("Cannot Find Editor Path"));
        return;
    }
    proc->start(editorPath_, {cacheFileName});
}
