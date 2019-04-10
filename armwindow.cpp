#include <QFileDialog>
#include "armwindow.h"
#include "ui_armwindow.h"
#include "FileInfoModel/logfilesystemmodel.h"
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include "aboutdialog.h"
#include "FileInfoModel/uncompressfilecache.h"
#include "folderopendialog.h"
#include "settingdialog.h"
#include "FileInfoModel/filemodelmgr.h"
#include "FileInfoModel/remoteprocess.h"
#include "consoledialog.h"
#include "remotecommanddialog.h"

ArmWindow::ArmWindow(core::ContextPtr context, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmWindow), context_(context),
    modelMgr_(new fileinfomodel::FileModelMgr(context_, this)),
    consoleDialog_(new ConsoleDialog(this)),
    configMgrPtr_(context->getComponent<core::IConfigMgr>(nullptr))
{
    ui->setupUi(this);
    init();
}

ArmWindow::~ArmWindow()
{
    delete ui;
    //::system("del cache*");

    QProcess* proc = new QProcess();
    proc->start("del cache*");
    proc->close();
    delete proc;
}

void ArmWindow::resizeColumn(const QString &path)
{
    QAbstractItemModel* model = modelMgr_->getModel();
    for (int column = 0; column < model->columnCount(); ++column)
        ui->treeView->resizeColumnToContents(column);
}

void ArmWindow::handleSftpOperationFailed(const QString &errorMessage)
{
    statusBar()->showMessage(errorMessage);
}

void ArmWindow::handleSftpOperationFinished(const QString &error)
{
    statusBar()->showMessage(error);
}

void ArmWindow::handleConnectionError(const QString &errorMessage)
{
    QMessageBox::warning(this, tr("Connection Error"),
        tr("Fatal SSH error: %1").arg(errorMessage));
}

void ArmWindow::createRemoteProcess()
{
    core::ConfigMgrPtr config = context_->getComponent<core::IConfigMgr>(nullptr);
    QSsh::SshConnectionParameters sshParams = config->getSshParameters();

    remoteProcess_ = new RemoteProcess(sshParams);
    void readyRead(QByteArray data);
    void processStdout(QByteArray data);
    void processStderr(QByteArray data);
    connect(remoteProcess_, &RemoteProcess::readyRead, this, &ArmWindow::handleReadyRead);
    connect(remoteProcess_, &RemoteProcess::processStdout, this, &ArmWindow::handleStdOut);
    connect(remoteProcess_, &RemoteProcess::processStderr, this, &ArmWindow::handleStdOut);
}

void ArmWindow::init()
{
    setWindowTitle("Arm v0.1");
    ui->treeView->setAnimated(false);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(false);
    ui->treeView->setWindowTitle(QObject::tr("Arm"));

    connect(ui->comboBox, &QComboBox::editTextChanged, this, &ArmWindow::findStringProcess);
    connect(modelMgr_, &fileinfomodel::FileModelMgr::directoryLoadedWrapper, this, &ArmWindow::resizeColumn);

    connect(modelMgr_, SIGNAL(sftpOperationFailed(QString)),
        SLOT(handleSftpOperationFailed(QString)));
    connect(modelMgr_, SIGNAL(connectionError(QString)), SLOT(handleConnectionError(QString)));
    connect(modelMgr_, SIGNAL(sftpOperationFinished(QString)),
        SLOT(handleSftpOperationFinished(QString)));

    createMenu();
    createPopMenu();
    createRemoteProcess();

    editorPath_ = configMgrPtr_->getConfigInfo("Arm/Setting/editorPath").toString();
    if (editorPath_.isEmpty())
        editorPath_ = QDir::cleanPath("C:/Program Files (x86)/Notepad++/notepad++.exe");
}

void ArmWindow::handleStdOut(QByteArray data)
{
    consoleDialog_->setMessageToEditor(QString::fromStdString(data.toStdString()));
}

void ArmWindow::handleReadyRead(QByteArray data)
{
//consoleDialog_->setMessageToEditor(QString::fromStdString(data.toStdString()));
}

void ArmWindow::findStringProcess(const QString& s)
{
#if 0
    QAbstractItemModel* model = modelMgr_->getModel();
    LogFileSystemModel* localFsModel = dynamic_cast<LogFileSystemModel*>(model);
    if (!localFsModel)
        return;
    localFsModel->setFilter(QDir::Files | QDir::Dirs);
    localFsModel->setNameFilters({s});
#endif
}

void ArmWindow::open()
{
    FolderOpenDialog folderDialog(this);
    if (folderDialog.exec() != QDialog::Accepted)
        return;

    QString fileName;
    fileName = folderDialog.getFileName();
    if (fileName.isEmpty())
        return;
    qDebug() << fileName;

    modelMgr_->setRootPath(QDir::cleanPath(fileName), ui->treeView);
    setWindowTitle("Arm v0.1 " + fileName);
}

void ArmWindow::setting()
{
    SettingDialog settingDialog(context_, this);
    if (settingDialog.exec() == QDialog::Accepted)
    {
        editorPath_ = configMgrPtr_->getConfigInfo("Arm/Setting/editorPath").toString();
        if (editorPath_.isEmpty())
            editorPath_ = QDir::cleanPath("C:/Program Files (x86)/Notepad++/notepad++.exe");
    }
}
void ArmWindow::createPopMenu()
{
    rightPopMenu_ = new QMenu(this);
    rightPopMenu_->addAction(tr("&Uncompress on Site"), this, SLOT(uncompressInRemote()));
}

void ArmWindow::uncompressInRemote()
{
    qDebug() << "uncompressInRemote: ";
    QModelIndex index = ui->treeView->currentIndex();
    QSsh::SftpFileNode* fn = static_cast<QSsh::SftpFileNode *>(index.internalPointer());
    QString rootPath = modelMgr_->getRootPath();
    RemoteCommandDialog remoteDialog(this);
    if (fn->fileInfo.name.contains(".gz"))
    {
        remoteDialog.setCommand("gunzip " + rootPath + fn->path);
    }
    else if (fn->fileInfo.name.contains(".zip"))
    {
        remoteDialog.setCommand("unzip -o -d ./ " + rootPath + fn->path);
    }
    else if (fn->fileInfo.name.contains(".tar"))
    {
        remoteDialog.setCommand("tar xvf " + rootPath + fn->path);
    }
    else if (fn->fileInfo.name.contains(".tgz"))
    {
        remoteDialog.setCommand("tar xvzf " + rootPath + fn->path);
    }

    if (remoteDialog.exec() != QDialog::Accepted)
    {
        return;
    }

    QString command = remoteDialog.getCommand();
    consoleDialog_->show();
    remoteProcess_->run(command);
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
    if ( qApp->mouseButtons() == Qt::RightButton)
        return;

    QString cacheFile = modelMgr_->createCacheFile(index);

    QProcess* proc = new QProcess(this);
    connect(proc, static_cast<void(QProcess::*)(int)>(&QProcess::finished),
       [proc, cacheFile, this](int){
       // QString command = "del " + QDir::cleanPath(cacheFileName);
       //::system(command.toStdString().c_str());
       // qWarning()<< command << " Clean cache"<< proc.use_count() << " " << test.use_count();
       proc->close();
    });

    qDebug() << "cacheFileName: " << cacheFile;
    if (editorPath_.isEmpty())
    {
        QMessageBox::information(this, tr("Warning"), tr("Cannot Find Editor Path"));
        return;
    }
    proc->start(editorPath_, {cacheFile});
}


void ArmWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    if ( qApp->mouseButtons() == Qt::LeftButton)
        return;
    qDebug() << "on_treeView_customContextMenuRequested: ";
    QModelIndex index = ui->treeView->currentIndex();
    QSsh::SftpFileNode* fn = static_cast<QSsh::SftpFileNode *>(index.internalPointer());

    if (fn->fileInfo.name.contains(".gz") || fn->fileInfo.name.contains(".zip") ||
            fn->fileInfo.name.contains(".tar") ||
            fn->fileInfo.name.contains(".tgz"))
    {
        rightPopMenu_->popup(QCursor::pos());
    }
}
