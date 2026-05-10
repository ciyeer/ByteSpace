/**
 * @file bytetracebase.h
 * @brief 串口追踪主界面
 *
 * 包含串口管理、数据收发、传输动画等核心功能。
 *
 * @author ByteSpace团队
 * @date 2024
 */
#ifndef BYTETRACEBASE_H
#define BYTETRACEBASE_H

#include <QWidget>
#include <QSerialPort>
#include <QByteArray>
#include <QTimer>
#include <QMutex>
#include <memory>
#include "leftbar.h"
#include "recvwidget.h"
#include "sendwidget.h"
#include "serialportmanager/serialportmanager.h"

class QMovie;

namespace Ui {
class BytetraceBase;
}

class BytetraceBase : public QWidget {
    Q_OBJECT

public:
    explicit BytetraceBase(QWidget *parent = nullptr);
    ~BytetraceBase();

private slots:
    void handleOpenCloseSerialPort();
    void onSerialPortError(QSerialPort::SerialPortError error);
    void onSerialDataReceived(const QByteArray& data);
    void onSendButtonClicked();
    void onClearSendBuffer();
    void onClearRecvBuffer();
    void onTimeout();
    void onTaskCompleted();
    void onTaskFailed();

private:
    void connections();
    void updateUI(bool isOpen);
    void sendData(const QByteArray& data);
    void initTransferAnimation();
    void showTransferAnimation(bool active);

    Ui::BytetraceBase *ui;
    LeftBar     *m_pLeftBar;
    RecvWidget  *m_pRecvWidget;
    SendWidget  *m_pSendWidget;

    std::shared_ptr<SerialPortManager> m_serialPortManager;
    QTimer* m_portMonitorTimer;
    QMovie* m_transferAnimation;
    QMutex m_serialMutex;
    bool m_isOpen;
};

#endif // BYTETRACEBASE_H
