# device_registry.py
# 系统的“数字孪生”，定义了所有物理设备的信息和能力

DEVICE_REGISTRY = {
    "livingroom": {
        "light": {
            "type": "switch",
            "valid_actions": ["ON", "OFF"]
        },
        "ac": {
            "type": "air_conditioner",
            "valid_actions": ["ON", "OFF", "SET_TEMP"]
        }
    },
    "bedroom": {
        "main_light": {
            "type": "switch",
            "valid_actions": ["ON", "OFF"]
        },
        "bedside_light": {
            "type": "dimmable_light",
            "valid_actions": ["ON", "OFF", "SET_BRIGHTNESS"]
        },
        "ac": {
            "type": "air_conditioner",
            "valid_actions": ["ON", "OFF", "SET_TEMP"]
        }
    },
    "kitchen": {
        "light": {
            "type": "switch",
            "valid_actions": ["ON", "OFF"]
        },
        "hood": {
            "type": "fan",
            "valid_actions": ["ON", "OFF", "SET_SPEED"]
        },
        "oven": {
            "type": "oven",
            "valid_actions": ["START", "STOP", "SET_MODE"]
        }
    },
    "bathroom": {
        "light": {
            "type": "switch",
            "valid_actions": ["ON", "OFF"]
        }
    }
}

def is_valid_request(room_id: str, device_id: str, action: str) -> bool:
    """
    校验API请求是否合法
    """
    room = DEVICE_REGISTRY.get(room_id)
    if not room:
        return False
    device = room.get(device_id)
    if not device:
        return False
    if action not in device["valid_actions"]:
        return False
    return True

