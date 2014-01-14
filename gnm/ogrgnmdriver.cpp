
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

    if(poDS->open(pszFilename, bUpdate, NULL) != NERR_NONE)
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

    if(poDS->create(pszName, papszOptions) != NERR_NONE)
    {
        delete poDS;
        return NULL;
    }
    else
        return poDS;
}


OGRErr OGRGnmDriver::DeleteDataSource(const char *pszName)
{
    // TODO: understand what to delete.
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
