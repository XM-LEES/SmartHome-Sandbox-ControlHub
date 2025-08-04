# 1 "C:\\Users\\Ming\\AppData\\Local\\Temp\\tmp0f1wrgf_"
#include <Arduino.h>
# 1 "E:/Projects/Lab/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"




#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "Config.h"

#if CURRENT_NODE == 1
    #include "nodeconfig/Node1Config.h"
#elif CURRENT_NODE == 2
    #include "nodeconfig/Node2Config.h"
#endif

#include "core/DeviceControl.h"
#include "core/SensorDataManager.h"
#include "ui/UIController.h"


WiFiClient espClient;
PubSubClient client(espClient);


UIController uiController;
void setup_wifi();
void reconnect();
void publish_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id);
void publish_sensor_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id, float value, const char* unit);
void publish_error_state(const char* room_id, const char* device_id, const char* correlation_id, const char* error_code, const char* error_message);
void callback(char* topic, byte* payload, unsigned int length);
void setup();
void loop();
#line 30 "E:/Projects/Lab/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to WiFi: ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}




void reconnect() {

    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        Serial.print(NODE_ID); Serial.print("'...");


        if (client.connect(NODE_ID)) {
            Serial.println("connected!");


            for (int i = 0; i < DEVICE_COUNT; i++) {
                char command_topic[128];
                snprintf(command_topic, sizeof(command_topic), "smarthome/%s/%s/command", devices[i].room_id, devices[i].device_id);
                client.subscribe(command_topic);
                Serial.print("Subscribed to: "); Serial.println(command_topic);
            }
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}







void publish_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id) {

    char state_topic[128];
    snprintf(state_topic, sizeof(state_topic), "smarthome/%s/%s/state", room_id, device_id);


    StaticJsonDocument<256> doc;
    doc["state"] = state;
    doc["correlation_id"] = correlation_id;




    char buffer[256];
    size_t n = serializeJson(doc, buffer);


    client.publish(state_topic, buffer, n);
    Serial.print("Published state to ");
    Serial.print(state_topic);
    Serial.print(": "); Serial.println(buffer);
}
# 113 "E:/Projects/Lab/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"
void publish_sensor_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id, float value, const char* unit) {

    char state_topic[128];
    snprintf(state_topic, sizeof(state_topic), "smarthome/%s/%s/state", room_id, device_id);


    StaticJsonDocument<256> doc;
    doc["state"] = state;
    doc["correlation_id"] = correlation_id;
    doc["value"] = value;
    doc["unit"] = unit;


    char buffer[256];
    size_t n = serializeJson(doc, buffer);


    client.publish(state_topic, buffer, n);
    Serial.print("Published sensor state to ");
    Serial.print(state_topic);
    Serial.print(": "); Serial.println(buffer);
}
# 144 "E:/Projects/Lab/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"
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







void callback(char* topic, byte* payload, unsigned int length) {
    Serial.println("----------");
    Serial.print("Message arrived on topic: ");
    Serial.println(topic);


    char room[32], device[32];
    if (sscanf(topic, "smarthome/%[^/]/%[^/]/command", room, device) != 2) {
        Serial.println("Error: Topic format does not match 'smarthome/{room}/{device}/command'");

        return;
    }


    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        publish_error_state(room, device, "unknown", "JSON_PARSE_ERROR", "Invalid JSON format");
        return;
    }


    const char* action = doc["action"];
    int value = doc["value"] | 0;
    const char* correlation_id = doc["correlation_id"];

    if (!action || !correlation_id) {
        Serial.println("Error: 'action' or 'correlation_id' missing.");
        publish_error_state(room, device, correlation_id ? correlation_id : "unknown", "MISSING_REQUIRED_FIELDS", "Missing required fields: action or correlation_id");
        return;
    }

    bool is_on = (strcmp(action, "ON") == 0);


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
    } else if (strcmp(device, "window") == 0) {
        control_success = control_window(room, is_on);
    } else if (strcmp(device, "door") == 0) {
        control_success = control_door(room, is_on);
    } else if (strcmp(device, "curtain") == 0) {
        control_success = control_curtain(room, is_on);
    } else if (strcmp(device, "temp_sensor") == 0) {
        int room_index = getRoomIndex(room);
        if (room_index == -1) {

            publish_error_state(room, device, correlation_id, "UNKNOWN_ROOM", "Unknown room ID");
            return;
        }

        SensorData sensor_data = getSensorData((RoomIndex)room_index);
        float temp_value = sensor_data.temperature;

        Serial.print("[HAL] '"); Serial.print(room);
        Serial.print("/temp_sensor' read: "); Serial.print(temp_value); Serial.println("°C");

        if (temp_value == -999.0) {

            publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Temperature sensor read failed");
            return;
        }

        publish_sensor_state(room, device, "READ", correlation_id, temp_value, "°C");
        return;
    } else if (strcmp(device, "humidity_sensor") == 0) {
        int room_index = getRoomIndex(room);
        if (room_index == -1) {

            publish_error_state(room, device, correlation_id, "UNKNOWN_ROOM", "Unknown room ID");
            return;
        }

        SensorData sensor_data = getSensorData((RoomIndex)room_index);
        float humidity_value = sensor_data.humidity;

        Serial.print("[HAL] '"); Serial.print(room);
        Serial.print("/humidity_sensor' read: "); Serial.print(humidity_value); Serial.println("%");

        if (humidity_value == -999.0) {

            publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Humidity sensor read failed");
            return;
        }


        publish_sensor_state(room, device, "READ", correlation_id, humidity_value, "%");
        return;
    }




    else {
        Serial.print("Warning: No control logic in .ino for device type '");
        Serial.print(device); Serial.println("'");

        publish_error_state(room, device, correlation_id, "UNKNOWN_DEVICE_TYPE", "Device type not supported");
        return;
    }


    if (!control_success) {
        publish_error_state(room, device, correlation_id, "DEVICE_NOT_FOUND", "Device not found in this node's configuration");
        return;
    }


    publish_state(room, device, action, correlation_id);
}




void setup() {
    Serial.begin(115200);
    setup_devices();
    initSensorData();

    #if ENABLE_UI_DISPLAY
    uiController.begin();
    #endif

    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
}




void loop() {
    #if ENABLE_UI_DISPLAY

    uiController.update();
    #endif


    if (!client.connected()) {
        reconnect();
    }


    client.loop();
}