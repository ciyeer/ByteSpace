## Why

ByteSpace 左栏和发送区有 15+ 个 UI 控件（HEX发送、定时发送、文件收发、DTR/RTS、CRC校验等）以按钮/复选框形式存在，但从未连接到任何功能逻辑——用户点击后无任何反应。同时，波特率仅提供 5 个选项，缺少接收时间戳、TX/RX 计数等同类软件标配功能。本次变更将这些"UI 空壳"逐一实现，让 ByteSpace 从"能用"升级为"好用"。

## What Changes

**发送增强**
- HEX 发送模式：将 `sendArea` 中的十六进制字符串解析为字节发送（替代当前仅 UTF-8 文本发送）
- 定时发送：按用户设定的毫秒间隔自动重复发送
- 发送新行：发送内容末尾自动追加 `\r\n`、`\r` 或 `\n`
- 文件发送：从本地文件读取内容并发送

**接收增强**
- 接收时间戳：每条数据前显示 `[HH:mm:ss.zzz]` 时间标记
- 自动换行显示：接收内容按固定宽度或自动折行
- 日志存文件：将接收内容实时写入本地日志文件
- TX/RX 字节计数：状态栏实时显示累计发送/接收字节数

**串口控制增强**
- DTR/RTS 控制：通过勾选框手动控制硬件流控引脚
- CRC 校验：支持 CRC-16/Modbus 等常用校验，发送时自动追加
- 波特率扩展：从 5 个选项扩展到覆盖 1200 ~ 921600 的常用速率

## Capabilities

### New Capabilities
- `hex-send`: HEX 发送模式 — 解析十六进制字符串并发送原始字节
- `timed-send`: 定时发送 — 按配置的时间间隔自动循环发送
- `file-transfer`: 文件收发 — 从文件发送数据，将接收数据保存到文件
- `dtr-rts-control`: DTR/RTS 引脚控制 — 手动切换硬件流控信号线
- `crc-checksum`: CRC 校验 — 计算校验和并追加到发送数据
- `receive-enhancements`: 接收增强 — 时间戳、自动换行、日志存文件、TX/RX 计数
- `baudrate-expansion`: 波特率扩展 — 覆盖 1200 ~ 921600 常用速率 + 自定义输入

### Modified Capabilities
<!-- 无现有 spec 需要修改 -->

## Impact

| 影响范围 | 说明 |
|---------|------|
| `src/framework/bytetracebase.cpp` | 重写 `onSendButtonClicked()` 支持 HEX 模式；新增定时发送定时器；实现 DTR/RTS 切换；接收数据处理（时间戳+计数+日志） |
| `src/framework/bytetracebase.h` | 新增定时发送 QTimer、收发计数器、CRC 计算器等成员 |
| `src/framework/sendwidget.cpp` | 新增 `sendAsHex()`、`sendFromFile()` 接口 |
| `src/framework/recvwidget.cpp` | 新增时间戳显示、自动换行、日志写入接口 |
| `src/framework/statusbar.cpp` | 实现 TX/RX 计数器实时更新 |
| `src/serialportmanager/` | 新增 `setDTR()`、`setRTS()` 控制方法 |
| `src/serialportmanager/serialportconfigurator.cpp` | 扩展波特率选项列表 |
| `src/framework/leftbar.cpp` | 新增 CRC、DTR/RTS、定时发送等新控件的封装方法 |
| `src/resources/styles/default.qss` | 新增 CRC/时间戳相关按钮样式 |
