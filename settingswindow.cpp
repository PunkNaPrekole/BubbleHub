#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QSettings>


SettingsWindow::SettingsWindow(QWidget *parent, SettingsManager* settingsManager)
    : QDialog(parent), settingsManager(settingsManager)
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

    QString oldUsername = settingsManager->getSetting("user/username", "").toString();
    QString oldPassword = settingsManager->getSetting("user/password", "").toString();
    bool pollingStateNow = settingsManager->getSetting("system/polling", true).toBool();
    bool networkModeNow = settingsManager->getSetting("system/networkMode", false).toBool();
    if (username != oldUsername || password != oldPassword) {
        // Если одно из значений изменилось, сбрасываем токен
        settingsManager->setSetting("server/token", "");
        settingsManager->setSetting("user/username", username);
        settingsManager->setSetting("user/password", password);
    }
    if (pollingState != pollingStateNow){
        emit pollingServerState(pollingState);
    }
    if (networkMode != networkModeNow){
        settingsManager->setSetting("server/autoSearch", networkMode);
        emit networkModeChanged();
    }

    settingsManager->setSetting("server/serverAddress", serverAddress);
    settingsManager->setSetting("server/serverPort", serverPort);
    settingsManager->setSetting("system/polling", pollingState);
    settingsManager->setSetting("system/networkMode", networkMode);
    // Закрыть окно после сохранения
    this->accept();
}

void SettingsWindow::loadSettings() {
    // Загружаем и отображаем текущие настройки в полях ввода
    QString serverAddress = settingsManager->getSetting("server/serverAddress", "").toString();
    QString serverPort = settingsManager->getSetting("server/serverPort", "").toString();
    QString username = settingsManager->getSetting("user/username", "").toString();
    QString password = settingsManager->getSetting("user/password", "").toString();
    bool pollerState = settingsManager->getSetting("system/polling", true).toBool();
    bool networkMode = settingsManager->getSetting("system/networkMode", false).toBool();

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
