#include "framework.h"
#include "utils/configmanager.h"
#include "./ui_framework.h"

// 在 Framework 类的构造函数中添加
Framework::Framework(QWidget *parent)
    : BaseFramework(parent)
    , ui(new Ui::Framework)
    , m_pTitleBar(nullptr)
    , m_pBytetraceBase(nullptr)
    , m_pSettingsUI(nullptr)
    , m_pStatusBar(nullptr)
    , m_pStackedWidget(nullptr) {
    ui->setupUi(this);
    this->setWindowFlag(Qt::FramelessWindowHint);

    // Add this line to ensure proper geometry handling
    this->setAttribute(Qt::WA_TranslucentBackground, false);
    
    m_pTitleBar = new TitleBar;
    m_pBytetraceBase = new BytetraceBase;
    m_pSettingsUI = new SettingsUI;
    m_pStatusBar = new StatusBar;

    // 创建 QStackedWidget
    m_pStackedWidget = new QStackedWidget;
    m_pStackedWidget->addWidget(m_pBytetraceBase);  // 索引 0
    m_pStackedWidget->addWidget(m_pSettingsUI);     // 索引 1

    ui->titleBarLayout->addWidget(m_pTitleBar);
    ui->termniteLayout->addWidget(m_pStackedWidget); // 使用 QStackedWidget 替代直接添加 m_pBytetraceBase
    ui->statusBarLayout->addWidget(m_pStatusBar);

    // 连接 TitleBar 中的设置按钮信号到切换界面的槽函数
    connect(m_pTitleBar, &TitleBar::settingsButtonClicked, this, &Framework::switchWidget);

    // 恢复窗口几何信息
    if (ConfigManager::instance().rememberWindowGeometry()) {
        QByteArray geometry = ConfigManager::instance().windowGeometry();
        if (!geometry.isEmpty()) {
            restoreGeometry(geometry);
        }
    }
}

// 添加切换界面的槽函数
void Framework::switchWidget() {
    // 如果当前是 BytetraceBase，则切换到 SettingsUI，反之亦然
    if (m_pStackedWidget->currentIndex() == 0) {
        m_pStackedWidget->setCurrentIndex(1);
    } 
    else {
        m_pStackedWidget->setCurrentIndex(0);
    }
}

// 在 Framework 类的析构函数或 closeEvent 方法中添加
void Framework::closeEvent(QCloseEvent *event) {
    // 保存窗口几何信息
    if (ConfigManager::instance().rememberWindowGeometry()) {
        ConfigManager::instance().setWindowGeometry(saveGeometry());
    }
    
    QWidget::closeEvent(event);
}

Framework::~Framework() {
    delete ui;
}
