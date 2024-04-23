#include "formchart.h"
#include "ui_formchart.h"

FormChart::FormChart(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormChart)
{
    ui->setupUi(this);
}

FormChart::~FormChart()
{
    delete ui;
}
