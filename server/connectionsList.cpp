#include "connectionsList.h"
#include "connectionsFilters.h"
#include "connectionsCommands.h"
#include "databaseManager.h"
#include "connection.h"

#include <QVBoxLayout>
#include <QListWidgetItem>
#include <QSqlError>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

ConnectionsList::ConnectionsList(QWidget *parent) : QWidget{parent}
{
    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    int count = 0;
    if (!query.exec("SELECT COUNT(*) FROM host_inventory")) {
        qDebug() << "Ошибка запроса:" << query.lastError().text();
    } else {
        if (query.next()) {
            count = query.value(0).toInt();
            // qDebug() << "Количество записей:" << count;
        }
    }

    connectionsListWidget = new QListWidget(this);
    connectionsListWidget->setSelectionMode(QAbstractItemView::NoSelection);
    connectionsListWidget->setSpacing(count);

    if (!query.exec("SELECT ex_hash FROM host_inventory")) {
        qDebug() << "Ошибка запроса в connectionsListWidget.";
    } else {
        while (query.next()) {
            QListWidgetItem *item = new QListWidgetItem(connectionsListWidget);
            Connection *connection = new Connection(query.value(0).toString(), this);

            connection->setListItem(item);
            item->setSizeHint(connection->sizeHint());
            connectionsListWidget->setItemWidget(item, connection);
        }
    }

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(connectionsListWidget, 1);
    mainLayout->addStretch();
}

void ConnectionsList::applyFilters(const QString &text) {
    bool showOnline;
    bool showOffline;

    QFile file(text);

    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        QJsonObject root = doc.object();

        QJsonObject connectionStatus = root["ConnectionStatus"].toObject();
        showOnline = connectionStatus["online"].toBool();
        showOffline = connectionStatus["offline"].toBool();
    }

    for (int i = 0; i < connectionsListWidget->count(); ++i) {
        QListWidgetItem *item = connectionsListWidget->item(i);
        Connection *conn = qobject_cast<Connection *>(connectionsListWidget->itemWidget(item));
        if (!conn) continue;

        // сделать приватной ее и гетер
        bool status = conn->getConnection_status(); // должен возвращать true (онлайн) или false (оффлайн)

        // Логика фильтрации:
        bool shouldShow = (status && showOnline) || (!status && showOffline);

        item->setHidden(!shouldShow);
    }
}
