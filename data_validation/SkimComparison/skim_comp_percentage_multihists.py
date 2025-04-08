import os
import ROOT

# Define directories and file names
base_dir = '/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter/2018/MultiLepton__/DATA/'
output_dir = 'printed_results'  # Directory to save the results

# Create the directory if it does not exist
if not os.path.exists(output_dir):
    os.makedirs(output_dir)

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

# List of histogram names
hist_names = [
    "ChannelCutFlow/HNL_ULIDv2/LL/Presel",
    "ChannelCutFlow/HNL_ULIDv2/LL/LLL_ControlRegions",
    "ChannelCutFlow/HNL_ULIDv2/LL/SS_ControlRegions",
    "ChannelCutFlow/HNL_ULIDv2/LL/SSVBF_ControlRegions"
]

# Color formatting for terminal output
RED = '\033[91m'  # Red color
RESET = '\033[0m'  # Reset color

# Loop over the histograms and files
for hist_name in hist_names:
    # Define the output file path
    hist_filename = os.path.join(output_dir, hist_name.split('/')[-1] + '_results.txt')
    
    with open(hist_filename, 'w') as output_file:
        output_file.write(f"Results for histogram: {hist_name}\n\n")

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
            hist = f.Get(hist_name)
            hist_gt36 = f_gt36.Get(hist_name)

            if hist and hist_gt36:
                output_file.write(f"\nProcessing: {file} and {gt36_file}\n")

                # Get bin contents
                bin_count = hist.GetNbinsX()
                for bin_num in range(1, bin_count + 1):
                    bin_content = hist.GetBinContent(bin_num)
                    bin_content_gt36 = hist_gt36.GetBinContent(bin_num)

                    # Get bin labels
                    bin_label = hist.GetXaxis().GetBinLabel(bin_num)

                    # Calculate the percentage difference
                    if bin_content != 0:  # Avoid division by zero
                        percentage_diff = ((bin_content_gt36 - bin_content) / bin_content) * 100
                    else:
                        percentage_diff = 0  # If original bin content is zero, set the percentage difference to zero

                    # Print the results with color formatting for the terminal
                    line = f"Bin {bin_num} [{bin_label}]: Original file value = {bin_content}, " \
                           f"GT36 file value = {bin_content_gt36}, Percentage Difference = {percentage_diff:.2f}%"
                    
                    # If percentage difference > 5%, print in red in terminal
                    if percentage_diff > 5:
                        print(RED + line + RESET)  # Print in red
                    else:
                        print(line)  # Print normally

                    # Write the results to the output file with the bin label
                    output_file.write(f"Bin {bin_num} [{bin_label}]: Original file value = {bin_content}, "
                                      f"GT36 file value = {bin_content_gt36}, "
                                      f"Percentage Difference = {percentage_diff:.2f}%\n")
            else:
                output_file.write(f"Histogram '{hist_name}' not found in one of the files: {file} or {gt36_file}\n")
            
            # Close ROOT files
            f.Close()
            f_gt36.Close()

    print(f"Results for histogram {hist_name} saved to {hist_filename}")
