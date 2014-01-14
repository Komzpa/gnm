#include "ogr_gnm.h"


NErr OGRGnmDataSource::open(const char* pszFilename, int bUpdate, char** papszOptions)
{
    // TUTORIAL: Note that Open() methods should try and determine that a file isn't
    // of the identified format as efficiently as possible, since many drivers may be
    // invoked with files of the wrong format before the correct driver is reached.

    // Open data source with the given format.

    // TUTORIAL: If this method fails, CPLGetLastErrorMsg() can be used to check
    // if there is an error message explaining why.
    geoDataSrc = OGRSFDriverRegistrar::Open(pszFilename, TRUE);
    if(geoDataSrc == NULL) return NERR_ANY;

//зачем это здесь, если это достаётся из слоёв, когда надо, в коде вызова?
    // TODO: Open table network_meta.
    // TODO: Read SRS and network format version.

    return NERR_NONE;
}


NErr OGRGnmDataSource::create(const char *pszFilename, char **papszOptions)
{
    // Open data source with given format.

    const char *driverName = "ESRI Shapefile";
    OGRSFDriver* geoDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driverName);
    // TODO: replace NERR_ANY with the specific error code.
    if (geoDriver == NULL) return NERR_ANY;

    this->geoDataSrc = geoDriver->CreateDataSource(pszFilename, NULL);
    if (this->geoDataSrc == NULL) return NERR_ANY;

    OGRFieldDefn *poField;
    OGRLayer *poLayer;
    OGRFeature *poFeature;

    // Create table network_meta.

    poLayer = this->geoDataSrc->CreateLayer("network_meta", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("param_name", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    poField = new OGRFieldDefn("value", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    // Write SRS and network format version.
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

    // Create table network_graph.

    poLayer = this->geoDataSrc->CreateLayer("network_graph", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("id", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    // TODO: make sure that the driver supports OFTIntegerList.
    // Otherwise operate with several tables?
    //poField = new OGRFieldDefn("connected_ids", OFTIntegerList);
    //if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    poField = new OGRFieldDefn("weight", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    // Create table network_rules.

    poLayer = this->geoDataSrc->CreateLayer("network_rules", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("connection_rules", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    // TODO: Write default rules.

    // Synchronize added layers with disk.
    if (geoDataSrc->SyncToDisk() != OGRERR_NONE) return NERR_ANY;

    return NERR_NONE;
}


OGRGnmDataSource::OGRGnmDataSource()
{
    pszName = NULL;
    geoDataSrc = NULL;
}

OGRGnmDataSource::~OGRGnmDataSource()
{
    // TODO: understand what should we delete using delete statement,
    // and what using CPLFree.
    CPLFree(pszName);
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


OGRLayer* OGRGnmDataSource::GetLayerByName(const char *name)
{
    return this->geoDataSrc->GetLayerByName(name);
}


int OGRGnmDataSource::TestCapability(const char *)
{
    return FALSE;
}


OGRLayer* OGRGnmDataSource::CreateLayer(const char *pszName, OGRSpatialReference *poSpatialRef,
                                        OGRwkbGeometryType eGType, char **papszOptions)
{
    // TODO: use srs from the network_meta table.
    OGRSpatialReference *poSR = NULL;

    // Check for available geometry types.
    // The list of types is about to be widen in future.
    if (eGType != wkbPoint && eGType != wkbLineString) return NULL;

    OGRLayer *tempLayer;
    tempLayer = this->geoDataSrc->CreateLayer(pszName, poSR, eGType, papszOptions);
    if (tempLayer == NULL) return NULL;

    // Add obligatory fields to the new layer: blocking state and direction.
    OGRFieldDefn *oField;
    oField = new OGRFieldDefn("is_blocked", OFTInteger);
    if(tempLayer->CreateField(oField) != OGRERR_NONE) return NULL;

    oField = new OGRFieldDefn("direction", OFTInteger);
    if(tempLayer->CreateField(oField) != OGRERR_NONE) return NULL;

    return tempLayer;
}






