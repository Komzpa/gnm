
#include "ogr_gnm.h"


OGRGnmDriver::~OGRGnmDriver()
{
}


const char* OGRGnmDriver::GetName()
{
    return "Geographic Network Model";
}


OGRDataSource* OGRGnmDriver::Open(const char* pszFilename, int bUpdate)
{
    OGRGnmDataSource *poDS = new OGRGnmDataSource();

    if(poDS->open(pszFilename, bUpdate, NULL) != OGRERR_NONE)
    {
        delete poDS;
        return NULL;
    }
    else
    {
        return poDS;
    }
}


OGRDataSource* OGRGnmDriver::CreateDataSource(const char *pszName, char **papszOptions)
{
    OGRGnmDataSource *poDS = new OGRGnmDataSource();

    if(poDS->create(pszName, papszOptions) != OGRERR_NONE)
    {
        delete poDS;
        return NULL;
    }
    else
        return poDS;
}


OGRErr OGRGnmDriver::DeleteDataSource(const char *pszName)
{ 
    // TODO: test capability to delete a particular layer format.

    OGRGnmDataSource *poDS;
    poDS = (OGRGnmDataSource *) this->Open(pszName, TRUE);
    if(poDS == NULL)
    {
        return OGRERR_FAILURE;
    }

    // Firstly delete all user layers, because they may have features,
    // which ids are stored in system layers.

    int i = 0;
    while (poDS->GetLayerCount() > GNMSystemLayersCount)
    {
        if (!poDS->isSystemLayer(i))
        {
           OGRErr err = poDS->DeleteLayer(i);
           if (err != OGRERR_NONE) return OGRERR_FAILURE;
        }
        else i++;
    }

    // Than delete system layers.
    // In this place we know, that system layers exist, because it was checked
    // during the opening of data source.
    do
    {
        i = poDS->GetLayerCount() - 1;
        poDS->unwrapLayer(i);
        OGRErr err = poDS->getInnerDataSource()->DeleteLayer(i);
        if (err != OGRERR_NONE) return OGRERR_FAILURE;
    }
    while (i > 0);

    OGRDataSource::DestroyDataSource(poDS);
    return OGRERR_NONE;
}


int OGRGnmDriver::TestCapability(const char* pszCap)
{
    if( EQUAL( pszCap, ODrCCreateDataSource ) )
        return TRUE;
    else if( EQUAL(pszCap, ODrCDeleteDataSource) )
        return TRUE;
    else
        return FALSE;
}
