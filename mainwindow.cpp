#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settingswindow.h"
#include "networkmanager.h"
#include "logger.h"
#include "devicecontrolfactory.h"
#include "ServiceDiscovery.h"
#include "SettingsManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logger(new Logger(QDir::currentPath() + "/event_log.txt"))  // инициализируем логгер и \/
    , networkManager(new NetworkManager(this, &settingsManager, logger))
    , deviceControlFactory(new DeviceControlFactory(this))   //                    нетворк менеджер
    , serviceDiscovery(new ServiceDiscovery(this))
    , settingsManager("PrekolTech", "BubbleHub")
{
    ui->setupUi(this);
    this->setMinimumSize(800, 600);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::GetSystemState);

    bool checkBoxState = settingsManager.getSetting("system/polling", false);
    bool isMDNSDiscoveryEnabled = settingsManager.getSetting("service/mdnsDiscovery", true);

    if (checkBoxState) {
        timer->start(2000); // Запускаем таймер
    } else {
        timer->stop(); // Останавливаем таймер
    }
    if (isMDNSDiscoveryEnabled) {
        serviceDiscovery->startDiscovery(); // Вызываем функцию только если настройка true
    }

    sendMessageToServer("validate token");
    // Подключаем сигналы к слотам
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui->connectButton, &QPushButton::clicked, this, [this]() { sendMessageToServer("validate token"); });
    connect(ui->getDataButton, &QPushButton::clicked, this, [this]() { sendMessageToServer("get data"); });
    connect(ui->getDevicesButton, &QPushButton::clicked, this, [this]() { sendMessageToServer("get devices"); });
    connect(ui->rebootButton, &QPushButton::clicked, this, [this]() { sendMessageToServer("reboot system"); });

    // Подключаем сигналы
    connect(networkManager, &NetworkManager::authenticationSuccess, this, &MainWindow::handlerAuthSuccess);
    connect(networkManager, &NetworkManager::requestFinished, this, &MainWindow::handlerServerResponse);
    connect(networkManager, &NetworkManager::connectionStatusChanged, this, &MainWindow::updateConnectionStatus);
    connect(networkManager, &NetworkManager::devicesReceived, this, &MainWindow::onDevicesReceived);
    connect(networkManager, &NetworkManager::dataReceived, this, &MainWindow::createCharts);
    connect(networkManager, &NetworkManager::temperatureReceived, this, &MainWindow::updateCharts);
    connect(networkManager, &NetworkManager::systemStateReceived, this, &MainWindow::updateSystemState);
    connect(networkManager, &NetworkManager::devicesStateReceived, this, &MainWindow::onDeviceStatusReceived);
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

void MainWindow::onServerFound(const QString &addr, int port, const QString &message){
    if (message == "bubbleCore"){
        serviceDiscovery->stopDiscovery();
        settingsManager.setSetting("server/serverAddress", addr);
        settingsManager.setSetting("server/serverPort", port);
    }

}
void MainWindow::updateModeState(bool checked){
    // функция смены режима работы локальная/глобальная сеть
    settingsManager.setSetting("server/autoSearch", checked);
    if (checked){
        //TODO допилить
    } else {
        //TODO допилить
    }
}

void MainWindow::openSettings() {
    // функция открывающая окно настроек
    SettingsWindow settingsWindow(this);
    connect(&settingsWindow, &SettingsWindow::pollingServerState, this, &MainWindow::updateTimerState);
    connect(&settingsWindow, &SettingsWindow::networkModeChanged, this, &MainWindow::updateModeState);
    if (settingsWindow.exec() == QDialog::Accepted) {
        logger->logEvent("Settings updated successfully");
        QString token = settingsManager.getSetting<QString>("server/token", "");

        if (token.isEmpty()) {
            ServerAuth();
        }
    }
}
void MainWindow::updateSystemState(const QJsonObject &state){
    QString brokerState = state.value("broker state").toString();
    QString dbState = state.value("db state").toString();
    QString devices = state.value("devices").toString();
    QString none = state.value("none").toString();

    ui->mqttBrokerStateLabel->setText(brokerState);
    ui->dbStateLabel->setText(dbState);
    ui->devicesCountLabel->setText(devices);
    ui->noneLabel->setText(none);

    ui->mqttBrokerStateLabel->setAlignment(Qt::AlignCenter);
    ui->dbStateLabel->setAlignment(Qt::AlignCenter);
    ui->devicesCountLabel->setAlignment(Qt::AlignCenter);
    ui->noneLabel->setAlignment(Qt::AlignCenter);
}

void MainWindow::ServerAuth() {
    // функция отправляющая запрос для получения auth токена
    QSettings settings("PrekolTech", "BubbleHub");
    QString username = settingsManager.getSetting<QString>("user/username", "");
    QString passwd = settingsManager.getSetting<QString>("user/password", "");
    networkManager->authenticate(username, passwd);
}

void MainWindow::GetSystemState(){
    // функция запроса данных по температуре
    sendMessageToServer("get_system_state");
}

void MainWindow::updateConnectionStatus(bool success) {
    // функция которая красит надпись(коннектед ор нот коннектед) рядом с кнопкой connect
    if (success) {
        ui->stateLabel->setText("Connected");
        ui->stateLabel->setStyleSheet("color: green;");
    } else {
        ui->stateLabel->setText("Failed to connect");
        ui->stateLabel->setStyleSheet("color: red;");
    }
}


void MainWindow::sendMessageToServer(const QString &message) {
    // функция отправки мессенджа на сервер
    networkManager->sendRequest(message);
}

void MainWindow::handlerAuthSuccess(const QString &token) {
    // хэндлер вызываемый при успешной аутентификации
    ui->stateLabel->setText("Connected");
    ui->stateLabel->setStyleSheet("color: green;");
    logger->logEvent("Successfully authenticated. Token: " + token);

    // Сохранение токена
    settingsManager.setSetting("server/token", token);
}

void MainWindow::handlerServerResponse(const QJsonObject &response) {
    // хэндлер ответа от сервера?
    qDebug() << "Server response:" << response;
}

void MainWindow::onDevicesReceived(const QJsonArray &devices) {
    // Создание новых элементов управления для устройств
    int row = 0, col = 0;  // Индексы для размещения в сетке
    const int maxColumns = 2;  // Количество столбцов

    for (const QJsonValue &deviceVal : devices) {
        QJsonObject deviceObj = deviceVal.toObject();
        int deviceId = deviceObj["id"].toInt();
        QString deviceType = deviceObj["type"].toString();
        QString deviceName = deviceObj["name"].toString();

        // Создаем блок управления для каждого устройства
        DeviceControlBlock *controlBlock = deviceControlFactory->createControlBlock(deviceType, deviceName, deviceId);

        // Устанавливаем фиксированный размер
        controlBlock->setFixedSize(150, 100);  // Задаем размеры блоков управления

        // Добавляем блок управления в сетку
        ui->devicesLayout->addWidget(controlBlock, row, col);
        deviceBlocks[deviceId] = controlBlock;

        // Обновляем индексы для следующего устройства
        col++;
        if (col >= maxColumns) {
            col = 0;
            row++;
        }
    }
}

void MainWindow::onDeviceStatusReceived(const QJsonArray &devicesStatusList)
{
    for (const QJsonValue &value : devicesStatusList) {
        QJsonObject deviceStatus = value.toObject();

        int deviceId = deviceStatus["id"].toInt();
        // Проверяем, существует ли блок управления для этого устройства
        if (deviceBlocks.contains(deviceId)) {
            DeviceControlBlock *controlBlock = deviceBlocks[deviceId];
            for (const QString &key : deviceStatus.keys()) {
                if (key != "id") {
                    int value = deviceStatus[key].toInt();
                    controlBlock->updateSliderForRole(key, value);
                }
            }
        } else {
            deviceControlFactory->createControlBlock(deviceStatus["type"].toString(), deviceStatus["name"].toString(), deviceStatus["id"].toInt());
        }
    }
}

void MainWindow::createCharts(const QJsonArray &response) {
    // Очищаем предыдущее содержимое виджета
    QLayoutItem* item;
    while ((item = ui->chartLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    // Проходим по каждому объекту в массиве
    for (const QJsonValue &chartValue : response) {
        QJsonObject chartObject = chartValue.toObject();
        QString chartName = chartObject["name"].toString();

        // Создаем график
        QChart *chart = new QChart();
        chart->setTitle(chartName);
        chart->setBackgroundBrush(QBrush(QColor("#323232"))); // фон для всего графика
        chart->setPlotAreaBackgroundBrush(QBrush(QColor("#1E1E1E"))); // фон для области построения графика
        chart->setTitleBrush(QBrush(QColor("#FFFFFF")));  //  цвет заголовка графика

        // Создаем оси, которые будут общими для всех кривых
        QValueAxis *axisX = new QValueAxis();
        axisX->setLabelFormat("%i");
        axisX->setTitleText("Data Points");
        axisX->setGridLineColor(QColor("#121212"));  // цвет линий сетки
        axisX->setLinePenColor(QColor("#121212"));   // цвет осей
        axisX->setLabelsColor(QColor("#FFFFFF"));    // Цвет подписей на осях
        axisX->setTitleBrush(QBrush(QColor("#FFFFFF")));  // Цвет заголовка оси
        chart->addAxis(axisX, Qt::AlignBottom);

        QValueAxis *axisY = new QValueAxis();
        axisY->setLabelFormat("%f");
        axisY->setTitleText("Values");
        axisY->setGridLineColor(QColor("#121212"));  // Цвет линий сетки
        axisY->setLinePenColor(QColor("#121212"));   // Цвет осей
        axisY->setLabelsColor(QColor("#FFFFFF"));    // Цвет подписей на осях
        axisY->setTitleBrush(QBrush(QColor("#FFFFFF")));  // Цвет заголовка оси
        chart->addAxis(axisY, Qt::AlignLeft);

        // Переменная для индекса по оси X
        int index = 0;

        // Проходим по каждому ключу, кроме "name", чтобы создать серию для каждой кривой
        for (const QString &key : chartObject.keys()) {
            if (key == "name") {
                continue; // Пропускаем ключ "name"
            }

            // Получаем массив данных для этой кривой
            QJsonArray dataArray = chartObject[key].toArray();

            // Создаем серию для графика
            QLineSeries *series = new QLineSeries();
            series->setName(key);  // Устанавливаем имя для кривой (например, "indoor", "outdoor")

            // Заполняем серию данными
            int i = 0;
            for (const QJsonValue &dataValue : dataArray) {
                series->append(i++, dataValue.toDouble());
            }

            // Добавляем серию в график
            chart->addSeries(series);
            series->attachAxis(axisX); // Привязываем к общей оси X
            series->attachAxis(axisY); // Привязываем к общей оси Y

        }
        axisY->setRange(0, 30);
        // Создаем виджет для отображения графика
        QChartView *chartView = new QChartView(chart);
        chartView->setRenderHint(QPainter::Antialiasing);

        // Добавляем график в компоновку
        ui->chartLayout->addWidget(chartView);
    }
}

void MainWindow::updateCharts(const double &temperature) {
    //pass
}

