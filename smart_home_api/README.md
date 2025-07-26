# Smart Home API

本项目为智能家居沙盘演示系统的上位机服务，基于 FastAPI + MQTT 实现。

## 架构说明
- FastAPI 提供 RESTful API
- 通过 MQTT 与下位机（如 ESP32）通信
- 支持闭环控制，API等待设备回执
- 异步处理高并发

## 依赖
- fastapi
- uvicorn
- paho-mqtt

## 启动方法
```bash
pip install -r requirements.txt
uvicorn main:app --reload
```

## 目录结构
- main.py: FastAPI入口与API路由
- mqtt_client.py: MQTT逻辑
- request_manager.py: 请求追踪
- config.py: 配置 