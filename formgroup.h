#ifndef FORMGROUP_H
#define FORMGROUP_H

#include <QWidget>
#include <QTableWidget>
#include <QTimer>
#include <QSettings>
#include <QCloseEvent>
#include <QMenu>
#include <QMessageBox>
#include <QInputDialog>

namespace Ui {
class FormGroup;
}

class FormGroup : public QWidget
{
    Q_OBJECT

public:
    explicit FormGroup(QWidget *parent = nullptr);
    ~FormGroup();

    void insert(int id,QString name,QString addr,int data,QString flag);
    void findChange(QString loaction,int data);

private:
    Ui::FormGroup *ui;
    QSettings *config;
    void setConfig();
    QList<int> removeDuplicates(const QList<int>& list);
    int row = 0;
    QList<int> reverseList(QList<int> list);
    bool compareDescending(const int &a, const int &b) {
        return a > b;
    }

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void groupDataChange(int id,int addr,int data);

private slots:
    void on_btnDeleteAll_clicked();
    void on_btnDelete_clicked();
    void on_btnShowChart_clicked();
    void on_menu(QPoint pos);
//    void on_activeCell(int row,int col);
//    void on_tableWidget_cellChanged(int row, int column);
    void doubleClicked(int row,int col);

};

#endif // FORMGROUP_H
