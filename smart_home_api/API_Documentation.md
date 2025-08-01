# 智能家居控制系统 API 文档

## 概述

本文档描述了智能家居沙盘演示系统的API接口规范。系统采用RESTful API设计，通过MQTT协议与下位机设备进行异步通信，支持可靠的闭环控制。

**基础URL**: `http://127.0.0.1:8000`  
**API版本**: v1  
**超时时间**: 3秒  
**内容类型**: `application/json`

---

## 设备列表

### 客厅设备 (livingroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 开关灯 | `ON`, `OFF` | 无需参数 |
| `ac` | 空调 | `ON`, `OFF`, `SET_TEMP` | `SET_TEMP`需要`value`参数(温度值) |

### 卧室设备 (bedroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `main_light` | 主灯 | `ON`, `OFF` | 无需参数 |
| `bedside_light` | 床头灯 | `ON`, `OFF`, `SET_BRIGHTNESS` | `SET_BRIGHTNESS`需要`value`参数(亮度0-100) |
| `ac` | 空调 | `ON`, `OFF`, `SET_TEMP` | `SET_TEMP`需要`value`参数(温度值) |

### 厨房设备 (kitchen)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 厨房灯 | `ON`, `OFF` | 无需参数 |
| `hood` | 抽油烟机 | `ON`, `OFF`, `SET_SPEED` | `SET_SPEED`需要`value`参数(速度等级) |
| `oven` | 烤箱 | `START`, `STOP`, `SET_MODE` | `SET_MODE`需要`value`参数(模式编号) |

### 浴室设备 (bathroom)

| 设备ID | 设备类型 | 支持操作 | 参数说明 |
|--------|----------|----------|----------|
| `light` | 浴室灯 | `ON`, `OFF` | 无需参数 |

---

## API 接口

### 设备控制接口

**接口地址**: `POST /api/v1/devices/{room_id}/{device_id}/action`

**路径参数**:
- `room_id`: 房间ID (livingroom, bedroom, kitchen, bathroom)
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

# 关闭卧室主灯
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/main_light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'
```

### 带参数操作

```bash
# 设置客厅空调温度为25度
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP", "value": 25}'

# 设置床头灯亮度为80%
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/bedside_light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_BRIGHTNESS", "value": 80}'

# 设置抽油烟机速度为3级
curl -X POST http://127.0.0.1:8000/api/v1/devices/kitchen/hood/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_SPEED", "value": 3}'
```

---

## 响应格式

### 成功响应 (HTTP 200)

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

### 错误响应

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

## 错误码对照表

| HTTP状态码 | 错误类型 | 说明 | 解决方案 |
|------------|----------|------|----------|
| 200 | 成功 | 设备操作成功执行 | - |
| 400 | 请求错误 | 房间、设备或操作参数无效 | 检查请求参数是否正确 |
| 504 | 网关超时 | 设备未在3秒内响应 | 检查设备是否在线，网络是否正常 |

---

## 使用注意事项

1. **超时机制**: 系统采用3秒超时机制，确保快速响应
2. **闭环控制**: 所有操作都会等待设备确认，确保操作可靠性
3. **参数验证**: 系统会自动验证房间、设备和操作的合法性
4. **异步通信**: 基于MQTT的异步通信，支持高并发处理

---

## 在线测试

访问 `http://127.0.0.1:8000/docs` 可查看自动生成的Swagger API文档并进行在线测试。

---

## 技术支持

如有问题，请检查：
1. FastAPI服务是否正常运行
2. MQTT Broker是否已启动
3. 设备是否在线并正常连接
4. 网络连接是否正常 