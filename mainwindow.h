#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include <QSettings>
#include <QDir>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QTimer>
#include <QMessageBox>
#include "logger.h"
#include "networkmanager.h"
#include "devicecontrolfactory.h"
#include "ServiceDiscovery.h"
#include "SettingsManager.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool connected = false;

private slots:
    void openSettings();  // Открытие окна настроек
    void handlerAuthSuccess(const QString &token);  // Обработка успешной аутентификации
    void handlerServerResponse(const QString &response);  // Обработка ответа от сервера
    void updateConnectionStatus(bool success, const QString &msg);
    void manageCharts(int sensorId, const QString &type, int lastEntry, const QString &chartName, const QJsonArray &readingsArray);
    void removeStaleCharts();
    void updateTimerState(bool checked);
    void updateSystemState(const QJsonObject &state);
    void updateControlBlock(int deviceId, const QJsonObject &state);
    void onServerFound(const QString &addr, int port, const QString &message);
    void searchServer();
    void processReceivedData(const QJsonObject &data);
    void unknownMessageHandler(const QString &message);
    void getSystemState();

signals:
    void serverResponseReceived(const QJsonObject &response);
    void onControlButtonPressed(const QString &deviceName, int action);
    void onSliderValueChanged(const QString &deviceName, int value);
    void startSearchingServer();

private:
    Ui::MainWindow *ui;
    Logger *logger;  // Экземпляр логгера
    NetworkManager *networkManager;  // Экземпляр нетворк манагера
    DeviceControlFactory *deviceControlFactory;
    ServiceDiscovery *serviceDiscovery;
    SettingsManager settingsManager;
    QTimer *timer;
    QMap<int, DeviceControlBlock*> deviceBlocks; // хранилище контрол блоков
    QMap<int, QPair<QChart*, QDateTime>> sensorCharts; // Хранилище графиков: sensorId -> (график, последнее обновление)
    QTimer cleanupTimer; // Таймер для очистки устаревших графиков
    const int maxIdleTimeMinutes = 15; // Максимальное время без обновлений
    int row = 0;
    int col = 0;
    const int maxColumns = 2;
    QMap<int, int> lastEntries; // мапа для хранения sensor_id:last_entry

};

#endif // MAINWINDOW_H
