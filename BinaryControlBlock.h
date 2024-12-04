#ifndef BINARYCONTROLBLOCK_H
#define BINARYCONTROLBLOCK_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QVBoxLayout>
#include "DeviceControlBlock.h"


class BinaryControlBlock : public DeviceControlBlock
{
    Q_OBJECT

public:
    explicit BinaryControlBlock(const QString &deviceName, int &deviceId, QWidget *parent = nullptr);

    void updateSliderForRole(const QString &name, bool value);

private:
    QLabel *deviceLabel;
    QCheckBox *stateSwitch;
    QPushButton *manageScenarioButton;
    QCheckBox *useScenarioSwitch;

signals:
    void controlButtonPressed(const QString &deviceName, int action);
    void createScenarioRequested(const QString &deviceName);
    void useScenarioToggled(const QString &deviceName, const QString &scenarioName, bool enabled);

private slots:
    void onStateSwitchToggled(bool checked);
    void onManageScenarioClicked();
    void onUseScenarioSwitchToggled(bool checked);
};

#endif // BINARYCONTROLBLOCK_H
