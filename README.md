# SmartHome-Sandbox-ControlHub

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT) 
[![Python 3.9+](https://img.shields.io/badge/Python-3.9%2B-green.svg)](https://www.python.org/) 
[![Platform: Raspberry Pi](https://img.shields.io/badge/Platform-Raspberry_Pi-red.svg)](https://www.raspberrypi.org/)

## 📖 概述 / Overview

🔥 **智能家居沙盘控制系统** | 基于树莓派与ESP32的高扩展性物联网解决方案

一个模块化的智能家居控制系统，通过Web API与MQTT协议实现设备闭环控制。树莓派作为中央枢纽运行异步FastAPI服务，支持高并发请求；ESP32作为分布式节点控制灯光、空调等设备，并通过MQTT实时反馈状态。适用于物联网演示、智能家居原型开发。


**技术亮点**：
- ☁️ RESTful API + MQTT 松耦合架构
- 🔄 闭环控制（命令执行状态实时反馈）
- ⚡ FastAPI 异步高性能处理
- 📡 ESP32 多设备分布式控制
- 🧩 模块化设计，支持快速扩展

**Smart Home Sandbox Control System：**
🔥 Smart Home Sandbox Control System | Scalable IoT Solution with Raspberry Pi & ESP32  
A modular smart home control system featuring closed-loop device control via Web API and MQTT protocol. Raspberry Pi runs async FastAPI service as the central 
hub, while ESP32 nodes control devices (lights, AC, etc.) and provide real-time MQTT feedback. Ideal for IoT education and smart home prototyping.  

**Key Features**:  
- ☁️ Decoupled Architecture (RESTful API + MQTT)  
-  🔄 Closed-loop Control with Execution Feedback  
-  ⚡ Async FastAPI for High Concurrency  
-  📡 Distributed ESP32 Device Control  
-  Extensible Modular Design  
---

## 🏗️ 技术栈 / Tech Stack

- **Central Hub**: Raspberry Pi 4B+ 
- **IoT Protocol**: MQTT (Mosquitto Broker)
- **Backend**: Python FastAPI (Async)
- **Device Nodes**: ESP32 + Arduino C++
- **Communication**: WiFi / Ethernet
- **Dependencies**: paho-mqtt, uvicorn, pydantic

---

## 📂 项目结构 / Project Structure

```
SmartHome-Sandbox-ControlHub/
├── smart_home_api/                    # 后端API服务
│   ├── main.py                       # FastAPI主入口
│   ├── mqtt_client.py                # MQTT通信管理
│   ├── request_manager.py            # 设备请求管理
│   ├── device_registry.py            # 设备注册表
│   ├── config.py                     # 配置文件
│   ├── requirements.txt              # Python依赖
│   ├── API_Documentation.md          # API文档和测试指南
│   └── README.md                     # API服务文档
├── GenericDeviceController/          # ESP32通用设备控制器
│   ├── platformio.ini               # PlatformIO配置
│   └── src/
│       ├── GenericDeviceController.ino  # 主程序
│       ├── Config.h                     # WiFi和MQTT配置
│       ├── DeviceControl.h              # 设备控制抽象层
│       ├── Node1Config.h                # 节点配置示例
│       ├── Device_Mapping.md            # 设备引脚映射文档
│       └── README.md                    # 固件文档
└── README.md                         # 项目说明文档
```

---

## 🚀 快速开始 / Quick Start

### 硬件准备
- 树莓派（安装Raspbian OS）
- ESP32开发板 x N
- 继电器模块/LED灯（模拟设备）

### 服务部署

#### 1. 安装Python依赖
```bash
cd smart_home_api
pip install -r requirements.txt
```

#### 2. 配置并启动MQTT Broker
详细配置过程请参考 [smart_home_api/README.md](smart_home_api/README.md)
```bash
# 使用Docker启动Mosquitto（推荐）
docker run -d \
  -p 1883:1883 \
  -p 9001:9001 \
  --name mosquitto \
  --restart always \
  eclipse-mosquitto
```

#### 3. 启动API服务
```bash
# 在项目根目录下运行
uvicorn smart_home_api.main:app --reload --host 0.0.0.0 --port 8000
```

#### 4. 上传ESP32固件
```bash
# 使用PlatformIO或Arduino IDE
cd GenericDeviceController
# 修改src/Config.h中的WiFi和MQTT配置
# 修改src/Node1Config.h中的设备引脚配置
# 编译并上传到ESP32
```

### 示例API调用

```bash
# 打开客厅灯光
curl -X POST http://127.0.0.1:8000/api/v1/devices/livingroom/light/action \
  -H "Content-Type: application/json" \
  -d '{"action": "ON"}'

# 设置卧室空调温度
curl -X POST http://127.0.0.1:8000/api/v1/devices/bedroom/ac/action \
  -H "Content-Type: application/json" \
  -d '{"action": "SET_TEMP", "value": 25}'


```

更多API测试示例请参考 [API_Documentation.md](smart_home_api/API_Documentation.md)

---

## 📝 开发文档 / Documentation

### API接口文档
- **在线文档**: 启动服务后访问 http://127.0.0.1:8000/docs
- **测试指南**: [smart_home_api/API_Documentation.md](smart_home_api/API_Documentation.md)
- **API服务文档**: [smart_home_api/README.md](smart_home_api/README.md)

### 设备通信协议
- **固件文档**: [GenericDeviceController/src/README.md](GenericDeviceController/src/README.md)
- **MQTT Topic格式**: `smarthome/{room_id}/{device_id}/command|state`
- **通信协议**: 基于JSON的请求-响应模式，支持correlation_id追踪

### 支持的设备类型
- **客厅**: 灯光(ON/OFF), 空调(ON/OFF/SET_TEMP), 窗户(ON/OFF), 门(ON/OFF), 窗帘(ON/OFF)
- **卧室**: 主灯(ON/OFF), 床头灯(ON/OFF), 空调(ON/OFF/SET_TEMP), 窗户(ON/OFF), 门(ON/OFF), 窗帘(ON/OFF)
- **厨房**: 灯光(ON/OFF), 抽油烟机(ON/OFF)
- **浴室**: 灯光(ON/OFF), 排气扇(ON/OFF), 门(ON/OFF)

---

## 🔧 配置说明 / Configuration

### MQTT配置
- **Broker地址**: 默认 `localhost:1883`
- **认证**: 开发环境支持匿名连接
- **持久化**: 支持消息持久化存储

### ESP32配置
- **WiFi**: 在 `Node1Config.h` 中配置SSID和密码
- **MQTT**: 配置Broker IP地址
- **设备映射**: 通过配置文件定义引脚和设备关系

---

## 🐛 故障排除 / Troubleshooting

### 常见问题
1. **504 Gateway Timeout**: 设备未响应，检查ESP32连接状态
2. **400 Bad Request**: 请求参数错误，检查room_id、device_id、action
3. **502 Bad Gateway**: 设备返回错误状态，检查错误代码和描述
4. **MQTT连接失败**: 确认Mosquitto服务已启动
5. **ESP32无法连接**: 检查WiFi配置和网络连接

### 调试方法
- 查看FastAPI日志: `uvicorn smart_home_api.main:app --log-level debug`
- 查看MQTT日志: `docker logs mosquitto`
- ESP32串口监视器: 查看设备连接和命令执行状态

---

## 🤝 参与贡献 / Contributing

欢迎提交Issue或PR！

### 开发流程
1. Fork本仓库，创建新分支进行开发
2. 遵循PEP8/Python代码规范和注释风格
3. 提交前请确保通过基本测试
4. 有任何建议欢迎在Issue区讨论

### 代码规范
- Python代码遵循PEP8规范
- 使用类型注解
- 添加适当的文档字符串
- 保持代码简洁和可读性

---

## 📄 License

本项目采用 MIT License，详见 [LICENSE](LICENSE) 文件。

---

## 🔮 未来计划 / Roadmap

- [ ] 支持更多设备类型（传感器、安防设备等）
- [ ] 添加Web管理界面
- [ ] 实现设备状态持久化
- [ ] 支持设备分组和场景模式
- [ ] 添加用户认证和权限管理
- [ ] 支持设备固件OTA更新