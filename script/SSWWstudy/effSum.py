# This is to sum signal efficiencies from all SRs

with open("eff.txt") as f1:
  lines = f1.readlines()

SSWW_list = []
DY_list = []
VBF_list = []

for i in range(len(lines)):
  if not ':' in lines[i]: pass
  if 'SSWW' in lines[i].split(':')[0]:
    SSWW=float(lines[i+1].split(':')[1])+float(lines[i+2].split(':')[1])+float(lines[i+3].split(':')[1])+float(lines[i+4].split(':')[1])+float(lines[i+5].split(':')[1])
    SSWW_list.append(SSWW)
    SSWW=float(lines[i+1].split(':')[2])+float(lines[i+2].split(':')[2])+float(lines[i+3].split(':')[2])+float(lines[i+4].split(':')[2])+float(lines[i+5].split(':')[2])
    SSWW_list.append(SSWW)
    SSWW=float(lines[i+1].split(':')[3])+float(lines[i+2].split(':')[3])+float(lines[i+3].split(':')[3])+float(lines[i+4].split(':')[3])+float(lines[i+5].split(':')[3])
    SSWW_list.append(SSWW)
    SSWW=float(lines[i+1].split(':')[4])+float(lines[i+2].split(':')[4])+float(lines[i+3].split(':')[4])+float(lines[i+4].split(':')[4])+float(lines[i+5].split(':')[4])
    SSWW_list.append(SSWW)
  if 'DY' in lines[i].split(':')[0]:
    DY=float(lines[i+1].split(':')[1])+float(lines[i+2].split(':')[1])+float(lines[i+3].split(':')[1])+float(lines[i+4].split(':')[1])+float(lines[i+5].split(':')[1])
    DY_list.append(DY)
    DY=float(lines[i+1].split(':')[2])+float(lines[i+2].split(':')[2])+float(lines[i+3].split(':')[2])+float(lines[i+4].split(':')[2])+float(lines[i+5].split(':')[2])
    DY_list.append(DY)
    DY=float(lines[i+1].split(':')[3])+float(lines[i+2].split(':')[3])+float(lines[i+3].split(':')[3])+float(lines[i+4].split(':')[3])+float(lines[i+5].split(':')[3])
    DY_list.append(DY)
    DY=float(lines[i+1].split(':')[4])+float(lines[i+2].split(':')[4])+float(lines[i+3].split(':')[4])+float(lines[i+4].split(':')[4])+float(lines[i+5].split(':')[4])
    DY_list.append(DY)
  if 'VBF' in lines[i].split(':')[0]:
    VBF=float(lines[i+1].split(':')[1])+float(lines[i+2].split(':')[1])+float(lines[i+3].split(':')[1])+float(lines[i+4].split(':')[1])+float(lines[i+5].split(':')[1])
    VBF_list.append(VBF)
    VBF=float(lines[i+1].split(':')[2])+float(lines[i+2].split(':')[2])+float(lines[i+3].split(':')[2])+float(lines[i+4].split(':')[2])+float(lines[i+5].split(':')[2])
    VBF_list.append(VBF)
    VBF=float(lines[i+1].split(':')[3])+float(lines[i+2].split(':')[3])+float(lines[i+3].split(':')[3])+float(lines[i+4].split(':')[3])+float(lines[i+5].split(':')[3])
    VBF_list.append(VBF)
    VBF=float(lines[i+1].split(':')[4])+float(lines[i+2].split(':')[4])+float(lines[i+3].split(':')[4])+float(lines[i+4].split(':')[4])+float(lines[i+5].split(':')[4])
    VBF_list.append(VBF)

print "SSWW :", SSWW_list
print "DY :", DY_list
print "VBF :", VBF_list
