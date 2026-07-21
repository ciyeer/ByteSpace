/**
 * @file serialportmanager.h
 * @brief 串口管理器
 *
 * 组合 SerialPortConfigurator 和 SerialPortOperator，
 * 提供统一的串口配置、操作和状态查询接口。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QSerialPort>
#include <QComboBox>
#include "serialportconfigurator.h"
#include "serialportoperator.h"

class SerialPortManager : public QObject {
    Q_OBJECT
public:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager();

    // === 子组件访问 ===
    SerialPortConfigurator& configurator() { return m_configurator; }
    const SerialPortConfigurator& configurator() const { return m_configurator; }

    // === UI 初始化和配置 ===
    bool initialize(QComboBox *portBox, QComboBox *baudrateBox,
                    QComboBox *databitsBox, QComboBox *parityBox,
                    QComboBox *stopBitsBox, QComboBox *flowControlBox);

    void configureSerialPort(const QString& portName, int baudRate,
                             QSerialPort::DataBits dataBits, QSerialPort::Parity parity,
                             QSerialPort::StopBits stopBits, QSerialPort::FlowControl flowControl);

    bool configurePort();

    // === 串口操作（有业务逻辑：emit 信号） ===
    bool openPort();
    void closePort();
    bool writeData(const QByteArray &data);
    QByteArray readData();

    // === 状态查询 ===
    bool isOpen() const;
    bool isWritable() const;
    bool isReadable() const;

    // === 常用信息查询（外部调用频率高，保留便捷方法） ===
    QString getPortName() const;
    QSerialPort::SerialPortError getError() const;

    // === 持续读取 ===
    void startReading();

signals:
    void portOpened();
    void portClosed();
    void dataReceived(const QByteArray &data);
    void dataWritten(const QByteArray &data);
    void dataRead(const QByteArray &data);
    void portAdded(const QString &portName);
    void portRemoved(const QString &portName);
    void serialPortError(QSerialPort::SerialPortError error);

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void handleReadyRead();

private:
    SerialPortConfigurator m_configurator;
    SerialPortOperator m_operator;
};

#endif // SERIALPORTMANAGER_H
