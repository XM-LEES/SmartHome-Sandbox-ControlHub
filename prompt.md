好的，没问题！将设计文档转化为一个清晰、全面的Prompt，是利用AI辅助开发一个非常高效的方式。一个好的Prompt能够确保AI生成的代码框架符合你的设计思想。

下面我为你准备了一个精心设计的Prompt。这个Prompt整合了你所有的需求和我们之前讨论的所有设计细节，并加入了具体的场景（房间和设备），使其可以直接用于生成项目的大框架代码。

你只需要将以下完整的Prompt内容复制给一个大型语言模型（如GPT-4、Claude等），它就能为你生成一个相当不错的项目起点。

【最终版 Prompt】用于生成智能家居沙盘控制系统项目框架

任务目标：
我需要你扮演一名资深的物联网（IoT）系统架构师和全栈开发者。请根据以下详细要求，为我生成一个“智能家居沙盘演示系统”的完整项目框架。该系统使用树莓派作为上位机，ESP32作为下位机。你需要提供上位机Python FastAPI服务的完整代码框架，以及下位机ESP32的Arduino C++代码框架。

核心要求：

架构清晰：严格遵循上位机（FastAPI + MQTT Broker）与下位机（ESP32固件）分离的松耦合架构。
闭环控制：必须实现从API调用到设备执行确认的完整闭环控制流程。API需等待下位机的执行回执，并处理超时。
异步处理：上位机FastAPI服务必须使用异步（async/await）来处理API请求和等待MQTT回执，确保高并发性能。
代码质量：代码需要有清晰的注释、良好的结构、模块化的设计，并遵循最佳实践。
场景具体化：项目需要包含以下具体的房间和设备：
客厅 (living_room): 灯 (light), 空调 (ac)
厨房 (kitchen): 灯 (light)
卧室 (bedroom): 灯 (light), 空调 (ac)
卫生间 (bathroom): 灯 (light)
第一部分：上位机 (树莓派) - Python FastAPI 服务

请生成一个多文件的Python FastAPI项目结构。

1. 项目文件结构：

复制代码
/smart_home_api/
|-- main.py              # FastAPI应用的入口和API路由定义
|-- mqtt_client.py       # 封装所有MQTT相关逻辑的模块
|-- request_manager.py   # 管理API请求与MQTT回执的追踪模块
|-- config.py            # 存放配置，如MQTT Broker地址
|-- requirements.txt     # 项目依赖


2. config.py 的内容：

定义MQTT Broker的主机地址（localhost）、端口（1883）和API请求的超时时间（例如5秒）。

3. mqtt_client.py 的详细要求：

创建一个名为 MQTTClient 的单例类或模块级实例。
负责连接到MQTT Broker、断开连接。
实现 publish 方法。
关键：实现 on_message 回调函数。此函数需要能解析收到的状态消息，并根据 correlation_id 触发相应的异步事件。它需要与 request_manager.py 模块交互。

4. request_manager.py 的详细要求：

创建一个名为 RequestManager 的单例类或模块级实例。
内部使用一个字典来存储正在进行的请求，key为correlation_id，value为一个asyncio.Event对象。
提供 start_request() 方法：接收一个correlation_id，创建并存储一个Event，然后返回这个Event。
提供 finish_request() 方法：接收一个correlation_id，找到对应的Event并set()它，然后从字典中移除。

5. main.py 的详细要求：

创建FastAPI应用实例。
在startup事件中初始化并连接MQTT客户端。
在shutdown事件中断开MQTT连接。
定义一个统一的API Endpoint：POST /api/v1/devices/{room_id}/{device_id}/action。
Endpoint逻辑：
接收room_id, device_id和请求体（包含action和可选的value）。
生成一个唯一的correlation_id。
调用RequestManager的start_request()获取一个event对象。
构造命令Topic（如 smarthome/living_room/light/command）。
构造带有correlation_id的JSON Payload。
临时订阅对应的状态Topic（如 smarthome/living_room/light/state）。
调用MQTTClient的publish()方法发送命令。
使用asyncio.wait_for()来await event.wait()，并处理TimeoutError。
无论成功或超时，都要取消订阅状态Topic。
根据结果返回正确的HTTP响应（成功或超时错误）。

6. requirements.txt 的内容：

列出所有必要的Python库，如fastapi, uvicorn, paho-mqtt。
第二部分：下位机 (ESP32) - Arduino C++ 固件

请为其中一个ESP32（例如，控制客厅的灯和空调）生成一个结构清晰的Arduino项目框架。

1. 项目文件结构：

LivingRoomController.ino (主文件)
DeviceControl.h (硬件抽象层)

2. DeviceControl.h 的详细要求：

这是一个头文件，用于封装硬件操作。
定义客厅灯和空调的GPIO引脚。
声明并实现以下函数：
void setup_devices(): 初始化所有引脚。
void control_light(bool is_on): 控制灯的开关。
void control_ac(bool is_on, int temperature): 控制空调的开关和设置温度。
函数内部应包含Serial.println输出来模拟硬件动作，方便调试。

3. LivingRoomController.ino 的详细要求：

包含库: WiFi.h, PubSubClient.h, 和你自己的 DeviceControl.h。
配置: 定义WiFi的SSID/密码，MQTT Broker的IP地址（树莓派的IP），以及本设备的room_id和device_id等。
setup()函数:
启动串口通信。
调用setup_devices()初始化硬件。
连接WiFi。
设置MQTT服务器和callback函数。
reconnect()函数:
实现MQTT断线重连逻辑。
关键: 在重连成功后，必须重新订阅该设备需要监听的所有command Topic，例如 smarthome/living_room/light/command 和 smarthome/living_room/ac/command。
callback(char* topic, byte* payload, unsigned int length)函数:
这是核心逻辑所在。
使用ArduinoJson库来解析收到的JSON payload。
提取action, value（如果存在）, 和 correlation_id。
使用strcmp()比较topic，判断是控制灯还是空调。
根据action调用DeviceControl.h中对应的函数。
关键: 操作完成后，立即构造一个JSON格式的状态回执（包含执行结果和原样返回的correlation_id），并将其发布到对应的state Topic。
loop()函数:
保持WiFi和MQTT的连接。
调用client.loop()来处理MQTT消息。

请确保两个部分的代码框架都注释清晰，逻辑完整，能够让开发者在此基础上快速进行填充和编码。