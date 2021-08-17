#!/usr/bin/python3

with open('input.txt', 'r') as f:
    txt = f.read()

class State:
    def __init__(self,x,y):
        self.x=x
        self.y=y

    def key(self):
        return "%u,%u" % (self.x, self.y)

    def move(self,ch):
        if ch == '^':
            self.y+=1
        elif ch == 'v':
            self.y-=1
        elif ch == '>':
            self.x+=1
        elif ch == '<':
            self.x-=1
        else:
            raise Exception("invalid ch='%c'" % ch)

class Visited:
    def __init__(self):
        self.map = {}

    def update(self, state):
        key = state.key()
        if key not in self.map:
            self.map[key] = 1
        else:
            self.map[key] += 1

    def unique(self):
        return len(self.map.keys())

states = [ State(0,0), State(0,0) ]
visited = Visited()
visited.update(states[0])
visited.update(states[1])

who = 0

for ch in txt:
    states[who].move(ch)
    visited.update(states[who])

    who = (who + 1) % 2

print(visited.unique())

