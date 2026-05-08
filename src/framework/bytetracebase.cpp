#include "bytetracebase.h"
#include "ui_bytetracebase.h"
#include <QComboBox>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QRegularExpression>
#include <QTextBrowser>
#include <QTextEdit>
#include "utils/errorhandler.h"
#include "utils/configmanager.h"
#include "logmanager/logutils.h"

BytetraceBase::BytetraceBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BytetraceBase)
    , m_pLeftBar(nullptr)
    , m_pRecvWidget(nullptr)
    , m_pSendWidget(nullptr)
    , m_serialPortManager(std::make_shared<SerialPortManager>()) {
    ui->setupUi(this);
    m_pLeftBar = new LeftBar;
    m_pRecvWidget = new RecvWidget;
    m_pSendWidget = new SendWidget;

    ui->leftbarLayout->addWidget(m_pLeftBar);
    ui->recvLayout->addWidget(m_pRecvWidget);
    ui->sendLayout->addWidget(m_pSendWidget);

    auto *portBox = m_pLeftBar->findChild<QComboBox*>("portComboBox");
    auto *baudrateBox = m_pLeftBar->findChild<QComboBox*>("baudrateComboBox");
    auto *databitsBox = m_pLeftBar->findChild<QComboBox*>("dataBitsComboBox");
    auto *parityBox = m_pLeftBar->findChild<QComboBox*>("parityComboBox");
    auto *stopBitsBox = m_pLeftBar->findChild<QComboBox*>("stopBitsComboBox");
    auto *flowControlBox = m_pLeftBar->findChild<QComboBox*>("flowControlComboBox");

    if (portBox && baudrateBox && databitsBox && parityBox && stopBitsBox && flowControlBox) {
        m_serialPortManager->initialize(portBox, baudrateBox, databitsBox, parityBox, stopBitsBox, flowControlBox);
    }

    loadSerialConfigToUi();
    setupConnections();
    updateUiState(false);
}

BytetraceBase::~BytetraceBase() {
    if (m_serialPortManager && m_serialPortManager->isOpen()) {
        m_serialPortManager->closePort();
    }
    delete ui;
}

void BytetraceBase::setupConnections() {
    auto *openCloseButton = m_pLeftBar->findChild<QPushButton*>("openCloseButton");
    auto *sendButton = m_pSendWidget->findChild<QPushButton*>("dataSendBtn");
    auto *clearSendButton = m_pSendWidget->findChild<QPushButton*>("clearSendBtn");
    auto *clearRecvButton = m_pSendWidget->findChild<QPushButton*>("clearRecvBtn");

    if (openCloseButton) {
        connect(openCloseButton, &QPushButton::clicked, this, &BytetraceBase::handleOpenClosePort);
    }
    if (sendButton) {
        connect(sendButton, &QPushButton::clicked, this, &BytetraceBase::handleSendData);
    }
    if (clearSendButton) {
        connect(clearSendButton, &QPushButton::clicked, this, &BytetraceBase::handleClearSend);
    }
    if (clearRecvButton) {
        connect(clearRecvButton, &QPushButton::clicked, this, &BytetraceBase::handleClearRecv);
    }

    connect(m_serialPortManager.get(), &SerialPortManager::dataRead, this, &BytetraceBase::onSerialDataReceived);
    connect(m_serialPortManager.get(), &SerialPortManager::serialPortError, this, &BytetraceBase::onSerialPortError);
}

void BytetraceBase::updateUiState(bool isOpen) {
    auto *openCloseButton = m_pLeftBar->findChild<QPushButton*>("openCloseButton");
    auto *sendButton = m_pSendWidget->findChild<QPushButton*>("dataSendBtn");
    auto *portBox = m_pLeftBar->findChild<QComboBox*>("portComboBox");
    auto *baudrateBox = m_pLeftBar->findChild<QComboBox*>("baudrateComboBox");
    auto *databitsBox = m_pLeftBar->findChild<QComboBox*>("dataBitsComboBox");
    auto *parityBox = m_pLeftBar->findChild<QComboBox*>("parityComboBox");
    auto *stopBitsBox = m_pLeftBar->findChild<QComboBox*>("stopBitsComboBox");
    auto *flowControlBox = m_pLeftBar->findChild<QComboBox*>("flowControlComboBox");

    if (openCloseButton) {
        openCloseButton->setText(isOpen ? "关闭串口" : "打开串口");
    }
    if (sendButton) {
        sendButton->setEnabled(isOpen);
    }

    if (portBox) portBox->setEnabled(!isOpen);
    if (baudrateBox) baudrateBox->setEnabled(!isOpen);
    if (databitsBox) databitsBox->setEnabled(!isOpen);
    if (parityBox) parityBox->setEnabled(!isOpen);
    if (stopBitsBox) stopBitsBox->setEnabled(!isOpen);
    if (flowControlBox) flowControlBox->setEnabled(!isOpen);
}

bool BytetraceBase::applySerialConfig() {
    auto *portBox = m_pLeftBar->findChild<QComboBox*>("portComboBox");
    auto *baudrateBox = m_pLeftBar->findChild<QComboBox*>("baudrateComboBox");
    auto *databitsBox = m_pLeftBar->findChild<QComboBox*>("dataBitsComboBox");
    auto *parityBox = m_pLeftBar->findChild<QComboBox*>("parityComboBox");
    auto *stopBitsBox = m_pLeftBar->findChild<QComboBox*>("stopBitsComboBox");
    auto *flowControlBox = m_pLeftBar->findChild<QComboBox*>("flowControlComboBox");

    if (!portBox || !baudrateBox || !databitsBox || !parityBox || !stopBitsBox || !flowControlBox) {
        return false;
    }

    const QString portName = portBox->currentText().trimmed();
    if (portName.isEmpty()) {
        QMessageBox::warning(this, "错误", "未检测到串口，请连接设备后重试");
        return false;
    }

    m_serialPortManager->setPortName(portName);
    m_serialPortManager->setBaudRate(baudrateBox->currentData().toInt());
    m_serialPortManager->setDataBits(static_cast<QSerialPort::DataBits>(databitsBox->currentData().toInt()));
    m_serialPortManager->setParity(static_cast<QSerialPort::Parity>(parityBox->currentData().toInt()));
    m_serialPortManager->setStopBits(static_cast<QSerialPort::StopBits>(stopBitsBox->currentData().toInt()));
    m_serialPortManager->setFlowControl(static_cast<QSerialPort::FlowControl>(flowControlBox->currentData().toInt()));
    persistSerialConfig();
    return true;
}

void BytetraceBase::loadSerialConfigToUi() {
    auto *portBox = m_pLeftBar->findChild<QComboBox*>("portComboBox");
    auto *baudrateBox = m_pLeftBar->findChild<QComboBox*>("baudrateComboBox");
    auto *databitsBox = m_pLeftBar->findChild<QComboBox*>("dataBitsComboBox");
    auto *parityBox = m_pLeftBar->findChild<QComboBox*>("parityComboBox");
    auto *stopBitsBox = m_pLeftBar->findChild<QComboBox*>("stopBitsComboBox");
    auto *flowControlBox = m_pLeftBar->findChild<QComboBox*>("flowControlComboBox");

    if (!portBox || !baudrateBox || !databitsBox || !parityBox || !stopBitsBox || !flowControlBox) {
        return;
    }

    const QString configPort = ConfigManager::instance().portName();
    const int portIndex = portBox->findText(configPort);
    if (portIndex >= 0) {
        portBox->setCurrentIndex(portIndex);
    }

    const int baudRate = ConfigManager::instance().baudRate();
    const int baudIndex = baudrateBox->findData(baudRate);
    if (baudIndex >= 0) {
        baudrateBox->setCurrentIndex(baudIndex);
    }

    const int dataBits = ConfigManager::instance().dataBits();
    const int dataBitsIndex = databitsBox->findText(QString::number(dataBits));
    if (dataBitsIndex >= 0) {
        databitsBox->setCurrentIndex(dataBitsIndex);
    }

    const QString parity = ConfigManager::instance().parity().trimmed().toUpper();
    if (parity == "EVEN") {
        const int index = parityBox->findData(static_cast<int>(QSerialPort::EvenParity));
        if (index >= 0) parityBox->setCurrentIndex(index);
    } else if (parity == "ODD") {
        const int index = parityBox->findData(static_cast<int>(QSerialPort::OddParity));
        if (index >= 0) parityBox->setCurrentIndex(index);
    } else if (parity == "MARK") {
        const int index = parityBox->findData(static_cast<int>(QSerialPort::MarkParity));
        if (index >= 0) parityBox->setCurrentIndex(index);
    } else if (parity == "SPACE") {
        const int index = parityBox->findData(static_cast<int>(QSerialPort::SpaceParity));
        if (index >= 0) parityBox->setCurrentIndex(index);
    } else {
        const int index = parityBox->findData(static_cast<int>(QSerialPort::NoParity));
        if (index >= 0) parityBox->setCurrentIndex(index);
    }

    const int stopBits = ConfigManager::instance().stopBits();
    if (stopBits == 2) {
        const int index = stopBitsBox->findData(static_cast<int>(QSerialPort::TwoStop));
        if (index >= 0) stopBitsBox->setCurrentIndex(index);
    } else if (stopBits == 3) {
        const int index = stopBitsBox->findData(static_cast<int>(QSerialPort::OneAndHalfStop));
        if (index >= 0) stopBitsBox->setCurrentIndex(index);
    } else {
        const int index = stopBitsBox->findData(static_cast<int>(QSerialPort::OneStop));
        if (index >= 0) stopBitsBox->setCurrentIndex(index);
    }
}

void BytetraceBase::persistSerialConfig() const {
    auto *portBox = m_pLeftBar->findChild<QComboBox*>("portComboBox");
    auto *baudrateBox = m_pLeftBar->findChild<QComboBox*>("baudrateComboBox");
    auto *databitsBox = m_pLeftBar->findChild<QComboBox*>("dataBitsComboBox");
    auto *parityBox = m_pLeftBar->findChild<QComboBox*>("parityComboBox");
    auto *stopBitsBox = m_pLeftBar->findChild<QComboBox*>("stopBitsComboBox");

    if (!portBox || !baudrateBox || !databitsBox || !parityBox || !stopBitsBox) {
        return;
    }

    ConfigManager::instance().setPortName(portBox->currentText().trimmed());
    ConfigManager::instance().setBaudRate(baudrateBox->currentData().toInt());
    ConfigManager::instance().setDataBits(databitsBox->currentText().toInt());
    ConfigManager::instance().setParity(parityBox->currentText().trimmed());

    int stopBitsValue = 1;
    const QString stopBitsText = stopBitsBox->currentText().trimmed();
    if (stopBitsText == "2") {
        stopBitsValue = 2;
    } else if (stopBitsText == "1.5") {
        stopBitsValue = 3;
    }
    ConfigManager::instance().setStopBits(stopBitsValue);
    ConfigManager::instance().sync();
}

QString BytetraceBase::formatHexData(const QByteArray& data) const {
    QString hexData;
    for (int i = 0; i < data.size(); ++i) {
        hexData += QString("%1 ").arg(static_cast<unsigned char>(data[i]), 2, 16, QChar('0'));
    }
    return hexData.trimmed();
}

QByteArray BytetraceBase::buildSendPayload(const QString& rawText, bool* ok) const {
    if (ok) {
        *ok = false;
    }

    auto *hexSendBtn = m_pLeftBar->findChild<QRadioButton*>("radioButton_7");
    auto *crcCheckBtn = m_pLeftBar->findChild<QRadioButton*>("radioButton_6");

    QByteArray payload;
    if (hexSendBtn && hexSendBtn->isChecked()) {
        QString normalized = rawText;
        normalized.remove(QRegularExpression("[\\s,]"));
        if (normalized.isEmpty() || (normalized.size() % 2) != 0) {
            return {};
        }
        for (int i = 0; i < normalized.size(); i += 2) {
            bool byteOk = false;
            const QString byteText = normalized.mid(i, 2);
            const int byteValue = byteText.toInt(&byteOk, 16);
            if (!byteOk) {
                return {};
            }
            payload.append(static_cast<char>(byteValue));
        }
    } else {
        payload = rawText.toUtf8();
    }

    if (crcCheckBtn && crcCheckBtn->isChecked()) {
        const quint16 crc = crc16Modbus(payload);
        payload.append(static_cast<char>(crc & 0xFF));
        payload.append(static_cast<char>((crc >> 8) & 0xFF));
    }

    if (ok) {
        *ok = true;
    }
    return payload;
}

quint16 BytetraceBase::crc16Modbus(const QByteArray& data) const {
    quint16 crc = 0xFFFF;
    for (unsigned char byte : data) {
        crc ^= static_cast<quint16>(byte);
        for (int i = 0; i < 8; ++i) {
            const bool lsbSet = (crc & 0x0001) != 0;
            crc >>= 1;
            if (lsbSet) {
                crc ^= 0xA001;
            }
        }
    }
    return crc;
}

void BytetraceBase::handleOpenClosePort() {
    if (m_serialPortManager->isOpen()) {
        m_serialPortManager->closePort();
        m_isPortOpen = false;
        updateUiState(false);
        return;
    }

    if (!applySerialConfig()) {
        return;
    }

    if (!m_serialPortManager->openPort()) {
        QMessageBox::warning(this, "串口错误", ErrorHandler::instance().getErrorMessage(m_serialPortManager->getError()));
        return;
    }

    m_serialPortManager->startReading();
    m_isPortOpen = true;
    updateUiState(true);
}

void BytetraceBase::handleSendData() {
    auto *sendArea = m_pSendWidget->findChild<QTextEdit*>("sendArea");
    if (!sendArea) {
        return;
    }
    if (!m_serialPortManager->isOpen()) {
        QMessageBox::warning(this, "错误", "串口未打开，请先打开串口");
        return;
    }

    bool payloadOk = false;
    const QByteArray data = buildSendPayload(sendArea->toPlainText(), &payloadOk);
    if (!payloadOk) {
        QMessageBox::warning(this, "发送失败", "HEX发送模式下请输入合法的十六进制数据");
        return;
    }
    if (data.isEmpty()) {
        return;
    }
    if (!m_serialPortManager->writeData(data)) {
        QMessageBox::warning(this, "发送失败", ErrorHandler::instance().getErrorMessage(m_serialPortManager->getError()));
    }
}

void BytetraceBase::handleClearSend() {
    auto *sendArea = m_pSendWidget->findChild<QTextEdit*>("sendArea");
    if (sendArea) {
        sendArea->clear();
    }
}

void BytetraceBase::handleClearRecv() {
    auto *recvArea = m_pRecvWidget->findChild<QTextBrowser*>("recvArea");
    if (recvArea) {
        recvArea->clear();
    }
}

void BytetraceBase::onSerialDataReceived(const QByteArray& data) {
    auto *recvArea = m_pRecvWidget->findChild<QTextBrowser*>("recvArea");
    auto *recvHexBtn = m_pLeftBar->findChild<QRadioButton*>("recvHexBtn");
    if (!recvArea) {
        return;
    }

    if (recvHexBtn && recvHexBtn->isChecked()) {
        recvArea->append(formatHexData(data));
    } else {
        recvArea->append(QString::fromUtf8(data));
    }
}

void BytetraceBase::onSerialPortError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) {
        return;
    }
    LogUtils::logMessage(QString("串口错误: %1").arg(static_cast<int>(error)), LOG::LOG_ERROR);
    if (error == QSerialPort::ResourceError || error == QSerialPort::ReadError || error == QSerialPort::WriteError) {
        m_serialPortManager->closePort();
        m_isPortOpen = false;
        updateUiState(false);
    }
}
