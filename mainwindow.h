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

private slots:
    void openSettings();  // Открытие окна настроек
    //void GetSystemState();
    void handlerAuthSuccess(const QString &token);  // Обработка успешной аутентификации
    void handlerServerResponse(const QJsonObject &response);  // Обработка ответа от сервера
    void updateConnectionStatus(bool success);
    void onDevicesReceived(const QJsonArray &devices);
    void createCharts(const QJsonArray &response);
    void updateCharts(const double &temperature);
    void updateTimerState(bool checked);
    void updateSystemState(const QJsonObject &state);
    void updateModeState(bool checked);
    void onDeviceStatusReceived(const QJsonArray &devicesStatusList);
    void onServerFound(const QString &addr, int port, const QString &message);
    void searchServerOverMDNS();


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
    QMap<int, DeviceControlBlock*> deviceBlocks;
};

#endif // MAINWINDOW_H
