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


/**********************************************************************/
/*                             Create network                         */
/**********************************************************************/
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


/**********************************************************************/
/*                         Read layer                                 */
/**********************************************************************/
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

    QByteArray ba = ui->lineEdit_3->text().toUtf8();
    const char* str2 = ba.data();
    OGRLayer *poLayer = poDS->GetLayerByName(str2);
    if(poLayer == NULL)
    {
        emit toLog(QString("[error] Can not open layer ") + QString(str2));
        return;
    }

//---test-----------------
    //const char *str1 = poLayer->GetFIDColumn();
    //const char *str2 = static_cast<OGRGnmLayer*>(poLayer)->getInnerLayer()->GetFIDColumn();
//------------------------

    QString line;
    OGRFeature *poFeature;
    poLayer->ResetReading();
    emit toLog("[output] " + QString(str2) + " features' attributes: ");
    int iField;
    while((poFeature = poLayer->GetNextFeature()) != NULL)
    {
        line = "FID=" + QString::number(poFeature->GetFID()) + "  ";
        OGRFeatureDefn *poFDefn = poLayer->GetLayerDefn();
        for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
        {
            line = line + "[" + poFeature->GetFieldAsString(iField) + "]  ";
        }
        emit toLog(line);
        OGRFeature::DestroyFeature(poFeature);
    }

    OGRDataSource::DestroyDataSource(poDS);
}


/**********************************************************************/
/*           add test user table with geometry objects                */
/**********************************************************************/
void Widget::on_pushButton_2_clicked()
{
    OGRRegisterAll();

    OGRDataSource *poDS;
    const char *str;
    str = "..\\..\\temp";
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str, TRUE);
    if(poDS == NULL)
    {
        emit toLog(QString("[error] Can not open data source ") + QString(str));
        return;
    }

    // Create additional (user) layer.
    OGRLayer *poLayer;
    poLayer = poDS->CreateLayer("test_point_layer", NULL, wkbPoint, NULL);
    if(poLayer == NULL)
    {
        emit toLog(QString("[error] Can not create layer"));
        return;
    }

    // Add an additional (user) attribute.
    OGRFieldDefn oField("test_field", OFTString);
    oField.SetWidth(32);
    if(poLayer->CreateField( &oField ) != OGRERR_NONE)
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

    OGRDataSource::DestroyDataSource(poDS);
}


/**********************************************************************/
/*                    Read an object from the network                 */
/**********************************************************************/
void Widget::on_pushButton_3_clicked()
{
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

    long UFID = ui->lineEdit->text().toLong();
    OGRFeature *poFeature = static_cast<OGRGnmDataSource*>(poDS)->getFeature(UFID);
    if (poFeature == NULL)
    {
        emit toLog(QString("[error] Can not get feature "));
        return;
    }
    else
    {
        QString line;
        emit toLog(QString("[output] Features attributes: "));
        int iField;
            line = "FID=" + QString::number(poFeature->GetFID()) + "  " ;
            OGRFeatureDefn *poFDefn = poFeature->GetDefnRef();
            for( iField = 0; iField < poFDefn->GetFieldCount(); iField++ )
            {
                line = line + "[" + poFeature->GetFieldAsString(iField) + "]  ";
            }
            emit toLog(line);
       OGRFeature::DestroyFeature(poFeature);
    }

    OGRDataSource::DestroyDataSource(poDS);
}


/**********************************************************************/
/*                          Delete network                            */
/**********************************************************************/
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


/**********************************************************************/
/*                            Import data                             */
/**********************************************************************/
void Widget::on_pushButton_6_clicked()
{
    OGRRegisterAll();
    OGRDataSource *poDS;
    const char *str;
    str = "..\\..\\temp";
    OGRSFDriver *dr = new OGRGnmDriver();
    poDS = dr->Open(str, TRUE);
    if( poDS == NULL )
    {
        emit toLog(QString("[error] Can not open data source ") + QString(str));
        return;
    }

    char *path = NULL;
    char *layer = NULL;
    QByteArray ba1 = ui->lineEdit_2->text().toUtf8();
    path = ba1.data();
    QByteArray ba2 = ui->lineEdit_4->text().toUtf8();
    layer = ba2.data();
    OGRErr err = static_cast<OGRGnmDataSource*>(poDS)->importLayer(path, layer, NULL);
    if (err != OGRERR_NONE)
        emit toLog(QString("[error] Can not import layer "));
    else
        emit toLog(QString("[info] The layer has been imported successfully"));

    OGRDataSource::DestroyDataSource(poDS);
}


void Widget::on_pushButton_7_clicked()
{

}






