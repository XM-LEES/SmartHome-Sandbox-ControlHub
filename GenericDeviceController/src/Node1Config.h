#ifndef NODE_1_CONFIG_H
#define NODE_1_CONFIG_H

#include <Arduino.h>

// =================== 物理节点配置 ===================
// 这个物理节点（ESP32）的唯一标识符，用于MQTT Client ID
const char* NODE_ID = "ESP32_Node_1_Kitchen_Bathroom";

// 1. 定义这个节点控制的设备总数
#define DEVICE_COUNT 16

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
Device devices[DEVICE_COUNT] = {
    // --- 客厅设备 ---
    { "livingroom", "light", 23, false }, // 客厅灯
    { "livingroom", "ac",    22, false }, // 客厅空调
    { "livingroom", "temp_sensor", 0, true }, // 客厅温度传感器（虚拟）
    { "livingroom", "humidity_sensor", 0, true }, // 客厅湿度传感器（虚拟）

    // --- 卧室设备 ---
    { "bedroom", "light", 20, false }, // 卧室灯
    { "bedroom", "bedside_light", 21, false }, // 卧室床头灯
    { "bedroom", "ac",    19, false }, // 卧室空调
    { "bedroom", "temp_sensor", 0, true }, // 卧室温度传感器（虚拟）
    { "bedroom", "humidity_sensor", 0, true }, // 卧室湿度传感器（虚拟）

    // --- 厨房设备 ---
    { "kitchen", "light", 18, false }, // 厨房灯
    { "kitchen", "hood",  5, false },  // 厨房油烟机
    { "kitchen", "temp_sensor", 0, true }, // 厨房温度传感器（虚拟）
    { "kitchen", "humidity_sensor", 0, true }, // 厨房湿度传感器（虚拟）
    
    // --- 卫生间设备 ---
    { "bathroom", "light", 16, false }, // 卫生间灯
    { "bathroom", "fan",   17, false }, // 卫生间排气扇
    { "bathroom", "temp_sensor", 0, true }, // 卫生间温度传感器（虚拟）
    { "bathroom", "humidity_sensor", 0, true }  // 卫生间湿度传感器（虚拟）
};
// ===============================================

#endif // NODE_1_CONFIG_H