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


//читаем с ДатаСорса (тестим)
void Widget::on_pushButton_clicked()
{
    const char *str;

    //далее согласно OGR API Tutorial, чтение из слоя

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
    poLayer = poDS->GetLayerByName(str);
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


//добавляем новый слой с геометрией + добавляем в него объекты
void Widget::on_pushButton_2_clicked()
{
    OGRRegisterAll();

    OGRDataSource *poDS;
    const char *str;
    str = "..\\..\\temp";
    //TEMP: аналогично открытию
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str,TRUE);
    if( poDS == NULL )
    {
        emit toLog(QString("[error] Can not open data source ") + QString(str));
        return;
    }

    //далее согласно OGR API Tutorial, создание слоя и запись объектов

    //создаём слой
    OGRLayer *poLayer;
    poLayer = poDS->CreateLayer("test_point_layer", NULL, wkbPoint, NULL);
    if( poLayer == NULL )
    {
        emit toLog(QString("[error] Can not create layer"));
        return;
    }

    //добавляем некий атрибут (к уже имеющимся стандартным атрибутам)
    OGRFieldDefn oField("test_field", OFTString);
    oField.SetWidth(32);
    if( poLayer->CreateField( &oField ) != OGRERR_NONE )
    {
        emit toLog(QString("[error] Can not create field"));
        return;
    }

    //добавляем объекты с геометрией
    for (int i=0; i<3; i++)
    {
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
        poFeature->SetField("test_field", "test string");
        poFeature->SetField("is_blocked", 99);
        poFeature->SetField("direction", 55);
        OGRPoint pt;
        pt.setX(i * 5.0);
        pt.setY(0.0);
        poFeature->SetGeometry(&pt);
        if(poLayer->CreateFeature(poFeature) != OGRERR_NONE)
        {
            emit toLog(QString("[error] Can not create geometry feature"));
            return;
        }

         OGRFeature::DestroyFeature(poFeature);
    }
    poDS->SyncToDisk();
    OGRDataSource::DestroyDataSource( poDS );
    emit toLog(QString("[info] Features added successfully to the new layer"));

}









