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
/*                       Rewrite existing feature                       */
/************************************************************************/

OGRErr OGRGnmLayer::SetFeature (OGRFeature *poFeature)
{
    // TODO: check for obligatory attributes' values
    // (if it is possible to set them this way).

    return OGRERR_NONE;
}


/************************************************************************/
/*                          CreateFeature()                             */
/************************************************************************/
OGRErr OGRGnmLayer::CreateFeature (OGRFeature *poFeature)
{
    // Check for obligatory attributes' correct values.
    int temp;
    temp = poFeature->GetFieldAsInteger("is_blocked");
    if (temp != GNM_FEATURE_BLOCKED
     && temp != GNM_FEATURE_UNBLOCKED)
    {
        // Set initial value if incoming value is incorrect.
        poFeature->SetField("is_blocked",GNM_FEATURE_UNBLOCKED);
    }
    temp = poFeature->GetFieldAsInteger("direction");
    if (temp != GNM_FEATURE_STRAIGHT_DIRECTION
     && temp != GNM_FEATURE_REVERSE_DIRECTION
     && temp != GNM_FEATURE_DOUBLE_DIRECTION)
    {
        // Set initial value if incoming value is incorrect.
        poFeature->SetField("direction",GNM_FEATURE_DOUBLE_DIRECTION);
    }

    // Set feature unique ID.
    long id = parentDataSrc->getNextFeatureId();
    poFeature->SetFID(id);

    // Add feature's id and layer name to the id ownership table.
    OGRLayer *poLr = parentDataSrc->getInnerDataSource()->GetLayerByName("network_ids");
    if (poLr == NULL) return OGRERR_FAILURE;
    OGRFeature *poFt;
    poFt = OGRFeature::CreateFeature(poLr->GetLayerDefn());
    const char* layerName = this->GetName();
    poFt->SetField("layer_name", layerName);
    poFt->SetFID(id);
    if(poLr->CreateFeature(poFt) != OGRERR_NONE) return OGRERR_FAILURE;

    // Create feature.
    return geoLayer->CreateFeature(poFeature);
}




