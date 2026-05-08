#ifndef BYTETRACEBASE_H
#define BYTETRACEBASE_H

#include <QWidget>
#include <memory>
#include <QSerialPort>
#include "leftbar.h"
#include "recvwidget.h"
#include "sendwidget.h"
#include "serialportmanager/serialportmanager.h"

namespace Ui {
class BytetraceBase;
}

class BytetraceBase : public QWidget {
    Q_OBJECT

public:
    explicit BytetraceBase(QWidget *parent = nullptr);
    ~BytetraceBase();

private:
    void setupConnections();
    void updateUiState(bool isOpen);
    bool applySerialConfig();
    void loadSerialConfigToUi();
    void persistSerialConfig() const;
    QByteArray buildSendPayload(const QString& rawText, bool* ok) const;
    quint16 crc16Modbus(const QByteArray& data) const;
    QString formatHexData(const QByteArray& data) const;

private slots:
    void handleOpenClosePort();
    void handleSendData();
    void handleClearSend();
    void handleClearRecv();
    void onSerialDataReceived(const QByteArray& data);
    void onSerialPortError(QSerialPort::SerialPortError error);

private:
    Ui::BytetraceBase *ui;
    LeftBar *m_pLeftBar;
    RecvWidget *m_pRecvWidget;
    SendWidget *m_pSendWidget;
    std::shared_ptr<SerialPortManager> m_serialPortManager;
    bool m_isPortOpen{false};
};

#endif // BYTETRACEBASE_H
