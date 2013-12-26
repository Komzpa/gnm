#include "ogr_gnm.h"


NErr OGRGnmDataSource::open(const char* pszFilename, int bUpdate, char** papszOptions)
{
    //TODO: Open data source with given format

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

    //TODO: формировать строку с новым названием и путём к данным заданного формата
    const char *conString = "..\\..\\temp\\test.shp";
    this->geoDataSrc = geoDriver->CreateDataSource(conString, NULL);
    if (this->geoDataSrc == NULL) return NERR_ANY;

    OGRFieldDefn *poField;
    OGRLayer *poLayer;

    //Create table network_meta

    poLayer = this->geoDataSrc->CreateLayer("network_meta", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;
    poField = new OGRFieldDefn("version", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;
    poField = new OGRFieldDefn("srs", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //TODO: Write SRS and network format version

    //Create table network_graph
    poLayer = this->geoDataSrc->CreateLayer("network_graph", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;
    poField = new OGRFieldDefn("id", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;
    //TODO: удостовериться, что драйвер поддерживает OFTIntegerList, иначе делать несколько таблиц (?)
    poField = new OGRFieldDefn("connected_ids", OFTIntegerList);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;
    poField = new OGRFieldDefn("weight", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //Create table network_rules
    poLayer = this->geoDataSrc->CreateLayer("network_rules", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;
    poField = new OGRFieldDefn("connection_rules", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //TODO: Write default rules

    return NERR_NONE;
}


OGRGnmDataSource::OGRGnmDataSource()
{
    pszName = NULL;
    //network_meta = NULL;
    //network_graph = NULL;
    //network_rules = NULL;
    formatName = NULL;
    geoDataSrc = NULL;
    idCounter = 0;
}

OGRGnmDataSource::~OGRGnmDataSource()
{
    //TODO: разобраться что удалять делитом, а что через ЦПЛФри

    //delete network_meta;
    //delete network_graph;
    //delete network_rules;

    CPLFree(pszName);
    CPLFree(formatName);
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

//проверяет возможности как себя, так и geoDataSrc
int OGRGnmDataSource::TestCapability(const char *)
{
    return FALSE;
}






