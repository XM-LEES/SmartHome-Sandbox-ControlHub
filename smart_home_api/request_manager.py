import asyncio

class RequestManager:
    _instance = None
    _requests = None
    _results = None

    def __new__(cls, *args, **kwargs):
        if not cls._instance:
            cls._instance = super().__new__(cls)
            cls._requests = {}
            cls._results = {}
        return cls._instance

    def start_request(self, correlation_id):
        event = asyncio.Event()
        self._requests[correlation_id] = event
        return event

    def finish_request(self, correlation_id, result=None):
        event = self._requests.get(correlation_id)
        if event:
            self._results[correlation_id] = result
            event.set()
            del self._requests[correlation_id]

    def get_result(self, correlation_id):
        return self._results.pop(correlation_id, None)

request_manager = RequestManager() 