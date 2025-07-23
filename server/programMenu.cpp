#include "programMenu.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#define MAX_WIDTH 200
#define MIN_WIDTH 50

ProgramMenu::ProgramMenu(QWidget *parent) : QFrame(parent) {
    this->setFixedWidth(MAX_WIDTH);
    // this->setStyleSheet("background-color: #3498db;");
    this->setFrameShape(QFrame::Box);
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *headLayout = new QHBoxLayout;
    logoLabel = new QLabel("EclipseX");
    headLayout->addWidget(logoLabel);
    btnToggle = new QPushButton("←");
    connect(btnToggle, &QPushButton::clicked, this, &ProgramMenu::toggleMenu);
    headLayout->addWidget(btnToggle);

    mainLayout->addLayout(headLayout);
    QPushButton *btnServerInfo = new QPushButton("Server");
    btnServerInfo->setIcon(QIcon(":/icons/src/icons/server.png"));
    btnServerInfo->setIconSize(QSize(16, 16));
    menuButtons.append(btnServerInfo);
    mainLayout->addWidget(btnServerInfo);

    QPushButton *btnDB = new QPushButton("DB");
    btnDB->setIcon(QIcon(":/icons/src/icons/database.png"));
    btnDB->setIconSize(QSize(16, 16));
    menuButtons.append(btnDB);
    mainLayout->addWidget(btnDB);

    QPushButton *btnConnections = new QPushButton("Connections");
    btnConnections->setIcon(QIcon(":/icons/src/icons/connections.png"));
    btnConnections->setIconSize(QSize(16, 16));
    menuButtons.append(btnConnections);
    mainLayout->addWidget(btnConnections);

    QPushButton *btnAbout = new QPushButton("About");
    btnAbout->setIcon(QIcon(":/icons/src/icons/info.png"));
    btnAbout->setIconSize(QSize(16, 16));
    menuButtons.append(btnAbout);
    mainLayout->addWidget(btnAbout);

    mainLayout->addStretch();

    connect(btnServerInfo, &QPushButton::clicked, this, [=]() {
        emit changePage(0);
    });
    connect(btnConnections, &QPushButton::clicked, this, [=]() {
        emit changePage(1);
    });
    connect(btnDB, &QPushButton::clicked, this, [=]() {
        emit changePage(2);
    });
    connect(btnAbout, &QPushButton::clicked, this, [=]() {
        emit changePage(3);
    });
}

void ProgramMenu::toggleMenu() {
    isCollapsed = !isCollapsed;

    if (isCollapsed) {
        this->setFixedWidth(MIN_WIDTH);                 // Узкая панель
        for (QPushButton *btn : menuButtons) {
            btn->setText("");                           // Убираем текст
            // btn->setToolTip("Some info");               // Чтобы появлялся при наведении
            // btn->setStyleSheet("text-align: left;");    // необязательно
        }
        logoLabel->setVisible(!isCollapsed);
        btnToggle->setText("→");
    } else {
        this->setFixedWidth(MAX_WIDTH);                 // Полная ширина
        menuButtons[0]->setText("Server");
        menuButtons[1]->setText("DB");
        menuButtons[2]->setText("Connections");
        menuButtons[3]->setText("About");
        logoLabel->setVisible(!isCollapsed);
        btnToggle->setText("←");
    }
}
