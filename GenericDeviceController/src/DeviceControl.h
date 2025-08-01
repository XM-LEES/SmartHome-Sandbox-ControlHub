// DeviceControl.h
// 硬件抽象层（HAL）接口，封装设备引脚操作
// 本文件封装所有与具体硬件引脚相关的操作，向上层代码提供清晰、语义化的函数接口
#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Arduino.h>
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
 * @brief 读取温度传感器数据（虚拟传感器）
 * @param room_id 传感器所在的房间ID。
 * @return 温度值（摄氏度），如果读取失败返回-999.0
 */
float read_temp_sensor(const char* room_id) {
    // 模拟传感器读取可能失败的情况
    // 在实际应用中，这里应该检查传感器是否响应、数据是否有效等
    if (random(100) < 5) { // 5%的概率模拟传感器故障
        return -999.0; // 特殊值表示读取失败
    }
    
    // 模拟不同房间的温度数据
    if (strcmp(room_id, "livingroom") == 0) {
        return 24.5 + random(-5, 6) * 0.1; // 24.0-25.0°C
    } else if (strcmp(room_id, "bedroom") == 0) {
        return 23.8 + random(-5, 6) * 0.1; // 23.3-24.4°C
    } else if (strcmp(room_id, "kitchen") == 0) {
        return 26.1 + random(-5, 6) * 0.1; // 25.6-26.7°C
    } else if (strcmp(room_id, "bathroom") == 0) {
        return 25.3 + random(-5, 6) * 0.1; // 24.8-25.9°C
    } else if (strcmp(room_id, "outdoor") == 0) {
        return 20.0 + random(-10, 11) * 0.1; // 19.0-21.1°C
    } else {
        return -999.0; // 未知房间，返回错误值
    }
}

/**
 * @brief 读取湿度传感器数据（虚拟传感器）
 * @param room_id 传感器所在的房间ID。
 * @return 湿度值（百分比），如果读取失败返回-999.0
 */
float read_humidity_sensor(const char* room_id) {
    // 模拟传感器读取可能失败的情况
    if (random(100) < 5) { // 5%的概率模拟传感器故障
        return -999.0; // 特殊值表示读取失败
    }
    
    // 模拟不同房间的湿度数据
    if (strcmp(room_id, "livingroom") == 0) {
        return 45.2 + random(-10, 11) * 0.2; // 43.2-47.4%
    } else if (strcmp(room_id, "bedroom") == 0) {
        return 48.5 + random(-10, 11) * 0.2; // 46.5-50.7%
    } else if (strcmp(room_id, "kitchen") == 0) {
        return 52.3 + random(-10, 11) * 0.2; // 50.3-54.5%
    } else if (strcmp(room_id, "bathroom") == 0) {
        return 65.8 + random(-10, 11) * 0.2; // 63.8-68.0%
    } else if (strcmp(room_id, "outdoor") == 0) {
        return 60.0 + random(-20, 21) * 0.2; // 56.0-64.2%
    } else {
        return -999.0; // 未知房间，返回错误值
    }
}

// ... 可以根据需要，在这里添加更多 `control_` 系列函数。

#endif // DEVICE_CONTROL_H
