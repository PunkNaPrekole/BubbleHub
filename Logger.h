#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger {
public:
    explicit Logger(const QString &filePath);
    void logEvent(const QString &message);

private:
    QString logFilePath;
};

#endif // LOGGER_H
