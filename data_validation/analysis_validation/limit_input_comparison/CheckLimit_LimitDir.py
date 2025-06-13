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

# Function to print in color if the % error is greater than 30
def print_in_color(message, is_red=False):
    if is_red:
        print(f"\033[31m{message}\033[0m")  # Red text
    else:
        print(message)  # Default text

# Base directory (no need to reset this in the loop)
base_dir = "/data9/Users/jalmond_public/SUS-24-014/LimitInputs/"

# Eras to loop over
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# List to hold all red lines (where % error > 30)
red_lines = []

# Select the directory
dir_path = select_directory(base_dir)

# List of possible ROOT files to process
file_names = [
    "M500_MuMu_card_input.root",
    "M500_EE_card_input.root",
    "M500_EMu_card_input.root",
    "M1000_MuMu_card_input.root",
    "M1000_EE_card_input.root",
    "M1000_EMu_card_input.root"
]

# Loop through eras and process ROOT files
for era in eras:
    sr = "sr1"

    # Loop over all file names
    for file_name in file_names:
        # Construct file path
        file_path = os.path.join(dir_path, era, sr, file_name)

        if not os.path.exists(file_path):
            print(f"File not found for era {era}: {file_path}")
            continue

        # Open the ROOT file
        root_file = ROOT.TFile.Open(file_path)
        if not root_file or root_file.IsZombie():
            print(f"Failed to open file: {file_path}")
            continue

        # List of histograms to process
        histograms_to_process = ["data_obs", "Signal"]

        # Loop through the histograms
        for hist_name in histograms_to_process:
            histogram = root_file.Get(hist_name)
            if hist_name == "Signal":
                histogram=root_file.Get("signalDYVBF")
                histogram2=root_file.Get("signalSSWW")
                histogram.Add(histogram2)
                
            if isinstance(histogram, ROOT.TH1):
                print(f"\nProcessing Histogram: {hist_name} ({era}, {file_name})")
                for bin_index in range(1, histogram.GetNbinsX() + 1):
                    bin_label = histogram.GetXaxis().GetBinLabel(bin_index)

                    bin_content = histogram.GetBinContent(bin_index)
                    bin_error = histogram.GetBinError(bin_index)
                    bin_percentage_error = (bin_error / bin_content * 100) if bin_content != 0 else 0

                    # Construct the message including histogram name
                    message = f"({era}_{file_name}_{hist_name}_{bin_label}, Content: {bin_content}, Error: {bin_error}, % Error: {bin_percentage_error:.2f}%)"

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
