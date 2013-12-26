#include "widget.h"
#include "ui_widget.h"

Widget::~Widget()
{
    delete ui;
}

void Widget::onToLog(QString str)
{
    ui->textEdit->append(str);
}

Widget::Widget(QWidget *parent) :QWidget(parent),ui(new Ui::Widget)
{
    ui->setupUi(this);

    //настриваем интерфейс
    connect(this,SIGNAL(toLog(QString)),this,SLOT(onToLog(QString)));
    ui->tabWidget->setTabText(0,QString::fromUtf8("log"));
    ui->pushButton_3->setEnabled(false);
}


//create test shapefile
void Widget::on_pushButton_clicked()
{

}


//load test shapefile
void Widget::on_pushButton_3_clicked()
{

}


//create test postgis db
void Widget::on_pushButton_2_clicked()
{

}



void Widget::on_pushButton_4_clicked()
{
    //Путь: \ogr\ogrsf_frmts\generic\ogrsfdriverregistrar.cpp

    OGRRegisterAll();

    OGRSFDriver *dr = new OGRGnmDriver();

    dr->CreateDataSource("test",NULL);

}









