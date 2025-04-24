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
    
    // UI 初始化和配置方法 - 委托给 SerialPortConfigurator
    bool initialize(QComboBox *portBox, QComboBox *baudrateBox,
                    QComboBox *databitsBox, QComboBox *parityBox,
                    QComboBox *stopBitsBox, QComboBox *flowControlBox);

    void configureSerialPort(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                             QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                             QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl);

    // 串口操作方法 - 委托给 SerialPortOperator
    bool openPort();
    void closePort();
    bool writeData(const QByteArray &data);
    QByteArray readData();
    
    // 从配置管理器加载配置
    bool configurePort();

    // 状态查询方法
    bool isOpen() const;
    bool isWritable() const;
    bool isReadable() const;

    // 设置串口参数 - 委托给 SerialPortConfigurator
    void setPortName(const QString& portName);
    void setBaudRate(int baudRate);
    void setDataBits(QSerialPort::DataBits dataBits);
    void setParity(QSerialPort::Parity parity);
    void setStopBits(QSerialPort::StopBits stopBits);
    void setFlowControl(QSerialPort::FlowControl flowControl);

    // 获取串口信息 - 委托给 SerialPortConfigurator
    QString getPortName() const;
    int getBaudRate() const;
    QSerialPort::DataBits getDataBits() const;
    QSerialPort::Parity getParity() const;
    QSerialPort::StopBits getStopBits() const;
    QSerialPort::FlowControl getFlowControl() const;
    QSerialPort::SerialPortError getError() const;

    QString getAvailablePortsInfo() const;
    bool isValidPort(const QString &portName) const;

    // 启动持续读取
    void startReading();

signals:
    void portOpened();
    void portClosed();
    void dataReceived(const QByteArray &data);
    void dataWritten(const QByteArray &data);
    void dataRead(const QByteArray &data);
    void portAdded(const QString &portName);
    void portRemoved(const QString &portName);
    void serialPortError(QSerialPort::SerialPortError error);  // 统一处理所有错误

private slots:
    void handleError(QSerialPort::SerialPortError error);
    void handleReadyRead();

private:
    SerialPortConfigurator m_configurator;
    SerialPortOperator m_operator;
};

#endif // SERIALPORTMANAGER_H
