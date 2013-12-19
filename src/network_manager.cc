
#include "network_manager.h"


GnmNetwork* GnmNetworkManager::createVoidNetwork(const char *conString, const char *driverName)
{
    OGRRegisterAll();

    GnmNetwork* network = new GnmNetwork();

    if (network->constructNetwork(conString,driverName) == GnmErr) return NULL;
    else return network;
}





