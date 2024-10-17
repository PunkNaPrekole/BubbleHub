template<typename T>
T SettingsManager::getSetting(const QString& key, const T& defaultValue) {
    return settings.value(key, defaultValue).template value<T>();
}

template<typename T>
void SettingsManager::setSetting(const QString& key, const T& value) {
    settings.setValue(key, QVariant::fromValue(value));
}
