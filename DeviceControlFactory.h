#ifndef DEVICECONTROLFACTORY_H
#define DEVICECONTROLFACTORY_H

#include <QWidget>
#include <QJsonObject>

class DeviceControlFactory {
public:
    static QWidget* createControl(const QJsonObject &device, QWidget *parent = nullptr);
};

#endif // DEVICECONTROLFACTORY_H
