
#include "ogr_gnm.h"

OGRGnmLayer::OGRGnmLayer(OGRLayer *mainLayer)
{
    geoLayer = mainLayer;
}

OGRGnmLayer::~OGRGnmLayer ()
{
    CPLFree(geoLayer);
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
    // TODO: check for obligatory attributes' values
    // (if it is possible to set them this way).

    // TODO: set feature unic ID via OGRLayer::GetFIDColumn()

    // TODO: set obligatory attributes' initial values.

    return OGRERR_NONE;
}




