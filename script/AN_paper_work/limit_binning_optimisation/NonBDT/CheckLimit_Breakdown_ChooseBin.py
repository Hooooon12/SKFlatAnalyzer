import os
import ROOT
import sys
import re

# Function to list and select a directory
def select_directory(base_path):
    print(f"Listing directories in: {base_path}")
    
    # List all subdirectories in the base path
    directories = [d for d in os.listdir(base_path) if os.path.isdir(os.path.join(base_path, d))]
    
    if not directories:
        print("No directories found in the specified path.")
        exit()

    # Display available directories
    for idx, directory in enumerate(directories, 1):
        print(f"{idx}. {directory}")

    # Ask the user to pick a directory
    while True:
        try:
            choice = int(input(f"Select a directory by entering the number (1-{len(directories)}): "))
            if 1 <= choice <= len(directories):
                selected_dir = directories[choice - 1]
                print(f"You selected: {selected_dir}")
                return os.path.join(base_path, selected_dir)
            else:
                print(f"Please enter a number between 1 and {len(directories)}.")
        except ValueError:
            print("Invalid input. Please enter a number.")

# Function to print in red if the content is negative or % error is greater than 30
def print_in_color(message, is_red=False):
    if is_red:
        print(f"\033[31m{message}\033[0m")  # Red text
    else:
        print(message)  # Default text

# Parse the command-line argument
if len(sys.argv) != 2:
    print("Usage: python CheckLimit_Breakdown_ChooseBin.py <era_histogram_binlabel>")
    exit()

input_arg = sys.argv[1]

# Extract era, histogram, and bin label from the argument
try:
    # Split input into era, histogram, and bin label
    parts = input_arg.split('_')

    if len(parts) < 3:
        print("Invalid input format. The format should be <era>_<histogram>_<binlabel> (e.g., 2016preVFP_MuonSR3_SR3_bin11).")
        exit()

    era = parts[0]
    hist_name = parts[1]
    bin_label = "_".join(parts[2:])  # Join the remaining parts to get the full bin label

    # Extract the bin number from the bin label using regex to handle formats like SR2_HTLT_Bin1, SR3_bin13
    bin_match = re.search(r'(\d+)$', bin_label)  # Look for the last set of digits in the label (i.e., after the last '_')
    if bin_match:
        bin_index = int(bin_match.group(1))  # Convert bin number to integer
    else:
        print("Invalid bin label format. Make sure the bin label includes 'bin' followed by a number.")
        exit()
except ValueError:
    print("Invalid input format. The format should be <era>_<histogram>_<binlabel> (e.g., 2016preVFP_MuonSR3_SR3_bin11).")
    exit()

# Set base directory
base_dir = '/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter/'

# Select the directory
dir_path = select_directory(base_dir)

# List of background files to process
background_files = [
    'HNL_SignalRegion_Plotter_CF.root',
    'HNL_SignalRegion_Plotter_WG.root',
    'HNL_SignalRegion_Plotter_ZG.root',
    'HNL_SignalRegion_Plotter_Other_Conv.root',
    'HNL_SignalRegion_Plotter_Fake.root',
    'HNL_SignalRegion_Plotter_Other.root',
    'HNL_SignalRegion_Plotter_TTV.root',
    'HNL_SignalRegion_Plotter_VVV.root',
    'HNL_SignalRegion_Plotter_WW.root',
    'HNL_SignalRegion_Plotter_WZ.root',
    'HNL_SignalRegion_Plotter_ZZ.root'
]

# Dictionary to store histograms, bins, and their labels
histogram_bin_info = {}

# Loop through the chosen era to gather histogram and bin names
histogram_bin_info[era] = {}
for bkg_file in background_files:
    file_path = os.path.join(dir_path, era, bkg_file)

    if not os.path.exists(file_path):
        print(f"File not found for {era} in {bkg_file}: {file_path}")
        continue

    # Open the ROOT file
    root_file = ROOT.TFile.Open(file_path)
    if not root_file or root_file.IsZombie():
        print(f"Failed to open file: {file_path}")
        continue

    # Access the histograms in the specified directory
    hist_dir = root_file.Get('LimitExtraction/HNL_ULIDv2/LL/LimitBins')
    if not hist_dir:
        print(f"Histogram directory not found in {file_path}")
        continue

    # Loop over histograms in the directory and store the bin names
    for key in hist_dir.GetListOfKeys():
        hist_name_check = key.GetName()
        histogram = hist_dir.Get(hist_name_check)

        if isinstance(histogram, ROOT.TH1):
            histogram_bin_info[era][hist_name_check] = [histogram.GetXaxis().GetBinLabel(i) for i in range(1, histogram.GetNbinsX() + 1)]

    # Close the ROOT file
    root_file.Close()

# Check if the selected histogram and bin exist
if era not in histogram_bin_info:
    print(f"Era '{era}' not found.")
    exit()

if hist_name not in histogram_bin_info[era]:
    print(f"Histogram '{hist_name}' not found in era '{era}'.")
    exit()

if bin_label not in histogram_bin_info[era][hist_name]:
    print(f"Bin label '{bin_label}' not found in histogram '{hist_name}' in era '{era}'.")
    exit()

# Process and print details for the selected era, histogram, and bin
print(f"\nProcessing Era: {era}, Histogram: {hist_name}, Bin Label: {bin_label}")

bkg_contributions = {bkg_file: {'content': 0, 'error': 0} for bkg_file in background_files}

for bkg_file in background_files:
    file_path = os.path.join(dir_path, era, bkg_file)

    if not os.path.exists(file_path):
        print(f"File not found for {era} in {bkg_file}: {file_path}")
        continue

    # Open the ROOT file
    root_file = ROOT.TFile.Open(file_path)
    if not root_file or root_file.IsZombie():
        print(f"Failed to open file: {file_path}")
        continue

    # Access the histograms in the specified directory
    hist_dir = root_file.Get('LimitExtraction/HNL_ULIDv2/LL/LimitBins')
    if not hist_dir:
        print(f"Histogram directory not found in {file_path}")
        continue

    # Access the specific histogram and bin label directly
    histogram = hist_dir.Get(hist_name)
    if isinstance(histogram, ROOT.TH1):
        bin_index = histogram.GetXaxis().FindBin(bin_label)  # Use bin label directly to get the bin index
        bin_content = histogram.GetBinContent(bin_index)
        bin_error = histogram.GetBinError(bin_index)

        # Print the content and error for the selected background file
        bin_percentage_error = (bin_error / bin_content * 100) if bin_content != 0 else 0
        message = f"Bin: {bin_label}, [{bkg_file.replace('HNL_SignalRegion_Plotter_', '').replace('.root', '')}] Content: {bin_content:.2f}, Error: {bin_error:.2f}, % Error: {bin_percentage_error:.2f}%"
        print_in_color(message, is_red=bin_content < 0)

    # Close the ROOT file
    root_file.Close()

print("Processing completed.")
