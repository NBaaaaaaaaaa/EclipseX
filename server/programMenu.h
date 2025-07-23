#ifndef PROGRAMMENU_H
#define PROGRAMMENU_H

#include <QWidget>
#include <QFrame>
#include <QPushButton>
#include <QList>
#include <QLabel>

class ProgramMenu : public QFrame
{
    Q_OBJECT
public:
    explicit ProgramMenu(QWidget *parent = nullptr);
private slots:
    void toggleMenu();
signals:
    void changePage(int index);
private:
    bool isCollapsed = false;
    QPushButton *btnToggle;
    QList<QPushButton*> menuButtons;
    QLabel *logoLabel;
};

#endif // PROGRAMMENU_H
