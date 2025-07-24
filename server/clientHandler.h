#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QObject>
#include <QTcpSocket>
#include "databaseManager.h"

class ClientHandler : public QObject
{
    Q_OBJECT
public:
    explicit ClientHandler(qintptr socketDescriptor, QObject *parent = nullptr);
    ~ClientHandler();
    void setIsSendExHash(bool value) {
        isSendExHash = value;
    }
    void setStr_ex_hash(const QString &value) {
        str_ex_hash = value;
    }
    QString getStr_ex_hash() {
        return str_ex_hash;
    }
    bool getIsSendExHash() {
        return isSendExHash;
    }

public slots:
    void start();
    void disconnectClient();

private slots:
    void onReadyRead();
    void onDisconnected();

signals:
    void finished(ClientHandler *handler);

private:
    bool checkExHash(const QString &str_ex_hash);
    void updateIpPort(const QString &str_ex_hash, const QString &ip, int port);
    void updateConnectionStatus(const QString &str_ex_hash, bool status);

    qintptr socketDescriptor;
    QTcpSocket *socket = nullptr;
    QString str_ex_hash;
    bool isSendExHash = false;
    QSqlDatabase db;
};

#endif // CLIENTHANDLER_H
