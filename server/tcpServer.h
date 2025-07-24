#ifndef TCPSERVER_H
#define TCPSERVER_H
#include "clientHandler.h"
#include <QObject>
#include <QTcpServer>
#include <QThread>
#include <QMutex>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
    bool serverStatus = false;
public slots:
    void doWork();
    void stopServer();
    void checkStatus() {
        emit getServerStatus(isListening());
    }
private slots:
    void clientFinished(ClientHandler *handler);
signals:
    void resultReady(int status);
    void getServerStatus(bool serverStatus);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private:
    QList<ClientHandler*> clientHandlers;
    QMutex mutex;
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
