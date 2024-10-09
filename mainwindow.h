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
    void ServerAuth();  // Аутентификация на сервере
    void GetTemperature();
    void sendMessageToServer(const QString &message);  // Отправка сообщения на сервер
    void handlerAuthSuccess(const QString &token);  // Обработка успешной аутентификации
    void handlerServerResponse(const QJsonObject &response);  // Обработка ответа от сервера
    void updateConnectionStatus(bool success);
    void onDevicesReceived(const QJsonArray &devices);
    void createCharts(const QJsonArray &response);
    void updateCharts(const double &temperature);
    void updateTimerState(bool checked);

signals:
    void serverResponseReceived(const QJsonObject &response);  // Сигнал при получении ответа от сервера

private:
    Ui::MainWindow *ui;
    Logger *logger;  // Экземпляр логгера
    NetworkManager *networkManager;  // Экземпляр нетворк манагера
    QTimer *timer;
};

#endif // MAINWINDOW_H
