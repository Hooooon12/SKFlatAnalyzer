import os
import ROOT

# Define directories and file names
base_dir = '/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/2018/MultiLepton__/DATA/'

# List of original files
files = [
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_D.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_D.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_D.root'
]

# Corresponding files with _GT36 suffix
gt36_files = [
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_D.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_D.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_D.root'
]

# Loop over the files
for file, gt36_file in zip(files, gt36_files):
    # Open the ROOT file
    file_path = os.path.join(base_dir, file)
    gt36_file_path = os.path.join(base_dir, gt36_file)

    # Ensure the files exist
    if not os.path.exists(file_path) or not os.path.exists(gt36_file_path):
        print(f"File not found: {file_path} or {gt36_file_path}")
        continue

    # Open ROOT files
    f = ROOT.TFile.Open(file_path)
    f_gt36 = ROOT.TFile.Open(gt36_file_path)

    # Retrieve histograms
    hist_name = "ChannelCutFlow/HNL_ULIDv2/LL/Presel"
    hist = f.Get(hist_name)
    hist_gt36 = f_gt36.Get(hist_name)

    if hist and hist_gt36:
        print(f"\nProcessing: {file} and {gt36_file}")

        # Get bin contents
        bin_count = hist.GetNbinsX()
        for bin_num in range(1, bin_count + 1):
            bin_content = hist.GetBinContent(bin_num)
            bin_content_gt36 = hist_gt36.GetBinContent(bin_num)

            # Compare the bin contents
            print(f"Bin {bin_num}: Original file value = {bin_content}, GT36 file value = {bin_content_gt36}")
    else:
        print(f"Histogram '{hist_name}' not found in one of the files: {file} or {gt36_file}")
    
    # Close ROOT files
    f.Close()
    f_gt36.Close()
