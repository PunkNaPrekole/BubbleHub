#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonArray>
#include <QSettings>
#include <QDir>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
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
    void sendMessageToServer(const QString &message);  // Отправка сообщения на сервер
    void handleAuthSuccess(const QString &token);  // Обработка успешной аутентификации
    void handleServerResponse(const QJsonObject &response);  // Обработка ответа от сервера
    void updateConnectionStatus(bool success);
    void onDevicesReceived(const QJsonArray &devices);
    void createCharts(const QJsonArray &response);

signals:
    void serverResponseReceived(const QJsonObject &response);  // Сигнал при получении ответа от сервера

private:
    Ui::MainWindow *ui;
    Logger *logger;  // Экземпляр логгера
    NetworkManager *networkManager;  // Экземпляр нетворк манагера
};

#endif // MAINWINDOW_H
