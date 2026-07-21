## ADDED Requirements

### Requirement: 串口参数自动恢复
打开串口时 SHALL 从 ConfigManager 读取上次保存的参数，并自动填充到对应的 ComboBox 控件。

#### Scenario: 首次启动无历史配置
- **WHEN** 首次启动应用（无历史配置）
- **AND** 打开 LeftBar 串口设置区域
- **THEN** ComboBox 显示系统默认值（第一个可用串口、115200、8N1）

#### Scenario: 再次启动恢复上次配置
- **WHEN** 应用之前使用过 COM3 / 9600 / 7 / Even / 2 并正常退出
- **AND** 再次启动应用
- **THEN** 串口 ComboBox 自动选中 COM3（如果仍可用）
- **AND** 波特率自动选中 9600
- **AND** 数据位、校验位、停止位自动恢复为上次设置
