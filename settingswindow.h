#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

signals:
    void pollingServerState(bool &state);
    void networkModeChanged(bool &mode);

private:
    Ui::SettingsWindow *ui;
    void saveSettings();
    void loadSettings();
    void togglePasswordVisibility();
};

#endif // SETTINGSWINDOW_H
