## Why

ByteSpace 代码库存在 21 个已确认的缺陷，涵盖功能性 bug（重复信号连接导致多次回调、持锁 emit 可能死锁、无线程安全保护）、代码质量（死代码、重复控件、内联样式泄露）、性能（Hex 转换 N 次堆分配、每秒全量枚举串口）和架构债务（封装破坏、命名混乱）。这些问题在功能叠加之前必须先修复，否则新功能将建立在不可靠的基础上。

## What Changes

**P0 — 功能性 Bug 修复（6 项）**
- 修复 UpdateManager 每次 `checkForUpdate()` 重复 `connect` 导致信号多次触发
- 修复 StateManager `changeState()` 持锁 emit 信号，消除死锁风险
- 为 ErrorHandler 添加 `QMutex` 保护所有状态读写
- 为 BytetraceBase 的 `m_isOpen` 等相关状态添加一致的锁保护
- 打通错误处理链路：BytetraceBase → ErrorHandler 完整接入
- 实现串口参数配置回读：打开串口时从 ConfigManager 恢复上次配置

**P1 — 代码质量 & 性能（8 项）**
- 优化 Hex 转换：使用 `QByteArray::toHex()` 替代循环拼接 QString
- 优化串口监控：端口存在性检查替代每秒全量枚举
- 移除 SettingsUI 内联样式，统一到 default.qss
- 清理 SendWidget 死代码（3 个无连接按钮 + stopSendBtn + 6 个空占位页）
- 清理 LeftBar 重复控件（radioButton_13、recvHexBtn_7）
- 消除 SettingsUI .ui 占位页 / SystemPage 双重构建等代码重复
- 搭建核心逻辑测试框架（至少覆盖 StateManager + ConfigManager）
- 将 `bytetracebase.cpp` 中硬编码颜色移至 QSS

**P2 — 架构改进（7 项，部分推迟）**
- 改进 LeftBar/SendWidget 封装，不暴露内部裸指针
- 统一命名规范（清除 `_7` 等无意义后缀）
- UpdateManager 集成到应用生命周期或移除
- 填充设置页面空白内容或移除空壳
- 添加数据协议抽象层（Modbus / 自定义帧）
- 添加 CI/CD 流水线（GitHub Actions）
- **推迟**：BytetraceBase 大类拆分

## Capabilities

### New Capabilities
- `thread-safety`: 线程安全保护 — 为 ErrorHandler 添加互斥锁，修复 StateManager 持锁 emit 死锁，统一 BytetraceBase 状态访问加锁
- `error-handling`: 错误处理链路 — 打通 SerialPortManager → ErrorHandler → UI 的完整错误传递和展示
- `code-quality`: 代码质量清理 — 移除死代码、重复控件、内联样式，统一命名规范
- `performance`: 性能优化 — Hex 转换优化、串口轮询策略改进
- `config-restore`: 配置恢复 — 打开串口时自动恢复上次保存的参数
- `testing-foundation`: 测试基础设施 — 搭建 Google Test / QtTest 框架，覆盖核心逻辑
- `ui-encapsulation`: UI 封装改进 — LeftBar/SendWidget 接口封装，SettingsUI 清理

### Modified Capabilities
<!-- 无现有 spec 需要修改 -->

## Impact

| 影响范围 | 说明 |
|---------|------|
| `src/updatemanager/` | 修复重复 connect，添加 disconnect 或连接守卫 |
| `src/statemanager/` | 重构 changeState()：先拷贝状态再释放锁后 emit |
| `src/utils/errorhandler.*` | 添加 QMutex + QMutexLocker 保护 |
| `src/framework/bytetracebase.*` | 加锁保护、接入 ErrorHandler、Hex 优化、颜色迁移、配置回读 |
| `src/framework/sendwidget.ui` | 删除死代码按钮和空占位页 |
| `src/framework/leftbar.ui` | 删除重复控件，规范化 objectName |
| `src/framework/settingsui/` | 移除内联样式，清理 .ui 占位页 |
| `src/resources/styles/default.qss` | 新增样式规则替代内联代码 |
| `CMakeLists.txt` | 添加测试子目录 |
| `tests/` (新建) | 测试代码目录 |
