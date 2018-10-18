from collections import defaultdict
import pandas as pd

d = defaultdict(list)

with open("testd.txt") as fin:
    for line in fin:
        k,v = line.strip().split(" ")
        d[k].append(v)
print d

df=pd.Series(d)
print df