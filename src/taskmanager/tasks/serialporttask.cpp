#include "serialporttask.h"
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
        case SerialPortTaskType::ClosePort:
            m_pSerialPortManager->closePort();
            break;
        default:
            LogUtils::logMessage("未知的任务类型", LOG::LOG_ERROR);
            emit taskFailed(QSerialPort::UnknownError);
            return false;
    }

    emit taskCompleted();
    return true;
}

void SerialPortTask::handleError(QSerialPort::SerialPortError error, const QString& message) {
    ErrorHandler::instance().handleSerialPortError(error);
    
    QString enhancedMessage = QString("%1 (错误代码: %2)").arg(message).arg(static_cast<int>(error));
    LogUtils::logMessage(enhancedMessage, LOG::LOG_ERROR);
    
    emit taskFailed(error);
}
