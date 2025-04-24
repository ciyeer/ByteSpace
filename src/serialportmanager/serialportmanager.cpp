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
#include "utils/errorhandler.h"
#include "utils/configmanager.h"

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject(parent)
    , m_serialPort(new QSerialPort(this))
    , m_baudRate(QSerialPort::Baud9600)
    , m_dataBits(QSerialPort::Data8)
    , m_parity(QSerialPort::NoParity)
    , m_stopBits(QSerialPort::OneStop)
    , m_flowControl(QSerialPort::NoFlowControl) {
    
    // 连接错误信号
    connect(m_serialPort, &QSerialPort::errorOccurred, this, &SerialPortManager::handleError, Qt::QueuedConnection);
}

SerialPortManager::~SerialPortManager() {
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

bool SerialPortManager::initialize(QComboBox* portBox, QComboBox* baudrateBox,
                                 QComboBox* databitsBox, QComboBox* parityBox,
                                 QComboBox* stopBitsBox, QComboBox* flowControlBox) {
    try {
        // 先获取可用串口列表并记录日志
        QString portsInfo = getAvailablePortsInfo();
        qDebug() << "初始化串口管理器，可用串口:" << portsInfo;
        LogUtils::logMessage("可用串口: " + portsInfo, LOG::LOG_DEBUG);

        // 直接调用配置器的初始化方法
        m_configurator.initialize(portBox, baudrateBox, databitsBox,
                                parityBox, stopBitsBox, flowControlBox);

        // 尝试从配置管理器加载配置
        if (configurePort()) {
            LogUtils::logMessage("从配置管理器加载串口配置成功", LOG::LOG_DEBUG);
            
            // 更新UI控件以匹配配置
            if (portBox) {
                int index = portBox->findText(m_serialPort->portName());
                if (index >= 0) {
                    portBox->setCurrentIndex(index);
                }
            }
        }
            
        // 设置默认串口参数
        // 确保设置初始串口名称
        if (portBox && portBox->count() > 0) {
            m_portName = portBox->currentText();
            m_serialPort->setPortName(m_portName); // 将串口名称设置到m_serialPort对象
            LogUtils::logMessage("设置串口名称: " + m_portName, LOG::LOG_DEBUG);
        }
        else {
            LogUtils::logMessage("无可用串口", LOG::LOG_ERROR);
            return false;
        }

        // 配置其他串口参数
        if (baudrateBox) {
            bool ok;
            int baudRate = baudrateBox->currentText().toInt(&ok);
            if (ok) {
                m_baudRate = static_cast<QSerialPort::BaudRate>(baudRate);
                m_serialPort->setBaudRate(baudRate);
            }
        }

        if (databitsBox && databitsBox->currentData().isValid()) {
            m_dataBits = static_cast<QSerialPort::DataBits>(databitsBox->currentData().toInt());
            m_serialPort->setDataBits(m_dataBits);
        }

        if (parityBox && parityBox->currentData().isValid()) {
            m_parity = static_cast<QSerialPort::Parity>(parityBox->currentData().toInt());
            m_serialPort->setParity(m_parity);
        }

        if (stopBitsBox && stopBitsBox->currentData().isValid()) {
            m_stopBits = static_cast<QSerialPort::StopBits>(stopBitsBox->currentData().toInt());
            m_serialPort->setStopBits(m_stopBits);
        }

        if (flowControlBox && flowControlBox->currentData().isValid()) {
            m_flowControl = static_cast<QSerialPort::FlowControl>(flowControlBox->currentData().toInt());
            m_serialPort->setFlowControl(m_flowControl);
        }

        // 验证串口配置
        QString portName = m_serialPort->portName();
        if (portName.isEmpty()) {
            LogUtils::logMessage("串口名称未设置", LOG::LOG_ERROR);
            return false;
        }

        LogUtils::logMessage("串口初始化成功: " + portName, LOG::LOG_DEBUG);
        return true;
    }
    catch (const std::exception& e) {
        QString errorMsg = QString("初始化串口管理器失败: %1").arg(e.what());
        LogUtils::logMessage(errorMsg, LOG::LOG_ERROR);
        return false;
    }
}

void SerialPortManager::configureSerialPort(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                                          QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                                          QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl) {
    if (!comBoxPortName || comBoxPortName->currentText().isEmpty()) {
        LogUtils::logMessage("无效的串口选择", LOG::LOG_ERROR);
        return;
    }

    QString portName = comBoxPortName->currentText();
    LogUtils::logMessage("正在配置串口: " + portName, LOG::LOG_DEBUG);

    // 配置串口参数
    m_serialPort->setPortName(portName);

    if (comBoxBaudRate) {
        m_serialPort->setBaudRate(comBoxBaudRate->currentText().toInt());
    }

    if (comBoxDataBits) {
        m_serialPort->setDataBits(static_cast<QSerialPort::DataBits>(comBoxDataBits->currentData().toInt()));
    }

    if (comBoxParity) {
        m_serialPort->setParity(static_cast<QSerialPort::Parity>(comBoxParity->currentData().toInt()));
    }

    if (comBoxStopBits) {
        m_serialPort->setStopBits(static_cast<QSerialPort::StopBits>(comBoxStopBits->currentData().toInt()));
    }

    if (comBoxFlowControl) {
        m_serialPort->setFlowControl(static_cast<QSerialPort::FlowControl>(comBoxFlowControl->currentData().toInt()));
    }

    LogUtils::logMessage("串口配置完成: " + portName, LOG::LOG_DEBUG);
}

bool SerialPortManager::openPort() {
    try {
        if (!m_serialPort) {
            LogUtils::logMessage("串口对象未初始化", LOG::LOG_ERROR);
            return false;
        }

        // 检查串口名称是否为空
        if (m_portName.isEmpty()) {
            LogUtils::logMessage("串口名称为空", LOG::LOG_ERROR);
            emit serialPortError(QSerialPort::DeviceNotFoundError);
            return false;
        }

        // 先关闭已打开的串口
        if (m_serialPort->isOpen()) {
            m_serialPort->close();
            LogUtils::logMessage("关闭已打开的串口", LOG::LOG_INFO);
        }

        // 从配置管理器获取最新配置
        if (!configurePort()) {
            LogUtils::logMessage("配置串口失败", LOG::LOG_ERROR);
            return false;
        }

        LogUtils::logMessage("正在尝试打开串口: " + m_portName, LOG::LOG_INFO);
        
        // 直接尝试打开串口
        bool opened = m_serialPort->open(QIODevice::ReadWrite);
        
        if (!opened) {
            LogUtils::logMessage("无法打开串口: " + m_portName + ", 错误: " + m_serialPort->errorString(), LOG::LOG_ERROR);
            emit serialPortError(m_serialPort->error());
            return false;
        }

        LogUtils::logMessage("串口打开成功: " + m_portName, LOG::LOG_INFO);
        emit portOpened();
        return true;
    }
    catch (const std::exception& e) {
        LogUtils::logMessage(QString("打开串口异常: %1").arg(e.what()), LOG::LOG_ERROR);
        emit serialPortError(QSerialPort::UnknownError);
        return false;
    }
}

void SerialPortManager::closePort() {
    if (m_serialPort->isOpen()) {
        LogUtils::logMessage("正在关闭串口: " + m_portName, LOG::LOG_INFO);
        m_serialPort->close();
        LogUtils::logMessage("串口已关闭: " + m_portName, LOG::LOG_INFO);
        emit portClosed();
    } 
    else {
        LogUtils::logMessage("串口已经是关闭状态: " + m_portName, LOG::LOG_INFO);
    }
}

bool SerialPortManager::writeData(const QByteArray& data) {
    if (!m_serialPort || !m_serialPort->isOpen()) {
        emit serialPortError(QSerialPort::NotOpenError);
        return false;
    }

    if (m_serialPort->write(data) == -1) {
        emit serialPortError(m_serialPort->error());
        return false;
    }

    emit dataWritten(data);
    return true;
}

QByteArray SerialPortManager::readData() {
    if (!m_serialPort || !m_serialPort->isOpen() || !m_serialPort->isReadable()) {
        return QByteArray();
    }
    
    QByteArray data = m_serialPort->readAll();
    if (!data.isEmpty()) {
        emit dataRead(data);  // 确保这行代码存在
        LogUtils::logMessage(QString("读取到 %1 字节数据").arg(data.size()), LOG::LOG_INFO);
    }
    return data;
}

void SerialPortManager::handleError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) {
        return;
    }
    
    // 对于串口移除错误，减少日志频率
    static QDateTime lastErrorTime = QDateTime::currentDateTime().addSecs(-10);
    if (error == QSerialPort::ResourceError) { // 串口被移除的错误
        QDateTime currentTime = QDateTime::currentDateTime();
        if (lastErrorTime.secsTo(currentTime) < 5) {
            // 如果距离上次错误不到5秒，不处理
            return;
        }
        lastErrorTime = currentTime;
    }
    
    // 使用ErrorHandler处理错误
    ErrorHandler::instance().handleSerialPortError(error);
    
    // 仍然发送原始错误信号，保持向后兼容
    emit serialPortError(error);
}

bool SerialPortManager::isOpen() const {
    return m_serialPort->isOpen();
}

bool SerialPortManager::isWritable() const {
    return m_serialPort->isWritable();
}

bool SerialPortManager::isReadable() const {
    return m_serialPort->isReadable();
}

QSerialPort::SerialPortError SerialPortManager::getError() const {
    return m_serialPort->error();  // 返回 QSerialPort::SerialPortError 类型的错误
}

// 获取和设置串口信息
QString SerialPortManager::getPortName() const {
    return m_portName;
}

void SerialPortManager::setPortName(const QString& portName) {
    m_portName = portName;
    LogUtils::logMessage("设置串口名称: " + portName, LOG::LOG_DEBUG);
}

// 获取和设置串口参数
int SerialPortManager::getBaudRate() const {
    return m_serialPort->baudRate();
}

void SerialPortManager::setBaudRate(int baudRate) {
    m_serialPort->setBaudRate(baudRate);
}

QSerialPort::DataBits SerialPortManager::getDataBits() const {
    return m_serialPort->dataBits();
}

void SerialPortManager::setDataBits(QSerialPort::DataBits dataBits) {
    m_serialPort->setDataBits(dataBits);
}

QSerialPort::Parity SerialPortManager::getParity() const {
    return m_serialPort->parity();
}

void SerialPortManager::setParity(QSerialPort::Parity parity) {
    m_serialPort->setParity(parity);
}

QSerialPort::StopBits SerialPortManager::getStopBits() const {
    return m_serialPort->stopBits();
}

void SerialPortManager::setStopBits(QSerialPort::StopBits stopBits) {
    m_serialPort->setStopBits(stopBits);
}

QSerialPort::FlowControl SerialPortManager::getFlowControl() const {
    return m_serialPort->flowControl();
}

void SerialPortManager::setFlowControl(QSerialPort::FlowControl flowControl) {
    m_serialPort->setFlowControl(flowControl);
}

QString SerialPortManager::getAvailablePortsInfo() const {
    QStringList portList;
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        portList << QString("%1 [%2]").arg(info.portName(), info.description());
    }
    return portList.join(", ");
}

bool SerialPortManager::isValidPort(const QString &portName) const {
    if (portName.isEmpty()) {
        return false;
    }

    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (info.portName() == portName) {
            return true;
        }
    }
    return false;
}

void SerialPortManager::startReading() {
    if (!m_serialPort || !m_serialPort->isOpen()) {
        LogUtils::logMessage("串口未打开，无法启动读取", LOG::LOG_ERROR);
        return;
    }
    
    // 确保已连接readyRead信号
    disconnect(m_serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);
    connect(m_serialPort, &QSerialPort::readyRead, this, &SerialPortManager::handleReadyRead);
    
    LogUtils::logMessage("串口数据读取监听已启动", LOG::LOG_INFO);
}

void SerialPortManager::handleReadyRead() {
    if (!m_serialPort || !m_serialPort->isOpen()) {
        return;
    }
    
    // 读取所有可用数据
    QByteArray data = m_serialPort->readAll();
    if (!data.isEmpty()) {
        // 发出数据读取信号
        emit dataRead(data);
    }
}

bool SerialPortManager::configurePort() {
    // 从配置管理器获取串口配置
    QString portName = ConfigManager::instance().portName();
    int baudRate = ConfigManager::instance().baudRate();
    int dataBits = ConfigManager::instance().dataBits();
    int stopBits = ConfigManager::instance().stopBits();
    QString parityStr = ConfigManager::instance().parity();
    
    // 设置串口参数
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);
    
    // 设置数据位
    QSerialPort::DataBits dataBitsEnum;
    switch (dataBits) {
        case 5: dataBitsEnum = QSerialPort::Data5; break;
        case 6: dataBitsEnum = QSerialPort::Data6; break;
        case 7: dataBitsEnum = QSerialPort::Data7; break;
        default: dataBitsEnum = QSerialPort::Data8; break;
    }
    m_serialPort->setDataBits(dataBitsEnum);
    
    // 设置停止位
    QSerialPort::StopBits stopBitsEnum;
    switch (stopBits) {
        case 2: stopBitsEnum = QSerialPort::TwoStop; break;
        case 3: stopBitsEnum = QSerialPort::OneAndHalfStop; break;
        default: stopBitsEnum = QSerialPort::OneStop; break;
    }
    m_serialPort->setStopBits(stopBitsEnum);
    
    // 设置校验位
    QSerialPort::Parity parityEnum = QSerialPort::NoParity;
    if (parityStr == "Even") {
        parityEnum = QSerialPort::EvenParity;
    }
    else if (parityStr == "Odd") {
        parityEnum = QSerialPort::OddParity;
    }
    else if (parityStr == "Space") {
        parityEnum = QSerialPort::SpaceParity;
    }
    else if (parityStr == "Mark") {
        parityEnum = QSerialPort::MarkParity;
    }
    m_serialPort->setParity(parityEnum);
    
    // 更新成员变量
    m_portName = portName;
    m_baudRate = static_cast<QSerialPort::BaudRate>(baudRate);
    m_dataBits = dataBitsEnum;
    m_parity = parityEnum;
    m_stopBits = stopBitsEnum;
    
    LogUtils::logMessage("从配置管理器加载串口配置: " + portName, LOG::LOG_DEBUG);
    return true;
}
