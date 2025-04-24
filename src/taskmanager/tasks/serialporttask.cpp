#include "serialporttask.h"
#include <QSerialPortInfo>
#include <QMutexLocker>
#include "logmanager/logutils.h"
#include "utils/errorhandler.h"

SerialPortTask::SerialPortTask(std::shared_ptr<SerialPortManager> manager,
                             SerialPortTaskType type,
                             const QByteArray& data,
                             int priority)
    : BytespaceTask(priority)
    , m_pSerialPortManager(manager)
    , m_taskType(type)
    , m_data(data) {
}

bool SerialPortTask::execute() {
    if (!m_pSerialPortManager) {
        return false;
    }
    
    switch (m_taskType) {
        case SerialPortTaskType::ConfigPort:
            return m_pSerialPortManager->configurePort();
        case SerialPortTaskType::OpenPort:
            if (!m_pSerialPortManager->openPort()) {
                emit taskFailed(m_pSerialPortManager->getError());
                return false;
            }
            break;
        case SerialPortTaskType::ClosePort:
            m_pSerialPortManager->closePort();
            break;
        case SerialPortTaskType::WriteData:
            if (!m_pSerialPortManager->writeData(m_data)) {
                emit taskFailed(m_pSerialPortManager->getError());
                return false;
            }
            break;
        case SerialPortTaskType::ReadData:
            readData();
            return true;  // readData() 方法内部会发出信号
        default:
            LogUtils::logMessage("未知的任务类型", LOG::LOG_ERROR);
            emit taskFailed(QSerialPort::UnknownError);  // 使用未知错误枚举
            return false;
    }

    emit taskCompleted();
    return true;
}

void SerialPortTask::handleError(QSerialPort::SerialPortError error, const QString& message) {
    // 使用ErrorHandler处理错误
    ErrorHandler::instance().handleSerialPortError(error);
    
    // 记录日志并发送任务失败信号
    QString enhancedMessage = QString("%1 (错误代码: %2)").arg(message).arg(static_cast<int>(error));
    LogUtils::logMessage(enhancedMessage, LOG::LOG_ERROR);
    
    emit taskFailed(error);
}

void SerialPortTask::openPort() {
    QMutexLocker locker(&m_mutex);

    if (!m_pSerialPortManager) {
        LogUtils::logMessage("串口管理器未初始化", LOG::LOG_ERROR);
        emit taskFailed(QSerialPort::ResourceError);  // 使用资源错误枚举
        return;
    }

    QString portName = m_pSerialPortManager->getPortName();
    if (portName.isEmpty()) {
        LogUtils::logMessage("串口名称为空", LOG::LOG_ERROR);
        emit taskFailed(QSerialPort::DeviceNotFoundError);  // 使用设备未找到错误枚举
        return;
    }

    bool portFound = false;
    // 存储为局部变量避免容器分离
    const auto availablePorts = QSerialPortInfo::availablePorts();
    // 使用 const auto& 避免拷贝和分离
    for (const auto& info : availablePorts) {
        if (info.portName() == portName) {
            portFound = true;
            LogUtils::logMessage(QString("找到串口: %1").arg(portName), LOG::LOG_DEBUG);
            break;
        }
    }

    if (!portFound) {
        LogUtils::logMessage(QString("串口不存在: %1").arg(portName), LOG::LOG_ERROR);
        emit taskFailed(QSerialPort::DeviceNotFoundError);  // 使用设备未找到错误枚举
        return;
    }

    if (!m_pSerialPortManager->openPort()) {
        QSerialPort::SerialPortError error = m_pSerialPortManager->getError();
        handleError(error, QString("打开串口失败: %1").arg(portName));
        return;
    }

    LogUtils::logMessage(QString("串口 %1 打开成功").arg(portName), LOG::LOG_INFO);
    emit portOpened();
    emit taskCompleted();
}

void SerialPortTask::writeData() {
    QMutexLocker locker(&m_mutex);
    if (!m_pSerialPortManager || !m_pSerialPortManager->isOpen() || !m_pSerialPortManager->isWritable()) {
        handleError(QSerialPort::NotOpenError, "串口未打开或不可写");
        return;
    }

    if (m_pSerialPortManager->writeData(m_data)) {
        emit dataWritten(m_data);
        emit taskCompleted();
    }
    else {
        handleError(m_pSerialPortManager->getError(), "写入数据失败");
    }
}

void SerialPortTask::readData() {
    QMutexLocker locker(&m_mutex);
    if (!m_pSerialPortManager || !m_pSerialPortManager->isOpen() || !m_pSerialPortManager->isReadable()) {
        handleError(QSerialPort::NotOpenError, "串口未打开或不可读");
        return;
    }

    QByteArray data = m_pSerialPortManager->readData();
    if (!data.isEmpty()) {
        emit dataRead(data);
        emit taskCompleted();
    }
    else {
        handleError(m_pSerialPortManager->getError(), "读取数据失败");
    }
}
