#include "SettingsManager.h"

SettingsManager::SettingsManager(const QString& organization, const QString& application)
    : settings(QSettings::IniFormat, QSettings::UserScope, organization, application) {
}

QString SettingsManager::getSettingsFilePath() const {
    return settings.fileName();
}

QVariant SettingsManager::getSetting(const QString& key, const QVariant& defaultValue) {
    return settings.value(key, defaultValue);
}

void SettingsManager::setSetting(const QString& key, const QVariant& value) {
    settings.setValue(key, value);
}
