#include "armwindow.h"
#include <QApplication>
#include <QDir>
#include <QTime>
#include <QTextCodec>
#include <FileIdentifier/ifileObject.h>
#include <FileIdentifier/fileidentifier.h>
#include <FileIdentifier/filetypecontainer.h>
#include <FileIdentifier/luafileobjectwrapper.h>
#include <Core/appcontext.h>
#include <Core/ThreadPool.h>
#include <Core/detail/Thread.h>
#include <Core/configmgr.h>
#include <Script/IScriptCenter.h>
#include <Script/ScriptCenter.h>
#include <Script/detail/ScriptCenterImpl.h>
#include <recentusemgr.h>
//#pragma execution_character_set("utf-8")

QString getLogFileName(const QString& logName)
{
    QFileInfo fi(logName);

    if (!fi.isFile())
        return logName;

    int num = 0;
    int startPos = logName.lastIndexOf("_");
    if (startPos > 0)
    {
        int endPos = logName.lastIndexOf(".");
        QString numStr = logName.mid(startPos+1, endPos - startPos -1);
        num += numStr.toInt() + 1;
    }

    return getLogFileName(QString("ArmLog_%1.log").arg(num));
}

QMutex messageMutex;
QString currentLogFileName = "ArmLog.log";
void MessageOutput(QtMsgType type,const QMessageLogContext& context,const QString& msg)
{
    QMutexLocker locker(&messageMutex);
    QString txtMessage;

    txtMessage += QString("[%1][%2][%3][%4]")
            .arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
            .arg((int)QThread::currentThreadId())
            .arg(context.file)
            .arg(context.function);
    switch (type) {
    case QtDebugMsg:
        txtMessage += QString("[Debug] %1").arg(msg);
        break;
    case QtWarningMsg:
        txtMessage += QString("[Warning] %1").arg(msg);
        break;
    case QtCriticalMsg:
        txtMessage += QString("[Critical] %1").arg(msg);
        break;
    case QtFatalMsg:
        txtMessage += QString("[Fatal] %1").arg(msg);
        break;
    default:
        txtMessage += QString("[UnKnown] %1").arg(msg);
        break;
    }
    txtMessage += QString(",{%1}").arg(context.line);
    txtMessage += QString("\r\n");

    //QFileInfo fi(currentLogFileName);
    //if (fi.size() > (5 * 1024 * 1024))
    //    currentLogFileName = getLogFileName(currentLogFileName);

    QFile file("ArmLog.log");
    if(file.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream out(&file);
        out<<txtMessage;
    }
    file.flush();
    file.close();
}


int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("DELLEMC");
    QCoreApplication::setOrganizationDomain("FILETEAM");
    QCoreApplication::setApplicationName("ARM");

    core::ContextPtr context = std::make_shared<core::AppContext>();
    // ConfigMgr
    core::ConfigMgrPtr configMgrPtr = std::make_shared<core::ConfigMgr>();
    context->addComponent(configMgrPtr);

    if(configMgrPtr->getConfigInfo("Arm/Setting/logEnable", false).toBool())
    {
        qInstallMessageHandler(MessageOutput);
    }

    // ThreadPool
    int threadNumber = configMgrPtr->getConfigInfo("Arm/Setting/threadNumber", 5).toInt();
    core::ThreadPoolPtr pool = std::make_shared<core::ThreadPool>(
                core::detail::Thread::getFactory());
    pool->createThreads(threadNumber);
    context->addComponent(pool);

    // ScriptCenter
    script::ScriptCenterPtr scriptCenter =
        std::make_shared<script::ScriptCenter>(
                std::make_shared<script::detail::ScriptCenterImpl>());
    scriptCenter->initialize();
    context->addComponent(scriptCenter);

    // FileTypeContainer
    fileIdentifier::FileTypeContainerPtr container =
            std::make_shared<fileIdentifier::FileTypeContainer>(
                fileIdentifier::LuaFileObjectWrapper::getFactory(), scriptCenter);

    QDir currentFolder("FileTypeScript");
    currentFolder.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks | QDir::AllDirs);
    const QFileInfoList list = currentFolder.entryInfoList();
    Q_FOREACH(const QFileInfo fileInfo, list)
    {
        qDebug() << "current : " << fileInfo.fileName();
        QString fileName = fileInfo.filePath();
        if (fileName.contains(".lua"))
            container->registerLuaFileType(fileName.toStdString());
    }

    fileIdentifier::FileIdentifierPtr fileIdentifier = std::make_shared<
            fileIdentifier::FileIdentifier>(container);
    context->addComponent(fileIdentifier);

    ArmWindow w(context, RecentUseMgr::getFactory());
    w.show();

    return a.exec();
}
