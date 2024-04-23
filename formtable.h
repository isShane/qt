#ifndef FORMTABLE_H
#define FORMTABLE_H

#include <QWidget>
#include <QTableWidget>

namespace Ui {
class FormTable;
}

class FormTable : public QWidget
{
    Q_OBJECT

public:
    explicit FormTable(QWidget *parent = nullptr);
    ~FormTable();

    bool loadFile(QString fileName);
    QTableWidget *tableWidget;



private:
    Ui::FormTable *ui;

};

#endif // FORMTABLE_H
