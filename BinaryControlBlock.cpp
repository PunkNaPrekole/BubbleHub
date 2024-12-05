#include "BinaryControlBlock.h"
#include "BubbleScriptsManager.h"


BinaryControlBlock::BinaryControlBlock(const QString &deviceName, int deviceId, QWidget *parent)
    : DeviceControlBlock()
{
    deviceLabel = new QLabel(deviceName, this);
    stateSwitch = new QCheckBox("off", this);
    stateSwitch->setTristate(false);

    manageScenarioButton = new QPushButton("manage workflows", this);

    // Переключатель для использования сценария
    useScenarioSwitch = new QCheckBox("use workflows", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(deviceLabel);
    mainLayout->addWidget(stateSwitch);
    mainLayout->addWidget(manageScenarioButton);
    mainLayout->addWidget(useScenarioSwitch);

    // Подключаем сигналы к слотам
    connect(stateSwitch, &QCheckBox::toggled, this, &BinaryControlBlock::onStateSwitchToggled);
    connect(manageScenarioButton, &QPushButton::clicked, this, &BinaryControlBlock::onManageScenarioClicked);
    connect(useScenarioSwitch, &QCheckBox::toggled, this, &BinaryControlBlock::onUseScenarioSwitchToggled);
}

void BinaryControlBlock::onStateSwitchToggled(bool checked)
{
    // Пикаем сигнал о состоянии переключателя
    emit controlButtonPressed(deviceLabel->text(), checked);
    if (checked) {
        stateSwitch->setText("on");
    } else {
        stateSwitch->setText("off");
    }
}

void BinaryControlBlock::onManageScenarioClicked()
{
    BubbleScriptsManager scriptsManager(deviceLabel->text(), this);
    if (scriptsManager.exec() == QDialog::Accepted) {
        qDebug() << "Scenario successfully managed!";
    }
}

void BinaryControlBlock::onUseScenarioSwitchToggled(bool checked)
{
    // TODO: Реализовать логику применения сценария
}

void BinaryControlBlock::updateButtonForRole(const QString &name, bool value)
{
    stateSwitch->setChecked(value);
}

