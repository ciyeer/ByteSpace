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
#include <QFile>
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

signals:
    void txRxUpdated(qint64 tx, qint64 rx);

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
    void onTimedSendTimeout();
    void onDtrToggled();
    void onRtsToggled();
    void onFileTransferClicked();
    void onSaveToFileToggled();

private:
    void connections();
    void updateUI(bool isOpen);
    void sendData(const QByteArray& data);
    QByteArray buildSendData();
    QByteArray parseHexString(const QString& hexStr);
    uint16_t calculateCrc16Modbus(const QByteArray& data);
    void updateTxRxCounters(qint64 tx, qint64 rx);
    void initTransferAnimation();
    void showTransferAnimation(bool active);

    Ui::BytetraceBase *ui;
    LeftBar     *m_pLeftBar;
    RecvWidget  *m_pRecvWidget;
    SendWidget  *m_pSendWidget;

    std::shared_ptr<SerialPortManager> m_serialPortManager;
    QTimer* m_portMonitorTimer;
    QTimer* m_timedSendTimer;
    QMovie* m_transferAnimation;
    QMutex m_serialMutex;
    bool m_isOpen;

    qint64 m_txBytes;
    qint64 m_rxBytes;
    QFile m_logFile;
    bool m_isLogging;
};

#endif // BYTETRACEBASE_H
