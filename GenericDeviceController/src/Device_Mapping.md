# 设备映射关系表

## 概述

本文档描述了智能家居系统中所有设备的映射关系，包括房间、设备ID、设备类型、控制函数和GPIO引脚分配。

## 设备映射表

### 物理设备（16个）

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

### 虚拟设备 - 传感器（10个）

| 房间 | 设备ID | 设备类型 | 数据管理 | GPIO引脚 | 备注 |
|------|--------|----------|----------|----------|------|
| livingroom | temp_sensor | 客厅温度传感器 | getSensorData() | 0 | 虚拟设备，默认24.5°C |
| livingroom | humidity_sensor | 客厅湿度传感器 | getSensorData() | 0 | 虚拟设备，默认45.2% |
| bedroom | temp_sensor | 卧室温度传感器 | getSensorData() | 0 | 虚拟设备，默认23.8°C |
| bedroom | humidity_sensor | 卧室湿度传感器 | getSensorData() | 0 | 虚拟设备，默认48.5% |
| kitchen | temp_sensor | 厨房温度传感器 | getSensorData() | 0 | 虚拟设备，默认26.1°C |
| kitchen | humidity_sensor | 厨房湿度传感器 | getSensorData() | 0 | 虚拟设备，默认52.3% |
| bathroom | temp_sensor | 浴室温度传感器 | getSensorData() | 0 | 虚拟设备，默认25.3°C |
| bathroom | humidity_sensor | 浴室湿度传感器 | getSensorData() | 0 | 虚拟设备，默认65.8% |
| outdoor | temp_sensor | 室外温度传感器 | getSensorData() | 0 | 虚拟设备，默认20.0°C |
| outdoor | humidity_sensor | 室外湿度传感器 | getSensorData() | 0 | 虚拟设备，默认60.0% |

## 设备总数

**总设备数量**: 26个
- **物理设备**: 16个（需要GPIO控制）
- **虚拟设备**: 10个（传感器，软件模拟）

## 房间分布

- **客厅 (livingroom)**: 7个设备（5个物理 + 2个传感器）
- **卧室 (bedroom)**: 8个设备（6个物理 + 2个传感器）  
- **厨房 (kitchen)**: 4个设备（2个物理 + 2个传感器）
- **浴室 (bathroom)**: 5个设备（3个物理 + 2个传感器）
- **室外 (outdoor)**: 2个设备（2个传感器）

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

## 传感器数据管理

### 传感器数据管理器
- **文件**: `SensorDataManager.h` 和 `SensorDataManager.cpp`
- **功能**: 管理所有房间的传感器数据，提供数据存储、更新和读取功能
- **数据更新**: 通过 `setSensorData()` 接口手动更新传感器数据
- **数据范围**: 每个房间都有预定义的温度和湿度范围

### 传感器数据特点
- **持久化存储**: 传感器数据在ESP32内存中持续维护
- **稳定数据**: 数据保持稳定，确保同一时间读取得到相同结果
- **故障模拟**: 支持模拟传感器读取失败
- **调试支持**: 提供数据打印功能，便于调试





## 注意事项

1. **床头灯复用**: 床头灯使用与普通灯相同的控制逻辑，简化了代码实现
2. **引脚冲突**: 确保没有引脚冲突，每个设备使用独立的GPIO引脚
3. **设备类型**: 所有设备都支持标准的ON/OFF操作，空调额外支持温度设置
4. **错误处理**: 每个控制函数都包含完整的错误处理和调试输出
5. **传感器数据**: 传感器数据由SensorDataManager统一管理，确保数据一致性和真实性
