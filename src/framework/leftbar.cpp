#include "leftbar.h"
#include "ui_leftbar.h"

LeftBar::LeftBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LeftBar)
{
    ui->setupUi(this);
}

LeftBar::~LeftBar()
{
    delete ui;
}

// === 语义化配置访问 ===

QString LeftBar::portName() const {
    return ui->portComboBox->currentText();
}

int LeftBar::baudRate() const {
    // 优先使用预设数据，如果是自定义输入则解析文本
    QVariant data = ui->baudrateComboBox->currentData();
    if (data.isValid() && data.toInt() > 0) {
        return data.toInt();
    }
    return ui->baudrateComboBox->currentText().toInt();
}

QSerialPort::DataBits LeftBar::dataBits() const {
    return ui->dataBitsComboBox->currentData().value<QSerialPort::DataBits>();
}

QSerialPort::Parity LeftBar::parity() const {
    return ui->parityComboBox->currentData().value<QSerialPort::Parity>();
}

QSerialPort::StopBits LeftBar::stopBits() const {
    return ui->stopBitsComboBox->currentData().value<QSerialPort::StopBits>();
}

QSerialPort::FlowControl LeftBar::flowControl() const {
    return ui->flowControlComboBox->currentData().value<QSerialPort::FlowControl>();
}

QString LeftBar::parityText() const {
    return ui->parityComboBox->currentText();
}

QString LeftBar::flowControlText() const {
    return ui->flowControlComboBox->currentText();
}

// 显示/发送选项
bool LeftBar::isHexDisplay() const { return ui->hexDisplayBtn->isChecked(); }
bool LeftBar::isHexSend() const { return ui->hexSendBtn->isChecked(); }
bool LeftBar::isCrcEnabled() const { return ui->radioButton_6->isChecked(); }
bool LeftBar::isTimedSend() const { return ui->radioButton_17->isChecked(); }
int LeftBar::timedSendInterval() const { return ui->lineEdit->text().toInt(); }
bool LeftBar::isSendNewline() const { return ui->radioButton_18->isChecked(); }
bool LeftBar::isAutoNewline() const { return ui->radioButton_14->isChecked(); }
bool LeftBar::isFileTransfer() const {
    return ui->radioButton_5->isChecked() || ui->radioButton_19->isChecked();
}
bool LeftBar::isSaveToFile() const { return ui->radioButton_20->isChecked(); }

// DTR/RTS
bool LeftBar::isDtrEnabled() const { return ui->radioButton_16->isChecked(); }
bool LeftBar::isRtsEnabled() const { return ui->radioButton_15->isChecked(); }

// === UI 状态控制 ===

void LeftBar::setPortOpen(bool open) {
    QPushButton* btn = ui->openCloseButton;
    if (open) {
        btn->setText("关闭串口");
        ui->portComboBox->setEnabled(false);
        ui->baudrateComboBox->setEnabled(false);
        ui->dataBitsComboBox->setEnabled(false);
        ui->parityComboBox->setEnabled(false);
        ui->stopBitsComboBox->setEnabled(false);
        ui->flowControlComboBox->setEnabled(false);
        // DTR/RTS 仅在串口打开时可用
        ui->radioButton_16->setEnabled(true);
        ui->radioButton_15->setEnabled(true);
    } else {
        btn->setText("打开串口");
        ui->portComboBox->setEnabled(true);
        ui->baudrateComboBox->setEnabled(true);
        ui->dataBitsComboBox->setEnabled(true);
        ui->parityComboBox->setEnabled(true);
        ui->stopBitsComboBox->setEnabled(true);
        ui->flowControlComboBox->setEnabled(true);
        // 串口关闭时禁用 DTR/RTS
        ui->radioButton_16->setEnabled(false);
        ui->radioButton_15->setEnabled(false);
    }

    btn->setProperty("portOpen", open);
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
}

// === 初始化用控件访问 ===

QComboBox* LeftBar::portComboBox() const { return ui->portComboBox; }
QComboBox* LeftBar::baudrateComboBox() const { return ui->baudrateComboBox; }
QComboBox* LeftBar::dataBitsComboBox() const { return ui->dataBitsComboBox; }
QComboBox* LeftBar::parityComboBox() const { return ui->parityComboBox; }
QComboBox* LeftBar::stopBitsComboBox() const { return ui->stopBitsComboBox; }
QComboBox* LeftBar::flowControlComboBox() const { return ui->flowControlComboBox; }

// === 按钮和动画控件 ===

QPushButton* LeftBar::openCloseButton() const { return ui->openCloseButton; }
QLabel* LeftBar::transferLabel() const { return ui->transferLabel; }

// === 控件访问（用于信号连接） ===

QRadioButton* LeftBar::hexSendBtn() const { return ui->hexSendBtn; }
QRadioButton* LeftBar::crcBtn() const { return ui->radioButton_6; }
QRadioButton* LeftBar::timedSendBtn() const { return ui->radioButton_17; }
QRadioButton* LeftBar::sendNewlineBtn() const { return ui->radioButton_18; }
QRadioButton* LeftBar::autoNewlineBtn() const { return ui->radioButton_14; }
QRadioButton* LeftBar::fileTransferBtn() const { return ui->radioButton_5; }
QRadioButton* LeftBar::saveToFileBtn() const { return ui->radioButton_20; }
QRadioButton* LeftBar::dtrBtn() const { return ui->radioButton_16; }
QRadioButton* LeftBar::rtsBtn() const { return ui->radioButton_15; }
QLineEdit* LeftBar::timedSendIntervalEdit() const { return ui->lineEdit; }
