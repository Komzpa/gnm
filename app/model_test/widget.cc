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

    // Build an interface.
    connect(this,SIGNAL(toLog(QString)),this,SLOT(onToLog(QString)));
    ui->tabWidget->setTabText(0,QString::fromUtf8("log"));
}


/* ------------------------------------------------------------------ */
/*                             Create network                         */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_4_clicked()
{
    OGRRegisterAll(); // ogr\ogrsf_frmts\generic\ogrsfdriverregistrar.cpp

    OGRSFDriver *dr = new OGRGnmDriver();

    // Selecting a directory and data source format.
    char* pszName = "..\\..\\temp";
    char** papszOptions = NULL;
    papszOptions = CSLAddNameValue(papszOptions, GNM_OPTION_DRIVER_NAME, "ESRI Shapefile");
    OGRDataSource *ds = dr->CreateDataSource(pszName, papszOptions);
    CSLDestroy(papszOptions);

    if (ds == NULL)
    {
        emit toLog(QString("[error] Can not create data source"));
        return;
    }

    emit toLog(QString("[info] Data source has been successfully created"));

    OGRDataSource::DestroyDataSource(ds);
}


/* ------------------------------------------------------------------ */
/*                         Read from the network                      */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_clicked()
{
    const char *str;

    OGRRegisterAll();

    OGRDataSource *poDS;
    str = "..\\..\\temp";
    //poDS = OGRSFDriverRegistrar::Open(str, FALSE);
    //(temporary instead of OGRSFDriverRegistrar)--------------
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str,FALSE);
    //---------------------------------------------------------
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

    // Read metadata from the model.
    OGRFeature *poFeature;
    poLayer->ResetReading();
    while( (poFeature = poLayer->GetNextFeature()) != NULL )
    {
        emit toLog("[output] Network metadata: ");
        emit toLog(QString(poFeature->GetFieldAsString(0)) + QString(" = ") + QString(poFeature->GetFieldAsString(1)));
    }

    OGRDataSource::DestroyDataSource(poDS);
}


/* ------------------------------------------------------------------ */
/*                          Write to the network                      */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_2_clicked()
{
    OGRRegisterAll();

    OGRDataSource *poDS;
    const char *str;
    str = "..\\..\\temp";
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str,TRUE);
    if( poDS == NULL )
    {
        emit toLog(QString("[error] Can not open data source ") + QString(str));
        return;
    }

    // Create additional (user) layer.
    OGRLayer *poLayer;
    poLayer = poDS->CreateLayer("test_point_layer", NULL, wkbPoint, NULL);
    if( poLayer == NULL )
    {
        emit toLog(QString("[error] Can not create layer"));
        return;
    }

    // Add an additional (user) attribute.
    OGRFieldDefn oField("test_field", OFTString);
    oField.SetWidth(32);
    if( poLayer->CreateField( &oField ) != OGRERR_NONE )
    {
        emit toLog(QString("[error] Can not create field"));
        return;
    }
    emit toLog(QString("[info] test_point_layer has been successfully created"));

    // Add objects with geometry.
    for (int i=0; i<10; i++)
    {
        OGRFeature *poFeature;
        poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
        poFeature->SetField("test_field", QString(QString("aaa") + QString::number(i)).toUtf8().data());
        //poFeature->SetField("test_field", "ddd");
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
    emit toLog(QString("[info] Features have been added to the new layer successfully"));

    if (poDS->SyncToDisk() != OGRERR_NONE)
    {
        emit toLog(QString("[error] Failed to write to disk: "));
        return;
    }
    emit toLog(QString("[info] Features have been successfully written"));

    OGRDataSource::DestroyDataSource(poDS);
}


/* ------------------------------------------------------------------ */
/*                  Read an object from the network                   */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_3_clicked()
{
    OGRRegisterAll();

    OGRDataSource *poDS;
    const char *str;
    str = "..\\..\\temp";
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str,TRUE);
    if( poDS == NULL )
    {
        emit toLog(QString("[error] Can not open data source ") + QString(str));
        return;
    }

    OGRFeature *poFeature = static_cast<OGRGnmDataSource*>(poDS)->getFeature(2);
    if (poFeature == NULL)
    {
        emit toLog(QString("[error] Can not get feature "));
        return;
    }

    emit toLog(QString("[output] Features attributes: "));
    emit toLog(QString(poFeature->GetFieldAsString("test_field")));

    OGRDataSource::DestroyDataSource(poDS);
}


/* ------------------------------------------------------------------ */
/*                          Delete network                            */
/* ------------------------------------------------------------------ */
void Widget::on_pushButton_5_clicked()
{
    OGRRegisterAll();

    OGRSFDriver *dr = new OGRGnmDriver();

    if (dr->DeleteDataSource("..\\..\\temp") != OGRERR_NONE)
    {
        emit toLog(QString("[error] Error while deleting layers in data source"));
        return;
    }

    emit toLog(QString("[info] All layers have been deleted successfully"));
}








