## ADDED Requirements

### Requirement: 移除 SendWidget 死代码
SendWidget 的 UI 文件 SHALL 移除无功能的按钮和空占位页。

#### Scenario: 无连接按钮被移除
- **WHEN** 查看 SendWidget 界面
- **THEN** "单条发送"（pushButton）、"多条发送"（pushButton_2）、"协议传输"（pushButton_3）不再显示
- **AND** "停止发送"（stopSendBtn）不再显示
- **AND** stackedWidget 的 page_2 到 page_7 空页被移除

### Requirement: 移除 LeftBar 重复控件
LeftBar 的 UI 文件 SHALL 移除重复的 HEX 发送/显示控件。

#### Scenario: 重复控件被移除
- **WHEN** 查看 LeftBar 界面
- **THEN** 只保留一组 HEX 发送和 HEX 显示控件
- **AND** 控件 objectName 符合命名规范（如 `hexSendBtn`、`hexDisplayBtn`）

### Requirement: 移除内联样式
所有 `setStyleSheet()` 调用 SHALL 从 C++ 代码中移除，样式统一在 `default.qss` 中定义。

#### Scenario: BytetraceBase 无内联样式
- **WHEN** 搜索 `bytetracebase.cpp` 中的 `setStyleSheet` 调用
- **THEN** 不存在硬编码颜色字符串

#### Scenario: SettingsUI 无内联样式
- **WHEN** 搜索 `settingsui.cpp` 中的 `setStyleSheet` 调用
- **THEN** 不存在内联 QSS 字符串，`QListWidget#listWidget` 样式由 `default.qss` 提供

### Requirement: 命名一致性
控件 objectName SHALL 使用语义化命名，遵循 `camelCase` 格式，不得包含无意义后缀（如 `_7`、`_13`）。

#### Scenario: LeftBar 控件命名规范
- **WHEN** 查看 `leftbar.ui`
- **THEN** 所有控件名去掉自动生成的无意义后缀
