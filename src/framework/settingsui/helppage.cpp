#include "helppage.h"
#include "ui_helppage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPushButton>

HelpPage::HelpPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HelpPage) {
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel *titleLabel = new QLabel("帮助", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // 帮助内容
    QGroupBox *helpGroup = new QGroupBox("帮助内容", this);
    QVBoxLayout *helpLayout = new QVBoxLayout(helpGroup);

    QLabel *versionLabel = new QLabel("版本: 1.3.3", helpGroup);
    QLabel *copyrightLabel = new QLabel("版权所有 © 2024", helpGroup);

    QPushButton *userGuideBtn = new QPushButton("用户指南", helpGroup);
    QPushButton *faqBtn = new QPushButton("常见问题", helpGroup);
    QPushButton *aboutBtn = new QPushButton("关于", helpGroup);

    userGuideBtn->setFixedWidth(120);
    faqBtn->setFixedWidth(120);
    aboutBtn->setFixedWidth(120);

    helpLayout->addWidget(versionLabel);
    helpLayout->addWidget(copyrightLabel);
    helpLayout->addWidget(userGuideBtn);
    helpLayout->addWidget(faqBtn);
    helpLayout->addWidget(aboutBtn);
    helpLayout->addStretch();

    layout->addWidget(helpGroup);
    layout->addStretch();
}

HelpPage::~HelpPage() {
    delete ui;
}
