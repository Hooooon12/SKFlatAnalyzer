import itertools, os

#groups = {
#    "inv": ["sr1_inv", "sr2_inv", "sr3_inv"],
#    "cf": ["cf_cr1", "cf_cr2", "cf_cr3"],
#    "wz": ["wz_cr1", "wz_cr2", "wz_cr3"],
#    "zg": ["zg_cr3"],
#    "zz": ["zz_cr3"],
#}
#
#min_group_size = 0
#max_group_size = len(groups) 
##max_group_size = 2
#
#all_combinations = []
#for size in range(min_group_size, max_group_size + 1):
#  for group_subset in itertools.combinations(groups.keys(), size):
#    selected_CR = sum((groups[group] for group in group_subset), [])
#    all_combinations.append(selected_CR)

#print all_combinations
#exit()

#all_combinations=[["sr1_inv","sr2_inv","sr3_inv","cf_cr1","cf_cr2","cf_cr3","zg_cr3","wz_cr1","wz_cr2","wz_cr3","zz_cr3"]] # This is the nominal
all_combinations=[["sr2_inv","zg_cr3","wz_cr2","zz_cr3"],["sr2_inv","zg_cr3","wz_cr2","wz_cr3","zz_cr3"],["sr2_inv","sr3_inv","zg_cr3","wz_cr2","zz_cr3"],["sr2_inv","sr3_inv","zg_cr3","wz_cr2","wz_cr3","zz_cr3"]]

for cr_list in all_combinations:
  cmd = "python MakeDataCard_ANv3.py --CR " + ' '.join(cr_list) + " --Syst --Decorr --JetDecorr"
  print "Running:",cmd
  os.system(cmd)
  cmd = "python MakeDataCard_ANv3.py --Combine CR --CR " + ' '.join(cr_list) + " --Syst --Decorr --JetDecorr" if len(cr_list)!=0 else "python MakeDataCard_ANv3.py --Combine SR --Syst --Decorr --JetDecorr"
  print "Running:",cmd
  os.system(cmd)
  cmd = "python MakeDataCard_ANv3.py --Combine Era --CR " + ' '.join(cr_list) + " --Syst --Decorr --JetDecorr"
  print "Running:",cmd
  os.system(cmd)
