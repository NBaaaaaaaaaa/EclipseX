#include "tcpServer.h"
#include <iostream>

void TcpServer::doWork() {
    if (!server->listen(QHostAddress::Any, 3476)) {
        emit resultReady(1);
        return;
    }
    connect(server, &QTcpServer::newConnection, this, [] {
        qDebug() << "new connect";
        // здесь обработка входящего соединения
    });
    emit resultReady(0);
}

void TcpServer::stopServer() {
    if (server->isListening()) {
        server->close();
        emit resultReady(0);
    }
}
