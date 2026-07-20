/**
 * @file errorhandler.h
 * @brief 错误处理器
 *
 * 统一管理应用程序中的各类错误，提供错误消息映射和错误状态跟踪。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <QObject>
#include <QSerialPort>
#include <QString>
#include <QMap>
#include <QMutex>

// 定义错误类型枚举
enum class ErrorType {
    SerialPort,    // 串口相关错误
    Network,       // 网络相关错误
    File,          // 文件操作错误
    Configuration, // 配置错误
    Application,   // 应用程序一般错误
    Unknown        // 未知错误
};

// 定义错误级别枚举
enum class ErrorLevel {
    Info,          // 信息级别
    Warning,       // 警告级别
    Error,         // 错误级别
    Critical       // 严重错误
};

class ErrorHandler : public QObject {
    Q_OBJECT
public:
    // 使用梅耶尔斯单例模式
    static ErrorHandler& instance();
    
    // 处理不同类型的错误
    void handleSerialPortError(QSerialPort::SerialPortError error);
    void handleError(ErrorType type, const QString& message, ErrorLevel severity = ErrorLevel::Error);
    
    // 获取错误信息
    QString getErrorMessage(QSerialPort::SerialPortError error) const;
    QString getLastError() const;
    bool hasActiveError() const;
    
    // 清除错误状态
    void clearErrors();

signals:
    void errorOccurred(const QString& message, ErrorType type, ErrorLevel severity);
    void errorCleared();

private:
    explicit ErrorHandler(QObject *parent = nullptr);
    ~ErrorHandler() = default;
    
    // 禁止复制
    ErrorHandler(const ErrorHandler&) = delete;
    ErrorHandler& operator=(const ErrorHandler&) = delete;
    
    // 错误状态管理
    mutable QMutex m_mutex;
    QString m_lastErrorMessage;
    ErrorType m_lastErrorType;
    ErrorLevel m_lastErrorLevel;
    bool m_hasActiveError;
    
    // 错误消息映射表
    QMap<QSerialPort::SerialPortError, QString> m_serialPortErrorMessages;
    
    // 初始化错误消息
    void initializeErrorMessages();
};

#endif // ERRORHANDLER_H