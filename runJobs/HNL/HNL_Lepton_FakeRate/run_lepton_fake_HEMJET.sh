analyzer=HNL_Lepton_FakeRate
rundir=HNL_Lepton_FakeRate
mcpath=${SKFlat_WD}/runJobs/HNL/${rundir}/mc_lists/
datapath=${SKFlat_WD}/runJobs/HNL/${rundir}/data_lists/
njobs=400
njobs_data=200
nmax=500
declare  -a era_list=("2018")

declare  -a joblist=("HEMJetFakes")


SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e 2018   --skim SkimTree_HNFakeBDT --userflags HEMJetFakes&
SKFlat.py -a $analyzer  -l $mcpath/MC.txt              -n ${njobs}       --nmax ${nmax}  -e 2018   --skim SkimTree_HNFakeBDT --userflags HEMJetFakesEE&
SKFlat.py -a $analyzer  -l $datapath/DATA_2018_mu.txt  -n ${njobs_data}  --nmax ${nmax}  -e 2018   --skim SkimTree_HNFakeBDT --userflags HEMJetFakes&
SKFlat.py -a $analyzer  -l $datapath/DATA_2018_el.txt  -n ${njobs_data}  --nmax ${nmax}  -e 2018   --skim SkimTree_HNFakeBDT --userflags HEMJetFakesEE&


