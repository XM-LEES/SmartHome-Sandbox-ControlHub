# FastAPI 接口测试指南

本指南介绍如何使用 curl 快速测试 Smart Home API 的主要接口，适用于开发调试和功能验证。

---

## 1. 使用 curl 命令行工具

### 发送设备控制请求

linux:
```bash
curl -X POST \
  http://127.0.0.1:8000/api/v1/devices/livingroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'
```

win cmd:
```cmd
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/light/action -H "Content-Type: application/json" -d "{\"action\": \"ON\"}"
```

### 带参数请求示例

linux：
```bash
curl -X POST \
  http://127.0.0.1:8000/api/v1/devices/bedroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP", "value": 27}'
```

win cmd:
```cmd
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/ac/action -H "Content-Type: application/json" -d "{\"action\": \"SET_TEMP\", \"value\": 27}"
```

### 更多测试示例

#### 客厅设备
```bash
# 打开客厅灯光
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 关闭客厅灯光
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "OFF"}'

# 打开客厅空调
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 设置客厅空调温度
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP", "value": 25}'
```

#### 卧室设备
```bash
# 打开卧室主灯
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/main_light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 设置床头灯亮度
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/bedside_light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_BRIGHTNESS", "value": 80}'
```

#### 厨房设备
```bash
# 打开厨房抽油烟机
curl -X POST http://127.0.0.1:8000/api/v1/devices/kitchen/hood/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 设置抽油烟机速度
curl -X POST http://127.0.0.1:8000/api/v1/devices/kitchen/hood/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_SPEED", "value": 3}'

# 启动烤箱
curl -X POST http://127.0.0.1:8000/api/v1/devices/kitchen/oven/action \
  -H "Content-Type: application/json" \
  -d '{"action": "START"}'
```

#### 浴室设备
```bash
# 打开浴室灯光
curl -X POST http://127.0.0.1:8000/api/v1/devices/bathroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'
```

---

## 2. 响应示例

### 成功响应
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "ON",
    "correlation_id": "550e8400-e29b-41d4-a716-446655440000"
  }
}
```

### 超时/失败响应
```json
{
  "detail": "Device did not respond in time."
}
```

### 无效请求响应
```json
{
  "detail": "Invalid request: room, device, or action not recognized."
}
```

---

## 3. 常见问题

- 若收到 504 Gateway Timeout，说明设备未及时响应或未连接。
- 若收到 400 Bad Request，检查 action 名称、room_id、device_id 是否正确。
- 若 API 无法访问，请确认 FastAPI 服务和 MQTT Broker 均已启动。

---

## 4. 参考
- 访问 http://127.0.0.1:8000/docs 可查看自动生成的 Swagger API 文档并在线测试。
- 查看 `device_registry.py` 了解所有支持的设备和操作。 