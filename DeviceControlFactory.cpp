#include "devicecontrolfactory.h"
#include <QPushButton>
#include <QSlider>
#include <QLabel>

QWidget* DeviceControlFactory::createControl(const QJsonObject &device, QWidget *parent) {
    QString deviceType = device["type"].toString();
    QString deviceName = device["name"].toString();

    if (deviceType == "switch") {
        QPushButton *button = new QPushButton("Toggle", parent);
        QObject::connect(button, &QPushButton::clicked, [deviceName]() {
            qDebug() << "Switch toggled for device:" << deviceName;
        });
        return button;
    } else if (deviceType == "slider") {
        QSlider *slider = new QSlider(Qt::Horizontal, parent);
        slider->setRange(0, 100);
        QObject::connect(slider, &QSlider::valueChanged, [deviceName](int value) {
            qDebug() << "Slider value for device" << deviceName << ":" << value;
        });
        return slider;
    }
    return nullptr;  // Если тип устройства не распознан
}
