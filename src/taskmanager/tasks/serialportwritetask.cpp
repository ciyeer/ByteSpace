#include "serialportwritetask.h"
#include "logmanager/logutils.h"

SerialPortWriteTask::SerialPortWriteTask(std::shared_ptr<SerialPortManager> manager,
                                       const QByteArray& data,
                                       int priority)
    : SerialPortTask(manager, SerialPortTaskType::WriteData, data, priority) {
}

bool SerialPortWriteTask::execute() {
    QMutexLocker locker(&m_mutex);
    if (!m_pSerialPortManager || !m_pSerialPortManager->isOpen()) {
        emit taskFailed(QSerialPort::NotOpenError);
        LogUtils::logMessage("串口没打开!", LOG::LOG_ERROR);
        return false;
    }

    if (!m_pSerialPortManager->writeData(m_data)) {
        emit taskFailed(m_pSerialPortManager->getError());
        LogUtils::logMessage(QString("SerialPortWriteTask::execute() failed: %1")
                            .arg(m_pSerialPortManager->getError()), LOG::LOG_ERROR);
        return false;
    }

    emit taskCompleted();
    return true;
}
