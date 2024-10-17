#ifndef SERVICEDISCOVERY_H
#define SERVICEDISCOVERY_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class ServiceDiscovery : public QObject {
    Q_OBJECT

public:
    explicit ServiceDiscovery(QObject *parent = nullptr);
    void startDiscovery();
    void stopDiscovery();
    void restartDiscovery();

signals:
    void serviceDiscovered(const QString &serviceAddress, int servicePort, const QString &message);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket *udpSocket;
    QHostAddress multicastAddress;
    quint16 multicastPort;
    bool discovering;
};

#endif // SERVICEDISCOVERY_H
