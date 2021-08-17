#!/usr/bin/python3

with open('input.txt', 'r') as f:
    lines = f.readlines()

def ribbon(dimms):
    s = sorted(dimms)
    return (2*(s[0]+s[1])) + (s[0]*s[1]*s[2])

total = 0
for line in lines:
    dimms = [ int(x) for x in line.split('x') ]
    total += ribbon(dimms)

print(total)
