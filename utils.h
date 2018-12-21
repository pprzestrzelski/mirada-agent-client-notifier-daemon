#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QMutex>
#include <QMutexLocker>

namespace Daemon
{
class Utils
{
public:
    static void log(const QString& message);

private:
    static QMutex mMutex;
};

namespace Consts
{
    static const QString ORG_NAME = "Mirada-Agent";
    static const QString ORG_DOMAIN = "mirada-agent.pl";
    static const QString APP_NAME = "Client Notifier Daemon";

    static const quint16 DEFAULT_UDP_PORT = 6969;

    static const QTime DEFAULT_NOTIFY_TIME(20, 0, 0);
}
}

#endif // UTILS_H
