#include "network_model.h"

/*
// **************************************************************
// ***************** Основные методы сети ***********************
// **************************************************************

//конструктор по умолчанию
NtwNetwork::NtwNetwork()
{
    //задаём атрибуты сети по умолчанию
    addAttrib(std::string("Format_Version"));
    addAttrib(std::string("Name"));
    addAttrib(std::string("Coordinate_System"));
}

NtwNetwork::~NtwNetwork()
{
    OGRDataSource::DestroyDataSource(pDataSrc);
}


// **************************************************************
// ********************* Метаданные сети ************************
// **************************************************************

//добавить атрибут сети
void NtwNetwork::addAttrib(std::string name)
{
    NtwStringPair pair = std::make_pair(name,std::string("Undefined"));
    nAttribs.push_back(pair);
}

//задать значение атрибута
void NtwNetwork::setAttribValue(int i, std::string value)
{

}

//задать значение атрибута
void NtwNetwork::setAttribValue(std::string name, std::string value)
{

}

//возвращает атрибут и его значение
NtwStringPair NtwNetwork::getAttrib(int i)
{
    return nAttribs[i];
}

//возвращает количество атрибутов
int NtwNetwork::getAttribCount()
{
    return nAttribs.size();
}



// **************************************************************
// ********************* Связность сети *************************
// **************************************************************

NtwObject::NtwObject(long featureId)
{
    Id = featureId;
}
long NtwObject::getId()
{
    return Id;
}

NtwLine::NtwLine(long featureId, long startPointId, long endPointId): NtwObject(featureId)
{
    sPId = startPointId;
    ePId = endPointId;
}

NtwPoint::NtwPoint(long featureId): NtwObject(featureId)
{
    //...
}
void NtwPoint::appendLine(long incomingLineId)
{
    incLIds.push_back(incomingLineId);
}

NtwGraph::NtwGraph()
{

}

int NtwGraph::addJunction(long featureId)
{
    NtwPoint* p = new NtwPoint(featureId);
    junctions.push_back(*p);
    return NtwOk;
}

//добавляем ребро в граф и подсоединяем его к вершинам
int NtwGraph::addEdge(long featureId, long startPointId, long endPointId)
{
    NtwPoint *sp, *ep;

    bool fl1 = false;
    bool fl2 = false;

    //ищем заданные точки
    for (int i = 0; i<junctions.size(); i++)
    {
        if (junctions[i].getId() == startPointId)
        {
            sp = &junctions[i];
            fl1 = true;
        }
        else if (junctions[i].getId() == endPointId)
        {
            ep = &junctions[i];
            fl2 = true;
        }
        if (fl1 && fl2) break;
    }

    //проверяем, найдены ли заданные точки
    if (!fl1 || !fl2) return NtwErr;
    else
    {//если найдены - подсоединяем
        sp->appendLine(featureId);
        ep->appendLine(featureId);
        NtwLine* l = new NtwLine(featureId,startPointId,endPointId);
        edges.push_back(*l);
        return NtwOk;
    }
}

*/


// **************************************************************
// ********************** Новый вариант *************************
// **************************************************************


GnmNetwork::GnmNetwork()
{
    //...
}


GnmNetwork::~GnmNetwork()
{
    //TODO: разобраться почему деструктор не вызывается при закрытии формы
    OGRDataSource::DestroyDataSource(dataSrc);
}


//TODO: разобраться, почему при повторном запуске приложения, вылетает с ошибкой
//(происходит тогда, когда шейпы уже созданы)
int GnmNetwork::constructNetwork(const char *conString, const char *driverName)
{
    //TODO: задаём атрибуты сети по умолчанию, включая систему координат
    //...

    //загружаем изначальный драйвер
    //TODO: разобраться, почему не грузит PostGIS драйвер
    //м.б. не находит libpq
    //м.б. не регистрируется через OGRRegisterAll(), как проверить что зарегистрировалось, а что нет?
    driver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(driverName);
    if (driver == NULL) return GnmErr;

    //создаём источник данных
    //TODO: не забыть, что для PostGIS по идее нельзя создавать БД, можно только открыть (написано в руководстве)
    dataSrc = driver->CreateDataSource(conString, NULL);
    if (dataSrc == NULL) return GnmErr;

    //создаём слои по умолчанию и добавляем в них поля
    addBaseLayer("SysObject"); //этот слой без геометрии
    if (addField(0,"global_id",OFTInteger) == GnmErr) return GnmErr;
    if (addField(0,"is_blocked",OFTInteger) == GnmErr) return GnmErr;
    if (addField(0,"direction",OFTInteger) == GnmErr) return GnmErr;
    if (addField(0,"cost",OFTInteger) == GnmErr) return GnmErr;
    addLayer("SysPoint", 0, wkbPoint);
    //TEMP--------------------------------------
            //if (addField(0,"id_l_list",OFTIntegerList) == GnmErr) return GnmErr;
    //-------------------------------------------
    addLayer("SysLine", 0, wkbLineString);
    //TEMP--------------------------------------
            //if (addField(0,"id_p_start",OFTInteger) == GnmErr) return GnmErr;
            //if (addField(0,"id_p_end",OFTInteger) == GnmErr) return GnmErr;
    //-------------------------------------------
    return GnmOk;
}


int GnmNetwork::addBaseLayer(const char *newLayerName)//OGRwkbGeometryType geomType)
{

    if (dataSrc == NULL) return GnmErr;

    OGRLayer *baseLayer;
    baseLayer = dataSrc->CreateLayer(newLayerName, NULL, wkbNone, NULL);
    if (baseLayer == NULL) return GnmErr;

    return GnmOk;
}


int GnmNetwork::addLayer(const char *newLayerName, int baseLayerId, OGRwkbGeometryType geomType)
                         //const char *baseLayerName),
                         //std::vector<std::string> fieldNames,
                         //std::vector<OGRFieldType> fieldTypes)
{
    if (dataSrc == NULL) return GnmErr;

    //берём базовый слой
    OGRLayer *baseLayer;
    baseLayer = dataSrc->GetLayer(baseLayerId);
    if (baseLayer == NULL) return GnmErr;

    //и создаём на основе его определения новый, который включает все атрибуты старого
    OGRLayer *newLayer;
    //если тип геометрии не задан, то берём её из базового слоя
    OGRwkbGeometryType gt;
    if (geomType == wkbUnknown) gt = baseLayer->GetGeomType(); else gt = geomType;
    //создаём слой
    newLayer = dataSrc->CreateLayer(newLayerName, NULL, gt, NULL);
    if (newLayer == NULL) return GnmErr;

    int ni = dataSrc->GetLayerCount()-1; //текущий слой - это последний добавленный
    for (int i = 0; i < baseLayer->GetLayerDefn()->GetFieldCount(); i++)
    {
        OGRFieldType ft = baseLayer->GetLayerDefn()->GetFieldDefn(i)->GetType();
        const char *fn = baseLayer->GetLayerDefn()->GetFieldDefn(i)->GetNameRef();
        if (addField(ni, fn, ft) == GnmErr) return GnmErr;
    }

    return GnmOk;
}


//TODO: разобраться, почему не работает GetLayerByName()
//м.б. из-за другого формата строковых констант (символов)?
//в документации OGRSFDriver::CreateDataSource упоминается UTF-8
//TODO: ещё разобраться почему не добавляется поле с типом OFTIntegerList
//м.б. потому что драйвер шейпа не поддерживает? Проверить через OGRLayer::TestCapability()
//int GnmNetwork::addField(const char *layerName, const char *fieldName, OGRFieldType fieldType);
int GnmNetwork::addField(int layerId, const char *fieldName, OGRFieldType fieldType)
{
    if (dataSrc == NULL) return GnmErr;

    OGRLayer *layer;
    //layer = dataSrc->GetLayerByName("layerName");
    layer = dataSrc->GetLayer(layerId);
    if (layer == NULL) return GnmErr;

    OGRFieldDefn *field;
    field = new OGRFieldDefn(fieldName, fieldType);
    if(layer->CreateField(field) != OGRERR_NONE) return GnmErr;

    return GnmOk;
}


int GnmNetwork::addFeature(int layerId, OGRGeometry *geometry)
{
    if (dataSrc == NULL) return GnmErr;

    OGRLayer *layer;
    layer = dataSrc->GetLayer(layerId);
    if (layer == NULL) return GnmErr;

    //проверяем на соответствие геометрию объекта и целевого слоя
    if (layer->GetGeomType() != geometry->getGeometryType()) return GnmErr;

    //добавляем объект без атрибутов
    OGRFeature *feature;
    feature = OGRFeature::CreateFeature(layer->GetLayerDefn());
    feature->SetGeometry(geometry);
    if(layer->CreateFeature(feature) != OGRERR_NONE) return GnmErr;

    //TODO: задавать значения атрибутов по умолчанию здесь
    //...

    OGRFeature::DestroyFeature(feature); //The OGRLayer::CreateFeature() does not take ownership of our feature so we clean it up when done with it.

    return GnmOk;
}


int GnmNetwork::setFeatureAttribute(int layerId, long featureId, int fieldId, int value)
{
    if (dataSrc == NULL) return GnmErr;

    OGRLayer *layer;
    layer = dataSrc->GetLayer(layerId);
    if (layer == NULL) return GnmErr;

    OGRFeature *feature;
    feature = layer->GetFeature(featureId);
    if (feature == NULL) return GnmErr;

    //задаём артибут
    /*
    OGRFieldType type = feature->GetFieldDefnRef(fieldId)->GetType();
    if (type == OFTInteger)
    {
        int *temp = reinterpret_cast<int*>(value);
        feature->SetField(fieldId,*temp);
    }
    else if (type == OFTIntegerList)
    {
        int *temp = reinterpret_cast<int*>(value);
        feature->SetField(fieldId,count,temp);
    }
    */

    feature->SetField(fieldId,value);
    if (layer->SetFeature(feature)  != OGRERR_NONE) return GnmErr;

    OGRFeature::DestroyFeature(feature);
    return GnmOk;
}


int GnmNetwork::syncDataToDisk()
{
    if (dataSrc == NULL) return GnmErr;

    if (dataSrc->SyncToDisk() != OGRERR_NONE) return GnmErr;
    return GnmOk;
}





