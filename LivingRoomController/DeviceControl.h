// DeviceControl.h
// 硬件抽象层（HAL）接口，封装设备引脚操作。
// 本文件封装所有与具体硬件引脚相关的操作，向上层代码提供清晰、语义化的函数接口，硬件引脚发生变化时只需要修改本文件
#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Arduino.h>

// 设备引脚定义
#define LIVING_ROOM_LIGHT_PIN 5  // 客厅灯GPIO 5
#define LIVING_ROOM_AC_PIN    18 // 客厅空调GPIO 18

/**
 * @brief 初始化设备引脚，设置为输出模式并关闭。
 */
void setup_devices() {
    pinMode(LIVING_ROOM_LIGHT_PIN, OUTPUT);
    pinMode(LIVING_ROOM_AC_PIN, OUTPUT);
    
    digitalWrite(LIVING_ROOM_LIGHT_PIN, LOW);
    digitalWrite(LIVING_ROOM_AC_PIN, LOW);
    
    // 通过串口打印日志，方便调试，确认HAL层已正确初始化
    Serial.println("[DeviceControl] All devices initialized.");
}

/**
 * @brief 控制客厅灯开关。
 * @param is_on true为开，false为关。
 */
void control_light(bool is_on) {
    digitalWrite(LIVING_ROOM_LIGHT_PIN, is_on ? HIGH : LOW);
    
    // 打印日志，输出执行了什么操作
    Serial.print("[DeviceControl] Living room light turned ");
    Serial.println(is_on ? "ON" : "OFF");
}

/**
 * @brief 控制客厅空调开关及温度。
 * @param is_on true为开，false为关。
 * @param temperature 设定温度（仅用于打印输出，不实际控制）。
 */
void control_ac(bool is_on, int temperature) {
    digitalWrite(LIVING_ROOM_AC_PIN, is_on ? HIGH : LOW);
    
    // 打印日志，输出执行了什么操作
    Serial.print("[DeviceControl] Living room AC turned ");
    Serial.print(is_on ? "ON" : "OFF");
    if (is_on) {
        Serial.print(", set temperature to: ");
        Serial.print(temperature);
    }
    Serial.println();
}

#endif // DEVICE_CONTROL_H
