# 全局配置参数文件，用于存放可调整的参数

# MQTT Broker (消息代理) 主机地址
# Mosquitto 和 FastAPI 服务都运行在同一台树莓派上，所以这里使用 "localhost"
MQTT_BROKER_HOST = "localhost"

# MQTT Broker 端口
MQTT_BROKER_PORT = 1883

# API 请求的超时时间（单位：秒）。
# 如果 FastAPI 发出命令后，在这个时间内没有收到ESP32的执行回执，
# 就会认为请求失败，并向客户端返回一个超时错误。
# 注意：窗帘设备需要较长时间（约6秒）来完成物理动作，所以超时时间设置为8秒
API_REQUEST_TIMEOUT = 8