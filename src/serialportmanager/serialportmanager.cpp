/**
 * @file serialportmanager.cpp
 * @brief 串口管理器实现
 *
 * @author ByteSpace团队
 * @date 2024
 */
#include "serialportmanager.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QComboBox>
#include "logmanager/logutils.h"

SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent) {
    QSerialPort* serialPort = m_configurator.serialPort();
    connect(serialPort, &QSerialPort::errorOccurred, this, &SerialPortManager::handleError);
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);
}

SerialPortManager::~SerialPortManager() {
}

bool SerialPortManager::initialize(QComboBox *portBox, QComboBox *baudrateBox,
                                  QComboBox *databitsBox, QComboBox *parityBox,
                                  QComboBox *stopBitsBox, QComboBox *flowControlBox) {
    m_configurator.initialize(portBox, baudrateBox, databitsBox, parityBox, stopBitsBox, flowControlBox);
    return m_configurator.isInitialized();
}

void SerialPortManager::configureSerialPort(const QString& portName, int baudRate,
                                           QSerialPort::DataBits dataBits, QSerialPort::Parity parity,
                                           QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl) {
    m_configurator.setPortName(portName);
    m_configurator.setBaudRate(baudRate);
    m_configurator.setDataBits(dataBits);
    m_configurator.setParity(parity);
    m_configurator.setStopBits(stopBits);
    m_configurator.setFlowControl(flowControl);
}

bool SerialPortManager::configurePort() {
    return m_configurator.configureFromSettings();
}

bool SerialPortManager::openPort() {
    if (m_operator.open(m_configurator.serialPort())) {
        emit portOpened();
        return true;
    }
    return false;
}

void SerialPortManager::closePort() {
    m_operator.close(m_configurator.serialPort());
    emit portClosed();
}

bool SerialPortManager::writeData(const QByteArray &data) {
    if (m_operator.write(m_configurator.serialPort(), data)) {
        emit dataWritten(data);
        return true;
    }
    return false;
}

QByteArray SerialPortManager::readData() {
    QByteArray data = m_operator.read(m_configurator.serialPort());
    if (!data.isEmpty()) {
        emit dataRead(data);
    }
    return data;
}

bool SerialPortManager::isOpen() const {
    return m_operator.isOpen(m_configurator.serialPort());
}

bool SerialPortManager::isWritable() const {
    return m_configurator.serialPort()->isWritable();
}

bool SerialPortManager::isReadable() const {
    return m_configurator.serialPort()->isReadable();
}

QString SerialPortManager::getPortName() const {
    return m_configurator.getPortName();
}

QSerialPort::SerialPortError SerialPortManager::getError() const {
    return m_configurator.getError();
}

void SerialPortManager::startReading() {
    QSerialPort* serialPort = m_configurator.serialPort();
    if (!serialPort || !serialPort->isOpen()) {
        LogUtils::logMessage("串口未打开，无法启动读取", LOG::LOG_ERROR);
        return;
    }

    disconnect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);

    LogUtils::logMessage("串口数据读取监听已启动", LOG::LOG_INFO);
}

void SerialPortManager::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) {
        return;
    }

    QString errorMessage = QString("串口错误: %1").arg(error);
    LogUtils::logMessage(errorMessage, LOG::LOG_ERROR);

    emit serialPortError(error);

    if (error == QSerialPort::ResourceError ||
        error == QSerialPort::WriteError ||
        error == QSerialPort::ReadError) {
        closePort();
        LogUtils::logMessage("由于严重错误，串口已关闭", LOG::LOG_WARNING);
    }
}

void SerialPortManager::handleReadyRead() {
    QByteArray data = readData();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}
