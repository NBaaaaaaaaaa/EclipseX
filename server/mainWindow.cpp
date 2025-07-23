#include "mainWindow.h"
#include "programMenu.h"
#include "content.h"
#include "databaseManager.h"

#include <QHBoxLayout>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    if (!DatabaseManager::instance().connect()) {
        qDebug() << "Ошибка подключения к базе!";
    }

    ProgramMenu *programMenu = new ProgramMenu(this);
    Content *content = new Content(this);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(programMenu);
    layout->addWidget(content, 1);  // растягивается

    QWidget *central = new QWidget(this);
    central->setLayout(layout);
    setCentralWidget(central);
    resize(1300, 600);

    connect(programMenu, &ProgramMenu::changePage, content, &Content::setPage);
}

MainWindow::~MainWindow() {}
