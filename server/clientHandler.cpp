#include "clientHandler.h"

ClientHandler::ClientHandler(qintptr descriptor, QObject *parent)
    : QObject(parent) {
    socketDescriptor = descriptor;
    db = DatabaseManager::instance().database();
}

ClientHandler::~ClientHandler() {
    qDebug() << "ClientHandler destroyed";
    if (socket) {
        socket->deleteLater();
    }
}

void ClientHandler::start() {
    socket = new QTcpSocket();

    if (!socket->setSocketDescriptor(socketDescriptor)) {
        emit finished(this);
        return;
    }

    connect(socket, &QTcpSocket::readyRead, this, &ClientHandler::onReadyRead);
    connect(socket, &QTcpSocket::disconnected, this, &ClientHandler::onDisconnected);

    qDebug() << "New client connected:" << QHostAddress(socket->peerAddress().toIPv4Address()).toString() << socket->peerPort();
}

void ClientHandler::onReadyRead() {
    QByteArray data = socket->readAll();
    QString recv = QString::fromUtf8(data);

    qDebug() << "Data received from client:" << recv;

    if (!isSendExHash) {
        if (!checkExHash(recv.toUtf8().constData())) {
            socket->disconnectFromHost();
            return;
        }

        updateConnectionStatus(recv, true);
        updateIpPort(recv, QHostAddress(socket->peerAddress().toIPv4Address()).toString(), socket->peerPort());

        str_ex_hash = recv;
        isSendExHash = true;

        qDebug() << "Dat";
    }
}

void ClientHandler::onDisconnected() {
    qDebug() << "Client disconnected";

    updateConnectionStatus(str_ex_hash, false);

    emit finished(this);
}

void ClientHandler::disconnectClient() {
    if (socket && socket->state() == QTcpSocket::ConnectedState) {
        updateConnectionStatus(str_ex_hash, false);
        socket->disconnectFromHost();
    }
}

bool ClientHandler::checkExHash(const QString &str_ex_hash) {
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

void ClientHandler::updateIpPort(const QString &str_ex_hash, const QString &ip, int port) {
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

void ClientHandler::updateConnectionStatus(const QString &str_ex_hash, bool status) {
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
