#include "DeviceControlBlock.h"

DeviceControlBlock::DeviceControlBlock(const QString &deviceName, int &deviceId, QWidget *parent)
    : QWidget(parent)
{
    deviceLabel = new QLabel(deviceName, this);
    controlButton1 = new QPushButton("On", this);
    controlButton2 = new QPushButton("Off", this);
    controlSlider = new QSlider(Qt::Horizontal, this);

    QString role = deviceName + "Slider";
    controlSlider->setObjectName(role);
    sliders[role] = controlSlider;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(controlButton1);
    buttonLayout->addWidget(controlButton2);

    mainLayout->addWidget(deviceLabel);
    mainLayout->addLayout(buttonLayout);
    mainLayout->addWidget(controlSlider);

    connect(controlButton1, &QPushButton::clicked, this, &DeviceControlBlock::onControlButton1Clicked);
    connect(controlButton2, &QPushButton::clicked, this, &DeviceControlBlock::onControlButton2Clicked);
    connect(controlSlider, &QSlider::valueChanged, this, &DeviceControlBlock::onSliderValueChanged);
}

void DeviceControlBlock::onControlButton1Clicked()
{
    // при нажатии кнопки 1 пикаем сигнал
    emit controlButtonPressed(deviceLabel->text(), 1);
}

void DeviceControlBlock::onControlButton2Clicked()
{
    // при нажатии кнопки 2 пикаем сигнал
    emit controlButtonPressed(deviceLabel->text(), 0);
}

void DeviceControlBlock::onSliderValueChanged(int value)
{
    // при изменении слайедра пикаем сигнал
    emit sliderValueChanged(deviceLabel->text(), value);
}

void DeviceControlBlock::updateSliderForRole(const QString &name, int value)
{
    QString role = name + "Slider";
    sliders[role]->setValue(value);

}


