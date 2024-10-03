#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settingswindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    networkManager = new QNetworkAccessManager(this);

    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui->connectButton, &QPushButton::clicked, this, [this]() {
        sendMessageToServer("validate token");
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::logEvent(const QString &message) {
    QFile logFile("event_log.txt");
    QString fullPath = QDir::currentPath() + "/event_log.txt";
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        out << "[" << timestamp << "] " << message << "\n";
        logFile.close();
    }
    qDebug() << "Log file location:" << fullPath;
}

void MainWindow::openSettings() {

    SettingsWindow settingsWindow(this);
    if (settingsWindow.exec() == QDialog::Accepted) {
        logEvent("Settings updated successfully");
        // После сохранения настроек, пытаемся подключиться
        ServerAuth();
    }
}

void MainWindow::ServerAuth(){

    QSettings settings("PrekolTech", "BubbleHub");
    QString serverAddress = settings.value("server/serverAddress", "").toString();
    int serverPort = settings.value("server/serverPort", "").toInt();
    QString username = settings.value("user/username", "").toString();
    QString passwd = settings.value("user/password", "").toString();

    QJsonObject json;
    json["type"] = "desktopClient1";
    json["message"] = "auth";
    json["username"] = username;
    json["password"] = passwd;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QString url = QString("http://%1:%2/settings").arg(serverAddress).arg(serverPort);
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [reply, this, serverAddress, serverPort]() {
        if (reply->error() == QNetworkReply::NoError) {

            QByteArray response = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
            QJsonObject jsonObject = jsonResponse.object();

            if (jsonObject["status"].toString() == "auth ok") {
                ui->stateLabel->setText("Connected");
                ui->stateLabel->setStyleSheet("color: green;");
                logEvent("Successfully connected to: " + serverAddress + ":" + QString::number(serverPort));
                QString token = jsonObject["token"].toString();
                QSettings settings("PrekolTech", "BubbleHub");
                settings.setValue("server/token", token);
                logEvent("Token saved: " + token);
            } else {
                ui->stateLabel->setText("authentication error");
                ui->stateLabel->setStyleSheet("color: orange;");
            }
        } else {
            ui->stateLabel->setText("Connection Failed");
            ui->stateLabel->setStyleSheet("color: red;");
            logEvent("Failed to connect to: " + serverAddress + ":" + QString::number(serverPort));
        }
        reply->deleteLater();
    });
}

void MainWindow::sendMessageToServer(const QString &message) {

    QSettings settings("PrekolTech", "BubbleHub");
    QString serverAddress = settings.value("server/serverAddress", "").toString();
    int serverPort = settings.value("server/serverPort", "").toInt();
    QString token = settings.value("server/token", "").toString();

    QString url = QString("http://%1:%2/settings").arg(serverAddress).arg(serverPort);
    QNetworkRequest request((QUrl(url)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject json;

    json["type"] = "desktopClient1";
    json["token"] = token;
    json["message"] = message;
    connect(networkManager, &QNetworkAccessManager::finished, this, &MainWindow::handlerNetworkReply);
    networkManager->post(request, QJsonDocument(json).toJson());
}

void MainWindow::handlerNetworkReply(QNetworkReply *reply) {

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(response);
        emit serverResponseReceived(jsonResponse.object()); // Эмитируем сигнал с ответом
    }
    reply->deleteLater();
}

void MainWindow::GetData() {

    connect(this, &MainWindow::serverResponseReceived, this, &MainWindow::handlerGetDataResponse);
    sendMessageToServer("Message for function 1");
}

void MainWindow::handlerGetDataResponse(const QJsonObject &response) {

    // Обработка ответа для function1
    qDebug() << "Function 1 response:" << response;
}

void MainWindow::createDeviceControls(const QJsonArray &devices) {

    // Очищаем предыдущие элементы управления, если они были
    while (QLayoutItem* item = ui->deviceLayout->takeAt(0)) {
        delete item->widget(); // Удаляем виджет
        delete item;           // Удаляем элемент макета
    }

    // Проходим по каждому устройству и добавляем соответствующие элементы управления
    for (const QJsonValue &value : devices) {
        QJsonObject device = value.toObject();
        QString deviceName = device["name"].toString();
        QString deviceType = device["type"].toString();

        QLabel *label = new QLabel(deviceName, this);
        ui->deviceLayout->addWidget(label);

        if (deviceType == "switch") {
            // Добавляем кнопку для переключателя
            QPushButton *button = new QPushButton("Toggle", this);
            connect(button, &QPushButton::clicked, this, [deviceName]() {
                qDebug() << "Switch toggled for device:" << deviceName;
            });
            ui->deviceLayout->addWidget(button);
        } else if (deviceType == "slider") {
            // Добавляем ползунок для термостата или другого устройства
            QSlider *slider = new QSlider(Qt::Horizontal, this);
            slider->setRange(0, 100);  // Пример диапазона
            connect(slider, &QSlider::valueChanged, this, [deviceName](int value) {
                qDebug() << "Slider value changed for device:" << deviceName << "Value:" << value;
            });
            ui->deviceLayout->addWidget(slider);
        }
    }
}
