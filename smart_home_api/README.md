# Smart Home API

本项目为智能家居沙盘演示系统的上位机服务，基于 FastAPI + MQTT 实现。

## 架构说明
- **API接口**: 提供一个标准的 RESTful API 接口，用于接收外部控制指令。
- **消息通信**: 通过 MQTT 协议与下位机（如 ESP32）进行异步通信。
- **闭环控制**: 支持可靠的闭环控制。API 会等待下位机成功执行命令后的状态回执。对于任何非成功的情况（如设备离线、指令解析失败、硬件执行失败），API 将会超时并返回错误。
- **高性能**: 完全基于异步I/O（asyncio），能够高效处理大量并发请求。

## 依赖
- fastapi
- uvicorn[standard]
- paho-mqtt

## 目录结构
- `main.py`: FastAPI 应用入口与 API 路由定义。
- `mqtt_client.py`: 封装所有 MQTT 客户端逻辑。
- `request_manager.py`: 负责追踪 API 请求与设备回执的匹配。
- `config.py`: 存放 MQTT Broker 地址、API 超时等全局配置。
- `device_registry.py`: 系统的“数字孪生”，定义所有合法设备及其操作。

## 启动方法

1.  安装依赖:
    ```bash
    pip install -r requirements.txt
    ```

2.  使用 Docker 运行 Mosquitto（MQTT Broker）:

    本项目推荐使用 Docker 快速部署 Mosquitto 作为 MQTT Broker，无需在本机环境安装额外软件。

    **前置条件**：
    - 已安装 Docker（Windows 推荐 Docker Desktop）。

    **拉取官方 Mosquitto 镜像**：
    ```bash
    docker pull eclipse-mosquitto
    ```

    **启动 Mosquitto 容器**  
    最简单方式（前台运行，便于查看日志）：
    ```bash
    docker run -it --name mosquitto -p 1883:1883 eclipse-mosquitto
    ```
    推荐方式（后台运行）：
    ```bash
    docker run -d --name mosquitto -p 1883:1883 eclipse-mosquitto
    ```

    **验证服务是否启动**：
    ```bash
    docker logs mosquitto
    ```
    出现 `Opening ipv4 listen socket on port 1883` 表示启动成功。

    **常用管理命令**：
    - 停止容器：
      ```bash
      docker stop mosquitto
      ```
    - 启动容器：
      ```bash
      docker start mosquitto
      ```
    - 删除容器：
      ```bash
      docker rm mosquitto
      ```

    **与本地代码配合**：
    - smart_home_api 代码无需更改，直接连接 `localhost:1883` 即可。

3.  启动服务:
    ```bash
    # 在 SmartHome-Sandbox-ControlHub 根目录下运行 uvicorn，指定模块路径为 smart_home_api.main:app
    uvicorn smart_home_api.main:app --reload
    ```

## API 格式约定
- **Endpoint**: `POST /api/v1/devices/{room_id}/{device_id}/action`
- **请求体 (JSON)**:
  ```json
  {
    "action": "ACTION_NAME",
    "value": 123
  }
  ```
- **成功响应 (HTTP 200 OK)**:
  ```json
  {
    "status": "success",
    "confirmed_result": {
      "state": "ACTION_NAME",
      "correlation_id": "..."
    }
  }
  ```
- **失败/超时响应 (HTTP 504 Gateway Timeout)**:
  ```json
  {
    "detail": "Device did not respond in time."
  }
  ```


## 未来改进方向 (Future Enhancements)

为了提供更丰富的调试信息和更明确的错误反馈，未来的版本可以升级为**“明确错误回执”（Explicit Error Reporting）**模式。

- **改进点**: 在上述所有失败路径中，固件不再是静默`return`，而是向`state` Topic发布一个带有`"state": "ERROR"`和具体`"error": "error message"`字段的回执。
- **优点**: 这将允许上位机区分**设备超时**（可能离线）和**设备明确报告的错误**（在线但执行失败），从而返回更精确的HTTP错误码（如 `502 Bad Gateway`）给最终用户。