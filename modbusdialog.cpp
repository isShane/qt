#include "modbusdialog.h"
#include "ui_modbusdialog.h"

Modbusdialog::Modbusdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Modbusdialog)
{
    ui->setupUi(this);
    this->setWindowTitle("ModBus参数");
    this->setFixedSize(300,150);
//    this->setWindowModified(true);
    Qt::WindowFlags flags = Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint;
    this->setWindowFlags(flags);

    connect(ui->pushButton, &QPushButton::clicked, [this]() {
        mModbusArg.mCode = ui->FuncodeBox->currentIndex();
        mModbusArg.mSlave = ui->editAddr->text().toInt();
        //emit isModbusOk();
        hide();
    });

}

Modbusdialog::~Modbusdialog()
{
    delete ui;
}

Modbusdialog::ModbusArg Modbusdialog::getModbusArg() const
{
    return mModbusArg;
}
