#include "logger.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>

Logger::Logger(const QString &filePath) : logFilePath(filePath) {}

void Logger::logEvent(const QString &message) {
    QFile logFile(logFilePath);
    if (logFile.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&logFile);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        out << "[" << timestamp << "] " << message << "\n";
        logFile.close();
    }
}
