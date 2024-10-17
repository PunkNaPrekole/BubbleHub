#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QSettings>


SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    loadSettings();

    connect(ui->saveButton, &QPushButton::clicked, this, &SettingsWindow::saveSettings);
    connect(ui->shwPasswdBtn, &QPushButton::clicked, this, &SettingsWindow::togglePasswordVisibility);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::saveSettings() {
    QString serverAddress = ui->serverAddressLineEdit->text();
    QString serverPort = ui->serverPortLineEdit->text();
    QString username = ui->usernameEdit->text();
    QString password = ui->passwdEdit->text();
    bool pollingState = ui->pollCheckBox->isChecked();
    bool networkMode = ui->networkModeCheckBox->isChecked();

    QSettings settings("PrekolTech", "BubbleHub");

    QString oldUsername = settings.value("user/username", "").toString();
    QString oldPassword = settings.value("user/password", "").toString();
    bool pollingStateNow = settings.value("system/polling").toBool();
    bool networkModeNow = settings.value("system/networkMode").toBool();
    if (username != oldUsername || password != oldPassword) {
        // Если одно из значений изменилось, сбрасываем токен
        settings.setValue("server/token", "");
        settings.setValue("user/username", username);
        settings.setValue("user/password", password);
    }
    if (pollingState != pollingStateNow){
        emit pollingServerState(pollingState);
    }
    if (networkMode != networkModeNow){
        emit networkModeChanged(networkMode);
    }

    settings.setValue("server/serverAddress", serverAddress);
    settings.setValue("server/serverPort", serverPort);
    settings.setValue("system/polling", pollingState);
    settings.setValue("system/networkMode", networkMode);
    // Закрыть окно после сохранения
    this->accept();
}

void SettingsWindow::loadSettings() {
    // Создаём объект QSettings для загрузки настроек
    QSettings settings("PrekolTech", "BubbleHub");

    // Загружаем и отображаем текущие настройки в полях ввода
    QString serverAddress = settings.value("server/serverAddress", "").toString();
    QString serverPort = settings.value("server/serverPort", "").toString();
    QString username = settings.value("user/username", "").toString();
    QString password = settings.value("user/password", "").toString();
    bool pollerState = settings.value("system/polling", true).toBool();
    bool networkMode = settings.value("system/networkMode").toBool();

    ui->serverAddressLineEdit->setText(serverAddress);
    ui->serverPortLineEdit->setText(serverPort);
    ui->usernameEdit->setText(username);
    ui->passwdEdit->setText(password);
    ui->pollCheckBox->setChecked(pollerState);
    ui->networkModeCheckBox->setChecked(networkMode);
}

void SettingsWindow::togglePasswordVisibility(){
    if (ui->passwdEdit->echoMode() == QLineEdit::Password) {
        ui->passwdEdit->setEchoMode(QLineEdit::Normal);
    } else {
        // Если пароль показан, скрываем его
        ui->passwdEdit->setEchoMode(QLineEdit::Password);
    }
}
