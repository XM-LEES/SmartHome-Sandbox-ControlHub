# main.py
# FastAPI Web服务入口，定义API接口和处理HTTP请求。

import uuid
import json
import asyncio
from fastapi import FastAPI, HTTPException, Request, status
from pydantic import BaseModel
from typing import Optional

from .config import API_REQUEST_TIMEOUT
from .mqtt_client import mqtt_client
from .request_manager import request_manager

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
    # 1. 生成一个唯一的correlation_id，用于匹配请求和响应
    correlation_id = str(uuid.uuid4())
    
    # 2. 从RequestManager获取一个Event对象，用于等待设备执行确认
    event = request_manager.start_request(correlation_id)
    
    # 3. 根据API路径参数构造MQTT的Topic，用于发送命令和接收回执
    command_topic = f"smarthome/{room_id}/{device_id}/command"
    state_topic = f"smarthome/{room_id}/{device_id}/state"
    
    # 4. 构造要发送给设备的JSON命令，包括action、value和correlation_id
    payload = {
        "action": req.action,
        "value": req.value,
        "correlation_id": correlation_id
    }

    # 使用 try...finally 结构确保清理工作总能被执行
    try:
        # 5. 临时订阅状态Topic，准备接收回执。必须在发布命令前订阅，以防错过快速的回执。
        mqtt_client.subscribe(state_topic)
        
        # 6. 发布命令
        mqtt_client.publish(command_topic, json.dumps(payload))

        # 7. 等待设备执行确认，如果设备在规定时间内没有响应，则抛出超时异常
        # 程序会在这里阻塞，直到event.set()被调用，或者超时
        await asyncio.wait_for(event.wait(), timeout=API_REQUEST_TIMEOUT)

        # 8. 从RequestManager获取设备返回的结果
        result = request_manager.get_result(correlation_id)
        
        # 9. 返回一个成功的HTTP响应给客户端
        return {"status": "success", "confirmed_result": result}
        
    except asyncio.TimeoutError:
        # 如果等待超时，asyncio.wait_for会抛出TimeoutError异常
        # 捕获异常，并返回一个表示网关超时的HTTP错误
        raise HTTPException(
            status_code=status.HTTP_504_GATEWAY_TIMEOUT,
            detail="Device did not respond in time."
        )
    finally:
        # 10. 取消订阅状态Topic，释放资源。这可以防止API服务不必要地接收该设备未来的所有状态更新
        mqtt_client.unsubscribe(state_topic)