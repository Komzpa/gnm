#include "widget.h"
#include "ui_widget.h"

Widget::~Widget()
{
    // Close data source if it was not.
    if (!wasDSClosed)
    {
        this->on_pushButton_7_clicked();
    }

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

    poDriver = NULL;
    poDataSource = NULL;
    OGRRegisterAll();
    wasDSClosed = true;
}


/**********************************************************************/
/*                             Create network                         */
/**********************************************************************/
void Widget::on_pushButton_4_clicked()
{
    if (!wasDSClosed)
    {
        emit toLog(QString("[error] There is still opened DS"));
        return;
    }

    //OGRRegisterAll(); // ogr\ogrsf_frmts\generic\ogrsfdriverregistrar.cpp

    ////OGRSFDriver *dr = new OGRGnmDriver();
    poDriver = new OGRGnmDriver();

    // Select a directory and data source format.
    QByteArray ba = ui->lineEdit_5->text().toUtf8();
    char* path = ba.data();
    QByteArray ba2 = ui->lineEdit_6->text().toUtf8();
    const char* driver = ba2.data();
//test--------------------------------------
    //char* pszName = "PG:\"dbname='test_postgis' host='127.0.0.1' port='5432' user='postgres' password='zsxcfv'\"";
    //char* pszName = "PG:dbname=test_postgis";
    //const char* pszDriver = "PostgreSQL";
//------------------------------------------

    // Create data source.
    char** papszOptions = NULL;
    papszOptions = CSLAddNameValue(papszOptions, GNM_OPTION_DRIVER_NAME, driver);
    ////OGRDataSource *ds = dr->CreateDataSource(pszName, papszOptions);
    poDataSource = poDriver->CreateDataSource(path, papszOptions);
    CSLDestroy(papszOptions);

    if (poDataSource == NULL)
    {
        emit toLog(QString("[error] Can not create data source"));
        return;
    }

    emit toLog(QString("[info] Data source has been successfully created"));
    ui->label->setText("Currently opened network:  [" + QString(path) + "]  [" + QString(driver) + "]");

    wasDSClosed = false;
    ////OGRDataSource::DestroyDataSource(ds);
}


/**********************************************************************/
/*                             Open network                           */
/**********************************************************************/
void Widget::on_pushButton_8_clicked()
{
    if (!wasDSClosed)
    {
        emit toLog(QString("[error] There is still opened DS"));
        return;
    }

    QByteArray ba = ui->lineEdit_8->text().toUtf8();
    char* path = ba.data();
    //(temporary instead of OGRSFDriverRegistrar)--------------
    //poDataSource = OGRSFDriverRegistrar::Open(path, FALSE);
    poDriver = new OGRGnmDriver();
    //---------------------------------------------------------

    poDataSource = poDriver->Open(path,FALSE);
    if(poDataSource == NULL)
    {
        emit toLog(QString("[error] Can not open data source "));
        return;
    }

    emit toLog(QString("[info] Data source has been successfully opened"));
    ui->label->setText("Currently opened network:  [" + QString(path) + "]");

    wasDSClosed = false;
}




/**********************************************************************/
/*                         Read layer                                 */
/**********************************************************************/
void Widget::on_pushButton_clicked()
{
    if (wasDSClosed)
    {
        emit toLog(QString("[error] There is no opened DS"));
        return;
    }

    ////const char *str;
    ////OGRRegisterAll();
    ////OGRDataSource *poDS;
    ////str = "..\\..\\temp";
    //////poDS = OGRSFDriverRegistrar::Open(str, FALSE);
    //////(temporary instead of OGRSFDriverRegistrar)--------------
    ////OGRSFDriver *dr = new OGRGnmDriver();
    ////poDS = dr->Open(str,FALSE);
    //////---------------------------------------------------------
    ////if( poDS == NULL )
    ////{
        ////emit toLog(QString("[error] Can not open data source ") + QString(str));
        ////return;
    ////}

    QByteArray ba = ui->lineEdit_3->text().toUtf8();
    const char* name = ba.data();
    OGRLayer *poLayer = poDataSource->GetLayerByName(name);
    if(poLayer == NULL)
    {
        emit toLog(QString("[error] Can not open layer "));
        return;
    }
//---test-----------------
    //const char *str1 = poLayer->GetFIDColumn();
    //const char *str2 = static_cast<OGRGnmLayer*>(poLayer)->getInnerLayer()->GetFIDColumn();
//------------------------

    QString line;
    OGRFeature *poFeature;
    poLayer->ResetReading();
    emit toLog("[output] " + QString(name) + " features' attributes: ");
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

    ////OGRDataSource::DestroyDataSource(poDS);
}


/**********************************************************************/
/*           add test user table with geometry objects                */
/**********************************************************************/
void Widget::on_pushButton_2_clicked()
{
    if (wasDSClosed)
    {
        emit toLog(QString("[error] There is no opened DS"));
        return;
    }

    ////OGRRegisterAll();
    ////OGRDataSource *poDS;
    ////const char *str;
    ////str = "..\\..\\temp";
    ////OGRSFDriver *dr = new OGRGnmDriver();
    ////poDS = dr->Open(str, TRUE);
    ////if(poDS == NULL)
    ////{
        ////emit toLog(QString("[error] Can not open data source ") + QString(str));
        ////return;
    ////}

    // Create additional (user) layer.
    OGRLayer *poLayer;
    poLayer = poDataSource->CreateLayer("test_point_layer", NULL, wkbPoint, NULL);
    if(poLayer == NULL)
    {
        emit toLog(QString("[error] Can not create layer"));
        return;
    }

    // Add an additional (user) attribute.
    OGRFieldDefn oField("test_field", OFTString);
    oField.SetWidth(32);
    if(poLayer->CreateField(&oField) != OGRERR_NONE)
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
        poFeature->SetField("test_field", QString(QString("tratata") + QString::number(i)).toUtf8().data());
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

    if (poDataSource->SyncToDisk() != OGRERR_NONE)
    {
        emit toLog(QString("[error] Features have been added but not written to disk"));
    }
    else
    {
        emit toLog(QString("[info] Features have been added to the new layer successfully"));
    }

    ////OGRDataSource::DestroyDataSource(poDS);
}


/**********************************************************************/
/*                    Read an object from the network                 */
/**********************************************************************/
void Widget::on_pushButton_3_clicked()
{
    if (wasDSClosed)
    {
        emit toLog(QString("[error] There is no opened DS"));
        return;
    }

    ////OGRRegisterAll();
    ////OGRDataSource *poDS;
    ////const char *str;
    ////str = "..\\..\\temp";
    ////OGRSFDriver *dr = new OGRGnmDriver();
    ////poDS = dr->Open(str,TRUE);
    ////if(poDS == NULL)
    ////{
        ////emit toLog(QString("[error] Can not open data source ") + QString(str));
        ////return;
    ////}

    long UFID = ui->lineEdit->text().toLong();
    OGRFeature *poFeature = static_cast<OGRGnmDataSource*>(poDataSource)->getFeature(UFID);
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

    ////OGRDataSource::DestroyDataSource(poDS);
}


/**********************************************************************/
/*                            Import data                             */
/**********************************************************************/
void Widget::on_pushButton_6_clicked()
{
    if (wasDSClosed)
    {
        emit toLog(QString("[error] There is no opened DS"));
        return;
    }

    ////OGRRegisterAll();
    ////OGRDataSource *poDS;
    ////const char *str;
    ////str = "..\\..\\temp";
    ////OGRSFDriver *dr = new OGRGnmDriver();
    ////poDS = dr->Open(str, TRUE);
    ////if( poDS == NULL )
    ////{
        ////emit toLog(QString("[error] Can not open data source ") + QString(str));
        ////return;
    ////}

    char *path = NULL;
    char *layer = NULL;
    QByteArray ba1 = ui->lineEdit_2->text().toUtf8();
    path = ba1.data();
    QByteArray ba2 = ui->lineEdit_4->text().toUtf8();
    layer = ba2.data();
    OGRErr err = static_cast<OGRGnmDataSource*>(poDataSource)->importLayer(path, layer, NULL);
    if (err != OGRERR_NONE)
        emit toLog(QString("[error] Can not import layer "));
    else
    {
        if (poDataSource->SyncToDisk() != OGRERR_NONE)
        {
            emit toLog(QString("[error] The layer has been imported but not written to disk"));
        }
        else
        {
            emit toLog(QString("[info] The layer has been imported successfully"));
        }
    }

    ////OGRDataSource::DestroyDataSource(poDS);
}


/**********************************************************************/
/*                          Close network                             */
/**********************************************************************/
void Widget::on_pushButton_7_clicked()
{
    if (wasDSClosed)
    {
        emit toLog(QString("[error] There is no opened DS"));
        return;
    }

    OGRDataSource::DestroyDataSource(poDataSource);
    delete poDriver;
    emit toLog(QString("[info] The data source has been successfully closed"));
    ui->label->setText("Currently opened network: -");
    wasDSClosed = true;
}


/**********************************************************************/
/*                          Delete network                            */
/**********************************************************************/
void Widget::on_pushButton_5_clicked()
{
    if (!wasDSClosed)
    {
        emit toLog(QString("[error] Close the DS before"));
        return;
    }
    // The poDataSource must be closed at this time (destroyed).

    QByteArray ba = ui->lineEdit_7->text().toUtf8();
    char* path = ba.data();

    OGRSFDriver *dr = new OGRGnmDriver();
    if (dr->DeleteDataSource(path) != OGRERR_NONE)
    {
        emit toLog(QString("[error] Error while deleting layers in data source"));
        return;
    }

    emit toLog(QString("[info] All layers have been deleted successfully"));
}




