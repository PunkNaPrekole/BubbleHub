#include "devicecontrolfactory.h"

DeviceControlFactory::DeviceControlFactory(QObject *parent) : QObject(parent)
{
}

DeviceControlBlock* DeviceControlFactory::createControlBlock(const QString &deviceType, const QString &deviceName, int deviceId)
{
    // В зависимости от типа устройства создаем соответствующий блок управления
    if (deviceType == "light") {
        return new DeviceControlBlock(deviceName, deviceId); // Например, блок для управления светом
    } else if (deviceType == "thermostat") {
        return new DeviceControlBlock(deviceName, deviceId); // Блок для термостата
    } else {
        return new DeviceControlBlock(deviceName, deviceId); // Блок по умолчанию
    }
}
