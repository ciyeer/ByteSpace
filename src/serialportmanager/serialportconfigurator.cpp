#include "serialportconfigurator.h"
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
