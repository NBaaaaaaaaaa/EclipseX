#include "connectionsFilters.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

#define MAX_WIDTH 200
#define MIN_WIDTH 50

ConnectionsFilters::ConnectionsFilters(QWidget *parent) : QFrame(parent) {
    this->setFixedWidth(MAX_WIDTH);
    // this->setStyleSheet("background-color: #3498db;");
    this->setFrameShape(QFrame::Box);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Header
    QHBoxLayout *headLayout = new QHBoxLayout;
    filtersLabel = new QLabel("Filters");
    headLayout->addWidget(filtersLabel);
    btnToggle = new QPushButton("←");
    connect(btnToggle, &QPushButton::clicked, this, &ConnectionsFilters::toggleFilters);
    headLayout->addWidget(btnToggle);

    mainLayout->addLayout(headLayout);

    // Text for collapsed
    textCollapsedWidget = new QWidget(this);
    QVBoxLayout *textCollapsedLayout = new QVBoxLayout(textCollapsedWidget);
    textCollapsedLayout->addWidget(new QLabel("F"));
    textCollapsedLayout->addWidget(new QLabel("I"));
    textCollapsedLayout->addWidget(new QLabel("L"));
    textCollapsedLayout->addWidget(new QLabel("T"));
    textCollapsedLayout->addWidget(new QLabel("E"));
    textCollapsedLayout->addWidget(new QLabel("R"));
    textCollapsedLayout->addWidget(new QLabel("S"));
    textCollapsedWidget->setVisible(isCollapsed);

    mainLayout->addWidget(textCollapsedWidget);

    // Filter Connection Status
    QWidget *connectionStatusWidget = new QWidget(this);
    QVBoxLayout *connectionStatusLayout = new QVBoxLayout(connectionStatusWidget);
    QPushButton *btnConnectionStatus = new QPushButton("↓ Connection Status");

    connectionStatusLayout->addWidget(btnConnectionStatus);

    QWidget *connectionStatusContentWidget = new QWidget(connectionStatusWidget);
    QVBoxLayout *connectionStatusContentLayout = new QVBoxLayout(connectionStatusContentWidget);
    isOnline = new QCheckBox("Online");
    isOffline = new QCheckBox("Offline");
    isOnline->setChecked(true);
    isOffline->setChecked(true);
    connectionStatusContentLayout->addWidget(isOnline);
    connectionStatusContentLayout->addWidget(isOffline);
    connectionStatusContentWidget->setVisible(isVisConStatF);

    connectionStatusLayout->addWidget(connectionStatusContentWidget);

    connect(btnConnectionStatus, &QPushButton::clicked, this, [=]() {
        isVisConStatF = !isVisConStatF;

        if (isVisConStatF) {
            btnConnectionStatus->setText("↑ Connection Status");
        } else {
            btnConnectionStatus->setText("↓ Connection Status");
        }

        connectionStatusContentWidget->setVisible(isVisConStatF);
    });

    filtersWidgets.append(connectionStatusWidget);
    mainLayout->addWidget(connectionStatusWidget);

    // Apply button
    btnApplyFilters = new QPushButton("Apply");
    connect(btnApplyFilters, &QPushButton::clicked, this, &ConnectionsFilters::onApplyButtonClicked);
    mainLayout->addWidget(btnApplyFilters);

    mainLayout->addStretch();
    emit toggleFilters();

}

void ConnectionsFilters::toggleFilters() {
    isCollapsed = !isCollapsed;

    if (isCollapsed) {
        this->setFixedWidth(MIN_WIDTH);                 // Узкая панель
        btnToggle->setText("→");
    } else {
        this->setFixedWidth(MAX_WIDTH);                 // Полная ширина
        btnToggle->setText("←");
    }

    for (QWidget *filtersWidget : filtersWidgets) {
        filtersWidget->setVisible(!isCollapsed);
    }
    filtersLabel->setVisible(!isCollapsed);
    textCollapsedWidget->setVisible(isCollapsed);
    btnApplyFilters->setVisible(!isCollapsed);
}

void ConnectionsFilters::onApplyButtonClicked() {
    const char *filePath = "filters.json";
    QJsonObject root;

    // Заполнение статуса подключения
    QJsonObject connectionStatus;
    connectionStatus["online"] = isOnline->isChecked();
    connectionStatus["offline"] = isOffline->isChecked();
    root["ConnectionStatus"] = connectionStatus;

    // Сохранение в файл
    QJsonDocument doc(root);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // Красивый формат
        file.close();
    } else {
        qDebug() << "Не удалось открыть файл для записи:" << filePath;
    }

    emit filtersApplied(filePath);
}
