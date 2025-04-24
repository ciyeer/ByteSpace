#include "appearancepage.h"
#include "ui_appearancepage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>

AppearancePage::AppearancePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AppearancePage) {
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel *titleLabel = new QLabel("外观设置", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // 主题设置组
    QGroupBox *themeGroup = new QGroupBox("主题设置", this);
    QVBoxLayout *themeLayout = new QVBoxLayout(themeGroup);

    QHBoxLayout *themeSelectLayout = new QHBoxLayout();
    QLabel *themeLabel = new QLabel("主题:", themeGroup);
    QComboBox *themeCombo = new QComboBox(themeGroup);
    themeCombo->addItem("浅色");
    themeCombo->addItem("深色");
    themeCombo->addItem("跟随系统");
    themeSelectLayout->addWidget(themeLabel);
    themeSelectLayout->addWidget(themeCombo);
    themeSelectLayout->addStretch();

    themeLayout->addLayout(themeSelectLayout);
    themeLayout->addStretch();

    layout->addWidget(themeGroup);
    layout->addStretch();
}

AppearancePage::~AppearancePage() {
    delete ui;
}
