#include "formtable.h"
#include "ui_formtable.h"

FormTable::FormTable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormTable)
{
    ui->setupUi(this);
    tableWidget = new QTableWidget(this);
//    tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

FormTable::~FormTable()
{
    delete ui;
}

#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
bool FormTable::loadFile(QString fileName)
{
    if(fileName != nullptr){
        QFile file(fileName);       //选择路径

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {

           return false;
        }else{
            QTextStream read(&file);
            QStringList LineList;
            int countRow = 0;
            while(!read.atEnd()){

                countRow++;
                QString Line = read.readLine();    //整行读取                       "这是一行,逗号分割"
                LineList = Line.split(",",QString::KeepEmptyParts);//分割后-> LineList[0] "这是一行" LineList[1]"逗号分割"

                tableWidget->setRowCount(countRow);

                  for(int i = 0; i < LineList.size(); i++){
                    if(i == 2)
                        tableWidget->setItem(countRow-1,i+1,new QTableWidgetItem(LineList[i]));
                    else
                        tableWidget->setItem(countRow-1,i,new QTableWidgetItem(LineList[i]));
            }
            }


            return true;
        }

        file.close();
    }else

        return false;
}
