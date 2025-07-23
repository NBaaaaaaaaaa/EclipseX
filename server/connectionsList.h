#ifndef CONNECTIONSLIST_H
#define CONNECTIONSLIST_H

#include <QWidget>
#include <QListWidget>

class ConnectionsList : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectionsList(QWidget *parent = nullptr);

signals:
public slots:
    void applyFilters(const QString &text);
private:
    QListWidget *connectionsListWidget;
};

#endif // CONNECTIONSLIST_H
