# 封装MQTT通信相关逻辑。

import json
import paho.mqtt.client as mqtt
from .config import MQTT_BROKER_HOST, MQTT_BROKER_PORT
from .request_manager import request_manager

class MQTTClient:
    # 单例模式，确保全局只有一个MQTT客户端实例
    _instance = None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super().__new__(cls)
        return cls._instance

    def __init__(self):
        # 防止重复初始化
        if hasattr(self, 'client'):
            return
        # 创建paho-mqtt客户端实例
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION1)
        # 绑定回调函数
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message
        # 跟踪已订阅的topic，避免重复订阅
        self._subscriptions = set()

    def connect(self):
        """
        连接到MQTT Broker并启动后台网络循环
        """
        print(f"Connecting to MQTT Broker at {MQTT_BROKER_HOST}:{MQTT_BROKER_PORT}")
        self.client.connect(MQTT_BROKER_HOST, MQTT_BROKER_PORT, 60)
        self.client.loop_start() # loop_start() 会在一个独立的后台线程中处理MQTT的网络通信，不会阻塞主程序。

    def disconnect(self):
        """
        断开MQTT连接
        """
        self.client.loop_stop()
        self.client.disconnect()
        print("Disconnected from MQTT Broker.")

    def publish(self, topic: str, payload: str):
        """
        发布消息到指定Topic
        """
        print(f"[MQTT] Publishing to topic '{topic}': {payload}")
        self.client.publish(topic, payload)

    def subscribe(self, topic: str):
        """
        订阅一个Topic，避免重复订阅
        """
        if topic not in self._subscriptions:
            print(f"[MQTT] Subscribing to topic: {topic}")
            self.client.subscribe(topic)
            self._subscriptions.add(topic)

    def unsubscribe(self, topic: str):
        """
        取消订阅一个Topic
        """
        if topic in self._subscriptions:
            print(f"[MQTT] Unsubscribing from topic: {topic}")
            self.client.unsubscribe(topic)
            self._subscriptions.remove(topic)

    def on_connect(self, client, userdata, flags, rc):
        """
        连接到Broker时的回调函数，当连接成功或失败时都会调用
        """
        if rc == 0: # 连接成功
            print("Successfully connected to MQTT Broker.")
        else: # 连接失败
            print(f"Failed to connect, return code {rc}\n")

    def on_message(self, client, userdata, msg):
        """
        任何已订阅的Topic收到消息时，都会调用此回调函数
        接收设备执行回执（ACK）的核心入口点
        """
        print(f"[MQTT] Received message on topic '{msg.topic}': {msg.payload.decode()}")
        try:
            # 解析收到的JSON消息
            payload = json.loads(msg.payload.decode())
            # 提取最重要的correlation_id
            correlation_id = payload.get("correlation_id")

            if correlation_id:
                # 如果消息里有ID，就通知RequestManager去完成对应的等待任务
                request_manager.finish_request(correlation_id, payload)
        except Exception as e:
            print(f"Error processing received message: {e}")

# 创建全局唯一实例
mqtt_client = MQTTClient()