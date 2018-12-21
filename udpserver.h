#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QUdpSocket>
#include <QTime>
#include "qtservice.h"
#include "utils.h"
#include "dailynotifier.h"

class UdpServer : public QUdpSocket
{
    Q_OBJECT

public:
    UdpServer(
            quint16 port,
            DailyNotifier* notifier,
            QObject *parent = nullptr);
    ~UdpServer();

    void setPort(quint16 port);
    quint16 port() { return mPort; }

private:
    void bindNewAddress();
    void parseDatagram(const QString& data);
    void handleSingleArgCommand(const QString& cmd);
    void handleDoubleArgCommand(const QString& cmd, const QString& val);

    quint16 mPort;
    DailyNotifier* mNotifier;

private slots:
    void handleReadyRead();
    void handleError(QAbstractSocket::SocketError se);
};

#endif // UDPSERVER_H
