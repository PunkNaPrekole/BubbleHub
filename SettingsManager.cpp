#include "SettingsManager.h"

SettingsManager::SettingsManager(const QString& organization, const QString& application)
    : settings(organization, application) {
    // Возможно, инициализация
}
