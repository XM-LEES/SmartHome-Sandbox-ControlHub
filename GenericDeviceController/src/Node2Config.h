#ifndef NODE_2_CONFIG_H
#define NODE_2_CONFIG_H

#include <Arduino.h>

// =================== 物理节点配置 ===================
// 这个物理节点（ESP32）的唯一标识符，用于MQTT Client ID
const char* NODE_ID = "ESP32_Node_2";

// =================== 硬件功能配置 ===================
// 根据此节点的实际硬件情况启用/禁用功能模块
#define ENABLE_UI_DISPLAY    true   // 此节点负责UI显示和用户交互

// UI硬件引脚定义（仅在ENABLE_UI_DISPLAY为true时生效）
#if ENABLE_UI_DISPLAY
// TFT屏幕引脚 (ST7735s)
#define TFT_SCLK  18   // SCL - SPI时钟线 (D18, VSPI_CLK原生)
#define TFT_MOSI  23   // SDA - SPI数据线 (D23, VSPI_MOSI原生)
#define TFT_RST   15    // RES - 复位 (D15)
#define TFT_DC    2    // DC  - 数据/命令选择 (D2)
#define TFT_CS    5    // CS  - 片选 (D5, VSPI_CS原生)
#define TFT_BLK   4   // BLK - 背光控制 (D4)

// EC11旋钮编码器引脚
#define EC11_A    12   // A   - 编码器A相
#define EC11_B    14   // B   - 编码器B相
#define EC11_SW   27   // PUSH- 编码器按键

// 独立按键引脚
#define BTN_OK    26   // KO  - OK确认按键
#endif

// 1. 定义这个节点控制的设备总数（主要是虚拟传感器）
#define DEVICE_COUNT 10

// 2. 定义设备结构体，包含room_id，用于支持多个房间
struct Device {
    const char* room_id;
    const char* device_id;
    uint8_t pin;
    bool is_virtual;  // 标记是否为虚拟设备（如传感器）
};

// 3. 初始化此节点控制的所有设备列表
// 需要将GPIO引脚号修改为你实际连接的引脚
// 传感器是虚拟设备，pin设为0，is_virtual设为true
// ⚠️ 重要：设备ID必须与上位机device_registry.py保持完全一致！
// 系统使用字符串比较来匹配设备，任何不一致都会导致控制失败
Device devices[DEVICE_COUNT] = {
    // --- 各房间传感器 ---
    { "livingroom", "temp_sensor", 0, true }, // 客厅温度传感器（虚拟）
    { "livingroom", "humidity_sensor", 0, true }, // 客厅湿度传感器（虚拟）
    
    { "bedroom", "temp_sensor", 0, true }, // 卧室温度传感器（虚拟）
    { "bedroom", "humidity_sensor", 0, true }, // 卧室湿度传感器（虚拟）
    
    { "kitchen", "temp_sensor", 0, true }, // 厨房温度传感器（虚拟）
    { "kitchen", "humidity_sensor", 0, true }, // 厨房湿度传感器（虚拟）
    
    { "bathroom", "temp_sensor", 0, true }, // 卫生间温度传感器（虚拟）
    { "bathroom", "humidity_sensor", 0, true }, // 卫生间湿度传感器（虚拟）
    
    { "outdoor", "temp_sensor", 0, true }, // 室外温度传感器（虚拟）
    { "outdoor", "humidity_sensor", 0, true } // 室外湿度传感器（虚拟）
    // 注意：此节点专注于UI显示和传感器数据，不控制物理设备
};
// ===============================================

#endif // NODE_2_CONFIG_H