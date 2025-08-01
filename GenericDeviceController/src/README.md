# 通用设备控制器固件 (GenericDeviceController)

本项目是为智能家居沙盘系统设计的通用ESP32固件，通过MQTT协议接收并执行来自上位机的控制指令。

## 功能特性
- **多设备支持**: 通过配置文件 (`Node_X_Config.h`) 可灵活定义单个ESP32节点所控制的多个设备及其引脚。
- **动态Topic订阅**: 启动时会根据配置文件自动订阅所有相关设备的`command` Topic。
- **可靠回执**: 只有在硬件操作**成功执行**后，才会向对应的`state` Topic发布状态回执，以完成闭环控制。
- **分层设计**: 代码结构清晰，分为主逻辑 (`.ino`) 和硬件抽象层 (`DeviceControl.h`)，便于维护和扩展。

## 依赖库
- `ArduinoJson`
- `PubSubClient`
- `WiFi`

## 使用方法
1.  **创建配置文件**: 复制或创建一个`Node_X_Config.h`文件，在其中定义此ESP32节点所控制的所有设备。
2.  **修改配置**: 在`Config.h`中修改WiFi凭据和MQTT Broker的IP地址。
3.  **上传固件**: 使用 Arduino IDE 或 PlatformIO 将代码编译并上传到ESP32。
4.  **监控**: 通过串口监视器查看设备的连接状态、收到的命令和执行日志。

## 通信协议与约定

### 1. Topic 结构
- **命令接收**: `smarthome/{room_id}/{device_id}/command`
- **状态回报**: `smarthome/{room_id}/{device_id}/state`

### 2. 命令Payload格式 (JSON)
- **接收格式**:
  ```json
  {
    "action": "ON",
    "value": 25,
    "correlation_id": "some-unique-id"
  }
  ```

### 3. 状态回执Payload格式 (JSON)
- **发送格式**:
  ```json
  {
    "state": "ON",
    "correlation_id": "some-unique-id"
  }
  ```

### 4. 核心通信逻辑 (当前实现)
本固件遵循**"明确错误回执"（Explicit Error Reporting）**模式。
- **成功路径**: 当收到一个有效的命令，并且对应的硬件操作成功执行后，固件会发送一个带有状态和`correlation_id`的回执。
- **失败路径**: 在以下任何情况下，固件会发送带有`"state": "ERROR"`和具体错误信息的回执：
    - 接收到的Topic格式不正确。
    - 接收到的Payload不是有效的JSON。
    - JSON中缺少`action`或`correlation_id`字段。
    - 命令中的设备是此节点不负责的。
    - 硬件执行函数返回`false`（表示执行失败）。

这允许上位机区分**设备超时**（可能离线）和**设备明确报告的错误**（在线但执行失败），从而返回更精确的HTTP错误码（如 `502 Bad Gateway`）给最终用户。
