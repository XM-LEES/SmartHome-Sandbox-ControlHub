# 智能家居控制系统 API 文档

## 1. 概述

本文档描述了智能家居沙盘演示系统的API接口规范。系统采用RESTful API设计，通过MQTT协议与下位机设备进行异步通信，支持可靠的闭环控制。

### 系统架构
- **上位机**：FastAPI Web服务，处理HTTP请求
- **消息中间件**：MQTT Broker，负责消息路由
- **下位机**：ESP32节点，控制物理设备

### 基础信息
- **基础URL**: `http://127.0.0.1:8000`
- **API版本**: v1
- **超时时间**: 3秒
- **内容类型**: `application/json`

---

## 2. 设备配置

### 2.1 设备类型概览

| 房间 | 设备数量 | 设备类型 |
|------|----------|----------|
| 客厅 (livingroom) | 6个 | 灯、空调、窗户、窗帘、温度传感器、湿度传感器 |
| 卧室 (bedroom) | 7个 | 灯、床头灯、空调、窗户、窗帘、温度传感器、湿度传感器 |
| 厨房 (kitchen) | 4个 | 灯、油烟机、温度传感器、湿度传感器 |
| 浴室 (bathroom) | 4个 | 灯、排气扇、温度传感器、湿度传感器 |
| 室外 (outdoor) | 2个 | 温度传感器、湿度传感器 |

**总设备数量**: 23个（13个物理设备 + 10个虚拟传感器）

### 2.2 详细设备列表

#### 2.2.1 客厅设备 (livingroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 开关灯 | `ON`, `OFF` | 无需参数 |
| `ac` | 空调 | `ON`, `OFF`, `SET_TEMP` | `ON`和`SET_TEMP`都需要`value`参数(温度值0-40°C) |
| `window` | 窗户 | `ON`, `OFF` | 无需参数 |
| `curtain` | 窗帘 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 2.2.2 卧室设备 (bedroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 灯 | `ON`, `OFF` | 无需参数 |
| `bedside_light` | 床头灯 | `ON`, `OFF` | 无需参数 |
| `ac` | 空调 | `ON`, `OFF`, `SET_TEMP` | `ON`和`SET_TEMP`都需要`value`参数(温度值0-40°C) |
| `window` | 窗户 | `ON`, `OFF` | 无需参数 |
| `curtain` | 窗帘 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 2.2.3 厨房设备 (kitchen)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 厨房灯 | `ON`, `OFF` | 无需参数 |
| `hood` | 油烟机 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 2.2.4 浴室设备 (bathroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 浴室灯 | `ON`, `OFF` | 无需参数 |
| `fan` | 排气扇 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 2.2.5 室外设备 (outdoor)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

---

## 3. API接口规范

### 3.1 通用接口定义

**接口地址**: `POST /api/v1/devices/{room_id}/{device_id}/action`

**路径参数**:
- `room_id`: 房间ID (livingroom, bedroom, kitchen, bathroom, outdoor)
- `device_id`: 设备ID (参考设备列表)

**请求体格式**:
```json
{
  "action": "ACTION_NAME",
  "value": 123
}
```

**参数说明**:
- `action`: 操作名称 (必需)
- `value`: 操作值 (可选，部分操作需要)

---

## 4. 设备控制API详细说明

### 4.1 开关类设备

#### 4.1.1 灯光设备 (light, bedside_light)

**支持操作**: `ON`, `OFF`

##### ON操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "ON",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440000"
  }
}
```

**错误响应示例**:
```json
// 400 - 无效房间
{
  "detail": "Invalid request: room, device, or action not recognized."
}

// 502 - 设备错误
{
  "detail": "Device error: DEVICE_NOT_FOUND - Device not found in this node's configuration"
}

// 504 - 设备超时
{
  "detail": "Device did not respond in time."
}
```

##### OFF操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/bedside_light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "OFF",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440001"
  }
}
```

**错误响应示例**: 同ON操作

#### 4.1.2 门窗设备 (door, window, curtain)

**支持操作**: `ON`, `OFF`

##### ON操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/door/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "ON",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440002"
  }
}
```

**错误响应示例**: 同灯光设备

##### OFF操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/curtain/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "OFF",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440003"
  }
}
```

**错误响应示例**: 同灯光设备

#### 4.1.3 风扇设备 (fan, hood)

**支持操作**: `ON`, `OFF`

##### ON操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/kitchen/hood/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "ON",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440004"
  }
}
```

**错误响应示例**: 同灯光设备

##### OFF操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/bathroom/fan/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "OFF",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440005"
  }
}
```

**错误响应示例**: 同灯光设备

### 4.2 其他设备

#### 4.2.1 空调 (ac)

**支持操作**: `ON`, `OFF`, `SET_TEMP`

 > **空调操作说明**：
 > - `ON`：开启空调，**必须**提供 `value` 参数设置温度（0-40°C）
 > - `SET_TEMP`：设置目标温度，**必须**提供 `value` 参数（0-40°C）
 > - `OFF`：关闭空调，**无需** `value` 参数

##### ON操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON", "value": 25}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "ON",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440006"
  }
}
```

**错误响应示例**:
```json
// 400 - 缺少温度参数
{
  "detail": "AC ON operation requires valid temperature value (0-40°C)."
}

// 400 - 温度超出范围
{
  "detail": "AC ON operation requires valid temperature value (0-40°C)."
}

// 502 - 设备错误
{
  "detail": "Device error: MISSING_OR_INVALID_VALUE - ON operation requires valid temperature value (0-40°C)"
}
```

##### OFF操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "OFF",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440007"
  }
}
```

**错误响应示例**: 同其他设备的基础错误

##### SET_TEMP操作

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP", "value": 23}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "SET_TEMP",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440008",
    "temperature": 23,
    "unit": "°C"
  }
}
```

**错误响应示例**:
```json
// 400 - 缺少温度参数或参数为null
{
  "detail": "AC SET_TEMP operation requires valid temperature value (0-40°C)."
}

// 502 - 温度超出范围
{
  "detail": "Device error: INVALID_TEMPERATURE - Temperature value invalid. Valid range: 0-40°C"
}
```

#### 4.2.2 传感器设备 (temp_sensor, humidity_sensor)

**支持操作**: `READ`

##### READ操作 (温度传感器)

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/temp_sensor/action \
  -H "Content-Type: application/json" \
  -d '{"action": "READ"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "sensor_data": {
    "value": 24.5,
    "unit": "°C"
  }
}
```

**错误响应示例**:
```json
// 502 - 传感器读取失败
{
  "detail": "Device error: SENSOR_READ_ERROR - Temperature sensor read failed"
}

// 502 - 数据不完整
{
  "detail": "Sensor data incomplete or malformed"
}
```

##### READ操作 (湿度传感器)

**请求示例**:
```bash
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/humidity_sensor/action \
  -H "Content-Type: application/json" \
  -d '{"action": "READ"}'
```

**成功响应示例** (HTTP 200):
```json
{
  "status": "success",
  "sensor_data": {
    "value": 45.2,
    "unit": "%"
  }
}
```

**错误响应示例**:
```json
// 502 - 传感器读取失败
{
  "detail": "Device error: SENSOR_READ_ERROR - Humidity sensor read failed"
}
```

---

## 5. 错误处理参考

### 5.1 HTTP状态码说明

| HTTP状态码 | 错误类型 | 说明 | 解决方案 |
|------------|----------|------|----------|
| 200 | 成功 | 设备操作成功执行 | - |
| 400 | 请求错误 | 房间、设备、操作参数无效或缺少必需参数 | 检查请求参数是否正确 |
| 502 | 设备错误 | 设备返回错误状态 | 检查设备配置和状态 |
| 504 | 网关超时 | 设备未在3秒内响应 | 检查设备是否在线，网络是否正常 |

### 5.2 设备错误代码对照表

| 错误代码 | 说明 | 可能原因 | 解决建议 |
|----------|------|----------|----------|
| `JSON_PARSE_ERROR` | JSON格式错误 | 请求体格式不正确 | 检查JSON语法 |
| `MISSING_REQUIRED_FIELDS` | 缺少必需字段 | action或correlation_id缺失 | 补充必需字段 |
| `UNKNOWN_DEVICE_TYPE` | 未知设备类型 | 设备ID不在配置中 | 检查设备ID是否正确 |
| `UNKNOWN_ROOM` | 未知房间ID | 房间ID不存在 | 检查房间ID是否正确 |
| `DEVICE_NOT_FOUND` | 设备未找到 | 设备在当前节点配置中不存在 | 检查设备是否在正确的节点 |
| `SENSOR_READ_ERROR` | 传感器读取失败 | 传感器硬件故障或通信问题 | 检查传感器连接 |
| `INVALID_TEMPERATURE` | 温度值无效 | 温度超出有效范围(0-40°C) | 使用有效的温度值 |
| `MISSING_OR_INVALID_VALUE` | 缺少或无效的value参数 | 空调操作缺少温度值 | 为空调操作提供温度值 |
| `UNKNOWN_ACTION` | 未知操作 | 设备不支持的操作 | 使用设备支持的操作 |

---