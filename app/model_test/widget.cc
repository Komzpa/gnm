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
    /*
    ui->tabWidget->setTabText(1,QString::fromUtf8("network_meta"));
    ui->tabWidget->setTabText(2,QString::fromUtf8("network_domain"));
    ui->tabWidget->setTabText(3,QString::fromUtf8("network_graph"));
    ui->tabWidget->setTabText(4,QString::fromUtf8("Network Objects"));
    ui->tabWidget->setTabText(5,QString::fromUtf8("GIS"));
    ui->tab->setEnabled(false);
    ui->tab_2->setEnabled(false);
    ui->tab_3->setEnabled(false);
    ui->tab_4->setEnabled(false);
    ui->tab_5->setEnabled(false);
    //ui->tabWidget->setTabText(4,QString::number(sizeof(long)));
    */

    /*
    //создаём пустую сеть
    net = NtwNetworkManager::createNetwork("ESRI Shapefile");

    //выводим её метаданные
    for (int i=0; i<net->getAttribCount(); i++)
    {
        QString str = QString::fromStdString(net->getAttrib(i).first)
                      + " = "
                      + QString::fromStdString(net->getAttrib(i).second);
        ui->listWidget->addItem(str);
    }
*/

}


//create test shapefile
void Widget::on_pushButton_clicked()
{
    //создаём пустую сеть, но со слоями по умолчанию
    emit toLog("[app] Creating ESRI Shapefile data source...");
    const char *str = "..\\..\\temp\\test.shp";
    network = GnmNetworkManager::createVoidNetwork(str, "ESRI Shapefile");
    if (network == NULL)
    {
        emit toLog("[app] Creating " + QString(str) + " failed!");
        return;
    }
    else
    {
        emit toLog("[app] Creating " + QString(str) + " was successfull.");
    }

//TEST----------------------------------------
    OGRPoint pt1;
    pt1.setX(0.0);
    pt1.setY(0.0);
    network->addFeature(1,&pt1);

    OGRPoint pt2;
    pt2.setX(5.0);
    pt2.setY(5.0);
    network->addFeature(1,&pt2);

    OGRPoint pt3;
    pt3.setX(10.0);
    pt3.setY(0.0);
    network->addFeature(1,&pt3);
    network->setFeatureAttribute(1,1,0,55);

    OGRLineString l1;
    l1.setNumPoints(3);
    l1.setPoint(0,&pt1);
    l1.setPoint(1,&pt2);
    l1.setPoint(2,&pt3);
    network->addFeature(2,&l1);

    network->addLayer("Gates",1);
    network->addField(3,"gate_param",OFTInteger);

    OGRPoint pt4;
    pt4.setX(5.0);
    pt4.setY(-5.0);
    network->addFeature(3,&pt4);
    network->setFeatureAttribute(3,0,4,123);

    network->syncDataToDisk();
//------------------------------------------------
}


//load test shapefile
void Widget::on_pushButton_3_clicked()
{

}


//create test postgis db
void Widget::on_pushButton_2_clicked()
{
    emit toLog("[app] Creating PostGIS data source...");
    const char *str = "dbname='test_postgis' host='localhost' port='5432' user='postgres' password='zsxcfv'";
    network = GnmNetworkManager::createVoidNetwork(str, "PostgreSQL/PostGIS");
    if (network == NULL)
        emit toLog("[app] Creating " + QString(str) + " failed!");
    else
        emit toLog("[app] Creating " + QString(str) + " was successfull.");
}
