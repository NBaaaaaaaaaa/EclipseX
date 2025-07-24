#include "db.h"

#include <QSqlTableModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QSortFilterProxyModel>

DB::DB(QWidget *parent) : QWidget{parent}
{
    QSqlTableModel *model = new QSqlTableModel(this);
    // model->setTable("connections");
    model->setTable("host_inventory");
    model->select(); // Загружает данные из таблицы

    // Прокси-модель для фильтрации
    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel();
    proxyModel->setSourceModel(model);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterKeyColumn(-1); // фильтровать по всем столбцам

    QTableView *tableView = new QTableView(this);
    tableView->setModel(proxyModel);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableView->setSortingEnabled(true);
    tableView->resizeColumnsToContents();  // Автоширина столбцов
    // tableView->resizeRowsToContents();     // Автовысота строк

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tableView);
}
