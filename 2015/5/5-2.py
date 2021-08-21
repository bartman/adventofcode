#!/usr/bin/python3

with open('input.txt', 'r') as f:
    lines = f.readlines()

vowels = 'aeiou'

def isnice(txt):
    cond1 = False
    cond2 = False
    for i in range(len(txt)):
        if i and not cond1:
            seek='%c%c' % (txt[i-1],txt[i])
            if txt.find(seek,i+1) != -1:
                #print('cond1', i, seek)
                cond1 = True
        if i>1 and not cond2:
            if txt[i-2] == txt[i]:
                #print('cond2', i, txt[i-2], txt[i-1], txt[i])
                cond2 = True
    #print(txt, cond2, cond2)
    return cond1 and cond2

nice=0
for _line in lines:
    line = _line.rstrip()
    if isnice(line):
        nice+=1

print(nice)

