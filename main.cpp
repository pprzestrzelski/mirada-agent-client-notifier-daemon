#include <clientnotifierdaemon.h>
#include "udpserver.h"


int main(int argc, char *argv[])
{    
    ClientNotifierDaemon daemon(argc, argv);
    return daemon.exec();
//    QCoreApplication app(argc, argv);
//    UdpServer server(6969);
//    return app.exec();
}
