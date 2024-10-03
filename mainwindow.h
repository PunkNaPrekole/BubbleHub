#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QSettings>
#include <QJsonArray>
#include <QSlider>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void sendMessageToServer(const QString &message);
    void openSettings();
    void createDeviceControls(const QJsonArray &devices);
    void logEvent(const QString &message);
    void GetData();
    void ServerAuth();

signals:
    void serverResponseReceived(const QJsonObject &jsonResponse);

private slots:
    void handlerNetworkReply(QNetworkReply *reply);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *networkManager;
    void handlerGetDataResponse(const QJsonObject &response);


};
#endif // MAINWINDOW_H
