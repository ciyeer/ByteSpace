#include "settingsui.h"
#include "ui_settingsui.h"

SettingsUI::SettingsUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsUI) {
    ui->setupUi(this);
    ui->listWidget->setIconSize(QSize(24, 24));

    m_pSystemPage = new SystemPage;
    m_pAppearancePage = new AppearancePage;
    m_pHelpPage = new HelpPage;
    m_pAboutPage = new AboutPage;

    ui->stackedWidget->insertWidget(0, m_pSystemPage);
    ui->stackedWidget->insertWidget(1, m_pAppearancePage);
    ui->stackedWidget->insertWidget(2, m_pHelpPage);
    ui->stackedWidget->insertWidget(3, m_pAboutPage);

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &SettingsUI::onCategoryChanged);

    ui->listWidget->setCurrentRow(0);
}

void SettingsUI::onCategoryChanged(int index) {
    ui->stackedWidget->setCurrentIndex(index);
}

SettingsUI::~SettingsUI() {
    delete ui;
}
