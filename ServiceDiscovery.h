#ifndef SERVICEDISCOVERY_H
#define SERVICEDISCOVERY_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>
#include <QTimer>

class ServiceDiscovery : public QObject {
    Q_OBJECT

public:
    explicit ServiceDiscovery(QObject *parent = nullptr);
    void startDiscovery();
    void stopDiscovery();
    void restartDiscovery();
    void sendDiscoveryRequest();


signals:
    void serviceDiscovered(const QString &serviceAddress, int servicePort, const QString &message);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    quint16 broadcastPort;
    bool discovering;
    QTimer *discoveryTimer;
};

#endif // SERVICEDISCOVERY_H
