#ifndef SERIALPORTWRITETASK_H
#define SERIALPORTWRITETASK_H

#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include <QByteArray>
#include <QMetaObject>
#include "serialporttask.h"

class SerialPortWriteTask : public SerialPortTask {
    Q_OBJECT
public:
    explicit SerialPortWriteTask(std::shared_ptr<SerialPortManager> manager, 
                                const QByteArray& data,
                                int priority = 0);

protected:
    bool execute() override;
};

#endif // SERIALPORTWRITETASK_H
