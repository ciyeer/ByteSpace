## ADDED Requirements

### Requirement: Hex 转换性能优化
数据接收的 HEX 转换 SHALL 使用 `QByteArray::toHex()` 替代循环 `QString::arg()` 拼接，将时间复杂度从 O(n²) 降到 O(n)。

#### Scenario: 接收大量数据时的 HEX 转换
- **WHEN** 接收 10KB 以上的串口数据
- **AND** HEX 显示模式开启
- **THEN** HEX 转换在 1ms 内完成（之前循环拼接需要 10ms+）

### Requirement: 串口监控按需轮询
串口可用性检查 SHALL 仅在串口打开时启用定时器，串口关闭时停止轮询。

#### Scenario: 串口关闭时停止轮询
- **WHEN** 串口处于关闭状态
- **THEN** `m_portMonitorTimer` 停止运行
- **AND** 不调用 `QSerialPortInfo::availablePorts()`

#### Scenario: 串口打开时恢复轮询
- **WHEN** 串口成功打开
- **THEN** `m_portMonitorTimer` 恢复运行，检测意外断开
