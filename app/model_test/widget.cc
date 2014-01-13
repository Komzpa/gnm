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
}


//создаём Дата Сорс
void Widget::on_pushButton_4_clicked()
{
    OGRRegisterAll(); //Путь: \ogr\ogrsf_frmts\generic\ogrsfdriverregistrar.cpp

    OGRSFDriver *dr = new OGRGnmDriver();

    dr->CreateDataSource("..\\..\\temp",NULL); //указываем только директорию для шейпа
}


//открываем Дата Сорс (тестим)
void Widget::on_pushButton_clicked()
{
    const char *str;

    //далее согласно OGR API Tutorial

    OGRRegisterAll();

    OGRDataSource *poDS;
    str = "..\\..\\temp";
    //poDS = OGRSFDriverRegistrar::Open(str, FALSE); //пока так сделать нельзя, драйвер не зареган
    //TEMP (вместо OGRSFDriverRegistrar) ---------------------------------
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str,FALSE);
    //--------------------------------------------------------------------
    if( poDS == NULL )
    {
        emit toLog(QString("[error] Can not open data source ") + QString(str));
        return;
    }

    OGRLayer *poLayer;
    str = "network_meta";
    poLayer = poDS->GetLayerByName("network_meta");
    if( poLayer == NULL )
    {
        emit toLog(QString("[error] Can not open layer ") + QString(str));
        return;
    }

    //считываем метаданные сети (выведутся версия и система координат)
    OGRFeature *poFeature;
    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        emit toLog("[output] Network metadata: ");
        emit toLog(QString(poFeature->GetFieldAsString(0)) + QString(" ") + QString(poFeature->GetFieldAsString(1)));
    }
}













