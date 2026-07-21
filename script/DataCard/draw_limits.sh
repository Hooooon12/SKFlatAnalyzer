#void DrawLimits(TString year="", TString channel="", bool DrawExt=false, bool AddPub=true, int SepLimit=0, bool CompareLimits=false, bool AppendLimitTable=false, bool IsXsecLimit=false, bool Logy=true, TString preset_name="", bool DrawObserved=false)

### 2D ###
# 2D expected
#root -l -q -b "DrawLimits_2D.C(\"massf\",300,0.1,false,\"\",\"/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/<YOUR_3CH_WP>/Run2Sum_3ch_HNL_syst_Asym_limit.txt\")"
# 2D observed
#root -l -q -b "DrawLimits_2D.C(\"massf\",300,0.1,true,\"\",\"/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/<YOUR_3CH_WP>/Run2Sum_3ch_HNL_syst_Asym_limit.txt\")"
# 2D 3ch vs envelope expected
#root -l -q -b "DrawLimits_2D.C(\"3ch_vs_envelope\",300,0.1,false,\"\",\"/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/<YOUR_3CH_WP>/Run2Sum_3ch_HNL_syst_Asym_limit.txt\",\"/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/<YOUR_ENVELOPE_WP>\")"
# 2D 3ch vs envelope observed
#root -l -q -b "DrawLimits_2D.C(\"3ch_vs_envelope\",300,0.1,true,\"\",\"/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/<YOUR_3CH_WP>/Run2Sum_3ch_HNL_syst_Asym_limit.txt\",\"/data9/Users/HNL_public/SUS-24-014/LimitExtraction/limits/<YOUR_ENVELOPE_WP>\")"


for ch in MuMu EE EMu;
#for ch in MuMu EE;
#for ch in MuMu;
  do
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",true,true,0,false,false,false,true)"; # Ext, AddPub, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",true,true,0,false,false,true ,true)"; # Ext, AddPub, xsec limit, logy
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,true,0,true,false,false,true)"; # AddPub, compare, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,true,0,true,false,false,false)"; # AddPub, compare, mixing limit, linear
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,true,0,true,false,true,true)"; # AddPub, compare, xsec limit, logy
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,false,1,true,false,false,true)"; # Sig sep limits, compare, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,false,2,true,false,false,true)"; # SR sep limits, compare, mixing limit, logy

#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,false,0,true,false,false,true)"; # compare, write table, mixing limit, logy #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"2017\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"2016preVFP\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"2016postVFP\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting

## Recent studies ##
# Era-dependent binning, Era-dependent datacard
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
# Era-dependent binning, Run2 vs. Era-dependent datacard
#root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,false,0,true,true,false,false,\"Run2Sum_vs_Run2\")"; # compare, append table, mixing limit, linear #internal limit comparison setting
# New nomial: Run2 binning. Compare Era-dependent vs. Run2 datacard
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,0,true,true,false,false,\"SR_only\")"; # compare, append table, mixing limit, linear #internal limit comparison setting

## Run2Sum version (ANv7) ##
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",true,true,0,false,false,false,true)"; # Ext, AddPub, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",true,true,0,false,false,true ,true)"; # Ext, AddPub, xsec limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,true,false,false,true)"; # AddPub, compare, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,true,false,false,false)"; # AddPub, compare, mixing limit, linear
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,true,false,true,true)"; # AddPub, compare, xsec limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,1,true,false,false,true)"; # Sig sep limits, compare, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,2,true,false,false,true)"; # SR sep limits, compare, mixing limit, logy

## CCDY+WG only limits ##
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,0,false,false,false,true,\"DYVBF_only\")"; # without published results, no comparison, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,false,false,false,true,\"DYVBF_only\")"; # add published results, no comparison, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,true,false,false,true,\"DYVBF_only\")"; # add published results, Do comparison, mixing limit, logy

## Run2Sum, update for unblind (ANv7) ##

# exp only #
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",true,true,0,false,false,false,true,\"\",false)"; # Ext, AddPub, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",true,true,0,false,false,true ,true,\"\",false)"; # Ext, AddPub, xsec limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,true,false,false,true,\"\",false)"; # AddPub, compare, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,true,false,false,false,\"\",false)"; # AddPub, compare, mixing limit, linear
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,true,0,true,false,true,true,\"\",false)"; # AddPub, compare, xsec limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,1,true,false,false,true,\"\",false)"; # Sig sep limits, compare, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,2,true,false,false,true,\"\",false)"; # SR sep limits, compare, mixing limit, logy
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,0,false,false,false,true,\"\",false)"; # mixing limit, logy

# exp, internal comparison #
root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,0,true,true,false,true,\"\",false)"; # compare, mixing limit, logy
root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,0,true,true,false,false,\"\",false)"; # compare, mixing limit, linear

# obs #
#root -l -q -b "DrawLimits.C(\"Run2Sum\",\"${ch}\",false,false,0,false,false,false,true,\"\",true)"; # mixing limit, logy

  done;
