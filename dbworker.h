#ifndef DBWORKER_H
#define DBWORKER_H
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDomElement>
#include <QComboBox>
#include <QPushButton>
#include <QMessageBox>
#include <QDebug>
#include <QTreeWidget>
#include <QAbstractButton>
class dbWorker
{
public:
    dbWorker();
    QSqlDatabase db;
    QSqlQuery *query;
public:
    void dbConnect(int n, QComboBox *QComboBox, QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions);
    void colorDbName(int n, QComboBox *QComboBox);
    void addComboBoxElements(int n, QComboBox *comboBox, QString dbName);
    void getControl(QTreeWidget *tree, QPushButton *button);
};

#endif // DBWORKER_H
