#include "connections.h"
#include "connectionsList.h"
#include "connectionsFilters.h"
#include "connectionsCommands.h"

#include <QHBoxLayout>
#include <QPushButton>

Connections::Connections(QWidget *parent)
    : QWidget{parent}
{
    ConnectionsFilters *connectionsFilters = new ConnectionsFilters(this);
    ConnectionsList *connectionsList = new ConnectionsList(this);
    ConnectionsCommands *connectionsCommands = new ConnectionsCommands(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    mainLayout->addWidget(connectionsFilters);
    mainLayout->addWidget(connectionsList, 1);  // растягивается
    mainLayout->addWidget(connectionsCommands);

    connect(connectionsFilters, &ConnectionsFilters::filtersApplied, connectionsList, &ConnectionsList::applyFilters);
}
