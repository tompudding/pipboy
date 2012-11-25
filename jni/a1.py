import sys,re,binascii

def ToNum(x):
    out = 0
    for b in x:
        out <<= 8
        out |= ord(b)
    print hex(out)
    return out

data = sys.stdin.read()

all = 0xffffffffffffffffffffffffffffffff

for word in re.findall('................',data):
    print binascii.hexlify(word)
    all &= ToNum(word)

print hex(all)
