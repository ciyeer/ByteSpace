/**
 * @file logutils.h
 * @brief 日志工具类
 *
 * 提供静态日志记录方法，封装 LogManager 的异步日志写入功能。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#ifndef LOGUTILS_H
#define LOGUTILS_H

#include <QString>
#include "logmanager.h"

class LogUtils {
public:
    LogUtils() = delete;

    static void logMessage(const QString &message, LOG level = LOG::LOG_INFO) {
        LogManager::getInstance().logToFileAsync(message, level);
    }

private:
    ~LogUtils() = delete;
};

#define LOG_INFO(message)    LogUtils::logMessage(message, LOG::LOG_INFO)
#define LOG_DEBUG(message)   LogUtils::logMessage(message, LOG::LOG_DEBUG)
#define LOG_WARNING(message) LogUtils::logMessage(message, LOG::LOG_WARNING)
#define LOG_ERROR(message)   LogUtils::logMessage(message, LOG::LOG_ERROR)

#endif // LOGUTILS_H
