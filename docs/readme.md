# ByteSpace串口调试工具

## 1. 项目概述
- **项目名称**: 基于 Qt6 的ByteSpace串口调试工具
- **开发者**: ciyeer
- **开发工具**: Qt Creator
- **目标平台**: Windows, Linux, macOS
- **版本控制**: Git
- **联系邮箱**: ciyeer@outlook.com

## 2. 版本控制策略

### 2.1 版本号规则
版本号采用 "主版本号.次版本号.修订号" 的格式，如 "1.0.0"。
- **主版本号**: 当进行不向后兼容的 API 修改时增加
- **次版本号**: 当增加功能时，且保持向后兼容时增加
- **修订号**: 当进行向后兼容的问题修正时增加

### 2.2 分支策略
- **主分支(master)**: 稳定版本，随时可用于发布
- **开发分支(develop)**: 新功能的开发和测试，完成后合并回主分支
- **特性分支(feature/特性名)**: 用于开发新功能，完成后合并到开发分支
- **修复分支(hotfix/修复名)**: 用于快速修复生产环境中的问题，完成后合并回主分支和开发分支

## 3. 功能特性
### 3.1 核心功能
- **串口通信**: 波特率、数据位、停止位、校验位、流控制配置；自动串口检测与热插拔；CRC 校验
- **数据收发**: 支持 ASCII 和 Hex 格式；日志记录和可视化展示；实时数据监控

### 3.2 高级特性
- **性能优化**: 多线程数据处理，自动错误恢复，在线更新
- **用户体验**: 配置管理，操作日志，错误提示与处理

### 3.3 用户界面
- 支持托盘功能、主题切换、语言切换、字体设置、快捷键支持

## 4. 使用说明

### 4.1 基本操作
- **串口配置**: 选择串口号，设置通信参数，点击"打开串口"开始通信
- **数据收发**: ASCII/Hex 格式切换，实时数据显示，支持 CRC 校验（可选）
- **日志管理**: 支持日志记录和导出，日志级别过滤功能

### 4.2 高级功能
- **自动重连**: 串口断开自动重试，支持智能退避算法
- **数据分析**: 数据统计、错误检测、性能监控

## 5. 技术实现
### 5.1 UI线程与串口操作的异步性
1. **UI响应性优化**
   - 在串口通信中，特别是发送大量数据时，避免UI线程阻塞
   - 使用异步处理和线程池确保界面流畅性
   - 使用`QFutureWatcher`结合`QtConcurrent::run`处理数据
2. **串口配置管理**
   - 将串口参数配置封装到专门的配置类
   - 提供UI配置接口
   - 支持动态调整串口参数
3. **数据接收与显示优化**
   - 使用缓冲区积累数据
   - 定时更新UI显示

### 5.2 日志系统实现
- **异步处理**: 使用 `LogManager` 类异步处理日志，保证多线程环境下的线程安全,防止主线程阻塞
- **日志分级**
  - DEBUG级别：调试信息
  - INFO级别：普通信息记录
  - WARNING级别：警告信息
  - ERROR级别：错误信息
  - CRITICAL级别：严重错误信息
- **日志持久化**: 支持文件日志记录，自动滚动和按级别过滤显示

## 6. 环境构建
### 6.1 构建环境
- Qt 6.x
- CMake 3.x
- C++17 或更高版本

### 6.2 项目结构
```
ByteSpace/
├── src/
│   ├── main.cpp                      
│   ├── configmanager/                      # 配置管理模块
│   │   ├── configmanager.cpp
│   │   └── configmanager.h
│   ├── framework/                          # 框架核心模块
│   │   ├── baseframework.cpp               # 基础框架实现
│   │   ├── baseframework.h
│   │   ├── framework.cpp                   # 主框架实现
│   │   ├── framework.h
│   │   ├── framework.ui
│   │   ├── statusbar.cpp                   # 状态栏实现
│   │   ├── statusbar.h
│   │   ├── statusbar.ui
│   │   ├── bytespace.cpp                   # 主窗口实现
│   │   ├── bytespace.h
│   │   ├── bytespace.ui
│   │   ├── titlebar.cpp                    # 标题栏实现
│   │   ├── titlebar.h
│   │   └── titlebar.ui
│   ├── logmanager/                         # 日志管理模块
│   │   ├── logmanager.cpp
│   │   ├── logmanager.h
│   │   └── logutils.h
│   ├── serialportmanager/                  # 串口管理模块
│   │   ├── serialportconfigurator.cpp
│   │   ├── serialportconfigurator.h
│   │   ├── serialportmanager.cpp
│   │   └── serialportmanager.h
│   ├── statemanager/                       # 状态管理模块
│   │   ├── statemanager.cpp                # 状态管理实现
│   │   └── statemanager.h                  # 状态管理接口
│   ├── taskmanager/                        # 任务管理模块
│   │   ├── bytespacetask.cpp               # 基础任务实现
│   │   ├── bytespacetask.h                 # 基础任务接口
│   │   ├── serialporttask.cpp              # 串口任务实现
│   │   ├── serialporttask.h                # 串口任务接口
│   │   ├── serialportwritetask.cpp         # 串口写任务实现
│   │   └── serialportwritetask.h           # 串口写任务接口
│   └── updatemanager/                      # 更新管理模块
│       ├── updatemanager.cpp               # 更新管理实现
│       └── updatemanager.h                 # 更新管理接口
├── docs/                                   # 文档目录
│   └── readme.md
└── tests/                                  # 测试用例目录
```

### 6.3 CMake 版本管理
1. **在 C++ 代码中使用版本号**
```cmake
# 创建版本信息的头文件
configure_file(
    "${PROJECT_SOURCE_DIR}/version.h.in"
    "${PROJECT_BINARY_DIR}/version.h"
)
```
2. **版本信息模板**
```cpp
// version.h.in
#define BYTESPACE_VERSION_MAJOR @PROJECT_VERSION_MAJOR@
#define BYTESPACE_VERSION_MINOR @PROJECT_VERSION_MINOR@
#define BYTESPACE_VERSION_PATCH @PROJECT_VERSION_PATCH@
#define BYTESPACE_VERSION "@PROJECT_VERSION@"
```

## 7. 改进和优化
### 7.1 内存与性能优化
- 动态调整缓冲区大小，避免频繁内存分配
- 使用内存池技术，提升性能
- 批量处理和零拷贝技术应用

### 7.2 多线程优化
- 使用线程池管理串口操作、数据处理和日志处理
- 使用读写锁和条件变量实现高效并发控制

### 7.3 UI响应优化
- 使用信号槽机制和延迟加载技术异步更新，避免 UI 阻塞
- 分页加载和局部更新优化数据展示

### 7.4 错误处理和恢复机制
- **错误分类：**设备未找到、权限错误、通信超时等
- **恢复机制：** 自动重试、错误日志记录、用户提示
- **资源管理：**内存使用优化，串口资源释放，异常安全保证

### 7.5 数据完整性验证
- **校验和验证**：通过简单的 CRC16 或 MD5 校验算法来验证接收数据的完整性。
- **错误重传机制**：如果数据校验失败，自动请求发送方重新发送数据。使用指数退避策略,可配置的最大重试次数,重试时间间隔动态调整。

### 7.6 串口状态监控与自动恢复
- **用户控制状态监控**：增加复选框让用户选择是否启用自动监控和自动恢复功能。
- **定时器调整**：根据用户需求设置自动重试的时间间隔。定时检查串口状态(不同平台下采用不同策略)，状态变化通知。

## 8. 未来计划
### 8.1 计划新增功能
- 实时数据流监控
- 用户自定义脚本支持
- 数据图形化展示
- CRC校验功能
- 在线更新功能
- 协议解析器
- 单元测试 : 建议为关键组件添加单元测试，特别是 SerialPortManager 和 TaskManager 。
- 国际化支持 : 考虑使用 Qt 的翻译机制 (QTranslator) 添加多语言支持。
- 代码文档 : 为公共 API 添加更详细的文档注释。
- 性能分析 : 使用 Qt Creator 的性能分析工具检测潜在的性能瓶颈。
- 内存泄漏检测 : 使用工具如 Valgrind 检测潜在的内存泄漏。
- 异步操作 : 对于耗时操作，考虑使用 Qt 的 QFuture 和 QtConcurrent 进行异步处理。

### 8.2 用户体验优化
- 托盘功能、主题切换、快捷键支持
- 开机启动、固定桌面最前端

### 8.3 系统功能
- 恢复默认设置、帮助文档、关于信息

