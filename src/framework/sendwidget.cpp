#include "sendwidget.h"
#include "ui_sendwidget.h"

SendWidget::SendWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SendWidget)
{
    ui->setupUi(this);
}

SendWidget::~SendWidget()
{
    delete ui;
}

QTextEdit* SendWidget::sendArea() const { return ui->sendArea; }
QPushButton* SendWidget::dataSendBtn() const { return ui->dataSendBtn; }
QPushButton* SendWidget::clearSendBtn() const { return ui->clearSendBtn; }
QPushButton* SendWidget::clearRecvBtn() const { return ui->clearRecvBtn; }
