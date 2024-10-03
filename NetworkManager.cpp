#include "networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>

NetworkManager::NetworkManager(QObject *parent) : QObject(parent) {
    manager = new QNetworkAccessManager(this);
}

void NetworkManager::authenticate(const QString &username, const QString &password) {
    QSettings settings("PrekolTech", "BubbleHub");
    QString serverAddress = settings.value("server/serverAddress", "").toString();
    int serverPort = settings.value("server/serverPort", "").toInt();

    QJsonObject json;
    json["type"] = "desktopClient1";
    json["message"] = "auth";
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QString url = QString("http://%1:%2/settings").arg(serverAddress).arg(serverPort);
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });

}

void NetworkManager::sendRequest(const QString &message) {
    QSettings settings("PrekolTech", "BubbleHub");
    QString serverAddress = settings.value("server/serverAddress", "").toString();
    int serverPort = settings.value("server/serverPort", "").toInt();
    QString token = settings.value("server/token", "").toString();

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
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleNetworkReply(reply);
    });

}

void NetworkManager::handleNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonResponse.object();

        if (jsonObject["message"] == "auth ok") {
            emit authenticationSuccess(jsonObject["token"].toString());
        } else if (jsonObject["message"] == "validate token") {
            // Проверка на успешность валидации токена
            bool success = jsonObject["success"].toBool();
            emit connectionStatusChanged(success);  // Отправляем сигнал с результатом
        } else if(jsonObject["message"] == "devices") {
            QJsonArray devicesArray = jsonObject["devices"].toArray();
            emit devicesReceived(devicesArray);
        } else {
            emit requestFinished(jsonObject);
        }
    } else {
        emit connectionStatusChanged(false);  // Ошибка сети или неправильный ответ
    }
    reply->deleteLater();
}
