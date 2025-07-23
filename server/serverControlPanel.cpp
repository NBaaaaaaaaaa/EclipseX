#include "serverControlPanel.h"
#include "databaseManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QProcess>

ServerControlPanel::ServerControlPanel(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);  // общий отступ между элементами. vmesto mainLayout->addSpacing(20);

    // Control Panel
    QWidget *controlPanelWidget = new QWidget(this);
    QVBoxLayout *controlPanelLayout = new QVBoxLayout(controlPanelWidget);
    controlPanelLayout->addWidget(new QLabel("Contol Panel"));

    QHBoxLayout *serverStatusLayout = new QHBoxLayout;
    serverStatusLabel = new QLabel(this);
    serverStatusLabel->setFixedSize(12, 12);
    // serverStatusLabel->setStyleSheet("QLabel { background-color: green; border-radius: 6px; }");
    serverStatusLabel->setStyleSheet("QLabel { background-color: red; border-radius: 6px; }");
    serverStatusLayout->addWidget(serverStatusLabel);
    serverStatusLayout->addWidget(new QLabel("Server status"));
    QPushButton *btnServerOnOff = new QPushButton("On/Off");
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

    controlPanelLayout->addLayout(serverStatusLayout);

    QHBoxLayout *dbStatusLayout = new QHBoxLayout;
    QLabel *dbStatusLabel = new QLabel(this);
    dbStatusLabel->setFixedSize(12, 12);
    // Вынести в отдельный поток проверку подключения к бд
    if (!DatabaseManager::instance().isConnectionAlive()) {
        dbStatusLabel->setStyleSheet("QLabel { background-color: red; border-radius: 6px; }");
        qDebug() << "Нет подключения к базе!";
    } else {
        dbStatusLabel->setStyleSheet("QLabel { background-color: green; border-radius: 6px; }");
    }
    dbStatusLayout->addWidget(dbStatusLabel);
    dbStatusLayout->addWidget(new QLabel("Database connection status"));
    QPushButton *btnDbOnOff = new QPushButton("Connect/Disconnect");
    dbStatusLayout->addWidget(btnDbOnOff);

    controlPanelLayout->addLayout(dbStatusLayout);
    mainLayout->addWidget(controlPanelWidget);

    // C2 methods
    QWidget *c2MethodsWidget = new QWidget(this);
    QVBoxLayout *c2MethodsLayout = new QVBoxLayout(c2MethodsWidget);

    c2MethodsLayout->addWidget(new QLabel("C2 method selection"));
    QRadioButton *radioSocket = new QRadioButton("Socket");
    QRadioButton *radioNext = new QRadioButton("Next");
    QPushButton *btnApplyC2Method= new QPushButton("Apply");


    c2MethodsLayout->addWidget(radioSocket);
    c2MethodsLayout->addWidget(radioNext);
    c2MethodsLayout->addWidget(btnApplyC2Method);
    mainLayout->addWidget(c2MethodsWidget);

    // Logs
    QWidget *logsWidget = new QWidget(this);
    QVBoxLayout *logsLayout = new QVBoxLayout(logsWidget);

    logsLayout->addWidget(new QLabel("Logs"));
    logViewer = new QTextEdit(this);
    logViewer->setReadOnly(true);

    logsLayout->addWidget(logViewer, 1);        // 1 = strech factor = будет растягиваться, занимая всё доступное пространство
    mainLayout->addWidget(logsWidget, 1);

    QProcess *tailProcess = new QProcess(this);
    tailProcess->start("tail", QStringList() << "-n" << "0" << "-F" << "/home/user/prog/test/logfile");

    connect(tailProcess, &QProcess::readyReadStandardOutput, this, [=]() {
        QByteArray newLines = tailProcess->readAllStandardOutput();
        logViewer->append(QString::fromUtf8(newLines).trimmed());
    });

    mainLayout->addStretch();
}
