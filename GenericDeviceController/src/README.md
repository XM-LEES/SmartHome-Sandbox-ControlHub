# 通用设备控制器固件 (GenericDeviceController)

本项目是为智能家居沙盘系统设计的通用ESP32固件，通过MQTT协议接收并执行来自上位机的控制指令。

## 🏗️ 架构设计

### 核心思想
- **一个通用程序** + **不同配置文件** = **适配不同硬件的节点**
- 通过条件编译实现硬件功能的启用/禁用
- 编译时优化，无硬件功能的代码完全不会被包含

## 功能特性
- **多设备支持**: 通过配置文件 (`NodeXConfig.h`) 可灵活定义单个ESP32节点所控制的多个设备及其引脚
- **动态Topic订阅**: 启动时会根据配置文件自动订阅所有相关设备的`command` Topic
- **可靠回执**: 只有在硬件操作**成功执行**后，才会向对应的`state` Topic发布状态回执，以完成闭环控制
- **分层设计**: 代码结构清晰，分为主逻辑 (`.ino`) 和硬件抽象层 (`DeviceControl.h`)，便于维护和扩展
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

### 编译和部署

#### 编译Node1（主控节点）
```cpp
// 在 GenericDeviceController.ino 中修改include
#include "Node1Config.h"
```

#### 编译Node2（执行节点）
```cpp
// 在 GenericDeviceController.ino 中修改include  
#include "Node2Config.h"
```

### 配置步骤
1.  **配置设备映射**: 修改相应的`NodeXConfig.h`文件，定义此ESP32节点所控制的所有设备
2.  **修改配置**: 在`Config.h`中修改WiFi凭据和MQTT Broker的IP地址
3.  **上传固件**: 使用 Arduino IDE 或 PlatformIO 将代码编译并上传到ESP32
4.  **监控**: 通过串口监视器查看设备的连接状态、收到的命令和执行日志

## ⚙️ 配置文件结构

每个NodeXConfig.h文件包含：

```cpp
// 1. 节点标识
const char* NODE_ID = "ESP32_Node_X";

// 2. 硬件功能开关
#define ENABLE_UI_DISPLAY    true/false
#define ENABLE_PHYSICAL_ENCODER true/false

// 3. 硬件引脚定义（条件编译）
#if ENABLE_UI_DISPLAY
// UI相关引脚定义
#endif

// 4. 设备列表
Device devices[DEVICE_COUNT] = {
    // 该节点控制的所有设备
};
```

## 🎯 节点分工

| 节点 | 功能 | 硬件 | 设备数量 | 备注 |
|------|------|------|----------|------|
| Node1 | 主控+厨房卫生间 | UI屏幕+设备控制 | 26个 | 用户交互中心 |
| Node2 | 客厅卧室控制 | 纯设备控制 | 10个 | 执行节点 |
| Node3 | (预留)传感器节点 | 传感器采集 | TBD | 数据采集中心 |

## 🔄 扩展新节点

1. 复制现有配置文件（如`Node1Config.h`）
2. 修改NODE_ID为新的节点标识
3. 设置功能开关（ENABLE_XXX）
4. 定义该节点负责的设备列表
5. 修改主程序include语句指向新配置文件

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
    - 命令中的房间ID不在支持的房间列表中。
    - 命令中的设备是此节点不负责的。
    - 硬件执行函数返回`false`（表示执行失败）。
    - 传感器读取失败（数据无效）。

这允许上位机区分**设备超时**（可能离线）和**设备明确报告的错误**（在线但执行失败），从而返回更精确的HTTP错误码（如 `502 Bad Gateway`）给最终用户。

## ⚠️ 重要注意事项

1. **设备ID一致性**：所有配置文件中的device_id必须与上位机`device_registry.py`保持一致
2. **引脚冲突**：不同节点的引脚分配要避免冲突，详见`Device_Mapping.md`
3. **编译优化**：未启用的功能代码完全不会被编译，节省Flash和RAM资源
4. **MQTT Topic**：每个节点订阅自己负责设备的command topic
5. **网络配置**：确保所有ESP32节点与树莓派在同一网络内
6. **UI硬件检测**：无UI硬件的节点不会尝试初始化显示屏，避免启动失败

## 🚀 架构优势

- ✅ **代码复用率高**：一套代码适配多种硬件组合
- ✅ **硬件适配灵活**：通过配置文件轻松支持不同设备组合
- ✅ **编译时优化**：无用功能代码不会被包含，节省资源
- ✅ **维护成本低**：统一的代码结构，易于调试和升级
- ✅ **扩展性强**：新增节点只需添加配置文件
- ✅ **故障隔离**：单个节点故障不影响其他节点运行
- ✅ **分布式控制**：支持多ESP32协同工作，提高系统可靠性

## 📋 相关文档

- **设备映射表**: [Device_Mapping.md](Device_Mapping.md) - 详细的引脚和设备对应关系
- **项目总体文档**: [../../README.md](../../README.md) - 完整系统架构和部署指南
- **API接口文档**: [../../smart_home_api/API_Documentation.md](../../smart_home_api/API_Documentation.md) - 上位机API使用说明
