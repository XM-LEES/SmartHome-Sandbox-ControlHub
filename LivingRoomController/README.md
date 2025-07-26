# LivingRoomController

本项目为ESP32实现的客厅智能家居控制器，支持灯和空调的MQTT远程控制。

## 功能
- 通过MQTT接收控制命令，控制客厅灯和空调
- 支持闭环回执，执行后上报状态
- 代码结构清晰，便于扩展

## 依赖
- ArduinoJson
- PubSubClient
- WiFi

## 使用方法
1. 修改 `LivingRoomController.ino` 中的 WiFi 和 MQTT Broker 配置。
2. 使用 Arduino IDE 或 PlatformIO 上传到 ESP32。
3. 确保树莓派上的MQTT Broker已启动。
4. 通过上位机API发送控制命令。 