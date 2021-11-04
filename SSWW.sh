#!/bin/bash
############################################
### SR, CR
############################################

### Test ###
#python python/SKFlat.py -a SSWW -y 2016 -i DoubleMuon:B_ver2 -n 50 --skim SkimTree_Dilepton --batchname auto &

### MC ###
#python python/SKFlat.py -a Signal -y 2016 -l submitList/Dilepton_SR_2016.txt -n 80 --skim SkimTree_Dilepton &

#python python/SKFlat.py -a Control -y 2016 -i ZGTo2LG -n 50 --skim SkimTree_Dilepton --nmax 80 &

### CF ###
#python python/SKFlat.py -a Signal -y 2016 -l submitList/2016_DoubleEG_BtoH.txt -n 80 --skim SkimTree_Dilepton --userflags RunCF &

### Fake ###
#python python/SKFlat.py -a Signal -y 2016 -l submitList/2016_DoubleMuon_BtoG.txt -n 80 --skim SkimTree_Dilepton --userflags RunFake &
#python python/SKFlat.py -a Signal -y 2016 -l submitList/2016_DoubleEG_BtoH.txt -n 80 --skim SkimTree_Dilepton --userflags RunFake &
#python python/SKFlat.py -a Signal -y 2016 -l submitList/2016_MuonEG_BtoG.txt -n 80 --skim SkimTree_Dilepton --userflags RunFake &
#python python/SKFlat.py -a Signal_2016H -y 2016 -l submitList/2016_periodH.txt -n 80 --skim SkimTree_Dilepton --userflags RunFake &

### DATA ###
#python python/SKFlat.py -a SSWW -y 2016 -l submitList/2016_DoubleMuon_BtoH.txt -n 50 --skim SkimTree_Dilepton --batchname auto &
#python python/SKFlat.py -a SSWW -y 2017 -l submitList/2017_DoubleMuon.txt -n 50 --skim SkimTree_Dilepton --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -l submitList/2018_DoubleMuon.txt -n 50 --skim SkimTree_Dilepton --batchname auto &

### signal ###
#python python/SKFlat.py -a SSWW -y 2018 -l submitList/SSWWTypeI.txt -n 50 --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -l submitList/SSWWTypeI.txt -n 50 --userflags jcln_inv --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -l submitList/SSWWTypeI.txt -n 50 --userflags fatjet_veto --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -l submitList/SSWWTypeI.txt -n 50 --userflags jcln_inv,fatjet_veto --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i DYTypeI_SS_MuMu_M1500 -n 50 --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i VBFTypeI_SS_MuMu_M1500 -n 50 --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i DYTypeI_SS_MuMu_M1500 -n 50 --userflags jcln_inv --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i VBFTypeI_SS_MuMu_M1500 -n 50 --userflags jcln_inv --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i DYTypeI_SS_MuMu_M1500 -n 50 --userflags fatjet_veto --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i VBFTypeI_SS_MuMu_M1500 -n 50 --userflags fatjet_veto --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i DYTypeI_SS_MuMu_M1500 -n 50 --userflags jcln_inv,fatjet_veto --batchname auto &
#python python/SKFlat.py -a SSWW -y 2018 -i VBFTypeI_SS_MuMu_M1500 -n 50 --userflags jcln_inv,fatjet_veto --batchname auto &

### fake measurement ###

#python python/SKFlat.py -a Fake -y 2016 -i SingleMuon:B_ver2 -n 50 --batchname auto &
#python python/SKFlat.py -a Fake -y 2016 -i SingleElectron:B_ver2 -n 50 --batchname auto &
#python python/SKFlat.py -a Fake -y 2016 -i DoubleMuon:B_ver2 -n 200 --userflags FR --batchname auto &
#python python/SKFlat.py -a Fake -y 2016 -i DoubleEG:B_ver2 -n 50 --userflags FR --batchname auto &
#python python/SKFlat.py -a Fake -y 2016 -l submitList/2016_SingleMuon_BtoH.txt -n 50 --batchname auto &
#python python/SKFlat.py -a Fake -y 2016 -l submitList/2016_SingleElectron_BtoH.txt -n 50 --batchname auto &
python python/SKFlat.py -a Fake -y 2016 -l submitList/2016_DoubleMuon_BtoH.txt --userflags FR,Norm -n 50 --batchname auto &
python python/SKFlat.py -a Fake -y 2016 -l submitList/2016_DoubleEG_BtoH.txt --userflags FR,Norm -n 50 --batchname auto &
python python/SKFlat.py -a Fake -y 2016 -l submitList/Fake_Norm_MC.txt --userflags FR,Norm -n 50 --batchname auto &

