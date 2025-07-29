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
  -d '{"action": "SET_TEMP", "value": "27"}'
```

win cmd:
```cmd
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/ac/action -H "Content-Type: application/json" -d "{\"action\": \"SET_TEMP\", \"value\": \"27\"}"
```

---

## 2. 响应示例

### 成功响应
```json
{
  "status": "success",
  "confirmed_result": {
    "state": "turn_on",
    "correlation_id": "..."
  }
}
```

### 超时/失败响应
```json
{
  "detail": "Device did not respond in time."
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