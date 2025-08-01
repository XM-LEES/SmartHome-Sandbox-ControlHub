# 设备映射关系表

## 概述

本文档描述了智能家居系统中所有设备的映射关系，包括房间、设备ID、设备类型、控制函数和GPIO引脚分配。

## 设备映射表

| 房间 | 设备ID | 设备类型 | 控制函数 | GPIO引脚 | 备注 |
|------|--------|----------|----------|----------|------|
| livingroom | light | 客厅灯 | control_light() | 23 | 开关控制 |
| livingroom | ac | 客厅空调 | control_ac() | 22 | 支持温度设置 |
| livingroom | window | 客厅窗户 | control_window() | 15 | 开关控制 |
| livingroom | door | 客厅门 | control_door() | 14 | 开关控制 |
| livingroom | curtain | 客厅窗帘 | control_curtain() | 10 | 开关控制 |
| bedroom | light | 卧室灯 | control_light() | 20 | 开关控制 |
| bedroom | bedside_light | 床头灯 | control_light() | 21 | 复用灯控制逻辑 |
| bedroom | ac | 卧室空调 | control_ac() | 19 | 支持温度设置 |
| bedroom | window | 卧室窗户 | control_window() | 13 | 开关控制 |
| bedroom | door | 卧室门 | control_door() | 12 | 开关控制 |
| bedroom | curtain | 卧室窗帘 | control_curtain() | 9 | 开关控制 |
| kitchen | light | 厨房灯 | control_light() | 18 | 开关控制 |
| kitchen | hood | 油烟机 | control_hood() | 5 | 开关控制 |
| bathroom | light | 浴室灯 | control_light() | 16 | 开关控制 |
| bathroom | fan | 排气扇 | control_fan() | 17 | 开关控制 |
| bathroom | door | 浴室门 | control_door() | 11 | 开关控制 |

## 设备总数

**总设备数量**: 16个

## 房间分布

- **客厅 (livingroom)**: 5个设备
- **卧室 (bedroom)**: 6个设备  
- **厨房 (kitchen)**: 2个设备
- **浴室 (bathroom)**: 3个设备

## GPIO引脚分配

### 已使用的引脚
- GPIO 5: 厨房油烟机
- GPIO 9: 卧室窗帘
- GPIO 10: 客厅窗帘
- GPIO 11: 浴室门
- GPIO 12: 卧室门
- GPIO 13: 卧室窗户
- GPIO 14: 客厅门
- GPIO 15: 客厅窗户
- GPIO 16: 浴室灯
- GPIO 17: 浴室排气扇
- GPIO 18: 厨房灯
- GPIO 19: 卧室空调
- GPIO 20: 卧室灯
- GPIO 21: 卧室床头灯
- GPIO 22: 客厅空调
- GPIO 23: 客厅灯

### 引脚分配原则
- 避免使用ESP32的特殊引脚（如启动引脚、下载引脚等）
- 优先使用数字引脚，便于控制
- 引脚分配考虑了物理布局的便利性

## 控制函数说明

### control_light(room_id, is_on)
- **功能**: 控制各种类型的灯
- **参数**: 
  - room_id: 房间ID
  - is_on: true=开, false=关
- **适用设备**: 所有灯类设备（light, bedside_light）

### control_ac(room_id, is_on, temperature)
- **功能**: 控制空调设备
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
  - temperature: 设定温度
- **适用设备**: 空调设备（ac）

### control_hood(room_id, is_on)
- **功能**: 控制油烟机
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
- **适用设备**: 油烟机（hood）

### control_fan(room_id, is_on)
- **功能**: 控制排气扇
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
- **适用设备**: 排气扇（fan）

### control_window(room_id, is_on)
- **功能**: 控制窗户
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
- **适用设备**: 窗户（window）

### control_door(room_id, is_on)
- **功能**: 控制门
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
- **适用设备**: 门（door）

### control_curtain(room_id, is_on)
- **功能**: 控制窗帘
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
- **适用设备**: 窗帘（curtain）

## MQTT Topic格式

### 命令Topic
```
smarthome/{room_id}/{device_id}/command
```

### 状态Topic
```
smarthome/{room_id}/{device_id}/state
```

### 示例
- 客厅灯命令: `smarthome/livingroom/light/command`
- 卧室空调状态: `smarthome/bedroom/ac/state`

## 注意事项

1. **床头灯复用**: 床头灯使用与普通灯相同的控制逻辑，简化了代码实现
2. **引脚冲突**: 确保没有引脚冲突，每个设备使用独立的GPIO引脚
3. **设备类型**: 所有设备都支持标准的ON/OFF操作，空调额外支持温度设置
4. **错误处理**: 每个控制函数都包含完整的错误处理和调试输出
