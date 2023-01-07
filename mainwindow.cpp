#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dbwork = new dbWorker();

    if (xml.open(PATH_DATABASE)) {          ///< В самом начале открывается database.xml, тем самым при успешном открытии происходит наполнение std::vector и заполнение comboBox..
        qDebug() << "[Information] Идёт чтение database.xml... Данные успешно получены!";
            dbworker = xml.ListElements(xml.getRoot(), "database");

            for(unsigned int n = 0; n < dbworker.size(); n++){
                dbwork->dbConnect (n,
                                   ui->comboBox_db,
                                   dbworker[n].attribute("driver"),
                                   dbworker[n].attribute("hostname"),
                                   dbworker[n].attribute("port").toInt(),
                                   dbworker[n].attribute("name"),
                                   "lostdox1",
                                   "postgres",
                                   dbworker[n].attribute("options"));

                dbwork->addComboBoxElements(n, ui->comboBox_db, dbworker[n].attribute("name"));
                dbwork->colorDbName(n, ui->comboBox_db);
            }
    }else ///< Если файл не откроется - пользователю выводится ошибка.
    {
        qDebug() << "[Error] Файл не открыт! Проверьте права доступа database.xml, убедитесь\nчто файл расположен по пути /mnt/abi/settings/,\nа также проверьте правильность содержимого xml файла.";
    }
}



void MainWindow::CreateUI_TreeWidget(const QString &description)
{
    if(description.isEmpty()){
         return;
    }

    QStringList list=description.split(",");


    if(list.length()!=treeConstants.descriptionCount)
        return ;

    QString dbnameStr = list.at(treeConstants.dbnameIdx);
    QString schemaStr = list.at(treeConstants.schemaIdx);
    QString tableStr  = list.at(treeConstants.tableIdx);
    QString descriptionStr= list.at(treeConstants.descriptionIdx);

    auto dbnameFind=ui->treeWidget->findItems(dbnameStr, Qt::MatchCaseSensitive,0);
    QTreeWidgetItem *dbName;
    dbnameFind.isEmpty() ? dbName=new QTreeWidgetItem
                          : dbName=dbnameFind.at(0);

    auto schemaFind=ui->treeWidget->findItems(schemaStr,
                                   Qt::MatchCaseSensitive | Qt::MatchRecursive, 0);
    QTreeWidgetItem *schemaName;
    schemaFind.isEmpty() ? schemaName=new QTreeWidgetItem(dbName)
                        : schemaName=schemaFind.at(0);
    QTreeWidgetItem *model  =new QTreeWidgetItem(schemaName);
    dbName->setText(0, dbnameStr);
    schemaName->setText(0, schemaStr);
    model->setText(1, tableStr);
    model->setText(2, descriptionStr);

    model->setFlags(Qt::ItemIsEditable|Qt::ItemIsEnabled);
    ui->treeWidget->setItemDelegateForColumn(0, new NonEditTableColumnDelegate);
    ui->treeWidget->setItemDelegateForColumn(1, new NonEditTableColumnDelegate);

    dbName->setToolTip(0, "Имя БД");

    schemaName->setToolTip(0, schemaName->parent()->text(0));

    for(int i=0; i<treeConstants.modelColumns; i++){

        switch (i){
        case 0:
            model->setToolTip(i, model->parent()->text(0));
            break;
        case 1:
            model->setToolTip(i, "Таблица");
            break;
        case 2:
            model->setToolTip(i, "Описание таблицы");
            break;
        }
    }
    ui->treeWidget->addTopLevelItem(dbName);



}

void MainWindow::settingsWidget()
{
    ui->treeWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void MainWindow::buttonChange()
{
    if(ui->pushButton_ckeck->text() == "Показать"){
        ui->pushButton_ckeck->setText("Отобразить для снятия с учёта");
        ui->pushButton_get->setText("Поставить на учёт");
        ui->label->setText("Список  таблиц базы данных " + ui->comboBox_db->currentText() + " для поставки на учёт");
        qDebug() << "[Information] Пользователь " + UserName + " просматривает список таблиц.";
    }else if(ui->pushButton_ckeck->text() == "Отобразить для снятия с учёта"){
        ui->pushButton_ckeck->setText("Отобразить для постановки на учёт");
        ui->pushButton_get->setText("Убрать с учёта");
        ui->label->setText("Список таблиц базы данных " + ui->comboBox_db->currentText() + " для снятия с учёта");
        qDebug() << "[Information] Пользователь " + UserName + " просматривает список  таблиц, поставленные на контроль.";
    }else if(ui->pushButton_ckeck->text() == "Отобразить для постановки на учёт"){
        ui->pushButton_ckeck->setText("Отобразить для снятия с учёта");
        ui->pushButton_get->setText("Поставить на учёт");
        ui->label->setText("Список таблиц базы данных " + ui->comboBox_db->currentText() + " для поставки на учёт");
        qDebug() << "[Information] Пользователь " + UserName + " просматривает список таблиц.";
    }
}

void MainWindow::removeRows()
{
    auto rows = ui->treeWidget->currentIndex();
    ui->treeWidget->model()->removeRow(rows.row(), rows.parent());
}

void MainWindow::checkTreeWidget()
{
    ui->treeWidget->clear();
    int i = ui->comboBox_db->currentIndex();
    dbname = ui->comboBox_db->currentText();
    dbwork->dbConnect (i,
                       ui->comboBox_db,
                       dbworker[i].attribute("driver"),
                       dbworker[i].attribute("hostname"),
                       dbworker[i].attribute("port").toInt(),
                       dbworker[i].attribute("name"),
                       "lostdox1",
                       "postgres",
                       dbworker[i].attribute("options"));

    if(dbwork->db.open() && ui->pushButton_get->text() == "Поставить на учёт"){
        AddElements();
        qDebug() << "[Information] Подключение к БД... \n[Success] Успешное подключение. Пользователь: " + UserName + ". Подключение к БД: " + dbname + " IP: "+ dbworker[i].attribute("hostname");
    }else if(dbwork->db.open() && ui->pushButton_get->text() == "Убрать с учёта"){
        AddElements();
    }else{
        QMessageBox::critical(0,"Ошибка соединения","Нет соединения с БД!");
        qDebug() << "[Information] Подключение к БД... \n[Error] Ошибка подключения к БД. Пользователь: " + UserName + ". Подключение к БД: " + dbname + " IP: "+ dbworker[i].attribute("hostname");
    }
}

void MainWindow::AddElements()
{
    QSqlQuery *query = new QSqlQuery(dbwork->db);
    QString dbNames = ui->comboBox_db->currentText();
    if(ui->pushButton_get->text() == "Поставить на учёт"){
        query->exec("SELECT t4.description, t2.nspname, t1.relname from pg_namespace as t2 LEFT JOIN pg_class as t1 on t1.relnamespace = t2.oid and  t1.relkind = 'r'::\"char\""
                                "LEFT JOIN  pg_description as t3 on  t3.objoid = t2.oid LEFT JOIN  pg_description as t4 on  t4.objoid = t1.oid and t4.objsubid=0 WHERE t2.nspname not in ('pg_catalog', 'pg_toast', 'pg_temp_1','information_schema', 'pg_toast_temp_1', 'chk')"
                                "AND t1.relname NOT IN(SELECT tablename FROM chk.\"Table_state\") AND t1.relname IS NOT NULL");
        for(int i = 0; query->next();i++)
        {
            CreateUI_TreeWidget(dbNames+","+ query->value(1).toString()+"," + query->value(2).toString() +"," + query->value(0).toString());
        }

        if(!query->exec("SELECT * FROM chk.\"Table_state\""))
        {
           QMessageBox::critical(0, "Ошибка!", "Необходимо создать таблицу Table_state в схеме chk для корректной работы программы!");
           return;
        }
    }else if(ui->pushButton_get->text() == "Убрать с учёта"){

        query->exec("SELECT description, schemaname, tablename FROM chk.\"Table_state\"");

        for(int i = 0; query->next();i++)
        {
            CreateUI_TreeWidget(dbNames+","+ query->value(1).toString()+"," + query->value(2).toString() +"," + query->value(0).toString());
        }

        if(!query->exec("SELECT * FROM chk.\"Table_state\""))
        {
           QMessageBox::critical(0, "Ошибка!", "Необходимо создать таблицу Table_state в схеме chk для корректной работы программы!");
           return;
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_close_clicked()
{
    QApplication::exit();
}

void MainWindow::on_pushButton_ckeck_clicked()
{
    buttonChange();
    checkTreeWidget();
}

void MainWindow::on_comboBox_db_currentIndexChanged(int index)
{
    ui->pushButton_ckeck->setText("Показать");
}

void MainWindow::on_pushButton_get_clicked()
{
    dbwork->getControl(ui->treeWidget,ui->pushButton_get);
}
