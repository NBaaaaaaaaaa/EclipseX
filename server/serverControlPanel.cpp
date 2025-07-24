#include "serverControlPanel.h"
#include "databaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QProcess>

ServerControlPanel::ServerControlPanel(QWidget *parent) : QWidget{parent}
{
    // Control Panel
    // Управление работой сервера (для socket - прослушка порта)
    serverStatusLabel = new QLabel(this);
    serverStatusLabel->setFixedSize(12, 12);
    // serverStatusLabel->setStyleSheet("QLabel { background-color: green; border-radius: 6px; }");
    serverStatusLabel->setStyleSheet("QLabel { background-color: red; border-radius: 6px; }");
    QPushButton *btnServerOnOff = new QPushButton("On/Off");

    QHBoxLayout *serverStatusLayout = new QHBoxLayout;
    serverStatusLayout->addWidget(serverStatusLabel);
    serverStatusLayout->addWidget(new QLabel("Server status"));
    serverStatusLayout->addWidget(btnServerOnOff);

    connect(btnServerOnOff, &QPushButton::clicked, this, [=]() {
        controllerTcpServer.checkServerStatus();
    });

    connect(&controllerTcpServer, &ControllerTcpServer::serverStatusReceived,
            this, [this](bool serverStatus) {
                if (serverStatus) {
                    controllerTcpServer.stopTcpServer();
                    // это надо мб через сигнал сделатьь
                    serverStatusLabel->setStyleSheet("QLabel { background-color: red; border-radius: 6px; }");
                } else {
                    controllerTcpServer.startTcpServer();
                    serverStatusLabel->setStyleSheet("QLabel { background-color: green; border-radius: 6px; }");
                }
            });

    // Управление подключением к бд
    QLabel *dbStatusLabel = new QLabel(this);
    dbStatusLabel->setFixedSize(12, 12);
    // Вынести в отдельный поток проверку подключения к бд
    if (!DatabaseManager::instance().isConnectionAlive()) {
        dbStatusLabel->setStyleSheet("QLabel { background-color: red; border-radius: 6px; }");
        qDebug() << "Нет подключения к базе!";
    } else {
        dbStatusLabel->setStyleSheet("QLabel { background-color: green; border-radius: 6px; }");
    }
    QPushButton *btnDbOnOff = new QPushButton("Connect/Disconnect");

    QHBoxLayout *dbStatusLayout = new QHBoxLayout;
    dbStatusLayout->addWidget(dbStatusLabel);
    dbStatusLayout->addWidget(new QLabel("Database connection status"));
    dbStatusLayout->addWidget(btnDbOnOff);

    QWidget *controlPanelWidget = new QWidget(this);
    QVBoxLayout *controlPanelLayout = new QVBoxLayout(controlPanelWidget);
    controlPanelLayout->addWidget(new QLabel("Contol Panel"));
    controlPanelLayout->addLayout(serverStatusLayout);
    controlPanelLayout->addLayout(dbStatusLayout);

    // C2 методы
    QRadioButton *radioSocket = new QRadioButton("Socket");
    QRadioButton *radioNext = new QRadioButton("Next");
    QPushButton *btnApplyC2Method= new QPushButton("Apply");

    QWidget *c2MethodsWidget = new QWidget(this);
    QVBoxLayout *c2MethodsLayout = new QVBoxLayout(c2MethodsWidget);
    c2MethodsLayout->addWidget(new QLabel("C2 method selection"));
    c2MethodsLayout->addWidget(radioSocket);
    c2MethodsLayout->addWidget(radioNext);
    c2MethodsLayout->addWidget(btnApplyC2Method);

    // Поле вывода логов
    logViewer = new QTextEdit(this);
    logViewer->setReadOnly(true);

    QWidget *logsWidget = new QWidget(this);
    QVBoxLayout *logsLayout = new QVBoxLayout(logsWidget);
    logsLayout->addWidget(new QLabel("Logs"));
    logsLayout->addWidget(logViewer, 1);        // 1 = strech factor = будет растягиваться, занимая всё доступное пространство

    QProcess *tailProcess = new QProcess(this);
    tailProcess->start("tail", QStringList() << "-n" << "0" << "-F" << "/home/user/prog/test/logfile");
    connect(tailProcess, &QProcess::readyReadStandardOutput, this, [=]() {
        QByteArray newLines = tailProcess->readAllStandardOutput();
        logViewer->append(QString::fromUtf8(newLines).trimmed());
    });


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);  // общий отступ между элементами. вместо mainLayout->addSpacing(20);
    mainLayout->addWidget(controlPanelWidget);
    mainLayout->addWidget(c2MethodsWidget);
    mainLayout->addWidget(logsWidget, 1);
    mainLayout->addStretch();
}
