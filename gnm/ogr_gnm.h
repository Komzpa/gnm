#ifndef OGR_GNM_H
#define OGR_GNM_H

#include "gnmcore.h"


//*****************************************************************************
//*************************** Надстройка над OGR ******************************
//*****************************************************************************
// В модуле ogr_gnm.h расположены только наследники от классов OGR.
// Методы, создающие объекты этих классов (т.е. формирующие сеть) находятся в
// gnmmanager.h, а методы по анализу созданной сети в gnmanalysis.h.


class OGRGnmLayer : public OGRLayer
{
    private:

     //хранит указатель на слой заданного формата, с которым будет производиться операция
     OGRLayer *geoLayer;

    public:

};


class OGRGnmDataSource : public OGRDataSource
{
    private:

     //имя данного источника данных
     char *pszName;

     //три основныхсистемных слоя
     //OGRGnmLayer* network_meta;
     //OGRGnmLayer* network_graph;
     //OGRGnmLayer* network_rules;

     //текущий формат сети (OGR-определённая строка)
     char *formatName;

     //основной ДатаСорс, хранящий гео данные в заданном формате
     OGRDataSource* geoDataSrc;

     //счётчик последнего выданного id объекту
     long idCounter;
     //метод, выдающий новый глобальный id объекту исходя из счётчика
     long getNewId();

     //таблица связей объектов (граф), где элементом со связями представлен каждый объект сети
     //std::vector<NElement> graph;

    public:

    //собственные методы

     //из туториала:
     //The constructor is a simple initializer to a default state.
     //The Open() will take care of actually attaching it to a file.
     //The destructor is responsible for orderly cleanup of layers.

     OGRGnmDataSource();
     ~OGRGnmDataSource();

     //открытие источника данных
     // - сетевые данные
     // - география в заданном формате
     NErr open(const char *pszFilename, int bUpdate, char **papszOptions);

     //создание
     NErr create(const char *pszFilename, char **papszOptions);

     //соединяет два объекта, добавляя связи в массив, причём
     // - перед этим смотрит правила соединения
     // - после этого смотрит правила поведения
     // - после этого смотрит правила влияния
     //NErr connect(long id1, long id2);

     //разъединяет два объекта (только при изменении правил)
     // - после этого смотрит правила поведения
     // - после этого смотрит правила влияния
     //void disconnect(long id1, long id2);

    //здесь должны быть только те методы, которые объявлены в абстрактном OGRDataSource

     //новый слой создаётся в geoDataSrc, причём:
     // - устанавливается уникальный id при помощи OGRFeature::SetFID()
     // - добавляются поля по умолчанию (блокировка, направление)
     //OGRLayer *CreateLayer(const char *pszName, OGRSpatialReference *poSpatialRef = NULL,
                           //OGRwkbGeometryType eGType = wkbUnknown,
                           //char ** papszOptions = NULL);

     //сохраняются сетевые данные и данные из geoDataSrc
     //OGRErr SyncToDisk();

     //запрос выполняется к geoDataSrc
     //OGRLayer *ExecuteSQL(const char *pszStatement, OGRGeometry *poSpatialFilter,
                            //const char *pszDialect);

     //методы по работе со слоями надо просто передать на выполнение в geoDataSrc

     const char *GetName();

     int GetLayerCount();

     OGRLayer *GetLayer(int nLayer);

     //проверяет возможности как себя, так и geoDataSrc
     int TestCapability(const char *);
};


class OGRGnmDriver : public OGRSFDriver
{
    public:

     ~OGRGnmDriver();

     const char *GetName();

     OGRDataSource *Open(const char *pszFilename, int bUpdate, char **papszOptions);

     OGRDataSource *Open(const char *pszFilename, int bUpdate);

     OGRDataSource *CreateDataSource(const char *pszName, char **papszOptions);

     OGRErr DeleteDataSource(const char *pszName);

     int TestCapability(const char* pszCap);

     //TODO: RegisterOGRGnm();
};


#endif // OGR_GNM_H
