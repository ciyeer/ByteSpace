#include "errorhandler.h"

// 梅耶尔斯单例实现
ErrorHandler& ErrorHandler::instance() {
    static ErrorHandler instance;
    return instance;
}

ErrorHandler::ErrorHandler(QObject *parent) : QObject(parent),
    m_lastErrorType(ErrorType::Unknown),
    m_lastErrorLevel(ErrorLevel::Info),
    m_hasActiveError(false) {
    initializeErrorMessages();
}

void ErrorHandler::initializeErrorMessages() {
    // 初始化串口错误消息映射
    m_serialPortErrorMessages[QSerialPort::NoError] = "没有错误";
    m_serialPortErrorMessages[QSerialPort::DeviceNotFoundError] = "找不到串口设备";
    m_serialPortErrorMessages[QSerialPort::PermissionError] = "没有权限访问串口";
    m_serialPortErrorMessages[QSerialPort::OpenError] = "串口已被其他程序占用";
    m_serialPortErrorMessages[QSerialPort::NotOpenError] = "串口未打开";
    m_serialPortErrorMessages[QSerialPort::WriteError] = "写入串口时发生错误";
    m_serialPortErrorMessages[QSerialPort::ReadError] = "读取串口时发生错误";
    m_serialPortErrorMessages[QSerialPort::ResourceError] = "串口资源不可用";
    m_serialPortErrorMessages[QSerialPort::UnsupportedOperationError] = "不支持的操作";
    m_serialPortErrorMessages[QSerialPort::TimeoutError] = "串口操作超时";
    m_serialPortErrorMessages[QSerialPort::UnknownError] = "未知串口错误";
}

void ErrorHandler::handleSerialPortError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) {
        return;
    }
    
    QString errorMessage = getErrorMessage(error);
    handleError(ErrorType::SerialPort, errorMessage, ErrorLevel::Error);
}

void ErrorHandler::handleError(ErrorType type, const QString& message, ErrorLevel level) {
    m_lastErrorMessage = message;
    m_lastErrorType = type;
    m_lastErrorLevel = level;
    m_hasActiveError = true;
    
    emit errorOccurred(message, type, level);
}

QString ErrorHandler::getErrorMessage(QSerialPort::SerialPortError error) const {
    return m_serialPortErrorMessages.value(error, "未知串口错误");
}

QString ErrorHandler::getLastError() const {
    return m_lastErrorMessage;
}

bool ErrorHandler::hasActiveError() const {
    return m_hasActiveError;
}

void ErrorHandler::clearErrors() {
    if (m_hasActiveError) {
        m_lastErrorMessage.clear();
        m_lastErrorType = ErrorType::Unknown;
        m_lastErrorLevel = ErrorLevel::Info;
        m_hasActiveError = false;
        
        emit errorCleared();
    }
}