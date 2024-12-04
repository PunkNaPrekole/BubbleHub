#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QVariant>
#include <QString>

class SettingsManager {
public:
    SettingsManager(const QString& organization, const QString& application);
    QString getSettingsFilePath() const;
    QVariant getSetting(const QString& key, const QVariant& defaultValue = QVariant());
    void setSetting(const QString& key, const QVariant& value);

private:
    QSettings settings;
};

#endif // SETTINGSMANAGER_H
