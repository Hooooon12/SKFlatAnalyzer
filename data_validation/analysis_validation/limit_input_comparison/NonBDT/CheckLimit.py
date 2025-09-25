import os
import ROOT
import math

Analyzer = "HNL_SignalRegion_Plotter"

def CalculateSignificance(Nsig, Nbkg):
    if Nsig < 0:
        Nsig = 0
    if  Nbkg < 0.1:
        Nbkg=0.1
    Signi =  math.sqrt(2* ((Nsig + Nbkg) * math.log(1 + (Nsig / Nbkg))) - Nsig )
    return round(Signi, 2)

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
base_dir = f"/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/{Analyzer}/"
Config_RunSettings_values = ['HNL_ULIDv2']

# Select the directory
dir_path = select_directory(base_dir)

# Eras to loop over
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# List to hold all red lines (where % error > 30)
red_lines = []

# List of files to process
files_to_process = [
    f"{Analyzer}_Bkg.root"]

# Loop over Config_RunSettings values
for Config_RunSettings in Config_RunSettings_values:
    log_filename = f"logs/{Config_RunSettings}_log.txt"
    
    # Open log file for writing
    with open(log_filename, 'w') as log_file:
        print(f"Processing with Config_RunSettings: {Config_RunSettings}", file=log_file)

        # Loop through eras and process ROOT files
        for era in eras:
            for file_name in files_to_process:
                file_path = os.path.join(dir_path, era, file_name)
                sig_500_file_path = os.path.join(dir_path, era, f"{Analyzer}_HNL_400.root")
                sig_1000_file_path = os.path.join(dir_path, era, f"{Analyzer}_HNL_1000.root")

                if not os.path.exists(file_path):
                    print(f"File not found for era {era}: {file_path}", file=log_file)
                    continue

                # Open the ROOT file
                root_file = ROOT.TFile.Open(file_path)
                if not root_file or root_file.IsZombie():
                    print(f"Failed to open file: {file_path}", file=log_file)
                    continue
                sig_500_root_file = ROOT.TFile.Open(sig_500_file_path)
                sig_1000_root_file = ROOT.TFile.Open(sig_1000_file_path)

                # Access the histograms in the specified directory
                hist_dir = root_file.Get(f"LimitExtraction/{Config_RunSettings}/LL/LimitBins")
                if not hist_dir:
                    print(f"Histogram directory not found in {file_path}", file=log_file)
                    continue

                sig_500_hist_dir = sig_500_root_file.Get(f"LimitExtraction/{Config_RunSettings}/LL/LimitBins")
                sig_1000_hist_dir = sig_1000_root_file.Get(f"LimitExtraction/{Config_RunSettings}/LL/LimitBins")

                # Loop over histograms in the directory
                for key in hist_dir.GetListOfKeys():
                    hist_name = key.GetName()
                    histogram = hist_dir.Get(hist_name)
                    sig_500_histogram = sig_500_hist_dir.Get(hist_name)
                    sig_1000_histogram = sig_1000_hist_dir.Get(hist_name)

                    if isinstance(histogram, ROOT.TH1):
                        print(f"\nProcessing Histogram: {hist_name} ({era}, {file_name})", file=log_file)
                        FOM500 = 0
                        FOM1000 = 0
                        for bin_index in range(1, histogram.GetNbinsX() + 1):
                            bin_label = histogram.GetXaxis().GetBinLabel(bin_index)
                            bin_content = histogram.GetBinContent(bin_index)
                            bin_error = histogram.GetBinError(bin_index)
                            bin_percentage_error = (bin_error / bin_content * 100) if bin_content != 0 else 0

                            sig_500_bin_content = sig_500_histogram.GetBinContent(bin_index)
                            sig_1000_bin_content = sig_1000_histogram.GetBinContent(bin_index)

                            sig_500 = CalculateSignificance(sig_500_bin_content, bin_content)
                            sig_1000 = CalculateSignificance(sig_1000_bin_content, bin_content)
                            FOM500 = FOM500 + sig_500
                            FOM1000 = FOM1000 + sig_1000

                            # Construct the message including histogram name
                            message1 = f"({era}_{file_name}_{hist_name}_{bin_label}_{Config_RunSettings}, Content: {bin_content}, Error: {bin_error}, % Error: {bin_percentage_error:.2f}%"
                            message2 = f"({era}_{file_name}_{hist_name}_{bin_label}_{Config_RunSettings}, sig_500_bin_content={sig_500_bin_content}:sig_1000_bin_content={sig_1000_bin_content} FOM[500] = {sig_500} FOM[1000] = {sig_1000})"

                            # Check if the percentage error is greater than 30% and print in red
                            if bin_percentage_error > 30:
                                print_in_color(message1, is_red=True)
                                print_in_color(message2, is_red=True)
                                print(message1, file=log_file)  # Write to log file
                                print(message2, file=log_file)  # Write to log file
                                red_lines.append(message1)  # Add to red_lines list
                                red_lines.append(message2)  # Add to red_lines list
                            else:
                                print_in_color(message1, is_red=False)
                                print_in_color(message2, is_red=False)
                                print(message1, file=log_file)  # Write to log file
                                print(message2, file=log_file)  # Write to log file
                        print(f"FOM500 = {FOM500} FOM1000 = {FOM1000}", file=log_file)
                # Close the ROOT file
                root_file.Close()

        # At the end, print all red lines to the log file
        if red_lines:
            print("\nLines with percentage error > 30%:", file=log_file)
            for line in red_lines:
                print(line, file=log_file)

        print(f"Processing completed with {Config_RunSettings}.", file=log_file)

