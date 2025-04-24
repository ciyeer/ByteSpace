#ifndef TERMNITE_H
#define TERMNITE_H

#include <QWidget>
#include <QSerialPort>
#include <QByteArray>
#include <QThreadPool>
#include <QMutex>
#include <QTimer>
#include <QScreen>
#include <QStyle>
#include <memory>
#include <QMetaObject>
#include <QSerialPortInfo>
#include "serialportmanager/serialportmanager.h"

// 在头文件中添加QMovie前向声明
class QMovie;

QT_BEGIN_NAMESPACE
namespace Ui {
class Termnite;
}
QT_END_NAMESPACE

class Termnite : public QWidget {
    Q_OBJECT

public:
    explicit Termnite(QWidget *parent = nullptr);
    ~Termnite();

private slots:  
    void handleOpenSerialPort();
    void handleOpenCloseSerialPort();
    void onSerialPortError(QSerialPort::SerialPortError error);
    void onPortOpened();
    void onTaskCompleted();
    void onTaskFailed();
    void onTimeout();
    void onDataWritten(const QByteArray &data);
    void onDataRead(const QByteArray &data);
    void onClearSendBuffer();
    void onClearRecvBuffer();
    void onDataSent();
    void onReadButtonClicked();
    void onSendButtonClicked();
    void onSerialDataReceived(const QByteArray& data);  // 处理接收到的串口数据

private:
    void connections();
    void setupConnections();
    void updateUI(bool isOpen);  
    void updateStatusBar(bool isOpen);  
    bool isPortAvailable(const QString &portName);
    void sendData(const QByteArray& data);

    void initTransferAnimation();  // 初始化传输动画
    void showTransferAnimation(bool active);  // 控制传输动画状态

private:
    Ui::Termnite *ui;
    std::shared_ptr<SerialPortManager> m_serialPortManager;
    QTimer* m_timer;
    bool m_isOpen;  
    QMovie* m_transferAnimation;
    QMutex m_serialMutex;
};
#endif // TERMNITE_H
