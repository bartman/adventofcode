#!/usr/bin/python3

with open('input.txt', 'r') as f:
    txt = f.readline()

floor = 0
for c in txt:
    if c == '(':
        floor+=1
    elif c == ')':
        floor-=1

print(floor)
