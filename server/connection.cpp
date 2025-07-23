#include "connection.h"
#include "databaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QFormLayout>

#define MAX_HEIGHT 200
#define MIN_HEIGHT 95

Connection::Connection(const QString &str_ex_hash, QWidget *parent) : QFrame(parent) {
    this->setFrameShape(QFrame::Box);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *headWidget = new QWidget(this);
    QHBoxLayout *headLayout = new QHBoxLayout(headWidget);

    QCheckBox *pickConnectionBox = new QCheckBox(this);

    QSqlDatabase db = DatabaseManager::instance().database();
    QSqlQuery query(db);

    query.prepare("SELECT name, connection_status "
                  "FROM host_inventory "
                  "WHERE ex_hash = :hash");

    query.bindValue(":hash", str_ex_hash);
    QString name = "Error";
    connection_status = false;
    if (query.exec() && query.next()) {
        name = query.value(0).toString();
        connection_status = query.value(1).toBool();
    } else {
        qDebug() << "Ошибка в name, connection_status";
    }

    QLabel *connectionStatusLabel = new QLabel(this);
    connectionStatusLabel->setFixedSize(12, 12);
    if (connection_status) {
        connectionStatusLabel->setStyleSheet("QLabel { background-color: green; border-radius: 6px; }");
    } else {
        connectionStatusLabel->setStyleSheet("QLabel { background-color: red; border-radius: 6px; }");
    }

    query.prepare("SELECT c.ip, c.port "
                  "FROM connections c "
                  "JOIN host_inventory h ON c.ex_id = h.id "
                  "WHERE h.ex_hash = :hash");

    query.bindValue(":hash", str_ex_hash);

    QString ip = "Error";
    int port = -1;
    if (query.exec() && query.next()) {
        ip = query.value(0).toString();
        port = query.value(1).toInt();
        qDebug() << "IP:" << ip << "Port:" << port;
    } else {
        qDebug() << "Нет результатов.";
    }

    QWidget *nameIpPortWidget = new QWidget(this);
    QFormLayout *nameIpPortLayout = new QFormLayout(nameIpPortWidget);
    nameIpPortLayout->addRow("Name", new QLabel(name));
    nameIpPortLayout->addRow("IP:Port", new QLabel(QString("%1:%2").arg(ip).arg(port)));

    QPushButton *btnBlockConnection = new QPushButton("Block/Unblock");
    QPushButton *btnMonitorConnection = new QPushButton("Monitoring");
    btnToggle = new QPushButton("↓");
    connect(btnToggle, &QPushButton::clicked, this, &Connection::toggleConnection);

    headLayout->addWidget(pickConnectionBox);
    headLayout->addSpacing(10);
    headLayout->addWidget(connectionStatusLabel);
    headLayout->addWidget(nameIpPortWidget);
    headLayout->addStretch();
    headLayout->addWidget(btnBlockConnection);
    headLayout->addWidget(btnMonitorConnection);
    headLayout->addWidget(btnToggle);

    // Скрытые элементы
    hideWidget = new QWidget(this);
    QFormLayout *hideLayout = new QFormLayout(hideWidget);

    hideLayout->addRow("ex_hash:", new QLabel(QString("%1").arg(str_ex_hash)));
    hideLayout->addRow(new QLabel(""));

    query.prepare("SELECT o.sysname, o.nodename, o.release, o.version, o.machine, o.domainname "
                  "FROM os_info o "
                  "JOIN host_inventory h ON o.ex_id = h.id "
                  "WHERE h.ex_hash = :hash");

    query.bindValue(":hash", str_ex_hash);

    QString sysname = "Null";
    QString nodename = "Null";
    QString release = "Null";
    QString version = "Null";
    QString machine = "Null";
    QString domainname = "Null";

    if (query.exec() && query.next()) {
        sysname = query.value(0).toString();
        nodename = query.value(1).toString();
        release = query.value(2).toString();
        version = query.value(3).toString();
        machine = query.value(4).toString();
        domainname = query.value(5).toString();
    } else {
        qDebug() << "Нет результатов.";
    }

    hideLayout->addRow(new QLabel("OS info"));
    hideLayout->addRow("  OS name:", new QLabel(QString("%1").arg(sysname)));
    hideLayout->addRow("  Hostname:", new QLabel(QString("%1").arg(nodename)));
    hideLayout->addRow("  Kernel version:", new QLabel(QString("%1").arg(release)));
    hideLayout->addRow("  Build info:", new QLabel(QString("%1").arg(version)));
    hideLayout->addRow("  Architecture:", new QLabel(QString("%1").arg(machine)));
    hideLayout->addRow("  Domainname:", new QLabel(QString("%1").arg(domainname)));

    mainLayout->addWidget(headWidget);
    mainLayout->addWidget(hideWidget);
    mainLayout->addStretch();

    emit toggleConnection();
}

void Connection::toggleConnection() {
    isCollapsed = !isCollapsed;
    btnToggle->setText(isCollapsed ? "↓" : "↑");
    hideWidget->setVisible(!isCollapsed);

    if (listItem) {
        listItem->setSizeHint(this->sizeHint()); // обновляем размер
    }
}

void Connection::setListItem(QListWidgetItem *item) {
    listItem = item;
}

bool Connection::getConnection_status() {
    return connection_status;
}
