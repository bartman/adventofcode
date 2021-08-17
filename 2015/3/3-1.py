#!/usr/bin/python3

with open('input.txt', 'r') as f:
    txt = f.read()

x=0
y=0
visited = { (x,y): 1 }
for ch in txt:
    if ch == '^':
        y+=1
    elif ch == 'v':
        y-=1
    elif ch == '>':
        x+=1
    elif ch == '<':
        x-=1
    else:
        raise Exception("invalid ch='%c'" % ch)
    key="%u,%u" % (x,y)
    if key not in visited:
        visited[key] = 1
    else:
        visited[key] += 1

print(len(visited.keys()))

