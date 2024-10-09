#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "settingswindow.h"
#include "networkmanager.h"
#include "logger.h"
#include "devicecontrolfactory.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , logger(new Logger(QDir::currentPath() + "/event_log.txt"))  // инициализируем логгер и \/
    , networkManager(new NetworkManager(this))                    //                    нетворк менеджер
{
    ui->setupUi(this);

    this->setMinimumSize(800, 600);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::GetTemperature);
    QSettings settings("PrekolTech", "BubbleHub");
    bool checkBoxState = settings.value("system/polling", false).toBool();
    if (checkBoxState) {
        timer->start(2000); // Запускаем таймер
    } else {
        timer->stop(); // Останавливаем таймер
    }
    sendMessageToServer("validate token");
    // Подключаем сигналы к слотам
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::openSettings);
    connect(ui->connectButton, &QPushButton::clicked, this, [this]() {
        sendMessageToServer("validate token");
    });
    connect(ui->getDataButton, &QPushButton::clicked, this, [this]() {
        sendMessageToServer("get data");
    });
    connect(ui->getDevicesButton, &QPushButton::clicked, this, [this]() {
        sendMessageToServer("get devices");
    });

    // Подключаем сигналы от нетворк манагера)
    connect(networkManager, &NetworkManager::authenticationSuccess, this, &MainWindow::handlerAuthSuccess);
    connect(networkManager, &NetworkManager::requestFinished, this, &MainWindow::handlerServerResponse);
    connect(networkManager, &NetworkManager::connectionStatusChanged, this, &MainWindow::updateConnectionStatus);
    connect(networkManager, &NetworkManager::devicesReceived, this, &MainWindow::onDevicesReceived);
    connect(networkManager, &NetworkManager::dataReceived, this, &MainWindow::createCharts);
    connect(networkManager, &NetworkManager::temperatureReceived, this, &MainWindow::updateCharts);


}

MainWindow::~MainWindow() {
    delete ui;
    delete logger;
    delete networkManager;
}
void MainWindow::updateTimerState(bool checked) {
    // функция включения/выключения пуллера
    if (checked) {
        timer->start(2000); // Запускаем пуллер
    } else {
        timer->stop(); // Останавливаем пуллер
    }
}

void MainWindow::openSettings() {
    // функция открывающая окно настроек
    SettingsWindow settingsWindow(this);
    connect(&settingsWindow, &SettingsWindow::pollingServerState, this, &MainWindow::updateTimerState);
    if (settingsWindow.exec() == QDialog::Accepted) {
        logger->logEvent("Settings updated successfully");
        QSettings settings("PrekolTech", "BubbleHub");
        QString token = settings.value("server/token", "").toString();

        if (token.isEmpty()) {
            ServerAuth();
        }
    }
}

void MainWindow::ServerAuth() {
    // функция отправляющая запрос для получения auth токена
    QSettings settings("PrekolTech", "BubbleHub");
    QString username = settings.value("user/username", "").toString();
    QString passwd = settings.value("user/password", "").toString();
    networkManager->authenticate(username, passwd);
}

void MainWindow::GetTemperature(){
    // функция запроса данных по температуре
    sendMessageToServer("get temperature");
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
    QSettings settings("PrekolTech", "BubbleHub");
    settings.setValue("server/token", token);
}

void MainWindow::handlerServerResponse(const QJsonObject &response) {
    // хэндлер ответа от сервера?
    qDebug() << "Server response:" << response;
    emit serverResponseReceived(response);  // пикаем сигнал
}

void MainWindow::onDevicesReceived(const QJsonArray &devices) {
    // очищаем deviceLayout
    while (QLayoutItem* item = ui->deviceLayout->takeAt(0)) {
        delete item->widget();
        delete item;
    }

    // Создание новых элементов управления для устройств
    for (const QJsonValue &value : devices) {
        QJsonObject device = value.toObject();
        QString deviceName = device["name"].toString();
        QLabel *label = new QLabel(deviceName, this);
        ui->deviceLayout->addWidget(label);
        QWidget *control = DeviceControlFactory::createControl(device, this);
        if (control) {
            ui->deviceLayout->addWidget(control);
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

