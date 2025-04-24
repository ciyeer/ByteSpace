#ifndef SERIALPORTREADTASK_H
#define SERIALPORTREADTASK_H

#include "serialporttask.h"

class SerialPortReadTask : public SerialPortTask {
    Q_OBJECT
public:
    explicit SerialPortReadTask(std::shared_ptr<SerialPortManager> manager, int priority = 0);
    
    bool execute() override;
    
signals:
    void dataRead(const QByteArray& data);
};

#endif // SERIALPORTREADTASK_H