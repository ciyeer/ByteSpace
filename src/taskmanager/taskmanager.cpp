#include "taskmanager.h"
#include <QDebug>
#include <QMetaObject>
#include <QtConcurrent>
#include "logmanager/logutils.h"
#include "utils/errorhandler.h"
#include "utils/configmanager.h"  // 添加头文件

TaskManager::TaskManager(QObject *parent) 
    : QObject(parent)
    , m_threadPool(new QThreadPool(this))
    , m_pendingTasks(0)
    , m_isProcessing(false) {
    // 从配置管理器获取线程池大小
    m_threadPool->setMaxThreadCount(ConfigManager::instance().maxThreadCount());
    
    // 监听配置变化
    connect(&ConfigManager::instance(), &ConfigManager::configChanged, this, [this](const QString& key) {
        if (key == "ThreadPool/MaxThreadCount") {
            m_threadPool->setMaxThreadCount(ConfigManager::instance().maxThreadCount());
            LogUtils::logMessage(QString("线程池大小已更新为: %1").arg(m_threadPool->maxThreadCount()), LOG::LOG_INFO);
        }
    });
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
    QMutexLocker locker(&m_queueMutex);  
    
    // 连接任务信号到管理器
    connect(task.get(), &BytespaceTask::taskCompleted, this, &TaskManager::onTaskCompleted);
    connect(task.get(), &BytespaceTask::taskFailed, this, &TaskManager::onTaskFailed);
    
    // 如果当前没有正在处理的任务，直接提交到线程池
    if (!m_isProcessing) {
        m_isProcessing = true;
        locker.unlock(); // 解锁，避免在执行任务时长时间持有锁
        m_threadPool->start(task.get());
        return;
    }
    
    // 否则添加到队列
    m_taskQueue.enqueue(task);
}

void TaskManager::processNextTask() {
    QMutexLocker locker(&m_queueMutex);  // 使用新名称
    
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
        // 使用与BytespaceTask::run相同的错误处理方式
        QString errorMsg = "任务指针为空";
        // 使用ErrorHandler处理错误
        ErrorHandler::instance().handleError(ErrorType::Application, errorMsg, ErrorLevel::Error);
        // 记录日志
        LogUtils::logMessage(errorMsg, LOG::LOG_ERROR);
        // 发送失败信号
        emit taskFailed(QSerialPort::ResourceError);
        // 处理下一个任务
        QMetaObject::invokeMethod(this, "processNextTask", Qt::QueuedConnection);
        return;
    }

    locker.unlock(); // 解锁，避免在执行任务时长时间持有锁
    // 直接使用QThreadPool运行任务，让任务的run方法处理异常
    m_threadPool->start(task.get());
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
    QMutexLocker locker(&m_queueMutex);  // 使用新名称
    --m_pendingTasks;
}

void TaskManager::setMaxThreads(int nMaxThreads) {
    // 更新线程池配置
    m_threadPool->setMaxThreadCount(nMaxThreads);
    
    // 同时更新配置管理器中的值
    ConfigManager::instance().setMaxThreadCount(nMaxThreads);
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
