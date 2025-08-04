#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
智能家居API快速测试脚本

快速测试基本功能，适合开发调试使用
"""

import requests
import json
import time

API_BASE_URL = "http://127.0.0.1:8000"

def test_api(room_id, device_id, action, value=None, description=""):
    """测试API调用"""
    url = f"{API_BASE_URL}/api/v1/devices/{room_id}/{device_id}/action"
    payload = {"action": action}
    if value is not None:
        payload["value"] = value
    
    print(f"测试: {description}")
    print(f"请求: {room_id}/{device_id} - {action}" + (f" (value={value})" if value is not None else ""))
    
    try:
        response = requests.post(url, json=payload, timeout=5)
        print(f"状态码: {response.status_code}")
        print(f"响应: {response.json()}")
        print("✓ 成功" if response.status_code == 200 else "✗ 失败")
    except Exception as e:
        print(f"错误: {e}")
        print("✗ 失败")
    
    print("-" * 50)
    time.sleep(0.5)

def main():
    """快速测试主要功能"""
    print("智能家居API快速测试")
    print("=" * 50)
    
    # 测试基本开关设备
    test_api("livingroom", "light", "ON", description="客厅灯开启")
    test_api("livingroom", "light", "OFF", description="客厅灯关闭")
    
    # 测试空调功能
    test_api("livingroom", "ac", "ON", 25, description="客厅空调开启25度")
    test_api("livingroom", "ac", "SET_TEMP", 23, description="客厅空调设置23度")
    test_api("livingroom", "ac", "OFF", description="客厅空调关闭")
    
    # 测试传感器
    test_api("livingroom", "temp_sensor", "READ", description="读取客厅温度")
    test_api("livingroom", "humidity_sensor", "READ", description="读取客厅湿度")
    
    # 测试错误情况
    test_api("livingroom", "ac", "ON", description="空调开启无温度参数(应该失败)")
    test_api("invalid_room", "light", "ON", description="无效房间(应该失败)")
    test_api("livingroom", "light", "INVALID", description="无效操作(应该失败)")
    
    print("快速测试完成!")

if __name__ == "__main__":
    main()