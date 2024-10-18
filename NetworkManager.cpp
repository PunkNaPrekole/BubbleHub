#include "networkmanager.h"
#include "SettingsManager.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

NetworkManager::NetworkManager(QObject *parent, SettingsManager *settings, Logger *logger)
    : QObject(parent), settingsManager(settings), logger(logger), manager(new QNetworkAccessManager(this)) {
}

void NetworkManager::authenticate() {
    QString serverAddress = settingsManager->getSetting<QString>("server/serverAddress", "");
    int serverPort = settingsManager->getSetting<int>("server/serverPort", 5000);
    QString username = settingsManager->getSetting<QString>("user/username", "");
    QString password = settingsManager->getSetting<QString>("user/password", "");

    QJsonObject json;
    json["type"] = "desktopClient1";
    json["message"] = "auth";
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QString url = QString("http://%1:%2/authentication").arg(serverAddress).arg(serverPort);
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { handlerNetworkReply(reply); });

}

void NetworkManager::sendRequest(const QString &message) {
    QSettings settings("PrekolTech", "BubbleHub");
    QString serverAddress = settingsManager->getSetting<QString>("server/serverAddress", "");
    int serverPort = settingsManager->getSetting<int>("server/serverPort", 5000);
    QString token = settingsManager->getSetting<QString>("server/token", "");

    QJsonObject json;
    json["type"] = "desktopClient1";
    json["token"] = token;
    json["message"] = message;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QString url = QString("http://%1:%2/settings").arg(serverAddress).arg(serverPort);
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { handlerNetworkReply(reply); });

}

void NetworkManager::handlerNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonResponse.object();

        QString message = jsonObject["message"].toString();

        // Используем словарь для сопоставления сообщений с действиями
        QMap<QString, std::function<void()>> messageHandlers = {
            {"auth ok", [&]() { emit authenticationSuccess(jsonObject["token"].toString()); }},
            {"validate token", [&]() {
                 bool success = jsonObject["success"].toBool();
                 emit connectionStatusChanged(success);
                 if (!success) {
                     authenticate();  // Вызываем authenticate, если success == false
                 }
             }},
            {"devices", [&]() { emit devicesReceived(jsonObject["devices"].toArray()); }},
            {"data", [&]() { emit dataReceived(jsonObject["charts"].toArray()); }},
            {"temperature_update", [&]() { emit temperatureReceived(jsonObject["temperature"].toDouble()); }},
            {"system_state", [&]() { emit systemStateReceived(jsonObject["system_state"].toObject()); }}
        };

        // Выполняем соответствующий обработчик
        if (messageHandlers.contains(message)) {
            messageHandlers[message]();
        } else {
            // Обработка по умолчанию
            emit requestFinished(jsonObject);
        }
    } else {
        emit connectionStatusChanged(false);  // Ошибка сети или неправильный ответ
    }

    reply->deleteLater();
}
