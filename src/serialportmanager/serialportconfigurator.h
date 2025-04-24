#ifndef SERIALPORTCONFIGURATOR_H
#define SERIALPORTCONFIGURATOR_H

#include <QObject>
#include <QSerialPort>
#include <QComboBox>
#include <QSerialPortInfo>

class SerialPortConfigurator : public QObject {
    Q_OBJECT
public:
    explicit SerialPortConfigurator(QObject *parent = nullptr);
    ~SerialPortConfigurator();  // 析构函数，确保关闭串口

    // UI 初始化和配置方法
    void initialize(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                    QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                    QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl);

    void configure(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                   QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                   QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl);

    // 从配置管理器加载配置
    bool configureFromSettings();

    // 获取配置好的串口对象
    QSerialPort* serialPort() const;

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

    // 串口信息查询
    QString getAvailablePortsInfo() const;
    bool isValidPort(const QString &portName) const;
    bool isInitialized() const;

private:
    // 映射方法
    QSerialPort::DataBits mapDataBits(int index) const;
    QSerialPort::Parity mapParity(int index) const;
    QSerialPort::StopBits mapStopBits(int index) const;
    QSerialPort::FlowControl mapFlowControl(int index) const;

private:
    QSerialPort *m_serialPort;  // 由于QObject父子关系管理，不需要改为智能指针
    bool m_isInitialized{false};
};

#endif // SERIALPORTCONFIGURATOR_H
