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
    
    // 获取任务类型
    SerialPortTaskType taskType() const { return m_taskType; }
    
protected:
    // 串口管理器
    std::shared_ptr<SerialPortManager> m_pSerialPortManager;
    
    // 任务类型
    SerialPortTaskType m_taskType;
    
    // 数据缓冲区
    QByteArray m_data;
    
    // 错误处理辅助方法
    void handleError(QSerialPort::SerialPortError error, const QString& message);
    
    // Add these method declarations
    void openPort();
    void writeData();
    void readData();
    
signals:
    // Add signal declarations that are emitted in the implementation
    void portOpened();
    void dataWritten(const QByteArray& data);
    void dataRead(const QByteArray& data);
};

#endif // SERIALPORTTASK_H
