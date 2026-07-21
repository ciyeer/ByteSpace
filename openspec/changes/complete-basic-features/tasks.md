## 1. HEX 发送

- [x] 1.1 在 `BytetraceBase` 中实现 HEX 字符串解析函数 `parseHexString(const QString&) → QByteArray`
- [x] 1.2 修改 `onSendButtonClicked()`：当 `hexSendBtn` 勾选时走 HEX 解析路径
- [x] 1.3 错误处理：奇数长度 / 非法字符时弹出 `QMessageBox::warning`
- [x] 1.4 在 `LeftBar` 中新增 `isHexSend()` 语义化 getter

## 2. 定时发送

- [x] 2.1 在 `BytetraceBase` 中添加 `m_timedSendTimer`（QTimer），默认间隔 1000ms
- [x] 2.2 实现定时发送启动/停止逻辑：勾选+串口打开时启动，取消或关闭串口时停止
- [x] 2.3 间隔下限保护（最小 10ms）
- [x] 2.4 定时发送与 HEX 模式 / CRC / 发送新行配合工作

## 3. 发送新行

- [x] 3.1 在 `BytetraceBase` 中实现追加换行符逻辑（`\r\n` / `\r` / `\n`）
- [x] 3.2 处理换行符与 CRC 的追加顺序（数据 → CRC → 换行符）

## 4. CRC 校验

- [x] 4.1 在 `BytetraceBase` 中实现 CRC-16/MODBUS 查表法（多项式 0x8005，初始值 0xFFFF）
- [x] 4.2 修改 `sendData()`：勾选 CRC 时计算校验值并追加（小端序）
- [x] 4.3 在 `LeftBar` 中新增 `isCrcEnabled()` 语义化 getter

## 5. 文件收发

- [x] 5.1 实现文件发送：`QFileDialog` 选择文件 → 分块（256B）读取 → 通过 `writeData()` 发送
- [x] 5.2 大文件（>1MB）警告对话框
- [x] 5.3 实现接收日志存文件：`QFileDialog` 选择保存路径 → `QFile::Append` 写入 → 实时 `flush()`
- [x] 5.4 串口关闭时自动停止文件发送和日志写入

## 6. DTR/RTS 控制

- [x] 6.1 在 `SerialPortManager` 中新增 `setDTR(bool)` 和 `setRTS(bool)` 方法
- [x] 6.2 在 `BytetraceBase` 中连接 DTR/RTS 复选框 → 即时调用串口 API
- [x] 6.3 在 `LeftBar` 中新增 `setDtrEnabled(bool)` / `setRtsEnabled(bool)` UI 绑定

## 7. 波特率扩展

- [x] 7.1 扩展 `SerialPortConfigurator::initialize()` 中波特率列表（1200~921600）
- [x] 7.2 将波特率 ComboBox 设为 `setEditable(true)`，支持自定义输入
- [x] 7.3 打开串口时验证波特率值有效性

## 8. 接收增强（时间戳 / 自动换行 / TX-RX 计数）

- [x] 8.1 实现接收时间戳前缀 `[HH:mm:ss.zzz]`（仅文本模式，HEX 模式不追加）
- [x] 8.2 实现自动换行切换：`QTextBrowser::setLineWrapMode()`
- [x] 8.3 在 `StatusBar` 中实现 TX/RX 字节计数实时更新
- [x] 8.4 在 `BytetraceBase` 中添加 `m_txBytes` / `m_rxBytes` 计数器
- [x] 8.5 "清空接收" 时 RX 计数归零

## 9. LeftBar 新控件封装

- [x] 9.1 补充 LeftBar 语义化 getter：`isHexSend()`、`isCrcEnabled()`、`isTimedSend()`、`timedSendInterval()`、`isDtrEnabled()`、`isRtsEnabled()`、`isAutoNewline()`、`isSendNewline()`、`isFileTransfer()`、`isSaveToFile()`
- [x] 9.2 DTR/RTS 复选框状态与串口打开状态联动（串口关闭时禁用）
