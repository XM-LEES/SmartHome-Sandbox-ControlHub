# 全局请求管理器，负责追踪API请求与设备回执。

import asyncio

class RequestManager:
    # 单例模式，确保全局只有一个RequestManager实例
    _instance = None
    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super().__new__(cls)
            cls._requests = {}  # _requests: key为correlation_id，value为asyncio.Event对象
            cls._results = {}   # _results: 临时存放设备返回的结果
        return cls._instance

    def start_request(self, correlation_id: str) -> asyncio.Event:
        """
        新的API请求到来时调用，创建一个可等待的Event。
        """
        event = asyncio.Event()
        self._requests[correlation_id] = event
        print(f"[RequestManager] Started tracking request: {correlation_id}")
        return event

    def finish_request(self, correlation_id: str, result: dict):
        """
        当MQTT客户端收到设备回执时调用，根据correlation_id找到对应的Event，唤醒并保存结果。
        """
        event = self._requests.get(correlation_id)
        if event:
            print(f"[RequestManager] Received result for: {correlation_id}")
            # 存储从设备返回的结果，以便API函数可以获取它
            self._results[correlation_id] = result
            # 唤醒正在等待它的那个API函数
            event.set()
            del self._requests[correlation_id]
        else:
            print(f"[RequestManager] Warning: Received result for an unknown or timed-out request: {correlation_id}")

    def get_result(self, correlation_id: str) -> dict:
        """
        API函数被唤醒后，调用此方法来获取设备返回的结果。
        """
        # pop方法会获取结果并从字典中删除它，完成清理工作
        return self._results.pop(correlation_id, None)

# 创建全局唯一实例
request_manager = RequestManager()