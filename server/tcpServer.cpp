#include "tcpServer.h"

#include <QTcpSocket>

TcpServer::TcpServer(QObject *parent) : QObject(parent) {
    server = new QTcpServer(this);
    db = DatabaseManager::instance().database();
}

void TcpServer::doWork() {
    if (!server->listen(QHostAddress::Any, 3476)) {
        emit resultReady(1);
        return;
    }

    connect(server, &QTcpServer::newConnection, this, [this] {
        qDebug() << "new connect";

        QTcpSocket *clientSocket = server->nextPendingConnection();
        if (!clientSocket) {
            qWarning() << "Failed to get pending connection socket!";
            emit resultReady(1);
            return;
        }
        qDebug() << QString("Got client socket from %1 %2").arg(QHostAddress(clientSocket->peerAddress().toIPv4Address()).toString()).arg(clientSocket->peerPort());

        // connect(clientSocket, &QTcpSocket::bytesWritten, this, [clientSocket](qint64 bytes) {
        //     qDebug() << "Sent bytes to client:" << bytes;
        // });

        // qint64 written = clientSocket->write("Hello from server\r\n");
        // clientSocket->flush();
        // qDebug() << "Write returned:" << written;
        // QThread::sleep(10);

        // Обработчик приема сообщений
        bool isSendExHash = false;
        connect(clientSocket, &QTcpSocket::readyRead, clientSocket, [this, clientSocket, &isSendExHash]() {
            QByteArray data = clientSocket->readAll();
            QString recv = QString::fromUtf8(data);
            qDebug() << "Data received from client:" << data;

            if (!isSendExHash) {
                // Полученный хэш не из таблицы host_inventory
                if (!checkExHash(recv.toUtf8().constData())) {
                    emit resultReady(1);
                    return;
                }

                updateConnectionStatus(recv.toUtf8().constData(), true);
                updateIpPort(recv.toUtf8().constData(), QHostAddress(clientSocket->peerAddress().toIPv4Address()).toString(), clientSocket->peerPort());

                isSendExHash = true;
            }

        });


        // connect(clientSocket, &QTcpSocket::disconnected, clientSocket, &QTcpSocket::deleteLater);
        // updateConnectionStatus(recv.toUtf8().constData(), false);
    });
    emit resultReady(0);
}

void TcpServer::stopServer() {
    if (server->isListening()) {
        server->close();
        emit resultReady(0);
    }
}

bool TcpServer::checkExHash(const QString &str_ex_hash) {
    QSqlQuery query(db);
    query.prepare("SELECT 1 FROM host_inventory "
                  "WHERE ex_hash = :hash "
                  "LIMIT 1;");

    query.bindValue(":hash", str_ex_hash);

    if (query.exec() && query.next()) {
        return true;
    } else {
        qDebug() << "Ошибка в checkExHash:" << query.lastError().text();
        return false;
    }
}

void TcpServer::updateIpPort(const QString &str_ex_hash, const QString &ip, int port) {
    QSqlQuery query(db);
    query.prepare("UPDATE connections "
                  "SET ip = :ip, port = :port "
                  "WHERE ex_id = ( "
                  "     SELECT id FROM host_inventory WHERE ex_hash = :hash "
                  ")");

    query.bindValue(":ip", ip);
    query.bindValue(":port", port);
    query.bindValue(":hash", str_ex_hash);

    if (!query.exec()) {
        qDebug() << "Ошибка в updateIpPort:" << query.lastError().text();
    }
}

void TcpServer::updateConnectionStatus(const QString &str_ex_hash, bool status) {
    QSqlQuery query(db);
    query.prepare("UPDATE host_inventory "
                  "SET connection_status = :status "
                  "WHERE ex_hash = :hash");

    query.bindValue(":status", status);
    query.bindValue(":hash", str_ex_hash);

    if (!query.exec()) {
        qDebug() << "Ошибка в updateConnectionStatus:" << query.lastError().text();
    }
}
