/**
 * @file titlebar.cpp
 * @brief 标题栏实现
 *
 * @author ByteSpace团队
 * @date 2024
 */
#include "titlebar.h"
#include "baseframework.h"
#include "ui_titlebar.h"
#include <QMouseEvent>
#include <QEvent>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TitleBar) {
    ui->setupUi(this);

    ui->labelLogo->installEventFilter(this);

    connect(ui->btnMin, &QPushButton::clicked, this, [=]() {
        if (QWidget *window = qobject_cast<QWidget *>(parentWidget())) {
            window->showMinimized();
        }
    });

    connect(ui->btnMax, &QPushButton::clicked, this, [=]() {
        if (auto *base = qobject_cast<BaseFramework*>(parentWidget())) {
            base->toggleMaximize();
            updateMaxButton();
        }
    });

    connect(ui->btnClose, &QPushButton::clicked, this, [=]() {
        if (QWidget *window = qobject_cast<QWidget *>(parentWidget())) {
            window->close();
        }
    });

    connect(ui->btnSPin, &QPushButton::clicked, this, &TitleBar::settingsButtonClicked);

    updateMaxButton();
}

bool TitleBar::event(QEvent *e) {
    if (e->type() == QEvent::ParentChange && parentWidget()) {
        parentWidget()->installEventFilter(this);
    }
    return QWidget::event(e);
}

TitleBar::~TitleBar() {
    delete ui;
}

bool TitleBar::eventFilter(QObject *watched, QEvent *event) {
    if (watched == ui->labelLogo && event->type() == QEvent::MouseButtonRelease) {
        emit settingsButtonClicked();
        return true;
    }
    if (watched == parentWidget() && event->type() == QEvent::WindowStateChange) {
        updateMaxButton();
    }
    return QWidget::eventFilter(watched, event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (auto *base = qobject_cast<BaseFramework*>(parentWidget())) {
            base->toggleMaximize();
            updateMaxButton();
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::updateMaxButton() {
    bool maximized = false;
    if (auto *base = qobject_cast<BaseFramework*>(parentWidget())) {
        maximized = base->isMaximized() || base->isCustomMaximized();
    }
    ui->btnMax->setText(maximized ? QStringLiteral("\u2750") : QStringLiteral("\u25A1"));
}
