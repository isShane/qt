#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>

#define FLAG_SIZE_ONE   12
#define FLAG_SIZE_TEN   13
#define FLAG_SIZE_HUN   14

#define DELAY_TIME 200
#define COILS    0
#define HOLDING  1
#define INPUT    2

#define MY_HEX 16
#define DATA_COL 2
#define MEAN_COL 0
#define ADDR_COL 1

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //设置窗口名
    this->setWindowTitle("ModBus Master");

    //定时自动读取
    mReadTimer = new QTimer();
    //modbus
    modbusDevice = new QModbusRtuSerialMaster();
    //串口参数
    mSerial = new SerialDialog(this);

    //    mModbus = new Modbusdialog();
    //数据表
    mGroup = new FormGroup;

    //曲线图对象
    mChart = new FormChart;

    //显示异常信息
    error = new QLabel;
    error->setStyleSheet("QLabel { font-size: 10pt; color: red; }");    //字体大小颜色
    ui->toolBar_2->addWidget(error);

    //状态栏信息
    infor = new QLabel(this);
    infor->setText("未连接");
    infor->setStyleSheet("QLabel {color: red; font:bold}");
    ui->toolBar_3->addWidget(infor);

    //设置断开连接不可用
    ui->disconnectAction->setDisabled(true);

    //初始化信号
    InitMenu();

    //自动读取定时器
    connect(mReadTimer,&QTimer::timeout,this,&MainWindow::on_mReadTimer);

    auto_getFile();

    //打开软件同时打开数据表

    readConfig();
    on_actionGroup_triggered();
}


MainWindow::~MainWindow()
{
    delete ui;
}

//处理信号与槽的连接
void MainWindow::InitMenu()
{
    //关闭动作
    connect(ui->closeAction,&QAction::triggered,this, &MainWindow::close);

    //连接动作弹出串口参数
    connect(ui->connectAction, &QAction::triggered, mSerial, [this]() {
        mSerial->mTimer->start(1000);  //定时刷新串口
        mSerial->exec();
    });

    //连接窗口点击ok，触发连接
    connect(mSerial,SIGNAL(isOk()),this,SLOT(on_actionconnect_triggered()));

    //断开
    connect(ui->disconnectAction,&QAction::triggered,this, &MainWindow::on_actionconnect_triggered);

    //自动读取
    connect(ui->autoReadAction,&QAction::triggered,this,&MainWindow::on_autoReadAction);

    //数据表写入值
    connect(mGroup,&FormGroup::groupDataChange,this,&MainWindow::WriteTable);

}

//连接
void MainWindow::on_actionconnect_triggered()
{
    mSerial->mTimer->stop();

    QIcon on_ico(":/Images/connected");
    QIcon off_ico(":/Images/disconnected");

    //状态是未连接
    if(modbusDevice->state() == QModbusDevice::UnconnectedState){

        //获取串口参数
        QString mPortName = mSerial->serialArg().mPortName;
        int  mBaud = mSerial->serialArg().mBaud,
                mParity = 0,                                    //校验位
                mDataBits = mSerial->serialArg().mDataBits,     //数据位
                mStopBits = mSerial->serialArg().mStopBits;     //停止位

        if(mPortName.contains("Null")){
            QMessageBox::information(mSerial,"提示","请选择可用串口！");
            ui->connectAction->trigger();
            return ;
        }
        modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,mPortName);  //端口号
        modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,mBaud);      //波特率
        modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,mParity);      //校验位
        modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,mDataBits);  //数据位
        modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,mStopBits);  //停止位
//        switch (mStopBits) {
//        case 1:
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::OneStop);  //停止位
//            break;
//        case 2:
//            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,QSerialPort::TwoStop);  //停止位
//            break;
//        }


        //设置失败重试次数
        modbusDevice->setNumberOfRetries(0);

        if(!modbusDevice->connectDevice())
        {

            QMessageBox::information(mSerial,"提示","串口 " + mPortName + "打开失败");

            infor->setText("拒绝访问："+ mPortName);

        }else{
            infor->setStyleSheet("QLabel {color: black; }");
            infor->setText("已连接： " +
                           mPortName+QString::number(mBaud)+"-"+
                           QString::number(mDataBits)+"-"+
                           "N"+"-"+
                           QString::number(mStopBits));

            ui->disconnectAction->setDisabled(false);
            ui->connectAction->setDisabled(true);

            ui->action_in->setDisabled(true);

            ui->actionconnect->setText("断开");
            ui->actionconnect->setIcon(off_ico);

            ui->actionClearDock->setDisabled(true);

            //有表的时候才会自动读取
            if(ui->mdiArea->subWindowList().count() > 0){
                ui->autoReadAction->setChecked(true);
                mReadTimer->start(DELAY_TIME);
            }

        }



    }else if(modbusDevice->state() == QModbusDevice::ConnectedState){

        modbusDevice->disconnectDevice();

        if(modbusDevice->state() == QModbusDevice::UnconnectedState){   //确认已断开

            infor->setText("未连接");
            infor->setStyleSheet("QLabel {color: red;}");

            mReadTimer->stop();                 //断开时停止定时器
            ui->actionconnect->setText("连接");
            ui->actionconnect->setIcon(on_ico);     //工具栏的连接
            ui->actionconnect->setDisabled(false);

            ui->disconnectAction->setDisabled(true);        //菜单栏的断开
            ui->connectAction->setDisabled(false);      //菜单栏的连接

            ui->action_in->setDisabled(false);
            ui->actionClearDock->setDisabled(false);

            ui->autoReadAction->setChecked(false);

        }

    }
}

//接收双击的单元格的行列
void MainWindow::on_DoubleClick(int row,int col)
{
    int data = 0,
            addr =0;

    QWidget *widget = ui->mdiArea->activeSubWindow()->widget();
    QTableWidget *Twidget = qobject_cast<QTableWidget *>(widget);

    //获取内容，内容为空会崩溃
    if(Twidget->item(row ,col) != nullptr){
        data = Twidget->item(row ,col)->text().toInt();
    }
    if(Twidget->item(1 ,col) != nullptr && col == 2){       //从第一行数据列开始
        addr = Twidget->item(1,1)->text().toInt(nullptr,16) + row - 1;  //寄存器地址
        //获取寄存器地址，一行一列是首地址，如首地址是2300，双击2301的数据格（2，2），
        //则当前双击的数据格对应的地址就是2300+行-1 = 2300+2-1 = 2301
        WriteDialog(addr, data,true);
    }
}

//自动读取
void MainWindow::on_autoReadAction()
{
    if(modbusDevice->state() == QModbusDevice::UnconnectedState){
        QMessageBox::information(this,"提示","设备未连接");
        ui->autoReadAction->setChecked(false);
        return ;
    }
    isAutoRead();

}

//判断是否打开自动读取
void MainWindow::isAutoRead()
{
    if(ui->autoReadAction->isChecked()){
        mReadTimer->start(DELAY_TIME);
    }
    else{
        mReadTimer->stop();
    }
}

//到时间触发读取按钮
void MainWindow::on_mReadTimer()
{
    on_readAction_triggered();
}

//读取按钮
void MainWindow::on_readAction_triggered()
{
    if(modbusDevice->state() == QModbusDevice::UnconnectedState){

        QMessageBox::information(this,"提示","设备未连接");

        return ;
    }

    mReadTimer->stop();     //按下读取后停止定时器

    for (int i = 0; i < m_table.count(); i++) {

        ReadTable(i);
    }
    // QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//读功能实现
void MainWindow::ReadTable(int tableCount)
{
    //读取表中的从机地址
    int mPortAddr = m_table[tableCount]->item(0,1)->text().toInt();  //2100

    //   int mCode    = mModbus->getModbusArg().mCode;       //03   功能码
    int mRegister = m_table[tableCount]->item(1,1)->text().toInt(nullptr,16);  //2100    寄存器地址
    int mSize    = m_table[tableCount]->rowCount() - 1;     //0000  根据表的行数确定要查询的数量     减去从机地址行

    QModbusReply *reply;

    //选择功能码
    //    if (mCode == COILS)
    //        reply = modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::Coils, mRegister, mSize),mPortAddr);

    //    if (mCode == HOLDING)     //默认使用holding
    reply = modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::HoldingRegisters, mRegister, mSize),mPortAddr);//发送读请求

    //    if (mCode == INPUT)
    //        reply = modbusDevice->sendReadRequest(QModbusDataUnit(QModbusDataUnit::InputRegisters,mRegister, mSize),mPortAddr);


    //等待接收完成
    if (!reply->isFinished()){

        connect(reply, &QModbusReply::finished, this, [=]() {       //lambda表达式，[=]表示可以使用局部变量
            onReadReady(mSize);     //读到的数据放入表中

            //没有错误
            if(reply->error() == QModbusDevice::NoError){

                error->setText(" ");

                //设置当前表标题
                m_table[tableCount]->setWindowTitle("从机 " + QString::number(mPortAddr,10)
                                                    + "-" + QString::number(mRegister,16).toUpper());
            }
        });
    }else{

        //接收未完成丢弃数据
        delete reply;
        return ;
    }

    connect(reply, &QModbusReply::errorOccurred, this, [=]() {


        if(reply->errorString().contains("timeout")){

            error->setText("读取超时(TimeOut)");
            m_table[tableCount]->setWindowTitle("读取超时(TimeOut)");
        }
        else if(reply->errorString().contains("Exception")){

            error->setText("读取异常(Exception)");
            m_table[tableCount]->setWindowTitle("读取异常(Exception)");

        }

    });


}

//接收完成后写入表格
void  MainWindow::onReadReady(int size)
{
    auto reply = qobject_cast<QModbusReply *>(sender());

    if (!reply)
        return;

    static int count;

    qint16 signedVal;
    int UnsignedVal;

    QString Val;

    for (int i = 0;i < size; i++) {

        //无符号显示
        if(!ui->actionUnsigned->isChecked())
        {
            signedVal = reply->result().value(i);
            Val = QString::number(signedVal);
        }
        else
        {
            UnsignedVal = reply->result().value(i);
            Val = QString::number(UnsignedVal);
        }

        if(m_table[count]->item(i+1,2) == nullptr)
        {
            m_table[count]->setItem(i+1,2,new QTableWidgetItem(Val));       //在第count个表中写入数据 i+1 因为0行是从机地址
        }else
            m_table[count]->item(i+1,2)->setText(Val);

    }

    //处理下一个表
    count++;

    if(count >= m_table.size())
    {
        count = 0;
        reply->deleteLater();
    }

    //判断是否再次读取
    isAutoRead();
}

//写按钮
void MainWindow::on_action_1_write_triggered()
{
    WriteDialog(0, 0,false);      //写入按钮默认值    寄存器地址和数据

}

//写实现
void MainWindow::WriteTable(int id,int start, int val)
{
    qDebug() << id <<start<<val;
    QModbusDataUnit writeUnit = QModbusDataUnit(QModbusDataUnit::HoldingRegisters, start, 1);   //1为修改个数

    writeUnit.setValue(0,val);      //单个修改所以第一个值为0
    modbusDevice->sendWriteRequest(writeUnit,id);
    //    if (auto reply = modbusDevice->sendWriteRequest(writeUnit,id)) {
    //        connect(reply, &QModbusReply::finished, this, [reply]() {

    //            if (!reply->isFinished()) {

    //                        reply->deleteLater();

    //                    }
    //        });


    //    }else
    //        reply->deleteLater();
}

//写入の对话框
void MainWindow::WriteDialog(int addr, int val, bool doubleCliked)
{
    if(modbusDevice->state() == QModbusDevice::UnconnectedState){

        QMessageBox::information(this,"提示","设备未连接");

        return ;
    }
    QDialog dialog(this,Qt::WindowCloseButtonHint);
    dialog.setWindowTitle("写入");
    QFormLayout form(&dialog);

    QSpinBox *spinbox1 = new QSpinBox(&dialog);
    spinbox1->setMaximum(0xffff);
    spinbox1->setDisplayIntegerBase(16);        //hex
    spinbox1->setValue(addr);

    if(doubleCliked){        //如果是双击单元格写入隐藏寄存器地址
        spinbox1->close();
        form.addRow(spinbox1);
    }else{
        QString addrLable = QString("寄存器地址(hex): ");
        form.addRow(addrLable,spinbox1);
    }


    QString value2 = QString("值: ");
    QSpinBox *spinbox2 = new QSpinBox(&dialog);
    spinbox2->setMaximum(65535);
    spinbox2->setMinimum(-32768);
    spinbox2->setValue(val);
    form.addRow(value2, spinbox2);
    // Add Cancel and OK button
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,Qt::Horizontal, &dialog);
    form.addRow(&buttonBox);
    QObject::connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    QObject::connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    if (dialog.exec() == QDialog::Accepted) {

        QWidget *widget = ui->mdiArea->activeSubWindow()->widget();
        QTableWidget *Twidget = qobject_cast<QTableWidget *>(widget);
        int id = 0;
        id = Twidget->item(0,1)->text().toInt();        //获取从机id
        WriteTable(id,spinbox1->value(), spinbox2->value());

    }

    //#if IMFORMATION_DEBUG
    //    qDebug() << spinbox1->value() << spinbox2->value();
    //#endif
}

//点击导入动作
void MainWindow::on_action_in_triggered()
{
    //单文件导入
    //    QString fileName = QFileDialog::getOpenFileName(this,"导入协议",
    //                                                    "","协议文件(*.csv);;所有文件(*.*)");
    //获取文件路径  string        多文件导入
    QStringList fileNameList = QFileDialog::getOpenFileNames(this,"导入协议",
                                                             "","协议文件(*.csv);;所有文件(*.*)");
    loadFiles(fileNameList);
}

//导入内容到表
bool MainWindow::loadFile(QString fileName)
{
    if(fileName != nullptr){
        QFile file(fileName);       //选择路径

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QMessageBox::warning(this,"提示","文件打开失败！");
            return false;
        }else{
            QTextStream read(&file);
            QStringList LineList;
            int countRow = 0;

            while(!read.atEnd()){

                QString Line = read.readLine();    //整行读取                       "这是一行,逗号分割"
                LineList = Line.split(",",QString::KeepEmptyParts);             //分割后-> LineList[0] "这是一行" LineList[1]"逗号分割"

                //读到一行就加表一行
                tableWidget->setRowCount(countRow + 1);

                for(int i = 0; i < LineList.size(); i++){

                    //第二列放数据，跳过
                    if(i == 2)
                        tableWidget->setItem(countRow,i+1,new QTableWidgetItem(LineList[i]));
                    else
                        tableWidget->setItem(countRow,i,new QTableWidgetItem(LineList[i]));
                }
                //统计行数
                countRow++;
            }
        }

        file.close();
        return true;
    }else
        return false;
}

//导入文件
void MainWindow::loadFiles(QList<QString> fileNameList)
{
    for (int i = 0; i < fileNameList.size(); i++){


        tableWidget = new QTableWidget(this);

        //添加表头
        tableWidget->setColumnCount(4);
        QStringList head;
        head<<"含义"<<"地址(Hex)"<<"数据"<<"备注";
        tableWidget->setHorizontalHeaderLabels(head);

        //获取文件内容
        if(loadFile(fileNameList[i]))

        {
            QString mPortAddr = QString::number(tableWidget->item(0,1)->text().toInt());
            QString mRegister = QString::number(tableWidget->item(1,1)->text().toInt(nullptr,16),16);

            //设置别名
            tableWidget->setObjectName("Table" + mPortAddr + mRegister);

            //            qDebug() << tableWidget->objectName();
            ui->mdiArea->addSubWindow(tableWidget,Qt::WindowMinMaxButtonsHint | Qt::WindowTitleHint);     //添加到subwindow，用于显示表格

            m_table.append(tableWidget);                //添加到列表m_table，用于操作表格

            //双击单元格の信号
            connect(tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(on_DoubleClick(int,int)));
            //回车
            connect(tableWidget,SIGNAL(cellActivated(int,int)),this,SLOT(on_DoubleClick(int,int)));

            //            connect(tableWidget,&QTableWidget::cellChanged,this,&MainWindow::getChangeCell);

            //设置窗口名
            QString myName = "从机 " + mPortAddr + "：" + mRegister;

            tableWidget->setWindowTitle(myName.toUpper());

            //设置右击菜单
            tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::onShowMenu);

            tableWidget->show();

        }
    }

}

//获取路径实现软件打开就导入
void MainWindow::auto_getFile()
{
    QString path = QCoreApplication::applicationDirPath()+ "/AUTOREAD";      //获取当前路径

    QDir fileDir(path);
    //指定后缀文件
    QStringList list = fileDir.entryList(QStringList() << "*.csv");
    QStringList fileNameList;

    foreach(const QString &name,list){
        fileNameList.append(path+"/"+name);
    }
    loadFiles(fileNameList);
    //    qDebug() <<fileNameList;


}

//读配置文件
void MainWindow::readConfig()
{
    QString path = QCoreApplication::applicationDirPath();      //获取当前路径
    QString iniPath = path + "/Config.ini";
    QSettings *config = new QSettings(iniPath, QSettings::IniFormat);

    QList<QString> allKeys = config->allKeys();

    foreach(QString key,allKeys){


        QString objName = config->value(key).toString();

        /*
            例：objName = 11Table12250
            意思是 1行1列对象名为 Table12250
            长度是12   长度为13或更高时说明行数是十位数或百位数
        */
        //当行为个位数时读取 objName 的第一个数字为行
        if(objName.count() == FLAG_SIZE_ONE )
            originalData(objName.mid(0,1).toInt(), objName.mid(2));

        //当行为十位数时读取 objName 的前两个数字为行
        else if(objName.count() == FLAG_SIZE_TEN )
            originalData(objName.mid(0,2).toInt(), objName.mid(3));

        //当行为百位数时读取 objName 的前三个数字为行
        else if(objName.count() == FLAG_SIZE_HUN )
            originalData(objName.mid(0,3).toInt(), objName.mid(4));

        qDebug() << key <<objName;
    }


}

//导出
void MainWindow::on_action_out_triggered()
{
    if(modbusDevice->state() == QModbusDevice::UnconnectedState){
        QMessageBox::information(this,"提示","设备未连接，数据为空！");
        return ;
    }
    outDialog *outSetting = new outDialog(this);      //记录定时和上线
    connect(outSetting,SIGNAL(IsOk(int, int)),this,SLOT(on_IsOk(int, int)));        //导出设置OK
    outSetting->exec();

}


//导出记录收到OK后执行
void MainWindow::on_IsOk(int time, int count)
{
    if(ui->mdiArea->subWindowList().count() > 0)
    {
        mOutTable = new FormOutTable();       //每次点击ok生成新对象

        QList<QTableWidgetItem*> sourceList;        //含义的列表
        QList<QTableWidgetItem*> dataList;          //数据列表

        QWidget *widget = ui->mdiArea->activeSubWindow()->widget();      //获取当前激活的控件
        Twidget = qobject_cast<QTableWidget *>(widget);


        //count+3 新建记录表的行数，设置数据上限的值，+3表示表头的3行；
        //因为记录表是横向的所以Twidget->rowCount()-1，原表的行数减去从机地址行
        mOutTable->newTable(count+3,Twidget->rowCount()-1,Twidget->item(0,1),Twidget->item(1,1));

        for (int i = 1; i < Twidget->rowCount(); i++) {     //获取原表
            sourceList.append(Twidget->item(i,0));
            dataList.append(Twidget->item(i,2));
        }

        mOutTable->InitTable(sourceList,dataList);    //显示原表数据到记录表
        mOutTable->show();

        connect(mOutTable,SIGNAL(startUpdata()),this,SLOT(on_startUpdata()));        //连接更新数据信号
        connect(mOutTable,SIGNAL(stopRecords()),this,SLOT(on_action_stop_triggered()));        //连接更新数据信号
        mOutTable->gogogo(true,time);        //定时开始

        ui->action_stop->setEnabled(true);
        ui->action_out->setEnabled(false);

    }else{
        QMessageBox::critical(this,"错误","协议未导入，当前没有可以记录的数据");

        //qDebug() << ui->mdiArea->subWindowList().count() << "当前没有可以记录的表";
        return ;

    }

}

//开始导出
void MainWindow::on_startUpdata()
{
    QList<QTableWidgetItem*> dataList;

    for (int i = 1; i < Twidget->rowCount(); i++) {     //获取更新数据
        dataList.append(Twidget->item(i,2));
    }

    mOutTable->updataTable(dataList);    //更新数据
}

//停止导出
void MainWindow::on_action_stop_triggered()
{
    ui->action_stop->setEnabled(false);
    ui->action_out->setEnabled(true);
    mOutTable->gogogo(false,0);
}

//右击菜单
void MainWindow::onShowMenu(QPoint pos)
{
    QWidget *widget = ui->mdiArea->activeSubWindow()->widget();
    QTableWidget *Twidget = qobject_cast<QTableWidget *>(widget);

    //确保当前右击的单元格不为空
    if(Twidget->itemAt(pos) == nullptr || Twidget->itemAt(pos)->row() == 0 )

        return ;

    //获取被右击的item
    QTableWidgetItem* getItem = Twidget->itemAt(pos);

    //创建菜单
    QMenu* menu = new QMenu(this);
    //创建action
    QAction* table = new QAction("提取到数据表", this);
    QAction* chart = new QAction("查看曲线图", this);
    //将action放入菜单中
    menu->addAction(table);
    menu->addAction(chart);
    //将菜单显示到鼠标所在位置
    menu->popup(Twidget->viewport()->mapToGlobal(pos));

    //当前行数
    int currRow = getItem->row();

    connect(table,&QAction::triggered,this,[=](){

        originalData(currRow,Twidget->objectName());
        mGroup->show();
    });

    connect(chart,&QAction::triggered,this,[=](){

        mChart->show();
    });

    //    chart->setDisabled(true);

    //单元格被修改时发出信号
    connect(Twidget,&QTableWidget::cellChanged,this,[=](int row,int col){

        getChangeCell(row,col,Twidget->objectName());
    });

}

//关闭询问
void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton result=QMessageBox::question(this, "确认", "确定要退出吗？",QMessageBox::Yes|QMessageBox::No);

    if (result==QMessageBox::Yes){
        event->accept();
        mGroup->close();
        //        mOutTable->close();
    }
    else
        event->ignore();
}

//隐藏所有窗口
void MainWindow::on_action_hide_triggered()
{
    QList<QMdiSubWindow*> subWindows = ui->mdiArea->subWindowList();

    for(int i = 0; i < subWindows.count(); i++){

        subWindows[i]->hide();
    }
}

//打开所有窗口
void MainWindow::on_actionShowAllDock_triggered()
{
    if(ui->mdiArea->subWindowList().count() > 0){
        QList<QMdiSubWindow*> subWindows = ui->mdiArea->subWindowList();

        for(int i = 0; i < subWindows.count(); i++){

            subWindows[i]->show();
        }
    }
}

//整理窗口
void MainWindow::on_action_tidy_triggered()
{
    ui->mdiArea->cascadeSubWindows();
}

//关闭所有窗口
void MainWindow::on_actionClearDock_triggered()
{
    QMessageBox::StandardButton result=QMessageBox::information(this, "提示", "该操作只能在断开连接时操作，将会清空所有窗口，并需要重新导入协议！"
                                                                ,QMessageBox::Yes|QMessageBox::No);

    if (result==QMessageBox::Yes){
        //         QList<QMdiSubWindow*> subWindows = ui->mdiArea->subWindowList();
        //         qDebug() << ui->mdiArea->subWindowList() << m_table;
        //                         for(int i = 0; i < subWindows.count(); i++){

        //                             subWindows[i]->close();
        //                     }
        ui->mdiArea->closeAllSubWindows();
        ui->mdiArea->subWindowList().clear();
        m_table.clear();
    }
    else return ;

}

//并列显示
void MainWindow::on_action_tab_triggered()
{
    if(ui->action_tab->isChecked()){
        ui->mdiArea->setViewMode(QMdiArea::TabbedView);
    }else
        ui->mdiArea->setViewMode(QMdiArea::SubWindowView);
}

//隐藏异常提示隐藏异常提示
void MainWindow::on_actionHideEorro_triggered()
{
    if(!ui->actionHideEorro->isChecked())
    {
        error = new QLabel;
        error->setStyleSheet("QLabel { font-size: 10pt; color: red; }");
        ui->toolBar_2->addWidget(error);
    }
    else
    {
        //error->setAttribute(Qt::WA_TranslucentBackground,false);
        ui->toolBar_2->clear();
    }
}

//单元格发生改变时传递值
void MainWindow::getChangeCell(int row, int col,QString objName)
{
    //找到被更改的表对象、行列、目标值
    QTableWidget *getTable = this->findChild<QTableWidget*>(objName);
    if(getTable->item(row,col) == nullptr)
        return ;
    int changeData = getTable->item(row,col)->text().toInt();

    QString flagT =  QString::number(row)+QString::number(2)+objName;
    qDebug() <<flagT;
    mGroup->findChange(flagT,changeData);

    qDebug() <<row<<col<<objName<<changeData;
}

//copy原表数据文本到新表
void MainWindow::originalData(int row, QString objName)
{
    QTableWidget *getTable = this->findChild<QTableWidget*>(objName);

    //单元格被修改时发出信号
    connect(getTable,&QTableWidget::cellChanged,this,[=](int row,int col){

        getChangeCell(row,col,getTable->objectName());
    });
    int data = 0;
    //寄存器首地址
    int addrLocation = getTable->item(1,ADDR_COL)->text().toInt(nullptr,MY_HEX);

    //从机地址
    int id = getTable->item(0,ADDR_COL)->text().toInt();

    //当前点击的寄存器地址
    int addr = addrLocation - 1 + row;
    QString addr_strHex = QString::number(addr,MY_HEX).toUpper();

    //数据
    if(getTable->item(row,DATA_COL) != nullptr)
        data = getTable->item(row,DATA_COL)->text().toInt();

    //含义
    QString name = getTable->item(row,MEAN_COL)->text();

    //给新建行加个标志  ：行列加对象名
    QString flagT =  QString::number(row)+QString::number(DATA_COL)+objName;
    qDebug() <<flagT;

    mGroup->insert(id,name,addr_strHex,data,flagT);
    //    qDebug() <<"ID:"<<id<<"含义:"<<name<<"地址:"<<addr_strHex<<"数据:"<<data;
}

//数据表
void MainWindow::on_actionGroup_triggered()
{

    mGroup->show();
    mGroup->move(25,30);
}
#define TEST1   0
#define TEST2   0
#define TEST3   0
#define TEST4   0
#define TEST5   0
//测试专用の无敌牛逼按钮PlusMAX
void MainWindow::on_action_test_triggered()
{
#if TEST1
    QTableWidget *table = ui->mdiArea->findChild<QTableWidget*>("Table12100");

    if(table != nullptr)
    {
        table->setItem(0,0,new QTableWidgetItem("成功"));
        qDebug() << table->item(1,0)->text();
    }
#endif

#if TEST2
    chart = new FormChart();
    chart->show();

#endif

#if TEST3
    group = new FormGroup;
    group->show();

#endif

#if TEST4


#endif

#if TEST5


#endif

}

