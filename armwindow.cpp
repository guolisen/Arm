#include <QFileDialog>
#include "armwindow.h"
#include "ui_armwindow.h"
#include "logfilesystemmodel.h"
#include <QDebug>
#include <QProcess>
#include "aboutdialog.h"
#include "uncompressfilecache.h"

ArmWindow::ArmWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmWindow),
    process_(new QProcess(this))
{
    ui->setupUi(this);

}

ArmWindow::~ArmWindow()
{
    delete ui;
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
    ui->treeView->setAnimated(true);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(false);
    ui->treeView->setWindowTitle(QObject::tr("Arm"));

    connect(ui->comboBox, &QComboBox::editTextChanged, this, &ArmWindow::findStringProcess);
    connect(model_, &LogFileSystemModel::directoryLoaded, this, &ArmWindow::resizeColumn);
    //connect(process_, &QProcess::finished, this, &ArmWindow::notefinished);

    QObject::connect(process_,static_cast<void(QProcess::*)(int)>(&QProcess::finished),
          [](int){
          qWarning()<<"Clean cache";
          ::system("del cache*"); });

    createMenu();
}

void ArmWindow::findStringProcess(const QString& s)
{
    model_->setFilter(QDir::Files | QDir::Dirs);
    model_->setNameFilters({s});
}

void ArmWindow::open()
{
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Choose Source Directory");
    //fd->setDirectory(buf);
    fileDialog->setFileMode(QFileDialog::DirectoryOnly);
    QStringList fileName;
    fileDialog->setOption(QFileDialog::ShowDirsOnly);

    if (fileDialog->exec() == QDialog::Accepted)
    {
        fileName = fileDialog->selectedFiles();
        model_->setRootPath(QDir::cleanPath(fileName[0]));
        const QModelIndex rootIndex = model_->index(QDir::cleanPath(fileName[0]));
        ui->treeView->setRootIndex(rootIndex);
        ui->treeView->update();
    }
}

void ArmWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *newAct = new QAction(tr("&Open Log Folder"), this);
    newAct->setShortcuts(QKeySequence::Open);
    newAct->setStatusTip(tr("Open a new folder"));
    connect(newAct, &QAction::triggered, this, &ArmWindow::open);
    fileMenu->addAction(newAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Open About"));
    connect(aboutAct, &QAction::triggered, this, [this](){
        AboutDialog dialog(this);
        if (dialog.exec() != QDialog::Accepted)
            return;
    });
    helpMenu->addAction(aboutAct);
}
void ArmWindow::notefinished(int exitCode)
{

}
void ArmWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    ::system("del cache*");
    LogFileSystemModel* m=(LogFileSystemModel*)index.model();
    QString activeFileName = m->filePath(index);
    UncompressFileCache fileCache;
    QString cacheFileName = fileCache.createUncompressCacheFile(activeFileName);
    if(cacheFileName.isEmpty())
        return;

    qDebug() << "cacheFileName: " << cacheFileName;
    //process_->start("C:\\Program Files (x86)\\Notepad++\\notepad++.exe");
    process_->start("C:/Program Files/TortoiseGit/bin/notepad2.exe", {cacheFileName});
}
