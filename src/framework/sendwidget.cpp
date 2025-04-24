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


