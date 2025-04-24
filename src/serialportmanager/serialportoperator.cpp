#include "serialportoperator.h"
#include <QDebug>

SerialPortOperator::SerialPortOperator(QObject *parent) : QObject(parent) {}

bool SerialPortOperator::open(QSerialPort* serialPort) {
    if (!serialPort) {
        qWarning() << "Serial port pointer is null";
        return false;
    }

    if (serialPort->isOpen()) {
        qWarning() << "Serial port is already open:" << serialPort->portName();
        return true;
    }

    if (serialPort->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial port opened successfully:" << serialPort->portName();
        return true;
    }

    qWarning() << "Failed to open serial port:" << serialPort->portName();
    return false;
}

void SerialPortOperator::close(QSerialPort* serialPort) {
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
        qDebug() << "Serial port closed:" << serialPort->portName();
    }
    else {
        qWarning() << "Serial port is not open or invalid";
    }
}

bool SerialPortOperator::write(QSerialPort* serialPort, const QByteArray &data) {
    if (!serialPort) {
        qWarning() << "Serial port pointer is null";
        return false;
    }

    if (!serialPort->isOpen() || !serialPort->isWritable()) {
        qWarning() << "Serial port is not open or not writable:" << serialPort->portName();
        return false;
    }

    qint64 bytesWritten = serialPort->write(data);
    if (bytesWritten <= 0) {
        qWarning() << "Failed to write data to serial port:" << serialPort->portName();
        return false;
    }

    if (!serialPort->waitForBytesWritten()) {
        qWarning() << "Failed to wait for data to be written:" << serialPort->portName();
        return false;
    }

    qDebug() << "Data written to serial port:" << data;
    return true;
}

QByteArray SerialPortOperator::read(QSerialPort* serialPort) {
    if (!serialPort) {
        qWarning() << "Serial port pointer is null";
        return QByteArray();
    }

    if (!serialPort->isOpen() || !serialPort->isReadable()) {
        qWarning() << "Serial port is not open or not readable:" << serialPort->portName();
        return QByteArray();
    }

    QByteArray data = serialPort->readAll();
    qDebug() << "Data read from serial port:" << data;
    return data;
}

bool SerialPortOperator::isOpen(const QSerialPort* serialPort) const {
    if (!serialPort) {
        qWarning() << "Serial port pointer is null";
        return false;
    }
    return serialPort->isOpen();
}
