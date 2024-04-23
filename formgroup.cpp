#include "formgroup.h"
#include "ui_formgroup.h"

FormGroup::FormGroup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormGroup)
{
    ui->setupUi(this);
    this->setWindowTitle("数据表");
    ui->tableWidget->setColumnCount(5);

    QStringList head;
    head<<"ID"<<"含义"<<"地址(Hex)"<<"数据"<<"标志";
    ui->tableWidget->setHorizontalHeaderLabels(head);

    ui->tableWidget->hideColumn(4);

    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    //设置第1列自适应
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    //显示交替颜色
    ui->tableWidget->setAlternatingRowColors(true);
    //设置第0列列宽
    ui->tableWidget->setColumnWidth(0,30);ui->tableWidget->setColumnWidth(2,70);ui->tableWidget->setColumnWidth(1,200);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
    //单元格不可编辑
    ui->tableWidget->setEditTriggers(QTableWidget::NoEditTriggers);

    //设置右击菜单
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &FormGroup::on_menu);

    //修改值
    connect(ui->tableWidget,&QTableWidget::cellDoubleClicked,this,&FormGroup::doubleClicked);

    ui->btnDelete->hide();
    ui->btnShowChart->hide();

}

FormGroup::~FormGroup()
{
    delete ui;
}
//#include <QDebug>

#define ID      0
#define NAME    1
#define ADDR    2
#define DATA    3
#define FLAG    4
//插入新行
void FormGroup::insert(int id, QString name, QString addr, int data,QString flag)
{
    ui->tableWidget->setRowCount(row + 1);
    ui->tableWidget->setItem(row,ID,new QTableWidgetItem(QString::number(id)));
    ui->tableWidget->setItem(row,NAME,new QTableWidgetItem(name));
    ui->tableWidget->setItem(row,ADDR,new QTableWidgetItem("0x"+addr));
    ui->tableWidget->setItem(row,DATA,new QTableWidgetItem(QString::number(data)));
    ui->tableWidget->setItem(row,FLAG,new QTableWidgetItem(flag));
    row++;
}

//数据变化时更新
void FormGroup::findChange(QString loaction, int data)
{
    for (int i = 0;i < ui->tableWidget->rowCount();i++) {

        //遍历标记和被更改数值的标记是否一样
        if(ui->tableWidget->item(i,FLAG)->text() == loaction)
            ui->tableWidget->setItem(i,DATA,new QTableWidgetItem(QString::number(data)));
    }

}

#include <QDebug>
//保存到配置文件
void FormGroup::setConfig()
{
    QString path = QCoreApplication::applicationDirPath();      //获取当前路径
    QString iniPath = path + "/Config.ini";
    config = new QSettings(iniPath, QSettings::IniFormat);

    QString key,val;

    config->beginGroup("group");

    //清除上一次的数据
    config->clear();

    //重新写入
    for (int i =0; i < ui->tableWidget->rowCount(); i++) {

        key = "key" + QString::number(i);
        val = ui->tableWidget->item(i,4)->text();
        config->setValue(key, val);
    }


    config->endGroup();

//    qDebug() << "写入配置文件";
}


void FormGroup::closeEvent(QCloseEvent *event)
{
    event->accept();
    setConfig();

}


//删除全部
void FormGroup::on_btnDeleteAll_clicked()
{
    QMessageBox::StandardButton result=QMessageBox::question(this, "确认", "确定删除全部",QMessageBox::Yes|QMessageBox::No);

    if(result == QMessageBox::No)
        return ;

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    row = 0;
}

//删除当前行
void FormGroup::on_btnDelete_clicked()
{
    QList<QTableWidgetItem*> list=ui->tableWidget->selectedItems();
    QList<int> rowList;

    if (list.isEmpty()) {
         QMessageBox::information(nullptr, "信息", "没有选中任何行！");
         return;
     }

    //获取选中的行
    for (int i = 0; i < list.count(); i++) {

        rowList.append(list[i]->row());
    }

    //去重复行
    rowList = removeDuplicates(rowList);

    QList<int> rowList_rever;

    //排序并翻转，从最后的行开始删除
    rowList_rever = reverseList(rowList);

    for (int i = 0; i < rowList_rever.count(); i++) {

        ui->tableWidget->removeRow(rowList_rever[i]);
        row--;
    }
}

//去除重复值
QList<int> FormGroup::removeDuplicates(const QList<int> &list)
{
    QSet<int> set;
    QList<int> result;

    // 将QList中的元素添加到QSet中，自动去除重复值
    for (int value : list) {
        set.insert(value);
    }

    // 将QSet中的元素转换回QList
    result = set.values();

    return result;
}

//翻转列表
QList<int> FormGroup::reverseList(QList<int> list)
{
    QList<int> temp;
    int count = list.count();

    //排序
    std::sort(list.begin(), list.end());

    while (count) {

        temp.append(list[count-1]);
        count--;
    }

    return temp;
}

void FormGroup::on_btnShowChart_clicked()
{

}

//右键菜单
void FormGroup::on_menu(QPoint pos)
{
    QMenu* menu = new QMenu(this);
    //创建action
    QAction* del = new QAction("删除", this);
    QAction* chart = new QAction("查看曲线图", this);

    menu->addAction(del);
    menu->addAction(chart);
    connect(del, &QAction::triggered, this, &FormGroup::on_btnDelete_clicked);
    connect(chart, &QAction::triggered, this, &FormGroup::on_btnShowChart_clicked);
    menu->popup(ui->tableWidget->viewport()->mapToGlobal(pos));
}

//双击触发修改值弹窗
void FormGroup::doubleClicked(int row, int col)
{
    int id = 0,
        data = 0,
        addr = 0;

    if(col != DATA)
        return ;

    QString id_str = ui->tableWidget->item(row,ID)->text();
    QString addr_str = ui->tableWidget->item(row,ADDR)->text();
    QString name_str = ui->tableWidget->item(row,NAME)->text();
    int currData = ui->tableWidget->item(row,col)->text().toInt();

    data = QInputDialog::getInt(this,"写入",
                                "#"+id_str+name_str+":",
                                currData);

//    if(ui->tableWidget->item(row,ID) != nullptr)
        id = ui->tableWidget->item(row,ID)->text().toInt();

//    if(ui->tableWidget->item(row,ADDR) != nullptr)
        addr = ui->tableWidget->item(row,ADDR)->text().toInt(nullptr,16);

//    if(ui->tableWidget->item(row,DATA) != nullptr)
//        data = ui->tableWidget->item(row,DATA)->text().toInt();

//    qDebug() << id <<addr<<data;
    emit groupDataChange(id,addr,data);
}
