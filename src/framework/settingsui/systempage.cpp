#include "systempage.h"
#include "ui_systempage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>

SystemPage::SystemPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SystemPage) {
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    // 添加标题
    QLabel *titleLabel = new QLabel("系统设置", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // 添加设置组
    QGroupBox *generalGroup = new QGroupBox("常规设置", this);
    QVBoxLayout *generalLayout = new QVBoxLayout(generalGroup);

    // 添加设置项
    QHBoxLayout *updateLayout = new QHBoxLayout();
    QLabel *updateLabel = new QLabel("升级配置:", generalGroup);
    QComboBox *updateCombo = new QComboBox(generalGroup);
    updateCombo->addItem("自动升级");
    updateCombo->addItem("手动升级");
    updateCombo->addItem("不升级");
    updateLayout->addWidget(updateLabel);
    updateLayout->addWidget(updateCombo);
    updateLayout->addStretch();

    QHBoxLayout *proxyLayout = new QHBoxLayout();
    QLabel *proxyLabel = new QLabel("网络代理:", generalGroup);
    QComboBox *proxyCombo = new QComboBox(generalGroup);
    proxyCombo->addItem("无代理");
    proxyCombo->addItem("系统代理");
    proxyCombo->addItem("自定义代理");
    proxyLayout->addWidget(proxyLabel);
    proxyLayout->addWidget(proxyCombo);
    proxyLayout->addStretch();

    QCheckBox *betaCheck = new QCheckBox("测试版", generalGroup);
    QPushButton *checkUpdateBtn = new QPushButton("检查更新", generalGroup);
    checkUpdateBtn->setFixedWidth(120);

    generalLayout->addLayout(updateLayout);
    generalLayout->addLayout(proxyLayout);
    generalLayout->addWidget(betaCheck);
    generalLayout->addWidget(checkUpdateBtn);
    generalLayout->addStretch();

    layout->addWidget(generalGroup);
    layout->addStretch();
}

SystemPage::~SystemPage() {
    delete ui;
}
