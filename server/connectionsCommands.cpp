#include "connectionsCommands.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>

#define MAX_WIDTH 200
#define MIN_WIDTH 50

ConnectionsCommands::ConnectionsCommands(QWidget *parent) : QFrame(parent) {
    this->setFixedWidth(MAX_WIDTH);
    // this->setStyleSheet("background-color: #3498db;");
    this->setFrameShape(QFrame::Box);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Header
    QHBoxLayout *headLayout = new QHBoxLayout;
    commandsLabel = new QLabel("Commands");
    headLayout->addWidget(commandsLabel);
    btnToggle = new QPushButton("←");
    connect(btnToggle, &QPushButton::clicked, this, &ConnectionsCommands::toggleCommands);
    headLayout->addWidget(btnToggle);

    mainLayout->addLayout(headLayout);

    // Text for collapsed
    textCollapsedWidget = new QWidget(this);
    QVBoxLayout *textCollapsedLayout = new QVBoxLayout(textCollapsedWidget);
    textCollapsedLayout->addWidget(new QLabel("C"));
    textCollapsedLayout->addWidget(new QLabel("O"));
    textCollapsedLayout->addWidget(new QLabel("M"));
    textCollapsedLayout->addWidget(new QLabel("M"));
    textCollapsedLayout->addWidget(new QLabel("A"));
    textCollapsedLayout->addWidget(new QLabel("N"));
    textCollapsedLayout->addWidget(new QLabel("D"));
    textCollapsedLayout->addWidget(new QLabel("S"));
    textCollapsedWidget->setVisible(isCollapsed);

    mainLayout->addWidget(textCollapsedWidget);

    // Filter Connection Status
    QWidget *connectionStatusWidget = new QWidget(this);
    QVBoxLayout *connectionStatusLayout = new QVBoxLayout(connectionStatusWidget);
    QPushButton *btnConnectionStatus = new QPushButton("↓ Connection Status");  //↑

    connectionStatusLayout->addWidget(btnConnectionStatus);

    QWidget *connectionStatusContentWidget = new QWidget(connectionStatusWidget);
    QVBoxLayout *connectionStatusContentLayout = new QVBoxLayout(connectionStatusContentWidget);
    QCheckBox *isOnline = new QCheckBox("Online");
    QCheckBox *isOffline = new QCheckBox("Offline");
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

    commandsWidgets.append(connectionStatusWidget);
    mainLayout->addWidget(connectionStatusWidget);

    // Apply button
    btnApplyCommands = new QPushButton("Apply");
    mainLayout->addWidget(btnApplyCommands);

    mainLayout->addStretch();
    emit toggleCommands();
}

void ConnectionsCommands::toggleCommands() {
    isCollapsed = !isCollapsed;

    if (isCollapsed) {
        this->setFixedWidth(MIN_WIDTH);                 // Узкая панель
        btnToggle->setText("←");
    } else {
        this->setFixedWidth(MAX_WIDTH);                 // Полная ширина
        btnToggle->setText("→");
    }

    for (QWidget *commandsWidget : commandsWidgets) {
        commandsWidget->setVisible(!isCollapsed);
    }
    commandsLabel->setVisible(!isCollapsed);
    textCollapsedWidget->setVisible(isCollapsed);
    btnApplyCommands->setVisible(!isCollapsed);
}
