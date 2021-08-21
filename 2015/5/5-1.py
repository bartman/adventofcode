#!/usr/bin/python3

with open('input.txt', 'r') as f:
    lines = f.readlines()

vowels = 'aeiou'

def isnice(txt):
    vowelcnt = 0
    repeated = False
    prev = None
    for ch in txt:
        if ch in vowels:
            vowelcnt += 1
        if ch == prev:
            repeated = True
        if prev and '%c%c' % (prev,ch) in [ 'ab', 'cd', 'pq', 'xy' ]:
            return False
        prev = ch
    return vowelcnt > 2 and repeated

nice=0
for _line in lines:
    line = _line.rstrip()
    if isnice(line):
        nice+=1

print(nice)

