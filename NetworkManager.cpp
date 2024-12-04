#include "networkmanager.h"
#include "SettingsManager.h"
#include "Logger.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QSettings>
#include <QCryptographicHash>

NetworkManager::NetworkManager(QObject *parent, SettingsManager *settings, Logger *logger)
    : QObject(parent), settingsManager(settings), logger(logger), manager(new QNetworkAccessManager(this)) {
}

void NetworkManager::authenticate() {
    QString serverAddress = settingsManager->getSetting("server/serverAddress", "").toString();
    int serverPort = settingsManager->getSetting("server/serverPort", 5000).toInt();
    QString username = settingsManager->getSetting("user/username", "").toString();
    QString password = settingsManager->getSetting("user/password", "").toString();

    QJsonObject json;
    json["type"] = "desktopClient1";
    json["message"] = "authentication request";
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
    QString serverAddress = settingsManager->getSetting("server/serverAddress", "192.1680.107").toString();
    int serverPort = settingsManager->getSetting("server/serverPort", 12345).toInt();
    QString token = settingsManager->getSetting("server/token", "").toString();

    QJsonObject json;
    json["type"] = "desktopClient";
    json["token"] = token;
    json["message"] = message;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QString url = QString("http://%1:%2/settings").arg(serverAddress).arg(serverPort);
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = manager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { handlerNetworkReply(reply); });
     qDebug() << "Request send: " << message;
     qDebug() << "request to:" << serverAddress;

}

void NetworkManager::handlerNetworkReply(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "message received";
        QByteArray response = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        QJsonObject jsonObject = jsonResponse.object();

        QString signature = jsonObject["sign"].toString();
        jsonObject.remove("sign");
        qDebug() << QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);

        if (!verifySignature(jsonObject, signature)) {
            qDebug() << "Error signature invalid";
            emit signatureInvalid(); // FIXME тут надо бы избавиться от сигнала и залогировать несоответсвтие хэшей
            reply->deleteLater();
            return;
        } else {
            qDebug() << "signature valid!";
        }
        QJsonObject serviceInfo = jsonObject["service_info"].toObject();
        QString message = serviceInfo["message"].toString();
        qDebug() << "message:" << message;
        // Используем словарь для сопоставления сообщений с действиями
        QMap<QString, std::function<void()>> messageHandlers = {
            {"auth ok", [&]() { emit authenticationSuccess(serviceInfo["token"].toString());
             qDebug() << "auth ok handler";
             qDebug() << "token:" << serviceInfo["token"].toString();}},
            {"validate token", [&]() {
                 bool success = jsonObject["success"].toBool();
                 emit connectionStatusChanged(success);
                 if (!success) {
                     authenticate();  // Вызываем authenticate, если success == false
                 }
             }},
            {"unknown message", [&]() { emit unknownMessageReceived(message); }},
            {"boot data", [&]() { emit systemStateReceived(jsonObject["devices_info"].toObject()); }},
            {"system state", [&]() { emit updateStateReceived(jsonObject["devices_info"].toObject()); }}
        };

        // Выполняем соответствующий обработчик
        if (messageHandlers.contains(message)) {
            messageHandlers[message]();
        } else {
            qDebug() << "benebebe";
            emit requestFinished(jsonObject);
        }
    } else {
        emit connectionStatusChanged(false);  // Ошибка сети или неправильный ответ
    }

    reply->deleteLater();
}

bool NetworkManager::verifySignature(const QJsonObject &message, const QString &expectedSignature) {
    QJsonObject messageCopy = message;

    // Сериализуем JSON-объект в строку
    QJsonDocument doc(messageCopy);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    // Генерируем SHA-256 хеш
    QByteArray hash = QCryptographicHash::hash(jsonData, QCryptographicHash::Sha256).toHex();

    // Сравниваем с ожидаемой подписью
    return QString(hash) == expectedSignature;
}
