#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
智能家居API压力测试脚本

测试API的性能和并发处理能力
"""

import requests
import json
import time
import threading
import statistics
from concurrent.futures import ThreadPoolExecutor, as_completed

API_BASE_URL = "http://127.0.0.1:8000"

class StressTester:
    def __init__(self):
        self.results = []
        self.lock = threading.Lock()
    
    def single_request(self, room_id, device_id, action, value=None):
        """单个请求测试"""
        url = f"{API_BASE_URL}/api/v1/devices/{room_id}/{device_id}/action"
        payload = {"action": action}
        if value is not None:
            payload["value"] = value
        
        start_time = time.time()
        try:
            response = requests.post(url, json=payload, timeout=10)
            end_time = time.time()
            
            result = {
                "success": response.status_code == 200,
                "status_code": response.status_code,
                "response_time": end_time - start_time,
                "request": f"{room_id}/{device_id}/{action}"
            }
        except Exception as e:
            end_time = time.time()
            result = {
                "success": False,
                "status_code": 0,
                "response_time": end_time - start_time,
                "request": f"{room_id}/{device_id}/{action}",
                "error": str(e)
            }
        
        with self.lock:
            self.results.append(result)
        
        return result
    
    def concurrent_test(self, num_threads=10, requests_per_thread=10):
        """并发测试"""
        print(f"开始并发测试: {num_threads} 线程, 每线程 {requests_per_thread} 请求")
        print(f"总请求数: {num_threads * requests_per_thread}")
        
        # 测试请求列表
        test_requests = [
            ("livingroom", "light", "ON"),
            ("livingroom", "light", "OFF"),
            ("bedroom", "ac", "ON", 25),
            ("bedroom", "ac", "SET_TEMP", 23),
            ("kitchen", "temp_sensor", "READ"),
            ("bathroom", "humidity_sensor", "READ"),
        ]
        
        start_time = time.time()
        
        with ThreadPoolExecutor(max_workers=num_threads) as executor:
            futures = []
            
            for thread_id in range(num_threads):
                for req_id in range(requests_per_thread):
                    # 循环使用测试请求
                    req = test_requests[req_id % len(test_requests)]
                    if len(req) == 4:
                        future = executor.submit(self.single_request, req[0], req[1], req[2], req[3])
                    else:
                        future = executor.submit(self.single_request, req[0], req[1], req[2])
                    futures.append(future)
            
            # 等待所有请求完成
            completed = 0
            for future in as_completed(futures):
                completed += 1
                if completed % 10 == 0:
                    print(f"已完成: {completed}/{len(futures)}")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        self.analyze_results(total_time)
    
    def sequential_test(self, num_requests=50):
        """顺序测试"""
        print(f"开始顺序测试: {num_requests} 请求")
        
        test_requests = [
            ("livingroom", "light", "ON"),
            ("livingroom", "light", "OFF"),
            ("bedroom", "ac", "ON", 25),
            ("bedroom", "ac", "OFF"),
            ("kitchen", "temp_sensor", "READ"),
        ]
        
        start_time = time.time()
        
        for i in range(num_requests):
            req = test_requests[i % len(test_requests)]
            if len(req) == 4:
                self.single_request(req[0], req[1], req[2], req[3])
            else:
                self.single_request(req[0], req[1], req[2])
            
            if (i + 1) % 10 == 0:
                print(f"已完成: {i + 1}/{num_requests}")
        
        end_time = time.time()
        total_time = end_time - start_time
        
        self.analyze_results(total_time)
    
    def analyze_results(self, total_time):
        """分析测试结果"""
        if not self.results:
            print("没有测试结果")
            return
        
        # 统计成功/失败
        successful = [r for r in self.results if r["success"]]
        failed = [r for r in self.results if not r["success"]]
        
        # 响应时间统计
        response_times = [r["response_time"] for r in successful]
        
        print("\n" + "="*60)
        print("测试结果分析")
        print("="*60)
        print(f"总请求数: {len(self.results)}")
        print(f"成功请求: {len(successful)}")
        print(f"失败请求: {len(failed)}")
        print(f"成功率: {len(successful)/len(self.results)*100:.1f}%")
        print(f"总耗时: {total_time:.2f}秒")
        print(f"吞吐量: {len(self.results)/total_time:.1f} 请求/秒")
        
        if response_times:
            print(f"\n响应时间统计:")
            print(f"  平均: {statistics.mean(response_times):.3f}秒")
            print(f"  中位数: {statistics.median(response_times):.3f}秒")
            print(f"  最小: {min(response_times):.3f}秒")
            print(f"  最大: {max(response_times):.3f}秒")
            if len(response_times) > 1:
                print(f"  标准差: {statistics.stdev(response_times):.3f}秒")
        
        if failed:
            print(f"\n失败请求分析:")
            status_codes = {}
            for f in failed:
                code = f.get("status_code", "unknown")
                status_codes[code] = status_codes.get(code, 0) + 1
            
            for code, count in status_codes.items():
                print(f"  状态码 {code}: {count} 次")
        
        print("="*60)
    
    def save_detailed_results(self, filename="stress_test_results.json"):
        """保存详细结果"""
        if not self.results:
            return
        
        # 计算统计信息
        successful = [r for r in self.results if r["success"]]
        response_times = [r["response_time"] for r in successful]
        
        detailed_results = {
            "summary": {
                "total_requests": len(self.results),
                "successful_requests": len(successful),
                "failed_requests": len(self.results) - len(successful),
                "success_rate": len(successful)/len(self.results)*100,
                "avg_response_time": statistics.mean(response_times) if response_times else 0,
                "median_response_time": statistics.median(response_times) if response_times else 0,
                "min_response_time": min(response_times) if response_times else 0,
                "max_response_time": max(response_times) if response_times else 0,
            },
            "details": self.results,
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
        }
        
        with open(f"test/{filename}", 'w', encoding='utf-8') as f:
            json.dump(detailed_results, f, ensure_ascii=False, indent=2)
        
        print(f"详细结果已保存到: test/{filename}")

def main():
    """主测试函数"""
    print("智能家居API压力测试")
    print("="*60)
    
    # 检查服务可用性
    try:
        response = requests.get(f"{API_BASE_URL}/docs", timeout=5)
        if response.status_code != 200:
            print(f"API服务不可用，状态码: {response.status_code}")
            return
    except Exception as e:
        print(f"无法连接到API服务: {e}")
        return
    
    tester = StressTester()
    
    print("选择测试类型:")
    print("1. 顺序测试 (50个请求)")
    print("2. 轻度并发测试 (5线程 x 10请求)")
    print("3. 中度并发测试 (10线程 x 20请求)")
    print("4. 重度并发测试 (20线程 x 30请求)")
    print("5. 自定义测试")
    
    choice = input("请选择 (1-5): ").strip()
    
    if choice == "1":
        tester.sequential_test(50)
    elif choice == "2":
        tester.concurrent_test(5, 10)
    elif choice == "3":
        tester.concurrent_test(10, 20)
    elif choice == "4":
        tester.concurrent_test(20, 30)
    elif choice == "5":
        try:
            threads = int(input("线程数: "))
            requests_per_thread = int(input("每线程请求数: "))
            tester.concurrent_test(threads, requests_per_thread)
        except ValueError:
            print("输入无效，使用默认参数")
            tester.concurrent_test(10, 10)
    else:
        print("无效选择，执行默认测试")
        tester.concurrent_test(10, 10)
    
    # 保存结果
    tester.save_detailed_results()

if __name__ == "__main__":
    main()