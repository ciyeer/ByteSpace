## 1. P0 — 功能性 Bug 修复

### 1.1 UpdateManager 重复连接 (#1)
- [x] 1.1.1 将 `checkForUpdate()` 中的 connect 改为连接 reply 的 finished 信号（非 networkManager），或添加 disconnect 守卫
- [x] 1.1.2 验证多次调用 `checkForUpdate()` 后 `onUpdateCheckFinished` 只触发一次

### 1.2 StateManager 持锁 emit 死锁修复 (#2)
- [x] 1.2.1 重构 `changeState()`：在锁作用域内拷贝状态值，释放锁后再 emit
- [x] 1.2.2 验证同线程直连槽回调 StateManager 方法不触发死锁

### 1.3 ErrorHandler 线程安全 (#3)
- [x] 1.3.1 为 ErrorHandler 添加 `mutable QMutex m_mutex` 成员
- [x] 1.3.2 在 `handleError()`、`handleSerialPortError()`、`clearErrors()`、`getLastError()`、`hasActiveError()` 中添加 `QMutexLocker`
- [x] 1.3.3 验证多线程并发调用无数据竞争

### 1.4 BytetraceBase 状态访问加锁 (#4)
- [x] 1.4.1 审查所有 `m_isOpen` 和 `m_serialPortManager` 的读写点
- [x] 1.4.2 在 `handleOpenCloseSerialPort()`、`onTimeout()`、`onTaskCompleted()` 中为关键状态访问添加 `QMutexLocker`

### 1.5 错误处理链路打通 (#5)
- [x] 1.5.1 在 `BytetraceBase::onSerialPortError()` 中处理所有错误类型（WriteError、ReadError、OpenError 等），不再仅处理 ResourceError
- [x] 1.5.2 在 `onSerialPortError()` 中调用 `ErrorHandler::instance().handleSerialPortError(error)`
- [x] 1.5.3 在 StatusBar 或合适位置展示错误信息（连接 `ErrorHandler::errorOccurred` 信号）

### 1.6 串口参数配置回读 (#6)
- [x] 1.6.1 在 `SerialPortConfigurator::initialize()` 填充完 ComboBox 后，从 ConfigManager 读取上次保存的参数
- [x] 1.6.2 如果上次保存的端口名仍可用则选中，否则选中第一个可用端口
- [x] 1.6.3 验证重启应用后串口参数自动恢复

## 2. P1 — 性能优化

### 2.1 Hex 转换优化 (#7)
- [ ] 2.1.1 将 `BytetraceBase::onSerialDataReceived()` 中的循环 `QString::arg()` 拼接替换为 `QByteArray::toHex(' ')`
- [ ] 2.1.2 同样优化发送端的 HEX 转字节逻辑（如存在类似问题）

### 2.2 串口轮询优化 (#8)
- [ ] 2.2.1 在 `updateUI(false)` 中调用 `m_portMonitorTimer->stop()`
- [ ] 2.2.2 在 `updateUI(true)` 中调用 `m_portMonitorTimer->start()`
- [ ] 2.2.3 保留 `errorOccurred` 信号处理作为双重保险

## 3. P1 — 代码质量清理

### 3.1 内联样式迁移 (#9, #10)
- [ ] 3.1.1 移除 `bytetracebase.cpp` 中 `setStyleSheet("background-color: #FF6B6B")` 和 `#4CAF50`
- [ ] 3.1.2 移除 `settingsui.cpp` 中的 `setStyleSheet` 调用
- [ ] 3.1.3 在 `default.qss` 中添加对应规则（使用 objectName 或动态属性选择器）

### 3.2 SendWidget 死代码清理 (#11)
- [ ] 3.2.1 从 `sendwidget.ui` 中删除 pushButton（单条发送）、pushButton_2（多条发送）、pushButton_3（协议传输）
- [ ] 3.2.2 从 `sendwidget.ui` 中删除 stopSendBtn
- [ ] 3.2.3 从 `sendwidget.ui` 中删除 page_2 到 page_7 空占位页
- [ ] 3.2.4 验证编译通过且界面布局正常

### 3.3 LeftBar 重复控件清理 (#12)
- [ ] 3.3.1 从 `leftbar.ui` 中删除 radioButton_13（重复的 HEX发送）和 recvHexBtn_7（重复的 HEX显示）
- [ ] 3.3.2 保留的控件重命名为语义化名称（`hexSendBtn`、`hexDisplayBtn` 等）
- [ ] 3.3.3 更新 `leftbar.h` 中的 getter 方法名（如 `recvHexBtn()` → `hexDisplayBtn()`）
- [ ] 3.3.4 更新 `bytetracebase.cpp` 中所有引用

### 3.4 SettingsUI / SystemPage 代码重复清理 (#13, #14)
- [ ] 3.4.1 移除 `settingsui.ui` 中 stackedWidget 的 5 个空白占位页
- [ ] 3.4.2 移除 `settingsui.ui` 中未连接的 btnReturn 按钮
- [ ] 3.4.3 重构 `SystemPage`：统一使用 .ui 文件或纯代码构建，移除双重构建

### 3.5 SerialPortConfigurator 重复映射修复 (#15)
- [ ] 3.5.1 `configureFromSettings()` 中的 dataBits switch 改为复用 `mapDataBits()`
- [ ] 3.5.2 `configureFromSettings()` 中的 stopBits switch 改为复用 `mapStopBits()`
- [ ] 3.5.3 `configureFromSettings()` 中的 parity if-else 链改为复用 `mapParity()`
- [ ] 3.5.4 验证配置读写一致性

### 3.6 SerialPortOperator 重复守卫提取 (#16)
- [ ] 3.6.1 提取 `validatePort(QSerialPort*, QIODevice::OpenMode)` 私有方法，消除 write/read 中的重复 null-check + isOpen 守卫

### 3.7 命名一致性 (#17)
- [ ] 3.7.1 重命名 LeftBar 中所有带自动生成后缀的控件（`_7`、`_13` 等）
- [ ] 3.7.2 检查全项目成员变量命名：统一 `m_` 前缀 + `m_p` 指针前缀
- [ ] 3.7.3 修复 `trayIcon` 等缺少前缀的变量（`updatemanager.h`）

## 4. P1 — 测试基础设施

### 4.1 测试框架搭建 (#18)
- [ ] 4.1.1 创建 `tests/` 目录和 `tests/CMakeLists.txt`
- [ ] 4.1.2 在顶层 `CMakeLists.txt` 中添加 `add_subdirectory(tests)`（可选开关）
- [ ] 4.1.3 编写 StateManager 状态转换测试用例
- [ ] 4.1.4 编写 ConfigManager 读写和事务测试用例

## 5. P2 — 架构改进

### 5.1 LeftBar 封装改进 (#19)
- [ ] 5.1.1 将 LeftBar 的裸指针 getter 改为信号+方法接口
- [ ] 5.1.2 添加 `setPortOpen(bool)`、`portName()`、`baudRate()` 等语义化方法
- [ ] 5.1.3 更新 SerialPortManager::initialize() 使用新接口

### 5.2 SerialPortManager 委托简化 (#20)
- [ ] 5.2.1 评估是否可以移除纯委托方法，直接通过 `configurator()` / `operator()` 访问器暴露
- [ ] 5.2.2 保留有业务逻辑的方法（如 `openPort()` 有 emit），移除无逻辑的纯委托

### 5.3 UpdateManager 生命周期 (#21)
- [ ] 5.3.1 在 `main.cpp` 中创建 UpdateManager 实例或确认其不需要、删除空壳

### 5.4 设置页面内容填充或移除
- [ ] 5.4.1 评估 HelpPage、AccountPage、AboutPage 是否需要保留
- [ ] 5.4.2 保留则填充内容，不需要则删除文件和 SettingsUI 中的引用
