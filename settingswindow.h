#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include "SettingsManager.h"


namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr, SettingsManager* settingsManager = nullptr);
    ~SettingsWindow();

signals:
    void pollingServerState(bool &state);
    void networkModeChanged();

private:
    Ui::SettingsWindow *ui;
    void saveSettings();
    void loadSettings();
    void togglePasswordVisibility();
    SettingsManager* settingsManager;
};

#endif // SETTINGSWINDOW_H
