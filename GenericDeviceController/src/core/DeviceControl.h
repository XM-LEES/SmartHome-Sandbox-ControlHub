// DeviceControl.h
// 硬件抽象层（HAL）接口，封装设备引脚操作
// 本文件封装所有与具体硬件引脚相关的操作，向上层代码提供清晰、语义化的函数接口
#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Arduino.h>

// =================== 空调状态管理 ===================
struct AirConditionerState {
    bool is_on;              // 空调是否开启
    int target_temperature;  // 目标温度
};

// 手动定义空调状态数组 - 按房间索引
// 0: livingroom, 1: bedroom, 2: kitchen, 3: bathroom
AirConditionerState ac_states[4] = {
    {false, 24},  // livingroom 空调：关闭，默认24度
    {false, 24},  // bedroom 空调：关闭，默认24度  
    {false, 26},  // kitchen 空调：关闭，默认26度（厨房稍热）
    {false, 25}   // bathroom 空调：关闭，默认25度
};

// 获取房间对应的数组索引
int get_room_index(const char* room_id) {
    if (strcmp(room_id, "livingroom") == 0) return 0;
    if (strcmp(room_id, "bedroom") == 0) return 1;
    if (strcmp(room_id, "kitchen") == 0) return 2;
    if (strcmp(room_id, "bathroom") == 0) return 3;
    return -1;  // 未知房间
}

// 获取空调状态
AirConditionerState* get_ac_state(const char* room_id) {
    int index = get_room_index(room_id);
    if (index >= 0 && index < 4) {
        return &ac_states[index];
    }
    return nullptr;
}

/**
 * @brief (私有辅助函数) 在设备数组中根据room_id和device_id查找对应的引脚。room_id和device_id定义在Node1Config.h
 * @param room_id 要查找的房间ID。
 * @param device_id 要查找的设备ID。
 * @return 找到则返回引脚号，未找到则返回-1。
 */
 int find_pin(const char* room_id, const char* device_id) {
    for (int i = 0; i < DEVICE_COUNT; i++) {
        if (strcmp(devices[i].room_id, room_id) == 0 && strcmp(devices[i].device_id, device_id) == 0) {
            return devices[i].pin;
        }
    }
    return -1;
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
    int pin = find_pin(room_id, "light");
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/light' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.println(is_on ? "ON" : "OFF");
        return true;
    } else {
        Serial.print("[HAL-ERROR] Device 'light' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false;
    }
}

/**
 * @brief 专门处理空调温度设置。
 * @param room_id 空调所在的房间ID。
 * @param temperature 目标温度。
 * @return true表示成功，false表示失败
 */
bool control_ac_set_temperature(const char* room_id, int temperature) {
    // 温度范围检查
    if (temperature < 0 || temperature > 40) {
        Serial.print("[HAL-ERROR] Invalid temperature: "); Serial.print(temperature);
        Serial.println(". Valid range: 0-40°C");
        return false;
    }
    
    // 获取空调状态
    AirConditionerState* state = get_ac_state(room_id);
    if (state == nullptr) {
        Serial.print("[HAL-ERROR] Room '"); Serial.print(room_id);
        Serial.println("' not found for AC temperature setting");
        return false;
    }
    
    // 更新目标温度
    state->target_temperature = temperature;
    
    // 这里可以添加实际的硬件控制逻辑（如红外发射、串口通信等）
    Serial.print("[HAL] AC in "); Serial.print(room_id);
    Serial.print(" temperature set to: "); Serial.print(temperature);
    Serial.println("°C");
    
    return true;
}

/**
 * @brief 控制指定房间的空调开关。
 * @param room_id 空调所在的房间ID。
 * @param is_on true为开，false为关。
 * @param temperature 设定温度（用于ON操作时更新目标温度）。
 * @return true表示成功，false表示失败
 */
 bool control_ac(const char* room_id, bool is_on, int temperature) {
    int pin = find_pin(room_id, "ac"); // 硬编码device_id为"ac"
    if (pin != -1) {
        digitalWrite(pin, is_on ? HIGH : LOW);
        
        // 更新状态
        AirConditionerState* state = get_ac_state(room_id);
        if (state != nullptr) {
            state->is_on = is_on;
            if (is_on && temperature >= 0 && temperature <= 40) {
                // ON操作时必须设置提供的温度值
                state->target_temperature = temperature;
            } else if (!is_on) {
                // OFF操作时保持当前温度设置
            }
        }
        
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/ac' (Pin "); Serial.print(pin);
        Serial.print(") turned "); Serial.print(is_on ? "ON" : "OFF");
        if (is_on && state != nullptr) {
            Serial.print(", temp: "); Serial.print(state->target_temperature); Serial.print("°C");
        }
        Serial.println();
        return true;
    } else {
        Serial.print("[HAL-ERROR] Device 'ac' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false;
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
        return true;
    } else {
        Serial.print("[HAL-ERROR] Device 'hood' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false;
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
        return true;
    } else {
        Serial.print("[HAL-ERROR] Device 'fan' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false;
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
        return true;
    } else {
        Serial.print("[HAL-ERROR] Device 'window' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false;
    }
}

/*
// 门设备控制功能已禁用
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
        return true;
    } else {
        Serial.print("[HAL-ERROR] Device 'door' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false;
    }
}
*/

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
        return true;
    } else {
        Serial.print("[HAL-ERROR] Device 'curtain' not found in room '");
        Serial.print(room_id); Serial.println("' for this node's config!");
        return false;
    }
}

// ... 可以根据需要，在这里添加更多 `control_` 系列函数。

// =================== 传感器控制函数 ===================
#if ENABLE_SENSOR_SIMULATOR
    #include "../sensorsimulator/SensorDataManager.h"

    /**
     * @brief 控制温度传感器读取
     * @param room_id 房间ID
     * @return 温度值，-999.0表示读取失败
     */
    float control_temperature_sensor(const char* room_id) {
        int room_index = getRoomIndex(room_id);
        if (room_index == -1) {
            Serial.print("[HAL-ERROR] Unknown room for temp sensor: "); Serial.println(room_id);
            return -999.0;
        }
        
        SensorData sensor_data = getSensorData((RoomIndex)room_index);
        float temp_value = sensor_data.temperature;
        
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/temp_sensor' read: "); Serial.print(temp_value); Serial.println("°C");
        
        return temp_value;
    }

    /**
     * @brief 控制湿度传感器读取
     * @param room_id 房间ID
     * @return 湿度值，-999.0表示读取失败
     */
    float control_humidity_sensor(const char* room_id) {
        int room_index = getRoomIndex(room_id);
        if (room_index == -1) {
            Serial.print("[HAL-ERROR] Unknown room for humidity sensor: "); Serial.println(room_id);
            return -999.0;
        }
        
        SensorData sensor_data = getSensorData((RoomIndex)room_index);
        float humidity_value = sensor_data.humidity;
        
        Serial.print("[HAL] '"); Serial.print(room_id);
        Serial.print("/humidity_sensor' read: "); Serial.print(humidity_value); Serial.println("%");
        
        return humidity_value;
    }

#else
    // 无传感器支持的存根实现
    /**
     * @brief 温度传感器存根实现（无传感器支持时）
     * @param room_id 房间ID
     * @return 始终返回-999.0表示不支持
     */
    float control_temperature_sensor(const char* room_id) {
        Serial.println("[HAL-ERROR] Temperature sensor not supported on this node");
        return -999.0;
    }

    /**
     * @brief 湿度传感器存根实现（无传感器支持时）
     * @param room_id 房间ID
     * @return 始终返回-999.0表示不支持
     */
    float control_humidity_sensor(const char* room_id) {
        Serial.println("[HAL-ERROR] Humidity sensor not supported on this node");
        return -999.0;
    }
#endif

#endif // DEVICE_CONTROL_H
