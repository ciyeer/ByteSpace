#include "accountpage.h"
#include "ui_accountpage.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>

AccountPage::AccountPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AccountPage) {
    ui->setupUi(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(30, 30, 30, 30);
    layout->setSpacing(20);

    QLabel *titleLabel = new QLabel("账号设置", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    layout->addWidget(titleLabel);

    // 账号信息组
    QGroupBox *accountGroup = new QGroupBox("账号信息", this);
    QVBoxLayout *accountLayout = new QVBoxLayout(accountGroup);

    QHBoxLayout *userLayout = new QHBoxLayout();
    QLabel *userLabel = new QLabel("用户名:", accountGroup);
    QLineEdit *userEdit = new QLineEdit(accountGroup);
    userEdit->setPlaceholderText("请输入用户名");
    userLayout->addWidget(userLabel);
    userLayout->addWidget(userEdit);

    QPushButton *loginBtn = new QPushButton("登录", accountGroup);
    loginBtn->setFixedWidth(120);

    accountLayout->addLayout(userLayout);
    accountLayout->addWidget(loginBtn);
    accountLayout->addStretch();

    layout->addWidget(accountGroup);
    layout->addStretch();
}

AccountPage::~AccountPage() {
    delete ui;
}
