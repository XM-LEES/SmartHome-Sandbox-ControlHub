# 通用设备控制器固件 (GenericDeviceController)

本项目是为智能家居沙盘系统设计的通用ESP32固件，通过MQTT协议接收并执行来自上位机的控制指令。

## 🏗️ 架构设计

### 核心思想
- **一个通用程序** + **不同配置文件** = **适配不同硬件的节点**
- 通过条件编译实现硬件功能的启用/禁用
- 编译时优化，无硬件功能的代码完全不会被包含

### 📁 目录结构
```
src/
├── GenericDeviceController.ino    # 主程序
├── Config.h                       # 编译配置
├── README.md                      # 项目文档
├── nodeconfig/                    # 节点配置
│   ├── Node1Config.h             # Node1节点配置
│   └── Node2Config.h             # Node2节点配置
├── core/                          # 核心模块
│   ├── DeviceControl.h           # 设备控制抽象层
│   ├── SensorDataManager.h       # 传感器数据管理
│   └── SensorDataManager.cpp     # 传感器数据实现
├── ui/                           # UI模块
│   ├── UIController.h            # UI控制器
│   └── UIController.cpp          # UI实现
└── doc/                          # 文档
    └── Device_Mapping.md         # 引脚映射文档
```

## 功能特性
- **多设备支持**: 通过配置文件 (`nodeconfig/NodeXConfig.h`) 可灵活定义单个ESP32节点所控制的多个设备及其引脚
- **动态Topic订阅**: 启动时会根据配置文件自动订阅所有相关设备的`command` Topic
- **可靠回执**: 只有在硬件操作**成功执行**后，才会向对应的`state` Topic发布状态回执，以完成闭环控制
- **分层设计**: 代码结构清晰，分为主逻辑 (`.ino`) 和硬件抽象层 (`core/DeviceControl.h`)，便于维护和扩展
- **硬件适配**: 支持有UI屏幕的交互式节点和无UI的纯控制节点

## 📁 节点配置文件

### Node1Config.h - 主控节点（厨房+卫生间）
```cpp
特点：
✅ 包含UI显示屏和编码器
✅ 包含物理控制设备
✅ 包含虚拟传感器
✅ 支持本地交互控制

硬件：
- ST7735 TFT显示屏
- EC11旋转编码器
- 返回按键
- 继电器控制的设备
```

### Node2Config.h - 执行节点（客厅+卧室）
```cpp
特点：
❌ 无UI硬件
✅ 专注物理设备控制
❌ 不包含传感器
✅ 纯MQTT命令执行

硬件：
- 继电器控制的设备
- 无显示屏
- 无用户交互界面
```

## 依赖库
- `ArduinoJson`
- `PubSubClient`
- `WiFi`
- `Adafruit_GFX` & `Adafruit_ST7735` (仅UI节点)

## 🔧 使用方法

### 配置和部署
1. **选择节点**: 在`Config.h`中设置`CURRENT_NODE = 1`或`2`
2. **配置设备**: 修改对应的`nodeconfig/NodeXConfig.h`文件
3. **上传固件**: 编译并上传到ESP32
4. **监控运行**: 通过串口监视器查看状态

## 📋 详细文档

- **设备配置**: 参见 `doc/Device_Mapping.md`
- **引脚映射**: 参见各节点配置文件


