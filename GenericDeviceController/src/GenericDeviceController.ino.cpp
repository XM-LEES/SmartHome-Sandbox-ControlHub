# 1 "C:\\Users\\XM-LEE\\AppData\\Local\\Temp\\tmpnuh23aj1"
#include <Arduino.h>
# 1 "C:/Users/XM-LEE/Desktop/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"




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

#if ENABLE_SENSOR_SIMULATOR
    #include "sensorsimulator/UIController.h"

    UIController uiController;
#endif


WiFiClient espClient;
PubSubClient client(espClient);


enum WiFiState {
    WIFI_DISCONNECTED,
    WIFI_CONNECTING,
    WIFI_CONNECTED
};

enum MQTTState {
    MQTT_STATE_DISCONNECTED,
    MQTT_STATE_CONNECTING,
    MQTT_STATE_CONNECTED
};


static WiFiState wifiState = WIFI_DISCONNECTED;
static MQTTState mqttState = MQTT_STATE_DISCONNECTED;
static unsigned long nextWifiRetryMs = 0;
static unsigned long nextMqttRetryMs = 0;
static unsigned long wifiConnectStartMs = 0;
static unsigned long mqttConnectStartMs = 0;


static WiFiState lastWifiState = WIFI_DISCONNECTED;
static MQTTState lastMqttState = MQTT_STATE_DISCONNECTED;


static const unsigned long WIFI_CONNECT_TIMEOUT_MS = 10000;
static const unsigned long WIFI_RETRY_INTERVAL_MS = 5000;
static const unsigned long MQTT_CONNECT_TIMEOUT_MS = 3000;
static const unsigned long MQTT_RETRY_INTERVAL_MS = 5000;
void handleWiFiState();
void handleMQTTState();
void setup_wifi();
void reconnect();
void publish_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id);
void publish_sensor_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id, float value, const char* unit);
void publish_ac_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id, int temperature);
void publish_error_state(const char* room_id, const char* device_id, const char* correlation_id, const char* error_code, const char* error_message);
void callback(char* topic, byte* payload, unsigned int length);
void setup();
void loop();
#line 62 "C:/Users/XM-LEE/Desktop/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"
void handleWiFiState() {
    switch (wifiState) {
        case WIFI_DISCONNECTED:

            if (millis() >= nextWifiRetryMs) {

                Serial.println("[WiFi] Starting connection...");
                WiFi.mode(WIFI_STA);
                WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
                wifiState = WIFI_CONNECTING;
                wifiConnectStartMs = millis();
            }
            break;

        case WIFI_CONNECTING:

            if (WiFi.status() == WL_CONNECTED) {

                wifiState = WIFI_CONNECTED;
                Serial.println("[WiFi] Connected successfully!");
                Serial.print("[WiFi] IP: ");
                Serial.println(WiFi.localIP());
            } else if (millis() - wifiConnectStartMs >= WIFI_CONNECT_TIMEOUT_MS) {

                Serial.println("[WiFi] Connection timeout, will retry later");
                wifiState = WIFI_DISCONNECTED;
                nextWifiRetryMs = millis() + WIFI_RETRY_INTERVAL_MS;
            }
            break;

        case WIFI_CONNECTED:

            if (WiFi.status() != WL_CONNECTED) {

                Serial.println("[WiFi] Connection lost");
                wifiState = WIFI_DISCONNECTED;
                nextWifiRetryMs = millis() + 1000;
            }
            break;
    }


    if (wifiState != lastWifiState) {
        lastWifiState = wifiState;
        #if ENABLE_SENSOR_SIMULATOR
        if (g_uiController) g_uiController->setRedraw();
        #endif
    }
}




void handleMQTTState() {
    switch (mqttState) {
        case MQTT_STATE_DISCONNECTED:

            if (wifiState == WIFI_CONNECTED && millis() >= nextMqttRetryMs) {

                Serial.println("[MQTT] Starting connection...");
                mqttState = MQTT_STATE_CONNECTING;
                mqttConnectStartMs = millis();
            }
            break;

        case MQTT_STATE_CONNECTING:

            if (client.connected()) {

                mqttState = MQTT_STATE_CONNECTED;
                Serial.println("[MQTT] Connected successfully!");


                for (int i = 0; i < DEVICE_COUNT; i++) {
                    char command_topic[128];
                    snprintf(command_topic, sizeof(command_topic), "smarthome/%s/%s/command", devices[i].room_id, devices[i].device_id);
                    client.subscribe(command_topic);
                    Serial.print("[MQTT] Subscribed to: ");
                    Serial.println(command_topic);
                }
            } else if (millis() - mqttConnectStartMs >= MQTT_CONNECT_TIMEOUT_MS) {

                Serial.println("[MQTT] Connection timeout, will retry later");
                mqttState = MQTT_STATE_DISCONNECTED;
                nextMqttRetryMs = millis() + MQTT_RETRY_INTERVAL_MS;
            } else {

                if (!client.connect(NODE_ID)) {

                    delay(100);
                }
            }
            break;

        case MQTT_STATE_CONNECTED:

            if (!client.connected()) {

                Serial.println("[MQTT] Connection lost");
                mqttState = MQTT_STATE_DISCONNECTED;
                nextMqttRetryMs = millis() + 1000;
            }
            break;
    }


    if (mqttState != lastMqttState) {
        lastMqttState = mqttState;
        #if ENABLE_SENSOR_SIMULATOR
        if (g_uiController) g_uiController->setRedraw();
        #endif
    }
}




void setup_wifi() {
    Serial.println();
    Serial.print("[WiFi] Connecting to: ");
    Serial.println(WIFI_SSID);


    wifiState = WIFI_DISCONNECTED;
    nextWifiRetryMs = millis() + 1000;
}





void reconnect() {

    Serial.println("[MQTT] reconnect() called, but using state machine instead");
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
# 234 "C:/Users/XM-LEE/Desktop/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"
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
# 265 "C:/Users/XM-LEE/Desktop/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"
void publish_ac_state(const char* room_id, const char* device_id, const char* state, const char* correlation_id, int temperature) {

    char state_topic[128];
    snprintf(state_topic, sizeof(state_topic), "smarthome/%s/%s/state", room_id, device_id);


    StaticJsonDocument<256> doc;
    doc["state"] = state;
    doc["correlation_id"] = correlation_id;
    doc["temperature"] = temperature;
    doc["unit"] = "°C";


    char buffer[256];
    size_t n = serializeJson(doc, buffer);


    client.publish(state_topic, buffer, n);
    Serial.print("Published AC state to ");
    Serial.print(state_topic);
    Serial.print(": "); Serial.println(buffer);
}
# 296 "C:/Users/XM-LEE/Desktop/SmartHome-Sandbox-ControlHub/GenericDeviceController/src/GenericDeviceController.ino"
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
        if (strcmp(action, "SET_TEMP") == 0) {

            if (value < 0 || value > 40) {
                publish_error_state(room, device, correlation_id, "INVALID_TEMPERATURE", "Temperature value invalid. Valid range: 0-40°C");
                return;
            }
            control_success = control_ac_set_temperature(room, value);
        } else if (strcmp(action, "ON") == 0) {

            if (value >= 0 && value <= 40) {
                control_success = control_ac(room, true, value);
            } else {
                publish_error_state(room, device, correlation_id, "MISSING_OR_INVALID_VALUE", "ON operation requires valid temperature value (0-40°C)");
                return;
            }
        } else if (strcmp(action, "OFF") == 0) {

            control_success = control_ac(room, false, 0);
        } else {
            publish_error_state(room, device, correlation_id, "UNKNOWN_ACTION", "Unsupported action for AC device");
            return;
        }
    } else if (strcmp(device, "hood") == 0) {
        control_success = control_hood(room, is_on);
    } else if (strcmp(device, "fan") == 0) {
        control_success = control_fan(room, is_on);
    } else if (strcmp(device, "bedside_light") == 0) {
        control_success = control_light(room, is_on);
    } else if (strcmp(device, "window") == 0) {
        control_success = control_window(room, is_on);


    } else if (strcmp(device, "curtain") == 0) {
        control_success = control_curtain(room, is_on);
    } else if (strcmp(device, "temp_sensor") == 0) {
        float temp_value = control_temperature_sensor(room);
        if (temp_value != -999.0) {
            publish_sensor_state(room, device, "READ", correlation_id, temp_value, "°C");
        } else {
            publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Temperature sensor read failed");
        }
        return;
    } else if (strcmp(device, "humidity_sensor") == 0) {
        float humidity_value = control_humidity_sensor(room);
        if (humidity_value != -999.0) {
            publish_sensor_state(room, device, "READ", correlation_id, humidity_value, "%");
        } else {
            publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Humidity sensor read failed");
        }
        return;
            } else if (strcmp(device, "brightness_sensor") == 0) {
            float brightness_value = control_brightness_sensor(room);
            if (brightness_value != -999.0) {
                publish_sensor_state(room, device, "READ", correlation_id, brightness_value, "%");
            } else {
                publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Brightness sensor read failed");
            }
            return;
        } else if (strcmp(device, "smoke_sensor") == 0) {
            float smoke_value = control_smoke_sensor(room);
            if (smoke_value != -999.0) {
                publish_sensor_state(room, device, "READ", correlation_id, smoke_value, "");
            } else {
                publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Smoke sensor read failed");
            }
            return;
        } else if (strcmp(device, "gas_sensor") == 0) {
            float gas_value = control_gas_sensor(room);
            if (gas_value != -999.0) {
                publish_sensor_state(room, device, "READ", correlation_id, gas_value, "");
            } else {
                publish_error_state(room, device, correlation_id, "SENSOR_READ_ERROR", "Gas sensor read failed");
            }
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


    if (strcmp(device, "ac") == 0 && strcmp(action, "SET_TEMP") == 0) {

        AirConditionerState* state = get_ac_state(room);
        if (state != nullptr) {
            publish_ac_state(room, device, action, correlation_id, state->target_temperature);
        } else {
            publish_state(room, device, action, correlation_id);
        }
    } else {

        publish_state(room, device, action, correlation_id);
    }
}




void setup() {
    Serial.begin(115200);
    setup_devices();

    #if ENABLE_SENSOR_SIMULATOR
    initSensorData();
    uiController.begin();
    #endif

    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setSocketTimeout(1);
    client.setCallback(callback);


    WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t){


    });
}




void loop() {
    #if ENABLE_SENSOR_SIMULATOR
    uiController.update();
    #endif


    handleWiFiState();


    handleMQTTState();



    client.loop();
}