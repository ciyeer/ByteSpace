# ByteSpace API 文档

## 目录
1. [任务管理系统](#任务管理系统)
2. [串口管理系统](#串口管理系统)
3. [日志管理系统](#日志管理系统)
4. [状态管理系统](#状态管理系统)
5. [配置管理系统](#配置管理系统)

## 任务管理系统

### TaskManager 类
任务管理器，负责管理和调度所有任务。

#### 公共方法
- `static TaskManager& getInstance()`: 获取任务管理器单例
- `void addTask(std::shared_ptr<ByteSpaceTask> task)`: 添加新任务到队列
- `void processNextTask()`: 处理队列中的下一个任务
- `void clearTasks()`: 清空任务队列
- `void setMaxThreads(int nMaxThreads)`: 设置最大线程数
- `int activeThreadCount() const`: 获取当前活动线程数
- `int maxThreadCount() const`: 获取最大线程数

#### 信号
- `void taskCompleted()`: 任务完成信号
- `void taskFailed(QSerialPort::SerialPortError error)`: 任务失败信号

### ByteSpaceTask 类
所有任务的基类。

#### 公共方法
- `virtual bool execute() = 0`: 执行任务（纯虚函数）
- `int getPriority() const`: 获取任务优先级
- `void setPriority(int priority)`: 设置任务优先级

#### 信号
- `void taskCompleted()`: 任务完成信号
- `void taskFailed(QSerialPort::SerialPortError error)`: 任务失败信号

## 串口管理系统

### SerialPortManager 类
串口管理器，负责串口的配置和操作。

#### 公共方法
- `bool initialize(QComboBox* portBox, ...)`: 初始化串口管理器
- `bool openPort()`: 打开串口
- `void closePort()`: 关闭串口
- `bool writeData(const QByteArray& data)`: 写入数据
- `QByteArray readData()`: 读取数据
- `bool isOpen() const`: 检查串口是否打开
- `bool isWritable() const`: 检查串口是否可写
- `bool isReadable() const`: 检查串口是否可读

#### 信号
- `void portOpened()`: 串口打开信号
- `void portClosed()`: 串口关闭信号
- `void dataReceived(const QByteArray& data)`: 数据接收信号
- `void serialPortError(QSerialPort::SerialPortError error)`: 串口错误信号

## 日志管理系统

### LogManager 类
日志管理器，负责系统日志的记录和管理。

#### 公共方法
- `static LogManager& instance()`: 获取日志管理器单例
- `void logMessage(const QString& message, LOG::LogLevel level)`: 记录日志消息
- `void setLogLevel(LOG::LogLevel level)`: 设置日志级别
- `void enableFileLogging(bool enable)`: 启用/禁用文件日志

## 状态管理系统

### StateManager 类
状态管理器，负责管理应用程序的状态。

#### 公共方法
- `static StateManager& instance()`: 获取状态管理器单例
- `void setState(State newState)`: 设置新状态
- `State getState() const`: 获取当前状态
- `bool canTransitionTo(State newState) const`: 检查是否可以转换到新状态

#### 信号
- `void stateChanged(State newState)`: 状态改变信号
- `void stateError(const QString& error)`: 状态错误信号

## 配置管理系统

### ConfigManager 类
配置管理器，负责管理应用程序配置。

#### 公共方法
- `static ConfigManager& instance()`: 获取配置管理器单例
- `void saveConfig()`: 保存配置
- `void loadConfig()`: 加载配置
- `void resetToDefault()`: 重置为默认配置
- `void beginTransaction()`: 开始配置事务
- `void commitTransaction()`: 提交配置事务
- `void rollbackTransaction()`: 回滚配置事务

#### 信号
- `void configChanged()`: 配置改变信号
- `void configError(const QString& error)`: 配置错误信号

## 使用示例

### 任务管理示例
```cpp
// 创建并执行串口写任务
auto writeTask = std::make_shared<SerialPortWriteTask>(serialPortManager, data);
TaskManager::instance().addTask(writeTask);