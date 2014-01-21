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

    int count = geoDataSrc->GetLayerCount();

    // Fill the papoLayers array with pointers to specific format layers.
    for (int i = 0; i < count; i++)
    {
        this->wrapLayer(geoDataSrc->GetLayer(i));
    }

    // Get the last value of id_counter.
    OGRLayer *poLayer = geoDataSrc->GetLayerByName("network_meta");
    if (poLayer == NULL) return NERR_ANY;
    OGRFeature *poFeature = poLayer->GetFeature(GNM_METADATA_ID_COUNTER);
    if (poFeature == NULL) return NERR_ANY;
    const char *temp = poFeature->GetFieldAsString("value");
    idCounter = atol(temp);

    return NERR_NONE;
}


/************************************************************************/
/*                       Create new data source                         */
/************************************************************************/

NErr OGRGnmDataSource::create(const char *pszFilename, char **papszOptions)
{
    // Create data source with given format.
    // TODO: get the driverName from papszOptions.
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
    poFeature->SetFID(GNM_METADATA_FORMAT_VERSION); // We set FID here in order to get this meta-feature with OGRLayer::getFeature().
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return NERR_ANY;
    OGRFeature::DestroyFeature(poFeature);

    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "SRS");
    poFeature->SetField("value", "WGS 84");
    poFeature->SetFID(GNM_METADATA_SRS);
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return NERR_ANY;
    OGRFeature::DestroyFeature(poFeature);

    // Write unique feature id counter.
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "id_counter");
    poFeature->SetField("value", "0");
    poFeature->SetFID(GNM_METADATA_ID_COUNTER);
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return NERR_ANY;
    OGRFeature::DestroyFeature(poFeature);

    this->wrapLayer(poLayer);

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

    this->wrapLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                   Create table network_rules.                      */
/* ------------------------------------------------------------------ */

    poLayer = this->geoDataSrc->CreateLayer("network_rules", NULL, wkbNone, NULL);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("con_rules", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    // TODO: Write default rules.

    this->wrapLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                  Create table network_ids.                         */
/* ------------------------------------------------------------------ */
    char** papszCrOptions = NULL;
    papszCrOptions = CSLAddNameValue(papszCrOptions, "SHPT", "NULL");
    poLayer = this->geoDataSrc->CreateLayer("network_ids", NULL, wkbNone, papszCrOptions);
    CSLDestroy(papszCrOptions);
    if (poLayer == NULL) return NERR_ANY;

    poField = new OGRFieldDefn("layer_name", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return NERR_ANY;

    //--------- test ----------
    //poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    //poFeature->SetField("layer_name", "aaa111");
    //if (poFeature->SetFID(this->getNextFeatureId()) != OGRERR_NONE) return NERR_ANY;
    //if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return NERR_ANY;
    //OGRFeature::DestroyFeature(poFeature);
    //-------------------------

    this->wrapLayer(poLayer);

    //OGRErr err = poLayer->SyncToDisk();

/* ------------------------------------------------------------------ */
/*                       Set other parameters                         */
/* ------------------------------------------------------------------ */

    // Initialize layers' id counter from begining. Here is not nessesery
    // to read it from table network_meta.
    idCounter = 0;

/* ------------------------------------------------------------------ */
/*                   Synchronize changes with disk                    */
/* ------------------------------------------------------------------ */

    if (geoDataSrc->SyncToDisk() != OGRERR_NONE) return NERR_ANY;

    // TODO: understand, how to flush pending changes to disk:
    // via DestroyDataSource() or reimplementing SyncToDisk() for
    // each layer.
    OGRDataSource::DestroyDataSource(geoDataSrc);

    return NERR_NONE;
}



OGRFeature* OGRGnmDataSource::getFeature(long nGFID)
{
    /*
    // Determine the layer which holds the feature with this unque ID.
    OGRLayer *poLayer;
    poLayer = this->GetLayerByName("network_ids");
    if (poLayer == NULL) return NULL;

    poLayer->ResetReading();
    OGRFeature *poFeature;
    poFeature = poLayer->GetFeature(nGFID);
    if (poFeature == NULL) return NULL;

    const char *layerName = "";
    layerName = poFeature->GetFieldAsString("layer_name");
    if (layerName == "") return NULL;
    OGRFeature::DestroyFeature(poFeature);

    // Get the final feature from this layer.
    OGRLayer *poTargetLayer;
    poTargetLayer = this->GetLayerByName(layerName);
    if (poTargetLayer == NULL) return NULL;

    poTargetLayer->ResetReading();
    OGRFeature *poTargetFeature;
    poTargetFeature = poTargetLayer->GetFeature(nGFID);
    if (poTargetFeature == NULL) return NULL;

    return poTargetFeature;
    */

    return NULL;
}


long OGRGnmDataSource::getNextFeatureId()
{
    long res = idCounter;
    idCounter++;
    return res;
}

OGRDataSource* OGRGnmDataSource::getInnerDataSource()
{
    return geoDataSrc;
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
    //idCounter здесь инициализировать нельзя! Только в открытии/создании
    //idCounter = 0;
}

OGRGnmDataSource::~OGRGnmDataSource()
{
    // TODO: understand what should we delete using delete statement,
    // and what using CPLFree.
    CPLFree(pszName);
    //CPLFree(geoDataSrc);

    for(int i = 0; i < nLayers; i++)
        delete papoLayers[i];
    CPLFree(papoLayers);

    OGRDataSource::DestroyDataSource(geoDataSrc);
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

    const char *str;
    //int nom;
    //bool fl = false;
    for (int i=0; i<nLayers; i++)
    {
        str = papoLayers[i]->GetName();
        if (strcmp(str, name) == 0)
        {
            return papoLayers[i];
            //nom = i;
            //fl = true;
        }
    }

    //if (fl == true) return papoLayers[nom];
    //else return NULL;
    return NULL;
}


int OGRGnmDataSource::TestCapability(const char *)
{
    return FALSE;
}


OGRErr OGRGnmDataSource::SyncToDisk()
{
    return geoDataSrc->SyncToDisk();
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
    return wrapLayer(geoLayer);
    //return geoLayer;
}


/************************************************************************/
/*                      Add new GnmLayer to the data source             */
/************************************************************************/

OGRGnmLayer* OGRGnmDataSource::wrapLayer(OGRLayer *layer)
{
    // TODO: check if nLayers has reached maximum.

    // Add a pointer to the general array of OGRGnmLayer pointers,
    // where each of them stores a pointer to the OGRLayer of
    // special format.

    nLayers++;
    papoLayers = (OGRGnmLayer **)
            CPLRealloc(papoLayers, sizeof(OGRGnmLayer *) * (nLayers));
    papoLayers[nLayers - 1] = new OGRGnmLayer(layer, this);

    return papoLayers[nLayers - 1];
}











