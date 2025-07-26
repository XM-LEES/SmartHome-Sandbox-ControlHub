import asyncio
import json
import paho.mqtt.client as mqtt
from .config import MQTT_BROKER_HOST, MQTT_BROKER_PORT
from .request_manager import RequestManager

class MQTTClient:
    _instance = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super().__new__(cls)
        return cls._instance

    def __init__(self):
        if hasattr(self, 'client'):
            return
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        self.loop = asyncio.get_event_loop()
        self._subscriptions = set()

    def connect(self):
        self.client.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT, 60)
        self.client.loop_start()

    def disconnect(self):
        self.client.loop_stop()
        self.client.disconnect()

    def publish(self, topic, payload):
        self.client.publish(topic, payload)

    def subscribe(self, topic):
        if topic not in self._subscriptions:
            self.client.subscribe(topic)
            self._subscriptions.add(topic)

    def unsubscribe(self, topic):
        if topic in self._subscriptions:
            self.client.unsubscribe(topic)
            self._subscriptions.remove(topic)

    def on_connect(self, client, userdata, flags, rc):
        print(f"Connected to MQTT Broker with result code {rc}")

    def on_message(self, client, userdata, msg):
        try:
            payload = json.loads(msg.payload.decode())
            correlation_id = payload.get("correlation_id")
            if correlation_id:
                RequestManager().finish_request(correlation_id, payload)
        except Exception as e:
            print(f"Error in on_message: {e}")

mqtt_client = MQTTClient() 