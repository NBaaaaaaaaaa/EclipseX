#ifndef CONNECTIONSCOMMANDS_H
#define CONNECTIONSCOMMANDS_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>

class ConnectionsCommands : public QFrame
{
    Q_OBJECT
public:
    explicit ConnectionsCommands(QWidget *parent = nullptr);
private slots:
    void toggleCommands();
signals:
private:
    bool isCollapsed = false;
    bool isVisConStatF = false;
    QPushButton *btnToggle;
    QList<QWidget*> commandsWidgets;
    QLabel *commandsLabel;
    QWidget *textCollapsedWidget;
    QPushButton *btnApplyCommands;
};

#endif // CONNECTIONSCOMMANDS_H
