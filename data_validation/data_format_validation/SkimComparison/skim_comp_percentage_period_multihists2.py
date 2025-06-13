import os
import ROOT

# Directory and file paths
base_dir = '/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/2018/MultiLepton__/DATA/'

# Original files for DoubleMuon, EGamma, MuonEG
files = [
    # DoubleMuon
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_D.root',
    # EGamma
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_D.root',
    # MuonEG
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_D.root',
]

# Corresponding GT36 files
gt36_files = [
    # DoubleMuon
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_DoubleMuon_GT36_D.root',
    # EGamma
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_EGamma_GT36_D.root',
    # MuonEG
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_A.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_B.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_C.root',
    'HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_MuonEG_GT36_D.root',
]

histogram_path = "ChannelCutFlow/HNL_ULIDv2/LL/Presel"

# Step 0 through Step 4
steps = ['Step0', 'Step1', 'Step2', 'Step3', 'Step4']

# Function to extract histogram data
def extract_histogram_data(file_paths):
    histogram_data = {}
    
    for file in file_paths:
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
    
    return histogram_data


# Extract histogram data for both original files and GT36 files
original_histogram_data = extract_histogram_data(files)
gt36_histogram_data = extract_histogram_data(gt36_files)

# Calculate total sum for each step (Step 0, Step 1, ..., Step 4) for original files and GT36 files
def calculate_step_totals(histogram_data):
    step_totals = {step: 0 for step in steps}

    for file, bin_data in histogram_data.items():
        for bin_label, bin_content in bin_data:
            if bin_label in steps:
                step_totals[bin_label] += bin_content
    
    return step_totals


# Calculate step totals for original files and GT36 files
original_step_totals = calculate_step_totals(original_histogram_data)
gt36_step_totals = calculate_step_totals(gt36_histogram_data)

# Debugging: Print out the total step content across all files
print("\n--- Total Step Content Across A + B + C + D (Original Files) ---")
for step in original_step_totals:
    print(f"{step}: {original_step_totals[step]}")

print("\n--- Total Step Content Across A + B + C + D (GT36 Files) ---")
for step in gt36_step_totals:
    print(f"{step}: {gt36_step_totals[step]}")


# Now calculate the percentage for each step in original files and GT36 files relative to total sum of that step across all files
def print_step_percentages(histogram_data, step_totals, files, label):
    print(f"\n--- Step Percentages for Each Step in {label} (Relative to A / A+B+C+D) ---")
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


# Print percentages for both original files and GT36 files
print_step_percentages(original_histogram_data, original_step_totals, files, "Original Files")
print_step_percentages(gt36_histogram_data, gt36_step_totals, gt36_files, "GT36 Files")
