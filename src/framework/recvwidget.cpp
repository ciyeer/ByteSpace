#include "recvwidget.h"
#include "ui_recvwidget.h"

RecvWidget::RecvWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecvWidget)
{
    ui->setupUi(this);
}

RecvWidget::~RecvWidget()
{
    delete ui;
}

QTextBrowser* RecvWidget::recvArea() const { return ui->recvArea; }
