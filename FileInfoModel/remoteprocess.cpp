/**************************************************************************
**
** This file is part of Qt Creator
**
** Copyright (c) 2012 Nokia Corporation and/or its subsidiary(-ies).
**
** Contact: http://www.qt-project.org/
**
**
** GNU Lesser General Public License Usage
**
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this file.
** Please review the following information to ensure the GNU Lesser General
** Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**************************************************************************/

#include "remoteprocess.h"

#include <ssh/sshpseudoterminal.h>

#include <QCoreApplication>
#include <QTextStream>
#include <QTimer>

#include <iostream>

using namespace QSsh;

const QByteArray StderrOutput("ChannelTest");

RemoteProcess::RemoteProcess(const SshConnectionParameters &params)
    : m_sshParams(params),
      m_timeoutTimer(new QTimer(this)),
      m_sshConnection(0),
      m_remoteRunner(new SshRemoteProcessRunner(this)),
      m_state(Inactive)
{
    m_timeoutTimer->setInterval(5000);
    connect(m_timeoutTimer, SIGNAL(timeout()), SLOT(handleTimeout()));

    connect(m_remoteRunner, SIGNAL(connectionError()),
        SLOT(handleConnectionError()));
    connect(m_remoteRunner, SIGNAL(processStarted()),
        SLOT(handleProcessStarted()));
    connect(m_remoteRunner, SIGNAL(readyReadStandardOutput()), SLOT(handleProcessStdout()));
    connect(m_remoteRunner, SIGNAL(readyReadStandardError()), SLOT(handleProcessStderr()));
    connect(m_remoteRunner, SIGNAL(processClosed(int)),
        SLOT(handleProcessClosed(int)));
}

RemoteProcess::~RemoteProcess()
{
    delete m_sshConnection;
}

void RemoteProcess::run(const QString& command)
{
    qDebug() << "Starting remote process... " << QByteArray::fromStdString(command.toStdString());
    m_state = TestingSuccess;
    m_started = false;
    m_timeoutTimer->start();
    m_remoteRunner->run(QByteArray::fromStdString(command.toStdString()), m_sshParams);
}

void RemoteProcess::handleConnectionError()
{
    const QString error = m_state == TestingIoDevice || m_state == TestingProcessChannels
        ? m_sshConnection->errorString() : m_remoteRunner->lastConnectionErrorString();

     qDebug() << "Error: Connection failure (" << qPrintable(error) << ")."  ;
    return;
}

void RemoteProcess::handleProcessStarted()
{
    if (m_started) {
         qDebug() << "Error: Received started() signal again."  ;
        return;
    } else {
        m_started = true;
        if (m_state == TestingCrash) {
            QSsh::SshRemoteProcessRunner * const killer
                = new QSsh::SshRemoteProcessRunner(this);
            killer->run("pkill -9 sleep", m_sshParams);
        } else if (m_state == TestingIoDevice) {
            connect(m_catProcess.data(), SIGNAL(readyRead()), SLOT(handleReadyRead()));
            m_textStream = new QTextStream(m_catProcess.data());
            *m_textStream << testString();
            m_textStream->flush();
        }
    }
}

void RemoteProcess::handleProcessStdout()
{
    if (!m_started) {
         qDebug() << "Error: Remote output from non-started process."
             ;
        return;
    } else if (m_state != TestingSuccess && m_state != TestingTerminal) {
         qDebug() << "Error: Got remote standard output in state " << m_state
            << "."  ;
        return;
    } else {
        m_remoteStdout += m_remoteRunner->readAllStandardOutput();
        emit processStdout(m_remoteStdout);
    }
}
void RemoteProcess::clearStdout()
{
    m_remoteStdout.clear();
}

void RemoteProcess::handleProcessStderr()
{
    if (!m_started) {
         qDebug() << "Error: Remote error output from non-started process."
             ;
        return;
    } else if (m_state == TestingSuccess) {
         qDebug() << "Error: Unexpected remote standard error output."
             ;
        return;
    } else {
        m_remoteStderr += m_remoteRunner->readAllStandardError();
        emit processStderr(m_remoteStderr);
    }
}

void RemoteProcess::handleProcessClosed(int exitStatus)
{
    emit processClosed(exitStatus);
    switch (exitStatus) {
    case SshRemoteProcess::NormalExit:
        if (!m_started) {
             qDebug() << "Error: Process exited without starting."  ;
            return;
        }
        switch (m_state) {
        case TestingSuccess: {
            const int exitCode = m_remoteRunner->processExitCode();
            if (exitCode != 0) {
                 qDebug() << "Error: exit code is " << exitCode
                    << ", expected zero.";
                if (!m_remoteStdout.isEmpty()) {
                    emit processStdout(m_remoteStdout);
                }
                earlyDisconnectFromHost();
                return;
            }
            if (m_remoteStdout.isEmpty()) {
                 qDebug() << "Error: Command did not produce output.";
                earlyDisconnectFromHost();
                return;
            }

            qDebug() << "Ok. unsuccessful remote process... "  ;
            m_state = TestingFailure;
            m_started = false;
            //m_timeoutTimer->start();
            //m_remoteRunner->run("top -n 1", m_sshParams); // Does not succeed without terminal.
            break;
        }
        case TestingFailure: {
            const int exitCode = m_remoteRunner->processExitCode();
            if (exitCode == 0) {
                qDebug() << "Error: exit code is zero, expected non-zero.";
                if (!m_remoteStdout.isEmpty()) {
                    emit processStdout(m_remoteStdout);
                }
                earlyDisconnectFromHost();
                return;
            }
            if (m_remoteStderr.isEmpty()) {
                 qDebug() << "Error: Command did not produce error output."  ;
                earlyDisconnectFromHost();
                return;
            }

            qDebug() << "Ok.\nTesting crashing remote process... "  ;
            m_state = TestingCrash;
            m_started = false;
            m_timeoutTimer->start();
            m_remoteRunner->run("/bin/sleep 100", m_sshParams);
            break;
        }
        case TestingCrash:
            if (m_remoteRunner->processExitCode() == 0) {
                 qDebug() << "Error: Successful exit from process that was "
                    "supposed to crash."  ;
                 earlyDisconnectFromHost();
                 return;
            } else {
                // Some shells (e.g. mksh) don't report "killed", but just a non-zero exit code.
                 handleSuccessfulCrashTest();
            }
            break;
        case TestingTerminal: {
            const int exitCode = m_remoteRunner->processExitCode();
            if (exitCode != 0) {
                 qDebug() << "Error: exit code is " << exitCode
                    << ", expected zero.";
                 if (!m_remoteStdout.isEmpty()) {
                     emit processStdout(m_remoteStdout);
                 }
                earlyDisconnectFromHost();
                return;
            }
            if (m_remoteStdout.isEmpty()) {
                 qDebug() << "Error: Command did not produce output."
                     ;
                earlyDisconnectFromHost();
                return;
            }
            qDebug() << "Ok.\nTesting I/O device functionality... "  ;
            m_state = TestingIoDevice;
            m_sshConnection = new QSsh::SshConnection(m_sshParams);
            connect(m_sshConnection, SIGNAL(connected()), SLOT(handleConnected()));
            connect(m_sshConnection, SIGNAL(error(QSsh::SshError)),
                SLOT(handleConnectionError()));
            m_sshConnection->connectToHost();
            m_timeoutTimer->start();
            break;
        }
        case TestingIoDevice:
            if (m_catProcess->exitCode() == 0) {
                 qDebug() << "Error: Successful exit from process that was supposed to crash."
                     ;
                qApp->exit(EXIT_FAILURE);
            } else {
                handleSuccessfulIoTest();
            }
            break;
        case TestingProcessChannels:
            if (m_remoteStderr.isEmpty()) {
                 qDebug() << "Error: Did not receive readyReadStderr()."  ;
                qApp->exit(EXIT_FAILURE);
                return;
            }
            if (m_remoteData != StderrOutput) {
                 qDebug() << "Error: Expected output '" << StderrOutput.data() << "', received '"
                    << m_remoteData.data() << "'."  ;
                qApp->exit(EXIT_FAILURE);
                return;
            }
            qDebug() << "Ok.\nAll tests succeeded."  ;
            return;
            break;
        case Inactive:
            Q_ASSERT(false);
        }
        break;
    case SshRemoteProcess::FailedToStart:
        if (m_started) {
             qDebug() << "Error: Got 'failed to start' signal for process "
                "that has not started yet."  ;
        } else {
             qDebug() << "Error: Process failed to start."  ;
        }
        return;
        break;
    case SshRemoteProcess::CrashExit:
        switch (m_state) {
        case TestingCrash:
            handleSuccessfulCrashTest();
            break;
        case TestingIoDevice:
            handleSuccessfulIoTest();
            break;
        default:
             qDebug() << "Error: Unexpected crash."  ;

            return;
        }
    }
}
void RemoteProcess::earlyDisconnectFromHost()
{
    qDebug() << "earlyDisconnectFromHost ";
   // m_remoteRunner->cancel();
    if (m_catProcess)
    {
        m_catProcess->close();
    }
    if (m_echoProcess)
    {
        m_echoProcess->close();
    }
    if (m_sshConnection)
    {
        m_sshConnection->disconnectFromHost();
    }

    m_timeoutTimer->stop();
}

void RemoteProcess::handleTimeout()
{
     //qDebug() << "Error: Timeout waiting for progress."  ;
    return;
}

void RemoteProcess::handleConnected()
{
    Q_ASSERT(m_state == TestingIoDevice);

    m_catProcess = m_sshConnection->createRemoteProcess(QString::fromLocal8Bit("/bin/cat").toUtf8());
    connect(m_catProcess.data(), SIGNAL(started()), SLOT(handleProcessStarted()));
    connect(m_catProcess.data(), SIGNAL(closed(int)), SLOT(handleProcessClosed(int)));
    m_started = false;
    m_timeoutTimer->start();
    m_catProcess->start();
}

QString RemoteProcess::testString() const
{
    return QLatin1String("x\r\n");
}

void RemoteProcess::handleReadyRead()
{
    switch (m_state) {
    case TestingIoDevice: {
        const QString &data = QString::fromUtf8(m_catProcess->readAll());
        if (data != testString()) {
             qDebug() << "Testing of QIODevice functionality failed: Expected '"
                << qPrintable(testString()) << "', got '" << qPrintable(data) << "'."  ;
            return;
        }
        QSsh::SshRemoteProcessRunner * const killer = new QSsh::SshRemoteProcessRunner(this);
        killer->run("pkill -9 cat", m_sshParams);
        break;
    }
    case TestingProcessChannels:
        m_remoteData += m_echoProcess->readAll();
        emit readyRead(m_remoteData);
        break;
    default:
        qFatal("%s: Unexpected state %d.", Q_FUNC_INFO, m_state);
    }

}

void RemoteProcess::handleReadyReadStdout()
{
    Q_ASSERT(m_state == TestingProcessChannels);

     qDebug() << "Error: Received unexpected stdout data."  ;
    //qApp->exit(EXIT_FAILURE);
}

void RemoteProcess::handleReadyReadStderr()
{
    Q_ASSERT(m_state == TestingProcessChannels);

    m_remoteStderr = "dummy";
}

void RemoteProcess::handleSuccessfulCrashTest()
{
    qDebug() << "Ok.\nTesting remote process with terminal... "  ;
    m_state = TestingTerminal;
    m_started = false;
    m_timeoutTimer->start();
    m_remoteRunner->runInTerminal("top -n 1", SshPseudoTerminal(), m_sshParams);
}

void RemoteProcess::handleSuccessfulIoTest()
{
    qDebug() << "Ok\nTesting process channels... "  ;
    m_state = TestingProcessChannels;
    m_started = false;
    m_remoteStderr.clear();
    m_echoProcess = m_sshConnection->createRemoteProcess("printf " + StderrOutput + " >&2");
    m_echoProcess->setReadChannel(QProcess::StandardError);
    connect(m_echoProcess.data(), SIGNAL(started()), SLOT(handleProcessStarted()));
    connect(m_echoProcess.data(), SIGNAL(closed(int)), SLOT(handleProcessClosed(int)));
    connect(m_echoProcess.data(), SIGNAL(readyRead()), SLOT(handleReadyRead()));
    connect(m_echoProcess.data(), SIGNAL(readyReadStandardError()),
            SLOT(handleReadyReadStderr()));
    m_echoProcess->start();
    m_timeoutTimer->start();
}
