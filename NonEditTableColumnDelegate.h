#ifndef NONEDITTABLECOLUMNDELEGATE_H
#define NONEDITTABLECOLUMNDELEGATE_H
#include <QItemDelegate>


/*!
  \file
  \brief Класс, реализующий запрет редактирования определённой колонки
  \date Начало модернизации лето 2022 года
  */
class NonEditTableColumnDelegate : public QItemDelegate
{
    Q_OBJECT
public:

    NonEditTableColumnDelegate(QObject * parent = 0) : QItemDelegate(parent) {}
    /*!
       * \brief createEdtor - Метод, позволяющий устанавливать на колонку режим чтения.
    */
    virtual QWidget * createEditor ( QWidget *, const QStyleOptionViewItem &,
                                     const QModelIndex &) const
    {
        return 0;
    }
};


#endif // NONEDITTABLECOLUMNDELEGATE_H
