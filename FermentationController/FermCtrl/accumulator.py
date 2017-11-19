from collections import deque

class Accumulator:
    __values = deque()
    __size = 0.0
    __total = 0.0
    __mean = 0.0

    def __init__(self, size):
        if size < 1:
            raise ValueError('Size must me > 0.')
        self.size = size

    def add(self, value):
        self.__values.append(value)
        self.__total += value
        if len(self.__values) > self.__size:
            self.__total -= self.__values[0]
            self.__values.popleft()
        self.__mean = self.__total / len(self.__values)

    def mean(self):
        return self.__mean

    def total(self):
        return self.__total
