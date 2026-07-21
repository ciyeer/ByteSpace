## Context

ByteSpace 现有架构分层清晰（Framework → SerialPortManager → Configurator/Operator），LeftBar 刚刚完成封装（`codebase-hardening` P2），暴露了语义化 getter。当前 15+ 个 UI 控件是空壳——本轮变更将它们全部接入功能。

约束：不引入新外部依赖，遵循现有模式（Meyers 单例、状态机、异步任务），代码风格同现有。

## Goals / Non-Goals

**Goals:**
- 实现左栏所有现有 UI 控件的后端逻辑（HEX发送、定时发送、DTR/RTS、CRC、文件收发、自动换行、发送新行）
- 接收区增加时间戳、TX/RX 计数、日志存文件
- 扩展波特率选项覆盖常用速率
- 所有新增功能通过 LeftBar 封装后的语义化 getter 访问

**Non-Goals:**
- 不新增 UI 控件（只接已有控件）
- 不引入新的协议解析（Modbus 等留给后续）
- 不改变现有架构分层
- 不添加数据绘图/波形功能
- 不添加 TCP/UDP 网络功能

## Decisions

### D1: HEX 发送 — 严格解析，非法字符报错

**选择**: 接收 `sendArea` 文本，去除空格/换行后按两字符一组解析。非法字符弹出 `QMessageBox::warning`，不发送。

```cpp
// 输入 "48 65 6C 6C 6F" → QByteArray {0x48, 0x65, 0x6C, 0x6C, 0x6F}
// 输入 "48 65 6C 6C 6" → 报错"奇数长度的十六进制字符串"
// 输入 "48 XX" → 报错"无效的十六进制字符: XX"
```

**备选**: 宽松解析（跳过非法字符）。不采用——硬件调试场景需要知道自己拼错了。

### D2: 定时发送 — 独立 QTimer，不阻塞主线程

**选择**: `m_timedSendTimer`（BytetraceBase 成员），默认 1000ms 间隔。勾选"定时发送"且串口已打开时启动，取消勾选或关闭串口时停止。

**备选**: 使用 TaskManager 的线程池。不采用——定时发送是 UI 驱动的周期性操作，用 QTimer 更简单直接。

### D3: CRC 校验 — 内置查表法实现 CRC-16/MODBUS

**选择**: 在 BytetraceBase 中实现 CRC-16/MODBUS（多项式 0x8005，初始值 0xFFFF），不依赖外部库。勾选 CRC 时，发送前自动计算并追加 2 字节校验值（小端序）。

**备选**: 使用 Qt 内置的 `qChecksum()`。不采用——这是 ISO 3309 CRC，不是嵌入式常用的 Modbus CRC。

### D4: DTR/RTS — 直接调用 QSerialPort API

**选择**: 通过 `QSerialPort::setDataTerminalReady()` 和 `QSerialPort::setRequestToSend()` 切换引脚状态。每次勾选/取消时即时生效。

**备选**: 在打开串口时设置。不采用——调试场景需要运行时动态切换。

### D5: 文件发送 — 分块读取+进度反馈

**选择**: 用户选择文件后，分块（每块 256 字节）读取并通过现有 `writeData()` 发送。大文件（>1MB）警告用户。

**备选**: 内存映射。不采用——串口速率慢，分块发送足够。

### D6: 接收时间戳 — 前缀格式 `[HH:mm:ss.zzz]`

**选择**: 每条 `readyRead` 数据前追加 ISO 风格时间戳前缀，格式 `[HH:mm:ss.zzz] `。仅文本模式下追加（HEX 模式下不加，避免破坏十六进制格式）。

**备选**: 参考 RealTerm 的 ISO8601/UTC 格式。不采用——本地时间对调试更友好。

### D7: 日志存文件 — QFile + QTextStream 追加写入

**选择**: 用户选择保存路径后，接收数据实时追加写入。使用 `QFile::Append` + `QTextStream`，每条数据写入后立即 `flush()`。

### D8: 波特率扩展 — 列表+可编辑 ComboBox

**选择**: 将波特率 ComboBox 设为 `setEditable(true)`，提供预设列表（1200/2400/4800/9600/14400/19200/38400/57600/115200/230400/460800/921600），同时允许用户手动输入自定义值。

## Risks / Trade-offs

| 风险 | 缓解 |
|------|------|
| 定时发送+大量数据可能阻塞 UI | 发送本身阻塞时间极短（串口 FIFO），定时器间隔下限设 10ms |
| CRC 模式与 HEX 模式同时勾选时行为复杂 | 明确顺序：先 HEX 解析 → 再 CRC 追加 → 再发送新行 |
| 日志文件持续写入可能影响性能 | 使用 `QFile::flush()` 而非每字节 `fsync`；用户可随时关闭日志 |
| 自定义波特率可能输入无效值 | 打开串口时验证，无效则拒绝并提示 |
