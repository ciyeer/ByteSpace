## ADDED Requirements

### Requirement: DTR 引脚控制
系统 SHALL 支持手动控制串口的 DTR（Data Terminal Ready）信号线。

#### Scenario: 置位 DTR
- **WHEN** 用户勾选 "DTR" 复选框且串口已打开
- **THEN** 系统调用 `setDataTerminalReady(true)` 将 DTR 引脚置为高电平

#### Scenario: 清除 DTR
- **WHEN** 用户取消勾选 "DTR" 复选框且串口已打开
- **THEN** 系统调用 `setDataTerminalReady(false)` 将 DTR 引脚置为低电平

#### Scenario: 串口未打开时操作 DTR
- **WHEN** 用户勾选/取消 "DTR" 但串口未打开
- **THEN** 系统忽略操作，复选框状态不生效

### Requirement: RTS 引脚控制
系统 SHALL 支持手动控制串口的 RTS（Request To Send）信号线。

#### Scenario: 置位 RTS
- **WHEN** 用户勾选 "RTS" 复选框且串口已打开
- **THEN** 系统调用 `setRequestToSend(true)` 将 RTS 引脚置为高电平

#### Scenario: 清除 RTS
- **WHEN** 用户取消勾选 "RTS" 复选框且串口已打开
- **THEN** 系统调用 `setRequestToSend(false)` 将 RTS 引脚置为低电平
