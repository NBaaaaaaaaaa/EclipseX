#ifndef CONNECTIONSFILTERS_H
#define CONNECTIONSFILTERS_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>

class ConnectionsFilters : public QFrame
{
    Q_OBJECT
public:
    explicit ConnectionsFilters(QWidget *parent = nullptr);
private slots:
    void toggleFilters();
signals:
    void filtersApplied(const QString &text);
private:
    void onApplyButtonClicked();
    bool isCollapsed = false;
    bool isVisConStatF = false;
    QPushButton *btnToggle;
    QList<QWidget*> filtersWidgets;
    QLabel *filtersLabel;
    QWidget *textCollapsedWidget;
    QPushButton *btnApplyFilters;

    QCheckBox *isOnline;
    QCheckBox *isOffline;
};

#endif // CONNECTIONSFILTERS_H
