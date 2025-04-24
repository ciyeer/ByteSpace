#ifndef LOGUTILS_H
#define LOGUTILS_H

#include <QObject>
#include <QString>
#include "logmanager.h"

class LogUtils : public QObject {
    Q_OBJECT

public:
    explicit LogUtils(QObject *parent = nullptr) : QObject(parent) {};
    ~LogUtils() override = default;

    // 公共日志记录方法
    static void logMessage(const QString &message, LOG level = LOG::LOG_INFO) {
        LogManager::getInstance().logToFileAsync(message, level);
    }
};

// 定义全局宏，使日志调用更简洁
#define LOG_INFO(message) LogUtils::logMessage(message, LOG::LOG_INFO)
#define LOG_DEBUG(message) LogUtils::logMessage(message, LOG::LOG_DEBUG)
#define LOG_WARNING(message) LogUtils::logMessage(message, LOG::LOG_WARNING)
#define LOG_ERROR(message) LogUtils::logMessage(message, LOG::LOG_ERROR)

#endif // LOGUTILS_H
