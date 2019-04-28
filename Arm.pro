#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T21:29:29
#
#-------------------------------------------------

QT       += core gui network
RC_ICONS += res/mhands.ico
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Arm
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    Core/detail/Thread.cpp \
    Core/appcontext.cpp \
    Core/ThreadPool.cpp \
    FileIdentifier/defaultfileobject.cpp \
    FileIdentifier/fileidentifier.cpp \
    FileIdentifier/filetypecontainer.cpp \
    FileIdentifier/luafileobjectwrapper.cpp \
    Script/detail/ScriptCenterImpl.cpp \
    Script/detail/ScriptUtil.cpp \
    Thirdparty/LuaWrapper/luaWrapper.cpp \
    armwindow.cpp \
    aboutdialog.cpp \
    folderopendialog.cpp \
    settingdialog.cpp \
    Thirdparty/Lua/src/lapi.c \
    Thirdparty/Lua/src/lauxlib.c \
    Thirdparty/Lua/src/lbaselib.c \
    Thirdparty/Lua/src/lbitlib.c \
    Thirdparty/Lua/src/lcode.c \
    Thirdparty/Lua/src/lcorolib.c \
    Thirdparty/Lua/src/lctype.c \
    Thirdparty/Lua/src/ldblib.c \
    Thirdparty/Lua/src/ldebug.c \
    Thirdparty/Lua/src/ldo.c \
    Thirdparty/Lua/src/ldump.c \
    Thirdparty/Lua/src/lfunc.c \
    Thirdparty/Lua/src/lgc.c \
    Thirdparty/Lua/src/linit.c \
    Thirdparty/Lua/src/liolib.c \
    Thirdparty/Lua/src/llex.c \
    Thirdparty/Lua/src/lmathlib.c \
    Thirdparty/Lua/src/lmem.c \
    Thirdparty/Lua/src/loadlib.c \
    Thirdparty/Lua/src/lobject.c \
    Thirdparty/Lua/src/lopcodes.c \
    Thirdparty/Lua/src/loslib.c \
    Thirdparty/Lua/src/lparser.c \
    Thirdparty/Lua/src/lstate.c \
    Thirdparty/Lua/src/lstring.c \
    Thirdparty/Lua/src/lstrlib.c \
    Thirdparty/Lua/src/ltable.c \
    Thirdparty/Lua/src/ltablib.c \
    Thirdparty/Lua/src/ltm.c \
    Thirdparty/Lua/src/lundump.c \
    Thirdparty/Lua/src/lutf8lib.c \
    Thirdparty/Lua/src/lvm.c \
    Thirdparty/Lua/src/lzio.c \
    FileInfoModel/uncompressfilecache.cpp \
    Core/configmgr.cpp \
    FileInfoModel/filemodelmgr.cpp \
    FileInfoModel/sftpreadtimejob.cpp \
    FileInfoModel/localreadtimejob.cpp \
    FileInfoModel/localfilemodel.cpp \
    FileInfoModel/sftpfilemodel.cpp \
    FileInfoModel/sftpmgr.cpp \
    FileInfoModel/remoteprocess.cpp \
    consoledialog.cpp \
    remotecommanddialog.cpp \
    recentusemgr.cpp \
    FileInfoModel/sortfilterproxymodel.cpp


HEADERS += \
    Core/detail/IThread.h \
    Core/detail/Thread.h \
    Core/appcontext.h \
    Core/IThreadPool.h \
    Core/ThreadPool.h \
    FileIdentifier/defaultfileobject.h \
    FileIdentifier/fileidentifier.h \
    FileIdentifier/filetypecontainer.h \
    FileIdentifier/ifileidentifier.h \
    FileIdentifier/ifileObject.h \
    FileIdentifier/ifiletypecontainer.h \
    FileIdentifier/luafileobjectwrapper.h \
    Script/detail/ScriptCenterImpl.h \
    Script/detail/ScriptUtil.h \
    Script/IScriptCenter.h \
    Script/ScriptCenter.h \
    Thirdparty/LuaWrapper/any.h \
    Thirdparty/LuaWrapper/luaClass.h \
    Thirdparty/LuaWrapper/luacommon.h \
    Thirdparty/LuaWrapper/luaFunction.h \
    Thirdparty/LuaWrapper/luaObject.h \
    Thirdparty/LuaWrapper/luaWrapper.h \
    Thirdparty/LuaWrapper/ObjPush.h \
    Thirdparty/LuaWrapper/PopValue.h \
    Thirdparty/LuaWrapper/Trait.h \
    Thirdparty/LuaWrapper/TypeList.h \
    Thirdparty/LuaWrapper/utility.h \
    armwindow.h \
    aboutdialog.h \
    folderopendialog.h \
    settingdialog.h \
    Thirdparty/Lua/src/lapi.h \
    Thirdparty/Lua/src/lauxlib.h \
    Thirdparty/Lua/src/lcode.h \
    Thirdparty/Lua/src/lctype.h \
    Thirdparty/Lua/src/ldebug.h \
    Thirdparty/Lua/src/ldo.h \
    Thirdparty/Lua/src/lfunc.h \
    Thirdparty/Lua/src/lgc.h \
    Thirdparty/Lua/src/llex.h \
    Thirdparty/Lua/src/llimits.h \
    Thirdparty/Lua/src/lmem.h \
    Thirdparty/Lua/src/lobject.h \
    Thirdparty/Lua/src/lopcodes.h \
    Thirdparty/Lua/src/lparser.h \
    Thirdparty/Lua/src/lprefix.h \
    Thirdparty/Lua/src/lstate.h \
    Thirdparty/Lua/src/lstring.h \
    Thirdparty/Lua/src/ltable.h \
    Thirdparty/Lua/src/ltm.h \
    Thirdparty/Lua/src/lua.hpp \
    Thirdparty/Lua/src/lua.h \
    Thirdparty/Lua/src/luaconf.h \
    Thirdparty/Lua/src/lualib.h \
    Thirdparty/Lua/src/lundump.h \
    Thirdparty/Lua/src/lvm.h \
    Thirdparty/Lua/src/lzio.h \
    FileInfoModel/logfilesystemmodel.h \
    FileInfoModel/uncompressfilecache.h \
    Core/configmgr.h \
    Core/iconfigmgr.h \
    FileInfoModel/filemodelmgr.h \
    FileInfoModel/sftpreadtimejob.h \
    FileInfoModel/localreadtimejob.h \
    FileInfoModel/ireadtimejob.h \
    FileInfoModel/ifilemodelmgr.h \
    FileInfoModel/ifilemodel.h \
    FileInfoModel/localfilemodel.h \
    FileInfoModel/sftpfilemodel.h \
    FileInfoModel/sftpmgr.h \
    FileInfoModel/remoteprocess.h \
    consoledialog.h \
    remotecommanddialog.h \
    irecentusemgr.h \
    recentusemgr.h \
    FileInfoModel/sortfilterproxymodel.h

FORMS += \
    armwindow.ui \
    aboutdialog.ui \
    folderopendialog.ui \
    settingdialog.ui \
    consoledialog.ui \
    remotecommanddialog.ui

INCLUDEPATH += Thirdparty/Lua/src \
               C:/Code/qt/Quazip/quazip/quazip \
               C:/"Program Files"/zlib/include \
               C:/Code/qt/MyQSsh/QSsh/src/libs

LIBS += C:/"Program Files"/zlib/lib/zlibstaticd.lib \
        C:/Code/qt/Quazip/build-quazip-Desktop_Qt_5_12_2_MSVC2017_64bit-Debug/quazip/debug/quazipd.lib \
        C:/Code/qt/MyQSsh/build-qssh-Desktop_Qt_5_12_2_MSVC2017_64bit-Debug/lib/QSshd.lib

#INCLUDEPATH += Thirdparty/Lua/src \
#               E:/code/qt/quazip/quazip/quazip \
#               E:/"Program Files"/zlib/include \
#               E:/code/qt/qssh/MyQssh/QSsh/src/libs

#LIBS += E:/"Program Files"/zlib/lib/zlibstaticd.lib \
#        E:/code/qt/quazip/build-quazip-Desktop_Qt_5_12_1_MSVC2017_64bit-Debug/quazip/debug/quazipd.lib \
#        E:/code/qt/qssh/MyQssh/build-qssh-Desktop_Qt_5_12_1_MSVC2017_64bit-Debug/lib/QSshd.lib


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/res.qrc

