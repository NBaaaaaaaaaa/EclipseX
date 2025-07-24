#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "databaseManager.h"

#include <QObject>
#include <QTcpServer>
#include <QThread>

class TcpServer : public QObject
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    bool serverStatus = false;
    bool checkExHash(const QString &str_ex_hash);
    void updateIpPort(const QString &str_ex_hash, const QString &ip, int port);
    void updateConnectionStatus(const QString &str_ex_hash, bool status);
public slots:
    void doWork();
    void stopServer();
    void checkStatus() {
        emit getServerStatus(server->isListening());
    }
signals:
    void resultReady(int status);
    void getServerStatus(bool serverStatus);
private:
    QTcpServer *server;
    QSqlDatabase db;
};

class ControllerTcpServer : public QObject
{
    Q_OBJECT
    QThread tcpServerThread;
public:
    ControllerTcpServer() {
        TcpServer *tcpServer = new TcpServer;
        tcpServer->moveToThread(&tcpServerThread);
        connect(&tcpServerThread, &QThread::finished, tcpServer, &QObject::deleteLater);
        connect(this, &ControllerTcpServer::startTcpServer, tcpServer, &TcpServer::doWork);
        connect(this, &ControllerTcpServer::stopTcpServer, tcpServer, &TcpServer::stopServer);
        connect(tcpServer, &TcpServer::resultReady, this, &ControllerTcpServer::handleResults);
        connect(this, &ControllerTcpServer::checkServerStatus, tcpServer, &TcpServer::checkStatus);
        connect(tcpServer, &TcpServer::getServerStatus, this, &ControllerTcpServer::handleServerStatus);

        tcpServerThread.start();
    }
    ~ControllerTcpServer() {
        tcpServerThread.quit();
        tcpServerThread.wait();
    }
public slots:
    void handleResults(int status) {
        qDebug() << "handleResults " << status;
    }
    void handleServerStatus(bool serverStatus) {
        // qDebug() << "Сервер работает? " << (serverStatus ? "Да" : "Нет");
        emit serverStatusReceived(serverStatus);
    }
signals:
    void startTcpServer();
    void stopTcpServer();
    void checkServerStatus();
    void serverStatusReceived(bool serverStatus);
};

#endif // TCPSERVER_H
