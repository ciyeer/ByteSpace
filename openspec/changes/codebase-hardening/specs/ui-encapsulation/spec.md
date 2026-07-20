## ADDED Requirements

### Requirement: LeftBar 封装改进
LeftBar SHALL 通过信号和方法接口暴露行为，而非裸指针暴露内部控件。

#### Scenario: 串口参数变更通知
- **WHEN** 用户在 LeftBar 中修改串口参数
- **THEN** LeftBar 发出相应信号（如 `portChanged(QString)`），而非外部轮询 ComboBox

#### Scenario: 打开/关闭按钮状态控制
- **WHEN** 外部代码需要更新打开/关闭按钮状态
- **THEN** 通过 `LeftBar::setPortOpen(bool)` 方法设置，而非通过 `openCloseButton()` 裸指针操作

### Requirement: SettingsUI 清理
SettingsUI SHALL 移除 .ui 文件中的占位页面和未连接的 btnReturn 按钮。

#### Scenario: SettingsUI 无死代码
- **WHEN** 查看 `settingsui.ui` 的 stackedWidget
- **THEN** 不包含空白的占位页面（systemPage、accountPage 等 5 个）
- **AND** 不包含未连接的 btnReturn 按钮

### Requirement: SystemPage 不双重构建 UI
SystemPage SHALL 要么使用 .ui 文件，要么纯代码构建，不得两者同时使用。

#### Scenario: SystemPage 单一构建方式
- **WHEN** 查看 `systempage.cpp`
- **THEN** UI 构建方式唯一：要么仅 setupUi + .ui 绑定，要么仅代码创建
