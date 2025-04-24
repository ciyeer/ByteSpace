#include "serialportopentask.h"
#include "logmanager/logutils.h"

SerialPortOpenTask::SerialPortOpenTask(std::shared_ptr<SerialPortManager> manager, int priority)
    : SerialPortTask(manager, SerialPortTaskType::OpenPort, QByteArray(), priority) {
}

bool SerialPortOpenTask::execute() {
    QMutexLocker locker(&m_mutex);
    
    if (!m_pSerialPortManager) {
        LogUtils::logMessage("串口管理器为空", LOG::LOG_ERROR);
        return false;
    }
    
    if (m_pSerialPortManager->isOpen()) {
        LogUtils::logMessage("串口已经打开", LOG::LOG_WARNING);
        return true; // 已经打开，视为成功
    }
    
    bool result = m_pSerialPortManager->openPort();
    if (result) {
        LogUtils::logMessage("串口打开成功", LOG::LOG_INFO);
    } 
    else {
        handleError(m_pSerialPortManager->getError(), "串口打开失败");
    }
    
    return result;
}
