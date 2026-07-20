#include "serialportconfigurator.h"
#include "utils/configmanager.h"
#include <QSerialPortInfo>

SerialPortConfigurator::SerialPortConfigurator(QObject *parent) : QObject(parent) {
    m_serialPort = new QSerialPort(this);
}

SerialPortConfigurator::~SerialPortConfigurator() {
    // 确保串口资源被正确释放
    if (m_serialPort->isOpen()) {
        m_serialPort->close();
    }
}

void SerialPortConfigurator::initialize(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                                        QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                                        QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl) {
    // 先清空所有 ComboBox
    comBoxPortName->clear();
    comBoxBaudRate->clear();
    comBoxDataBits->clear();
    comBoxParity->clear();
    comBoxStopBits->clear();
    comBoxFlowControl->clear();

    // 获取所有可用端口
    for (const auto &portInfo : QSerialPortInfo::availablePorts()) {
        comBoxPortName->addItem(portInfo.portName());
    }

    // 波特率选项
    QList<QPair<QString, int>> baudRates {
        {"9600", QSerialPort::Baud9600},
        {"19200", QSerialPort::Baud19200},
        {"38400", QSerialPort::Baud38400},
        {"57600", QSerialPort::Baud57600},
        {"115200", QSerialPort::Baud115200}
    };
    for (const auto &baudRate : baudRates) {
        comBoxBaudRate->addItem(baudRate.first, baudRate.second);
    }

    // 数据位选项
    QList<QPair<QString, QSerialPort::DataBits>> dataBits {
        {"8", QSerialPort::Data8},
        {"7", QSerialPort::Data7},
        {"6", QSerialPort::Data6},
        {"5", QSerialPort::Data5}
    };
    for (const auto &dataBit : dataBits) {
        comBoxDataBits->addItem(dataBit.first, dataBit.second);
    }

    // 校验位选项
    QList<QPair<QString, QSerialPort::Parity>> parityBits {
        {"NONE", QSerialPort::NoParity},
        {"ODD", QSerialPort::OddParity},
        {"EVEN", QSerialPort::EvenParity},
        {"MARK", QSerialPort::MarkParity},
        {"SPACE", QSerialPort::SpaceParity}
    };
    for (const auto &parityBit : parityBits) {
        comBoxParity->addItem(parityBit.first, parityBit.second);
    }

    // 停止位选项
    QList<QPair<QString, QSerialPort::StopBits>> stopBits {
        {"1", QSerialPort::OneStop},
        {"1.5", QSerialPort::OneAndHalfStop},
        {"2", QSerialPort::TwoStop}
    };
    for (const auto &stopBit : stopBits) {
        comBoxStopBits->addItem(stopBit.first, stopBit.second);
    }

    // 流控制选项
    QList<QPair<QString, QSerialPort::FlowControl>> flowControl {
        {"NONE", QSerialPort::NoFlowControl},
        {"RTS/CTS", QSerialPort::HardwareControl},
        {"XON/XOFF", QSerialPort::SoftwareControl}
    };
    for (const auto &flowCtl : flowControl) {
        comBoxFlowControl->addItem(flowCtl.first, flowCtl.second);
    }
    // 设置初始UI状态（例如禁用串口设置，直到选择端口）
    m_isInitialized = true;

    // 从 ConfigManager 恢复上次保存的配置
    QString savedPortName = ConfigManager::instance().portName();
    int savedBaudRate = ConfigManager::instance().baudRate();
    int savedDataBits = ConfigManager::instance().dataBits();
    int savedStopBits = ConfigManager::instance().stopBits();
    QString savedParity = ConfigManager::instance().parity();
    QString savedFlowControl = ConfigManager::instance().flowControl();

    // 恢复端口名（如果仍可用）
    int portIdx = comBoxPortName->findText(savedPortName);
    if (portIdx >= 0) {
        comBoxPortName->setCurrentIndex(portIdx);
    }

    // 恢复波特率
    int baudIdx = comBoxBaudRate->findData(savedBaudRate);
    if (baudIdx >= 0) {
        comBoxBaudRate->setCurrentIndex(baudIdx);
    }

    // 恢复数据位
    int dataIdx = comBoxDataBits->findData(static_cast<QSerialPort::DataBits>(savedDataBits));
    if (dataIdx >= 0) {
        comBoxDataBits->setCurrentIndex(dataIdx);
    }

    // 恢复校验位
    QSerialPort::Parity savedParityEnum = QSerialPort::NoParity;
    if (savedParity == "None") savedParityEnum = QSerialPort::NoParity;
    else if (savedParity == "Even") savedParityEnum = QSerialPort::EvenParity;
    else if (savedParity == "Odd") savedParityEnum = QSerialPort::OddParity;
    else if (savedParity == "Mark") savedParityEnum = QSerialPort::MarkParity;
    else if (savedParity == "Space") savedParityEnum = QSerialPort::SpaceParity;
    int parityIdx = comBoxParity->findData(savedParityEnum);
    if (parityIdx >= 0) {
        comBoxParity->setCurrentIndex(parityIdx);
    }

    // 恢复停止位
    QSerialPort::StopBits savedStopBitsEnum;
    if (savedStopBits == 1) savedStopBitsEnum = QSerialPort::OneStop;
    else if (savedStopBits == 3) savedStopBitsEnum = QSerialPort::OneAndHalfStop;
    else savedStopBitsEnum = QSerialPort::TwoStop;
    int stopIdx = comBoxStopBits->findData(savedStopBitsEnum);
    if (stopIdx >= 0) {
        comBoxStopBits->setCurrentIndex(stopIdx);
    }

    // 恢复流控制
    QSerialPort::FlowControl savedFlowEnum = QSerialPort::NoFlowControl;
    if (savedFlowControl == "NONE") savedFlowEnum = QSerialPort::NoFlowControl;
    else if (savedFlowControl == "RTS/CTS") savedFlowEnum = QSerialPort::HardwareControl;
    else if (savedFlowControl == "XON/XOFF") savedFlowEnum = QSerialPort::SoftwareControl;
    int flowIdx = comBoxFlowControl->findData(savedFlowEnum);
    if (flowIdx >= 0) {
        comBoxFlowControl->setCurrentIndex(flowIdx);
    }
}

void SerialPortConfigurator::configure(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                                       QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                                       QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl) {
    m_serialPort->setPortName(comBoxPortName->currentText());
    m_serialPort->setBaudRate(comBoxBaudRate->currentText().toInt());
    m_serialPort->setDataBits(mapDataBits(comBoxDataBits->currentIndex()));
    m_serialPort->setParity(mapParity(comBoxParity->currentIndex()));
    m_serialPort->setStopBits(mapStopBits(comBoxStopBits->currentIndex()));
    m_serialPort->setFlowControl(mapFlowControl(comBoxFlowControl->currentIndex()));
}

bool SerialPortConfigurator::isInitialized() const { 
    return m_isInitialized; 
}

QSerialPort::DataBits SerialPortConfigurator::mapDataBits(int index) const {
    switch (index) {
    case 0: return QSerialPort::Data8;
    case 1: return QSerialPort::Data7;
    case 2: return QSerialPort::Data6;
    case 3: return QSerialPort::Data5;
    default: return QSerialPort::Data8;  // 默认值
    }
}

QSerialPort::Parity SerialPortConfigurator::mapParity(int index) const {
    switch (index) {
    case 0: return QSerialPort::NoParity;
    case 1: return QSerialPort::OddParity;
    case 2: return QSerialPort::EvenParity;
    case 3: return QSerialPort::MarkParity;
    case 4: return QSerialPort::SpaceParity;
    default: return QSerialPort::NoParity;  // 默认值
    }
}

QSerialPort::StopBits SerialPortConfigurator::mapStopBits(int index) const {
    switch (index) {
    case 0: return QSerialPort::OneStop;
    case 1: return QSerialPort::OneAndHalfStop;
    case 2: return QSerialPort::TwoStop;
    default: return QSerialPort::OneStop;  // 默认值
    }
}

QSerialPort::FlowControl SerialPortConfigurator::mapFlowControl(int index) const {
    switch (index) {
    case 0: return QSerialPort::NoFlowControl;
    case 1: return QSerialPort::HardwareControl;
    case 2: return QSerialPort::SoftwareControl;
    default: return QSerialPort::NoFlowControl;  // 默认值
    }
}

QSerialPort* SerialPortConfigurator::serialPort() const {
    return m_serialPort;
}

void SerialPortConfigurator::setPortName(const QString& portName) {
    m_serialPort->setPortName(portName);
}

void SerialPortConfigurator::setBaudRate(int baudRate) {
    m_serialPort->setBaudRate(baudRate);
}

void SerialPortConfigurator::setDataBits(QSerialPort::DataBits dataBits) {
    m_serialPort->setDataBits(dataBits);
}

void SerialPortConfigurator::setParity(QSerialPort::Parity parity) {
    m_serialPort->setParity(parity);
}

void SerialPortConfigurator::setStopBits(QSerialPort::StopBits stopBits) {
    m_serialPort->setStopBits(stopBits);
}

void SerialPortConfigurator::setFlowControl(QSerialPort::FlowControl flowControl) {
    m_serialPort->setFlowControl(flowControl);
}

QString SerialPortConfigurator::getPortName() const {
    return m_serialPort->portName();
}

int SerialPortConfigurator::getBaudRate() const {
    return m_serialPort->baudRate();
}

QSerialPort::DataBits SerialPortConfigurator::getDataBits() const {
    return m_serialPort->dataBits();
}

QSerialPort::Parity SerialPortConfigurator::getParity() const {
    return m_serialPort->parity();
}

QSerialPort::StopBits SerialPortConfigurator::getStopBits() const {
    return m_serialPort->stopBits();
}

QSerialPort::FlowControl SerialPortConfigurator::getFlowControl() const {
    return m_serialPort->flowControl();
}

QSerialPort::SerialPortError SerialPortConfigurator::getError() const {
    return m_serialPort->error();
}

QString SerialPortConfigurator::getAvailablePortsInfo() const {
    QString info;
    for (const QSerialPortInfo &port : QSerialPortInfo::availablePorts()) {
        info += port.portName() + ": " + port.description() + "\n";
    }
    return info;
}

bool SerialPortConfigurator::isValidPort(const QString &portName) const {
    for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
        if (info.portName() == portName) {
            return true;
        }
    }
    return false;
}

bool SerialPortConfigurator::configureFromSettings() {
    // 从配置管理器获取串口配置
    QString portName = ConfigManager::instance().portName();
    int baudRate = ConfigManager::instance().baudRate();
    int dataBits = ConfigManager::instance().dataBits();
    int stopBits = ConfigManager::instance().stopBits();
    QString parityStr = ConfigManager::instance().parity();

    // 设置串口参数
    m_serialPort->setPortName(portName);
    m_serialPort->setBaudRate(baudRate);

    // 复用 mapDataBits（ConfigManager 存的是 bits 数值，ComboBox index = 8 - value）
    int dataIdx = 8 - dataBits;
    m_serialPort->setDataBits(mapDataBits(dataIdx));

    // 复用 mapStopBits
    static const QHash<int, int> stopBitsToIdx = {{1, 0}, {3, 1}, {2, 2}};
    m_serialPort->setStopBits(mapStopBits(stopBitsToIdx.value(stopBits, 0)));

    // 复用 mapParity（通过字符串名找到 ComboBox index）
    static const QStringList parityOrder = {"NONE", "ODD", "EVEN", "MARK", "SPACE"};
    int parityIdx = parityOrder.indexOf(parityStr.toUpper());
    m_serialPort->setParity(mapParity(parityIdx >= 0 ? parityIdx : 0));

    return true;
}
