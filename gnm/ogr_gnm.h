#ifndef OGR_GNM_H
#define OGR_GNM_H

#include "gnmcore.h"

#define GNM_FEATURE_BLOCKED 1
#define GNM_FEATURE_UNBLOCKED 0

#define GNM_FEATURE_STRAIGHT_DIRECTION 1
#define GNM_FEATURE_REVERSE_DIRECTION 0
#define GNM_FEATURE_DOUBLE_DIRECTION 2


//реализация этого класса нужна для того, чтобы перехватывать методы по
//созданию объектов в слоях
//Собственно, этот класс нужен, чтобы перехватить SetFeature и CreateFeature
class OGRGnmLayer : public OGRLayer
{
    private:

     //хранит указатель на слой заданного формата, с которым будет
     //производиться все операции
     OGRLayer *geoLayer;

    public:

    //обязательные
    OGRGnmLayer (OGRLayer *mainLayer);
    ~OGRGnmLayer ();
    void ResetReading ();
    OGRFeature *GetNextFeature ();
    OGRFeatureDefn *GetLayerDefn ();
    int TestCapability (const char *);

    //дополнительные, которые надо реализовать
    OGRErr CreateField (OGRFieldDefn *poField, int bApproxOK);
    OGRErr SetFeature (OGRFeature *poFeature);
    OGRErr CreateFeature (OGRFeature *poFeature);

};


class OGRGnmDataSource : public OGRDataSource
{
    private:

     //массив указателей на собственные слои, где каждый слой содержит указатель на среальный слой
     OGRGnmLayer **papoLayers;
     int nLayers;

     //имя данного источника данных
     char *pszName;

     //основной ДатаСорс, хранящий гео данные в заданном формате
     OGRDataSource* geoDataSrc;

     //счётчик последнего выданного id объекту
     //long idCounter;

     //метод, выдающий новый глобальный id объекту исходя из счётчика
     //long getNewId();

     //таблица связей объектов (граф), где элементом со связями представлен каждый объект сети
     //std::vector<NElement> graph;

    public:

     //из туториала:
     //The constructor is a simple initializer to a default state.
     //The Open() will take care of actually attaching it to a file.
     //The destructor is responsible for orderly cleanup of layers.

     OGRGnmDataSource();
     ~OGRGnmDataSource();

//свои ------------------------------
     NErr open(const char *pszFilename, int bUpdate, char **papszOptions);

     NErr create(const char *pszFilename, char **papszOptions);

     OGRGnmLayer *addLayer(OGRLayer *layer);

     //соединяет два объекта, добавляя связи в массив, причём
     // - перед этим смотрит правила соединения
     // - после этого смотрит правила поведения
     // - после этого смотрит правила влияния
     //NErr connect(long nGFID1, long nGFID2);

     //разъединяет два объекта (только при изменении правил)
     // - после этого смотрит правила поведения
     // - после этого смотрит правила влияния
     //void disconnect(long nGFID1, long nGFID2);

     //void blockFeature(long nGFID);

     //void unblockFeature(long nGFID);

     //void setFeatureDirection(long nGFID, char direction);
//-----------------------------------------

//обязательные-----------------------------
     const char *GetName ();

     int GetLayerCount ();

     OGRLayer *GetLayer (int iLayer);

     int TestCapability (const char *);
//------------------------------------------

//дополнительно-----------------------------
     OGRLayer *GetLayerByName (const char *name);

     OGRLayer *CreateLayer 	(const char *pszName, OGRSpatialReference *poSpatialRef,
                             OGRwkbGeometryType eGType, char **papszOptions);

     OGRErr SyncToDisk ();
//------------------------------------------

};


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
