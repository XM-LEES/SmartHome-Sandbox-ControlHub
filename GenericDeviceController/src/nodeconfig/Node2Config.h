#ifndef NODE_2_CONFIG_H
#define NODE_2_CONFIG_H

#include <Arduino.h>

// =================== Node2特定配置 ===================
// =================== WiFi配置 ===================
// 替换为实际的WiFi SSID和密码
const char* WIFI_SSID = "IOT-Router";
const char* WIFI_PASSWORD = "1234567890";
// =================== MQTT配置 ===================
// 替换为实际的MQTT Broker地址（树莓派IP地址）
const char* MQTT_SERVER = "192.168.3.100";
const int MQTT_PORT = 1883;
// 这个物理节点（ESP32）的唯一标识符，用于MQTT Client ID
const char* NODE_ID = "ESP32_Node_2";

// 1. 定义这个节点控制的设备总数（主要是虚拟传感器）
#define DEVICE_COUNT 15

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
    { "livingroom", "brightness_sensor", 0, true }, // 客厅亮度传感器（虚拟）
    { "livingroom", "ac",    21, false }, // 客厅空调

    { "bedroom", "temp_sensor", 0, true }, // 卧室温度传感器（虚拟）
    { "bedroom", "humidity_sensor", 0, true }, // 卧室湿度传感器（虚拟）
    { "bedroom", "brightness_sensor", 0, true }, // 卧室亮度传感器（虚拟）
    { "bedroom", "ac",    34, false }, // 卧室空调
    
    { "kitchen", "temp_sensor", 0, true }, // 厨房温度传感器（虚拟）
    { "kitchen", "humidity_sensor", 0, true }, // 厨房湿度传感器（虚拟）
    
    { "bathroom", "temp_sensor", 0, true }, // 卫生间温度传感器（虚拟）
    { "bathroom", "humidity_sensor", 0, true }, // 卫生间湿度传感器（虚拟）
    
    { "outdoor", "temp_sensor", 0, true }, // 室外温度传感器（虚拟）
    { "outdoor", "humidity_sensor", 0, true }, // 室外湿度传感器（虚拟）
    { "outdoor", "brightness_sensor", 0, true } // 室外亮度传感器（虚拟）
    // 注意：此节点专注于UI显示和传感器数据，不控制物理设备
};
// ===============================================

#endif // NODE_2_CONFIG_H