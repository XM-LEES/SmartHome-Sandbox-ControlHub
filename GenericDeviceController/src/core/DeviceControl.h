// DeviceControl.h
// 硬件抽象层（HAL）接口，封装设备引脚操作
// 本文件封装所有与具体硬件引脚相关的操作，向上层代码提供清晰、语义化的函数接口
#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Arduino.h>
#include "SensorDataManager.h"
// 依赖于主程序.ino先包含一个Node_X_Config.h文件，
// 从而获得 `devices` 数组和 `DEVICE_COUNT` 宏的定义。

/**
 * @brief (私有辅助函数) 在设备数组中根据room_id和device_id查找对应的引脚。room_id和device_id定义在Node1Config.h
 * @param room_id 要查找的房间ID。
 * @param device_id 要查找的设备ID。
 * @return 找到则返回引脚号，未找到则返回-1。
 */
 int find_pin(const char* room_id, const char* device_id) {
    for (int i = 0; i < DEVICE_COUNT; i++) {
        if (strcmp(devices[i].room_id, room_id) == 0 && strcmp(devices[i].device_id, device_id) == 0) {
            return devices[i].pin; // 找到匹配的设备，返回其引脚号
        }
    }
    return -1; // 遍历完也未找到
}



/**
 * @brief 初始化设备引脚，设置为输出模式并关闭。
 */
 void setup_devices() {
    Serial.println("[HAL] Initializing all configured devices...");
    for (int i = 0; i < DEVICE_COUNT; i++) {
        if (!devices[i].is_virtual) {
            // 只初始化非虚拟设备（有物理引脚的设备）
            pinMode(devices[i].pin, OUTPUT);
            digitalWrite(devices[i].pin, LOW);
        }
    }
    Serial.println("[HAL] All physical devices initialized and turned OFF.");
}

/**
 * @brief 控制指定房间的灯的开关。
 * @param room_id 灯所在的房间ID。
 * @param is_on true为开，false为关。
 * @return true表示成功，false表示失败
 */
 bool control_light(const char* room_id, bool is_on) {
    int pin = find_pin(room_id, "light"); // 硬编码device_id为"light"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/light' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.println(is_on ? "ON" : "OFF");
        return true; // 成功
    } else {
        Serial.print("[HAL-ERROR] Device 'light' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false; // 失败
    }
}

/**
 * @brief 控制指定房间的空调开关。
 * @param room_id 空调所在的房间ID。
 * @param is_on true为开，false为关。
 * @param temperature 设定温度（仅用于打印输出）。
 * @return true表示成功，false表示失败
 */
 bool control_ac(const char* room_id, bool is_on, int temperature) {
    int pin = find_pin(room_id, "ac"); // 硬编码device_id为"ac"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/ac' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.print(is_on ? "ON" : "OFF");
        if (is_on) {
            Serial.print(", set temp to: "); Serial.print(temperature);
        }
        Serial.println();
        return true; // 成功
    } else {
        Serial.print("[HAL-ERROR] Device 'ac' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false; // 失败
    }
}

/**
 * @brief 控制指定房间的油烟机开关。
 * @param room_id 油烟机所在的房间ID。
 * @param is_on true为开，false为关。
 * @return true表示成功，false表示失败
 */
bool control_hood(const char* room_id, bool is_on) {
    int pin = find_pin(room_id, "hood"); // 硬编码device_id为"hood"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/hood' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.println(is_on ? "ON" : "OFF");
        return true; // 成功
    } else {
        Serial.print("[HAL-ERROR] Device 'hood' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false; // 失败
    }
}

/**
 * @brief 控制指定房间的排气扇开关。
 * @param room_id 排气扇所在的房间ID。
 * @param is_on true为开，false为关。
 * @return true表示成功，false表示失败
 */
bool control_fan(const char* room_id, bool is_on) {
    int pin = find_pin(room_id, "fan"); // 硬编码device_id为"fan"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/fan' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.println(is_on ? "ON" : "OFF");
        return true; // 成功
    } else {
        Serial.print("[HAL-ERROR] Device 'fan' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false; // 失败
    }
}



/**
 * @brief 控制指定房间的窗户开关。
 * @param room_id 窗户所在的房间ID。
 * @param is_on true为开，false为关。
 * @return true表示成功，false表示失败
 */
bool control_window(const char* room_id, bool is_on) {
    int pin = find_pin(room_id, "window"); // 硬编码device_id为"window"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/window' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.println(is_on ? "ON" : "OFF");
        return true; // 成功
    } else {
        Serial.print("[HAL-ERROR] Device 'window' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false; // 失败
    }
}

/**
 * @brief 控制指定房间的门开关。
 * @param room_id 门所在的房间ID。
 * @param is_on true为开，false为关。
 * @return true表示成功，false表示失败
 */
bool control_door(const char* room_id, bool is_on) {
    int pin = find_pin(room_id, "door"); // 硬编码device_id为"door"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/door' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.println(is_on ? "ON" : "OFF");
        return true; // 成功
    } else {
        Serial.print("[HAL-ERROR] Device 'door' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false; // 失败
    }
}

/**
 * @brief 控制指定房间的窗帘开关。
 * @param room_id 窗帘所在的房间ID。
 * @param is_on true为开，false为关。
 * @return true表示成功，false表示失败
 */
bool control_curtain(const char* room_id, bool is_on) {
    int pin = find_pin(room_id, "curtain"); // 硬编码device_id为"curtain"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/curtain' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.println(is_on ? "ON" : "OFF");
        return true; // 成功
    } else {
        Serial.print("[HAL-ERROR] Device 'curtain' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false; // 失败
    }
}

// ... 可以根据需要，在这里添加更多 `control_` 系列函数。

#endif // DEVICE_CONTROL_H
