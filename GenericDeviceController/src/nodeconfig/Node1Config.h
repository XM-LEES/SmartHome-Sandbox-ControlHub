#ifndef NODE_1_CONFIG_H
#define NODE_1_CONFIG_H

#include <Arduino.h>

// =================== Node1特定配置 ===================
// =================== WiFi配置 ===================
// 替换为实际的WiFi SSID和密码
const char* WIFI_SSID = "IOT-Router";
const char* WIFI_PASSWORD = "1234567890";
// =================== MQTT配置 ===================
// 替换为实际的MQTT Broker地址（树莓派IP地址）
const char* MQTT_SERVER = "192.168.3.100";
const int MQTT_PORT = 1883;
// 这个物理节点（ESP32）的唯一标识符，用于MQTT Client ID
const char* NODE_ID = "ESP32_Node_1";

// 1. 定义这个节点控制的设备总数（仅物理设备）
#define DEVICE_COUNT 14

// 2. 定义设备结构体，包含room_id，用于支持多个房间
struct Device {
    const char* room_id;
    const char* device_id;
    uint8_t pin;
    bool is_virtual;  // 标记是否为虚拟设备（如传感器）
};

// 3. 初始化此节点控制的所有设备列表
// 需要将GPIO引脚号修改为你实际连接的引脚
// 虚拟设备（传感器）的pin设为0，is_virtual设为true
// ⚠️ 重要：设备ID必须与上位机device_registry.py保持完全一致！
// 系统使用字符串比较来匹配设备，任何不一致都会导致控制失败
Device devices[DEVICE_COUNT] = {
    // --- 客厅设备 ---
    { "livingroom", "light", 22, false }, // 客厅灯
    // { "livingroom", "ac",    21, false }, // 客厅空调
    { "livingroom", "window", 19, false }, // 客厅窗户
    { "livingroom", "door", 17, false }, // 客厅门
    { "livingroom", "curtain", 16, false }, // 客厅窗帘

    // --- 卧室设备 ---
    { "bedroom", "light", 4, false }, // 卧室灯
    { "bedroom", "bedside_light", 2, false }, // 卧室床头灯
    // { "bedroom", "ac",    34, false }, // 卧室空调
    { "bedroom", "window", 32, false }, // 卧室窗户
    { "bedroom", "door", 33, false }, // 卧室门
    { "bedroom", "curtain", 25, false }, // 卧室窗帘

    // --- 厨房设备 ---
    { "kitchen", "light", 26, false }, // 厨房灯
    { "kitchen", "hood",  27, false },  // 厨房油烟机
    
    // --- 卫生间设备 ---
    { "bathroom", "light", 14, false }, // 卫生间灯
    { "bathroom", "fan",   13, false }, // 卫生间排气扇
    { "bathroom", "door", 12, false } // 卫生间门
};
// ===============================================

#endif // NODE_1_CONFIG_H