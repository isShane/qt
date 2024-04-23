#include "outdialog.h"
#include "ui_outdialog.h"

outDialog::outDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::outDialog)
{
    ui->setupUi(this);
    this->setFixedSize(300,150);
    this->setWindowTitle("导出记录");
     setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    //connect(ui->pushButton,&QPushButton::clicked,mOutTable,&QDialog::show);
    connect(ui->pushButton,&QPushButton::clicked,[this](){
        oArg.timer = ui->spinTimes->value();
        oArg.countLog = ui->spinCounts->value();
        emit IsOk(ui->spinTimes->value(), ui->spinCounts->value());
        close();
    });
}

outDialog::~outDialog()
{
    delete ui;
}

outDialog::outArg outDialog::getParam() const
{
    return oArg;
}
