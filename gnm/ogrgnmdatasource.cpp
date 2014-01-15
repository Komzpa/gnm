#include "ogr_gnm.h"


/************************************************************************/
/*                       Open an existing data source                   */
/************************************************************************/

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

    // Fill the papoLayers array with pointers to specific format layers.
    for (int i = 0; i < geoDataSrc->GetLayerCount(); i++)
    {
        this->addLayer(geoDataSrc->GetLayer(i));
    }

//зачем это здесь, если это достаётся из слоёв, когда надо, в коде вызова?
    // TODO: Open table network_meta.
    // TODO: Read SRS and network format version.

    return NERR_NONE;
}


/************************************************************************/
/*                       Create new data source                         */
/************************************************************************/

NErr OGRGnmDataSource::create(const char *pszFilename, char **papszOptions)
{
    // Open data source with given format.

    // TODO: get the driverName from papszOptions
    const char *driverName = "ESRI Shapefile";
    OGRSFDriver* geoDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driverName);
    // TODO: replace NERR_ANY with the specific error code.
    if (geoDriver == NULL) return NERR_ANY;

    this->geoDataSrc = geoDriver->CreateDataSource(pszFilename, NULL);
    if (this->geoDataSrc == NULL) return NERR_ANY;

    OGRFieldDefn *poField;
    OGRLayer *poLayer;
    OGRFeature *poFeature;

/* ------------------------------------------------------------------ */
/*                      Create table network_meta.                    */
/* ------------------------------------------------------------------ */

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

    this->addLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                  Create table network_graph.                       */
/* ------------------------------------------------------------------ */

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

    this->addLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                   Create table network_rules.                      */
/* ------------------------------------------------------------------ */

    poLayer = this->geoDataSrc->CreateLayer("network_rules", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("connection_rules", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    // TODO: Write default rules.

    this->addLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                   Synchronize changes with disk                    */
/* ------------------------------------------------------------------ */

    if (geoDataSrc->SyncToDisk() != OGRERR_NONE) return NERR_ANY;

    return NERR_NONE;
}


/************************************************************************/
/*                       Essential mathods                              */
/************************************************************************/

OGRGnmDataSource::OGRGnmDataSource()
{
    pszName = NULL;
    geoDataSrc = NULL;
    nLayers = 0;
    papoLayers = NULL;
}

OGRGnmDataSource::~OGRGnmDataSource()
{
    // TODO: understand what should we delete using delete statement,
    // and what using CPLFree.
    CPLFree(pszName);
    CPLFree(geoDataSrc);

    for(int i = 0; i < nLayers; i++)
        delete papoLayers[i];
    CPLFree(papoLayers);
}


const char* OGRGnmDataSource::GetName()
{
    return pszName;
}


int OGRGnmDataSource::GetLayerCount()
{
    //return this->geoDataSrc->GetLayerCount();

    return nLayers;
}


OGRLayer* OGRGnmDataSource::GetLayer(int iLayer)
{
    //return this->geoDataSrc->GetLayer(index);

    if(iLayer < 0 || iLayer >= nLayers) return NULL;
    else return papoLayers[iLayer];
}


OGRLayer* OGRGnmDataSource::GetLayerByName(const char *name)
{
    //return this->geoDataSrc->GetLayerByName(name);

    for (int i=0; i<nLayers; i++)
    {
        if (strcmp(papoLayers[i]->GetName(), name) == 0)
        {
            return papoLayers[i];
        }
    }

    return NULL;
}


int OGRGnmDataSource::TestCapability(const char *)
{
    return FALSE;
}


/************************************************************************/
/*                       Create new layer in data source                */
/************************************************************************/

OGRLayer* OGRGnmDataSource::CreateLayer(const char *pszName, OGRSpatialReference *poSpatialRef,
                                        OGRwkbGeometryType eGType, char **papszOptions)
{
    // TODO: use srs from the network_meta table.
    OGRSpatialReference *poSR = NULL;

    // Check for available geometry types.
    // The list of types is about to be widen in future.
    if (eGType != wkbPoint && eGType != wkbLineString) return NULL;

    OGRLayer *geoLayer;
    geoLayer = this->geoDataSrc->CreateLayer(pszName, poSR, eGType, papszOptions);
    if (geoLayer == NULL) return NULL;

    // Add obligatory fields to the new layer: blocking state and direction.
    OGRFieldDefn *oField;
    oField = new OGRFieldDefn("is_blocked", OFTInteger);
    if(geoLayer->CreateField(oField) != OGRERR_NONE) return NULL;

    oField = new OGRFieldDefn("direction", OFTInteger);
    if(geoLayer->CreateField(oField) != OGRERR_NONE) return NULL;

    // Wrap a special format layer with our format and return it.
    return addLayer(geoLayer);
}


/************************************************************************/
/*                      Add new GnmLayer to the data source             */
/************************************************************************/

OGRGnmLayer* OGRGnmDataSource::addLayer(OGRLayer *layer)
{
    // TODO: check if nLayers has reached maximum.

    // Add a pointer to the general array of OGRGnmLayer pointers,
    // where each of them stores a pointer to the OGRLayer of
    // special format.
    nLayers++;
    papoLayers = (OGRGnmLayer **)
            CPLRealloc(papoLayers, sizeof(OGRGnmLayer *) * (nLayers));
    //papoLayers[nLayers - 1] = layer;
    papoLayers[nLayers - 1] = new OGRGnmLayer(layer);

    return papoLayers[nLayers - 1];
}











