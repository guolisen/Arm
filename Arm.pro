#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T21:29:29
#
#-------------------------------------------------

QT       += core gui

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
    logfilesystemmodel.cpp \
    armwindow.cpp

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
    logfilesystemmodel.h \
    armwindow.h

FORMS += \
    armwindow.ui

INCLUDEPATH += E:/code/lua-5.3.5/src

LIBS += E:/code/lua-5.3.5/luaLib/x64/Debug/luaLib.lib

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
