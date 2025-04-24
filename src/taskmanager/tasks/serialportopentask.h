#ifndef SERIALPORTOPENTASK_H
#define SERIALPORTOPENTASK_H

#include "serialporttask.h"

// SerialPortOpenTask 专门用于打开串口的任务
class SerialPortOpenTask : public SerialPortTask {
    Q_OBJECT
public:
    explicit SerialPortOpenTask(std::shared_ptr<SerialPortManager> manager, int priority = 0);
    
protected:
    // 实现基类的纯虚函数
    bool execute() override;
};

#endif // SERIALPORTOPENTASK_H
