#ifndef BYTESPACETASK_H
#define BYTESPACETASK_H

#include <QObject>
#include <QRunnable>
#include <QSerialPort>
#include <QMutex>

// BytespaceTask 作为所有任务的抽象基类
class BytespaceTask : public QObject, public QRunnable {
    Q_OBJECT
public:
    explicit BytespaceTask(int priority = 0);
    virtual ~BytespaceTask() = default;
    // 任务执行入口点
    void run() override;
    // 获取任务优先级
    int priority() const { return m_priority; }
    // 执行任务的主要方法，由TaskManager调用
    virtual bool execute() = 0;
    
signals:
    void taskCompleted();
    void taskFailed(QSerialPort::SerialPortError error);
    
protected:
    // 任务优先级
    int m_priority;
    
    // 互斥锁保护任务执行
    QMutex m_mutex;
};

#endif // BYTESPACETASK_H
