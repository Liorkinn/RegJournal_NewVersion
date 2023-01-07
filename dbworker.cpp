#include "dbworker.h"

dbWorker::dbWorker()
{

}

void dbWorker::dbConnect(int n, QComboBox *QComboBox,  QString driverName, QString hostName, int port, QString dbName, QString pwd, QString userName, QString connectOptions)
{
    db = QSqlDatabase::addDatabase                  (driverName);
    db.setHostName                                  (hostName);
    db.setPort                                      (port);
    db.setDatabaseName                              (dbName);
    db.setPassword                                  (pwd);
    db.setUserName                                  (userName);
    db.setConnectOptions                            (connectOptions);
    if(db.open()){
        qDebug() << "[Success] Подключение к БД " + dbName + " произошло успешно!";
    }else{
        qDebug() << "[Success] Подключение к БД " + dbName + " произошло с провалом!";
    }
}

void dbWorker::colorDbName(int n, QComboBox *QComboBox)
{
    if(db.open()){
        QComboBox->setItemData( n, QColor( Qt::green ), Qt::BackgroundRole );
    }else{
        QComboBox->setItemData( n, QColor( Qt::red ), Qt::BackgroundRole );
    }
}

void dbWorker::addComboBoxElements(int n, QComboBox *comboBox, QString dbName)
{
    comboBox->addItem(dbName ,QVariant(n));
}

void dbWorker::getControl(QTreeWidget *tree, QPushButton *button)
{
    query = new QSqlQuery(db);
    QString schemaname = tree->selectionModel()->currentIndex().parent().siblingAtColumn(0).data().toString(); //схема
    QString tablename = tree->selectionModel()->currentIndex().siblingAtColumn(1).data().toString();  //таблица
    QString description = tree->selectionModel()->currentIndex().siblingAtColumn(2).data().toString(); //описание
    db = QSqlDatabase::database();

    if(tablename == NULL && description == NULL)
    {
        QMessageBox::critical(0, "Ошибка добавления", "Необходимо выбрать строку!");
    }else if(button->text() == "Поставить на учёт"){

        QString str = "CREATE TRIGGER Journal_i AFTER INSERT ON " + schemaname +"." + '"' + tablename + '"' + " FOR EACH ROW EXECUTE PROCEDURE chk.insert_journal_audit();"
                      "CREATE TRIGGER Journal_u AFTER UPDATE ON " + schemaname +"." + '"' + tablename + '"' + " FOR EACH ROW EXECUTE PROCEDURE chk.update_journal_audit();"
                      "CREATE TRIGGER Journal_d AFTER DELETE ON " + schemaname +"." + '"' + tablename + '"' + " FOR EACH ROW EXECUTE PROCEDURE chk.delete_journal_audit();"
                      "ALTER TABLE "  + schemaname +"." + '"' + tablename + '"' + " DISABLE TRIGGER journal_i;"
                      "ALTER TABLE "  + schemaname +"." + '"' + tablename + '"' + " DISABLE TRIGGER journal_u;"
                      "ALTER TABLE "  + schemaname +"." + '"' + tablename + '"' + " DISABLE TRIGGER journal_d;";
        if(query->exec(str)){
            QMessageBox::information(0, "Успешное добавление", "Триггеры на таблицу " + schemaname + "." + tablename + " успешно добавлены!");
            qDebug() << "[Success] Триггеры на таблицу " + schemaname + "." + tablename + " успешно добавлены! Выполненный запрос: " + str;}
        else{
            QMessageBox::critical(0,"Ошибка","Ошибка постановки защищаемого ресурса на контроль!");
            qDebug() << "[Error] Триггеры на таблицу " + schemaname + "." + tablename + " не добавлены! Проваленный запрос: " + str;}

        query->prepare("INSERT INTO chk.\"Table_state\"(description, schemaname, tablename) VALUES(:bdescription, :bschemaname, :btablename);");
        query->bindValue(":bdescription", description);
        query->bindValue(":bschemaname", schemaname);
        query->bindValue(":btablename", tablename);

        if(query->exec())
        {
            qDebug() << "[Success] Ресурс " + schemaname + "." + tablename + " успешно добавлен в журнал! Выполненный запрос: " + str;

            auto rows = tree->currentIndex();
            tree->model()->removeRow(rows.row(), rows.parent());

        }else
        {
             QMessageBox::critical(0,"Ошибка","Ошибка постановки защищаемого ресурса на контроль! Данные в журнал не добавлены!");
             qDebug() << "[Error] Ошибка выполнения запроса: " +  str;
        }
    }else if(button->text() == "Убрать с учёта")
    {
        QMessageBox msgBox;
        msgBox.setWindowTitle("Просмотр"); msgBox.setText("Вы уверены в удалении?"); msgBox.setMinimumHeight(500);
        QAbstractButton *remove = msgBox.addButton(QObject::tr("Удалить"), QMessageBox::ActionRole);
        QAbstractButton *cancel = msgBox.addButton(QObject::tr("Отмена"), QMessageBox::ActionRole);
        msgBox.exec();
        if(msgBox.clickedButton() == remove){
            QString delTrigger, delUpdate, delDelete;
            delTrigger = "DROP TRIGGER journal_i ON " + schemaname +"." + '"' + tablename + '"' + ";";
            delUpdate  = "DROP TRIGGER journal_u ON " + schemaname +"." + '"' + tablename + '"' + ";";
            delDelete  = "DROP TRIGGER journal_d ON " + schemaname +"." + '"' + tablename + '"' + ";";
            if(query->exec(delTrigger))
            {
                qDebug() << "[Success] Триггер вставки " + schemaname + "." + tablename + " успешно удален! Выполненный запрос: " + delTrigger;
            }else{
                qDebug() << "[Error] Ошибка выполнения запроса: " + delTrigger; QMessageBox::critical(0,"Удаление события", "Ошибка удаления события вставки из " + schemaname + "." + tablename + "!");
            }

            if(query->exec(delUpdate))
            {
                qDebug() << "[Success] Триггер обновления " + schemaname + "." + tablename + " успешно удален! Выполненный запрос: " + delUpdate;
            }else{
                qDebug() << "[Error] Ошибка выполнения запроса: " + delUpdate; QMessageBox::critical(0,"Удаление события", "Ошибка удаления события обновления из" + schemaname + "." + tablename + "!");
            }

            if(query->exec(delDelete))
            {
                qDebug() << "[Success] Триггер удаления" + schemaname + "." + tablename + " успешно удален! Выполненный запрос: " + delDelete;
            }else{
                qDebug() << "[Error] Ошибка выполнения запроса: " + delDelete; QMessageBox::critical(0,"Удаление события", "Ошибка удаления события удаления из:" + schemaname + "." + tablename + "!");
            }

            query->prepare("DELETE FROM chk.\"Table_state\" WHERE schemaname = :bschemaname AND tablename = :btablename;");
            query->bindValue(":bschemaname", schemaname);
            query->bindValue(":btablename", tablename);
            if(query->exec())
            {
               qDebug() << "[Success] Триггеры " + schemaname + "." + tablename + " успешно удалены!";

               auto rows = tree->currentIndex();
               tree->model()->removeRow(rows.row(), rows.parent());

            }else
            {
                QMessageBox::critical(0, "Ошибка удаления","При снятии с учёта возникла ошибка.");
                qDebug() << "[Error] Ошибка выполнения запроса для " + schemaname + "." + tablename + "!";
            }
        }else if(msgBox.clickedButton() == cancel)
            return;
    }
}


