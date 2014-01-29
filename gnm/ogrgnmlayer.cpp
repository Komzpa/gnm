#include "ogr_gnm.h"

OGRGnmLayer::OGRGnmLayer(OGRLayer *mainLayer, OGRGnmDataSource *parentDataSource)
{
    geoLayer = mainLayer;
    parentDataSrc = parentDataSource;
}

OGRGnmLayer::~OGRGnmLayer ()
{
    // We must not delete the geoLayer itself here.
}

void OGRGnmLayer::ResetReading ()
{
    geoLayer->ResetReading();
}

OGRFeature* OGRGnmLayer::GetNextFeature ()
{
    return geoLayer->GetNextFeature();
}

OGRFeatureDefn* OGRGnmLayer::GetLayerDefn ()
{
    return geoLayer->GetLayerDefn();
}

int OGRGnmLayer::TestCapability (const char *)
{
    return FALSE;
}


/************************************************************************/
/*                            CreateField()                             */
/************************************************************************/
OGRErr OGRGnmLayer::CreateField(OGRFieldDefn *poField, int bApproxOK)
{
    return geoLayer->CreateField(poField, bApproxOK);
}


/************************************************************************/
/*                            SetFeature()                              */
/************************************************************************/

OGRErr OGRGnmLayer::SetFeature (OGRFeature *poFeature)
{
    return geoLayer->SetFeature(poFeature);
}


/************************************************************************/
/*                          CreateFeature()                             */
/************************************************************************/
OGRErr OGRGnmLayer::CreateFeature (OGRFeature *poFeature)
{
    // Check for obligatory attributes' correct values.
    int temp;
    temp = poFeature->GetFieldAsInteger("is_blocked");
    if (temp != GNM_FEATURE_BLOCKED &&
        temp != GNM_FEATURE_UNBLOCKED)
    {
        // Set initial value if incoming value is incorrect.
        poFeature->SetField("is_blocked",GNM_FEATURE_UNBLOCKED);
    }
    temp = poFeature->GetFieldAsInteger("direction");
    if (temp != GNM_FEATURE_STRAIGHT_DIRECTION &&
        temp != GNM_FEATURE_REVERSE_DIRECTION &&
        temp != GNM_FEATURE_DOUBLE_DIRECTION)
    {
        // Set initial value if incoming value is incorrect.
        poFeature->SetField("direction",GNM_FEATURE_DOUBLE_DIRECTION);
    }

    /*
    // Set feature unique ID.
    long id = parentDataSrc->getNextFeatureId();
    OGRErr err = poFeature->SetFID(id);
    poFeature->SetFID(id);

    // Add feature's id and layer name to the id relation table.
    OGRLayer *poLr = parentDataSrc->getInnerDataSource()->GetLayerByName("network_ids");
    OGRFeature *poFt;
    poFt = OGRFeature::CreateFeature(poLr->GetLayerDefn());
    const char* layerName = this->GetName();
    poFt->SetField("layer_name", layerName);
    poFt->SetFID(id);
    if(poLr->CreateFeature(poFt) != OGRERR_NONE) return OGRERR_FAILURE;
    */

    // Create feature.
    OGRErr err = geoLayer->CreateFeature(poFeature);
    if (err != OGRERR_NONE)
        return OGRERR_FAILURE;

    // Add to relation table info about new feature.
    const char* lrNm = this->GetName();
    long ftGId = parentDataSrc->getNextFeatureId();
    long ftLId = poFeature->GetFID();
    OGRLayer *poLr = parentDataSrc->getInnerDataSource()->GetLayerByName("network_ids");
    OGRFeature *poFt;
    poFt = OGRFeature::CreateFeature(poLr->GetLayerDefn());
    poFt->SetField("id_global", ftGId);
    poFt->SetField("layer_name", lrNm);
    poFt->SetField("id_local", ftLId);
    if(poLr->CreateFeature(poFt) != OGRERR_NONE)
        return OGRERR_FAILURE;

    OGRFeature::DestroyFeature(poFt);
    return OGRERR_NONE;
}


/************************************************************************/
/*                         GetFeature()                                 */
/************************************************************************/
OGRFeature* OGRGnmLayer::GetFeature(long nFID)
{
    return geoLayer->GetFeature(nFID);
}


/************************************************************************/
/*                          DeleteFeature()                             */
/************************************************************************/
OGRErr OGRGnmLayer::DeleteFeature(long nFID)
{
    // TODO: delete corresponding features from system tables.
    //...

    return geoLayer->DeleteFeature(nFID);
}


/************************************************************************/
/*                          SyncToDisk()                                */
/************************************************************************/
OGRErr OGRGnmLayer::SyncToDisk()
{
    return geoLayer->SyncToDisk();
}


/************************************************************************/
/*                          getInnerLayer()                             */
/************************************************************************/
OGRLayer* OGRGnmLayer::getInnerLayer()
{
    return geoLayer;
}




