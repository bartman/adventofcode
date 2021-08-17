#!/usr/bin/python3

import sys

def main():
    with open('input.txt', 'r') as f:
        txt = f.readline()

    floor = 0
    index = 0
    for c in txt:
        if c == '(':
            floor+=1
        elif c == ')':
            floor-=1
        index+=1
        if floor < 0:
            print(index)
            return

    print(floor)

if __name__ == '__main__':
    main()
