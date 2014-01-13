#ifndef GNMCORE_H
#define GNMCORE_H

#include <utility> //std::pair, std::make_pair
#include <string>
#include <vector>

#include "ogrsf_frmts.h"

//возвращаемый код ошибки
typedef int NErr;

#define NERR_NONE 0
#define NERR_ANY 1


//элемент графа с его связями
//struct NElement
//{
    //какой объект
    //long id;

    //с какими связан
    //std::vector<long> conIds;

    //вес (цена прохода по этому объекту)
    // - должна формироваться исходя из правил в методе подготовки сети
    //long weight;
//};

//массив таких правил формируется при добавлении строк правил
//class NRule
//{
    //public:
     //NRule(std::string ruleString);
//};


#endif // GNMCORE_H
