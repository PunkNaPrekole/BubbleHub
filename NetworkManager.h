#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>

class NetworkManager : public QObject {
    Q_OBJECT

public:
    explicit NetworkManager(QObject *parent = nullptr);
    void authenticate(const QString &username, const QString &password);
    void sendRequest(const QString &message);

signals:
    void authenticationSuccess(const QString &token);
    void requestFinished(const QJsonObject &response);
    void connectionStatusChanged(bool success);
    void devicesReceived(const QJsonArray &devices);

private slots:
    void handleNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
};

#endif // NETWORKMANAGER_H
