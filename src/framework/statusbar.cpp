#include "statusbar.h"
#include "ui_statusbar.h"

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatusBar) {
    ui->setupUi(this);
}

void StatusBar::onRecvMessage(const QString &msg) {
    if (!msg.isEmpty()) {
        ui->labelStatus->setText(msg);
    }
}

void StatusBar::updateTxRx(qint64 tx, qint64 rx) {
    ui->label_2->setText(QString::number(tx));
    ui->label_4->setText(QString::number(rx));
}

StatusBar::~StatusBar() {
    delete ui;
}
