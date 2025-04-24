#include "statusbar.h"
#include "ui_statusbar.h"

StatusBar::StatusBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatusBar) {
    ui->setupUi(this);
}

void StatusBar::onRecvMessage(const QString &msg) {
    // 防止无意义更新
    if (!msg.isEmpty()) {
        ui->labelStatus->setText(msg);
    }
}

StatusBar::~StatusBar() {
    delete ui;
}
