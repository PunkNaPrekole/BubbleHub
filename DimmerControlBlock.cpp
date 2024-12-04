#include "DimmerControlBlock.h"
#include "BubbleScriptsManager.h"


DimmerControlBlock::DimmerControlBlock(const QString &deviceName, int &deviceId, QWidget *parent)
    : DeviceControlBlock()
{
    deviceLabel = new QLabel(deviceName, this);
    toggleSwitch = new QCheckBox("off", this);
    toggleSwitch->setTristate(false);

    manageScenarioButton = new QPushButton("manage workflows", this);

    // Переключатель для использования сценария
    useScenarioSwitch = new QCheckBox("use workflows", this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(deviceLabel);
    mainLayout->addWidget(toggleSwitch);
    mainLayout->addWidget(manageScenarioButton);
    mainLayout->addWidget(useScenarioSwitch);

    // Подключаем сигналы
    connect(toggleSwitch, &QCheckBox::toggled, this, &DimmerControlBlock::onToggleSwitchToggled);
    connect(manageScenarioButton, &QPushButton::clicked, this, &DimmerControlBlock::onManageScenarioClicked);
    connect(useScenarioSwitch, &QCheckBox::toggled, this, &DimmerControlBlock::onUseScenarioToggled);
}

void DimmerControlBlock::updateSliders(const QJsonObject &state)
{
    for (auto it = state.begin(); it != state.end(); ++it) {
        QString sliderName = it.key();
        int sliderValue = it.value().toInt();

        if (sliders.contains(sliderName)) {
            sliders[sliderName]->setValue(sliderValue);
        } else {
            QSlider *slider = new QSlider(Qt::Horizontal, this);
            slider->setRange(0, 100);
            slider->setValue(sliderValue);
            sliders[sliderName] = slider;

            QLabel *sliderLabel = new QLabel(sliderName, this);
            QHBoxLayout *sliderLayout = new QHBoxLayout();
            sliderLayout->addWidget(sliderLabel);
            sliderLayout->addWidget(slider);

            QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(this->layout());
            if (mainLayout) {
                mainLayout->addLayout(sliderLayout);
            }

            connect(slider, &QSlider::valueChanged, this, [this, sliderName](int value) {
                emit sliderValueChanged(deviceLabel->text(), sliderName, value);
            });

        }
    }
}

void DimmerControlBlock::onToggleSwitchToggled(bool checked)
{
    emit toggleSwitchToggled(deviceLabel->text(), checked);
}

void DimmerControlBlock::onManageScenarioClicked()
{
    BubbleScriptsManager scriptsManager(deviceLabel->text(), this);
}

void DimmerControlBlock::onUseScenarioToggled(bool checked)
{

}
