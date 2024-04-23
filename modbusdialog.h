#ifndef MODBUSDIALOG_H
#define MODBUSDIALOG_H

#include <QDialog>

namespace Ui {
class Modbusdialog;
}

class Modbusdialog : public QDialog
{
    Q_OBJECT

public:
    explicit Modbusdialog(QWidget *parent = nullptr);
    ~Modbusdialog();

    struct ModbusArg
    {
        int mCode = 1;
        int mSlave = 1;
    };

    ModbusArg getModbusArg() const;

private:
    Ui::Modbusdialog *ui;
    ModbusArg mModbusArg;

signals:
    void isModbusOk();
};

#endif // MODBUSDIALOG_H
