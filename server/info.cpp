#include "info.h"

#include <QVBoxLayout>
#include <QTextBrowser>

Info::Info(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QTextBrowser *infoTextViewer = new QTextBrowser(this);
    infoTextViewer->setReadOnly(true);
    infoTextViewer->setOpenExternalLinks(true);  // Разрешить открытие ссылок
    infoTextViewer->setTextInteractionFlags(Qt::TextBrowserInteraction); // Включает взаимодействие (клик по ссылке и т.д.)

    QString html = R"(
<b>EclipseX</b> — исследовательский проект.<br>
Цель — изучить внутреннее устройство ядра Linux.<br><br>
Исходный код проекта доступен по ссылке:
<a href='https://github.com/NBaaaaaaaaaa/EclipseX/tree/main'>GitHub</a>
)";

    infoTextViewer->setHtml(html);  // Устанавливаем HTML-текст

    mainLayout->addWidget(infoTextViewer, 1);

    mainLayout->addStretch();
}
