#include "BubbleScriptsManager.h"

BubbleScriptsManager::BubbleScriptsManager(const QString &deviceName, QWidget *parent)
    : QDialog(parent), deviceName(deviceName)
{
    // Устанавливаем заголовок окна
    this->setWindowTitle("Bubble Scripts Manager - " + deviceName);
    this->resize(400, 300);

    // Создаём текстовое сообщение
    QLabel *label = new QLabel("Настройка сценариев для устройства: " + deviceName, this);
    label->setAlignment(Qt::AlignCenter);

    // Кнопка "Ок"
    QPushButton *okButton = new QPushButton("Ок", this);
    connect(okButton, &QPushButton::clicked, this, &BubbleScriptsManager::accept);

    // Layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(label);
    layout->addWidget(okButton);
    setLayout(layout);
}
