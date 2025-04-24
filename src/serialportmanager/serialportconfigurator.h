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

    void initialize(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                    QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                    QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl);

    void configure(QComboBox *comBoxPortName, QComboBox *comBoxBaudRate,
                   QComboBox *comBoxDataBits, QComboBox *comBoxParity,
                   QComboBox *comBoxStopBits, QComboBox *comBoxFlowControl);

    bool isInitialized() const;

private:
    QSerialPort::DataBits mapDataBits(int index) const;
    QSerialPort::Parity mapParity(int index) const;
    QSerialPort::StopBits mapStopBits(int index) const;
    QSerialPort::FlowControl mapFlowControl(int index) const;

private:
    QSerialPort *m_serialPort;
    bool m_isInitialized{false};
};

#endif // SERIALPORTCONFIGURATOR_H
