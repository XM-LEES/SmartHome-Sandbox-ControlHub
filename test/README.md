# 智能家居API测试脚本

本目录包含智能家居API的测试脚本，用于验证所有设备功能是否正常工作。

## 测试脚本说明

### 1. `api_test.py` - 完整测试脚本

**功能**：
- 测试所有房间的所有设备的所有功能
- 包含正常操作和错误情况的测试
- 生成详细的测试报告
- 保存测试结果到JSON文件

**测试覆盖**：
- **5个房间**：livingroom, bedroom, kitchen, bathroom, outdoor
- **26个设备**：16个物理设备 + 10个传感器
- **所有操作**：ON, OFF, SET_TEMP, READ
- **错误场景**：无效参数、超出范围、不存在的设备等

**使用方法**：
```bash
# 在项目根目录执行
cd /path/to/SmartHome-Sandbox-ControlHub
python test/api_test.py
```

**输出**：
- 实时控制台输出测试进度
- 生成 `test/test_results.json` 测试结果文件

### 2. `quick_test.py` - 快速测试脚本

**功能**：
- 快速测试主要功能
- 适合开发调试使用
- 简单直观的输出

**使用方法**：
```bash
# 在项目根目录执行
python test/quick_test.py
```

## 测试前准备

1. **启动API服务**：
   ```bash
   cd smart_home_api
   python -m uvicorn main:app --reload
   ```

2. **确保ESP32设备在线**（如需测试实际硬件）

3. **安装依赖**：
   ```bash
   pip install requests
   ```

## 测试内容详解

### 开关类设备测试
- **设备**：light, bedside_light, window, door, curtain, fan, hood
- **操作**：ON, OFF
- **错误测试**：无效操作

### 空调设备测试
- **设备**：ac (livingroom, bedroom)
- **正常操作**：
  - ON + 温度值 (0-40°C)
  - SET_TEMP + 温度值 (0-40°C)  
  - OFF
- **错误测试**：
  - 缺少温度参数
  - 温度超出范围 (-1°C, 41°C)
  - 无效操作

### 传感器设备测试
- **设备**：temp_sensor, humidity_sensor
- **操作**：READ
- **错误测试**：无效操作

### 无效请求测试
- 不存在的房间ID
- 不存在的设备ID
- 设备不在指定房间

## 测试结果说明

### 控制台输出格式
```
[时间] INFO: 测试描述
[时间] PASS: ✓ 通过 - 状态码: 200
[时间] FAIL: ✗ 失败 - 期望: 200, 实际: 400
```

### JSON结果文件格式
```json
{
  "summary": {
    "total_tests": 100,
    "passed_tests": 95,
    "failed_tests": 5,
    "success_rate": 95.0
  },
  "details": [
    {
      "test": "测试描述",
      "status": "PASS",
      "details": {
        "status_code": 200,
        "data": {...}
      }
    }
  ],
  "timestamp": "2024-01-01 12:00:00"
}
```

## 常见问题

### 1. 连接失败
```
错误: 无法连接到API服务
```
**解决**：确保API服务正在运行在 `http://127.0.0.1:8000`

### 2. 设备超时
```
状态码: 504
```
**解决**：检查ESP32设备是否在线，MQTT Broker是否运行

### 3. 设备未找到
```
Device error: DEVICE_NOT_FOUND
```
**解决**：检查设备配置，确保设备在正确的节点上

## 自定义测试

可以修改 `api_test.py` 中的配置：

```python
# 修改API地址
API_BASE_URL = "http://your-server:8000"

# 修改超时时间
REQUEST_TIMEOUT = 10

# 修改测试间隔
TEST_DELAY = 1.0
```

## 持续集成

测试脚本支持CI/CD集成：
- 返回值：成功时返回0，有失败测试时返回1
- 可用于自动化测试流水线
- JSON结果文件可用于生成测试报告