#ifndef DIMMERCONTROLBLOCK_H
#define DIMMERCONTROLBLOCK_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QMap>
#include "DeviceControlBlock.h"

class DimmerControlBlock : public DeviceControlBlock
{
    Q_OBJECT

public:
    explicit DimmerControlBlock(const QString &deviceName, int &deviceId, QWidget *parent = nullptr);

    // Метод для обновления слайдеров по JSON-объекту
    void updateSliders(const QJsonObject &state);

signals:
    void toggleSwitchToggled(const QString &deviceName, bool enabled);
    void sliderValueChanged(const QString &deviceName, const QString &sliderName, int value);
    void createScenarioRequested(const QString &deviceName);
    void useScenarioToggled(const QString &deviceName, const QString &scenarioName, bool enabled);

private slots:
    void onToggleSwitchToggled(bool checked);
    void onManageScenarioClicked();
    void onUseScenarioToggled(bool checked);

private:
    QLabel *deviceLabel;
    QCheckBox *toggleSwitch;
    QMap<QString, QSlider*> sliders;
    QPushButton *manageScenarioButton;
    QCheckBox *useScenarioSwitch;
};

#endif // DIMMERCONTROLBLOCK_H
