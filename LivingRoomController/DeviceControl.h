#ifndef DEVICE_CONTROL_H
#define DEVICE_CONTROL_H

#include <Arduino.h>

#define LIGHT_PIN 5
#define AC_PIN 18

void setup_devices() {
    pinMode(LIGHT_PIN, OUTPUT);
    pinMode(AC_PIN, OUTPUT);
    Serial.println("[DeviceControl] Devices initialized.");
}

void control_light(bool is_on) {
    digitalWrite(LIGHT_PIN, is_on ? HIGH : LOW);
    Serial.print("[DeviceControl] Light turned ");
    Serial.println(is_on ? "ON" : "OFF");
}

void control_ac(bool is_on, int temperature) {
    digitalWrite(AC_PIN, is_on ? HIGH : LOW);
    Serial.print("[DeviceControl] AC turned ");
    Serial.print(is_on ? "ON" : "OFF");
    if (is_on) {
        Serial.print(", set temperature: ");
        Serial.print(temperature);
    }
    Serial.println();
}

#endif 