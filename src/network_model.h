#ifndef NETWORK_MODEL_H
#define NETWORK_MODEL_H

#include <utility> //std::pair, std::make_pair
#include <string>
#include <vector>

#include "ogrsf_frmts.h"

//TODO: расписать коды ошибок
#define GnmOk 0 //выполнилось без ошибок
#define GnmErr 1 //выполнилось с ошибками

//typedef std::pair <std::string, std::string> NtwStringPair; //для краткости

// **************************************************************
// ***************** Сетевая надстройка *************************
// **************************************************************

/*
class NtwObject
{
    private:
     long Id; //глобальный id объекта для связи с соответствующим OGRFeature
    public:
     NtwObject(long featureId);
     long getId();
};

class NtwPoint: public NtwObject
{
    private:
     std::vector<long> incLIds;
    public:
     NtwPoint(long featureId); //создаёт отдельностоящуюю вершину
     void appendLine(long incomingLineId); //присоединяет к вершине ребро
};

class NtwLine: public NtwObject
{
    private:
     long sPId;
     long ePId;
    public:
     NtwLine(long featureId, long startPointId, long endPointId);
};

//граф связности объектов
class NtwGraph
{
    private:
     std::vector<NtwPoint> junctions;
     std::vector<NtwLine> edges;
    public:
     NtwGraph(); //создаёт пустой граф
     int addEdge(long featureId, long startPointId, long endPointId); //добавляем ребро и его связи
     int addJunction(long featureId); //добавляем вершину (связи к ней пристыковываются потом)
};

*/


// **************************************************************
// ***************** Главный класс сети *************************
// **************************************************************
// объединяет OGR и сеть
/*
class NtwNetwork
{
    private:
     std::vector<NtwStringPair> nAttribs; //атрибуты самой сети

     //NtwDataCache nDataCache; //кэш объектов сети, находящихся в памяти
     //NtwData nData; //все объекты сети
     //NtwGraph nGraph; //связи всех объектов
     //TEMP------------------------------
     OGRSFDriver* pDriver;
     OGRDataSource* pDataSrc;
     //----------------------------------

    public:
     NtwNetwork();
     ~NtwNetwork();

     //...

     void addAttrib(std::string name);
     void setAttribValue(int i, std::string value);
     void setAttribValue(std::string name, std::string value);
     NtwStringPair getAttrib(int i);
     int getAttribCount();
};
*/

// **************************************************************
// ********************** Новый вариант *************************
// **************************************************************


//Проблемы:
//1. Если использовать только атрибуты OGR для полей объектов и связности, то получается,
//   что id объекта будет не long, а int
//2. Если задавать значение атрибута через универсальный указатель и единым методом, то
//   нельзя будет внутри проверить соответствие передаваемого типа, типу атрибута (т.к.
//   используем reinterpret_cast)
//3. Как кэшировать данные, если SQL-запрос идёт уже к готовому OGRDataSource?
//
//
//
//

struct GnmPoint
{
    long objId;
    std::vector<long> incLIds;
};
struct GnmLine
{
    long objId;
    long startPId;
    long endPId;
};

class GnmNetwork
{
    private:

     OGRSFDriver* driver;
     OGRDataSource* dataSrc; //нужен для сохранения, загрузки, SQL-запросов

     //связи объектов
     //TODO: возможно организовать связи в отдельный OGRLayer с геометрией wkbNone
     std::vector<GnmPoint> points;
     std::vector<GnmLine> lines;

    protected:
     //TODO: возможно перенести сюда методы типа addField(), а поля добавлять
     //через массивы в методах по созданию слоя

    public:

     GnmNetwork();
     ~GnmNetwork();

     //----------- создание ------------------

     //TODO: сделать метод по кэшированию данных

     //создаём сеть и задаём для неё по-умолчанию: слои и их поля, атрибуты сети
     int constructNetwork(const char *conString, const char *driverName);

     //создаём новый базовый слой без геометрии
     int addBaseLayer(const char *newLayerName);

     //создаём новый слой (класс объектов) на основе любого существующего
     int addLayer(const char *newLayerName, int baseLayerId, OGRwkbGeometryType geomType = wkbUnknown);

     //добавляем поле в слой
     int addField(int layerId, const char *fieldName, OGRFieldType fieldType);

     //добавляем объект в выбранный слой
     int addFeature(int layerId, OGRGeometry *geometry);

     //задаём значение атрибуту объекта
     int setFeatureAttribute(int layerId, long featureId, int fieldId, int value);

     int syncDataToDisk();

     //----------- трассировка ----------------

     //подготовка сети к расчётам (задание весов, установление блокировок, ...)
     //int prepareNetwork();
};


#endif // NETWORK_MODEL_H
