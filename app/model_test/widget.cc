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


/* ------------------------------------------------------------------ */
/*        Создаём модель (Дата сорс) со слоями по умолчанию           */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_4_clicked()
{
    OGRRegisterAll(); //Путь: \ogr\ogrsf_frmts\generic\ogrsfdriverregistrar.cpp

    OGRSFDriver *dr = new OGRGnmDriver();

    OGRDataSource *ds = dr->CreateDataSource("..\\..\\temp",NULL); //указываем только директорию для шейпа

    if (ds == NULL)
    {
        emit toLog(QString("[error] Can not create data source"));
        return;
    }

    emit toLog(QString("[info] Data source has been successfully created"));
}


/* ------------------------------------------------------------------ */
/*                             Читаем модель                          */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_clicked()
{
    const char *str;

    //далее согласно OGR API Tutorial, чтение из слоя

    OGRRegisterAll();

    OGRDataSource *poDS;
    str = "..\\..\\temp";
    //poDS = OGRSFDriverRegistrar::Open(str, FALSE); //пока так сделать нельзя, драйвер не зареган
    //(вместо OGRSFDriverRegistrar) -------------
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str,FALSE);
    //-------------------------------------------
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
        emit toLog(QString(poFeature->GetFieldAsString(0)) + QString(" = ") + QString(poFeature->GetFieldAsString(1)));
    }

    OGRDataSource::DestroyDataSource( poDS );
}


/* ------------------------------------------------------------------ */
/*                          Пишем в модель                            */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_2_clicked()
{
    OGRRegisterAll();

    OGRDataSource *poDS;
    const char *str;
    str = "..\\..\\temp";
    //аналогично открытию
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
    emit toLog(QString("[info] test_point_layer has been successfully created"));



    //добавляем объекты с геометрией
    //если внутри кода кнопки on_pushButton_2_clicked, то работает
    for (int i=0; i<3; i++)
    {
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
        poFeature->SetField("test_field", "aaaaaa");
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
    emit toLog(QString("[info] Features added to the new layer successfully"));

    if (poDS->SyncToDisk() != OGRERR_NONE)
    {
        emit toLog(QString("[error] Failed to write to disk: "));
        return;
    }
    emit toLog(QString("[info] Features have been successfully written"));



    OGRDataSource::DestroyDataSource(poDS);
}



//добавляем в тестовый слой объекты
void Widget::on_pushButton_3_clicked()
{
    //если раскоментить следующий код (аналогичен предыдущей кнопке),
    //то объекты не добавятся в слой. Разобраться, что это за баг.

    /*
    OGRRegisterAll();

    OGRDataSource *poDS;
    const char *str;
    str = "..\\..\\temp";
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str,TRUE);
    if(poDS == NULL)
    {
        emit toLog(QString("[error] Can not open data source ") + QString(str));
        return;
    }

    OGRLayer *poLayer;
    poLayer = poDS->GetLayerByName("test_point_layer");
    if (poLayer == NULL)
    {
        emit toLog(QString("[error] Can not open layer (test_point_layer)"));
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
        emit toLog(QString("[info] Features added to the new layer successfully"));

        if (poDS->SyncToDisk() != OGRERR_NONE)
        {
            emit toLog(QString("[error] Failed to write to disk: "));
            return;
        }
        emit toLog(QString("[info] Features have been successfully written"));
        OGRDataSource::DestroyDataSource( poDS );
*/
}






