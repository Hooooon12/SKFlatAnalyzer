#void DrawLimits(TString year = "", TString channel = "", bool DrawExt = false, bool AddPub = true, int SepLimit = 0, bool CompareLimits = false, bool AppendLimitTable = false, bool IsXsecLimit = false, bool Logy = true)

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
root -l -q -b "DrawLimits.C(\"Run2\",\"${ch}\",false,false,0,true,false,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"2017\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"2016preVFP\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
#root -l -q -b "DrawLimits.C(\"2016postVFP\",\"${ch}\",false,false,0,true,true,false,false)"; # compare, append table, mixing limit, linear #internal limit comparison setting
  done;
