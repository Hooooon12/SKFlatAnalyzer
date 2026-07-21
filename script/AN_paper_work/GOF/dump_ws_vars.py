import ROOT
import sys

f = ROOT.TFile.Open(sys.argv[1])
w = f.Get("w")

vars = w.allVars()
it = vars.createIterator()

names = []
v = it.Next()
while v:
    names.append(v.GetName())
    v = it.Next()

for name in sorted(names):
    print(name)
