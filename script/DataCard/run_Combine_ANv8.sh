##### ANv6 legacy #####

#python MakeDataCard_ANv6.py --CR --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv6.py --Combine CR --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv6.py --Combine Era --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Work 
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --pdf

#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Breakdown
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Breakdown --pdf

#python MakeRunList.py ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Limit 
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic

#python MakeRunList.py ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Work -s DY VBF SSWW
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Limit -s DY VBF SSWW
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic


#python MakeDataCard_ANv6.py --CR --Syst --Decorr --JetDecorr --Ext
#python MakeDataCard_ANv6.py --Combine CR --Syst --Decorr --JetDecorr --Ext
#python MakeDataCard_ANv6.py --Combine Era --CR --Syst --Decorr --JetDecorr --Ext

#python MakeRunList.py ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Work --Ext
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Limit --Ext
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic



# GOF TEST
#python MakeRunList.py ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Work -m 1000 -c MuMu
#./create-batch.py -l RunList_Run2_ANv6_FixSyst_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --GOF





##### ANv6: Comparison to the consistent binning

#python MakeDataCard_ANv6.py --CR --Syst --Decorr --JetDecorr -c MuMu -s ""
#python MakeDataCard_ANv6.py --Combine CR --Syst --Decorr --JetDecorr -c MuMu -s ""
#python MakeDataCard_ANv6.py --Combine Era --CR --Syst --Decorr --JetDecorr -c MuMu -s ""
#
#python MakeRunList.py ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 -c MuMu --Work 
#./create-batch.py -l RunList_Run2_ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 -c MuMu --Limit
#./create-batch.py -l RunList_Run2_ANv6_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Asymptotic




##### ANv7: Fixed Muon RECO SF to be pt-dependent

## SingularBinning SR-combined run

#python MakeDataCard_ANv7.py --CR --Syst --Decorr --JetDecorr -s ""
#python MakeDataCard_ANv7.py --Combine CR --Syst --Decorr --JetDecorr -s ""
#python MakeDataCard_ANv7.py --Combine Era --CR --Syst --Decorr --JetDecorr -s ""

#python MakeRunList.py ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 --Work 
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 --Limit
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Asymptotic


## SingularBinning SR-dependent runs

#python MakeRunList.py ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Asymptotic




##### ANv7 nominal SR-combined run

#python MakeDataCard_ANv7.py --CR --Syst --Decorr --JetDecorr -s ""
#python MakeDataCard_ANv7.py --Combine CR --Syst --Decorr --JetDecorr -s ""
#python MakeDataCard_ANv7.py --Combine Era --CR --Syst --Decorr --JetDecorr -s ""
#
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Work 
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic

## CLs results

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Limit -m 1000 -c MuMu
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Limit -m 100 10000 -c EE EMu MuMu
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Q3 -t 500

## Impacts

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Work 
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact # B-only
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --pdf
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --InjectSignal 1 # S+B
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --pdf --InjectSignal 1

## Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --CR --Syst --Decorr --JetDecorr -s Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine CR --Syst --Decorr --JetDecorr -s Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine Era --CR --Syst --Decorr --JetDecorr -s Weinberg

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -s Weinberg --Work 
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact # B-only
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --InjectSignal 1 # S+B
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --pdf
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Impact --pdf --InjectSignal 1

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -s Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Q3 -t 500

## Uncertainty breakdowns

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -m 100 1000 10000 --Work  # NOTE ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr doesn't have 'HNL' tag. That's after NewCLs tag.
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -m 100 --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Breakdown
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Breakdown --pdf
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -s Weinberg --Work





## SingularBinning Weinberg

#python MakeDataCard_ANv7.py -wp ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning --CR --Syst --Decorr --JetDecorr -s Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning --Combine CR --Syst --Decorr --JetDecorr -s Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning --Combine Era --CR --Syst --Decorr --JetDecorr -s Weinberg

#python MakeRunList.py ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 -s Weinberg --Work 
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Work

#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Impact # B-only
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Impact --InjectSignal 1 # S+B
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Impact --pdf
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Impact --pdf --InjectSignal 1

#python MakeRunList.py ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr -e Run2 -s Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_SingularBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SingularBinning_Decorr_JetDecorr.txt --Asymptotic



## Nominal SR-dependent runs
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -t SR1 SR2 SR3 --Work 
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Asymptotic


## GOF TEST
#python MakeDataCard_ANv7.py --CR --Syst --Decorr --JetDecorr --Ext               -m M1000 -c MuMu -s ""
#python MakeDataCard_ANv7.py --Combine CR --Syst --Decorr --JetDecorr --Ext       -m M1000 -c MuMu -s ""
#python MakeDataCard_ANv7.py --Combine Era --CR --Syst --Decorr --JetDecorr --Ext -m M1000 -c MuMu -s ""
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 --Work -m 1000 -c MuMu
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --GOF



### Implement parallel CLs
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --CR --Syst --Decorr --JetDecorr               -o NewCLs -s HNL -m 100
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine CR --Syst --Decorr --JetDecorr       -o NewCLs -s HNL -m 100
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine Era --CR --Syst --Decorr --JetDecorr -o NewCLs -s HNL -m 100
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --CR --Syst --Decorr --JetDecorr               -o NewCLs -s HNL -m 10000
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine CR --Syst --Decorr --JetDecorr       -o NewCLs -s HNL -m 10000
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine Era --CR --Syst --Decorr --JetDecorr -o NewCLs -s HNL -m 10000
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --CR --Syst --Decorr --JetDecorr               -o NewCLs -s HNL -m 1000
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine CR --Syst --Decorr --JetDecorr       -o NewCLs -s HNL -m 1000
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst --Combine Era --CR --Syst --Decorr --JetDecorr -o NewCLs -s HNL -m 1000

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 100 --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Work
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 10000 --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Work
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 1000 --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Work

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 100 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Asymptotic
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 10000 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Asymptotic
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 1000 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Asymptotic

## CLs with definite r range

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -c MuMu --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 500 -n 50 --rRange 0.1:0.5:0.05 --Diagnostic
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 500 -n 50 --rRange 0.3:0.4:0.01 --Diagnostic
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 50 --Diagnostic

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 100 -c EE --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 1000 -n 100 --rRange 0.65:0.85:0.02 --Diagnostic
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 1000 -n 100 --rRange 0.7:0.9:0.01 --Diagnostic

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 100 -c EMu --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 1000 -n 100 --rRange 0.25:0.45:0.02 --Diagnostic

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -c MuMu -m 10000 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 1000 -n 100 --rRange 4:6:0.1 --Diagnostic
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -c EE -m 10000 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 1000 -n 100 --rRange 4.5:6.5:0.1 --Diagnostic
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -c EMu -m 10000 --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 1000 -n 100 --rRange 1:3:0.1 --Diagnostic

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs -e Run2 -m 1000 -c EE --Limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewCLs.txt --Q3 -t 1000 -n 100 --rRange 7.5:9.5:0.01 --Diagnostic



### Full JES, prepend CMS_SUS24014, change sr*_Inv* to cr*_Inv* @260128
#python MakeDataCard_ANv7.py -wp ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS --CR --Syst --Decorr               -s HNL Weinberg -c MuMu EE
#python MakeDataCard_ANv7.py -wp ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS --Combine CR --Syst --Decorr       -s HNL Weinberg -c MuMu EE
#python MakeDataCard_ANv7.py -wp ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS --Combine Era --CR --Syst --Decorr -s HNL Weinberg -c MuMu EE

#python MakeRunList.py ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr -e Run2 -c MuMu EE -s HNL Weinberg --Work 
#./create-batch.py -l RunList_Run2_ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr.txt --Work
#./create-batch.py -l RunList_Run2_ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr.txt --Impact # B-only
#./create-batch.py -l RunList_Run2_ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr.txt --Impact --InjectSignal 1 # S+B
#./create-batch.py -l RunList_Run2_ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr.txt --Impact --pdf
#./create-batch.py -l RunList_Run2_ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr.txt --Impact --InjectSignal 1 --pdf

#python MakeRunList.py ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr -e Run2 -c MuMu EE --Limit
#./create-batch.py -l RunList_Run2_ANv7_FullJESNS_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_FullJESNS_Decorr.txt --Asymptotic




### TEST full EMu limit
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull -e Run2 -c EMu --Work  # NOTE M100 is scaled by 0.001 in this WP, DYVBFscaler is 0.1 when MN > 3000 GeV
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --Work

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull -e Run2 -m 100 -c EMu --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --MDfit --r 0 # 2D blind Asymptotic limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --MDfit --r 1 --f 0.5

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull -e Run2 -m 1000 -c EMu --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --MDfit --r 0 # 2D blind Asymptotic limit

#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull -e Run2 -m 10000 -c EMu --Work  # NOTE M100 is scaled by 0.001 in this WP, DYVBFscaler is 0.1 when MN > 3000 GeV
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --MDfit --r 0 # 2D blind Asymptotic limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuFull.txt --MDfit --r 1 --f 0.5




### TEST 3ch limit
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch -e Run2 -c 3ch --Work  # NOTE M100 is scaled by 0.001 in this WP, DYVBFscaler is 0.1 when MN > 3000 GeV
#python MakeRunList.py ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch -e Run2 -m 1000 -c 3ch --Work  # NOTE M100 is scaled by 0.001 in this WP, DYVBFscaler is 0.1 when MN > 3000 GeV
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch.txt --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch.txt --MDfit --r 0 # 2D blind Asymptotic limit
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch.txt --MDfit --r 1 --f 0.5 # signal-injected toy



### ANv7 nominal with prepending CMS_SUS24014, changing sr*_Inv* to cr*_Inv*, and update to include HEM, autoMCStats in the nuisance group @260210 (having wrong ZZ (SSMultiLep skim) in CR...)
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst --Combine Era --CR --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Work

#python MakeRunList.py ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -m 100 1000 10000 --Work
#python MakeRunList.py ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2 -m 100 --Work
#./create-batch.py -l RunList_Run2_ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr.txt --Breakdown

#python PrintUncertTableForPaper.py -wp ANv7_HNL_ULIDv2_NewName_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr -e Run2


### ANv7 EMuCF: include EMu cf, wz pdf/scale uncertainties, wz and wz_ewk split, correct ZZ in CR, data_obs is aligned with tot bkg, 0 error assigned in negative-weight-truncation.
#python MakeDataCard_ANv7.py -wp ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_EMuCF.txt --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_EMuCF.txt --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_EMuCF.txt --Combine Era --CR --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF.txt --Work

#python MakeRunList.py ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF.txt --Asymptotic

#python MakeRunList.py ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF -e Run2 -s HNL -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2_ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF.txt --Breakdown
#python MakeRunList.py ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF -e Run2 -s Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF.txt --Breakdown

#python PrintUncertBreakdown.py -wp ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF
#python PrintUncertTableForPaper.py -wp ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF


## 2017 run ##
#python MakeRunList.py ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF -e 2017 -s HNL Weinberg --Work
#./create-batch.py -l RunList_ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF.txt --Work

#python MakeRunList.py ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF -e 2017 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_EMuCF.txt --Asymptotic





### ANv7 Preapproval: Fake syst now shape, separate QCD scale. Merged: Merged BDT and new SingularBinning. (lepton channel based binning -- but SR1 only)

## 2017 run
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -e 2017 --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -e 2017 --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e 2017 -s HNL Weinberg --Work
#./create-batch.py -l RunList_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e 2017 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic

## 2016 run
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -e 2016preVFP 2016postVFP --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -e 2016preVFP 2016postVFP --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e 2016preVFP 2016postVFP -s HNL Weinberg --Work
#./create-batch.py -l RunList_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e 2016preVFP 2016postVFP -s HNL Weinberg --Limit
#./create-batch.py -l RunList_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic





## Merged
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged -sk card_skeleton_ANv7_Preapproval.txt --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged -sk card_skeleton_ANv7_Preapproval.txt --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Merged_Decorr_JetDecorr_Preapproval.txt --Asymptotic


## Rerun the pre-approval nominal with the full Run2
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --CR               --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine CR       --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic

#Impact
#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact --pdf
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact --InjectSignal 1
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact --InjectSignal 1 --pdf

# and SR-dependently...

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic



## Pre-approval nominal, but include signals in the CRs
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o SigInCR --CR               --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o SigInCR --Combine CR       --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o SigInCR --Combine Era --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_SigInCR_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_SigInCR_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_SigInCR_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_SigInCR_Preapproval.txt --Asymptotic



## BDT trained with OnlySS signal samples
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS -sk card_skeleton_ANv7_Preapproval.txt --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS -sk card_skeleton_ANv7_Preapproval.txt --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_OnlySS_Decorr_JetDecorr_Preapproval.txt --Asymptotic



## Use W mass constraint (mostly for SR1) and Remove central VBF jets (for SR2)
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets -sk card_skeleton_ANv7_Preapproval.txt --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets -sk card_skeleton_ANv7_Preapproval.txt --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval.txt --Asymptotic

# and SR-dependently...

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_UseWMassConstraint_RemoveCentralVBFJets_Decorr_JetDecorr_Preapproval.txt --Asymptotic


## Flav/Flav-Era Dep binning
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep -sk card_skeleton_ANv7_Preapproval.txt --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep -sk card_skeleton_ANv7_Preapproval.txt --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavDep_Decorr_JetDecorr_Preapproval.txt --Asymptotic


#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep -sk card_skeleton_ANv7_Preapproval.txt --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep -sk card_skeleton_ANv7_Preapproval.txt --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR_FlavEraDep_Decorr_JetDecorr_Preapproval.txt --Asymptotic



## Run fake syst as lnN
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval_FakelnN.txt --CR               --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval_FakelnN.txt --Combine CR       --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval_FakelnN.txt --Combine Era --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN.txt --Asymptotic

# Impact
#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN.txt --Impact
#./create-batch.py -l RunList_Run2_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval_FakelnN.txt --Impact --pdf



## SR2 binning refined (2016preVFP/postVFP, 2017/18 share the same SR2 binning) (currently 2016 only) <-- turned out to be wrong (not fixed yet @260407)
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_BinRefinement -sk card_skeleton_ANv7_Preapproval.txt --CR         -e 2016preVFP 2016postVFP --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_BinRefinement -sk card_skeleton_ANv7_Preapproval.txt --Combine CR -e 2016preVFP 2016postVFP --Syst --Decorr --JetDecorr -s HNL Weinberg

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_BinRefinement_Decorr_JetDecorr_Preapproval -e 2016preVFP 2016postVFP -s HNL Weinberg --Work
#./create-batch.py -l RunList_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_BinRefinement_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_BinRefinement_Decorr_JetDecorr_Preapproval -e 2016preVFP 2016postVFP -s HNL Weinberg --Limit
#./create-batch.py -l RunList_ANv7_Preapproval_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_BinRefinement_Decorr_JetDecorr_Preapproval.txt --Asymptotic



## ANv7_L2review (treat low pt prompt in ConversionSplitting, use Tight Bjets in IB CRs @260407)
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --CR         --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine CR --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL -m 100 -c MuMu --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --FitDiag



## ANv7_L2review but test if Combine works with zero data with 2016postVFP CR2 IB EE @260409
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o ZeroData -c EE -m 1000 -s HNL --CR               --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o ZeroData -c EE -m 1000 -s HNL --Combine CR       --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o ZeroData -c EE -m 1000 -s HNL --Combine Era --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_ZeroData_Preapproval -e Run2 -c EE -m 1000 -s HNL --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_ZeroData_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_ZeroData_Preapproval -e Run2 -c EE -m 1000 -s HNL --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_ZeroData_Preapproval.txt --Asymptotic


## ANv7_L2review but FixHessian_AddGluGluTaus (which gives you x10 PDF uncertainty for DY, SSWW, WZ!!) @260418
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --CR         --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine CR --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s HNL -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact --pdf
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Breakdown

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact --pdf
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Breakdown

#python PrintUncertBreakdown.py     -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -s HNL Weinberg
#python PrintUncertTableForPaper.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval

# Signal, SR-dependent limits
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --CR         --Syst --Decorr --JetDecorr -s DY VBF SSWW
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine CR --Syst --Decorr --JetDecorr -s DY VBF SSWW
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt --Combine Era --CR --Syst --Decorr --JetDecorr -s DY VBF SSWW

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s DY VBF SSWW --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -s DY VBF SSWW --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic
#
#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Asymptotic

# EMu M800 impact check
#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval -e Run2 -c EMu -m 800 -s HNL --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Preapproval.txt --Impact




## HNL 3ch limit @260418
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o 3ch --Combine Channel --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval -e Run2 -c 3ch -s HNL --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval -e Run2 -c 3ch -s HNL --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval.txt --Asymptotic

#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval.txt --MDfit --r 0 # 2D blind Asymptotic limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval.txt --MDfit --r 1 --f 0.5 # signal-injected toy

# 3ch model but use 1D datacard, to study the effect of simple envelope vs actual combination
#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval_StudyEnvelope -e Run2 -s HNL --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval_StudyEnvelope.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval_StudyEnvelope -e Run2 -s HNL --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_3ch_Preapproval_StudyEnvelope.txt --Asymptotic



## ANv7_L2review but FixHessian_AddGluGluTaus BUT NEW HNL+WEINBERG 3CH PHYSICS MODEL @260418
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o New3ch --CR         --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o New3ch --Combine CR --Syst --Decorr --JetDecorr -s HNL Weinberg
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o New3ch --Combine Era --CR --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o New3ch --Combine Channel --CR --Syst --Decorr --JetDecorr

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_New3ch_Preapproval -e Run2 -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_New3ch_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_New3ch_Preapproval -e Run2 -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_New3ch_Preapproval.txt --Asymptotic

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_New3ch_Preapproval -e Run2 -c 3ch -s Weinberg --Limit
#./create-batch.py -l RunList_Run2_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_New3ch_Preapproval.txt --Asymptotic





## ANv7_L2review but FixHessian_AddGluGluTaus, TEST Run2Sum @260425
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o Run2Sum --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o Run2Sum --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o Run2Sum --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -s HNL Weinberg --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -c MuMu EE EMu -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Asymptotic

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -c 3ch -s HNL Weinberg --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Asymptotic

# Signal, SR-dependent limits
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o Run2Sum --CR         --Syst --Decorr --JetDecorr --Run2Sum -s DY VBF SSWW
#python MakeDataCard_ANv7.py -wp ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_Preapproval.txt -o Run2Sum --Combine CR --Syst --Decorr --JetDecorr --Run2Sum -s DY VBF SSWW

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -s DY VBF SSWW --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Work

#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -s DY VBF SSWW --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Asymptotic
#
#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Asymptotic

# EMu M800 impact check
#python MakeRunList.py ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval -e Run2Sum -c EMu -m 800 -s HNL --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_Run2Sum_Preapproval.txt --Impact


## L2review but Medium Bjet in CR2 IB, update muon fake highpt syst, Run2 shape bin optimization, mu/ele separate fake syst (PR187), Fix highpt syst bug (PR188) @260427
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Era --CR --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr

#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2 --Work
#./create-batch.py -l RunList_Run2_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2 --Limit
#./create-batch.py -l RunList_Run2_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2 -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2 -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic


## Same with above, SR2_PerFlavour_SR1_GlobalMass binning @260427
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass -sk card_skeleton_ANv7_PR188.txt --Combine Era --CR --Syst --Decorr --JetDecorr
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr

#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188 -e Run2 -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188 -e Run2 -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188.txt --Asymptotic

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR2_PerFlavour_SR1_GlobalMass_Decorr_JetDecorr_PR188.txt --Asymptotic



## Same with above, SR1_Global binning (global mass, global flavor. We won't use this as worse sensitivity is expected. only study purposes) @260429
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_Global_Decorr_JetDecorr_PR188.txt --Asymptotic


## Same with above, SR1_Global binning (global mass, global flavor. We won't use this as worse sensitivity is expected. only study purposes) @260429
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR1_MassGroups_Decorr_JetDecorr_PR188.txt --Asymptotic


## Same with above, SR1 per mass and BDT binning but checking stat requirement of Nbkg > 0.15 && rel. unc. < 0.5 per era. @260429
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_StatReqEra_Decorr_JetDecorr_PR188.txt --Asymptotic


## Same with above, SR3 binning updates @260501
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_SR3Update_Decorr_JetDecorr_PR188.txt --Asymptotic



## Same with above, final binning @260504
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR191_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic


## Same with above, final binning after fixing the bug @260506
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --pdf

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Breakdown
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Breakdown --pdf

# Extend M10000 range and rerun M10000 Breakdown @260512
#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -m 10000 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Breakdown
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Breakdown --pdf


# Ext Run @260512
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         --Ext --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR --Ext --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Ext --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum --Work --Ext
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum --Limit --Ext
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic

# signal-dependent limits
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         -s DY VBF SSWW --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR -s DY VBF SSWW --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -s DY VBF SSWW --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -s DY VBF SSWW --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic



#python PrintUncertBreakdown.py     -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188
#python PrintUncertTableForPaper.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188




# signal-dependent limits -- why I didn't run DYVBF... @260515
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         -s DYVBF --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR -s DYVBF --Syst --Decorr --JetDecorr --Run2Sum

# signal-dependent limits -- why I didn't run DYVBF... @260518
#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -s DYVBF --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -s DYVBF --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic



# CR3H bug fixed, SR data unblinded @260521
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind --pdf 


#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t SR1 SR2 SR3 -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind


#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --GOF --Unblind

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR -m 125 -c EE --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --GOF --Unblind

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t SR1 SR2 SR3 -m 125 -c EE --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t SR1 SR2 SR3 -m 1000 -c EE --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Asymptotic --Unblind

# TEST after updating GOF function
#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR -m 1000 -c MuMu --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --GOF --Unblind


# ZG pruning @260602
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR188.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Work

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR SR1 SR2 SR3 -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind

#python MakeRunList.py ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --Impact --Unblind --pdf

#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --GOF --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --GOF --Unblind --pdf

#./create-batch.py -l RunList_Run2Sum_ANv7_NewBinning_PR192_HNL_ULIDv2_FixCR3H_PruneZG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR188.txt --FitDiag --Unblind



# Conversion update: Don't use DYJets in SR1 and SR2. These SRs already have lepton pt > 15 GeV @260608 --> This change was updated was PR194
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind

#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic --Unblind

#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Q3 -t 1000 -n 100 --rRange 7.5:9.5:0.01 --Diagnostic
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Unblind -t 1000 -n 100 --rRange 7.5:9.5:0.01 --Diagnostic



# Conversion update but remove DY MG 10to50 @260613
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf


# Conversion update but use MiNNLO @260613
#python MakeDataCard_ANv7.py -wp ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine Channel --CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf


# Test CnC @260622
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf

#python MakeDataCard_ANv7.py -wp ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_CnC_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf


# Test NoTGTTG @260623
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf

#python MakeDataCard_ANv7.py -wp ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_DYConvUpdate_PR192_HNL_ULIDv2_NoTGTTG_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf



# Test LowStatNeff5 @260623
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic --Unblind

## test other GOFs
#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --toysNoSystematics --Unblind -n 20
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --tag ext1 -t 5000 -n 25
#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR3 -c MuMu -m 85 90 95 100 125 150 200 250 300 350 400 450 500 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --mask cr3_InvBJet -t 5000 -n 25
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --mask cr3_InvMET -t 5000 -n 25
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --mask cr3_InvBJet cr3_InvMET -t 5000 -n 25
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --mask cr3_InvBJet --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --mask cr3_InvMET --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --mask cr3_InvBJet cr3_InvMET --pdf

## test other impacts
#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --tag rRange1000 --rRange 1000
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --tag rRange1000 --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --tag rRange10000 --rRange 10000
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --tag rRange10000 --pdf


# Test other fit methods @260624
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic --Unblind


#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR3EEBin1314_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR3EEBin1314_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR3EEBin1314_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR3EEBin1314_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR3EEBin1314_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_MergeSR3EEBin1314_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf


#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_SmoothEE_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_SmoothEE_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_SmoothEE_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_SmoothEE_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_SmoothEE_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_SmoothEE_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf


# LowStatNeff5 was successful. Only small suspicious behavior in the highest masses. Test different HighMassScalers. @260626
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p2_V3_Strict_15_Bin_RunSyst -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p2_V3_Strict_15_Bin_RunSyst -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf

#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf

#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler1_V3_Strict_15_Bin_RunSyst -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler1_V3_Strict_15_Bin_RunSyst -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler1_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler1_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler1_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_HighMassScaler1_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf



## Now NoLowDYMG + LowStatNeff5 is a new nominal. Additional fit tests: Merge SR2 bin7-8 or Fill any holes with 0.001 @260707

# Merge SR2 bin7-8
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Asymptotic --Unblind

# Check pulls with masking @260810
#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -m 85 90 95 100 125 150 200 250 300 350 400 450 500 -t SR3 --Work
# sr3 only
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask cr3_InvBJet cr3_InvMET
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask cr3_InvBJet cr3_InvMET --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask cr3_InvBJet cr3_InvMET zz_cr zg_cr --pdf
# cr3_InvBJet only
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvMET zz_cr zg_cr
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvMET zz_cr zg_cr --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvMET
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvMET --pdf
# cr3_InvMET only
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvBJet zz_cr zg_cr
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvBJet zz_cr zg_cr --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvBJet
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 cr3_InvBJet --pdf
# cr3_InvBJet + cr3_InvMET only
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 zz_cr zg_cr
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 zz_cr zg_cr --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --mask sr3 --pdf
# all combo
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind --pdf



# FillHoles
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --pdf


## Now NoLowDYMG + LowStatNeff5 is a new nominal. Additional fit tests: Merge SR2 bin7-8 + Fill any holes with 0.001 @260708
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR194.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --Impact --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_FillHoles_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194.txt --GOF --Unblind --pdf



## Test adding Fake Loose ID uncertainty (targeting MuMu SR3 GOF) @260722
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --GOF --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind --pdf



## Test adding Fake Loose ID uncertainty + AltWZ (targeting MuMu SR3 GOF) @260722
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZ_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Asymptotic --Unblind


## Test adding Fake Loose ID uncertainty + AltWZNorm : to see how the WZNorm and GOF changes @260731
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --GOF --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNorm_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Asymptotic --Unblind



## Test AltWZ symm + inflation (No extra fake syst, AltWZ only) @260812
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m highmass -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t SR1 SR2 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work
#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind


#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind


#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind



#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m lowmass -t AllSR --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind




## It turned out that AltWZSym0 stabilizes the GOF, but screws up the Impact fit. Let's see whether restoring the extra fake syst fixes the issue. @260827
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind --pdf



#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind --pdf



#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195 -e Run2Sum -t AllSR --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_MergeSR2Bin78_AltWZNormSym20_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR195.txt --Impact --Unblind --pdf




## Test AltWZ but LowStatNeff5 applied; Also test RegDecorr @260902
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf



###################################### Nominal for ANv8 #######################################################

#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --FitDiag --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF -t 5000 -n 25 --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --GOF --Unblind --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf

# This is the new nominal at the unblinding stage. Run full-combo limits. @260916
#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -t AllSR SR1 SR2 SR3 --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind

#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly -s DYVBF DY VBF SSWW --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly -s DYVBF DY VBF SSWW --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -s DYVBF DY VBF SSWW --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -s DYVBF DY VBF SSWW --Limit
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind

#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Ext --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Ext --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum --Work --Ext
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum --Limit --Ext
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Asymptotic --Unblind

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange1000 --rRange 1000
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange1000 --rRange 1000 --pdf
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange10000 --rRange 10000
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --tag rRange10000 --rRange 10000 --pdf

# High-mass scaler for impacts <-- actually we didn't have to run this; extending rRange was enough.
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR1 SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 5000 7500 10000 15000 20000 25000 30000 40000 50000 60000 -t AllSR SR2 SR3 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_HighMassScaler0p5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Impact --Unblind --pdf



## Remake the datacard with updated nuisance grouping to include WZ modeling and to separate Ren- and Fac-Scale @261006
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --CR         --Syst --Decorr --JetDecorr --Run2Sum
#python MakeDataCard_ANv7.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst -sk card_skeleton_ANv7_PR195.txt -o AltWZonly --Combine CR --Syst --Decorr --JetDecorr --Run2Sum

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Work

#python MakeRunList.py ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195 -e Run2Sum -m 100 1000 10000 --Work
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Breakdown
#./create-batch.py -l RunList_Run2Sum_ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195.txt --Breakdown --pdf

#python PrintUncertBreakdown.py     -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195
#python PrintUncertTableForPaper.py -wp ANv7_ExtraFakeSyst_PR195_HNL_ULIDv2_NoLowDYMG_NewLowStatNeff5_MergeSR2Bin78_AltWZSym0_AltWZRegDecorr_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_AltWZonly_PR195
