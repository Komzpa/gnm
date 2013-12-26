
#include "ogr_gnm.h"


OGRGnmDriver::~OGRGnmDriver()
{
}


const char* OGRGnmDriver::GetName()
{
    return "Geographic Network Model";
}


OGRDataSource* OGRGnmDriver::Open(const char* pszFilename, int bUpdate, char** papszOptions)
{
    OGRGnmDataSource *poDS = new OGRGnmDataSource();

    //TODO: брать из papszOptions формат, при помощи которого в OGRGnmDataSource создавать
    //внутренний источник данных

    char *test = "test";
    if(poDS->open(pszFilename, bUpdate, &test) != NERR_NONE)
    {
        delete poDS;
        return NULL;
    }
    else
        return poDS;
}


OGRDataSource* OGRGnmDriver::Open(const char* pszFilename, int bUpdate)
{
    return Open(pszFilename,bUpdate,NULL);
}


OGRDataSource* OGRGnmDriver::CreateDataSource(const char *pszName, char **papszOptions)
{
    //TODO: брать из papszOptions формат, при помощи которого в OGRGnmDataSource создавать
    //внутренний источник данных

    OGRGnmDataSource *poDS = new OGRGnmDataSource();
    char *test = "test";
    if(poDS->create(pszName, &test) != NERR_NONE)
    {
        delete poDS;
        return NULL;
    }
    else
        return poDS;
}


OGRErr OGRGnmDriver::DeleteDataSource(const char *pszName)
{
    //TODO: разобраться, что удалять

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
