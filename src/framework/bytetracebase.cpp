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
#include "logmanager/logutils.h"
#include "taskmanager/taskmanager.h"
#include "taskmanager/tasks/serialportopentask.h"
#include "statemanager/statemanager.h"
#include "utils/errorhandler.h"

BytetraceBase::BytetraceBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BytetraceBase)
    , m_transferAnimation(nullptr)
    , m_isOpen(false) {
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
    m_portMonitorTimer->start();

    initTransferAnimation();
    connections();
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
}

void BytetraceBase::handleOpenCloseSerialPort() {
    if (m_isOpen) {
        StateManager::instance().changeState(AppState::Disconnecting);
        m_serialPortManager->closePort();
        m_isOpen = false;
        updateUI(false);
        showTransferAnimation(false);
        StateManager::instance().changeState(AppState::Idle);
        LogUtils::logMessage("串口已关闭", LOG::LOG_INFO);
    } else {
        QString selectedPort = m_pLeftBar->portComboBox()->currentText();
        if (selectedPort.isEmpty()) {
            QMessageBox::warning(this, "错误", "请先选择串口");
            return;
        }

        m_serialPortManager->setPortName(selectedPort);

        LogUtils::logMessage(QString("串口配置 - 波特率: %1, 数据位: %2, 校验位: %3, 停止位: %4")
            .arg(m_pLeftBar->baudrateComboBox()->currentText(),
                 m_pLeftBar->dataBitsComboBox()->currentText(),
                 m_pLeftBar->parityComboBox()->currentText(),
                 m_pLeftBar->stopBitsComboBox()->currentText()), LOG::LOG_INFO);

        LogUtils::logMessage("正在打开串口 " + selectedPort + "...", LOG::LOG_INFO);

        StateManager::instance().changeState(AppState::Connecting);

        auto task = std::make_shared<SerialPortOpenTask>(m_serialPortManager);
        TaskManager::getInstance().addTask(task);
    }
}

void BytetraceBase::onTaskCompleted() {
    if (m_serialPortManager->isOpen()) {
        m_isOpen = true;
        updateUI(true);
        m_serialPortManager->startReading();
        showTransferAnimation(true);
        StateManager::instance().changeState(AppState::Connected);
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
    QString errorMsg = ErrorHandler::instance().getErrorMessage(error);

    if (error == QSerialPort::ResourceError) {
        m_serialPortManager->closePort();
        m_isOpen = false;
        updateUI(false);
        StateManager::instance().changeState(AppState::Error);
    }
}

void BytetraceBase::onSerialDataReceived(const QByteArray& rawData) {
    LogUtils::logMessage(QString("接收到 %1 字节数据").arg(rawData.size()), LOG::LOG_INFO);

    QString hexData;
    for (int i = 0; i < rawData.size(); ++i) {
        hexData += QString("%1 ").arg(static_cast<unsigned char>(rawData[i]), 2, 16, QChar('0'));
    }
    LogUtils::logMessage(QString("接收数据内容(HEX): %1").arg(hexData.trimmed()), LOG::LOG_DEBUG);

    if (m_pLeftBar->recvHexBtn()->isChecked()) {
        m_pRecvWidget->recvArea()->append(hexData.trimmed());
    } else {
        QString textData = QString::fromUtf8(rawData);
        m_pRecvWidget->recvArea()->append(textData);
    }
}

void BytetraceBase::onSendButtonClicked() {
    if (!m_serialPortManager || !m_serialPortManager->isOpen()) {
        LogUtils::logMessage("串口未打开，无法发送数据", LOG::LOG_ERROR);
        QMessageBox::warning(this, "错误", "串口未打开，请先打开串口");
        return;
    }

    QString text = m_pSendWidget->sendArea()->toPlainText();
    if (text.isEmpty()) {
        LogUtils::logMessage("发送内容为空", LOG::LOG_WARNING);
        return;
    }

    QByteArray sendBytes = text.toUtf8();
    LogUtils::logMessage(QString("准备发送 %1 字节数据").arg(sendBytes.size()), LOG::LOG_INFO);

    sendData(sendBytes);
}

void BytetraceBase::onClearSendBuffer() {
    m_pSendWidget->sendArea()->clear();
}

void BytetraceBase::onClearRecvBuffer() {
    m_pRecvWidget->recvArea()->clear();
}

void BytetraceBase::onTimeout() {
    if (m_isOpen && !m_serialPortManager->isOpen()) {
        LogUtils::logMessage("Serial port unexpectedly closed. Attempting recovery...", LOG::LOG_WARNING);
        m_isOpen = false;
        updateUI(false);
        StateManager::instance().changeState(AppState::Error);
    }

    QList<QSerialPortInfo> currentPorts = QSerialPortInfo::availablePorts();
    QString currentPort = m_pLeftBar->portComboBox()->currentText();

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
            m_serialPortManager->closePort();
            m_isOpen = false;
        }
        updateUI(false);
    }
}

void BytetraceBase::updateUI(bool isOpen) {
    if (isOpen) {
        m_pLeftBar->openCloseButton()->setText("关闭串口");
        m_pLeftBar->openCloseButton()->setStyleSheet("background-color: #FF6B6B;");

        m_pLeftBar->portComboBox()->setEnabled(false);
        m_pLeftBar->baudrateComboBox()->setEnabled(false);
        m_pLeftBar->dataBitsComboBox()->setEnabled(false);
        m_pLeftBar->parityComboBox()->setEnabled(false);
        m_pLeftBar->stopBitsComboBox()->setEnabled(false);
        m_pLeftBar->flowControlComboBox()->setEnabled(false);

        m_pSendWidget->dataSendBtn()->setEnabled(true);
    } else {
        m_pLeftBar->openCloseButton()->setText("打开串口");
        m_pLeftBar->openCloseButton()->setStyleSheet("background-color: #4CAF50;");

        m_pLeftBar->portComboBox()->setEnabled(true);
        m_pLeftBar->baudrateComboBox()->setEnabled(true);
        m_pLeftBar->dataBitsComboBox()->setEnabled(true);
        m_pLeftBar->parityComboBox()->setEnabled(true);
        m_pLeftBar->stopBitsComboBox()->setEnabled(true);
        m_pLeftBar->flowControlComboBox()->setEnabled(true);

        m_pSendWidget->dataSendBtn()->setEnabled(false);
    }
}

void BytetraceBase::sendData(const QByteArray& rawData) {
    QMutexLocker<QMutex> locker(&m_serialMutex);
    try {
        if (m_serialPortManager && m_serialPortManager->isOpen()) {
            if (m_serialPortManager->writeData(rawData)) {
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
    if (!m_transferAnimation || !m_pLeftBar->transferLabel()) {
        return;
    }

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
    if (m_portMonitorTimer) {
        m_portMonitorTimer->stop();
    }
    if (m_transferAnimation) {
        m_transferAnimation->stop();
    }
    if (m_serialPortManager && m_serialPortManager->isOpen()) {
        m_serialPortManager->closePort();
    }
    delete ui;
}
