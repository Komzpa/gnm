#ifndef OGR_GNM_H
#define OGR_GNM_H

#include "ogrsf_frmts.h"


#define GNM_FEATURE_BLOCKED 1
#define GNM_FEATURE_UNBLOCKED 0

#define GNM_FEATURE_STRAIGHT_DIRECTION 1
#define GNM_FEATURE_REVERSE_DIRECTION 0
#define GNM_FEATURE_DOUBLE_DIRECTION 2

#define GNM_METADATA_FORMAT_VERSION 0
#define GNM_METADATA_SRS 1
#define GNM_METADATA_ID_COUNTER 2

#define GNM_OPTION_DRIVER_NAME "drvname"


// Available supported drivers. The list must end with NULL.
static const char *GNMSupportedDrivers[] = {
    "ESRI Shapefile",
    "GeoJSON",
    "PostgreSQL/PostGIS",
    "SQLite",
    NULL };

// Names of all system layers. The list must end with NULL.
static const char *GNMSystemLayers[] = {
    "network_meta",
    "network_graph",
    "network_rules",
    "network_ids",
    NULL };

// Count of GNMSystemLayers array's elements.
static int GNMSystemLayersCount = 4;


class OGRGnmDataSource;


/************************************************************************/
/*                            OGRGnmLayer                               */
/************************************************************************/
//Behaves like a proxy to the real layer with geometry and attributes
class OGRGnmLayer : public OGRLayer
{
    private:

     // Parent data source.
     OGRGnmDataSource *parentDataSrc;

     // Pointer to the real layer.
     OGRLayer *geoLayer;

    public:

    OGRGnmLayer (OGRLayer *mainLayer, OGRGnmDataSource *parentDataSource);

    ~OGRGnmLayer ();

//required----------------------------------------
    void ResetReading ();

    OGRFeature *GetNextFeature ();

    OGRFeatureDefn *GetLayerDefn ();

    int TestCapability (const char *);
//------------------------------------------------

//additional--------------------------------------
    OGRErr CreateField (OGRFieldDefn *poField, int bApproxOK);

    OGRErr SetFeature (OGRFeature *poFeature);

    OGRErr CreateFeature (OGRFeature *poFeature);
//-------------------------------------------------

};


/************************************************************************/
/*                            OGRGnmDataSource                          */
/************************************************************************/
class OGRGnmDataSource : public OGRDataSource
{
    private:

     // The name of this data source (not inner).
     char *pszName;

     OGRGnmLayer **papoLayers;
     int nLayers;

     // Inner data source, which stores all layers in given format.
     OGRDataSource* geoDataSrc;

     long idCounter;

    public:

     OGRGnmDataSource ();

     ~OGRGnmDataSource ();

//new----------------------------------
     // Open an existing data source.
     OGRErr open (const char *pszFilename, int bUpdate, char **papszOptions);

     // Create new data source.
     OGRErr create (const char *pszFilename, char **papszOptions);

     // Wraps the OGRLayer layer with proxy OGRGnmLayer.
     OGRGnmLayer *wrapLayer (OGRLayer *layer);

     // Delete the proxy.
     void unwrapLayer (int iLayer);

     // Get the feature from the whole data source.
     OGRFeature *getFeature (long nGFID);

     // Returns geoDataSrc.
     OGRDataSource *getInnerDataSource ();

     // Returns true, if the given layer is one of the system layers.
     // List of system layers is stored in GNMSystemLayers[].
     bool isSystemLayer (int iLayer);

     // Get next id for new feature.
     long getNextFeatureId ();

     //NErr connect(long nGFID1, long nGFID2);

     //void disconnect(long nGFID1, long nGFID2);

     //void blockFeature(long nGFID);

     //void unblockFeature(long nGFID);

     //void setFeatureDirection(long nGFID, char direction);
//-----------------------------------------

//required----------------------------------
     const char *GetName ();

     int GetLayerCount ();

     OGRLayer *GetLayer (int iLayer);

     int TestCapability (const char *);
//------------------------------------------

//additional--------------------------------
     OGRLayer *GetLayerByName (const char *name);

     OGRLayer *CreateLayer 	(const char *pszName, OGRSpatialReference *poSpatialRef,
                             OGRwkbGeometryType eGType, char **papszOptions);

     OGRErr SyncToDisk ();

     OGRErr DeleteLayer (int iLayer);
//------------------------------------------

};


/************************************************************************/
/*                            OGRGnmDriver                              */
/************************************************************************/
class OGRGnmDriver : public OGRSFDriver
{
    public:

     ~OGRGnmDriver();

     const char *GetName();

     OGRDataSource *Open(const char *pszFilename, int bUpdate);

     OGRDataSource *CreateDataSource(const char *pszName, char **papszOptions);

     OGRErr DeleteDataSource(const char *pszName);

     int TestCapability(const char* pszCap);

     //TODO: RegisterOGRGnm();
};


#endif // OGR_GNM_H
