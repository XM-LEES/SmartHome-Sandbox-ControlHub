#ifndef CONFIG_H
#define CONFIG_H

// =================== WiFi配置 ===================
// 替换为实际的WiFi SSID和密码
const char* WIFI_SSID = "PDCN";
const char* WIFI_PASSWORD = "1234567890";

// =================== MQTT配置 ===================
// 替换为实际的MQTT Broker地址（树莓派IP地址）
const char* MQTT_SERVER = "192.168.123.210";
const int MQTT_PORT = 1883;

// =================== 开发环境配置 ===================
// 如果需要切换到开发环境，取消注释下面的配置并注释上面的配置
// const char* WIFI_SSID = "YOUR_WIFI_SSID";
// const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";
// const char* MQTT_SERVER = "192.168.1.100";

#endif // CONFIG_H 