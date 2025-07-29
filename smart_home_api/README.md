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

2. 配置并启动 Mosquitto MQTT Broker (使用 Docker)

    为了保证稳定运行和数据持久化，我们使用 Docker 配合自定义配置文件来启动 Mosquitto。

    步骤1: 创建配置文件和目录

    在项目目录或Home目录下，创建 Mosquitto 所需的文件夹结构。

    ```bash
    # 创建主目录
    mkdir mosquitto
    cd mosquitto

    # 创建子目录
    mkdir config data log

    # 创建配置文件
    nano config/mosquitto.conf
    ```

    将以下内容粘贴到 `mosquitto.conf` 文件中并保存：

    ```conf
    # 持久化配置，将数据保存在 /mosquitto/data/ 目录
    persistence true
    persistence_location /mosquitto/data/

    # 日志配置，将日志输出到 /mosquitto/log/ 目录下的文件
    log_dest file /mosquitto/log/mosquitto.log
    log_type all

    # === 网络核心配置 ===
    # 监听所有网络接口的1883端口，允许局域网设备（如ESP32）连接
    listener 1883

    # 允许匿名连接，方便设备在开发阶段接入
    allow_anonymous true
    ```

    步骤2: 修复目录权限

    为了避免 Docker 容器内的用户无法写入主机目录，需要修复文件夹权限。

    首先，找到你当前用户的 ID：
    ```bash
    id
    ```
    记下你的 `uid` 和 `gid` (通常都是 `1000`)。

    然后，使用 `chown` 命令更改目录所有者（**可选，但推荐**），或者在`docker run`命令中使用`--user`参数。这里我们推荐后者。

    步骤3: 启动 Mosquitto 容器

    返回到 `mosquitto` 目录的上级目录，执行以下命令。**请将 `1000:1000`替换成你自己的 `uid:gid`**。

    ```bash
    # 使用 -v 挂载配置、数据和日志目录，并使用 --user 指定运行用户
    docker run -d \
      -p 1883:1883 \
      -p 9001:9001 \
      --name mosquitto \
      --restart always \
      --user 1000:1000 \
      -v ./mosquitto/config:/mosquitto/config \
      -v ./mosquitto/data:/mosquitto/data \
      -v ./mosquitto/log:/mosquitto/log \
      eclipse-mosquitto
    ```
    *   `--restart always`: 确保 Docker 服务重启或树莓派重启后，Mosquitto 容器能自动启动。
    *   `-v ./mosquitto/...`: 从当前目录挂载，更具可移植性。

    步骤4: 验证服务

    ```bash
    # 查看容器是否正在运行
    docker ps

    # 实时查看日志（如果需要排错）
    tail -f ./mosquitto/log/mosquitto.log
    ```
    如果看到 `Opening ipv4 listen socket on port 1883`，则表示启动成功。

    常用 Docker 管理命令
    - 停止容器: `docker stop mosquitto`
    - 启动已停止的容器: `docker start mosquitto`
    - 查看日志: `docker logs mosquitto` 或 `tail -f ./mosquitto/log/mosquitto.log`
    - 删除容器 (会保留数据卷): `docker rm mosquitto`

    **与本地代码配合**：
    - smart_home_api 代码无需更改，直接连接 `localhost:1883` 即可。

3.  启动服务:
    在 SmartHome-Sandbox-ControlHub 根目录下运行 uvicorn，指定模块路径为 smart_home_api.main:app
    ```bash
    uvicorn smart_home_api.main:app --reload --host 0.0.0.0
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