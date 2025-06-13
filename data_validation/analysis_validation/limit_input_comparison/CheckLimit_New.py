import os
import ROOT

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

# Function to print in red if the % error is greater than 30
def print_in_color(message, is_red=False):
    if is_red:
        print(f"\033[31m{message}\033[0m")  # Red text
    else:
        print(message)  # Default text

# Set base directory
base_dir = '/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_TestRun/'
Config_RunSettings='HNL_ULIDv2'

# Select the directory
dir_path = select_directory(base_dir)

# Eras to loop over
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# List to hold all red lines (where % error > 30)
red_lines = []

# Open the ROOT file and loop over Config_RunSettings (subdirectories within LimitExtraction)
for era in eras:
    file_path = os.path.join(dir_path, era, 'HNL_SignalRegion_TestRun_Bkg.root')

    if not os.path.exists(file_path):
        print(f"File not found for era {era}: {file_path}")
        continue

    # Open the ROOT file
    root_file = ROOT.TFile.Open(file_path)
    if not root_file or root_file.IsZombie():
        print(f"Failed to open file: {file_path}")
        continue

    # Access the histograms in the specified directory
    limit_extraction_dir = root_file.Get("LimitExtraction")
    if not limit_extraction_dir:
        print(f"LimitExtraction directory not found in {file_path}")
        continue

    # Get all subdirectories in LimitExtraction/ directory
    config_run_settings_dirs = [key.GetName() for key in limit_extraction_dir.GetListOfKeys() if isinstance(limit_extraction_dir.Get(key.GetName()), ROOT.TDirectory)]

    # Loop over each subdirectory in LimitExtraction/
    for config_run_setting in config_run_settings_dirs:
        print(f"\nProcessing Config_RunSettings: {config_run_setting}")

        hist_dir = limit_extraction_dir.Get(f"{config_run_setting}/LL/LimitBins")
        if not hist_dir:
            print(f"Histogram directory not found in {file_path} for Config_RunSettings: {config_run_setting}")
            continue

        # Loop over histograms in the directory
        for key in hist_dir.GetListOfKeys():
            hist_name = key.GetName()
            histogram = hist_dir.Get(hist_name)

            if isinstance(histogram, ROOT.TH1):
                print(f"\nProcessing Histogram: {hist_name} ({era})")
                for bin_index in range(1, histogram.GetNbinsX() + 1):
                    bin_label = histogram.GetXaxis().GetBinLabel(bin_index)
                    bin_content = histogram.GetBinContent(bin_index)
                    bin_error = histogram.GetBinError(bin_index)
                    bin_percentage_error = (bin_error / bin_content * 100) if bin_content != 0 else 0

                    # Construct the message including histogram name
                    message = f"({era}_{config_run_setting}_{hist_name}_{bin_label}, Content: {bin_content}, Error: {bin_error}, % Error: {bin_percentage_error:.2f}%"

                    # Check if the percentage error is greater than 30% and print in red
                    if bin_percentage_error > 30:
                        print_in_color(message, is_red=True)
                        red_lines.append(message)  # Add to red_lines list
                    else:
                        print_in_color(message, is_red=False)

    # Close the ROOT file
    root_file.Close()

# At the end, print all red lines
if red_lines:
    print("\nLines with percentage error > 30%:")
    for line in red_lines:
        print(line)

print("Processing completed.")
