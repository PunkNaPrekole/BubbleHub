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
    void authenticate(const QString &username, const QString &password);
    void sendRequest(const QString &message);

signals:
    void authenticationSuccess(const QString &token);
    void requestFinished(const QJsonObject &response);
    void connectionStatusChanged(bool success);
    void devicesReceived(const QJsonArray &devices);
    void dataReceived(const QJsonArray &data);
    void temperatureReceived(const double &temperature);
    void systemStateReceived(const QJsonObject &state);
    void devicesStateReceived(QJsonArray &devicesStatusList);

private slots:
    void handlerNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    SettingsManager *settingsManager;
    Logger *logger;
};

#endif // NETWORKMANAGER_H
