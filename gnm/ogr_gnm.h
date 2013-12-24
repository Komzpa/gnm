#ifndef OGR_GNM_H
#define OGR_GNM_H

#include "gnmcore.h"

//*****************************************************************************
//*************************** Надстройка над OGR ******************************
//*****************************************************************************
// В модуле ogr_gnm.h расположены только наследники от классов OGR.
// Методы, создающие объекты этих классов (т.е. формирующие сеть) находятся в
// gnmmanager.h, а методы по анализу созданной сети в gnmanalysis.h.


// Базовые объявления находятся в ogrsf_frmts.h


// Вопросы
// 1. Наследование от OGR означает только реализацию виртуальных методов, объявленных
//    в абстрактных классах. Как вызывать методы, специфические именно для наследников?
//    Возможно: всю работу организовать в некой библиотеке (gnmmanager.h)


class OGRGnmLayer : public OGRLayer //: public OGRAbstractProxiedLayer
{
    private:

     //хранит указатель на слой заданного формата, с которым будет производиться операция
     OGRLayer *geoLayer;

    public:

     //...
     virtual OGRErr CreateField (...);

     //...
     virtual OGRErr CreateFeature (...);

     //...
     virtual OGRFeature *GetFeature (...);
};

class OGRGnmDataSource : public OGRDataSource
{
    private:

     //текущий формат сети (OGR-определённая строка)
     const char *formatName;

     //основной ДатаСорс, хранящий гео данные в заданном формате
     OGRDataSource* geoDataSrc;

     //счётчик последнего выданного id объекту
     long idCounter;
     //метод, выдающий новый глобальный id объекту исходя из счётчика
     long getNewId();

     //таблица связей объектов (граф), где элементом со связями представлен каждый объект сети
     std::vector<NElement> graph;

     //соединяет два объекта, добавляя связи в массив, причём
     // - перед этим смотрит правила соединения
     // - после этого смотрит правила поведения
     // - после этого смотрит правила влияния
     NErr connect(long id1, long id2);

     //разъединяет два объекта (только при изменении правил)
     // - после этого смотрит правила поведения
     // - после этого смотрит правила влияния
     void disconnect(long id1, long id2);

    public:

    //здесь должны быть только те методы, которые объявлены в абстрактном OGRDataSource

     //здесь также инициализируем и удаляем geoDataSrc в зависимости от выбранного формата
     OGRGnmDataSource();
     ~OGRGnmDataSource();

     //новый слой создаётся в geoDataSrc, причём:
     // - устанавливается уникальный id при помощи OGRFeature::SetFID()
     // - добавляются поля по умолчанию (блокировка, направление)
     virtual OGRLayer *CreateLayer(...);

     //сохраняются сетевые данные и данные из geoDataSrc
     virtual OGRErr SyncToDisk(...);

     //запрос выполняется к geoDataSrc
     virtual OGRLayer *ExecuteSQL(...);

     //проверяет возможности как себя, так и geoDataSrc
     virtual int TestCapability(...);

     //методы по работе со слоями надо просто передать на выполнение в geoDataSrc
     virtual int GetLayerCount(...);
     virtual OGRLayer *GetLayer(...);
     virtual OGRLayer *GetLayerByName(...);
};

class OGRGnmkDriver : public OGRSFDriver
{
    public:

     ~OGRGnmDriver();

     //создаём источник данных, открывая файлы/бд нужного формата и подгружая к ним сетевые данные, причём:
     // - проверять на соответствие версию формата сети
     OGRDataSource *Open(...);

     //создаёт OGRNetworkDataSource и вызывает CreateDataSource() для заданного формата
     virtual OGRDataSource *CreateDataSource(...);

};



#endif // OGR_GNM_H
