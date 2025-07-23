#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>

class DatabaseManager {
public:
    static DatabaseManager& instance() {
        static DatabaseManager _instance;
        return _instance;
    }

    bool connect() {
        db = QSqlDatabase::addDatabase("QPSQL");
        db.setHostName("localhost");
        db.setDatabaseName("eclipsex_db");
        db.setUserName("eclipsex");
        db.setPassword("eclipsexpassword");
        return db.open();
    }

    QSqlDatabase& database() {
        return db;
    }

    bool isConnectionAlive() {
        QSqlQuery query(db);
        if (!query.exec("SELECT 1")) {
            return false;
        }
        return true;
    }

private:
    DatabaseManager() {}
    ~DatabaseManager() {
        if (db.isOpen()) db.close();
    }
    QSqlDatabase db;
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
};


#endif // DATABASEMANAGER_H
