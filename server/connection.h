#ifndef CONNECTION_H
#define CONNECTION_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QListWidgetItem>

class Connection : public QFrame
{
    Q_OBJECT
public:
    explicit Connection(const QString &str_ex_hash, QWidget *parent = nullptr);
    void setListItem(QListWidgetItem *item);
    bool getConnection_status();
private slots:
    void toggleConnection();
private:
    bool isCollapsed = false;
    QPushButton *btnToggle;
    QWidget *hideWidget;
    QListWidgetItem *listItem = nullptr;

    bool connection_status;
};

#endif // CONNECTION_H
