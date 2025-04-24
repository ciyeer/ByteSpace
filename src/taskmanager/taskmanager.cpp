#include "taskmanager.h"
#include <QDebug>
#include <QMetaObject>
#include <QtConcurrent>
#include "logmanager/logutils.h"
#include "utils/errorhandler.h"

TaskManager::TaskManager(QObject *parent) 
    : QObject(parent)
    , m_threadPool(new QThreadPool(this))
    , m_pendingTasks(0)
    , m_isProcessing(false) {
    m_threadPool->setMaxThreadCount(MAXTHREADNUM);
}

TaskManager::~TaskManager() {
    // 停止线程池接收新任务
    m_threadPool->clear();  // 清空未开始执行的任务
    m_threadPool->waitForDone();  // 等待所有任务完成，确保安全销毁
}

TaskManager& TaskManager::getInstance() {
    static TaskManager instance;
    return instance;
}

void TaskManager::addTask(std::shared_ptr<BytespaceTask> task) {
    if (!task) {
        LogUtils::logMessage("尝试添加空任务", LOG::LOG_ERROR);
        return;
    }
    
    // 使用互斥锁保护任务队列
    QMutexLocker locker(&m_mutex);
    
    // 连接任务信号到管理器
    connect(task.get(), &BytespaceTask::taskCompleted, this, &TaskManager::onTaskCompleted);
    connect(task.get(), &BytespaceTask::taskFailed, this, &TaskManager::onTaskFailed);
    
    // 添加到队列
    m_taskQueue.enqueue(task);
    
    // 如果当前没有任务在执行，启动任务
    if (!m_isProcessing) {
        processNextTask();
    }
}

void TaskManager::processNextTask() {
    QMutexLocker locker(&m_mutex);
    
    if (m_taskQueue.isEmpty()) {
        m_isProcessing = false;
        return;
    }
    
    m_isProcessing = true;
    auto task = m_taskQueue.dequeue();
    locker.unlock(); // 解锁，避免在执行任务时长时间持有锁
    
    // 调用统一的任务执行方法
    executeTask(task);
}

void TaskManager::executeTask(std::shared_ptr<BytespaceTask> task) {
    QMutexLocker locker(&m_taskMutex);
    
    if (!task) {
        LogUtils::logMessage("任务指针为空", LOG::LOG_ERROR);
        emit taskFailed(QSerialPort::ResourceError);
        QMetaObject::invokeMethod(this, "processNextTask", Qt::QueuedConnection);
        return;
    }

    locker.unlock(); // 解锁，避免在执行任务时长时间持有锁
    
    try {
        LogUtils::logMessage("开始执行任务...", LOG::LOG_DEBUG);
        if (task->execute()) {
            LogUtils::logMessage("任务执行成功", LOG::LOG_DEBUG);
            QMetaObject::invokeMethod(this, "onTaskCompleted", Qt::QueuedConnection);
        } 
        else {
            LogUtils::logMessage("任务执行失败", LOG::LOG_ERROR);
            QMetaObject::invokeMethod(this, "onTaskFailed", Qt::QueuedConnection,
                                     Q_ARG(QSerialPort::SerialPortError, QSerialPort::UnknownError));
        }
    } 
    catch (const std::exception& e) {
        QString errorMsg = QString("任务执行异常: %1").arg(e.what());
        LogUtils::logMessage(errorMsg, LOG::LOG_ERROR);
        QMetaObject::invokeMethod(this, "onTaskFailed", Qt::QueuedConnection,
                                 Q_ARG(QSerialPort::SerialPortError, QSerialPort::UnknownError));
    }
    catch (...) {
        LogUtils::logMessage("任务执行未知异常", LOG::LOG_ERROR);
        QMetaObject::invokeMethod(this, "onTaskFailed", Qt::QueuedConnection,
                                 Q_ARG(QSerialPort::SerialPortError, QSerialPort::UnknownError));
    }
}

void TaskManager::onTaskCompleted() {
    // 发送任务完成信号
    emit taskCompleted();
    
    // 处理下一个任务
    QMetaObject::invokeMethod(this, "processNextTask", Qt::QueuedConnection);
}

void TaskManager::onTaskFailed(QSerialPort::SerialPortError error) {
    // 使用ErrorHandler处理错误
    ErrorHandler::instance().handleSerialPortError(error);
    // 发送任务错误信号
    emit taskError(error);
    // 处理下一个任务
    QMetaObject::invokeMethod(this, "processNextTask", Qt::QueuedConnection);
}

void TaskManager::decrementPendingTasks() {
    QMutexLocker locker(&m_mutex);
    --m_pendingTasks;
}

void TaskManager::setMaxThreads(int nMaxThreads) {
    m_threadPool->setMaxThreadCount(nMaxThreads);
}

int TaskManager::activeThreadCount() const {
    return m_threadPool->activeThreadCount();
}

int TaskManager::maxThreadCount() const {
    return m_threadPool->maxThreadCount();
}

void TaskManager::clearTasks() {
    QMutexLocker locker(&m_taskMutex);
    m_threadPool->clear();
    m_pendingTasks = 0;
}
