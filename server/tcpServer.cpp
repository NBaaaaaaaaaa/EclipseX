#include "tcpServer.h"

#include <QTcpSocket>
#include <QThread>

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent) {}

void TcpServer::doWork() {
    if (!listen(QHostAddress::Any, 3476)) {
        emit resultReady(1);
        return;
    }
    emit resultReady(0);
}

void TcpServer::incomingConnection(qintptr socketDescriptor) {
    QMutexLocker locker(&mutex);

    QThread *thread = new QThread;
    ClientHandler *clientHandler = new ClientHandler(socketDescriptor);

    clientHandler->moveToThread(thread);

    connect(thread, &QThread::started, clientHandler, &ClientHandler::start);
    connect(clientHandler, &ClientHandler::finished, this, &TcpServer::clientFinished);
    connect(clientHandler, &ClientHandler::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(clientHandler, &ClientHandler::finished, clientHandler, &QObject::deleteLater);

    clientHandlers.append(clientHandler);
    thread->start();
}

void TcpServer::stopServer() {
    if (isListening()) {
        close();
    }

    {
        QMutexLocker locker(&mutex);
        for (ClientHandler *client : qAsConst(clientHandlers)) {
            if (client) {
                QMetaObject::invokeMethod(client, "disconnectClient", Qt::QueuedConnection);
            }
        }
    }

    // clientHandlers.clear();
    emit resultReady(0);
}

void TcpServer::clientFinished(ClientHandler *handler) {
    QMutexLocker locker(&mutex);
    clientHandlers.removeOne(handler);
}
