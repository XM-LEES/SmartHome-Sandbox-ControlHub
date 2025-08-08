# 设备映射文档

## 概述

本文档描述了智能家居系统中所有设备的映射关系，包括房间、设备ID、设备类型、控制函数和GPIO引脚分配。

## 设备映射表

### 物理设备（13个）

| 房间 | 设备ID | 设备类型 | 控制函数 | GPIO引脚 | 备注 |
|------|--------|----------|----------|----------|------|
| livingroom | light | 客厅灯 | control_light() | 22 | 开关控制 |
| livingroom | ac | 客厅空调 | control_ac() | 21 | 支持温度设置 |
| livingroom | window | 客厅窗户 | control_window() | 19 | 开关控制 |
| livingroom | curtain | 客厅窗帘 | control_curtain() | 16 | 开关控制 |
| bedroom | light | 卧室灯 | control_light() | 4 | 开关控制 |
| bedroom | bedside_light | 床头灯 | control_light() | 2 | 复用灯控制逻辑 |
| bedroom | ac | 卧室空调 | control_ac() | 34 | 支持温度设置 |
| bedroom | window | 卧室窗户 | control_window() | 32 | 开关控制 |
| bedroom | curtain | 卧室窗帘 | control_curtain() | 25 | 开关控制 |
| kitchen | light | 厨房灯 | control_light() | 26 | 开关控制 |
| kitchen | hood | 油烟机 | control_hood() | 27 | 开关控制 |
| bathroom | light | 浴室灯 | control_light() | 14 | 开关控制 |
| bathroom | fan | 排气扇 | control_fan() | 13 | 开关控制 |


### 虚拟设备 - 传感器（13个）

| 房间 | 设备ID | 设备类型 | 数据管理 | GPIO引脚 | 备注 |
|------|--------|----------|----------|----------|------|
| livingroom | temp_sensor | 客厅温度传感器 | getSensorData() | 0 | 虚拟设备，默认24.5°C |
| livingroom | humidity_sensor | 客厅湿度传感器 | getSensorData() | 0 | 虚拟设备，默认45.2% |
| livingroom | brightness_sensor | 客厅亮度传感器 | getSensorData() | 0 | 虚拟设备，默认65.0% |
| bedroom | temp_sensor | 卧室温度传感器 | getSensorData() | 0 | 虚拟设备，默认23.8°C |
| bedroom | humidity_sensor | 卧室湿度传感器 | getSensorData() | 0 | 虚拟设备，默认48.5% |
| bedroom | brightness_sensor | 卧室亮度传感器 | getSensorData() | 0 | 虚拟设备，默认45.0% |
| kitchen | temp_sensor | 厨房温度传感器 | getSensorData() | 0 | 虚拟设备，默认26.1°C |
| kitchen | humidity_sensor | 厨房湿度传感器 | getSensorData() | 0 | 虚拟设备，默认52.3% |
| bathroom | temp_sensor | 浴室温度传感器 | getSensorData() | 0 | 虚拟设备，默认25.3°C |
| bathroom | humidity_sensor | 浴室湿度传感器 | getSensorData() | 0 | 虚拟设备，默认65.8% |
| outdoor | temp_sensor | 室外温度传感器 | getSensorData() | 0 | 虚拟设备，默认20.0°C |
| outdoor | humidity_sensor | 室外湿度传感器 | getSensorData() | 0 | 虚拟设备，默认60.0% |
| outdoor | brightness_sensor | 室外亮度传感器 | getSensorData() | 0 | 虚拟设备，默认85.0% |

## 设备总数

**总设备数量**: 29个
- **物理设备**: 16个（需要GPIO控制）
- **虚拟设备**: 13个（传感器，软件模拟）

## 房间分布

- **客厅 (livingroom)**: 8个设备（5个物理 + 3个传感器）
- **卧室 (bedroom)**: 9个设备（6个物理 + 3个传感器）  
- **厨房 (kitchen)**: 4个设备（2个物理 + 2个传感器）
- **浴室 (bathroom)**: 5个设备（3个物理 + 2个传感器）
- **室外 (outdoor)**: 3个设备（3个传感器）

## GPIO引脚分配

### 已使用的引脚
- GPIO 2: 卧室床头灯
- GPIO 4: 卧室灯
- GPIO 12: 浴室门
- GPIO 13: 浴室排气扇
- GPIO 14: 浴室灯
- GPIO 16: 客厅窗帘
- GPIO 17: 客厅门
- GPIO 19: 客厅窗户
- GPIO 21: 客厅空调
- GPIO 22: 客厅灯
- GPIO 25: 卧室窗帘
- GPIO 26: 厨房灯
- GPIO 27: 厨房油烟机
- GPIO 32: 卧室窗户
- GPIO 33: 卧室门
- GPIO 34: 卧室空调

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
- **功能**: 控制空调开关状态
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
  - temperature: 设定温度（ON操作时必须提供有效值）
- **适用设备**: 空调设备（ac）
- **支持操作**: ON, OFF
- **调用规则**:
  - `ON`：必须提供有效温度(0-40°C)，开启空调并设置温度
  - `OFF`：温度参数被忽略，仅关闭空调

### control_ac_set_temperature(room_id, temperature)
- **功能**: 设置空调目标温度
- **参数**:
  - room_id: 房间ID
  - temperature: 目标温度（0-40°C）
- **适用设备**: 空调设备（ac）
- **支持操作**: SET_TEMP
- **返回值**: true=成功, false=失败（温度超出范围或房间不存在）

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

### control_curtain(room_id, is_on)
- **功能**: 控制窗帘
- **参数**:
  - room_id: 房间ID
  - is_on: true=开, false=关
- **适用设备**: 窗帘（curtain）

### control_temperature_sensor(room_id)
- **功能**: 读取温度传感器数据
- **参数**:
  - room_id: 房间ID
- **返回值**: 温度值（°C），-999.0表示读取失败或不支持
- **适用设备**: 温度传感器（temp_sensor）
- **注意**: 仅在ENABLE_SENSOR_SIMULATOR=1时有效

### control_humidity_sensor(room_id)
- **功能**: 读取湿度传感器数据
- **参数**:
  - room_id: 房间ID
- **返回值**: 湿度值（%），-999.0表示读取失败或不支持
- **适用设备**: 湿度传感器（humidity_sensor）
- **注意**: 仅在ENABLE_SENSOR_SIMULATOR=1时有效

### control_brightness_sensor(room_id)
- **功能**: 读取亮度传感器数据
- **参数**:
  - room_id: 房间ID
- **返回值**: 亮度值（%），-999.0表示读取失败或不支持
- **适用设备**: 亮度传感器（brightness_sensor）
- **注意**: 仅在ENABLE_SENSOR_SIMULATOR=1时有效

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
- **文件**: `sensorsimulator/SensorDataManager.h` 和 `sensorsimulator/SensorDataManager.cpp`
- **功能**: 管理所有房间的传感器数据，提供数据存储、更新和读取功能
- **数据更新**: 通过 `setSensorData()` 接口手动更新传感器数据
- **数据范围**: 每个房间都有预定义的温度和湿度范围