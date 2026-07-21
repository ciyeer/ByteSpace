/**
 * @file bytetracebase.cpp
 * @brief 串口追踪主界面实现
 *
 * @author ByteSpace团队
 * @date 2024
 */
#include "bytetracebase.h"
#include "ui_bytetracebase.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QMovie>
#include <QDateTime>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegularExpression>
#include "logmanager/logutils.h"
#include "taskmanager/taskmanager.h"
#include "taskmanager/tasks/serialportopentask.h"
#include "statemanager/statemanager.h"
#include "utils/errorhandler.h"
#include "utils/configmanager.h"

// CRC-16/MODBUS 查找表（多项式 0x8005，初始值 0xFFFF）
static const uint16_t crc16Table[256] = {
    0x0000,0xC0C1,0xC181,0x0140,0xC301,0x03C0,0x0280,0xC241,
    0xC601,0x06C0,0x0780,0xC741,0x0500,0xC5C1,0xC481,0x0440,
    0xCC01,0x0CC0,0x0D80,0xCD41,0x0F00,0xCFC1,0xCE81,0x0E40,
    0x0A00,0xCAC1,0xCB81,0x0B40,0xC901,0x09C0,0x0880,0xC841,
    0xD801,0x18C0,0x1980,0xD941,0x1B00,0xDBC1,0xDA81,0x1A40,
    0x1E00,0xDEC1,0xDF81,0x1F40,0xDD01,0x1DC0,0x1C80,0xDC41,
    0x1400,0xD4C1,0xD581,0x1540,0xD701,0x17C0,0x1680,0xD641,
    0xD201,0x12C0,0x1380,0xD341,0x1100,0xD1C1,0xD081,0x1040,
    0xF001,0x30C0,0x3180,0xF141,0x3300,0xF3C1,0xF281,0x3240,
    0x3600,0xF6C1,0xF781,0x3740,0xF501,0x35C0,0x3480,0xF441,
    0x3C00,0xFCC1,0xFD81,0x3D40,0xFF01,0x3FC0,0x3E80,0xFE41,
    0xFA01,0x3AC0,0x3B80,0xFB41,0x3900,0xF9C1,0xF881,0x3840,
    0x2800,0xE8C1,0xE981,0x2940,0xEB01,0x2BC0,0x2A80,0xEA41,
    0xEE01,0x2EC0,0x2F80,0xEF41,0x2D00,0xEDC1,0xEC81,0x2C40,
    0xE401,0x24C0,0x2580,0xE541,0x2700,0xE7C1,0xE681,0x2640,
    0x2200,0xE2C1,0xE381,0x2340,0xE101,0x21C0,0x2080,0xE041,
    0xA001,0x60C0,0x6180,0xA141,0x6300,0xA3C1,0xA281,0x6240,
    0x6600,0xA6C1,0xA781,0x6740,0xA501,0x65C0,0x6480,0xA441,
    0x6C00,0xACC1,0xAD81,0x6D40,0xAF01,0x6FC0,0x6E80,0xAE41,
    0xAA01,0x6AC0,0x6B80,0xAB41,0x6900,0xA9C1,0xA881,0x6840,
    0x7800,0xB8C1,0xB981,0x7940,0xBB01,0x7BC0,0x7A80,0xBA41,
    0xBE01,0x7EC0,0x7F80,0xBF41,0x7D00,0xBDC1,0xBC81,0x7C40,
    0xB401,0x74C0,0x7580,0xB541,0x7700,0xB7C1,0xB681,0x7640,
    0x7200,0xB2C1,0xB381,0x7340,0xB101,0x71C0,0x7080,0xB041,
    0x5000,0x90C1,0x9181,0x5140,0x9301,0x53C0,0x5280,0x9241,
    0x9601,0x56C0,0x5780,0x9741,0x5500,0x95C1,0x9481,0x5440,
    0x9C01,0x5CC0,0x5D80,0x9D41,0x5F00,0x9FC1,0x9E81,0x5E40,
    0x5A00,0x9AC1,0x9B81,0x5B40,0x9901,0x59C0,0x5880,0x9841,
    0x8801,0x48C0,0x4980,0x8941,0x4B00,0x8BC1,0x8A81,0x4A40,
    0x4E00,0x8EC1,0x8F81,0x4F40,0x8D01,0x4DC0,0x4C80,0x8C41,
    0x4400,0x84C1,0x8581,0x4540,0x8701,0x47C0,0x4680,0x8641,
    0x8201,0x42C0,0x4380,0x8341,0x4100,0x81C1,0x8081,0x4040
};

BytetraceBase::BytetraceBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BytetraceBase)
    , m_transferAnimation(nullptr)
    , m_isOpen(false)
    , m_txBytes(0)
    , m_rxBytes(0)
    , m_isLogging(false) {
    ui->setupUi(this);

    m_pLeftBar = new LeftBar;
    m_pRecvWidget = new RecvWidget;
    m_pSendWidget = new SendWidget;

    ui->leftbarLayout->addWidget(m_pLeftBar);
    ui->recvLayout->addWidget(m_pRecvWidget);
    ui->sendLayout->addWidget(m_pSendWidget);

    m_serialPortManager = std::make_shared<SerialPortManager>();
    m_serialPortManager->initialize(
        m_pLeftBar->portComboBox(),
        m_pLeftBar->baudrateComboBox(),
        m_pLeftBar->dataBitsComboBox(),
        m_pLeftBar->parityComboBox(),
        m_pLeftBar->stopBitsComboBox(),
        m_pLeftBar->flowControlComboBox()
    );

    m_portMonitorTimer = new QTimer(this);
    m_portMonitorTimer->setInterval(1000);
    connect(m_portMonitorTimer, &QTimer::timeout, this, &BytetraceBase::onTimeout);

    m_timedSendTimer = new QTimer(this);
    connect(m_timedSendTimer, &QTimer::timeout, this, &BytetraceBase::onTimedSendTimeout);

    initTransferAnimation();
    connections();

    updateUI(false);
}

void BytetraceBase::connections() {
    connect(m_pLeftBar->openCloseButton(), &QPushButton::clicked,
            this, &BytetraceBase::handleOpenCloseSerialPort);

    connect(m_pSendWidget->dataSendBtn(), &QPushButton::clicked,
            this, &BytetraceBase::onSendButtonClicked);

    connect(m_pSendWidget->clearSendBtn(), &QPushButton::clicked,
            this, &BytetraceBase::onClearSendBuffer);
    connect(m_pSendWidget->clearRecvBtn(), &QPushButton::clicked,
            this, &BytetraceBase::onClearRecvBuffer);

    connect(m_serialPortManager.get(), &SerialPortManager::serialPortError,
            this, &BytetraceBase::onSerialPortError, Qt::QueuedConnection);

    connect(m_serialPortManager.get(), &SerialPortManager::dataReceived,
            this, &BytetraceBase::onSerialDataReceived, Qt::QueuedConnection);

    connect(&TaskManager::getInstance(), &TaskManager::taskCompleted,
            this, &BytetraceBase::onTaskCompleted, Qt::QueuedConnection);
    connect(&TaskManager::getInstance(), &TaskManager::taskFailed,
            this, &BytetraceBase::onTaskFailed, Qt::QueuedConnection);

    // 新功能信号连接
    connect(m_pLeftBar->dtrBtn(), &QRadioButton::toggled,
            this, &BytetraceBase::onDtrToggled);
    connect(m_pLeftBar->rtsBtn(), &QRadioButton::toggled,
            this, &BytetraceBase::onRtsToggled);
    connect(m_pLeftBar->fileTransferBtn(), &QRadioButton::toggled,
            this, &BytetraceBase::onFileTransferClicked);
    connect(m_pLeftBar->saveToFileBtn(), &QRadioButton::toggled,
            this, &BytetraceBase::onSaveToFileToggled);
    connect(m_pLeftBar->timedSendBtn(), &QRadioButton::toggled,
            this, [this](bool checked) {
                if (checked && m_isOpen) {
                    int interval = m_pLeftBar->timedSendInterval();
                    if (interval < 10) interval = 10;
                    m_timedSendTimer->start(interval);
                } else {
                    m_timedSendTimer->stop();
                }
            });
    connect(m_pLeftBar->autoNewlineBtn(), &QRadioButton::toggled,
            this, [this](bool checked) {
                m_pRecvWidget->recvArea()->setLineWrapMode(
                    checked ? QTextEdit::WidgetWidth : QTextEdit::NoWrap);
            });
}

// ============================================================================
// 串口打开/关闭
// ============================================================================

void BytetraceBase::handleOpenCloseSerialPort() {
    QMutexLocker locker(&m_serialMutex);
    if (m_isOpen) {
        // 关闭前停止定时发送和日志
        m_timedSendTimer->stop();
        if (m_isLogging) {
            m_logFile.close();
            m_isLogging = false;
        }

        StateManager::instance().changeState(AppState::Disconnecting);
        m_serialPortManager->closePort();
        m_isOpen = false;
        updateUI(false);
        showTransferAnimation(false);
        StateManager::instance().changeState(AppState::Idle);
        LogUtils::logMessage("串口已关闭", LOG::LOG_INFO);
    } else {
        QString selectedPort = m_pLeftBar->portName();
        if (selectedPort.isEmpty()) {
            QMessageBox::warning(this, "错误", "请先选择串口");
            return;
        }

        // 验证波特率
        bool ok;
        int baud = m_pLeftBar->baudRate();
        if (baud <= 0) {
            QMessageBox::warning(this, "错误", "无效的波特率值");
            return;
        }

        m_serialPortManager->configureSerialPort(
            selectedPort,
            baud,
            m_pLeftBar->dataBits(),
            m_pLeftBar->parity(),
            m_pLeftBar->stopBits(),
            m_pLeftBar->flowControl()
        );

        ConfigManager::instance().setPortName(selectedPort);
        ConfigManager::instance().setBaudRate(baud);
        ConfigManager::instance().setDataBits(static_cast<int>(m_pLeftBar->dataBits()));
        ConfigManager::instance().setStopBits(static_cast<int>(m_pLeftBar->stopBits()));
        ConfigManager::instance().setParity(m_pLeftBar->parityText());
        ConfigManager::instance().setFlowControl(m_pLeftBar->flowControlText());

        LogUtils::logMessage(QString("串口配置 - 波特率: %1, 数据位: %2, 校验位: %3, 停止位: %4")
            .arg(baud)
            .arg(static_cast<int>(m_pLeftBar->dataBits()))
            .arg(m_pLeftBar->parityText())
            .arg(static_cast<int>(m_pLeftBar->stopBits())), LOG::LOG_INFO);

        LogUtils::logMessage("正在打开串口 " + selectedPort + "...", LOG::LOG_INFO);

        StateManager::instance().changeState(AppState::Connecting);

        auto task = std::make_shared<SerialPortOpenTask>(m_serialPortManager);
        TaskManager::getInstance().addTask(task);
    }
}

void BytetraceBase::onTaskCompleted() {
    QMutexLocker locker(&m_serialMutex);
    if (m_serialPortManager->isOpen()) {
        m_isOpen = true;
        updateUI(true);
        m_serialPortManager->startReading();
        showTransferAnimation(true);
        StateManager::instance().changeState(AppState::Connected);

        // 串口打开后应用 DTR/RTS 初始状态
        if (m_pLeftBar->isDtrEnabled()) {
            m_serialPortManager->configurator().serialPort()->setDataTerminalReady(true);
        }
        if (m_pLeftBar->isRtsEnabled()) {
            m_serialPortManager->configurator().serialPort()->setRequestToSend(true);
        }

        // 如果定时发送已勾选，启动定时器
        if (m_pLeftBar->isTimedSend()) {
            int interval = m_pLeftBar->timedSendInterval();
            if (interval < 10) interval = 10;
            m_timedSendTimer->start(interval);
        }

        LogUtils::logMessage("串口 " + m_serialPortManager->getPortName() + " 打开成功", LOG::LOG_INFO);
    } else {
        StateManager::instance().changeState(AppState::Error);
        LogUtils::logMessage("串口打开失败", LOG::LOG_ERROR);
    }
}

void BytetraceBase::onTaskFailed() {
    StateManager::instance().setErrorState("串口打开任务失败");
    updateUI(false);
    LogUtils::logMessage("串口打开任务失败", LOG::LOG_ERROR);
}

void BytetraceBase::onSerialPortError(QSerialPort::SerialPortError error) {
    if (error == QSerialPort::NoError) return;

    ErrorHandler::instance().handleSerialPortError(error);

    if (error == QSerialPort::ResourceError
        || error == QSerialPort::WriteError
        || error == QSerialPort::ReadError) {
        QMutexLocker locker(&m_serialMutex);
        m_timedSendTimer->stop();
        if (m_isLogging) { m_logFile.close(); m_isLogging = false; }
        m_serialPortManager->closePort();
        m_isOpen = false;
        updateUI(false);
        StateManager::instance().changeState(AppState::Error);
    }
}

// ============================================================================
// 数据发送
// ============================================================================

QByteArray BytetraceBase::parseHexString(const QString& hexStr) {
    // 去除所有空白字符
    QString cleaned = hexStr;
    cleaned.remove(QRegularExpression("[\\s]"));

    if (cleaned.isEmpty()) {
        return QByteArray();
    }

    if (cleaned.length() % 2 != 0) {
        QMessageBox::warning(const_cast<BytetraceBase*>(this),
                             "HEX 格式错误",
                             "十六进制字符串长度无效（必须为偶数个字符）");
        return QByteArray();
    }

    QByteArray result;
    result.reserve(cleaned.length() / 2);

    for (int i = 0; i < cleaned.length(); i += 2) {
        bool ok;
        uint8_t byte = static_cast<uint8_t>(cleaned.mid(i, 2).toUInt(&ok, 16));
        if (!ok) {
            QMessageBox::warning(const_cast<BytetraceBase*>(this),
                                 "HEX 格式错误",
                                 QString("无效的十六进制字符: '%1'").arg(cleaned.mid(i, 2)));
            return QByteArray();
        }
        result.append(static_cast<char>(byte));
    }

    return result;
}

uint16_t BytetraceBase::calculateCrc16Modbus(const QByteArray& data) {
    uint16_t crc = 0xFFFF;
    for (int i = 0; i < data.size(); ++i) {
        uint8_t index = static_cast<uint8_t>(crc ^ static_cast<uint8_t>(data[i]));
        crc = (crc >> 8) ^ crc16Table[index];
    }
    return crc;
}

QByteArray BytetraceBase::buildSendData() {
    QString text = m_pSendWidget->sendArea()->toPlainText();
    if (text.isEmpty()) return QByteArray();

    QByteArray data;

    if (m_pLeftBar->isHexSend()) {
        data = parseHexString(text);
        if (data.isEmpty() && !text.isEmpty()) {
            return QByteArray();  // HEX 解析失败
        }
    } else {
        data = text.toUtf8();
    }

    // CRC 校验追加（在换行符之前）
    if (m_pLeftBar->isCrcEnabled()) {
        uint16_t crc = calculateCrc16Modbus(data);
        data.append(static_cast<char>(crc & 0xFF));       // 低字节
        data.append(static_cast<char>((crc >> 8) & 0xFF)); // 高字节
    }

    // 发送新行追加（在 CRC 之后）
    if (m_pLeftBar->isSendNewline()) {
        data.append("\r\n");
    }

    return data;
}

void BytetraceBase::onSendButtonClicked() {
    if (!m_serialPortManager || !m_serialPortManager->isOpen()) {
        LogUtils::logMessage("串口未打开，无法发送数据", LOG::LOG_ERROR);
        QMessageBox::warning(this, "错误", "串口未打开，请先打开串口");
        return;
    }

    QByteArray sendBytes = buildSendData();
    if (sendBytes.isEmpty()) {
        LogUtils::logMessage("发送内容为空", LOG::LOG_WARNING);
        return;
    }

    LogUtils::logMessage(QString("准备发送 %1 字节数据").arg(sendBytes.size()), LOG::LOG_INFO);
    sendData(sendBytes);
}

void BytetraceBase::onTimedSendTimeout() {
    if (!m_isOpen || !m_serialPortManager->isOpen()) {
        m_timedSendTimer->stop();
        return;
    }

    QByteArray sendBytes = buildSendData();
    if (!sendBytes.isEmpty()) {
        sendData(sendBytes);
    }
}

// ============================================================================
// 数据接收
// ============================================================================

void BytetraceBase::onSerialDataReceived(const QByteArray& rawData) {
    m_rxBytes += rawData.size();
    updateTxRxCounters(0, rawData.size());

    LogUtils::logMessage(QString("接收到 %1 字节数据").arg(rawData.size()), LOG::LOG_INFO);

    QString hexData = QString::fromLatin1(rawData.toHex(' '));
    LogUtils::logMessage(QString("接收数据内容(HEX): %1").arg(hexData), LOG::LOG_DEBUG);

    QString displayText;

    if (m_pLeftBar->isHexDisplay()) {
        displayText = hexData;
    } else {
        QString timestamp = QDateTime::currentDateTime().toString("[HH:mm:ss.zzz] ");
        displayText = timestamp + QString::fromUtf8(rawData);
    }

    m_pRecvWidget->recvArea()->append(displayText);

    // 日志存文件
    if (m_isLogging && m_logFile.isOpen()) {
        QTextStream stream(&m_logFile);
        stream << displayText << "\n";
        stream.flush();
    }
}

// ============================================================================
// 文件收发
// ============================================================================

void BytetraceBase::onFileTransferClicked(bool checked) {
    if (!checked) return;
    if (!m_isOpen) {
        QMessageBox::warning(this, "错误", "请先打开串口");
        m_pLeftBar->fileTransferBtn()->setChecked(false);
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(this, "选择要发送的文件");
    if (filePath.isEmpty()) {
        m_pLeftBar->fileTransferBtn()->setChecked(false);
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + filePath);
        m_pLeftBar->fileTransferBtn()->setChecked(false);
        return;
    }

    qint64 fileSize = file.size();
    if (fileSize > 1024 * 1024) {
        auto reply = QMessageBox::question(this, "大文件警告",
            QString("文件大小 %1 KB，发送可能需要较长时间。是否继续？")
                .arg(fileSize / 1024),
            QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            m_pLeftBar->fileTransferBtn()->setChecked(false);
            return;
        }
    }

    // 分块发送
    const int chunkSize = 256;
    QByteArray chunk;
    qint64 totalSent = 0;
    while (!file.atEnd()) {
        chunk = file.read(chunkSize);
        if (!chunk.isEmpty()) {
            QMutexLocker locker(&m_serialMutex);
            if (!m_serialPortManager->isOpen()) break;
            m_serialPortManager->writeData(chunk);
            totalSent += chunk.size();
            m_txBytes += chunk.size();
        }
    }
    file.close();
    updateTxRxCounters(0, 0);

    LogUtils::logMessage(QString("文件发送完成: %1 字节").arg(totalSent), LOG::LOG_INFO);

    // 单次操作后取消勾选
    m_pLeftBar->fileTransferBtn()->setChecked(false);
}

void BytetraceBase::onSaveToFileToggled(bool checked) {
    if (checked) {
        QString filePath = QFileDialog::getSaveFileName(this, "保存接收日志",
            QDateTime::currentDateTime().toString("yyyy-MM-dd_HHmmss") + ".log",
            "日志文件 (*.log *.txt);;所有文件 (*)");
        if (filePath.isEmpty()) {
            m_pLeftBar->saveToFileBtn()->setChecked(false);
            return;
        }
        m_logFile.setFileName(filePath);
        if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QMessageBox::warning(this, "错误", "无法创建日志文件: " + filePath);
            m_pLeftBar->saveToFileBtn()->setChecked(false);
            return;
        }
        m_isLogging = true;
        LogUtils::logMessage("接收日志已开启: " + filePath, LOG::LOG_INFO);
    } else {
        if (m_logFile.isOpen()) {
            m_logFile.close();
        }
        m_isLogging = false;
        LogUtils::logMessage("接收日志已停止", LOG::LOG_INFO);
    }
}

// ============================================================================
// DTR/RTS 控制
// ============================================================================

void BytetraceBase::onDtrToggled(bool checked) {
    if (!m_isOpen) return;
    m_serialPortManager->configurator().serialPort()->setDataTerminalReady(checked);
    LogUtils::logMessage(QString("DTR: %1").arg(checked ? "ON" : "OFF"), LOG::LOG_INFO);
}

void BytetraceBase::onRtsToggled(bool checked) {
    if (!m_isOpen) return;
    m_serialPortManager->configurator().serialPort()->setRequestToSend(checked);
    LogUtils::logMessage(QString("RTS: %1").arg(checked ? "ON" : "OFF"), LOG::LOG_INFO);
}

// ============================================================================
// 清空
// ============================================================================

void BytetraceBase::onClearSendBuffer() {
    m_pSendWidget->sendArea()->clear();
}

void BytetraceBase::onClearRecvBuffer() {
    m_pRecvWidget->recvArea()->clear();
    m_rxBytes = 0;
    updateTxRxCounters(0, 0);
}

// ============================================================================
// 端口监控
// ============================================================================

void BytetraceBase::onTimeout() {
    QMutexLocker locker(&m_serialMutex);
    if (m_isOpen && !m_serialPortManager->isOpen()) {
        LogUtils::logMessage("Serial port unexpectedly closed.", LOG::LOG_WARNING);
        m_timedSendTimer->stop();
        if (m_isLogging) { m_logFile.close(); m_isLogging = false; }
        m_isOpen = false;
        updateUI(false);
        StateManager::instance().changeState(AppState::Error);
    }

    QList<QSerialPortInfo> currentPorts = QSerialPortInfo::availablePorts();
    QString currentPort = m_pLeftBar->portName();

    bool portStillAvailable = false;
    for (const auto &port : currentPorts) {
        if (port.portName() == currentPort) {
            portStillAvailable = true;
            break;
        }
    }

    if (!portStillAvailable) {
        LogUtils::logMessage("Current serial port was removed.", LOG::LOG_ERROR);
        if (m_isOpen) {
            m_timedSendTimer->stop();
            if (m_isLogging) { m_logFile.close(); m_isLogging = false; }
            m_serialPortManager->closePort();
            m_isOpen = false;
        }
        updateUI(false);
    }
}

// ============================================================================
// UI 更新
// ============================================================================

void BytetraceBase::updateUI(bool isOpen) {
    m_pLeftBar->setPortOpen(isOpen);

    if (isOpen) {
        m_pSendWidget->dataSendBtn()->setEnabled(true);
        m_portMonitorTimer->start();
    } else {
        m_pSendWidget->dataSendBtn()->setEnabled(false);
        m_portMonitorTimer->stop();
        m_timedSendTimer->stop();
    }
}

void BytetraceBase::updateTxRxCounters(qint64, qint64) {
    emit txRxUpdated(m_txBytes, m_rxBytes);
}

void BytetraceBase::sendData(const QByteArray& rawData) {
    QMutexLocker<QMutex> locker(&m_serialMutex);
    try {
        if (m_serialPortManager && m_serialPortManager->isOpen()) {
            if (m_serialPortManager->writeData(rawData)) {
                m_txBytes += rawData.size();
                LogUtils::logMessage("数据发送成功", LOG::LOG_INFO);
            } else {
                LogUtils::logMessage("数据发送失败", LOG::LOG_ERROR);
                onSerialPortError(m_serialPortManager->getError());
            }
        } else {
            LogUtils::logMessage("串口未打开，无法发送数据", LOG::LOG_ERROR);
        }
    } catch (const std::exception& e) {
        QString error = QString("发送数据异常: %1").arg(e.what());
        LogUtils::logMessage(error, LOG::LOG_ERROR);
    }
}

// ============================================================================
// 动画
// ============================================================================

void BytetraceBase::initTransferAnimation() {
    m_transferAnimation = new QMovie(this);
    m_transferAnimation->setFileName(":/images/transfer.gif");

    if (!m_transferAnimation->isValid()) {
        LogUtils::logMessage("动画资源加载失败", LOG::LOG_ERROR);
        return;
    }

    QLabel* label = m_pLeftBar->transferLabel();
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    label->setScaledContents(true);
    label->setAlignment(Qt::AlignCenter);
    label->setMovie(m_transferAnimation);

    m_transferAnimation->setScaledSize(label->size());
    m_transferAnimation->setSpeed(300);

    m_transferAnimation->start();
    m_transferAnimation->setPaused(true);
}

void BytetraceBase::showTransferAnimation(bool active) {
    if (!m_transferAnimation || !m_pLeftBar->transferLabel()) return;

    if (active) {
        m_transferAnimation->start();
    } else {
        m_transferAnimation->stop();
        m_transferAnimation->start();
        m_transferAnimation->jumpToFrame(0);
        m_transferAnimation->setPaused(true);
    }
}

BytetraceBase::~BytetraceBase() {
    if (m_portMonitorTimer) m_portMonitorTimer->stop();
    if (m_timedSendTimer) m_timedSendTimer->stop();
    if (m_transferAnimation) m_transferAnimation->stop();
    if (m_isLogging) m_logFile.close();
    if (m_serialPortManager && m_serialPortManager->isOpen()) {
        m_serialPortManager->closePort();
    }
    delete ui;
}
