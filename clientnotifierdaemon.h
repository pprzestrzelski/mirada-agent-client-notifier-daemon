#ifndef CLIENTNOTIFIERDAEMON_H
#define CLIENTNOTIFIERDAEMON_H

#include <QtService/qtservice.h>
#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include "udpserver.h"
#include "dailynotifier.h"
#include "utils.h"

class ClientNotifierDaemon : public QtService<QCoreApplication>
{
public:
    ClientNotifierDaemon(int argc, char* argv[]);
    ~ClientNotifierDaemon();

    void start();
    void stop();
    void pause();
    void resume();

private:
    UdpServer *mServer;
    DailyNotifier *mNotifier;
};

#endif // CLIENTNOTIFIERDAEMON_H
