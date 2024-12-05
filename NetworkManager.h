#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>

class SettingsManager;
class Logger;

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent, SettingsManager *settings, Logger *logger);
    void authenticate();
    void sendRequest(const QString &message);
    void sendControlSignal(const QString &dev_name, QVariant state);

signals:
    void authenticationSuccess(const QString &token);
    void requestFinished(const QString &message);
    void connectionStatusChanged(bool success);
    void systemStateReceived(const QJsonObject &state);
    void updateStateReceived(const QJsonObject &state);
    void signatureInvalid();
    void unknownMessageReceived(QString &message);

private slots:
    void handlerNetworkReply(QNetworkReply *reply);
    bool verifySignature(const QJsonObject &message, const QString &expectedSignature);

private:
    QNetworkAccessManager *manager;
    SettingsManager *settingsManager;
    Logger *logger;
};

#endif // NETWORKMANAGER_H
