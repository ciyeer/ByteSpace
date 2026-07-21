## ADDED Requirements

### Requirement: 接收时间戳
系统 SHALL 支持在接收数据前显示时间戳。

#### Scenario: 文本模式下显示时间戳
- **WHEN** 接收区在文本模式（非 HEX 显示），接收到数据 `Hello`
- **THEN** 系统在接收区追加 `[14:30:25.123] Hello`

#### Scenario: HEX 模式下不显示时间戳
- **WHEN** 接收区在 HEX 显示模式
- **THEN** 系统不追加时间戳前缀，保持纯十六进制数据

### Requirement: 自动换行显示
系统 SHALL 支持接收区自动换行。

#### Scenario: 启用自动换行
- **WHEN** 用户勾选 "自动换行" 复选框
- **THEN** 接收区启用自动换行模式（`QTextBrowser::setLineWrapMode(QTextEdit::WidgetWidth)`）

#### Scenario: 关闭自动换行
- **WHEN** 用户取消勾选 "自动换行"
- **THEN** 接收区关闭自动换行（`QTextEdit::NoWrap`）

### Requirement: 发送新行
系统 SHALL 支持在发送内容末尾自动追加换行符。

#### Scenario: 追加 CR+LF
- **WHEN** 用户勾选 "发送新行" 且选择 `\r\n`，发送文本 `AT`
- **THEN** 系统发送 `AT\r\n`

#### Scenario: 仅追加 LF
- **WHEN** 用户勾选 "发送新行" 且选择 `\n`
- **THEN** 系统发送 `AT\n`

### Requirement: TX/RX 字节计数
系统 SHALL 在状态栏实时显示累计发送和接收字节数。

#### Scenario: 发送数据后 TX 计数更新
- **WHEN** 用户发送 10 字节数据
- **THEN** 状态栏 TX 计数增加 10

#### Scenario: 接收数据后 RX 计数更新
- **WHEN** 系统接收到 5 字节数据
- **THEN** 状态栏 RX 计数增加 5

#### Scenario: 清除接收区时 RX 计数归零
- **WHEN** 用户点击 "清空接收"
- **THEN** RX 计数归零，TX 计数不变
