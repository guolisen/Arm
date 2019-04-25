#include <QFileDialog>
#include <QHeaderView>
#include "armwindow.h"
#include "ui_armwindow.h"
#include "FileInfoModel/logfilesystemmodel.h"
#include <QDebug>
#include <QProcess>
#include <QSettings>
#include <QMessageBox>
#include <QToolBar>
#include "aboutdialog.h"
#include "FileInfoModel/uncompressfilecache.h"
#include "folderopendialog.h"
#include "settingdialog.h"
#include "FileInfoModel/filemodelmgr.h"
#include "FileInfoModel/remoteprocess.h"
#include "consoledialog.h"
#include "remotecommanddialog.h"

ArmWindow::ArmWindow(core::ContextPtr context, IRecentUseMgr::Factory recentUseFactory, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ArmWindow), context_(context), recentUseFactory_(recentUseFactory),
    modelMgr_(new fileinfomodel::FileModelMgr(context_, this)),
    remoteProcess_(nullptr),
    consoleDialog_(new ConsoleDialog(this)),
    configMgrPtr_(context->getComponent<core::IConfigMgr>(nullptr)),
    isNeedUpdate_(false)
{
    ui->setupUi(this);
    init();
}

ArmWindow::~ArmWindow()
{
    delete ui;
    //::system("del cache*");

    QProcess* proc = new QProcess();
    proc->start("cmd", {"/c", "del", "cache*"});
    proc->waitForFinished(1000);
    proc->close();
    delete proc;
}

void ArmWindow::resizeColumn(const QString &path)
{
    if (modelMgr_->getCurrentModeType() == fileinfomodel::RemoteFileSystemModel)
        return;
    qDebug() << "resizeColumn";
    QAbstractItemModel* model = modelMgr_->getModel();
    for (int column = 0; column < model->columnCount(); ++column)
        ui->treeView->resizeColumnToContents(column);
}

void ArmWindow::updateColumn(const QModelIndex& index)
{
    qDebug() << "updateColumn" << index.column();
    ui->treeView->resizeColumnToContents(index.column());
}

void ArmWindow::handleSftpOperationFailed(const QString &errorMessage)
{
    statusBar()->showMessage(errorMessage);
}

void ArmWindow::handleSftpOperationFinished(const QString &error)
{
    ui->treeView->resizeColumnToContents(0);
    //ui->treeView->sortByColumn(0);
    statusBar()->showMessage(error);
    if (error.isEmpty())
        statusBar()->showMessage("Process OK!");
}

void ArmWindow::handleConnectionError(const QString &errorMessage)
{
    QMessageBox::warning(this, tr("Connection Error"),
        tr("Fatal SSH error: %1").arg(errorMessage));
}

RemoteProcess* ArmWindow::createRemoteProcess()
{
    if (remoteProcess_)
    {
        delete remoteProcess_;
        remoteProcess_ = nullptr;
    }
    core::ConfigMgrPtr config = context_->getComponent<core::IConfigMgr>(nullptr);
    QSsh::SshConnectionParameters sshParams = config->getSshParameters();

    RemoteProcess* remoteProcess = new RemoteProcess(sshParams);
    connect(remoteProcess, &RemoteProcess::readyRead, this, &ArmWindow::handleReadyRead);
    connect(remoteProcess, &RemoteProcess::processStdout, this, &ArmWindow::handleStdOut);
    connect(remoteProcess, &RemoteProcess::processStderr, this, &ArmWindow::handleStdOut);
    connect(remoteProcess, &RemoteProcess::processClosed, this, &ArmWindow::handleClosed);

    remoteProcess_ = remoteProcess;
    return remoteProcess;
}

void ArmWindow::init()
{
    QString version = "Arm v" + configMgrPtr_->getCurrentVersion();
    setWindowTitle(version);
    ui->treeView->setAnimated(false);
    ui->treeView->setIndentation(20);
    ui->treeView->setSortingEnabled(true);
    ui->treeView->setWindowTitle(QObject::tr("Arm"));

    //connect(ui->treeView->header(), &QHeaderView::sectionClicked, ui->treeView, &QTreeView::sortByColumn);
    connect(ui->comboBox, &QComboBox::editTextChanged, this, &ArmWindow::findStringProcess);
    connect(modelMgr_, &fileinfomodel::FileModelMgr::directoryLoadedWrapper, this, &ArmWindow::resizeColumn);
    connect(modelMgr_, &fileinfomodel::FileModelMgr::directoryUpdateWrapper, this, &ArmWindow::updateColumn);

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

void ArmWindow::handleClosed(int exitStatus)
{
    qDebug() << " ArmWindow::handleClosed Command result: " << exitStatus;
    if (isNeedUpdate_)
    {
        QModelIndex currentInd = ui->treeView->currentIndex();
        if (currentInd.isValid())
            modelMgr_->update(currentInd);
        isNeedUpdate_ = false;
    }
    consoleDialog_->appendMessageToEditor("Command Done!");
}

void ArmWindow::handleStdOut(QByteArray data)
{
    QString result = QString::fromStdString(data.toStdString());
    //qDebug() << "Command Output: " << result;
    consoleDialog_->appendMessageToEditor(result);
}

void ArmWindow::handleReadyRead(QByteArray data)
{
    QString result = QString::fromStdString(data.toStdString());
    qDebug() << "handleReadyRead result: " << result;
    //consoleDialog_->setMessageToEditor(QString::fromStdString(data.toStdString()));
}

void ArmWindow::findStringProcess(const QString& s)
{
    if (modelMgr_->getCurrentModeType() == fileinfomodel::RemoteFileSystemModel)
        return;
    QAbstractItemModel* model = modelMgr_->getModel();
    fileinfomodel::LocalFileModelType* localFsModel =
            dynamic_cast<fileinfomodel::LocalFileModelType*>(model);
    if (!localFsModel)
        return;
    if(s.isEmpty())
    {
        localFsModel->setNameFilterDisables(true);
    }
    else
    {
        localFsModel->setNameFilterDisables(false);
    }
    localFsModel->setFilter(QDir::Files | QDir::Dirs);
    localFsModel->setNameFilters({s});
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

    QString version = "Arm v" + configMgrPtr_->getCurrentVersion();
    setWindowTitle(version + " " + fileName);

    if (modelMgr_->getCurrentModeType() == fileinfomodel::RemoteFileSystemModel)
    {
        statusBar()->showMessage("Connecting server...");
    }
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
    rightPopMenu_->addAction(tr("&Uncompress on Site"), this, SLOT(unCompressRemoteFile()));
}

void ArmWindow::unCompressRemoteFile()
{
    qDebug() << "uncompressInRemote";
    QModelIndex index = ui->treeView->currentIndex();
    QString uncompressCommand = "";
    QSsh::SftpFileNode* fn = static_cast<QSsh::SftpFileNode *>(index.internalPointer());
    if (fn)
    {
        QString currentPath = fn->path.mid(0, fn->path.lastIndexOf("/"));
        if (fn->fileInfo.name.contains(".gz"))
        {
            uncompressCommand = "gunzip " + fn->path;
        }
        else if (fn->fileInfo.name.contains(".zip"))
        {
            uncompressCommand = "unzip -o " + fn->path + " -d " + currentPath;
        }
        else if (fn->fileInfo.name.contains(".tar"))
        {
            uncompressCommand = "tar xvf " + fn->path + " -C " + currentPath;
        }
        else if (fn->fileInfo.name.contains(".tgz"))
        {
            uncompressCommand = "tar xvzf " + fn->path + " -C " + currentPath;
        }
    }

    if (runRemoteCommand(uncompressCommand))
        isNeedUpdate_ = true;
}

bool ArmWindow::runRemoteCommand(const QString& defaultCommand)
{
    //qDebug() << "runRemoteCommand default: " << defaultCommand;

    RemoteCommandDialog remoteDialog(this);
    remoteDialog.setCommand(defaultCommand);
    if (remoteDialog.exec() != QDialog::Accepted)
    {
        return false;
    }

    QString commandfinal = remoteDialog.getCommand();
    consoleDialog_->setMessageToEditor("");
    consoleDialog_->show();
    RemoteProcess* rprocess = createRemoteProcess();
    rprocess->run(commandfinal);
    return true;
}

void ArmWindow::createMenu()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon openIcon = QIcon::fromTheme("open", QIcon(":/folder.ico"));
    QAction *newAct = new QAction(openIcon, tr("&Open Log Folder"), this);
    newAct->setShortcuts(QKeySequence::Open);
    newAct->setStatusTip(tr("Open a new folder"));
    connect(newAct, &QAction::triggered, this, &ArmWindow::open);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);
    //-----
    const QIcon settingIcon = QIcon::fromTheme("setting", QIcon(":/setting.ico"));
    QAction *settingAct = new QAction(settingIcon, tr("&Preferences..."), this);
    settingAct->setShortcuts(QKeySequence::Preferences);
    settingAct->setStatusTip(tr("Setting..."));
    connect(settingAct, &QAction::triggered, this, &ArmWindow::setting);
    fileMenu->addAction(settingAct);
    fileToolBar->addAction(settingAct);
    fileMenu->addSeparator();
    //-----
    const QIcon exitIcon = QIcon::fromTheme("exit", QIcon(":/exit.ico"));
    QAction *quitAct = new QAction(exitIcon, tr("&Quit"), this);
    quitAct->setShortcuts(QKeySequence::Quit);
    quitAct->setStatusTip(tr("Quit"));
    connect(quitAct, &QAction::triggered, this, [this](){close();});
    fileMenu->addAction(quitAct);
    fileToolBar->addAction(quitAct);
    //-------------------------------------------
    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QToolBar *toolsToolBar = addToolBar(tr("Tools"));
    const QIcon runCommandIcon = QIcon::fromTheme("runCommandIcon", QIcon(":/play2.ico"));
    QAction *runCommandAct = new QAction(runCommandIcon, tr("&Run Command"), this);
    runCommandAct->setStatusTip(tr("Run Command"));
    connect(runCommandAct, &QAction::triggered, this, [this](){
        //modelMgr_->update();
        runRemoteCommand("");
    });
    toolsMenu->addAction(runCommandAct);
    toolsToolBar->addAction(runCommandAct);

    const QIcon reloadLogIcon = QIcon::fromTheme("reloadLogIcon", QIcon(":/reload.ico"));
    QAction *reloadLogAct = new QAction(reloadLogIcon, tr("&Reload Log Time"), this);
    reloadLogAct->setStatusTip(tr("Reload Log time"));
    connect(reloadLogAct, &QAction::triggered, this, [this](){
        modelMgr_->clearCache();
    });
    toolsMenu->addAction(reloadLogAct);
    toolsToolBar->addAction(reloadLogAct);

    const QIcon reloadModelIcon = QIcon::fromTheme("reloadModelIcon", QIcon(":/reloadMd.ico"));
    QAction *reloadModelAct = new QAction(reloadModelIcon, tr("&Reload Model"), this);
    reloadModelAct->setStatusTip(tr("Reload Model"));
    connect(reloadModelAct, &QAction::triggered, this, [this](){
        QModelIndex currentInd = ui->treeView->currentIndex();
        if (currentInd.isValid())
            modelMgr_->update(currentInd);
    });
    toolsMenu->addAction(reloadModelAct);
    //toolsToolBar->addAction(reloadModelAct);
    //-------------------------------------------
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QToolBar *helpToolBar = addToolBar(tr("Help"));
    const QIcon infoIcon = QIcon::fromTheme("info", QIcon(":/info.ico"));
    QAction *aboutAct = new QAction(infoIcon, tr("&About"), this);
    aboutAct->setStatusTip(tr("Open About"));
    connect(aboutAct, &QAction::triggered, this, [this](){
        AboutDialog dialog(this->context_, this);
        if (dialog.exec() != QDialog::Accepted)
            return;
    });
    helpMenu->addAction(aboutAct);
    helpToolBar->addAction(aboutAct);

    const QIcon howtologinIcon = QIcon::fromTheme("howtologin", QIcon(":/help.ico"));
    QAction *howtologinAct = new QAction(howtologinIcon, tr("How To Login"), this);
    howtologinAct->setStatusTip(tr("How to login"));
    connect(howtologinAct, &QAction::triggered, this, [this](){
        QProcess* proc = new QProcess(this);
        proc->start("cmd", {"/c", "ArmHelp.chm"});
        proc->waitForStarted(1000);
        proc->waitForFinished(1000);
        proc->close();
        delete proc;
    });
    helpMenu->addAction(howtologinAct);
    //helpToolBar->addAction(howtologinAct);
}

void ArmWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    if ( qApp->mouseButtons() == Qt::RightButton)
        return;

    QString cacheFile = modelMgr_->createCacheFile(index);
    if (cacheFile.isEmpty())
        return;

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
    QFileInfo editorFi(editorPath_);
    if (!editorFi.isFile())
    {
        QMessageBox::information(this, tr("Warning"), tr("The Editor doesn't exist, Please set again in Setting Panel."));
        return;
    }
    proc->start(editorPath_, {cacheFile});
}


void ArmWindow::on_treeView_customContextMenuRequested(const QPoint &pos)
{
    qDebug() << "on_treeView_customContextMenuRequested: ";
    if ( qApp->mouseButtons() == Qt::LeftButton)
        return;
    if (modelMgr_->getCurrentModeType() != fileinfomodel::RemoteFileSystemModel)
        return;

    QModelIndex index = ui->treeView->indexAt(pos);
    QSsh::SftpFileNode* fn = static_cast<QSsh::SftpFileNode *>(index.internalPointer());

    if (fn && (fn->fileInfo.name.contains(".gz") || fn->fileInfo.name.contains(".zip") ||
            fn->fileInfo.name.contains(".tar") ||
            fn->fileInfo.name.contains(".tgz")))
    {
        rightPopMenu_->popup(QCursor::pos());
    }
}
