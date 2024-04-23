#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QModbusRtuSerialMaster>
#include <QtSerialPort/QSerialPort>         // 提供访问串口的功能
#include <QtSerialPort/QSerialPortInfo>
#include <QMessageBox>
#include <QTimer>
#include <QCloseEvent>
#include <QTableWidget>
#include <QInputDialog>
#include <QFormLayout>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QMdiSubWindow>
#include <QFileInfo>
#include <QDateTime>

#include "serialdialog.h"
#include "modbusdialog.h"
#include "formouttable.h"
#include "outdialog.h"
#include "formgroup.h"
#include "formchart.h"

#define IMFORMATION_DEBUG 1

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void InitMenu();
    void ReadTable(int tableCount);
    void WriteTable(int id, int start, int val);
    void WriteDialog(int addr, int val, bool doubleCliked);
    void isAutoRead();
    void onReadReady(int size);
    bool loadFile(QString fileName);
    void loadFiles(QList<QString> fileNameList);
    void auto_getFile();
    void readConfig();

protected:

    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_mReadTimer();
    void on_autoReadAction();
    void on_DoubleClick(int row,int col);
    void on_actionconnect_triggered();
    void on_action_1_write_triggered();
    void on_readAction_triggered();
    void on_actionClearDock_triggered();
    void on_actionShowAllDock_triggered();    
    void on_action_tidy_triggered();
    void on_action_test_triggered();
    void on_action_stop_triggered();
    void on_action_in_triggered();
    void on_action_tab_triggered();
    void on_action_hide_triggered();
    void on_action_out_triggered();
    void on_IsOk(int time, int count);
    void on_startUpdata();
    void onShowMenu(QPoint pos);
    void on_actionHideEorro_triggered();
    void getChangeCell(int row, int col,QString objName);
    void originalData(int row,QString objName);

    void on_actionGroup_triggered();

private:
    Ui::MainWindow *ui;
    QModbusClient  *modbusDevice = nullptr;
    QTimer *mReadTimer;
    SerialDialog *mSerial;
    Modbusdialog *mModbus;
    FormOutTable *mOutTable;
    outDialog *outSetting;
    QList<QTableWidget*> m_table;
    QTableWidget *tableWidget;
    QLabel *error;
    QLabel *infor;
    QTableWidget *Twidget;
    FormGroup *mGroup;


    FormChart *mChart;


signals:

};


#endif // MAINWINDOW_H
