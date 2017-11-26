from collections import deque

class Accumulator:
    def __init__(self, size):
        self.values = deque()
        self.size = 0.0
        self.total = 0.0
        self.mean = 0.0
        if size < 1:
            raise ValueError('Size must me > 0.')
        self.size = size

    def add(self, value):
        self.values.append(value)
        self.total += value
        if len(self.values) > self.size:
            self.total -= self.values[0]
            self.values.popleft()
        self.mean = self.total / float(len(self.values))

    def get_mean(self):
        return self.mean

    def get_total(self):
        return self.total
