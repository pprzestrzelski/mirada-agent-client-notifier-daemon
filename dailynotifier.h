#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <QCoreApplication>
#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QDateTime>
#include <QSettings>
#include "utils.h"
#include "smtpclient.h"
#include "mimemessage.h"
#include "mimetext.h"

class UdpServer;

class DailyNotifier : public QObject
{
    Q_OBJECT
    friend UdpServer;

public:
    explicit DailyNotifier(QObject *parent = nullptr);
    ~DailyNotifier();

    void start();
    void stop();

private:
    QTimer mTimer;
    QDateTime mNotifyTime;
    SmtpClient mSmtp;
    bool mSendEmails;
    bool mSendSms;

    qint64 startTimer();
    void stopTimer();
    void startEmail();
    void stopEmail();
    void startSms();
    void stopSms();

    void setNotifyTime(const QTime newTime);

    void setEmailHost(const QString& host);
    void setEmailPort(quint16 port);
    void setEmailUser(const QString& user);
    void setEmailPassword(const QString& passwd);

    void setDbusername(const QString& dbUsername);
    void setDbPassword(const QString& dbPasswd);
    void setDbName(const QString& dbName);
    void setDbAddress(const QString& dbAddress);
    void setDbPortName(const QString& dbPort);

    // TODO: add settings for an sms api gate

    void clearSettings();
    void createDefaultSettings();
    bool defaultSettingsAvailable();
    void readSettings();

    void doNotification();
    qint64 restartTimer();
    void sendEmails();
    void sendSmsNotifications();
    void sendIndividualEmail(
            const QString& emailFrom,
            const QString& nameFrom,
            const QString& emailTo,
            const QString& nameTo,
            const QString& subject,
            const QString& msg);
    void sendSms();
    void updateNotifyTime();
    qint64 msecToNotifyTime();

private slots:
    void notify();
    void handleSmtpError(SmtpClient::SmtpError e, int responceCode);
};

#endif // NOTIFIER_H
