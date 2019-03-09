#include <QFileDialog>
#include "armwindow.h"
#include "ui_armwindow.h"
#include "logfilesystemmodel.h"
#include <QDebug>
#include "aboutdialog.h"

ArmWindow::ArmWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmWindow)
{
    ui->setupUi(this);
}

ArmWindow::~ArmWindow()
{
    delete ui;
}

void ArmWindow::init()
{
    model_ = new LogFileSystemModel(this);
    model_->init();
    ui->treeView->setModel(model_);
    ui->treeView->setAnimated(true);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(false);
    //const QSize availableSize = QApplication::desktop()->availableGeometry(ui->treeView).size();
   // ui->treeView->resize(availableSize / 2);
   // ui->treeView->setColumnWidth(0, ui->treeView->width() / 3);

    ui->treeView->setWindowTitle(QObject::tr("Arm"));

    createMenu();
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
