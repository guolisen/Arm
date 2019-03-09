#include "armwindow.h"
#include "ui_armwindow.h"
#include "logfilesystemmodel.h"
#include <QDebug>


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
    const QModelIndex rootIndex = model_->index(QDir::cleanPath("C:\\test"));
    ui->treeView->setRootIndex(rootIndex);


    //if (parser.isSet(dontUseCustomDirectoryIconsOption))
    //    model.iconProvider()->setOptions(QFileIconProvider::DontUseCustomDirectoryIcons);


    //if (!rootPath.isEmpty()) {
    //    const QModelIndex rootIndex = model.index(QDir::cleanPath(rootPath));
    //    if (rootIndex.isValid())
    //        ui->treeView->.setRootIndex(rootIndex);
    //}

    ui->treeView->setAnimated(true);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(false);
    //const QSize availableSize = QApplication::desktop()->availableGeometry(ui->treeView).size();
   // ui->treeView->resize(availableSize / 2);
   // ui->treeView->setColumnWidth(0, ui->treeView->width() / 3);

    ui->treeView->setWindowTitle(QObject::tr("Arm"));
}
