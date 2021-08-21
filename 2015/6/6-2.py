#!/usr/bin/python3

from collections import namedtuple
import re

with open('input.txt', 'r') as f:
    lines = f.readlines()

Point = namedtuple('Point', [ 'col', 'row' ])

class Array:

    def __init__(self, cols, rows):
        self.__cols = cols
        self.__rows = rows
        self.__data = [ [0]*cols for i in range(rows) ]

    def count(self):
        total = 0
        for col in range(self.__cols):
            for row in range(self.__rows):
                total += self.__data[row][col]
        return total

    def set(self,col,row):
        self.__data[row][col] += 1

    def clear(self,col,row):
        if self.__data[row][col]:
            self.__data[row][col] -= 1

    def toggle(self,col,row):
        self.__data[row][col] += 2

    def apply(self,op,p1,p2):
        if op == 'turn on':
            fn = Array.set
        elif op == 'turn off':
            fn = Array.clear
        elif op == 'toggle':
            fn = Array.toggle
        else:
            raise Exception("invalid op '%s'" % op)

        for col in range(p1.col, p2.col+1):
            for row in range(p1.row, p2.row+1):
                fn(self,col,row)

array = Array(1000, 1000)

for line in lines:
    m = re.match('(turn on|turn off|toggle) (\d+),(\d+) through (\d+),(\d+)', line)
    if m:
        p1 = Point(int(m.group(2)), int(m.group(3)))
        p2 = Point(int(m.group(4)), int(m.group(5)))
        array.apply(m.group(1), p1, p2)
    else:
        raise Exception("failed parsing '%s'" % line.rtrim())


print(array.count())
