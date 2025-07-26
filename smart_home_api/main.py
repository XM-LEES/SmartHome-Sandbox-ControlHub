import uuid
import json
import asyncio
from fastapi import FastAPI, HTTPException, Request
from pydantic import BaseModel
from .config import API_REQUEST_TIMEOUT
from .mqtt_client import mqtt_client
from .request_manager import request_manager

app = FastAPI()

class ActionRequest(BaseModel):
    action: str
    value: int = None

@app.on_event("startup")
def startup_event():
    mqtt_client.connect()

@app.on_event("shutdown")
def shutdown_event():
    mqtt_client.disconnect()

@app.post("/api/v1/devices/{room_id}/{device_id}/action")
async def device_action(room_id: str, device_id: str, req: ActionRequest):
    correlation_id = str(uuid.uuid4())
    event = request_manager.start_request(correlation_id)
    command_topic = f"smarthome/{room_id}/{device_id}/command"
    state_topic = f"smarthome/{room_id}/{device_id}/state"
    payload = {
        "action": req.action,
        "value": req.value,
        "correlation_id": correlation_id
    }
    mqtt_client.subscribe(state_topic)
    mqtt_client.publish(command_topic, json.dumps(payload))
    try:
        await asyncio.wait_for(event.wait(), timeout=API_REQUEST_TIMEOUT)
        result = request_manager.get_result(correlation_id)
        return {"status": "success", "result": result}
    except asyncio.TimeoutError:
        raise HTTPException(status_code=504, detail="Device response timeout")
    finally:
        mqtt_client.unsubscribe(state_topic) 