#include "settingsui.h"
#include "ui_settingsui.h"
// #include <QIcon>
// #include <QFont>
// #include <QScrollArea>

SettingsUI::SettingsUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsUI) {
    ui->setupUi(this);
    ui->listWidget->setIconSize(QSize(24, 24));
    //ui->listWidget->setSpacing(10);
    // 设置列表样式
    ui->listWidget->setStyleSheet(
        "QListWidget {"
        "   background-color: #f5f5f7;"
        "   border: none;"
        "   outline: 0;"
        "   padding: 10px 0px;"
        "}"
        "QListWidget::item {"
        "   height: 40px;"
        "   padding-left: 20px;"
        "   border-radius: 6px;"
        "   margin: 2px 10px;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #4285f4;"
        "   color: white;"
        "}"
        "QListWidget::item:hover:!selected {"
        "   background-color: #e0e0e0;"
        "}"
        );

    ui->stackedWidget->setStyleSheet(
        "QStackedWidget {"
        "   background-color: white;"
        "   border: none;"
        "}"
        );

    m_pAboutPage = new AboutPage;
    m_pAccountPage = new AccountPage;
    m_pAppearancePage = new AppearancePage;
    m_pHelpPage = new HelpPage;
    m_pSystemPage = new SystemPage;

    ui->stackedWidget->insertWidget(0, m_pSystemPage);
    ui->stackedWidget->insertWidget(1, m_pAccountPage);
    ui->stackedWidget->insertWidget(2, m_pAppearancePage);
    ui->stackedWidget->insertWidget(3, m_pHelpPage);
    ui->stackedWidget->insertWidget(4, m_pAboutPage);

    // 连接信号和槽
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &SettingsUI::onCategoryChanged);

    // 默认选中第一项
    ui->listWidget->setCurrentRow(0);
}

void SettingsUI::onCategoryChanged(int index) {
    ui->stackedWidget->setCurrentIndex(index);
}

SettingsUI::~SettingsUI() {
    delete ui;
}


