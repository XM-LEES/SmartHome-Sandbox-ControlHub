#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DeviceControl.h"

// WiFi配置
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// MQTT Broker配置
const char* mqtt_server = "192.168.1.100"; // 树莓派IP

// 设备标识
const char* room_id = "living_room";
const char* light_device_id = "light";
const char* ac_device_id = "ac";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("LivingRoomController")) {
            Serial.println("connected");
            client.subscribe("smarthome/living_room/light/command");
            client.subscribe("smarthome/living_room/ac/command");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void send_state(const char* device, const char* action, int value, const char* correlation_id, bool success) {
    char topic[64];
    snprintf(topic, sizeof(topic), "smarthome/living_room/%s/state", device);
    StaticJsonDocument<256> doc;
    doc["action"] = action;
    doc["value"] = value;
    doc["correlation_id"] = correlation_id;
    doc["success"] = success;
    char buffer[256];
    size_t n = serializeJson(doc, buffer);
    client.publish(topic, buffer, n);
}

void callback(char* topic, byte* payload, unsigned int length) {
    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, payload, length);
    if (error) {
        Serial.println("[MQTT] Failed to parse JSON");
        return;
    }
    const char* action = doc["action"];
    int value = doc["value"] | 0;
    const char* correlation_id = doc["correlation_id"];
    bool success = false;
    if (strcmp(topic, "smarthome/living_room/light/command") == 0) {
        if (strcmp(action, "on") == 0) {
            control_light(true);
            success = true;
        } else if (strcmp(action, "off") == 0) {
            control_light(false);
            success = true;
        }
        send_state(light_device_id, action, value, correlation_id, success);
    } else if (strcmp(topic, "smarthome/living_room/ac/command") == 0) {
        if (strcmp(action, "on") == 0) {
            control_ac(true, value);
            success = true;
        } else if (strcmp(action, "off") == 0) {
            control_ac(false, value);
            success = true;
        }
        send_state(ac_device_id, action, value, correlation_id, success);
    }
}

void setup() {
    Serial.begin(115200);
    setup_devices();
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
} 