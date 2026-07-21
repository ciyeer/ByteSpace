## ADDED Requirements

### Requirement: CRC-16/MODBUS 校验
系统 SHALL 支持在发送数据末尾自动追加 CRC-16/MODBUS 校验值。

#### Scenario: 发送数据时追加 CRC
- **WHEN** 用户勾选 "CRC校验" 复选框，在发送区输入 `01 03 00 00 00 01`（HEX 模式），点击发送
- **THEN** 系统计算 CRC-16/MODBUS 校验值并追加到数据末尾发送（发送 `01 03 00 00 00 01 84 0A`）

#### Scenario: 文本模式下 CRC 校验
- **WHEN** 用户勾选 "CRC校验" 但未勾选 HEX 发送模式，发送 UTF-8 文本
- **THEN** 系统对文本的 UTF-8 字节序列计算 CRC 并追加

#### Scenario: CRC 与发送新行同时生效
- **WHEN** 用户同时勾选 "CRC校验" 和 "发送新行"
- **THEN** 系统先对数据计算 CRC 追加，再追加换行符（数据 + CRC + 换行符）

#### Scenario: 未勾选 CRC 时不追加
- **WHEN** 用户未勾选 "CRC校验"
- **THEN** 系统按正常模式发送，不追加任何校验值
