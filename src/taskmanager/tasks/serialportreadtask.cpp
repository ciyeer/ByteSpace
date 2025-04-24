#include "serialportreadtask.h"
#include "logmanager/logutils.h"

SerialPortReadTask::SerialPortReadTask(std::shared_ptr<SerialPortManager> manager,
                                     int priority)
    : SerialPortTask(manager, SerialPortTaskType::ReadData, QByteArray(), priority) {
}

bool SerialPortReadTask::execute() {
    QMutexLocker locker(&m_mutex);
    if (!m_pSerialPortManager || !m_pSerialPortManager->isOpen()) {
        emit taskFailed(QSerialPort::NotOpenError);
        LogUtils::logMessage("串口没打开，无法读取数据!", LOG::LOG_ERROR);
        return false;
    }

    if (!m_pSerialPortManager->isReadable()) {
        emit taskFailed(QSerialPort::ReadError);
        LogUtils::logMessage("串口不可读!", LOG::LOG_ERROR);
        return false;
    }

    QByteArray data = m_pSerialPortManager->readData();
    if (data.isEmpty()) {
        // 没有数据可读不一定是错误，可能只是当前没有数据
        LogUtils::logMessage("没有数据可读", LOG::LOG_DEBUG);
        emit taskCompleted();
        return true;
    }

    emit dataRead(data);
    LogUtils::logMessage(QString("成功读取 %1 字节数据").arg(data.size()), LOG::LOG_DEBUG);
    emit taskCompleted();
    return true;
}

