import uproot
import numpy as np

def read_limit(path):
    arr = uproot.open(path)["limit"].arrays(["limit", "iToy", "iSeed"], library="np")
    return arr

mass_1 = "1500"
mass_2 = "30000"

obs_1 = uproot.open(f"/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Run2Sum_EE_M{mass_1}_HNL_sr2_syst_Combined/GOF/Unblind/higgsCombine_gof_bonly_obs_Run2Sum_EE_M{mass_1}_HNL_sr2_syst_Combined.GoodnessOfFit.mH120.root")["limit"].arrays(["limit"], library="np")["limit"][0]
toy_1 = read_limit(f"/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Run2Sum_EE_M{mass_1}_HNL_sr2_syst_Combined/GOF/Unblind/higgsCombine_gof_bonly_toys_Ntoy1000_Run2Sum_EE_M{mass_1}_HNL_sr2_syst_Combined.GoodnessOfFit.mH120.123456.root")

obs_2 = uproot.open(f"/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Run2Sum_EE_M{mass_2}_HNL_sr2_syst_Combined/GOF/Unblind/higgsCombine_gof_bonly_obs_Run2Sum_EE_M{mass_2}_HNL_sr2_syst_Combined.GoodnessOfFit.mH120.root")["limit"].arrays(["limit"], library="np")["limit"][0]
toy_2 = read_limit(f"/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Run2Sum_EE_M{mass_2}_HNL_sr2_syst_Combined/GOF/Unblind/higgsCombine_gof_bonly_toys_Ntoy1000_Run2Sum_EE_M{mass_2}_HNL_sr2_syst_Combined.GoodnessOfFit.mH120.123456.root")

x = toy_1["limit"]
y = toy_2["limit"]

print(f"obs {mass_1} =", obs_1)
print(f"obs {mass_2} =", obs_2)

print(f"seed {mass_1} =", np.unique(toy_1["iSeed"]))
print(f"seed {mass_2} =", np.unique(toy_2["iSeed"]))

print(f"tail {mass_1} =", np.sum(x >= obs_1), "/", len(x))
print(f"tail {mass_2} =", np.sum(y >= obs_2), "/", len(y))

print("toys exactly same?", np.allclose(x, y, rtol=0, atol=0))
print("toy correlation =", np.corrcoef(x, y)[0, 1])
print("max abs diff =", np.max(np.abs(x - y)))
