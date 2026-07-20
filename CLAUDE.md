# ByteSpace

串口通信调试工具，基于 Qt6 Widgets 构建的无边框桌面应用。

## 技术栈

- **语言**: C++17
- **UI**: Qt6 (Widgets + QSS 样式表)
- **构建**: CMake 3.16+
- **编译器**: MSVC (Windows) / MinGW / GCC
- **编码**: UTF-8，中文注释
- **Qt 模块**: `Qt6::Widgets`、`Qt6::Network`、`Qt6::SerialPort`、`Qt6::Concurrent`

## 架构模式

### 单例模式（Meyers Singleton）

全项目核心类统一使用 `static ClassName& instance()` 静态局部变量实现单例，禁止拷贝和赋值：

| 类 | 职责 |
|---|------|
| `ConfigManager` | 应用配置，QSettings 持久化 + 内存缓存 + 事务批处理 |
| `StateManager` | 应用状态机，6 个状态 + 合法转换表 + 线程安全 |
| `TaskManager` | 异步任务队列，基于 QThreadPool + 优先级调度 |
| `ErrorHandler` | 统一错误处理，错误消息映射 + 错误级别分类 |
| `LogManager` | 异步日志写入 |

新增全局管理类时遵循此模式。

### 应用状态机

```
Idle ↔ Connecting → Connected ↔ Disconnecting → Idle
  ↓                                       ↓
Error  ←───────────────────────────────── Processing
```

状态转换由 `StateManager::changeState()` 控制，只能在预定义的合法转换表中流转。通过 `stateChanged(AppState, AppState)` 信号通知 UI 更新。

### 异步任务系统

- 所有耗时操作（串口读写、文件 I/O）继承 `BytespaceTask`（同时继承 `QObject` + `QRunnable`）
- `TaskManager` 通过 `QThreadPool` 调度执行，支持优先级和并发控制
- 任务完成/失败通过信号通知主线程（`Qt::QueuedConnection`）

## 编码约定

### 命名规范

- **类名**: PascalCase（如 `SerialPortManager`、`BytetraceBase`）
- **成员变量**: `m_` 前缀，布尔值用 `is`/`has`（如 `m_isOpen`、`m_hasActiveError`）
- **指针成员**: `m_p` 前缀（如 `m_pTitleBar`、`m_pSendWidget`）
- **方法名**: camelCase（如 `handleOpenCloseSerialPort`）
- **常量**: `UPPER_SNAKE_CASE` 或 `static const`（如 `DEFAULT_BAUD_RATE = 115200`）

### 注释规范

- 头文件用 Doxygen 格式：`@file`、`@brief`、`@author`、`@date`
- 公开接口在头文件注释，内部逻辑在 .cpp 注释
- 中文优先（团队母语）

### 日志规范

使用 `LogUtils` 提供的宏，不要直接调用 `LogManager`：

```cpp
LOG_INFO("串口 " + portName + " 打开成功");
LOG_DEBUG("接收数据内容(HEX): " + hexStr);
LOG_WARNING("发送内容为空");
LOG_ERROR("串口打开失败");
```

### UI 规范

- 布局用 `.ui` 文件，样式用 `.qss` 文件（`src/resources/styles/default.qss`）
- 基础样式固定为 `Fusion`，QSS 从 Qt 资源文件加载
- 不要在代码中写内联样式（`setStyleSheet`），统一放到 QSS 文件
- 标题栏 4 个按钮：`btnSPin`（设置）、`btnMin`、`btnMax`、`btnClose`

### 线程安全

- 共享数据用 `QMutex` + `QMutexLocker` 保护
- 跨线程信号槽显式指定 `Qt::QueuedConnection`
- 配置缓存读写加锁，事务操作加锁

### 平台适配

```cpp
#ifdef Q_OS_WIN
    // Windows 特定：无边框窗口任务栏交互、WM_SYSCOMMAND 处理
#endif
```

## 窗口特性

- **无边框**: `Qt::FramelessWindowHint`，自定义标题栏拖拽和双击最大化
- **自定义边框缩放**: `BaseFramework` 处理 8 方向边缘拖拽缩放，`BORDER_WIDTH = 5px`
- **自定义最大化**: 使用 `QScreen::availableGeometry()` 避开 Windows 任务栏
- **最小尺寸**: `200 × 150`
- **窗口几何持久化**: 通过 `ConfigManager` 的 `rememberWindowGeometry` 控制

## 构建

```bash
# 配置
cmake -B build -DCMAKE_BUILD_TYPE=Release

# 构建
cmake --build build --config Release
```

构建信息由 `BuildConfig.h.in` 生成，包含：版本号、Git hash、Git 分支、构建时间、构建编号。

## Superpowers 自动化规则

本项目使用 superpowers 技能体系。AI 助手在处理以下场景时，必须调用对应技能：

| 场景 | 技能 | 说明 |
|------|------|------|
| 新功能 / 修改行为 | `superpowers:brainstorming` | 先理清需求和设计，再写代码 |
| 有设计文档后 | `superpowers:writing-plans` | 生成实现计划 |
| 修 bug / 异常行为 | `superpowers:systematic-debugging` | 先排查根因，不要猜测修复 |
| 写实现代码 | `superpowers:test-driven-development` | 先写测试，再写实现 |
| 声称完成前 | `superpowers:verification-before-completion` | 运行验证，确认通过再声明 |
| 代码审查 | `superpowers:requesting-code-review` | 完成后请求审查 |
| 收到审查意见 | `superpowers:receiving-code-review` | 逐条分析后再改 |
| 多分支开发 | `superpowers:using-git-worktrees` | 为独立功能创建隔离工作空间 |

## 项目结构

```
src/
├── framework/          # 主窗口、标题栏、侧边栏、收发区、设置页
│   └── settingsui/     # 设置子页面：关于、账号、外观、帮助、系统
├── serialportmanager/  # 串口配置器、操作器、管理器（打开/读写/关闭）
├── taskmanager/        # BytespaceTask 基类 + TaskManager + 具体任务子类
│   └── tasks/          # SerialPortOpenTask / ReadTask / WriteTask
├── statemanager/       # 应用状态机（Idle→Connecting→Connected→…）
├── logmanager/         # LogManager（异步写文件）+ LogUtils（静态宏封装）
├── utils/              # ConfigManager、ErrorHandler、StyleManager
├── updatemanager/      # 应用更新管理
├── resources/          # QSS 样式、图片、字体、.qrc 资源文件
│   └── styles/         # default.qss
└── main.cpp            # 入口：初始化配置→样式→主窗口→事件循环
```

## OpenSpec

本项目使用 OpenSpec 管理变更流程。相关命令通过 `/opsx:` 前缀调用：

| 命令 | 用途 |
|------|------|
| `/opsx:explore` | 探索和讨论想法 |
| `/opsx:propose` | 创建变更提案 |
| `/opsx:apply` | 实现任务 |
| `/opsx:archive` | 归档已完成的变更 |
| `/opsx:sync` | 同步 delta spec 到主 spec |
| `/opsx:update` | 更新已有规划文档 |
