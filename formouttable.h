#ifndef FORMOUTTABLE_H
#define FORMOUTTABLE_H

#include <QWidget>
#include <QTableWidget>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QFileDialog>
#include <QAxObject>
#include "mythread.h"

#define START_VAL 3
namespace Ui {
class FormOutTable;
}

class FormOutTable : public QWidget
{
    Q_OBJECT

public:
    explicit FormOutTable(QWidget *parent = nullptr);
    ~FormOutTable();
    void newTable(int row, int col,QTableWidgetItem *id,QTableWidgetItem *addr);

    void updataTable( QList<QTableWidgetItem *> dataList);
    //void headTable();
    void InitTable(QList<QTableWidgetItem*> sourceList ,QList<QTableWidgetItem*> dataList);
    void saveCSV(const QString &fileName);
    void saveExcel(const QString &fileName);
    void gogogo(bool go,int timer);
    void autoSave();
    QTimer *mExortTimer;

//    QTableWidget * table;

protected:
    void closeEvent(QCloseEvent *event) override;


private slots:
    void on_saveBtn_clicked();

private:
    Ui::FormOutTable *ui;
    int count = START_VAL;
    int limitCount;
    bool isSave = false;
        //Mythread mythread;


signals:
    void stopRecords();
    void startUpdata();

private slots:
        void on_mExortTimer();
        void on_pushButton_clicked();
};

#endif // FORMOUTTABLE_H
