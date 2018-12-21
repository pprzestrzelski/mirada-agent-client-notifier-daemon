#include "utils.h"

void Daemon::Utils::log(const QString& message)
{
    QMutexLocker locker(&Daemon::Utils::mMutex);
    // TODO: for final daemon find a proper place for log file!
    QFile outputFile("udpserver.log");
    outputFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outputFile);

    QString time = QDateTime::currentDateTime().toString("hh:mm:ss.z");
    // FIXME: Primitive padding - reimplement it in a smarter way
    while (time.length() < 12)
    {
        time += "0";
    }

    ts << time
       << " => "
       << message
       << endl;
}

QMutex Daemon::Utils::mMutex;
