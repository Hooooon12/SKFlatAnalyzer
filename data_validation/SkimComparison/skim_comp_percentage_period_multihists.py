import os
import ROOT

# Directory and file paths
base_dir = '/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/2018/MultiLepton__/DATA/'
files = [
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_D.root',
]
histogram_path = "ChannelCutFlow/HNL_ULIDv2/LL/Presel"

# Dictionary to store bin contents and labels for each file
histogram_data = {}

# Step 0 through Step 4
steps = ['Step0', 'Step1', 'Step2', 'Step3', 'Step4']

# Open ROOT files and access histograms
for file in files:
    file_path = os.path.join(base_dir, file)
    
    if not os.path.exists(file_path):
        print(f"File not found: {file_path}")
        continue
    
    # Open the ROOT file
    root_file = ROOT.TFile.Open(file_path)
    
    if not root_file or root_file.IsZombie():
        print(f"Error: Unable to open file {file}")
        continue
    
    # Retrieve the histogram
    hist = root_file.Get(histogram_path)
    
    if hist:
        print(f"Successfully retrieved histogram for file: {file}, Integral = {hist.Integral()}")
        
        # Store bin contents and bin labels
        bin_data = []
        n_bins = hist.GetNbinsX()
        for bin in range(1, n_bins + 1):
            bin_label = hist.GetXaxis().GetBinLabel(bin)
            bin_content = hist.GetBinContent(bin)
            bin_data.append((bin_label, bin_content))
        
        # Store the bin data in the dictionary
        histogram_data[file] = bin_data
    else:
        print(f"Error: Histogram '{histogram_path}' not found in {file}")

# Calculate total sum for each step (Step 0, Step 1, ..., Step 4)
step_totals = {step: 0 for step in steps}

# Calculate the total content for each step across all files (A + B + C + D)
for file, bin_data in histogram_data.items():
    for bin_label, bin_content in bin_data:
        if bin_label in steps:
            step_totals[bin_label] += bin_content

# Debugging: Print out the total step content across all files
print("\n--- Total Step Content Across A + B + C + D ---")
for step in step_totals:
    print(f"{step}: {step_totals[step]}")

# Now calculate the percentage for each step in file A relative to total sum of that step across all files
print("\n--- Step Percentages for Each Step (Relative to A / A+B+C+D) ---")
for step in steps:
    print(f"\n--- {step} ---")
    
    for file in files:
        # Get the content of the current step from each file
        bin_content = 0
        for bin_label, content in histogram_data.get(file, []):
            if bin_label == step:
                bin_content = content
                break
        
        # Calculate the percentage of this step for the current file
        if step_totals[step] > 0:
            percentage = (bin_content / step_totals[step]) * 100
        else:
            percentage = 0

        print(f"{file}: Step {step} content = {bin_content}, Percentage of total {step} = {percentage:.2f}%")
