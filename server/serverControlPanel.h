#ifndef SERVERCONTROLPANEL_H
#define SERVERCONTROLPANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QProcess>

#include "tcpServer.h"

class ServerControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ServerControlPanel(QWidget *parent = nullptr);
    ~ServerControlPanel();
private:
    QTextEdit *logViewer;
    ControllerTcpServer controllerTcpServer;

    QProcess *tailProcess;
    QLabel *serverStatusLabel;
signals:
};

#endif // SERVERCONTROLPANEL_H
