## ADDED Requirements

### Requirement: ErrorHandler 线程安全
ErrorHandler 的所有公开方法 SHALL 使用 QMutex 保护内部状态，确保多线程并发调用时数据不竞争。

#### Scenario: 多线程同时报告错误
- **WHEN** 线程 A 调用 `handleError()` 且线程 B 同时调用 `handleSerialPortError()`
- **THEN** 两个调用的状态写入互不干扰，最后读取 `getLastError()` 返回完整的错误信息

#### Scenario: 清除错误与报告错误并发
- **WHEN** 线程 A 调用 `clearErrors()` 同时线程 B 调用 `handleError()`
- **THEN** 不会出现 `m_hasActiveError` 和 `m_lastErrorMessage` 不一致的状态

### Requirement: StateManager 无死锁 emit
StateManager::changeState() SHALL 在释放互斥锁后 emit stateChanged 信号，消除同线程直连槽回调导致的死锁风险。

#### Scenario: 槽函数回调 StateManager
- **WHEN** `changeState()` 发出 `stateChanged` 信号
- **AND** 连接的槽函数（同线程直连）调用 `currentState()` 或 `isInState()`
- **THEN** 槽函数成功获取锁并返回，不发生死锁

### Requirement: BytetraceBase 状态访问加锁
BytetraceBase 的 `m_isOpen` 和 `m_serialPortManager` 状态 SHALL 在跨线程访问时使用一致的互斥锁保护。

#### Scenario: 定时器回调与用户操作并发
- **WHEN** `onTimeout()` 正在检查 `m_isOpen`
- **AND** 用户同时点击打开/关闭按钮触发 `handleOpenCloseSerialPort()`
- **THEN** 两次访问不会产生竞态条件

### Requirement: UpdateManager 不重复连接信号
UpdateManager::checkForUpdate() SHALL 每次被调用时只建立一次信号连接，多次调用不会累积重复回调。

#### Scenario: 多次检查更新
- **WHEN** 用户连续两次调用 `checkForUpdate()`
- **THEN** `onUpdateCheckFinished` 仅被调用一次
