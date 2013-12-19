#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H


#include "network_model.h"


class GnmNetworkManager
{
    public:

     //создаём сеть со слоями и их атрибутами по умолчанию
     static GnmNetwork* createVoidNetwork(const char *conString, const char *driverName);
};



#endif // NETWORK_MANAGER_H
