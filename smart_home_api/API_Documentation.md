# 智能家居控制系统 API 文档

## 目录

- [概述](#概述)
- [设备配置](#设备配置)
- [API 接口](#api-接口)
- [请求示例](#请求示例)
- [响应格式](#响应格式)
- [错误处理](#错误处理)
- [使用指南](#使用指南)

---

## 概述

本文档描述了智能家居沙盘演示系统的API接口规范。系统采用RESTful API设计，通过MQTT协议与下位机设备进行异步通信，支持可靠的闭环控制。

**基础URL**: `http://127.0.0.1:8000` 
**API版本**: v1 
**超时时间**: 3秒 
**内容类型**: `application/json`

---


## 设备配置

### 设备类型概览

| 房间 | 设备数量 | 设备类型 |
|------|----------|----------|
| 客厅 (livingroom) | 7个 | 灯、空调、窗户、门、窗帘、温度传感器、湿度传感器 |
| 卧室 (bedroom) | 8个 | 灯、床头灯、空调、窗户、门、窗帘、温度传感器、湿度传感器 |
| 厨房 (kitchen) | 4个 | 灯、油烟机、温度传感器、湿度传感器 |
| 浴室 (bathroom) | 5个 | 灯、排气扇、门、温度传感器、湿度传感器 |
| 室外 (outdoor) | 2个 | 温度传感器、湿度传感器 |

### 详细设备列表

#### 客厅设备 (livingroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 开关灯 | `ON`, `OFF` | 无需参数 |
| `ac` | 空调 | `ON`, `OFF`, `SET_TEMP` | `SET_TEMP`需要`value`参数(温度值) |
| `window` | 窗户 | `ON`, `OFF` | 无需参数 |
| `door` | 门 | `ON`, `OFF` | 无需参数 |
| `curtain` | 窗帘 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 卧室设备 (bedroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 灯 | `ON`, `OFF` | 无需参数 |
| `bedside_light` | 床头灯 | `ON`, `OFF` | 无需参数 |
| `ac` | 空调 | `ON`, `OFF`, `SET_TEMP` | `SET_TEMP`需要`value`参数(温度值) |
| `window` | 窗户 | `ON`, `OFF` | 无需参数 |
| `door` | 门 | `ON`, `OFF` | 无需参数 |
| `curtain` | 窗帘 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 厨房设备 (kitchen)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 厨房灯 | `ON`, `OFF` | 无需参数 |
| `hood` | 油烟机 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 浴室设备 (bathroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 浴室灯 | `ON`, `OFF` | 无需参数 |
| `fan` | 排气扇 | `ON`, `OFF` | 无需参数 |
| `door` | 门 | `ON`, `OFF` | 无需参数 |
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

#### 室外设备 (outdoor)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `temp_sensor` | 温度传感器 | `READ` | 无需参数 |
| `humidity_sensor` | 湿度传感器 | `READ` | 无需参数 |

---

## API 接口

### 设备控制接口

**接口地址**: `POST /api/v1/devices/{room_id}/{device_id}/action`

**路径参数**:
- `room_id`: 房间ID (livingroom, bedroom, kitchen, bathroom, outdoor)
- `device_id`: 设备ID (参考设备列表)

**请求体**:
```json
{
  "action": "ACTION_NAME",
  "value": 123
}
```

**参数说明**:
- `action`: 操作名称 (参考设备列表中的支持操作)
- `value`: 操作值 (可选，仅部分操作需要)

---

## 请求示例

### 基础开关操作

```bash
# 打开客厅灯光
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 关闭卧室灯
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'

# 打开卧室床头灯
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/bedside_light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 打开厨房油烟机
curl -X POST http://127.0.0.1:8000/api/v1/devices/kitchen/hood/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 打开浴室排气扇
curl -X POST http://127.0.0.1:8000/api/v1/devices/bathroom/fan/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 打开客厅空调
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 关闭卧室空调
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'

# 打开客厅窗户
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/window/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 关闭卧室门
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/door/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'

# 打开客厅窗帘
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/curtain/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 关闭卧室窗帘
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/curtain/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'
```

### 带参数操作

```bash
# 设置客厅空调温度为25度
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP", "value": 25}'

# 设置卧室空调温度为23度
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP", "value": 23}'
```

### 传感器读取操作

```bash
# 读取客厅温度传感器
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/temp_sensor/action \
  -H "Content-Type: application/json" \
  -d '{"action": "READ"}'

# 读取卧室湿度传感器
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/humidity_sensor/action \
  -H "Content-Type: application/json" \
  -d '{"action": "READ"}'

# 读取厨房温度传感器
curl -X POST http://127.0.0.1:8000/api/v1/devices/kitchen/temp_sensor/action \
  -H "Content-Type: application/json" \
  -d '{"action": "READ"}'

# 读取室外湿度传感器
curl -X POST http://127.0.0.1:8000/api/v1/devices/outdoor/humidity_sensor/action \
  -H "Content-Type: application/json" \
  -d '{"action": "READ"}'
```

### 错误请求示例

```bash
# 无效房间ID
curl -X POST http://127.0.0.1:8000/api/v1/devices/invalid_room/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 无效设备ID
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/invalid_device/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 无效操作
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "INVALID_ACTION"}'

# 缺少必需参数
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP"}'
```

---

## 响应格式

### 设备操作响应 (HTTP 200)

```json
{
  "status": "success",
  "confirmed_result": {
    "state": "ACTION_NAME",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440000"
  }
}
```

**响应字段说明**:
- `status`: 操作状态，成功时为"success"
- `confirmed_result.state`: 设备确认的执行状态
- `confirmed_result.correlation_id`: 请求关联ID，用于追踪

### 传感器读取响应 (HTTP 200)

```json
{
  "status": "success",
  "sensor_data": {
    "value": 24.5,
    "unit": "°C"
  }
}
```

**传感器响应字段说明**:
- `status`: 操作状态，成功时为"success"
- `sensor_data.value`: 传感器数值
- `sensor_data.unit`: 数值单位

---

## 错误处理

### 错误码对照表

| HTTP状态码 | 错误类型 | 说明 | 解决方案 |
|------------|----------|------|----------|
| 200 | 成功 | 设备操作成功执行 | - |
| 400 | 请求错误 | 房间、设备或操作参数无效 | 检查请求参数是否正确 |
| 502 | 设备错误 | 设备返回错误状态 | 检查设备配置和状态 |
| 504 | 网关超时 | 设备未在3秒内响应 | 检查设备是否在线，网络是否正常 |

### 错误响应示例

#### 400 Bad Request - 无效请求
```json
{
  "detail": "Invalid request: room, device, or action not recognized."
}
```

**触发条件**:
- 房间ID不存在
- 设备ID不存在
- 操作名称不支持

#### 502 Bad Gateway - 设备错误
```json
{
  "detail": "Device error: DEVICE_NOT_FOUND - Device not found in this node's configuration"
}
```

**触发条件**:
- 设备返回了错误状态
- 设备不支持请求的操作
- 设备配置错误

**常见错误代码**:
- `JSON_PARSE_ERROR`: JSON格式错误
- `MISSING_REQUIRED_FIELDS`: 缺少必需字段
- `UNKNOWN_DEVICE_TYPE`: 未知设备类型
- `UNKNOWN_ROOM`: 未知房间ID
- `DEVICE_NOT_FOUND`: 设备在当前节点配置中不存在
- `SENSOR_READ_ERROR`: 传感器读取失败

#### 504 Gateway Timeout - 设备超时
```json
{
  "detail": "Device did not respond in time."
}
```

**触发条件**:
- 设备在3秒内未响应
- 设备离线或网络异常
- 设备执行失败

---

## 使用指南

### 在线测试

访问 `http://127.0.0.1:8000/docs` 可查看自动生成的Swagger API文档并进行在线测试。

### 故障排除

如遇问题，请按以下顺序检查：

1. **网络连接**: 确认网络连接是否正常
2. **FastAPI服务**: 确认服务是否正常运行
3. **MQTT Broker**: 确认MQTT服务器是否已启动
4. **设备连接**: 确认下位机设备是否正常连接