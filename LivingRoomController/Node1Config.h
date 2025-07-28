#ifndef NODE_1_CONFIG_H
#define NODE_1_CONFIG_H

#include <Arduino.h>

// =================== 物理节点配置 ===================
// 这个物理节点（ESP32）的唯一标识符，用于MQTT Client ID
const char* NODE_ID = "ESP32_Node_1_Kitchen_Bathroom";

// 1. 定义这个节点控制的设备总数
#define DEVICE_COUNT 4

// 2. 定义设备结构体，包含room_id，用于支持多个房间
struct Device {
    const char* room_id;
    const char* device_id;
    uint8_t pin;
};

// 3. 初始化此节点控制的所有设备列表
// 需要将GPIO引脚号修改为你实际连接的引脚
Device devices[DEVICE_COUNT] = {
    // 厨房的设备
    { "kitchen", "light", 33 }, // 厨房灯
    { "kitchen", "hood",  32 }, // 厨房油烟机
    { "kitchen", "oven",  15 }, // 厨房烤箱

    // 浴室的设备
    { "bathroom", "light", 4 }  // 浴室灯
};
// ===============================================

#endif // NODE_1_CONFIG_H