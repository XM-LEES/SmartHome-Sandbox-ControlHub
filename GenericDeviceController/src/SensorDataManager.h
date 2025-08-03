#ifndef SENSOR_DATA_MANAGER_H
#define SENSOR_DATA_MANAGER_H

#include <Arduino.h>

// 房间索引枚举
enum RoomIndex {
    LIVINGROOM = 0,
    BEDROOM = 1,
    KITCHEN = 2,
    BATHROOM = 3,
    OUTDOOR = 4
};

// 传感器数据结构
struct SensorData {
    float temperature;    // 当前温度
    float humidity;       // 当前湿度
};

// 房间环境数据结构
struct RoomEnvironment {
    SensorData sensors;
    // 预留接口，后续可以添加硬件编码器输入
    // 比如：手动调节旋钮、按键等
};

// 全局变量声明
extern RoomEnvironment rooms[5];

// 接口函数声明
/**
 * @brief 获取指定房间的传感器数据
 * @param room 房间索引
 * @return 传感器数据
 */
SensorData getSensorData(RoomIndex room);

/**
 * @brief 设置指定房间的传感器数据（为后续硬件编码器预留）
 * @param room 房间索引
 * @param temp 温度值
 * @param humidity 湿度值
 */
void setSensorData(RoomIndex room, float temp, float humidity);

/**
 * @brief 初始化传感器数据，设置默认值
 */
void initSensorData();

/**
 * @brief 根据房间ID字符串获取房间索引
 * @param room_id 房间ID字符串
 * @return 房间索引，如果未找到返回-1
 */
int getRoomIndex(const char* room_id);

#endif // SENSOR_DATA_MANAGER_H 