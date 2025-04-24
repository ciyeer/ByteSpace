#include "titlebar.h"
#include "ui_titlebar.h"
#include <QMouseEvent>  // 添加这一行

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TitleBar) {
    ui->setupUi(this);

    // 为labelLogo安装事件过滤器
    ui->labelLogo->installEventFilter(this);

    connect(ui->btnMin, &QPushButton::clicked, this, [=]() {
        if (QWidget *window = qobject_cast<QWidget *>(parentWidget())) {
            window->showMinimized();
        }
    });

    connect(ui->btnMax, &QPushButton::clicked, this, [=]() {
        if (QWidget *window = qobject_cast<QWidget *>(parentWidget())) {
            if (window->isMaximized()) {
                window->showNormal();
            }
            else {
                window->showMaximized();
            }
        }
    });

    connect(ui->btnClose, &QPushButton::clicked, this, [=]() {
        if (QWidget *window = qobject_cast<QWidget *>(parentWidget())) {
            window->close();
        }
    });

    // 连接设置按钮的点击信号
    connect(ui->btnSPin, &QPushButton::clicked, this, &TitleBar::settingsButtonClicked);
}

TitleBar::~TitleBar() {
    delete ui;
}

// 添加 eventFilter 实现
bool TitleBar::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->labelLogo && event->type() == QEvent::MouseButtonRelease) {
        // 发送设置按钮点击信号
        emit settingsButtonClicked();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

// 实现双击事件
void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (QWidget *window = qobject_cast<QWidget *>(parentWidget())) {
            if (window->isMaximized()) {
                window->showNormal();
            }
            else {
                window->showMaximized();
            }
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}
