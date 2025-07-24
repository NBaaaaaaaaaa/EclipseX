#include "content.h"
#include "serverControlPanel.h"
#include "connections.h"
#include "db.h"
#include "info.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>

Content::Content(QWidget *parent) : QFrame(parent) {
    // this->setStyleSheet("background-color:rgb(117, 240, 219);");
    this->setFrameShape(QFrame::Box);

    ServerControlPanel *serverControlPanel = new ServerControlPanel(this);
    Connections *connections = new Connections(this);
    DB *db = new DB(this);
    Info *info = new Info(this);

    stackedWidget = new QStackedWidget(this);
    stackedWidget->addWidget(serverControlPanel);
    stackedWidget->addWidget(connections);
    stackedWidget->addWidget(db);
    stackedWidget->addWidget(info);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(stackedWidget, 1);
    setLayout(mainLayout);
}

void Content::setPage(int index) {
    if (index >= 0 && index < stackedWidget->count()) {
        stackedWidget->setCurrentIndex(index);
    }
}
