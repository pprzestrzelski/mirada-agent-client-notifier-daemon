#include "dailynotifier.h"

DailyNotifier::DailyNotifier(QObject *parent) :
    QObject(parent),
    mSendEmails(false),
    mSendSms(false)
{
    QCoreApplication::setOrganizationName(Daemon::Consts::ORG_NAME);
    QCoreApplication::setOrganizationDomain(Daemon::Consts::ORG_DOMAIN);
    QCoreApplication::setApplicationName(Daemon::Consts::APP_NAME);

    clearSettings();
    createDefaultSettings();
    readSettings();

    connect(&mTimer, SIGNAL(timeout()), this, SLOT(notify()));
    connect(&mSmtp, SIGNAL(smtpError(SmtpClient::SmtpError, int)),
            this, SLOT(handleSmtpError(SmtpClient::SmtpError, int)));
    Daemon::Utils::log("Notifier is up");
}

DailyNotifier::~DailyNotifier()
{
    Daemon::Utils::log("DailyNotifier is down");
}

void DailyNotifier::clearSettings()
{
    QSettings settings;
    settings.clear();
//    settings.remove("organization");
//    settings.remove("timer");
//    settings.remove("smtp");
}

void DailyNotifier::createDefaultSettings()
{
    QSettings settings;

    if (!defaultSettingsAvailable())
    {
        settings.setValue("organization", "Mirada-Agent");

        settings.beginGroup("timer");
        settings.setValue("notificationTimer", QTime(20, 0, 0));
        settings.endGroup();

        settings.beginGroup("smtp");
        settings.setValue("host", "wn08.webd.pl");
        settings.setValue("port", 465);
        settings.setValue("user", "powiadomienia@mirada-agent.pl");
        // TODO: use hash function on password!!!
        settings.setValue("password", "komputer13");
        settings.endGroup();

        settings.beginGroup("db");
        // TODO: add database settings
        settings.endGroup();
    }
}

bool DailyNotifier::defaultSettingsAvailable()
{
    QSettings settings;
    return settings.contains("organization");
}

void DailyNotifier::readSettings()
{
    QSettings settings;
    mNotifyTime.setDate(QDate::currentDate());
    mNotifyTime.setTime(settings.value("timer/notificationTimer")
                        .value<QTime>());

    mSmtp.setHost(settings.value("smtp/host").toString());
    quint16 port = static_cast<quint16>(settings.value("smtp/port")
                                        .toInt());
    mSmtp.setPort(port);
    mSmtp.setConnectionType(SmtpClient::SslConnection);
    mSmtp.setAuthMethod(SmtpClient::AuthPlain);
    mSmtp.setUser(settings.value("smtp/user").toString());
    mSmtp.setPassword(settings.value("smtp/password").toString());
}

void DailyNotifier::start()
{
    startEmail();
    startSms();
    startTimer();
}

void DailyNotifier::startEmail()
{
    if (!mSendEmails) mSendEmails = true;
}

void DailyNotifier::startSms()
{
    if (!mSendSms) mSendSms = true;
}

qint64 DailyNotifier::startTimer()
{
    qint64 timeoutWithin = msecToNotifyTime();
    if (timeoutWithin < 0)
    {
        updateNotifyTime();
        timeoutWithin = msecToNotifyTime();
    }
    mTimer.start(static_cast<int>(timeoutWithin));

    return timeoutWithin;
}

qint64 DailyNotifier::msecToNotifyTime()
{
    QDateTime curr = QDateTime::currentDateTime();
    return curr.msecsTo(mNotifyTime);
}

void DailyNotifier::updateNotifyTime()
{
    // FIXME: One-minute notifier for debug purposes
    //mNotifyTime = mNotifyTime.addDays(1);
    mNotifyTime = mNotifyTime.addMSecs(60000);
}

void DailyNotifier::stop()
{
    stopEmail();
    stopSms();
    stopTimer();
}

void DailyNotifier::stopEmail()
{
    if (mSendEmails) mSendEmails = false;
}

void DailyNotifier::stopSms()
{
    if (mSendSms) mSendSms = false;
}

void DailyNotifier::stopTimer()
{
    mTimer.stop();
}

void DailyNotifier::setNotifyTime(const QTime newTime)
{
    QDateTime oldTime = mNotifyTime;
    mNotifyTime.setTime(newTime);
    startTimer();
    Daemon::Utils::log(QString("New time has been setup ") +
                       "(old:" + oldTime.toString("hh:mm") + ", "
                       "new:" + mNotifyTime.toString("hh:mm") +
                       ")");
}

void DailyNotifier::setDbusername(const QString& dbUsername)
{
    Q_UNUSED(dbUsername)
}

void DailyNotifier::setDbPassword(const QString& dbPasswd)
{
    Q_UNUSED(dbPasswd)
}

void DailyNotifier::setDbName(const QString& dbName)
{
    Q_UNUSED(dbName)
}

void DailyNotifier::setDbAddress(const QString& dbAddress)
{
    Q_UNUSED(dbAddress)
}

void DailyNotifier::setDbPortName(const QString& dbPort)
{
    Q_UNUSED(dbPort)
}

void DailyNotifier::setEmailHost(const QString& host)
{
    Q_UNUSED(host)
}

void DailyNotifier::setEmailPort(quint16 port)
{
    Q_UNUSED(port)
}

void DailyNotifier::setEmailUser(const QString& user)
{
    Q_UNUSED(user)
}

void DailyNotifier::setEmailPassword(const QString& passwd)
{
    Q_UNUSED(passwd)
}

void DailyNotifier::notify()
{
    doNotification();
    restartTimer();
}

void DailyNotifier::doNotification()
{
    if (mSendEmails) sendEmails();
    if (mSendSms) sendSms();
    Daemon::Utils::log("Clients have been notified!");
}

void DailyNotifier::sendEmails()
{
    try
    {
        if (!mSmtp.connectToHost())
        {
            Daemon::Utils::log("Failed to connect to the host");
            return;
        }

        if (!mSmtp.login())
        {
            Daemon::Utils::log("Failed to login to the host");
            return;
        }

        QSettings settings;
        sendIndividualEmail(
            settings.value("smtp/user").toString(),
            settings.value("organization").toString(),
            "pawel.przestrzelski@gmail.com",
            "Pawel Przestrzelski",
            "TEST MESSAGE",
            "This is a simple test message.\n");

        mSmtp.quit();
    }
    catch (...)
    {
        Daemon::Utils::log(QString("SMTP error occured. ") +
                           "Can't send emails. " +
                           "No Internet connection?!");
    }
}

void DailyNotifier::sendIndividualEmail(
        const QString& emailFrom,
        const QString& nameFrom,
        const QString& emailTo,
        const QString& nameTo,
        const QString& subject,
        const QString& msg)
{
    MimeMessage message;
    message.setSender(new EmailAddress(emailFrom, nameFrom));
    message.addRecipient(new EmailAddress(emailTo, nameTo));
    message.setSubject(subject);

    MimeText text;
    text.setText(msg);
    message.addPart(&text);

    if (!mSmtp.sendMail(message))
    {
        Daemon::Utils::log("Failed to send an email to " + emailTo);
    }
}

qint64 DailyNotifier::restartTimer()
{
    return startTimer();
}

void DailyNotifier::sendSms()
{
    // TODO: implement
}

void DailyNotifier::handleSmtpError(
        SmtpClient::SmtpError e,
        int responseCode)
{
    QString error = "SMTP error occured: ";
    switch (e)
    {
    case SmtpClient::SmtpError::ConnectionTimeoutError:     // 0
        error += "connection timeout";
        break;
    case SmtpClient::SmtpError::ResponseTimeoutError:       // 1
        error += "response timeout";
        break;
    case SmtpClient::SmtpError::SendDataTimeoutError:       // 2
        error += "send data timeout";
        break;
    case SmtpClient::SmtpError::AuthenticationFailedError:  // 3
        error += "authentication failed";
        break;
    case SmtpClient::SmtpError::ServerError:                // 4
        error += "server error";
        break;
    case SmtpClient::SmtpError::ClientError:                // 5
        error += "client error";
        break;
    }

    Daemon::Utils::log(error + " (response code:" +
                       QString(responseCode) + ")");
}
