#include "termnite.h"
#include "./ui_termnite.h"
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QMovie>
#include "logmanager/logutils.h"
#include "taskmanager/taskmanager.h"
#include "utils/errorhandler.h"

Termnite::Termnite(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Termnite)
    , m_serialPortManager(std::make_shared<SerialPortManager>())
    , m_timer(new QTimer(this))
    , m_isOpen(false)
    , m_transferAnimation(nullptr) {  // 初始化动画指针
    ui->setupUi(this);
    
    // 初始化定时器
    m_timer->setInterval(1000);  // 设置1秒的检查间隔
    connect(m_timer, &QTimer::timeout, this, &Termnite::onTimeout);
    m_timer->start();
    
    // 使用正确的成员变量名
    m_serialPortManager->initialize(ui->portComboBox, ui->baudrateComboBox,
                                  ui->dataBitsComboBox, ui->parityComboBox,
                                  ui->stopBitsComboBox, ui->flowControlComboBox);
    connections();
    // 初始化传输动画
    initTransferAnimation();
}

void Termnite::connections() {
    // 连接开关端口按钮
    connect(ui->openCloseButton, &QPushButton::clicked, this, &Termnite::handleOpenCloseSerialPort);
    
    // 确保这些连接是正确的
    connect(m_serialPortManager.get(), &SerialPortManager::portOpened, this, &Termnite::onPortOpened, Qt::QueuedConnection);
    connect(m_serialPortManager.get(), &SerialPortManager::serialPortError, this, &Termnite::onSerialPortError, Qt::QueuedConnection);
    
    // 任务完成信号连接
    connect(&TaskManager::getInstance(), &TaskManager::taskCompleted, this, &Termnite::onTaskCompleted, Qt::QueuedConnection);
    connect(&TaskManager::getInstance(), &TaskManager::taskFailed, this, &Termnite::onTaskFailed, Qt::QueuedConnection);
    
    // 添加发送按钮的连接
    connect(ui->dataSendBtn, &QPushButton::clicked, this, &Termnite::onSendButtonClicked);
    
    // 添加清除按钮的连接
    connect(ui->clearSendBtn, &QPushButton::clicked, this, &Termnite::onClearSendBuffer);
    connect(ui->clearRecvBtn, &QPushButton::clicked, this, &Termnite::onClearRecvBuffer);
}

void Termnite::onSerialPortError(QSerialPort::SerialPortError error) {
    // 使用ErrorHandler获取错误消息
    QString errorMsg = ErrorHandler::instance().getErrorMessage(error);
    
    // // 更新UI显示错误
    // if (QThread::currentThread() != qApp->thread()) {
    //     QMetaObject::invokeMethod(this, "updateErrorStatus", Qt::QueuedConnection,
    //                              Q_ARG(QString, errorMsg));
    // }
    // else {
    //     updateErrorStatus(errorMsg);
    // }
    
    // 如果是严重错误，关闭串口
    if (error == QSerialPort::ResourceError) {
        m_serialPortManager->closePort();
        m_isOpen = false;
        updateUI(false);
    }
}

void Termnite::onClearSendBuffer() {
    ui->sendArea->clear();
}

void Termnite::onClearRecvBuffer() {
    ui->recvArea->clear();
}

void Termnite::onDataWritten(const QByteArray &data) {
    Q_UNUSED(data)
}

void Termnite::onDataRead(const QByteArray &data) {
    Q_UNUSED(data)
}

void Termnite::onTimeout() {
    if (m_isOpen && !m_serialPortManager->isOpen()) {
        LogUtils::logMessage("Serial port unexpectedly closed. Attempting recovery...", LOG::LOG_WARNING);
        m_isOpen = false;
        updateUI(false);
    }

    QList<QSerialPortInfo> currentPorts = QSerialPortInfo::availablePorts();
    QString currentPort = ui->portComboBox->currentText();

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
        }
        updateUI(false);
    }
}

void Termnite::onPortOpened() {
    // 始终使用 invokeMethod 确保在主线程中执行
    if (QThread::currentThread() != qApp->thread()) {
        QMetaObject::invokeMethod(this, "onPortOpened", Qt::QueuedConnection);
        return;
    }

    m_isOpen = true;
    LogUtils::logMessage("Port " + m_serialPortManager->getPortName() + " opened successfully.", LOG::LOG_INFO);
    updateUI(true);
    ui->openCloseButton->setEnabled(true);  // 重新启用按钮
    
    // 断开之前的连接，避免重复连接
    disconnect(m_serialPortManager.get(), &SerialPortManager::dataRead, 
            this, &Termnite::onSerialDataReceived);
    
    // 重新连接数据接收信号
    connect(m_serialPortManager.get(), &SerialPortManager::dataRead, 
            this, &Termnite::onSerialDataReceived, Qt::QueuedConnection);
    
    // 启用自动读取
    m_serialPortManager->startReading();
    
    LogUtils::logMessage("串口数据接收监听已启动", LOG::LOG_INFO);
}

void Termnite::onSerialDataReceived(const QByteArray& data) {
    // 处理接收到的数据
    LogUtils::logMessage(QString("接收到 %1 字节数据").arg(data.size()), LOG::LOG_INFO);
    
    // 记录接收到的原始数据（十六进制格式）
    QString hexData;
    for (int i = 0; i < data.size(); ++i) {
        hexData += QString("%1 ").arg(static_cast<unsigned char>(data[i]), 2, 16, QChar('0'));
    }
    LogUtils::logMessage(QString("接收数据内容(HEX): %1").arg(hexData.trimmed()), LOG::LOG_DEBUG);
    
    // 根据接收模式显示数据
    if (ui->recvHexBtn->isChecked()) {
        // 十六进制模式显示
        ui->recvArea->append(hexData.trimmed());
    } else {
        // ASCII模式显示
        // 尝试将数据转换为UTF-8字符串
        QString textData = QString::fromUtf8(data);
        ui->recvArea->append(textData);
    }
}

void Termnite::onTaskCompleted() {
    // 任务完成后更新UI或执行其他操作
}

void Termnite::onTaskFailed() {

}

void Termnite::updateUI(bool isOpen) {
    // 更新按钮文本和状态
    if (ui->openCloseButton) {
        if (isOpen) {
            ui->openCloseButton->setText("关闭串口");
            ui->openCloseButton->setStyleSheet("background-color: #FF6B6B;");
            
            // 禁用串口设置控件
            if (ui->portComboBox) ui->portComboBox->setEnabled(false);
            if (ui->baudrateComboBox) ui->baudrateComboBox->setEnabled(false);
            if (ui->dataBitsComboBox) ui->dataBitsComboBox->setEnabled(false);
            if (ui->parityComboBox) ui->parityComboBox->setEnabled(false);
            if (ui->stopBitsComboBox) ui->stopBitsComboBox->setEnabled(false);
            if (ui->flowControlComboBox) ui->flowControlComboBox->setEnabled(false);
            
            // 启用发送按钮
            if (ui->dataSendBtn) ui->dataSendBtn->setEnabled(true);
        }
        else {
            ui->openCloseButton->setText("打开串口");
            ui->openCloseButton->setStyleSheet("background-color: #4CAF50;");
            
            // 启用串口设置控件
            if (ui->portComboBox) ui->portComboBox->setEnabled(true);
            if (ui->baudrateComboBox) ui->baudrateComboBox->setEnabled(true);
            if (ui->dataBitsComboBox) ui->dataBitsComboBox->setEnabled(true);
            if (ui->parityComboBox) ui->parityComboBox->setEnabled(true);
            if (ui->stopBitsComboBox) ui->stopBitsComboBox->setEnabled(true);
            if (ui->flowControlComboBox) ui->flowControlComboBox->setEnabled(true);
            
            // 禁用发送按钮
            if (ui->dataSendBtn) ui->dataSendBtn->setEnabled(false);
        }
    }
    
    // 更新状态栏
    updateStatusBar(isOpen);
}

void Termnite::handleOpenSerialPort() {
    try {
        // 获取用户在界面上选择的串口名称
        QString selectedPort = ui->portComboBox->currentText();
        
        // 检查串口名称是否为空
        if (selectedPort.isEmpty()) {
            LogUtils::logMessage("串口名称为空，请先选择串口", LOG::LOG_ERROR);
            QMessageBox::warning(this, "错误", "请先选择串口");
            return;
        }
        
        // 设置串口名称到管理器
        m_serialPortManager->setPortName(selectedPort);
        
        LogUtils::logMessage("正在打开串口 " + selectedPort + "...", LOG::LOG_INFO);
        
        // 直接调用串口管理器的打开方法
        if (m_serialPortManager->openPort()) {
            m_isOpen = true;
            updateUI(true);
            LogUtils::logMessage("串口 " + selectedPort + " 打开成功", LOG::LOG_INFO);
        }
        else {
            m_isOpen = false;
            updateUI(false);
            LogUtils::logMessage("串口 " + selectedPort + " 打开失败", LOG::LOG_ERROR);
        }
    }
    catch (const std::exception& e) {
        LogUtils::logMessage(QString("打开串口异常: %1").arg(e.what()), LOG::LOG_ERROR);
        m_isOpen = false;
        updateUI(false);
    }
}

void Termnite::handleOpenCloseSerialPort() {
    bool isCurrentlyOpen = m_serialPortManager->isOpen();
    
    if (isCurrentlyOpen) {
        LogUtils::logMessage("正在关闭串口...", LOG::LOG_INFO);
        m_serialPortManager->closePort();
        m_isOpen = false;
        updateUI(false);
        
        // 串口关闭时停止动画
        showTransferAnimation(false);
        
        LogUtils::logMessage("串口已关闭", LOG::LOG_INFO);
    } 
    else {
        // 获取用户在界面上选择的串口名称
        QString selectedPort = ui->portComboBox->currentText();
        LogUtils::logMessage("用户选择的串口: " + selectedPort, LOG::LOG_INFO);
        
        // 设置串口名称到管理器
        m_serialPortManager->setPortName(selectedPort);
        
        // 记录串口配置信息
        LogUtils::logMessage(QString("串口配置 - 波特率: %1, 数据位: %2, 校验位: %3, 停止位: %4")
            .arg(ui->baudrateComboBox->currentText(),
                 ui->dataBitsComboBox->currentText(),
                 ui->parityComboBox->currentText(),
                 ui->stopBitsComboBox->currentText()), LOG::LOG_INFO);
        
        // 正在打开串口
        LogUtils::logMessage("正在打开串口 " + selectedPort + "...", LOG::LOG_INFO);
        
        // 直接打开串口，不使用任务
        if (m_serialPortManager->openPort()) {
            m_isOpen = true;
            updateUI(true);
            
            // 串口打开时启动动画并持续播放
            showTransferAnimation(true);
            
            LogUtils::logMessage("串口 " + selectedPort + " 打开成功", LOG::LOG_INFO);
        } 
        else {
            m_isOpen = false;
            updateUI(false);
            LogUtils::logMessage("串口 " + selectedPort + " 打开失败", LOG::LOG_ERROR);
        }
    }
}

bool Termnite::isPortAvailable(const QString &portName) {
    try {
        for (const QSerialPortInfo &info : QSerialPortInfo::availablePorts()) {
            if (info.portName() == portName) {
                QSerialPort testPort(info);
                if (testPort.open(QIODevice::ReadWrite)) {
                    testPort.close();
                    return true;
                }
                break;
            }
        }
    } 
    catch (const std::exception& e) {
        LogUtils::logMessage(QString("检查串口可用性时发生异常: %1").arg(e.what()), LOG::LOG_ERROR);
    }
    return false;
}

// 添加初始化动画的方法
void Termnite::initTransferAnimation() {
    // 创建QMovie对象
    m_transferAnimation = new QMovie(this);
    m_transferAnimation->setFileName(":/images/transfer.gif");  // 设置GIF文件路径
    
    // 检查动画是否可用
    if (!m_transferAnimation->isValid()) {
        LogUtils::logMessage("动画资源加载失败", LOG::LOG_ERROR);
        return;
    }
    
    // 设置Label的大小策略
    ui->transferLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        
    // 设置缩放模式
    ui->transferLabel->setScaledContents(true);
    ui->transferLabel->setAlignment(Qt::AlignCenter);
        
    // 设置动画
    ui->transferLabel->setMovie(m_transferAnimation);
        
    // 设置动画大小
    m_transferAnimation->setScaledSize(ui->transferLabel->size());
        
    // 设置动画速度（默认是100，数值越大播放越快）
    m_transferAnimation->setSpeed(300);  // 设置为原速度的3倍
        
    // 默认显示动画但不播放
    m_transferAnimation->start();
    m_transferAnimation->setPaused(true);
}

// 添加控制动画状态的方法
void Termnite::showTransferAnimation(bool active) {
    if (!m_transferAnimation || !ui->transferLabel) {
        return;
    }
    
    if (active) {
        m_transferAnimation->start();  // 重新开始动画
    }
    else {
        m_transferAnimation->stop();   // 完全停止动画
        m_transferAnimation->start();  // 重新开始以显示第一帧
        m_transferAnimation->jumpToFrame(0);  // 跳转到第一帧
        m_transferAnimation->setPaused(true); // 暂停在第一帧
    }
}

// 修改数据发送方法，移除动画控制相关代码
void Termnite::sendData(const QByteArray& data) {
    QMutexLocker<QMutex> locker(&m_serialMutex);
    try {
        // 直接使用 SerialPortManager 发送数据，而不是通过任务
        if (m_serialPortManager && m_serialPortManager->isOpen()) {
            if (m_serialPortManager->writeData(data)) {
                LogUtils::logMessage("数据发送成功", LOG::LOG_INFO);
            } 
            else {
                LogUtils::logMessage("数据发送失败", LOG::LOG_ERROR);
                onSerialPortError(m_serialPortManager->getError());
            }
        } 
        else {
            LogUtils::logMessage("串口未打开，无法发送数据", LOG::LOG_ERROR);
            onSerialPortError(QSerialPort::NotOpenError);
        }
    }
    catch (const std::exception& e) {
        QString error = QString("发送数据异常: %1").arg(e.what());
        LogUtils::logMessage(error, LOG::LOG_ERROR);
        onSerialPortError(QSerialPort::WriteError);
    }
}

// 修改析构函数，释放动画资源
Termnite::~Termnite() {
    // 先停止定时器
    if (m_timer) {
        m_timer->stop();
    }
    
    // 停止并释放动画
    if (m_transferAnimation) {
        m_transferAnimation->stop();
        delete m_transferAnimation;
        m_transferAnimation = nullptr;
    }
    
    // 关闭串口
    if (m_serialPortManager && m_serialPortManager->isOpen()) {
        m_serialPortManager->closePort();
        LogUtils::logMessage("串口关闭", LOG::LOG_INFO);
    }
    delete ui;
}

void Termnite::onSendButtonClicked() {
    if (!m_serialPortManager || !m_serialPortManager->isOpen()) {
        LogUtils::logMessage("串口未打开，无法发送数据", LOG::LOG_ERROR);
        QMessageBox::warning(this, "错误", "串口未打开，请先打开串口");
        return;
    }
    
    // 获取发送区域的文本
    QString text = ui->sendArea->toPlainText();
    if (text.isEmpty()) {
        LogUtils::logMessage("发送内容为空", LOG::LOG_WARNING);
        return;
    }
    
    QByteArray data = text.toUtf8();
    LogUtils::logMessage(QString("准备发送 %1 字节数据").arg(data.size()), LOG::LOG_INFO);
    
    // 发送数据
    sendData(data);
}

void Termnite::onDataSent() {
    // 数据发送完成后的处理
    LogUtils::logMessage("数据发送完成", LOG::LOG_INFO);
}

void Termnite::onReadButtonClicked() {
    if (!m_serialPortManager || !m_serialPortManager->isOpen()) {
        LogUtils::logMessage("串口未打开，无法读取数据", LOG::LOG_ERROR);
        QMessageBox::warning(this, "错误", "串口未打开，请先打开串口");
        return;
    }
    
    // Start reading data
    m_serialPortManager->startReading();
    LogUtils::logMessage("开始读取串口数据", LOG::LOG_INFO);
}

void Termnite::updateStatusBar(bool isOpen) {
    Q_UNUSED(isOpen)
    // if (isOpen) {
    //     QString portInfo = QString("%1 - %2, %3, %4, %5, %6")
    //         .arg(m_serialPortManager->getPortName())
    //         .arg(ui->baudrateComboBox->currentText())
    //         .arg(ui->dataBitsComboBox->currentText())
    //         .arg(ui->parityComboBox->currentText())
    //         .arg(ui->stopBitsComboBox->currentText())
    //         .arg(ui->flowControlComboBox->currentText());
        
    //     ui->statusLabel->setText("已连接: " + portInfo);
    //     ui->statusLabel->setStyleSheet("color: #4CAF50;"); // 绿色
    // }
    // else {
    //     ui->statusLabel->setText("未连接");
    //     ui->statusLabel->setStyleSheet("color: #FF6B6B;"); // 红色
    // }
}
