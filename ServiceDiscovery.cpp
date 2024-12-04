#include "ServiceDiscovery.h"
#include <QDebug>

ServiceDiscovery::ServiceDiscovery(QObject *parent)
    : QObject(parent),
    broadcastPort(12345),
    discovering(false) {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &ServiceDiscovery::readPendingDatagrams);

    discoveryTimer = new QTimer(this);
    connect(discoveryTimer, &QTimer::timeout, this, &ServiceDiscovery::sendDiscoveryRequest);
}

void ServiceDiscovery::startDiscovery() {
    if (discovering) return;
    udpSocket->bind(QHostAddress("192.168.0.255"), broadcastPort);
    discovering = true;
    discoveryTimer->start(1000);
    sendDiscoveryRequest();
}

void ServiceDiscovery::sendDiscoveryRequest() {
    QByteArray datagram = "DISCOVER_BUBBLECORE";
    udpSocket->writeDatagram(datagram, QHostAddress("192.168.0.255"), broadcastPort);
    //qDebug() << "send datagram";
}

void ServiceDiscovery::stopDiscovery() {
    if (!discovering) return;

    udpSocket->close();
    discovering = false;

    discoveryTimer->stop();
}

void ServiceDiscovery::readPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;

        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        QString message = QString::fromUtf8(datagram);

        emit serviceDiscovered(sender.toString(), static_cast<int>(senderPort), message);
    }
}

void ServiceDiscovery::restartDiscovery()
{
    stopDiscovery();
    startDiscovery();
}
