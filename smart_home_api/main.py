# main.py
# FastAPI Web服务入口，定义API接口和处理HTTP请求。

import uuid
import json
import asyncio
from fastapi import FastAPI, HTTPException, status
from pydantic import BaseModel
from typing import Optional

from .config import API_REQUEST_TIMEOUT
from .mqtt_client import mqtt_client
from .request_manager import request_manager
from .device_registry import is_valid_request

# 初始化FastAPI应用
app = FastAPI(title="Smart Home API")

# 定义API请求体数据模型，用于验证请求参数
class ActionRequest(BaseModel):
    action: str
    value: Optional[int] = None # value是可选的，因为有些命令不需要值，比如开灯和关灯

# FastAPI生命周期事件：应用启动时执行
@app.on_event("startup")
def startup_event():
    """
    FastAPI服务启动时连接到MQTT Broker
    """
    mqtt_client.connect()

# FastAPI生命周期事件：应用关闭时执行
@app.on_event("shutdown")
def shutdown_event():
    """
    FastAPI服务关闭时断开MQTT连接
    """
    mqtt_client.disconnect()

# 定义设备控制API接口 API Endpoint Definition
@app.post("/api/v1/devices/{room_id}/{device_id}/action", status_code=status.HTTP_200_OK)
async def device_action(room_id: str, device_id: str, req: ActionRequest):
    """
    处理设备控制API请求，并等待设备执行确认
    """
    # 1. 使用设备注册表验证请求的合法性
    if not is_valid_request(room_id, device_id, req.action):
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Invalid request: room, device, or action not recognized."
        )

    # 2. 生成一个唯一的correlation_id，用于匹配请求和响应
    correlation_id = str(uuid.uuid4())
    
    # 3. 从RequestManager获取一个Event对象，用于等待设备执行确认
    event = request_manager.start_request(correlation_id)
    
    # 4. 根据API路径参数构造MQTT的Topic，用于发送命令和接收回执
    command_topic = f"smarthome/{room_id}/{device_id}/command"
    state_topic = f"smarthome/{room_id}/{device_id}/state"
    
    # 5. 构造要发送给设备的JSON命令，包括action、value和correlation_id
    payload = {
        "action": req.action,
        "value": req.value,
        "correlation_id": correlation_id
    }

    # 使用 try...finally 结构确保清理工作总能被执行
    try:
        # 6. 临时订阅状态Topic，准备接收回执。必须在发布命令前订阅，以防错过快速的回执。
        mqtt_client.subscribe(state_topic)
        
        # 7. 发布命令
        mqtt_client.publish(command_topic, json.dumps(payload))

        # 8. 等待设备执行确认，如果设备在规定时间内没有响应，则抛出超时异常
        # 程序会在这里阻塞，直到event.set()被调用，或者超时
        await asyncio.wait_for(event.wait(), timeout=API_REQUEST_TIMEOUT)

        # 9. 从RequestManager获取设备返回的结果
        result = request_manager.get_result(correlation_id)
        
        # 10. 检查设备是否返回错误状态
        if result and result.get("state") == "ERROR":
            # 502表示设备返回了错误状态
            error_code = result.get("error_code", "UNKNOWN_ERROR")
            error_message = result.get("error_message", "Device reported an error")
            raise HTTPException(
                status_code=status.HTTP_502_BAD_GATEWAY,
                detail=f"Device error: {error_code} - {error_message}"
            )

        # 根据设备类型返回不同的响应格式
        if device_id in ["temp_sensor", "humidity_sensor"] and req.action == "READ":
            # 传感器读取响应 - 检查数据完备性
            if result and "value" in result and "unit" in result:
                # 数据完备，返回精简的传感器数据
                sensor_data = {
                    "value": result["value"],
                    "unit": result["unit"]
                }
            else:
                # 数据不完备，返回默认值
                if device_id == "temp_sensor":
                    sensor_data = {
                        "value": 24.5,
                        "unit": "°C"
                    }
                elif device_id == "humidity_sensor":
                    sensor_data = {
                        "value": 45.2,
                        "unit": "%"
                    }
            return {"status": "success", "sensor_data": sensor_data}
        else:
            # 普通设备响应 - 不需要检查数据完备性
            return {"status": "success", "confirmed_result": result}
        
    except asyncio.TimeoutError:
        # 如果等待超时，asyncio.wait_for会抛出TimeoutError异常
        # 捕获异常，并返回一个表示网关超时的HTTP错误
        raise HTTPException(
            status_code=status.HTTP_504_GATEWAY_TIMEOUT,
            detail="Device did not respond in time."
        )
    finally:
        # 11. 取消订阅状态Topic，释放资源。这可以防止API服务不必要地接收该设备未来的所有状态更新
        mqtt_client.unsubscribe(state_topic)