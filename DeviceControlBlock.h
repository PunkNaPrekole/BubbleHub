#ifndef DEVICECONTROLBLOCK_H
#define DEVICECONTROLBLOCK_H

#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

class DeviceControlBlock : public QWidget
{
    Q_OBJECT

public:
    explicit DeviceControlBlock(const QString &deviceName, int &deviceId, QWidget *parent = nullptr);
    void updateSliderForRole(const QString &role, int value);

private:
    QLabel *deviceLabel;
    QPushButton *controlButton1;
    QPushButton *controlButton2;
    QSlider *controlSlider;
    QMap<QString, QSlider*> sliders;

signals:
    void controlButtonPressed(const QString &deviceName, int action);  // Нажатие кнопки: 1 - On, 0 - Off
    void sliderValueChanged(const QString &deviceName, int value);

public slots:
    void onControlButton1Clicked();
    void onControlButton2Clicked();
    void onSliderValueChanged(int value);
};

#endif // DEVICECONTROLBLOCK_H
