#include "udpserver.h"

UdpServer::UdpServer(
        quint16 port,
        DailyNotifier* notifier,
        QObject* parent) :
    QUdpSocket(parent),
    mPort(port),
    mNotifier(notifier)
{
    bindNewAddress();
    connect(this, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(handleError(QAbstractSocket::SocketError)));
    Daemon::Utils::log("UDP server is up");
}

UdpServer::~UdpServer()
{
    close();
    Daemon::Utils::log("UDP server is down");
}

void UdpServer::bindNewAddress()
{
    close();
    bind(QHostAddress::AnyIPv4, mPort, QUdpSocket::ShareAddress);
}

void UdpServer::handleReadyRead()
{
    if (hasPendingDatagrams())
    {
        QByteArray buffer;
        buffer.resize(static_cast<int>(pendingDatagramSize()));
        QHostAddress sender;
        quint16 senderPort;
        readDatagram(buffer.data(), buffer.size(), &sender, &senderPort);
        Daemon::Utils::log(QString("UDP server received command: ") +
                           buffer);
        parseDatagram(buffer);
    }
}

void UdpServer::parseDatagram(const QString& data)
{
    QStringList list = data.split('|');
    QString cmd = list[0];
    int argc = list.length();
    if (argc == 1)
    {
        handleSingleArgCommand(cmd);
    }
    else if (argc == 2)
    {
        list[1];
        handleDoubleArgCommand(cmd, list[1]);
    }
    else if (argc == 3)
    {
        // TODO: implement
        // handleTripleArgCommand(cmd, list[1], list[2]);
    }
    else
    {
        Daemon::Utils::log("Too many arguments send to daemon's server");
    }
}

void UdpServer::handleSingleArgCommand(const QString &cmd)
{
    if (cmd == "start_notifying")
    {
        mNotifier->start();
    }
    else if (cmd == "start_notifying")
    {
        mNotifier->stop();
    }
    else if (cmd == "start_email_notifying")
    {
        mNotifier->startEmail();
    }
    else if (cmd == "stop_email_notifying")
    {
        mNotifier->stopEmail();
    }
    else if (cmd == "start_sms_notifying")
    {
        mNotifier->startSms();
    }
    else if (cmd == "stop_sms_notifying")
    {
        mNotifier->stopSms();
    }
    else
    {
        Daemon::Utils::log("Unknonw command received: " + cmd);
    }
}

void UdpServer::handleDoubleArgCommand(
        const QString &cmd, const QString &val)
{
    if (cmd == "set_notification_time")
    {
        QTime time = QTime::fromString(val, "hh:mm");
        if (time.isValid())
        {
            mNotifier->setNotifyTime(time);
        }
        else
        {
            qDebug() << "ERROR: time should be: hh:mm, not" << val;
        }
    }
    else if (cmd == "set_udp_port")
    {
        bool succ = false;
        int portInt = val.toInt(&succ);
        if (succ)
        {
            if (portInt < 1024 || portInt > 65535)
            {
                Daemon::Utils::log(
                            QString("Requested port is out") +
                            "of allowed range <1024, 65535>");
            }
            else
            {
                quint16 port = static_cast<quint16>(portInt);
                setPort(port);
            }
        }
    }
    else
    {
        Daemon::Utils::log("Unknonw command received: " + cmd);
    }
}

void UdpServer::handleError(QAbstractSocket::SocketError se)
{
    Q_UNUSED(se)
    Daemon::Utils::log(QString("UDP server error: ") + errorString());
}

void UdpServer::setPort(quint16 port)
{
    Daemon::Utils::log(QString("Port changed from ") +
           QString::number(mPort) +
           " to " +
           QString::number(port));
    mPort = port;
    bindNewAddress();
}
