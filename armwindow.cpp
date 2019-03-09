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
    ui->treeView->setAnimated(true);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(false);
    ui->treeView->setWindowTitle(QObject::tr("Arm"));

    connect(ui->comboBox, &QComboBox::editTextChanged, this, &ArmWindow::findStringProcess);
    connect(model_, &LogFileSystemModel::directoryLoaded, this, &ArmWindow::resizeColumn);



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


void ArmWindow::editFinish(int, std::shared_ptr<TestClass> test)
{
    //qWarning()<<"Clean cache"<< proc.use_count() << " " << test.use_count();
    //::system("del cache*");
    //proc->close();
    test.reset();
}
void ArmWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    LogFileSystemModel* m=(LogFileSystemModel*)index.model();
    QString activeFileName = m->filePath(index);
    UncompressFileCache fileCache;
    QString cacheFileName = fileCache.createUncompressCacheFile(activeFileName);
    if(cacheFileName.isEmpty())
        return;

    //TO DO: check memory leak
    std::shared_ptr<QProcess> proc = std::make_shared<QProcess>();
    std::shared_ptr<TestClass> test = std::make_shared<TestClass>();

    //connect(proc.get(),static_cast<void(QProcess::*)(int)>(&QProcess::finished),
    //        std::bind(&ArmWindow::editFinish, this, std::placeholders::_1, test));

#if 1
    connect(proc.get(),static_cast<void(QProcess::*)(int)>(&QProcess::finished),
          [proc, test, cacheFileName, this](int){
         // QString command = "del " + QDir::cleanPath(cacheFileName);
          //::system(command.toStdString().c_str());
         // qWarning()<< command << " Clean cache"<< proc.use_count() << " " << test.use_count();
          //proc->close();
          //proc->disconnect();
    });
#endif
    qDebug() << "cacheFileName: " << cacheFileName;
    proc->start("C:/Program Files (x86)/Notepad++/notepad++.exe", {cacheFileName});
    //proc->start("C:/Program Files/TortoiseGit/bin/notepad2.exe", {cacheFileName});


}
