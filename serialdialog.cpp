#include "serialdialog.h"
#include "ui_serialdialog.h"

SerialDialog::SerialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SerialDialog)
{
    ui->setupUi(this);
    this->setFixedSize(300,300);
    this->setWindowTitle("串口参数");         //设置窗口名

    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    //mSerialArg.isOk = false;
    mTimer = new QTimer();

    Refresh();

    connect(ui->pushButton, &QPushButton::clicked, [this]() {
        mSerialArg.mBaud = ui->baudBox->currentText().toInt();
        mSerialArg.mPortName = ui->portBox->currentText().mid(0,6);
        mSerialArg.mParity = //ui->parityBox->currentText()
        mSerialArg.mDataBits = ui->dataBitsBox->currentText().toInt();

        mSerialArg.mStopBits = ui->stopBitsBox->currentText().toInt();
        emit isOk();
//        mTimer->stop();
        hide();
    });
    connect(mTimer,&QTimer::timeout,this,&SerialDialog::on_mTimer);
}

SerialDialog::~SerialDialog()
{
    delete ui;
}

void SerialDialog::Refresh()
{
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        Portinfo += (info.portName() +"  "+ info.description());        //串口信息列表，显示
        newPortList += info.portName();             //用于判断列表长度是否变更
    }

    if(newPortList.size() != oldPortList.size())
    {
        oldPortList = newPortList;
        ui->portBox->clear();
        ui->portBox->addItems(Portinfo);
        //ui->portBox->setCurrentIndex(1);
    }
    Portinfo.clear();
    newPortList.clear();

#if 0

    qDebug() << oldPortList;

#endif
}

SerialDialog::SerialArg SerialDialog::serialArg() const
{
    return mSerialArg;
}

void SerialDialog::on_mTimer()
{
    Refresh();
}
