#ifndef SERIALPORTTASK_H
#define SERIALPORTTASK_H

#include "taskmanager/bytespacetask.h"
#include "serialportmanager/serialportmanager.h"
#include <memory>

// 定义串口任务类型
enum class SerialPortTaskType {
    OpenPort,
    ClosePort,
    ReadData,
    WriteData,
    ConfigPort
};

// SerialPortTask 作为所有串口任务的基类
class SerialPortTask : public BytespaceTask {
    Q_OBJECT
public:
    explicit SerialPortTask(std::shared_ptr<SerialPortManager> manager, 
                           SerialPortTaskType type,
                           const QByteArray& data = QByteArray(),
                           int priority = 0);
    virtual ~SerialPortTask() = default;
    
    bool execute() override;
    
    SerialPortTaskType taskType() const { return m_taskType; }
    
protected:
    std::shared_ptr<SerialPortManager> m_pSerialPortManager;
    
    SerialPortTaskType m_taskType;
    
    QByteArray m_data;
    
    void handleError(QSerialPort::SerialPortError error, const QString& message);
};

#endif // SERIALPORTTASK_H
