#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <memory>

enum class LOG {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
};

class LogManager : public QObject {
    Q_OBJECT

public:
    static LogManager& getInstance();
    virtual ~LogManager() override;
    void logToFileAsync(const QString& message, LOG level = LOG::LOG_INFO);

private:
    explicit LogManager(QObject *parent = nullptr);
    QString logLevelToString(LOG level);
    void rotateLogFile(QFile* file);
    bool ensureFileOpen(std::unique_ptr<QFile>& file);

    std::unique_ptr<QFile> m_debugLogFile;
    std::unique_ptr<QFile> m_errorLogFile;
    QMutex m_logMutex;
};

#endif // LOGMANAGER_H
