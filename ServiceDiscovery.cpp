#include "ServiceDiscovery.h"
#include <QDebug>

ServiceDiscovery::ServiceDiscovery(QObject *parent)
    : QObject(parent),
    multicastAddress(QHostAddress("224.0.0.1")), // Пример адреса для multicast
    multicastPort(12345), // Пример порта
    discovering(false) {
    udpSocket = new QUdpSocket(this);
    connect(udpSocket, &QUdpSocket::readyRead, this, &ServiceDiscovery::readPendingDatagrams);
}

void ServiceDiscovery::startDiscovery() {
    if (discovering) return;

    // Присоединяемся к multicast-группе
    udpSocket->bind(multicastPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    udpSocket->joinMulticastGroup(multicastAddress);
    discovering = true;

    // Отправляем запрос на обнаружение
    QByteArray datagram = "DISCOVER"; // Сообщение для обнаружения
    udpSocket->writeDatagram(datagram, multicastAddress, multicastPort);
}

void ServiceDiscovery::stopDiscovery() {
    // функция остановки поиска сервера
    if (!discovering) return;

    udpSocket->leaveMulticastGroup(multicastAddress);
    udpSocket->close();
    discovering = false;
}

void ServiceDiscovery::readPendingDatagrams() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        QHostAddress sender;
        quint16 senderPort;

        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
        QString message = QString::fromUtf8(datagram);

        qDebug() << "Discovered service at" << sender.toString() << ":" << senderPort;
        int port = static_cast<int>(senderPort);
        emit serviceDiscovered(sender.toString(), port, message);
    }
}

void ServiceDiscovery::restartDiscovery()
{
    stopDiscovery(); // Останавливаем текущий поиск, если он активен
    startDiscovery(); // Запускаем новый поиск
}
