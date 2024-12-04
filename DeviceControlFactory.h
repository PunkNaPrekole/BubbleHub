#ifndef DEVICECONTROLFACTORY_H
#define DEVICECONTROLFACTORY_H

#include <QObject>


class DeviceControlBlock;

class DeviceControlFactory : public QObject
{
    Q_OBJECT

public:
    explicit DeviceControlFactory(QObject *parent = nullptr);

    // Метод для создания блока управления для устройства
    DeviceControlBlock* createControlBlock(const QString &deviceType, const QString &deviceName, int deviceId);

};

#endif // DEVICECONTROLFACTORY_H
