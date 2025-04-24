#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QThreadPool>
#include <QSerialPort>
#include <memory>
#include "bytespacetask.h"

// 定义最大线程数常量
#define MAXTHREADNUM 5

class TaskManager : public QObject {
    Q_OBJECT
public:
    static TaskManager& getInstance();

    void addTask(std::shared_ptr<BytespaceTask> task);
    void processNextTask();
    void clearTasks();
    void setMaxThreads(int nMaxThreads);
    int activeThreadCount() const;
    int maxThreadCount() const;

signals:
    void taskCompleted();
    void taskFailed(QSerialPort::SerialPortError error);
    void taskError(QSerialPort::SerialPortError error);

private slots:
    void onTaskCompleted();
    void onTaskFailed(QSerialPort::SerialPortError error);
    void decrementPendingTasks();

private:
    // 构造函数设为私有
    explicit TaskManager(QObject *parent = nullptr);
    ~TaskManager();
    
    // 将executeTask移到私有方法
    void executeTask(std::shared_ptr<BytespaceTask> task);

    QQueue<std::shared_ptr<BytespaceTask>> m_taskQueue;
    QMutex m_mutex;
    QMutex m_taskMutex;
    QThreadPool* m_threadPool;
    std::shared_ptr<BytespaceTask> m_currentTask;
    int m_pendingTasks;
    bool m_isProcessing = false;
};

#endif // TASKMANAGER_H
