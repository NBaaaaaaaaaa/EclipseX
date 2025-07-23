#ifndef SERVERCONTROLPANEL_H
#define SERVERCONTROLPANEL_H

#include <QWidget>
#include <QTextEdit>
#include <QLabel>

#include "tcpServer.h"

class ServerControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ServerControlPanel(QWidget *parent = nullptr);
private:
    QTextEdit *logViewer;
    ControllerTcpServer controllerTcpServer;

    QLabel *serverStatusLabel;
signals:
};

#endif // SERVERCONTROLPANEL_H
