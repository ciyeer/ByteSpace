#include "serialportmanager.h"
#include <QSerialPortInfo>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QEventLoop>
#include <QTimer>
#include <QComboBox>
#include <QThread>
#include <QDebug>
#include "logmanager/logutils.h"

SerialPortManager::SerialPortManager(QObject *parent) : QObject(parent) {
    // 不再需要创建 m_serialPort，由 m_configurator 管理
    // 连接错误信号
    QSerialPort* serialPort = m_configurator.serialPort();
    connect(serialPort, &QSerialPort::errorOccurred, this, &SerialPortManager::handleError);
    connect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);
}

SerialPortManager::~SerialPortManager() {
    // 不再需要手动释放 m_serialPort，由 m_configurator 管理
}

bool SerialPortManager::initialize(QComboBox *portBox, QComboBox *baudrateBox,
                                  QComboBox *databitsBox, QComboBox *parityBox,
                                  QComboBox *stopBitsBox, QComboBox *flowControlBox) {
    // 委托给 m_configurator
    m_configurator.initialize(portBox, baudrateBox, databitsBox, parityBox, stopBitsBox, flowControlBox);
    return m_configurator.isInitialized();
}

void SerialPortManager::configureSerialPort(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                                           QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                                           QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl) {
    // 委托给 m_configurator
    m_configurator.configure(comBoxPortName, comBoxBaudRate, comBoxDataBits, comBoxParity, 
                            comBoxStopBits, comBoxFlowControl);
}

bool SerialPortManager::openPort() {
    // 使用 m_operator 打开 m_configurator 管理的串口
    if (m_operator.open(m_configurator.serialPort())) {
        emit portOpened();
        return true;
    }
    return false;
}

void SerialPortManager::closePort() {
    // 使用 m_operator 关闭 m_configurator 管理的串口
    m_operator.close(m_configurator.serialPort());
    emit portClosed();
}

bool SerialPortManager::writeData(const QByteArray &data) {
    // 使用 m_operator 写入数据到 m_configurator 管理的串口
    if (m_operator.write(m_configurator.serialPort(), data)) {
        emit dataWritten(data);
        return true;
    }
    return false;
}

QByteArray SerialPortManager::readData() {
    // 使用 m_operator 从 m_configurator 管理的串口读取数据
    QByteArray data = m_operator.read(m_configurator.serialPort());
    if (!data.isEmpty()) {
        emit dataRead(data);
    }
    return data;
}

bool SerialPortManager::configurePort() {
    // 委托给 m_configurator
    return m_configurator.configureFromSettings();
}

bool SerialPortManager::isOpen() const {
    // 使用 m_operator 检查 m_configurator 管理的串口状态
    return m_operator.isOpen(m_configurator.serialPort());
}

bool SerialPortManager::isWritable() const {
    return m_configurator.serialPort()->isWritable();
}

bool SerialPortManager::isReadable() const {
    return m_configurator.serialPort()->isReadable();
}

// 设置串口参数 - 委托给 m_configurator
void SerialPortManager::setPortName(const QString& portName) {
    m_configurator.setPortName(portName);
    LogUtils::logMessage("设置串口名称: " + portName, LOG::LOG_DEBUG);
}

void SerialPortManager::setBaudRate(int baudRate) {
    m_configurator.setBaudRate(baudRate);
}

void SerialPortManager::setDataBits(QSerialPort::DataBits dataBits) {
    m_configurator.setDataBits(dataBits);
}

void SerialPortManager::setParity(QSerialPort::Parity parity) {
    m_configurator.setParity(parity);
}

void SerialPortManager::setStopBits(QSerialPort::StopBits stopBits) {
    m_configurator.setStopBits(stopBits);
}

void SerialPortManager::setFlowControl(QSerialPort::FlowControl flowControl) {
    m_configurator.setFlowControl(flowControl);
}

// 获取串口信息 - 委托给 m_configurator
QString SerialPortManager::getPortName() const {
    return m_configurator.getPortName();
}

int SerialPortManager::getBaudRate() const {
    return m_configurator.getBaudRate();
}

QSerialPort::DataBits SerialPortManager::getDataBits() const {
    return m_configurator.getDataBits();
}

QSerialPort::Parity SerialPortManager::getParity() const {
    return m_configurator.getParity();
}

QSerialPort::StopBits SerialPortManager::getStopBits() const {
    return m_configurator.getStopBits();
}

QSerialPort::FlowControl SerialPortManager::getFlowControl() const {
    return m_configurator.getFlowControl();
}

QSerialPort::SerialPortError SerialPortManager::getError() const {
    return m_configurator.getError();
}

QString SerialPortManager::getAvailablePortsInfo() const {
    return m_configurator.getAvailablePortsInfo();
}

bool SerialPortManager::isValidPort(const QString &portName) const {
    return m_configurator.isValidPort(portName);
}

void SerialPortManager::startReading() {
    QSerialPort* serialPort = m_configurator.serialPort();
    if (!serialPort || !serialPort->isOpen()) {
        LogUtils::logMessage("串口未打开，无法启动读取", LOG::LOG_ERROR);
        return;
    }
    
    // 只在未连接时才连接信号
    if (!serialPort->receivers(SIGNAL(readyRead()))) {
        connect(serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);
        LogUtils::logMessage("串口数据读取监听已启动", LOG::LOG_INFO);
    }
}

void SerialPortManager::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) {
        return;  // 忽略无错误的情况
    }
    
    // 记录错误信息
    QString errorMessage = QString("串口错误: %1").arg(error);
    LogUtils::logMessage(errorMessage, LOG::LOG_ERROR);
    
    // 发送错误信号
    emit serialPortError(error);
    
    // 对于严重错误，可能需要关闭串口
    if (error == QSerialPort::ResourceError || 
        error == QSerialPort::WriteError ||
        error == QSerialPort::ReadError) {
        closePort();
        LogUtils::logMessage("由于严重错误，串口已关闭", LOG::LOG_WARNING);
    }
}

void SerialPortManager::handleReadyRead() {
    // 数据接收逻辑保持不变
    QByteArray data = readData();
    if (!data.isEmpty()) {
        emit dataReceived(data);
    }
}
