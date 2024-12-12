#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settingswindow.h"
#include "networkmanager.h"
#include "logger.h"
#include "devicecontrolfactory.h"
#include "ServiceDiscovery.h"
#include "SettingsManager.h"
#include "DimmerControlBlock.h"
#include "BinaryControlBlock.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logger(new Logger(QDir::currentPath() + "/event_log.txt"))  // инициализируем логгер и \/
    , networkManager(new NetworkManager(this, &settingsManager, logger)) //         нетворк менеджер
    , deviceControlFactory(new DeviceControlFactory(this))
    , serviceDiscovery(new ServiceDiscovery(this))
    , settingsManager("PrekolTech", "BubbleHub")
{
    ui->setupUi(this);
    this->setMinimumSize(800, 600);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() { networkManager->sendRequest("get_system_state"); });

    bool checkBoxState = settingsManager.getSetting("system/polling", false).toBool();
    networkManager->sendRequest("validate_token");
    if (checkBoxState) {
        timer->start(2000); // Запускаем таймер
    } else {
        timer->stop(); // Останавливаем таймер
    }
    // Подключаем сигналы к слотам
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui->connectButton, &QPushButton::clicked, this, [this]() { networkManager->sendRequest("validate_token"); });
    connect(ui->getDataButton, &QPushButton::clicked, this, [this]() { networkManager->sendRequest("get_start_data"); });
    connect(ui->getDevicesButton, &QPushButton::clicked, this, [this]() { networkManager->sendRequest("get devices"); });
    connect(ui->rebootButton, &QPushButton::clicked, this, [this]() { networkManager->sendRequest("reboot system"); });

    // Подключаем сигналы
    connect(networkManager, &NetworkManager::authenticationSuccess, this, &MainWindow::handlerAuthSuccess);
    connect(networkManager, &NetworkManager::requestFinished, this, &MainWindow::handlerServerResponse);
    connect(networkManager, &NetworkManager::connectionStatusChanged, this, &MainWindow::updateConnectionStatus);
    connect(networkManager, &NetworkManager::systemStateReceived, this, &MainWindow::processReceivedData);
    connect(networkManager, &NetworkManager::unknownMessageReceived, this, &MainWindow::unknownMessageHandler);
    connect(serviceDiscovery, &ServiceDiscovery::serviceDiscovered, this, &MainWindow::onServerFound);



}

MainWindow::~MainWindow() {
    delete ui;
    delete logger;
    delete networkManager;
    delete deviceControlFactory;
    delete serviceDiscovery;
}

void MainWindow::updateTimerState(bool checked) {
    // функция включения/выключения пуллера
    if (checked) {
        timer->start(2000); // Запускаем пуллер
    } else {
        timer->stop(); // Останавливаем пуллер
    }
}

void MainWindow::updateSystemState(const QJsonObject &state){
    QString brokerState = state.value("mqtt broker").toBool() ? "active" : "inactive";
    QString dbState = state.value("database").toString();
    QString devices = QString("count devices: %1").arg(state.value("devices").toInt());
    QString none = state.value("option").toString();

    ui->mqttBrokerStateLabel->setText(brokerState);
    ui->dbStateLabel->setText(dbState);
    ui->devicesCountLabel->setText(devices);
    ui->noneLabel->setText(none);

    ui->mqttBrokerStateLabel->setAlignment(Qt::AlignCenter);
    ui->dbStateLabel->setAlignment(Qt::AlignCenter);
    ui->devicesCountLabel->setAlignment(Qt::AlignCenter);
    ui->noneLabel->setAlignment(Qt::AlignCenter);

    if (dbState == "local_db") {
        ui->dbRestartBtn->setEnabled(false);
    } else {
        ui->dbRestartBtn->setEnabled(true);
    }
}

void MainWindow::openSettings() {
    // функция открывающая окно настроек
    qDebug() << "settings open";
    SettingsWindow settingsWindow(this, &settingsManager);
    qDebug() << "SettingsWindow created";
    settingsWindow.setWindowTitle("Settings");
    connect(&settingsWindow, &SettingsWindow::pollingServerState, this, &MainWindow::updateTimerState);
    connect(&settingsWindow, &SettingsWindow::networkModeChanged, this, &MainWindow::searchServer);
    qDebug() << "Connections established";
    if (settingsWindow.exec() == QDialog::Accepted) {
        logger->logEvent("Settings updated successfully");
        QString token = settingsManager.getSetting("server/token", "").toString();
        qDebug() << "event 1";

        if (token.isEmpty()) {
            networkManager->authenticate();
        }
    }
}

void MainWindow::onServerFound(const QString &addr, int port, const QString &message){
    if (message == "bubbleCore"){
        serviceDiscovery->stopDiscovery();
        settingsManager.setSetting("server/serverAddress", addr);
        settingsManager.setSetting("server/serverPort", port);
        networkManager->sendRequest("validate_token");
        qDebug() << "Service found:" << addr;
    }

}

void MainWindow::getSystemState(){
    QJsonObject sensorsObject;
    for (auto it = lastEntries.begin(); it != lastEntries.end(); ++it) {
        sensorsObject[QString::number(it.key())] = it.value();
    }
    QJsonObject result;
    result["sensors"] = sensorsObject;
    networkManager->sendRequest("get_system_state", result);
}

void MainWindow::searchServer(){
    if (!connected){
        bool isMDNSDiscoveryEnabled = settingsManager.getSetting("service/mdnsDiscovery", true).toBool();
        if (isMDNSDiscoveryEnabled) {
            serviceDiscovery->startDiscovery();
            ui->stateLabel->setText("trying to discover the server...");
            ui->stateLabel->setStyleSheet("color: orange;");
            qDebug() << "Server searching...";
        } else {
            networkManager->sendRequest("validate_token");
        }
    }
}

void MainWindow::unknownMessageHandler(const QString &message){
    QMessageBox::information(this, "The core was unable to process the message ¯|_(ツ)_/¯", "The core reports that it has received an unknown message from the client, most likely your version of BubbleCore is outdated");
}

void MainWindow::updateConnectionStatus(bool success, const QString &msg) {
    // функция которая красит надпись(коннектед ор нот коннектед)
    if (success) {
        ui->stateLabel->setText("Connected");
        ui->stateLabel->setStyleSheet("color: green;");
        connected = true;
    } else {
        if (msg == "auth"){
            ui->stateLabel->setText("Auth failed!");
            ui->stateLabel->setStyleSheet("color: red;");
            QMessageBox::information(this, "Authentication error! (>_<)", "Check the correctness of the login and password, if the error persists, contact the system administrator");
        }
        else if(msg == "connection"){
            ui->stateLabel->setText("Connection error!");
            ui->stateLabel->setStyleSheet("color: red;");
            QMessageBox::information(this, "Error connecting to BubbleCore (>_<)", "Failed to connect to BubbleCore, please enter other data to connect, or activate the core auto-search setting!");
        }
    }
}

void MainWindow::handlerAuthSuccess(const QString &token) {
    // хэндлер вызываемый при успешной аутентификации
    connected = true;
    ui->stateLabel->setText("Connected");
    ui->stateLabel->setStyleSheet("color: green;");
    logger->logEvent("Successfully authenticated. Token: " + token);
    settingsManager.setSetting("server/token", token);
    qDebug() << "auth succes!!!";
}

void MainWindow::handlerServerResponse(const QString &response) {
    // хэндлер ответа от сервера при неизвестном сообщении
    QMessageBox::information(this, "Unknown message received o_0", "An unknown message has been received from the core, most likely your client's version is outdated");
    qDebug() << "Server response:" << response;
}

void MainWindow::processReceivedData(const QJsonObject &state)
{   qDebug() << "received data" << state;
    QJsonObject systate = state["system_info"].toObject();
    updateSystemState(systate);
    QJsonObject devicesInfo = state["devices_info"].toObject();
    if (devicesInfo.contains("executive")) {
        qDebug() << "contains exec!";
        QJsonArray executiveArray = devicesInfo["executive"].toArray();
        for (const QJsonValue &execItem : executiveArray) {
            QJsonObject execObject = execItem.toObject();

            int deviceId = execObject["id"].toInt();
            QString name = execObject["name"].toString();
            QString type = execObject["type"].toString();
            QJsonObject state = execObject["state"].toObject();

            if (!deviceBlocks.contains(deviceId)) {
                qDebug() << "added new control block..." << deviceId;
                DeviceControlBlock *controlBlock = deviceControlFactory->createControlBlock(type, name, deviceId);
                if (controlBlock) {
                    controlBlock->setFixedSize(200, 150);
                    ui->devicesLayout->addWidget(controlBlock, row, col);
                    deviceBlocks[deviceId] = controlBlock;
                    connect(controlBlock, &DeviceControlBlock::controlButtonPressed, this, [this, controlBlock](const QString &deviceName, QVariant state) {
                        qDebug() << "Device:" << deviceName << "State:" << state;
                        QJsonObject controlData;
                        controlData["device_name"] = deviceName;
                        controlData["state"] = QJsonValue::fromVariant(state);
                        networkManager->sendRequest("control_signal", controlData);
                    });
                    qDebug() << "added new control block!!!";
                }

                col++;
                if (col >= maxColumns) {
                    col = 0;
                    row++;
                }
            } else {
                qDebug() << "update control block!";
                updateControlBlock(deviceId, state);
            }

        }
    }

    if (devicesInfo.contains("sensors")) {
        qDebug() << "contains sensors!";
        QJsonArray sensorsArray = devicesInfo["sensors"].toArray();
        for (const QJsonValue &sensorItem : sensorsArray) {
            QJsonObject sensorObject = sensorItem.toObject();

            int sensorId = sensorObject["id"].toInt();
            QString type = sensorObject["type"].toString();
            if (type == "measurement") {
                qDebug() << "contains measurement sensors!";
                int lastEntry = sensorObject["last_entry"].toInt();
                QString chartName = sensorObject["chart_name"].toString();
                QJsonObject readings = sensorObject["readings"].toObject();
                QJsonArray readingsArray;
                for (const QString &timestamp : readings.keys()) {
                    QJsonObject readingItem;
                    readingItem["timestamp"] = timestamp;
                    readingItem["value"] = readings[timestamp];
                    readingsArray.append(readingItem);
                }
                qDebug() << "manageChart for: " << sensorId;
                lastEntries[sensorId] = lastEntry;
                manageCharts(sensorId, type, lastEntry, chartName, readingsArray);
            } else if (type == "state") {
                // TODO
            }

        }
    }
}


void MainWindow::updateControlBlock(int deviceId, const QJsonObject &state)
{
    DeviceControlBlock *controlBlock = deviceBlocks[deviceId];
    if (!controlBlock) return;

    // В зависимости от типа устройства обновляем слайдеры
    if (auto *binaryControlBlock = dynamic_cast<BinaryControlBlock *>(controlBlock)) {
        // Обработка для binary устройства
        for (const QString &key : state.keys()) {
            int value = state[key].toInt();
            binaryControlBlock->updateButtonForRole(key, value);
        }
    } else if (auto *dimmerControlBlock = dynamic_cast<DimmerControlBlock *>(controlBlock)) {
        // Обработка для dimmer устройства
        dimmerControlBlock->updateSliders(state);
    } else {
        qWarning() << "Unknown device type for update: " << deviceId;
    }
}

void MainWindow::manageCharts(int sensorId, const QString &type, int lastEntry, const QString &chartName, const QJsonArray &readingsArray)
{
    if (sensorCharts.contains(sensorId)) {
        qDebug() << "chart exist!";
        auto &chartPair = sensorCharts[sensorId];
        QChart *chart = chartPair.first;
        QLineSeries *series = dynamic_cast<QLineSeries*>(chart->series().first());
        for (const QJsonValue &readingItem : readingsArray) {
            QJsonObject readingObject = readingItem.toObject();
            QDateTime timestamp = QDateTime::fromString(readingObject["timestamp"].toString(), Qt::ISODate);
            double value = readingObject["value"].toDouble();
            series->append(timestamp.toMSecsSinceEpoch(), value);
        }
        qDebug() << "Chart succefull updtae!";
        chartPair.second = QDateTime::currentDateTime();
    } else {
        qDebug() << "creating new chart...";
        QChart *chart = new QChart();
        chart->setTitle(chartName);
        QLineSeries *series = new QLineSeries();
        series->setName(type);
        for (const QJsonValue &readingItem : readingsArray) {
            QJsonObject readingObject = readingItem.toObject();
            QDateTime timestamp = QDateTime::fromString(readingObject["timestamp"].toString(), Qt::ISODate);
            double value = readingObject["value"].toDouble();

            series->append(timestamp.toMSecsSinceEpoch(), value);
        }
        chart->addSeries(series);
        QValueAxis *axisX = new QValueAxis();
        axisX->setTitleText("Time");
        axisX->setLabelFormat("%H:%M");
        chart->addAxis(axisX, Qt::AlignBottom);
        series->attachAxis(axisX);

        QValueAxis *axisY = new QValueAxis();
        axisY->setTitleText("Value");
        chart->addAxis(axisY, Qt::AlignLeft);
        series->attachAxis(axisY);

        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);
        int totalCharts = sensorCharts.size() + 1;
        int columns = qMax(2, static_cast<int>(sqrt(totalCharts)));
        int rows = (totalCharts + columns - 1) / columns; // Округление вверх для строк

        // Устанавливаем одинаковое растяжение для всех строк и колонок
        for (int i = 0; i < columns; ++i) {
            ui->chartLayout->setColumnStretch(i, 1);
        }
        for (int i = 0; i < rows; ++i) {
            ui->chartLayout->setRowStretch(i, 1);
        }

        int row = (totalCharts - 1) / columns;
        int col = (totalCharts - 1) % columns;
        ui->chartLayout->addWidget(chartView, row, col);
        sensorCharts[sensorId] = qMakePair(chart, QDateTime::currentDateTime());
        qDebug() << "new chart succesfull created!!!";
    }
}

void MainWindow::removeStaleCharts()
{
    QDateTime now = QDateTime::currentDateTime();
    for (auto it = sensorCharts.begin(); it != sensorCharts.end();) {
        if (it.value().second.msecsTo(now) > maxIdleTimeMinutes * 60 * 1000) {
            QChart *chart = it.value().first;
            for (int i = 0; i < ui->chartLayout->count(); ++i) {
                QLayoutItem *layoutItem = ui->chartLayout->itemAt(i);
                if (layoutItem) {
                    QWidget *widget = layoutItem->widget();
                    QChartView *chartView = dynamic_cast<QChartView *>(widget);
                    if (chartView && chartView->chart() == chart) {
                        ui->chartLayout->removeWidget(chartView);
                        delete chartView;
                        break;
                    }
                }
            }
        } else {
            ++it;
        }
    }
}
