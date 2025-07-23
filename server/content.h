#ifndef CONTENT_H
#define CONTENT_H

#include <QFrame>
#include <QStackedWidget>

class Content : public QFrame
{
    Q_OBJECT
public:
    explicit Content(QWidget *parent = nullptr);
public slots:
    void setPage(int index);
private:
    QStackedWidget *stackedWidget;
};

#endif // CONTENT_H
