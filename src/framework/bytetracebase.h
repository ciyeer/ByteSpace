#ifndef BYTETRACEBASE_H
#define BYTETRACEBASE_H

#include <QWidget>
#include "leftbar.h"
#include "recvwidget.h"
#include "sendwidget.h"

namespace Ui {
class BytetraceBase;
}

class BytetraceBase : public QWidget {
    Q_OBJECT

public:
    explicit BytetraceBase(QWidget *parent = nullptr);
    ~BytetraceBase();

private:
    Ui::BytetraceBase *ui;
    LeftBar     *m_pLeftBar;
    RecvWidget  *m_pRecvWidget;
    SendWidget  *m_pSendWidget;
};

#endif // BYTETRACEBASE_H
