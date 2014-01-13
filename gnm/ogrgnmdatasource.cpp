#include "ogr_gnm.h"


NErr OGRGnmDataSource::open(const char* pszFilename, int bUpdate, char** papszOptions)
{
    //Open data source with given format

    //TODO: брать из опций строку подключения
    geoDataSrc = OGRSFDriverRegistrar::Open(pszFilename, TRUE); //If this method fails, CPLGetLastErrorMsg() can be used to check if there is an error message explaining why.
    if(geoDataSrc == NULL) return NERR_ANY;

    //TODO: Open table network_meta
    //TODO: Read SRS and network format version

    return NERR_NONE;
}


NErr OGRGnmDataSource::create(const char *pszFilename, char **papszOptions)
{
    //Open data source with given format

    //TODO: доставать из papszOptions нужные строки, в т.ч. и имя драйвера
    const char *driverName = "ESRI Shapefile";
    OGRSFDriver* geoDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driverName);
    //TODO: заменить на код конкретной ошибки
    if (geoDriver == NULL) return NERR_ANY;

    this->geoDataSrc = geoDriver->CreateDataSource(pszFilename, NULL);
    if (this->geoDataSrc == NULL) return NERR_ANY;

    OGRFieldDefn *poField;
    OGRLayer *poLayer;
    OGRFeature *poFeature;

    //Create table network_meta

    poLayer = this->geoDataSrc->CreateLayer("network_meta", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("param_name", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    poField = new OGRFieldDefn("value", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //Write SRS and network format version
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "Format version");
    poFeature->SetField("value", "1.0");
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return NERR_ANY;
    OGRFeature::DestroyFeature(poFeature);

    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "SRS");
    poFeature->SetField("value", "WGS 84");
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return NERR_ANY;
    OGRFeature::DestroyFeature(poFeature);

    //Create table network_graph

    poLayer = this->geoDataSrc->CreateLayer("network_graph", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("id", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //TODO: удостовериться, что драйвер поддерживает OFTIntegerList, иначе делать несколько таблиц (?)
    //poField = new OGRFieldDefn("connected_ids", OFTIntegerList);
    //if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY; //узнать ошибку можно вернув код OGRErr

    poField = new OGRFieldDefn("weight", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //Create table network_rules

    poLayer = this->geoDataSrc->CreateLayer("network_rules", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("connection_rules", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //TODO: Write default rules

    //записываем по умолчанию добавленные слои
    if (geoDataSrc->SyncToDisk() != OGRERR_NONE) return NERR_ANY;

    return NERR_NONE;
}


OGRGnmDataSource::OGRGnmDataSource()
{
    pszName = NULL;
    //formatName = NULL;
    geoDataSrc = NULL;
    //idCounter = 0;
}

OGRGnmDataSource::~OGRGnmDataSource()
{
    //TODO: разобраться что удалять делитом, а что через ЦПЛФри

    CPLFree(pszName);
    //CPLFree(formatName);
    CPLFree(geoDataSrc);
}


const char* OGRGnmDataSource::GetName()
{
    return pszName;
}


int OGRGnmDataSource::GetLayerCount()
{
    return this->geoDataSrc->GetLayerCount();
}


OGRLayer* OGRGnmDataSource::GetLayer(int index)
{
    return this->geoDataSrc->GetLayer(index);
}


int OGRGnmDataSource::TestCapability(const char *)
{
    return FALSE;
}






