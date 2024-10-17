#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QVariant>
#include <QString>

class SettingsManager {
public:
    SettingsManager(const QString& organization, const QString& application);

    // Шаблонные методы
    template<typename T>
    T getSetting(const QString& key, const T& defaultValue);

    template<typename T>
    void setSetting(const QString& key, const T& value);

private:
    QSettings settings;
};

// Включение файла с реализацией шаблонов
#include "SettingsManager.tpp"

#endif // SETTINGSMANAGER_H
