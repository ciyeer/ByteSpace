#ifndef SERIALPORTOPERATOR_H
#define SERIALPORTOPERATOR_H

#include <QObject>
#include <QSerialPort>

class SerialPortOperator : public QObject {
    Q_OBJECT
public:
    explicit SerialPortOperator(QObject *parent = nullptr);
    bool open(QSerialPort* serialPort);
    void close(QSerialPort* serialPort);
    bool write(QSerialPort* serialPort, const QByteArray &data);
    QByteArray read(QSerialPort* serialPort);
    bool isOpen(const QSerialPort* serialPort) const;
};

#endif // SERIALPORTOPERATOR_H
