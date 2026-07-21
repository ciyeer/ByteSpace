## ADDED Requirements

### Requirement: 测试框架搭建
项目 SHALL 集成 QtTest 测试框架，并添加 CMake 测试目标。

#### Scenario: 运行所有测试
- **WHEN** 执行 `cmake --build build && ctest --test-dir build`
- **THEN** 所有已编写的测试用例被执行
- **AND** 测试结果汇总输出到控制台

### Requirement: StateManager 单元测试
StateManager 的核心状态转换逻辑 SHALL 有测试覆盖。

#### Scenario: 合法状态转换
- **WHEN** 从 Idle 转换到 Connecting
- **THEN** changeState 返回 true 且 currentState 变为 Connecting

#### Scenario: 非法状态转换拒绝
- **WHEN** 从 Idle 直接转换到 Disconnecting
- **THEN** changeState 返回 false 且 currentState 保持 Idle

### Requirement: ConfigManager 单元测试
ConfigManager 的读写和事务逻辑 SHALL 有测试覆盖。

#### Scenario: 基本读写
- **WHEN** 设置 portName 为 "COM5"
- **AND** 读取 portName
- **THEN** 返回 "COM5"

#### Scenario: 事务提交
- **WHEN** 开始事务并修改多个配置项
- **AND** 提交事务
- **THEN** 所有修改持久化
