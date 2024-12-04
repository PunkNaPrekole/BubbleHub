#include "devicecontrolfactory.h"
#include "BinaryControlBlock.h"
#include "DimmerControlBlock.h"

DeviceControlFactory::DeviceControlFactory(QObject *parent) : QObject(parent)
{
}

DeviceControlBlock* DeviceControlFactory::createControlBlock(const QString &deviceType, const QString &deviceName, int deviceId)
{
    if (deviceType == "binary") {
        return new BinaryControlBlock(deviceName, deviceId);
    } else if (deviceType == "dimmer") {
        return new DimmerControlBlock(deviceName, deviceId);
    } else {
        // Вернуть nullptr или создать блок по умолчанию
        qWarning() << "Unknown device type:" << deviceType;
        return nullptr;
    }
}
