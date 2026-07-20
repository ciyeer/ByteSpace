## Context

ByteSpace 是一个 Qt6 C++ 串口通信工具，当前架构分层清晰（Manager → Utility → UI），但存在线程安全、错误处理链路、代码质量等多方面债务。本次是修复性变更，不引入新外部依赖，遵循现有代码模式。

## Goals / Non-Goals

**Goals:**
- 消除所有已确认的功能性 bug（重复连接、死锁风险、竞态条件）
- 统一错误处理链路，打通 SerialPortManager → ErrorHandler → UI
- 清理死代码、重复代码、内联样式，提升可维护性
- 优化 Hex 转换和串口轮询两个性能热点
- 搭建测试基础设施

**Non-Goals:**
- 不拆分 BytetraceBase（推迟到后续变更）
- 不引入新的外部库或框架
- 不改变现有功能行为（仅修复 bug，不新增功能）
- 不处理数据协议抽象层（留给后续 feature change）

## Decisions

### D1: StateManager 死锁修复 — 解锁后 emit

**选择**: 在 `changeState()` 中将 emit 移出锁作用域

```cpp
// Before (有风险):
{QMutexLocker locker(&m_mutex); emit stateChanged(old, new);}

// After (安全):
AppState oldState;
{QMutexLocker locker(&m_mutex); oldState = m_currentState; m_currentState = new;}
emit stateChanged(oldState, newState);
```

**备选**: `QMetaObject::invokeMethod(Qt::QueuedConnection)` 延迟 emit。不采用——改动更大，且同线程直接 emit 在解锁后已安全。

### D2: ErrorHandler 线程安全 — QMutex 保护

**选择**: 参考 StateManager 模式，添加 `mutable QMutex m_mutex` + `QMutexLocker`

所有公开方法（`handleError`、`handleSerialPortError`、`clearErrors`、`getLastError`、`hasActiveError`）在函数入口加 `QMutexLocker`。

### D3: UpdateManager 重复连接 — 连接守卫

**选择**: 在 `checkForUpdate()` 的 connect 前先 `disconnect`，或使用 `Qt::UniqueConnection`

```cpp
// 方案A（推荐）：
disconnect(networkManager, &QNetworkAccessManager::finished, this, nullptr);
connect(networkManager, &QNetworkAccessManager::finished, this, &UpdateManager::onUpdateCheckFinished);

// 方案B：单次连接直接用 reply 的信号（参考 startDownload 的写法）
```

**推荐方案B**: 将 `checkForUpdate()` 改为连接 reply 的信号而非 networkManager，避免与多次请求冲突。

### D4: Hex 转换 — QByteArray::toHex()

**选择**: 用 `QByteArray::toHex(' ')` 替代循环拼接 QString

```cpp
// Before: O(n) 次堆分配
for (int i = 0; i < rawData.size(); ++i) {
    hexData += QString("%1 ").arg(static_cast<unsigned char>(rawData[i]), 2, 16, QChar('0'));
}

// After: 单次分配
hexData = QString::fromLatin1(rawData.toHex(' '));
```

**注意**: `toHex(' ')` 来自 Qt 5.14+，本项目用 Qt6，完全可用。

### D5: 串口轮询 — 事件驱动替代定时器

**选择**: 串口已打开时保持定时器（检查意外断开），串口关闭时停止定时器

```cpp
// updateUI(false) 时：
m_portMonitorTimer->stop();

// updateUI(true) 时：
m_portMonitorTimer->start();
```

备选：完全依赖 QSerialPort::errorOccurred 信号。不采用——ResourceError 在某些场景下不会触发信号，定时器作为兜底是合理的。

### D6: 配置回读 — initialize 时从 ConfigManager 恢复

**选择**: `SerialPortConfigurator::initialize()` 完成后，从 ConfigManager 读取上次保存的参数覆盖 ComboBox 当前值。

```cpp
// 在 initialize 填充完 ComboBox 后：
if (ConfigManager::instance().portName() != DEFAULT_PORT_NAME) {
    // 设置 ComboBox 到上次保存的值
}
```

### D7: 测试框架 — QtTest

**选择**: 使用 Qt 自带的 `QTestLib`，无需额外依赖。

## Risks / Trade-offs

| 风险 | 缓解 |
|------|------|
| StateManager emit 重构可能改变信号时序 | 保持同线程直连，解锁后立即 emit，时序不变 |
| ErrorHandler 加锁引入性能开销 | ErrorHandler 调用频率极低（仅在错误时），无影响 |
| 串口轮询改为条件启动可能漏检断开 | 保留 errorOccurred 信号处理 + 打开时定时器双保险 |
| 配置回读可能与 ComboBox 初始化顺序冲突 | 在 initialize() 填充完默认项后再覆盖，避免空指针 |
| 移除 SendWidget 按钮可能破坏 .ui 文件布局 | 逐项删除并验证编译，保留 horizontalLayout 结构 |
