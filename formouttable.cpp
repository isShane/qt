#include "formouttable.h"
#include "ui_formouttable.h"

FormOutTable::FormOutTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormOutTable)
{
    ui->setupUi(this);
       this->setWindowTitle("记录表");
 setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    mExortTimer = new QTimer;
    connect(mExortTimer,&QTimer::timeout,this,&FormOutTable::on_mExortTimer);
}

FormOutTable::~FormOutTable()
{
    delete ui;
}



void FormOutTable::newTable(int row, int col,QTableWidgetItem *id,QTableWidgetItem *addr)
{
    ui->tableWidget->setRowCount(row);
    ui->tableWidget->setColumnCount(col);
    QTableWidgetItem *newid =new QTableWidgetItem(*id);
    QTableWidgetItem *newaddr =new QTableWidgetItem(*addr);
    ui->tableWidget->setItem(0,0,newid);
    ui->tableWidget->setItem(1,0,newaddr);
    limitCount = row - 3;
}

void FormOutTable::InitTable(QList<QTableWidgetItem *> sourceList, QList<QTableWidgetItem *> dataList)
{
//    QList<QTableWidgetItem *> newList;
//    QList<QTableWidgetItem *> newDataList;

    for (int i = 0; i < sourceList.size(); i++) {

        QTableWidgetItem *newItem = new QTableWidgetItem(*(sourceList[i]));
        ui->tableWidget->setItem(2,i,newItem);
        //newList.append(newItem);
    }
    for (int i = 0; i < dataList.size(); i++) {

        QTableWidgetItem *newDataItem = new QTableWidgetItem(*(dataList[i]));
        ui->tableWidget->setItem(3,i,newDataItem);

    }

}

void FormOutTable::on_saveBtn_clicked()
{
    QString outFileName;
    mExortTimer->stop();

//    outFileName = QFileDialog::getSaveFileName(this,tr("导出记录"),"./导出记录.xlsx",tr("Excel(*.xlsx)"));      //设置导出路径
//    outFileName = QFileDialog::getSaveFileName(this,"导出记录","./导出记录","Excel表格(*.xlsx);;CSV表格(*.csv)");      //设置导出路径
      outFileName = QFileDialog::getSaveFileName(this,"导出记录","./导出记录","CSV表格(*.csv)");      //设置导出路径

    if(outFileName.isEmpty()){
            ;
    }else{
        if(outFileName.back() == "v"){
            saveCSV(outFileName);
            //qDebug() << "csv";
        }
        else{
            saveExcel(outFileName);
            qDebug() << "excel";
        }

        isSave = true;
    }
    mExortTimer->start();
}

void FormOutTable::saveCSV(const QString &fileName)
{
   QFile file(fileName);
   if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
   {
   qDebug() << "Open file failed!";
   QMessageBox::warning(this, "警告", "保存失败，检查文件是否被占用或权限不足！");

   return;
   }
   QTextStream out(&file);
   QString str;
   //获取表格内容
   int row = ui->tableWidget->rowCount();//表格总行数
   int col = ui->tableWidget->columnCount();
   for(int i = 0; i < row; i ++)
   {
       for(int j = 0; j < col; j++)
       {
           if(ui->tableWidget->item(i, j) != nullptr)
                str = ui->tableWidget->item(i, j)->text();
           else
               str = nullptr;
           out << str << ",";// 写入文件
       }
       out << "\n";
   }


   file.close();
}

void FormOutTable::saveExcel(const QString &fileName)
{
//    QAxObject* excel = new QAxObject("Excel.Application");
//    QAxObject* workbooks = excel->querySubObject("Workbooks");
//    QAxObject* workbook = workbooks->querySubObject("Add");
//    QAxObject* sheet = workbook->querySubObject("ActiveWorkBook");

//    int rowCount = ui->tableWidget->rowCount();
//    int columnCount = ui->tableWidget->columnCount();

//    for (int row = 0; row < rowCount; ++row) {
//        for (int column = 0; column < columnCount; ++column) {
//            QTableWidgetItem *item = ui->tableWidget->item(row, column);
//            if (item) {
//                QString value = item->text();
//                qDebug() << value;
//                QVariant var = QVariant(value);
//                sheet->dynamicCall("SetCellValue(int, int, const QVariant&)", row + 1, column + 1, var);
//            }
//        }
//    }

//    workbook->dynamicCall("SaveAs(const QString&)", fileName);
//    excel->dynamicCall("Quit()");

//    delete excel;

}


void FormOutTable::updataTable( QList<QTableWidgetItem *> dataList)
{
    if(limitCount != 0){
        for (int i = 0; i < dataList.size(); i++) {

            QTableWidgetItem *newDataItem = new QTableWidgetItem(*(dataList[i]));
            ui->tableWidget->setItem(count,i,newDataItem);
    //        newDataList.append(newItem);
        }
    }else{
        mExortTimer->stop();
        QMessageBox::warning(this,"警告","数据记录已达到上限");
    }
    count++;

}

void FormOutTable::gogogo(bool go,int timer)
{

    if(go){
        mExortTimer->start(timer*1000);

    }else{
        mExortTimer->stop();
        count = START_VAL;
    }
}

void FormOutTable::autoSave()
{
    QString path = QCoreApplication::applicationDirPath();      //获取当前路径
    QString fileName = path + "/自动保存.csv";
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
    //qDebug() << "Open file failed!";
    //QMessageBox::warning(this, "警告", "保存失败，检查文件是否被占用或权限不足！");
    ui->label->setText("<font color=\"red\">自动保存失败，检查文件是否被占用或权限不足！</font>");
    return;
    }
    ui->label->setText("<font color=\"green\">已开启自动保存</font>");
    QTextStream out(&file);
    QString str;
    //获取表格内容
    int row = ui->tableWidget->rowCount();//表格总行数
    int col = ui->tableWidget->columnCount();
    for(int i = 0; i < row; i ++)
    {
        for(int j = 0; j < col; j++)
        {
            if(ui->tableWidget->item(i, j) != nullptr)
                 str = ui->tableWidget->item(i, j)->text();
            else
                str = nullptr;
            out << str << ",";// 写入文件
        }
        out << "\n";
    }


    file.close();

}

void FormOutTable::on_mExortTimer()
{
    limitCount--;
    qDebug() << limitCount;
    if(ui->checkBox->isChecked())
    autoSave();
    else
        ui->label->setText("<font color=\"red\">自动保存未开启</font>");
        emit startUpdata();     //到点崔更

}

void FormOutTable::closeEvent(QCloseEvent *event)
{
    if(!isSave){
        QMessageBox::StandardButton result=QMessageBox::warning(this, "保存", "数据未手动保存，是否保存记录？",QMessageBox::Yes|QMessageBox::No|QMessageBox::Cancel);
    
         if (result==QMessageBox::Yes){

             on_saveBtn_clicked();

         }else if(result==QMessageBox::Cancel){
             event->ignore();
             return ;
         }
         count = START_VAL;
         isSave = false;
         mExortTimer->stop();
         emit stopRecords();
         event->accept();
    }else{

        count = START_VAL;
        isSave = false;
        mExortTimer->stop();
        emit stopRecords();
        event->accept();
    }

}

void FormOutTable::on_pushButton_clicked()
{
    if(ui->pushButton->text() == "暂停记录"){
    mExortTimer->stop();
     //emit stopRecords();
    ui->pushButton->setText("继续记录");
    }else{
        mExortTimer->start();
        ui->pushButton->setText("暂停记录");
    }
}
