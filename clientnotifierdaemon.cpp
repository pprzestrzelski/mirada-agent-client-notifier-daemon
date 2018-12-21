#include "clientnotifierdaemon.h"

ClientNotifierDaemon::ClientNotifierDaemon(int argc, char* argv[])
    : QtService<QCoreApplication> (argc, argv, "macnd")
{
    setServiceDescription("This is a client notifier (sms and \
                          e-mail) of the Mirada-Agent application.");
    setServiceFlags(QtServiceBase::CanBeSuspended);
    qDebug() << "Daemon"
             << serviceName()
             << "is created and initialized.";
}

ClientNotifierDaemon::~ClientNotifierDaemon()
{

}

void ClientNotifierDaemon::start()
{
    try
    {
        QCoreApplication *app = application();

        mNotifier = new DailyNotifier(app);
        mServer = new UdpServer(
                    Daemon::Consts::DEFAULT_UDP_PORT,
                    mNotifier,
                    app);
        mNotifier->start();

        qDebug().noquote().nospace()
                << "Daemon started ("
                << app->applicationDirPath()
                << ")";
    }
    catch (...)
    {
        qCritical() << "An unknown error in the start";
    }
}

void ClientNotifierDaemon::stop()
{
    try
    {
        mNotifier->stop();
        qDebug().noquote().nospace()
                << "Daemon stopped";
    }
    catch (...)
    {
        qCritical() << "An unknown error in the start";
    }
}

void ClientNotifierDaemon::pause()
{
    try
    {
        mNotifier->stop();
        qDebug().noquote().nospace()
                << "Daemon paused";
    }
    catch (...)
    {
        qCritical() << "An unknown error in the start";
    }
}

void ClientNotifierDaemon::resume()
{
    try
    {
        mNotifier->start();
        qDebug().noquote().nospace()
                << "Daemon resumed";
    }
    catch (...)
    {
        qCritical() << "An unknown error in the start";
    }
}
