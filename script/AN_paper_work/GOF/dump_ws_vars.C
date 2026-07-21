// Run this after cmsenv.

void dump_ws_vars() {
    TFile *f = TFile::Open("/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN/ANv7_ConvUpdate_PR192_HNL_ULIDv2_NoLowDYMG_LowStatNeff5_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_PR194/Run2Sum_EE_M15000_HNL_sr2_syst_Combined/GOF/Unblind/Run2Sum_EE_M15000_HNL_sr2_syst_Combined.root");
    RooWorkspace *w = (RooWorkspace*)f->Get("w");
    w->allVars().Print("v");
}
