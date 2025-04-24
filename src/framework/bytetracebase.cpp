#include "bytetracebase.h"
#include "ui_bytetracebase.h"

BytetraceBase::BytetraceBase(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BytetraceBase) {
    ui->setupUi(this);
    m_pLeftBar = new LeftBar;
    m_pRecvWidget = new RecvWidget;
    m_pSendWidget = new SendWidget;

    ui->leftbarLayout->addWidget(m_pLeftBar);
    ui->recvLayout->addWidget(m_pRecvWidget);
    ui->sendLayout->addWidget(m_pSendWidget);
}

BytetraceBase::~BytetraceBase() {
    delete ui;
}
