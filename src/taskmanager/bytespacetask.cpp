#include "bytespacetask.h"
#include <QSerialPortInfo>
#include <QMetaObject>
#include <QElapsedTimer>
#include <QDebug>
#include "logmanager/logutils.h"
#include "utils/errorhandler.h"

// 只需要基类的基本实现
BytespaceTask::BytespaceTask(int priority)
    : m_priority(priority) {
    // 生命周期由TaskManager管理，避免与shared_ptr产生双重释放风险
    setAutoDelete(false);
}

void BytespaceTask::run() {
    try {
        if (execute()) {
            emit taskCompleted();
        }
    }
    catch (const std::exception& e) {
        QString errorMsg = QString("任务执行异常: %1").arg(e.what());
        // 使用ErrorHandler处理错误
        ErrorHandler::instance().handleError(ErrorType::Application, errorMsg, ErrorLevel::Error);
        // 仍然记录日志并发送失败信号
        LogUtils::logMessage(errorMsg, LOG::LOG_ERROR);
        emit taskFailed(QSerialPort::UnknownError);
    }
}
