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
    
    bool initialize(QComboBox *portBox, QComboBox *baudrateBox,
                    QComboBox *databitsBox, QComboBox *parityBox,
                    QComboBox *stopBitsBox, QComboBox *flowControlBox);

    void configureSerialPort(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                              QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                              QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl);

    bool openPort();
    void closePort();
    bool writeData(const QByteArray &data);
    QByteArray readData();
    
    bool configurePort();

    bool isOpen() const;
    bool isWritable() const;
    bool isReadable() const;

    // 设置串口参数
    void setPortName(const QString& portName);
    void setBaudRate(int baudRate);
    void setDataBits(QSerialPort::DataBits dataBits);
    void setParity(QSerialPort::Parity parity);
    void setStopBits(QSerialPort::StopBits stopBits);
    void setFlowControl(QSerialPort::FlowControl flowControl);

    // 获取串口信息
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
    QSerialPort* m_serialPort;
    QString m_portName;
    int m_baudRate;
    QSerialPort::DataBits m_dataBits;
    QSerialPort::Parity m_parity;
    QSerialPort::StopBits m_stopBits;
    QSerialPort::FlowControl m_flowControl;
    SerialPortConfigurator m_configurator;
    SerialPortOperator m_operator;
};

#endif // SERIALPORTMANAGER_H
