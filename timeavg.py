from collections import deque
import numpy as np

class TimeAvg():
    def __init__(self, delta_t):
        self.delta_t = delta_t
        self.stream = deque()

    def append(self, datapoint):
        stream = self.stream
        stream.append(datapoint)    # appends on the right
        length = len(stream)
        if length < 1:
            return -1
        real_delta_t = stream[length - 1][1] - stream[0][1]
        while real_delta_t > self.delta_t:
            stream.popleft()
            length -= 1
            real_delta_t = stream[length - 1][1] - stream[0][1]

    def calculate(self):
        stream = self.stream
        length = len(stream)
        if length < 1:
            return -1
        real_delta_t = stream[length - 1][1] - stream[0][1]
        while real_delta_t > self.delta_t:
            stream.popleft()
            length -= 1
            real_delta_t = stream[length - 1][1] - stream[0][1]
        y = np.array(stream)[:, 0]
        t = np.array(stream)[:, 1]
        return np.trapz(y, x=t) / (real_delta_t)


if __name__ == '__main__':
    import time
    import random
    reading = TimeAvg(3)
    t0 = time.time()
    while time.time() < t0 + 15:
        reading.append([random.randint(0, 10), time.time()])
        print(reading.calculate())
