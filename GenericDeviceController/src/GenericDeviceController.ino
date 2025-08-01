// GenericDeviceController.ino
// ESP32主程序，处理设备控制与通信逻辑。

// 必要库引用
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Config.h"
#include "Node1Config.h"
#include "DeviceControl.h"

// --- 初始化客户端实例 ---
WiFiClient espClient;
PubSubClient client(espClient);

/**
 * @brief 连接到WiFi网络。
 */
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    // 循环等待，直到WiFi连接成功
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); // 打印获取到的IP地址，方便调试
}

/**
 * @brief 当MQTT连接断开时，尝试重新连接并重新订阅Topic。
 */
void reconnect() {
    // 循环尝试，直到重连成功
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        Serial.print(NODE_ID); Serial.print("'...");

        // 尝试连接到Broker，NODE_ID是这个客户端的唯一ID
        if (client.connect(NODE_ID)) {
            Serial.println("connected!");

            // 动态订阅所有设备的command topic
            for (int i = 0; i < DEVICE_COUNT; i++) {
                char command_topic[128];
                snprintf(command_topic, sizeof(command_topic), "smarthome/%s/%s/command", devices[i].room_id, devices[i].device_id);
                client.subscribe(command_topic);
                Serial.print("Subscribed to: "); Serial.println(command_topic);
            }
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state()); // 打印失败原因代码
            Serial.println(" try again in 5 seconds");
            delay(5000); // 等待5秒后重试
        }
    }
}

/**
 * @brief 发布设备状态，向对应的状态Topic发布执行回执（ACK）。
 * @param device_id 设备ID，用于构建状态Topic
 * @param state 状态字符串，"ON"或"OFF"等
 * @param correlation_id 关联ID，从命令中收到的原始ID用于匹配请求和响应
 */
void publish_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id) {
    // 构造状态Topic的字符串
    char state_topic[128];
    snprintf(state_topic, sizeof(state_topic), "smarthome/%s/%s/state", room_id, device_id);

    // 使用ArduinoJson创建一个JSON对象
    StaticJsonDocument<256> doc;
    doc["state"] = state;
    doc["correlation_id"] = correlation_id;
    // 可以添加更多状态信息，如错误码等
    // doc["error"] = nullptr; 

    // 将JSON对象序列化为字符串
    char buffer[256];
    size_t n = serializeJson(doc, buffer);

    // 发布回执消息
    client.publish(state_topic, buffer, n);
    Serial.print("Published state to "); 
    Serial.print(state_topic);
    Serial.print(": "); Serial.println(buffer);
}

/**
 * @brief 发布传感器状态，包含传感器数据
 * @param room_id 房间ID
 * @param device_id 传感器设备ID
 * @param state 状态字符串，"READ"等
 * @param correlation_id 关联ID
 * @param value 传感器数值
 * @param unit 单位
 */
void publish_sensor_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id, float value, const char* unit) {
    // 构造状态Topic的字符串
    char state_topic[128];
    snprintf(state_topic, sizeof(state_topic), "smarthome/%s/%s/state", room_id, device_id);

    // 使用ArduinoJson创建一个JSON对象
    StaticJsonDocument<256> doc;
    doc["state"] = state;
    doc["correlation_id"] = correlation_id;
    doc["value"] = value;
    doc["unit"] = unit;

    // 将JSON对象序列化为字符串
    char buffer[256];
    size_t n = serializeJson(doc, buffer);

    // 发布回执消息
    client.publish(state_topic, buffer, n);
    Serial.print("Published sensor state to "); 
    Serial.print(state_topic);
    Serial.print(": "); Serial.println(buffer);
}

/**
 * @brief 发送错误回执给上位机
 * @param room_id 房间ID
 * @param device_id 设备ID
 * @param correlation_id 请求关联ID
 * @param error_code 错误代码
 * @param error_message 错误描述
 */
void publish_error_state(const char* room_id, const char* device_id, const char* correlation_id, const char* error_code, const char* error_message) {
    StaticJsonDocument<256> doc;
    doc["state"] = "ERROR";
    doc["correlation_id"] = correlation_id;
    doc["error_code"] = error_code;
    doc["error_message"] = error_message;
    
    char buffer[256];
    serializeJson(doc, buffer);
    
    char state_topic[128];
    snprintf(state_topic, sizeof(state_topic), "smarthome/%s/%s/state", room_id, device_id);
    
    Serial.print("Published error state to ");
    Serial.print(state_topic);
    Serial.print(": "); Serial.println(buffer);
    
    client.publish(state_topic, buffer);
}

/**
 * @brief MQTT消息回调函数。当任何已订阅的Topic收到消息时，此函数会被自动调用。
 *        这是整个下位机的“大脑”。
 * @param topic 收到消息的Topic名称
 * @param payload 消息的具体内容
 * @param length 消息的长度
 */
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("----------");
    Serial.print("Message arrived on topic: ");
    Serial.println(topic);

    // 解析Topic获取房间和设备信息
    // 语法: sscanf(topic, "smarthome/%[^/]/%[^/]/command", room, device);
    // %[^/] 会匹配所有不是'/'的字符，直到遇到'/'或字符串末尾。
    char room[32], device[32];
    if (sscanf(topic, "smarthome/%[^/]/%[^/]/command", room, device) != 2) {
        Serial.println("Error: Topic format does not match 'smarthome/{room}/{device}/command'");
        // 无法解析Topic，无法发送错误回执
        return;
    }

    // 使用ArduinoJson解析收到的JSON payload
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        publish_error_state(room, device, "unknown", "JSON_PARSE_ERROR", "Invalid JSON format");
        return;
    }

    // 从JSON中提取关键信息
    const char* action = doc["action"];
    int value = doc["value"] | 0; // 如果"value"不存在，则默认为0
    const char* correlation_id = doc["correlation_id"];
    
    if (!action || !correlation_id) {
        Serial.println("Error: 'action' or 'correlation_id' missing.");
        publish_error_state(room, device, correlation_id ? correlation_id : "unknown", "MISSING_REQUIRED_FIELDS", "Missing required fields: action or correlation_id");
        return;
    }

    bool is_on = (strcmp(action, "ON") == 0);

    // --- 将解析出的设备类型分发给对应的HAL函数 ---
    bool control_success = false;
    if (strcmp(device, "light") == 0) {
        control_success = control_light(room, is_on);
    } else if (strcmp(device, "ac") == 0) {
        control_success = control_ac(room, is_on, value);
    } else if (strcmp(device, "hood") == 0) {
        control_success = control_hood(room, is_on);
    } else if (strcmp(device, "fan") == 0) {
        control_success = control_fan(room, is_on);
    } else if (strcmp(device, "bedside_light") == 0) {
        control_success = control_light(room, is_on);
    } 
    // 添加其他设备类型的判断...
    // else if (strcmp(device, "oven") == 0) {
    //     control_oven(room, is_on);
    // } 
    else if (strcmp(device, "temp_sensor") == 0) {
        // 温度传感器读取操作
        float temp_value = read_temp_sensor(room);
        Serial.print("[HAL] '"); Serial.print(room);
        Serial.print("/temp_sensor' read: "); Serial.print(temp_value); Serial.println("°C");
        
        // 检查传感器读取是否成功
        if (temp_value == -999.0) {
            // 传感器读取失败，发送错误回执
            publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Temperature sensor read failed");
            return;
        }
        
        // 发送包含传感器数据的回执
        publish_sensor_state(room, device, "READ", correlation_id, temp_value, "°C");
        return; // 已经发送回执，直接返回
    } else if (strcmp(device, "humidity_sensor") == 0) {
        // 湿度传感器读取操作
        float humidity_value = read_humidity_sensor(room);
        Serial.print("[HAL] '"); Serial.print(room);
        Serial.print("/humidity_sensor' read: "); Serial.print(humidity_value); Serial.println("%");
        
        // 检查传感器读取是否成功
        if (humidity_value == -999.0) {
            // 传感器读取失败，发送错误回执
            publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Humidity sensor read failed");
            return;
        }
        
        // 发送包含传感器数据的回执
        publish_sensor_state(room, device, "READ", correlation_id, humidity_value, "%");
        return; // 已经发送回执，直接返回
    } 
    // 添加其他设备类型的判断...
    // else if (strcmp(device, "oven") == 0) {
    //     control_oven(room, is_on);
    // } 
    else {
        Serial.print("Warning: No control logic in .ino for device type '");
        Serial.print(device); Serial.println("'");
        publish_error_state(room, device, correlation_id, "UNKNOWN_DEVICE_TYPE", "Device type not supported");
        return; // 未知设备类型，发送错误回执
    }

    // 检查设备控制是否成功
    if (!control_success) {
        publish_error_state(room, device, correlation_id, "DEVICE_NOT_FOUND", "Device not found in this node's configuration");
        return;
    }
    
    // 如果上面的if/else块执行了，说明是已知设备，发送回执
    publish_state(room, device, action, correlation_id);
}

/**
 * @brief 程序入口和初始化。
 */
void setup() {
    Serial.begin(115200);   // 启动串口，用于调试输出
    setup_devices();        // 初始化硬件设备
    setup_wifi();           // 连接WiFi
    client.setServer(MQTT_SERVER, MQTT_PORT);    // 设置MQTT Broker的地址
    client.setCallback(callback);           // **注册核心的回调函数**
}

/**
 * @brief 主循环。
 */
void loop() {
    // 检查MQTT是否还连接着，如果断了就尝试重连
    if (!client.connected()) {
        reconnect();
    }
    // PubSubClient库的心跳函数，必须在loop中持续调用
    // 它负责处理底层的网络收发和消息检查，并在有新消息时触发我们注册的callback函数
    client.loop();
}