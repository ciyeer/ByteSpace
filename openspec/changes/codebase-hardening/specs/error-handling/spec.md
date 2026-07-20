## ADDED Requirements

### Requirement: 完整的串口错误处理链路
当 SerialPortManager 检测到串口错误时，SHALL 通过 ErrorHandler 统一处理，并通知 UI 层展示给用户。

#### Scenario: 串口写入错误
- **WHEN** 串口写入失败（QSerialPort::WriteError）
- **THEN** SerialPortManager 关闭串口并发出 serialPortError 信号
- **AND** BytetraceBase 接收信号后调用 `ErrorHandler::instance().handleSerialPortError(error)`
- **AND** ErrorHandler 记录错误并发出 errorOccurred 信号
- **AND** UI（StatusBar）显示错误信息

#### Scenario: 串口读取错误
- **WHEN** 串口读取失败（QSerialPort::ReadError）
- **THEN** 同 WriteError 处理流程，用户可见错误提示

#### Scenario: 串口资源错误
- **WHEN** 串口设备被拔出（QSerialPort::ResourceError）
- **THEN** SerialPortManager 关闭串口
- **AND** BytetraceBase 收到通知后更新 UI 为断开状态
- **AND** 通过 ErrorHandler 记录并展示错误

### Requirement: BytetraceBase 覆盖所有串口错误类型
BytetraceBase::onSerialPortError() SHALL 处理所有 QSerialPort::SerialPortError 枚举值，不再仅处理 ResourceError。

#### Scenario: 任意串口错误到达
- **WHEN** 任意非 NoError 的串口错误发生
- **THEN** onSerialPortError 至少记录日志并更新 UI 状态
- **AND** 对于严重错误（ResourceError、WriteError、ReadError 等）触发 ErrorHandler
