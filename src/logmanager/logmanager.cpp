#include "logmanager.h"
#include <QFileInfo>
#include <QThreadPool>
#include <QMutexLocker>
#include <QDir>
#include <QDebug>
#include <QCoreApplication>

LogManager& LogManager::getInstance() {
    static LogManager instance;
    return instance;
}

LogManager::LogManager(QObject *parent) : QObject(parent) {
    QString logPath = QCoreApplication::applicationDirPath() + "/logs";
    QDir logDir(logPath);
    
    if (!logDir.exists()) {
        if (!logDir.mkpath(".")) {
            qWarning() << "Failed to create logs directory:" << logPath;
            return;
        }
    }

    m_debugLogFile = std::make_unique<QFile>(logPath + "/debug_log.txt");
    m_errorLogFile = std::make_unique<QFile>(logPath + "/error_log.txt");

    if (!m_debugLogFile->open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open debug log file.";
        m_debugLogFile.reset();  // 如果打开失败，清除文件指针
    }

    if (!m_errorLogFile->open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Failed to open error log file.";
        m_errorLogFile.reset();  // 如果打开失败，清除文件指针
    }
}

// 在析构函数中关闭文件并清理资源
LogManager::~LogManager() {
    // 关闭日志文件
    if (m_debugLogFile && m_debugLogFile->isOpen()) {
        m_debugLogFile->close();
    }

    if (m_errorLogFile && m_errorLogFile->isOpen()) {
        m_errorLogFile->close();
    }
}

void LogManager::rotateLogFile(QFile* file) {
    if (!file || !file->isOpen()) {
        return;
    }

    // 获取当前文件大小
    qint64 size = file->size();
    
    // 如果文件大于 10MB，进行轮转
    const qint64 MAX_SIZE = 10 * 1024 * 1024; // 10MB
    if (size > MAX_SIZE) {
        QString fileName = file->fileName();
        file->close();
        
        // 限制备份文件数量
        QDir dir = QFileInfo(fileName).dir();
        QStringList filters;
        filters << QFileInfo(fileName).fileName() + ".*.bak";
        QFileInfoList backups = dir.entryInfoList(filters, QDir::Files, QDir::Time);
        
        // 如果备份文件超过10个，删除最旧的
        const int MAX_BACKUPS = 10;
        if (backups.size() >= MAX_BACKUPS) {
            for (int i = MAX_BACKUPS - 1; i < backups.size(); ++i) {
                QFile::remove(backups[i].absoluteFilePath());
            }
        }
        
        // 备份旧文件
        QString backupName = fileName + "." + QDateTime::currentDateTime()
            .toString("yyyy-MM-dd-hh-mm-ss") + ".bak";
        
        QFile::rename(fileName, backupName);
        
        // 重新打开新文件
        if (!file->open(QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Failed to rotate log file:" << fileName;
        }
    }
}

void LogManager::logToFileAsync(const QString& message, LOG level) {
    QThreadPool::globalInstance()->start([=]() {
        QMutexLocker locker(&m_logMutex);

        std::unique_ptr<QFile>& file = (level == LOG::LOG_ERROR) ? m_errorLogFile : m_debugLogFile;

        // 使用 ensureFileOpen 确保文件是打开的
        if (!ensureFileOpen(file)) {
            qWarning() << "Failed to ensure log file is open.";
            return;
        }

        QTextStream logStream(file.get());
        QString logLevelStr = logLevelToString(level);
        QString timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        logStream << "[" << timeStamp << "] " << logLevelStr << ": " << message << '\n';
        qDebug() << "Log written: " << message;

        rotateLogFile(file.get());
    });
}

QString LogManager::logLevelToString(LOG level) {
    switch (level) {
    case LOG::LOG_DEBUG:
        return "[DEBUG]";
    case LOG::LOG_INFO:
        return "[INFO]";
    case LOG::LOG_WARNING:
        return "[WARNING]";
    case LOG::LOG_ERROR:
        return "[ERROR]";
    default: return "";
    }
}

bool LogManager::ensureFileOpen(std::unique_ptr<QFile>& file) {
    if (!file) return false;
    
    if (!file->isOpen()) {
        // 尝试重新打开文件
        if (!file->open(QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Failed to reopen log file:" << file->fileName();
            return false;
        }
    }
    return true;
}
