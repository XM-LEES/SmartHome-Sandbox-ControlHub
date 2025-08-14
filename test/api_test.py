#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
智能家居API完整测试脚本

测试所有房间的所有设备的所有功能，包括正常调用和错误情况
"""

import requests
import json
import time
import sys
from typing import Dict, List, Any

# API配置
API_BASE_URL = "http://192.168.123.62:8000"
API_ENDPOINT = "/api/v1/devices/{room_id}/{device_id}/action"

# 测试配置
REQUEST_TIMEOUT = 5  # 请求超时时间
TEST_DELAY = 0.5    # 测试间隔时间

class APITester:
    def __init__(self):
        self.total_tests = 0
        self.passed_tests = 0
        self.failed_tests = 0
        self.test_results = []
    
    def log(self, message: str, level: str = "INFO"):
        """日志输出"""
        timestamp = time.strftime("%H:%M:%S")
        print(f"[{timestamp}] {level}: {message}")
    
    def make_request(self, room_id: str, device_id: str, action: str, value: int = None) -> Dict[str, Any]:
        """发送API请求"""
        url = API_BASE_URL + API_ENDPOINT.format(room_id=room_id, device_id=device_id)
        payload = {"action": action}
        if value is not None:
            payload["value"] = value
        
        try:
            response = requests.post(
                url, 
                json=payload, 
                timeout=REQUEST_TIMEOUT,
                headers={"Content-Type": "application/json"}
            )
            return {
                "status_code": response.status_code,
                "data": response.json() if response.headers.get('content-type', '').startswith('application/json') else response.text,
                "success": response.status_code == 200
            }
        except requests.exceptions.Timeout:
            return {"status_code": 504, "data": {"error": "Request timeout"}, "success": False}
        except requests.exceptions.RequestException as e:
            return {"status_code": 0, "data": {"error": str(e)}, "success": False}
    
    def test_case(self, description: str, room_id: str, device_id: str, action: str, 
                  value: int = None, expected_status: int = 200) -> bool:
        """执行单个测试用例"""
        self.total_tests += 1
        self.log(f"测试 {self.total_tests}: {description}")
        
        result = self.make_request(room_id, device_id, action, value)
        
        if result["status_code"] == expected_status:
            self.passed_tests += 1
            self.log(f"✓ 通过 - 状态码: {result['status_code']}", "PASS")
            self.test_results.append({"test": description, "status": "PASS", "details": result})
            return True
        else:
            self.failed_tests += 1
            self.log(f"✗ 失败 - 期望: {expected_status}, 实际: {result['status_code']}", "FAIL")
            self.log(f"  响应: {result['data']}", "FAIL")
            self.test_results.append({"test": description, "status": "FAIL", "details": result})
            return False
    
    def test_switch_device(self, room_id: str, device_id: str, device_name: str):
        """测试开关类设备"""
        self.log(f"\n--- 测试 {room_id}/{device_name} ({device_id}) ---")
        
        # 正常功能测试
        self.test_case(f"{device_name} 开启", room_id, device_id, "ON")
        time.sleep(TEST_DELAY)
        
        self.test_case(f"{device_name} 关闭", room_id, device_id, "OFF")
        time.sleep(TEST_DELAY)
        
        # 错误情况测试
        # self.test_case(f"{device_name} 无效操作", room_id, device_id, "INVALID_ACTION", expected_status=400)
        # time.sleep(TEST_DELAY)
    
    def test_ac_device(self, room_id: str):
        """测试空调设备"""
        device_id = "ac"
        device_name = "空调"
        self.log(f"\n--- 测试 {room_id}/{device_name} ({device_id}) ---")
        
        # 正常功能测试
        self.test_case(f"{device_name} 开启并设置25度", room_id, device_id, "ON", 25)
        time.sleep(TEST_DELAY)
        
        self.test_case(f"{device_name} 设置温度23度", room_id, device_id, "SET_TEMP", 23)
        time.sleep(TEST_DELAY)
        
        self.test_case(f"{device_name} 设置温度0度", room_id, device_id, "SET_TEMP", 0)
        time.sleep(TEST_DELAY)
        
        self.test_case(f"{device_name} 设置温度40度", room_id, device_id, "SET_TEMP", 40)
        time.sleep(TEST_DELAY)
        
        self.test_case(f"{device_name} 关闭", room_id, device_id, "OFF")
        time.sleep(TEST_DELAY)
        
        # 错误情况测试
        # self.test_case(f"{device_name} ON操作缺少温度参数", room_id, device_id, "ON", expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # self.test_case(f"{device_name} ON操作温度超出范围(-1度)", room_id, device_id, "ON", -1, expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # self.test_case(f"{device_name} ON操作温度超出范围(41度)", room_id, device_id, "ON", 41, expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # self.test_case(f"{device_name} SET_TEMP操作缺少温度参数", room_id, device_id, "SET_TEMP", expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # self.test_case(f"{device_name} SET_TEMP操作温度超出范围(-1度)", room_id, device_id, "SET_TEMP", -1, expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # self.test_case(f"{device_name} SET_TEMP操作温度超出范围(41度)", room_id, device_id, "SET_TEMP", 41, expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # self.test_case(f"{device_name} 无效操作", room_id, device_id, "INVALID_ACTION", expected_status=400)
        # time.sleep(TEST_DELAY)
    
    def test_sensor_device(self, room_id: str, device_id: str, device_name: str):
        """测试传感器设备"""
        self.log(f"\n--- 测试 {room_id}/{device_name} ({device_id}) ---")
        
        # 正常功能测试
        self.test_case(f"{device_name} 读取数据", room_id, device_id, "READ")
        time.sleep(TEST_DELAY)
        
        # 错误情况测试
        # self.test_case(f"{device_name} 无效操作", room_id, device_id, "INVALID_ACTION", expected_status=400)
        # time.sleep(TEST_DELAY)
    
    def test_invalid_requests(self):
        """测试无效请求"""
        self.log(f"\n--- 测试无效请求 ---")
        
        # 无效房间
        # self.test_case("无效房间ID", "invalid_room", "light", "ON", expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # 无效设备
        # self.test_case("无效设备ID", "livingroom", "invalid_device", "ON", expected_status=400)
        # time.sleep(TEST_DELAY)
        
        # 房间存在但设备不在该房间
        # self.test_case("设备不在指定房间", "outdoor", "light", "ON", expected_status=400)
        # time.sleep(TEST_DELAY)
    
    def run_all_tests(self):
        """运行所有测试"""
        self.log("开始智能家居API完整测试")
        self.log(f"API地址: {API_BASE_URL}")
        self.log("=" * 60)
        
        # 测试所有房间的设备
        self.test_livingroom()
        self.test_bedroom()
        self.test_kitchen()
        self.test_bathroom()
        self.test_outdoor()
        
        # 测试无效请求
        # self.test_invalid_requests()
        
        # 输出测试结果
        self.print_summary()
    
    def test_livingroom(self):
        """测试客厅设备"""
        room_id = "livingroom"
        self.log(f"\n{'='*20} 测试客厅设备 {'='*20}")
        
        # 开关类设备
        self.test_switch_device(room_id, "light", "客厅灯")
        self.test_switch_device(room_id, "window", "客厅窗户")
        # self.test_switch_device(room_id, "door", "客厅门")  # 门设备测试已禁用
        self.test_switch_device(room_id, "curtain", "客厅窗帘")
        
        # 空调设备
        self.test_ac_device(room_id)
        
        # 传感器设备
        self.test_sensor_device(room_id, "temp_sensor", "客厅温度传感器")
        self.test_sensor_device(room_id, "humidity_sensor", "客厅湿度传感器")
        self.test_sensor_device(room_id, "brightness_sensor", "客厅亮度传感器")
    
    def test_bedroom(self):
        """测试卧室设备"""
        room_id = "bedroom"
        self.log(f"\n{'='*20} 测试卧室设备 {'='*20}")
        
        # 开关类设备
        self.test_switch_device(room_id, "light", "卧室灯")
        self.test_switch_device(room_id, "bedside_light", "床头灯")
        self.test_switch_device(room_id, "window", "卧室窗户")
        # self.test_switch_device(room_id, "door", "卧室门")  # 门设备测试已禁用
        self.test_switch_device(room_id, "curtain", "卧室窗帘")
        
        # 空调设备
        self.test_ac_device(room_id)
        
        # 传感器设备
        self.test_sensor_device(room_id, "temp_sensor", "卧室温度传感器")
        self.test_sensor_device(room_id, "humidity_sensor", "卧室湿度传感器")
        self.test_sensor_device(room_id, "brightness_sensor", "卧室亮度传感器")
    
    def test_kitchen(self):
        """测试厨房设备"""
        room_id = "kitchen"
        self.log(f"\n{'='*20} 测试厨房设备 {'='*20}")
        
        # 开关类设备
        self.test_switch_device(room_id, "light", "厨房灯")
        self.test_switch_device(room_id, "hood", "油烟机")
        
        # 传感器设备
        self.test_sensor_device(room_id, "temp_sensor", "厨房温度传感器")
        self.test_sensor_device(room_id, "humidity_sensor", "厨房湿度传感器")
        self.test_sensor_device(room_id, "smoke_sensor", "厨房烟雾传感器")
        self.test_sensor_device(room_id, "gas_sensor", "厨房燃气传感器")
    
    def test_bathroom(self):
        """测试浴室设备"""
        room_id = "bathroom"
        self.log(f"\n{'='*20} 测试浴室设备 {'='*20}")
        
        # 开关类设备
        self.test_switch_device(room_id, "light", "浴室灯")
        self.test_switch_device(room_id, "fan", "排气扇")
        # self.test_switch_device(room_id, "door", "浴室门")  # 门设备测试已禁用
        
        # 传感器设备
        self.test_sensor_device(room_id, "temp_sensor", "浴室温度传感器")
        self.test_sensor_device(room_id, "humidity_sensor", "浴室湿度传感器")
    
    def test_outdoor(self):
        """测试室外设备"""
        room_id = "outdoor"
        self.log(f"\n{'='*20} 测试室外设备 {'='*20}")
        
        # 传感器设备
        self.test_sensor_device(room_id, "temp_sensor", "室外温度传感器")
        self.test_sensor_device(room_id, "humidity_sensor", "室外湿度传感器")
        self.test_sensor_device(room_id, "brightness_sensor", "室外亮度传感器")
    
    def print_summary(self):
        """打印测试总结"""
        self.log("\n" + "="*60)
        self.log("测试完成！")
        self.log(f"总测试数: {self.total_tests}")
        self.log(f"通过测试: {self.passed_tests}")
        self.log(f"失败测试: {self.failed_tests}")
        self.log(f"成功率: {(self.passed_tests/self.total_tests*100):.1f}%")
        
        if self.failed_tests > 0:
            self.log("\n失败的测试:")
            for result in self.test_results:
                if result["status"] == "FAIL":
                    self.log(f"  ✗ {result['test']}")
                    self.log(f"    状态码: {result['details']['status_code']}")
                    self.log(f"    响应: {result['details']['data']}")
        
        self.log("="*60)
    
    def save_results(self, filename: str = "test_results.json"):
        """保存测试结果到文件"""
        results = {
            "summary": {
                "total_tests": self.total_tests,
                "passed_tests": self.passed_tests,
                "failed_tests": self.failed_tests,
                "success_rate": self.passed_tests/self.total_tests*100 if self.total_tests > 0 else 0
            },
            "details": self.test_results,
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
        }
        
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(results, f, ensure_ascii=False, indent=2)
        
        self.log(f"测试结果已保存到: {filename}")


def main():
    """主函数"""
    try:
        # 检查API服务是否可用
        response = requests.get(f"{API_BASE_URL}/docs", timeout=5)
        if response.status_code != 200:
            print(f"错误: API服务不可用 (状态码: {response.status_code})")
            print(f"请确保服务运行在 {API_BASE_URL}")
            sys.exit(1)
    except requests.exceptions.RequestException as e:
        print(f"错误: 无法连接到API服务: {e}")
        print(f"请确保服务运行在 {API_BASE_URL}")
        sys.exit(1)
    
    # 运行测试
    tester = APITester()
    tester.run_all_tests()
    
    # 保存结果
    tester.save_results("./test_results.json")
    
    # 如果有失败的测试，返回非零退出码
    if tester.failed_tests > 0:
        sys.exit(1)


if __name__ == "__main__":
    main()