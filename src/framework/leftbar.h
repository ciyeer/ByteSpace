#ifndef LEFTBAR_H
#define LEFTBAR_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLabel>
#include <QRadioButton>
#include <QLineEdit>
#include <QSerialPort>

namespace Ui {
class LeftBar;
}

class LeftBar : public QWidget {
    Q_OBJECT

public:
    explicit LeftBar(QWidget *parent = nullptr);
    ~LeftBar();

    // === 语义化配置访问 ===
    QString portName() const;
    int baudRate() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::Parity parity() const;
    QSerialPort::StopBits stopBits() const;
    QSerialPort::FlowControl flowControl() const;
    QString parityText() const;
    QString flowControlText() const;

    // 发送选项
    bool isHexDisplay() const;
    bool isHexSend() const;
    bool isCrcEnabled() const;
    bool isTimedSend() const;
    int timedSendInterval() const;
    bool isSendNewline() const;
    bool isAutoNewline() const;
    bool isFileTransfer() const;
    bool isSaveToFile() const;

    // DTR/RTS
    bool isDtrEnabled() const;
    bool isRtsEnabled() const;

    // === UI 状态控制 ===
    void setPortOpen(bool open);

    // === 初始化用控件访问（仅供 SerialPortConfigurator 填充列表） ===
    QComboBox* portComboBox() const;
    QComboBox* baudrateComboBox() const;
    QComboBox* dataBitsComboBox() const;
    QComboBox* parityComboBox() const;
    QComboBox* stopBitsComboBox() const;
    QComboBox* flowControlComboBox() const;

    // === 按钮和动画控件 ===
    QPushButton* openCloseButton() const;
    QLabel* transferLabel() const;

    // === 控件访问（用于信号连接） ===
    QRadioButton* hexSendBtn() const;
    QRadioButton* crcBtn() const;
    QRadioButton* timedSendBtn() const;
    QRadioButton* sendNewlineBtn() const;
    QRadioButton* autoNewlineBtn() const;
    QRadioButton* fileTransferBtn() const;
    QRadioButton* saveToFileBtn() const;
    QRadioButton* dtrBtn() const;
    QRadioButton* rtsBtn() const;
    QLineEdit* timedSendIntervalEdit() const;

private:
    Ui::LeftBar *ui;
};

#endif // LEFTBAR_H
