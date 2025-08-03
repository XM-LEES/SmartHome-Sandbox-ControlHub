#include "SensorDataManager.h"

// 全局变量定义
RoomEnvironment rooms[5];

/**
 * @brief 获取指定房间的传感器数据
 * @param room 房间索引
 * @return 传感器数据
 */
SensorData getSensorData(RoomIndex room) {
    if (room >= 0 && room < 5) {
        return rooms[room].sensors;
    }
    // 返回错误数据
    SensorData error_data = {-999.0, -999.0};
    return error_data;
}

/**
 * @brief 设置指定房间的传感器数据（为后续硬件编码器预留）
 * @param room 房间索引
 * @param temp 温度值
 * @param humidity 湿度值
 */
void setSensorData(RoomIndex room, float temp, float humidity) {
    if (room >= 0 && room < 5) {
        rooms[room].sensors.temperature = temp;
        rooms[room].sensors.humidity = humidity;
        Serial.print("[SensorDataManager] Updated room ");
        Serial.print(room);
        Serial.print(": temp=");
        Serial.print(temp);
        Serial.print("°C, humidity=");
        Serial.print(humidity);
        Serial.println("%");
    }
}

/**
 * @brief 初始化传感器数据，设置默认值
 */
void initSensorData() {
    Serial.println("[SensorDataManager] Initializing sensor data...");
    
    // 客厅默认数据
    rooms[LIVINGROOM].sensors.temperature = 24.5;
    rooms[LIVINGROOM].sensors.humidity = 45.2;
    
    // 卧室默认数据
    rooms[BEDROOM].sensors.temperature = 23.8;
    rooms[BEDROOM].sensors.humidity = 48.5;
    
    // 厨房默认数据
    rooms[KITCHEN].sensors.temperature = 26.1;
    rooms[KITCHEN].sensors.humidity = 52.3;
    
    // 浴室默认数据
    rooms[BATHROOM].sensors.temperature = 25.3;
    rooms[BATHROOM].sensors.humidity = 65.8;
    
    // 室外默认数据
    rooms[OUTDOOR].sensors.temperature = 20.0;
    rooms[OUTDOOR].sensors.humidity = 60.0;
    
    Serial.println("[SensorDataManager] Sensor data initialized successfully");
}

/**
 * @brief 根据房间ID字符串获取房间索引
 * @param room_id 房间ID字符串
 * @return 房间索引，如果未找到返回-1
 */
int getRoomIndex(const char* room_id) {
    if (strcmp(room_id, "livingroom") == 0) {
        return LIVINGROOM;
    } else if (strcmp(room_id, "bedroom") == 0) {
        return BEDROOM;
    } else if (strcmp(room_id, "kitchen") == 0) {
        return KITCHEN;
    } else if (strcmp(room_id, "bathroom") == 0) {
        return BATHROOM;
    } else if (strcmp(room_id, "outdoor") == 0) {
        return OUTDOOR;
    } else {
        return -1; // 未知房间
    }
} 