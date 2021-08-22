#!/usr/bin/python3

from collections import namedtuple
import re
import sys

filename = 'input.txt'
if len(sys.argv) > 1:
    filename = sys.argv[1]

with open(filename, 'r') as f:
    lines = f.readlines()

gates = {}

class Input:
    def __init__(self, in1):
        if in1 and in1.isnumeric():
            self.in1 = int(in1)
        else:
            self.in1 = in1

    def __str__(self):
        return str(self.in1)

    def value(self):
        if type(self.in1) is not int:
            self.in1 = gates[self.in1].value()
        return self.in1

opers = {
        None:      lambda g: g.in1.value(),
        'NOT':     lambda g: (~g.in1.value()) & 0xFFFF,
        'AND':     lambda g: g.in1.value() & g.in2.value(),
        'OR':      lambda g: g.in1.value() | g.in2.value(),
        'LSHIFT':  lambda g: (g.in1.value() << g.in2.value()) & 0xFFFF,
        'RSHIFT':  lambda g: g.in1.value() >> g.in2.value(),
        }

class Gate:
    def __init__(self, op, in1, in2):
        self.op = op
        self.in1 = Input(in1)
        self.in2 = Input(in2)
        self.out = None

    def __str__(self):
        return "%s %s %s (%s)" % (str(self.op), str(self.in1), str(self.in2), str(self.out))

    def value(self):
        if not self.out:
            #print(self.op, self.in1, self.in2)
            op = opers[self.op]
            self.out = op(self)

        return self.out

for line in lines:
    words = line.rstrip().split(' ')

    if len(words) == 3 and words[1] == '->':
        in1 = words[0]
        out = words[2]
        gates[out] = Gate(None, in1, None)

    elif len(words) == 4 and words[2] == '->':
        op  = words[0]
        in1 = words[1]
        out = words[3]
        gates[out] = Gate(op, in1, None)

    elif len(words) == 5 and words[3] == '->':
        in1 = words[0]
        op  = words[1]
        in2 = words[2]
        out = words[4]
        gates[out] = Gate(op, in1, in2)

    else:
        raise Exception("failed parsing: %s", line.rstrip())

print(gates['a'].value())
