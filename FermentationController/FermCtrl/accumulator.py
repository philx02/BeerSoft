from collections import deque
from numbers import Number
import math

class Accumulator:
    def __init__(self, size):
        self.values = deque()
        self.size = 0.0
        self.total = 0.0
        self.mean = 0.0
        self.difference_quotient = 0.0
        if size < 1:
            raise ValueError('Size must me > 0.')
        self.size = size

    def add(self, value):
        if not isinstance(value, Number) or math.isnan(value):
            return
        self.values.append(value)
        self.total += value
        if len(self.values) > self.size:
            self.total -= self.values[0]
            self.values.popleft()
        self.mean = self.total / float(len(self.values))
        if len(self.values) >= 2:
            self.difference_quotient = self.values[len(self.values) - 1] - self.values[len(self.values) - 2]

    def get_mean(self):
        return self.mean

    def get_total(self):
        return self.total
    
    def last_value(self):
        return self.values[len(self.values) - 1]

    def get_difference_quotient(self):
        return self.difference_quotient
