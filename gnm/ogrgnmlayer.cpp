
#include "ogr_gnm.h"

OGRGnmLayer::OGRGnmLayer(OGRLayer *mainLayer)
{
    geoLayer = mainLayer;
}

OGRGnmLayer::~OGRGnmLayer ()
{
    //здесь нельзя удалять сам geoLayer, т.к. это только ещё одна ссылка на слой
    //CPLFree(geoLayer);
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
/*                       Create new field                               */
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
/*                       Create new feature in layer                    */
/************************************************************************/

OGRErr OGRGnmLayer::CreateFeature (OGRFeature *poFeature)
{
    // Check for obligatory attributes' correct values

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

    // TODO: set feature unic ID via OGRLayer::GetFIDColumn() or OGRFeature::SetFID()


    // Create feature.
    return geoLayer->CreateFeature(poFeature);
}




