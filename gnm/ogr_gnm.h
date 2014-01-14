#ifndef OGR_GNM_H
#define OGR_GNM_H

#include "gnmcore.h"


//зачем нужен этот класс, если работа всё равно идёт со слоями внутреннего ДатаСорса?
//если он необходим для встраивания в ОГР, тогда его надо реализовать как заглушку
class OGRGnmLayer : public OGRLayer
{
    private:

     //хранит указатель на слой заданного формата, с которым будет производиться операция???
     //OGRLayer *geoLayer;

    public:

    /*
    //обязательные
    OGRGnmLayer (const char *pszFilename);
    ~OGRGnmLayer ();
    void ResetReading ();
    OGRFeature *GetNextFeature ();
    OGRFeatureDefn *GetLayerDefn ();
    int TestCapability (const char *);

    //дополнительные, которые надо реализовать
    OGRErr SetFeature (OGRFeature *poFeature);
    OGRErr CreateFeature (OGRFeature *poFeature);
    */

};


class OGRGnmDataSource : public OGRDataSource
{
    private:

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

     NErr open(const char *pszFilename, int bUpdate, char **papszOptions);

     NErr create(const char *pszFilename, char **papszOptions);

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

//обязательные-----------------------------
     const char *GetName ();

     int GetLayerCount ();

     OGRLayer *GetLayer (int nLayer);

     int TestCapability (const char *);
//------------------------------------------

//дополнительно-----------------------------
     OGRLayer *GetLayerByName (const char *name);

     OGRLayer *CreateLayer 	(const char *pszName, OGRSpatialReference *poSpatialRef,
                             OGRwkbGeometryType eGType, char **papszOptions);
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
