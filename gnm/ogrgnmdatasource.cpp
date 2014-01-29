#include "ogr_gnm.h"


/************************************************************************/
/*                                 open()                               */
/************************************************************************/
OGRErr OGRGnmDataSource::open(const char* pszFilename, int bUpdate, char** papszOptions)
{
    // TODO: (TUTORIAL) Note that Open() methods should try and determine that a file isn't
    // of the identified format as efficiently as possible, since many drivers may be
    // invoked with files of the wrong format before the correct driver is reached.

    // Open data source with the given format.
    // TODO: check the obtained format of the data source, or force to use one of
    // supported formats (may be using papszOptions).
    geoDataSrc = OGRSFDriverRegistrar::Open(pszFilename, TRUE);
    if (geoDataSrc == NULL) return OGRERR_FAILURE;

    int count = geoDataSrc->GetLayerCount();

    // Fill the papoLayers array with pointers to specific format layers.
    for (int i = 0; i < count; i++)
    {
        this->wrapLayer(geoDataSrc->GetLayer(i));
    }

    // TODO: check that all system layers were found.
    //...

    // TODO: check that all obligatory metadata was found.
    //...

    /*
    // Get the last value of id_counter.
    //OGRLayer *poLayer = geoDataSrc->GetLayerByName("network_meta");
    //if (poLayer == NULL) return OGRERR_FAILURE;
    //OGRFeature *poFeature = poLayer->GetFeature(GNM_METADATA_ID_COUNTER);
    //if (poFeature == NULL) return OGRERR_FAILURE;
    //const char *temp = poFeature->GetFieldAsString("value");
    //idCounter = atol(temp); // atoi is also used in GetFieldAsString().
    */

    //const char *idStr = this->getMetaParamValue("id_counter");
    //idCounter = atol(idStr);

    // Give a name to this DataSource.
    //pszName = pszFilename;
    pszName = CPLStrdup(pszFilename);

    return OGRERR_NONE;
}


/************************************************************************/
/*                              create()                                */
/************************************************************************/
OGRErr OGRGnmDataSource::create(const char *pszFilename, char **papszOptions)
{
    // TODO: replace any NERR_ANY with the specific error code.

    // Get the driverName from papszOptions.
    const char *driverName = CSLFetchNameValue(papszOptions, GNM_OPTION_DRIVER_NAME);

    // Check for the valid driverName.
    //if (CSLFindString(GNMSupportedDrivers, driverName) == -1) return NERR_ANY;

    //while(!fl)
    //{
        //if (GNMSystemLayers[j] == NULL) break;

    int i = 0;
    while(TRUE)
    {
        if (strcmp(driverName,GNMSupportedDrivers[i]) == 0) break;
        else if (GNMSupportedDrivers[i] == NULL) return OGRERR_FAILURE;
    }

/* ------------------------------------------------------------------ */
/*               Create data source with given format.                */
/* ------------------------------------------------------------------ */
    OGRSFDriver* geoDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driverName);
    if (geoDriver == NULL) return OGRERR_FAILURE;

    this->geoDataSrc = geoDriver->CreateDataSource(pszFilename, NULL);
    if (this->geoDataSrc == NULL) return OGRERR_FAILURE;

    OGRFieldDefn *poField;
    OGRLayer *poLayer;
    OGRFeature *poFeature;

/* ------------------------------------------------------------------ */
/*                      Create table network_meta.                    */
/* ------------------------------------------------------------------ */
    poLayer = this->geoDataSrc->CreateLayer("network_meta", NULL, wkbNone, NULL);
    if (poLayer == NULL) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("param_name", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("value", OFTString);
    if(poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    //poField = new OGRFieldDefn("OGC_FID", OFTInteger);
    //if(poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    // Write network format version.
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "Format version");
    poFeature->SetField("value", "1.0");
    //poFeature->SetFID(GNM_METADATA_FORMAT_VERSION); // We set FID here in order to get this meta-feature with OGRLayer::getFeature().
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return OGRERR_FAILURE;
    OGRFeature::DestroyFeature(poFeature);

    // Write SRS.
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "SRS");
    poFeature->SetField("value", "WGS 84");
    //poFeature->SetFID(GNM_METADATA_SRS);
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return OGRERR_FAILURE;
    OGRFeature::DestroyFeature(poFeature);

    // Write unique feature id counter.
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "id_counter");
    poFeature->SetField("value", "0");
    //poFeature->SetFID(GNM_METADATA_ID_COUNTER);
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return OGRERR_FAILURE;
    OGRFeature::DestroyFeature(poFeature);

    // Write default network alias.
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "network_alias");
    poFeature->SetField("value", "mynetwork"); // Without "_".
    //poFeature->SetFID(GNM_METADATA_DEFAULT_NETWORK_ALIAS);
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return OGRERR_FAILURE;
    OGRFeature::DestroyFeature(poFeature);

    // Write default network name.
    poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());
    poFeature->SetField("param_name", "network_name");
    poFeature->SetField("value", "My network");
    //poFeature->SetFID(GNM_METADATA_DEFAULT_NETWORK_NAME);
    if(poLayer->CreateFeature(poFeature) != OGRERR_NONE) return OGRERR_FAILURE;
    OGRFeature::DestroyFeature(poFeature);

    this->wrapLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                  Create table network_graph.                       */
/* ------------------------------------------------------------------ */
    poLayer = this->geoDataSrc->CreateLayer("network_graph", NULL, wkbNone, NULL);
    if (poLayer == NULL) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("id", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    // TODO: make sure that the driver supports OFTIntegerList.
    // Otherwise operate with several tables?
    //poField = new OGRFieldDefn("connected_ids", OFTIntegerList);
    //if (poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("weight", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    this->wrapLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                   Create table network_rules.                      */
/* ------------------------------------------------------------------ */
    poLayer = this->geoDataSrc->CreateLayer("network_rules", NULL, wkbNone, NULL);
    if (poLayer == NULL) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("con_rules", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    // TODO: Write default rules.

    this->wrapLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                  Create table network_ids.                         */
/* ------------------------------------------------------------------ */
    //char** papszCrOptions = NULL;
    //papszCrOptions = CSLAddNameValue(papszCrOptions, "SHPT", "NULL");
    //poLayer = this->geoDataSrc->CreateLayer("network_ids", NULL, wkbNone, papszCrOptions);
    //CSLDestroy(papszCrOptions);
    //if (poLayer == NULL) return NERR_ANY;

    poLayer = this->geoDataSrc->CreateLayer("network_ids", NULL, wkbNone, NULL);
    if (poLayer == NULL) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("id_global", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("layer_name", OFTString);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    poField = new OGRFieldDefn("id_local", OFTInteger);
    if (poLayer->CreateField(poField) != OGRERR_NONE) return OGRERR_FAILURE;

    this->wrapLayer(poLayer);

/* ------------------------------------------------------------------ */
/*                       Set other parameters.                        */
/* ------------------------------------------------------------------ */
    // Give a name to this DataSource.
    //pszName = pszFilename;
    pszName = CPLStrdup(pszFilename);

    // Initialize layers' id counter from begining. Here is not nessesery
    // to read it from table network_meta.
    //idCounter = 0;

/* ------------------------------------------------------------------ */
/*                   Synchronize changes with disk                    */
/* ------------------------------------------------------------------ */
    //if (geoDataSrc->SyncToDisk() != OGRERR_NONE)
        //return OGRERR_FAILURE;

    return OGRERR_NONE;
}


/************************************************************************/
/*                               getFeature()                           */
/************************************************************************/
OGRFeature* OGRGnmDataSource::getFeature(long globalFID)
{
    /*
    // Determine the layer which holds the feature with this unique ID.
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

    // Get the final feature from this layer.
    OGRLayer *poTargetLayer;
    poTargetLayer = this->geoDataSrc->GetLayerByName(layerName);
    if (poTargetLayer == NULL) return NULL;

    poTargetLayer->ResetReading();
    OGRFeature *poTargetFeature;
    poTargetFeature = poTargetLayer->GetFeature(nGFID);
    if (poTargetFeature == NULL) return NULL;

    return poTargetFeature;
    */

    // Determine the layer which holds the feature with this unique ID.
    OGRLayer *poLayer;
    poLayer = this->GetLayerByName("network_ids");
    poLayer->ResetReading();

    OGRFeature *poFeature;
    while((poFeature = poLayer->GetNextFeature()) != NULL)
    {
        if (poFeature->GetFieldAsInteger("id_global") == globalFID)
        {
            // Get the final feature from this layer.
            const char* layerName = poFeature->GetFieldAsString("layer_name");
            long layerId = poFeature->GetFieldAsInteger("id_local");
            OGRLayer *poTargetLayer = this->geoDataSrc->GetLayerByName(layerName);
            OGRFeature *poTargetFeature = poTargetLayer->GetFeature(layerId);
            return poTargetFeature;
        }
    }

    return NULL;
}


/************************************************************************/
/*                              wrapLayer()                             */
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


/************************************************************************/
/*                           unwrapLayer()                              */
/************************************************************************/
void OGRGnmDataSource::unwrapLayer (int iLayer)
{
    OGRGnmLayer* poLayerToDelete = (OGRGnmLayer*) papoLayers[iLayer];
    delete poLayerToDelete;
    int i = iLayer;
    while(i < nLayers - 1)
    {
        papoLayers[i] = papoLayers[i+1];
        i++;
    }
    nLayers--;
}


/************************************************************************/
/*                          getNextFeatureId()                          */
/************************************************************************/
long OGRGnmDataSource::getNextFeatureId()
{
    //long newId = idCounter;
    //idCounter = idCounter + 2;
    //return newId;

    //OGRLayer *l = this->GetLayerByName("network_meta");
    //OGRFeature *f = l->GetFeature(GNM_METADATA_ID_COUNTER);
    //long id = f->GetFieldAsInteger("value");
    //f->SetField("value", id + 1);
    //OGRFeature::DestroyFeature(f);
    //return id;

    // TODO: check for the maximum value of the id_counter.

    const char *idStr1 = this->getMetaParamValue("id_counter");
    long lastId = atol(idStr1);

    long newId = lastId + 1;

    char *s = (char*)malloc(sizeof(long)*8+1);
    const char *idStr2 = ltoa(newId, s, 10);
    this->setMetaParamValue("id_counter", idStr2);

    return lastId;
}


/************************************************************************/
/*                       getInnerDataSource()                           */
/************************************************************************/
OGRDataSource* OGRGnmDataSource::getInnerDataSource()
{
    return geoDataSrc;
}


/************************************************************************/
/*                            isSystemLayer()                           */
/************************************************************************/
bool OGRGnmDataSource::isSystemLayer(int iLayer)
{
    if (iLayer < 0 || iLayer >= nLayers) return false;

    const char *name = this->GetLayer(iLayer)->GetName();
    int i = 0;
    while (GNMSystemLayers[i] != NULL)
    {
        if (strcmp(name, GNMSystemLayers[i]) == 0)
            return true;
        i++;
    }
    return false;
}


/************************************************************************/
/*                            importLayer()                             */
/************************************************************************/
OGRErr OGRGnmDataSource::importLayer(const char *pszDataSrc, const char *pszLayer, char** papszOptions = NULL)
{
    OGRDataSource *sourceDS;
    sourceDS = OGRSFDriverRegistrar::Open(pszDataSrc, FALSE); // Open only for reading.
    if(sourceDS == NULL)
        return OGRERR_FAILURE;

    OGRLayer *sourceLayer;
    sourceLayer = sourceDS->GetLayerByName(pszLayer);
    if(sourceLayer == NULL)
        return OGRERR_FAILURE;

    // TODO: use papszOptins to select the way how data from the new layer must
    // be imported: only features with geometry or features with geometry and
    // attributes.
    //...

    // TODO: construct the special unique name for new layer, accordingly to the
    // special rules.
    //...

    const char *newLayerName = sourceLayer->GetName();

    // TODO: check for the appropriate GetGeomType() of source layer.

    // Create new layer with proxy and obligatory fields.
    OGRLayer *newLayer;
    newLayer = this->CreateLayer(newLayerName,
                                 NULL,//tempLayer->GetSpatialRef(),
                                 sourceLayer->GetGeomType(),
                                 NULL);
    if (newLayer == NULL)
        return OGRERR_FAILURE;

    // TODO: reproject geometry to this data source's coordinate system.
    //...

    // Copy the layer field defenitions from the source layer to the new layer.
    OGRFeatureDefn *poFDefn = sourceLayer->GetLayerDefn();
    int count = poFDefn->GetFieldCount();
    for (int i = 0; i < count; i++)
    {
        newLayer->CreateField(poFDefn->GetFieldDefn(i));
    }

    // Copy all source layer's features.
    OGRFeature *sourceFeature;
    sourceLayer->ResetReading();
    while((sourceFeature = sourceLayer->GetNextFeature()) != NULL)
    {
        // Create new feature with new layer definition.
        OGRFeature *newFeature = OGRFeature::CreateFeature(newLayer->GetLayerDefn());
        if (newFeature == NULL)
            return OGRERR_FAILURE;

        // Get the fields and geometry from the source feature.
        newFeature->SetFrom(sourceFeature);

        // Add a feature to the new layer, and set obligatory fields' default values.
        if(newLayer->CreateFeature(newFeature) != OGRERR_NONE)
            return OGRERR_FAILURE;

        OGRFeature::DestroyFeature(newFeature);
        OGRFeature::DestroyFeature(sourceFeature);
    }

    OGRDataSource::DestroyDataSource(sourceDS);
    return OGRERR_NONE;
}


/************************************************************************/
/*                        getMetaParamValue()                           */
/************************************************************************/
const char* OGRGnmDataSource::getMetaParamValue(const char *paramName)
{
    OGRLayer *poLayer = this->GetLayerByName("network_meta");
    OGRFeature *poFeature;
    const char *res;
    for (int i = 0; i < poLayer->GetFeatureCount(); i++)
    {
        poFeature = poLayer->GetFeature(i);
        const char *pName = poFeature->GetFieldAsString("param_name");
        if (strcmp(pName, paramName) == 0)
        {
            res = poFeature->GetFieldAsString("value");
            // TODO: understand, if it is ok not to destroy feature here.
            //OGRFeature::DestroyFeature(poFeature);
            return res;
        }
       // OGRFeature::DestroyFeature(poFeature);
    }
    return "";
}


/************************************************************************/
/*                        setMetaParamValue()                           */
/************************************************************************/
void OGRGnmDataSource::setMetaParamValue(const char *paramName, const char *paramValue)
{
    OGRLayer *poLayer = this->GetLayerByName("network_meta");
    OGRFeature *poFeature;
    for (int i = 0; i < poLayer->GetFeatureCount(); i++)
    {
        poFeature = poLayer->GetFeature(i);
        const char *pName = poFeature->GetFieldAsString("param_name");
        if (strcmp(pName,paramName) == 0)
        {
            poFeature->SetField("value", paramValue);
            OGRErr err = poLayer->SetFeature(poFeature);
            //poLayer->SyncToDisk();
            OGRFeature::DestroyFeature(poFeature);
            break;
        }
        OGRFeature::DestroyFeature(poFeature);
    }
}


/************************************************************************/
/*                           Constructor                                */
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


/************************************************************************/
/*                           Destructor                                 */
/************************************************************************/
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


/************************************************************************/
/*                       GetName()                                      */
/************************************************************************/
const char* OGRGnmDataSource::GetName()
{
    return pszName;
}


/************************************************************************/
/*                       GetLayerCount()                                */
/************************************************************************/
int OGRGnmDataSource::GetLayerCount()
{
    //return this->geoDataSrc->GetLayerCount();

    return nLayers;
}


/************************************************************************/
/*                       GetLayer()                                     */
/************************************************************************/
OGRLayer* OGRGnmDataSource::GetLayer(int iLayer)
{
    //return this->geoDataSrc->GetLayer(index);

    if(iLayer < 0 || iLayer >= nLayers) return NULL;
    else return papoLayers[iLayer];
}


/************************************************************************/
/*                          GetLayerByName()                            */
/************************************************************************/
OGRLayer* OGRGnmDataSource::GetLayerByName(const char *name)
{
    //return this->geoDataSrc->GetLayerByName(name);

    const char *str;
    //int nom;
    //bool fl = false;
    for (int i=0; i<nLayers; i++)
    {
        str = papoLayers[i]->GetName();
        if (strcmp(str, name) == 0) // strcmp is also used in some other drivers.
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


/************************************************************************/
/*                          TestCapability()                            */
/************************************************************************/
int OGRGnmDataSource::TestCapability(const char *)
{
    // TODO: remove this stub.
    return FALSE;
}


/************************************************************************/
/*                           CreateLayer()                              */
/************************************************************************/
OGRLayer* OGRGnmDataSource::CreateLayer(const char *pszName, OGRSpatialReference *poSpatialRef,
                                        OGRwkbGeometryType eGType, char **papszOptions)
{
    // TODO: use srs from the network_meta table.
    OGRSpatialReference *poSR = NULL;

    // Check for available geometry types.
    // The list of types is about to be widen in future.
    if (eGType != wkbPoint && eGType != wkbLineString)
        return NULL;

    OGRLayer *geoLayer;
    geoLayer = this->geoDataSrc->CreateLayer(pszName, poSR, eGType, papszOptions);
    if (geoLayer == NULL)
        return NULL;

    // Add obligatory fields to the new layer: blocking state and direction.
    OGRFieldDefn *oField;
    oField = new OGRFieldDefn("is_blocked", OFTInteger);
    if(geoLayer->CreateField(oField) != OGRERR_NONE)
        return NULL;

    oField = new OGRFieldDefn("direction", OFTInteger);
    if(geoLayer->CreateField(oField) != OGRERR_NONE)
        return NULL;

    // Wrap a special format layer with our format and return it.
    return wrapLayer(geoLayer);
}


/************************************************************************/
/*                          SyncToDisk()                                */
/************************************************************************/
OGRErr OGRGnmDataSource::SyncToDisk()
{
    // SyncToDisk does not call when DestroyDatasource calls.
    return geoDataSrc->SyncToDisk();
}


/************************************************************************/
/*                          DeleteLayer()                               */
/************************************************************************/
OGRErr OGRGnmDataSource::DeleteLayer(int iLayer)
{
    // TODO: check if this layer is not a system layer. If so, delete it
    // accordingly. Overwise do nothing.

    // Delete the pointer from papoLayers.
    this->unwrapLayer(iLayer);

    // TODO: delete all corresponding features from network_ids.

    // TODO: delete all corresponding features from network_graph.

    // Delete real layer.
    return geoDataSrc->DeleteLayer(iLayer);
}











