#ifndef SERIALDIALOG_H
#define SERIALDIALOG_H

#include <QDialog>
#include <QSerialPort>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtDebug>
#include <QTimer>

namespace Ui {
class SerialDialog;
}

class SerialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SerialDialog(QWidget *parent = nullptr);
    ~SerialDialog();

    void Refresh();

    QStringList Portinfo,oldPortList,newPortList;
    QTimer *mTimer;

    struct SerialArg {
        QString mPortName = "Null ";
        int mParity,mBaud,mDataBits,mStopBits;
        bool isOk;
//        int responseTime = 1000;
//        int numberOfRetries = 3;
    };
    SerialArg serialArg() const;
private:
    Ui::SerialDialog *ui;
    SerialArg mSerialArg;

private slots:
    void on_mTimer();
signals:
    void isOk();
};


#endif // SERIALDIALOG_H
