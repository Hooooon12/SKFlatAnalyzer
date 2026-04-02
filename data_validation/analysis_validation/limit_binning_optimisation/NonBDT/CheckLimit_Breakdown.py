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
base_dir = '/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter/'

# Select the directory
dir_path = select_directory(base_dir)

# Eras to loop over
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

# List of background files to process
background_files = [
    'HNL_SignalRegion_Plotter_CF.root',
    'HNL_SignalRegion_Plotter_Conv.root',
    'HNL_SignalRegion_Plotter_Fake.root',
    'HNL_SignalRegion_Plotter_Other.root',
    'HNL_SignalRegion_Plotter_TTV.root',
    'HNL_SignalRegion_Plotter_VVV.root',
    'HNL_SignalRegion_Plotter_WW.root',
    'HNL_SignalRegion_Plotter_WZ.root',
    'HNL_SignalRegion_Plotter_ZZ.root'
]

# List to hold all red lines (where % error > 30)
red_lines = []

# Loop through eras and process ROOT files
for era in eras:
    # Loop over background files
    bkg_totals = {}  # Dictionary to store total background per bin
    bkg_contributions = {bkg_file: {} for bkg_file in background_files}  # Dictionary to store individual bkg contributions

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

        # Loop over histograms in the directory
        for key in hist_dir.GetListOfKeys():
            hist_name = key.GetName()
            histogram = hist_dir.Get(hist_name)

            if isinstance(histogram, ROOT.TH1):
                if hist_name not in bkg_totals:
                    bkg_totals[hist_name] = {'content': [0] * (histogram.GetNbinsX() + 1), 'error': [0] * (histogram.GetNbinsX() + 1)}  # Initialize total background list for the histogram

                print(f"\nProcessing Histogram: {hist_name} ({era}, {bkg_file})")
                total_entries = histogram.GetEntries()  # Get total number of entries in the histogram
                print(f"Total entries in histogram: {total_entries}")

                for bin_index in range(1, histogram.GetNbinsX() + 1):
                    bin_label = histogram.GetXaxis().GetBinLabel(bin_index)  # Get actual bin label
                    bin_content = histogram.GetBinContent(bin_index)
                    bin_error = histogram.GetBinError(bin_index)

                    # Add bin content and error to the total background for this bin
                    bkg_totals[hist_name]['content'][bin_index] += bin_content
                    bkg_totals[hist_name]['error'][bin_index] += bin_error ** 2  # Sum squares for the error

                    # Store the contribution for this bin and this background file
                    if bin_index not in bkg_contributions[bkg_file]:
                        bkg_contributions[bkg_file][bin_index] = {'content': 0, 'error': 0}
                    bkg_contributions[bkg_file][bin_index]['content'] += bin_content
                    bkg_contributions[bkg_file][bin_index]['error'] += bin_error ** 2

        # Close the ROOT file
        root_file.Close()

    # After processing all background files, print the breakdown for each histogram
    for hist_name in bkg_totals:
        print(f"\n({era}) Histogram: {hist_name}")
        for bin_index in range(1, len(bkg_totals[hist_name]['content'])):
            bin_label = bkg_totals[hist_name]['content'][bin_index]  # Using actual bin label
            total_bkg_content = bkg_totals[hist_name]['content'][bin_index]
            total_bkg_error = (bkg_totals[hist_name]['error'][bin_index])**0.5  # Error is the square root of the sum of squares

            # Calculate the percentage error
            bin_percentage_error = (total_bkg_error / total_bkg_content * 100) if total_bkg_content != 0 else 0

            # Print total background for the bin
            message = f"Bin: {bin_label}, [Bkg] Content: {total_bkg_content:.2f}, Error: {total_bkg_error:.2f}, % Error: {bin_percentage_error:.2f}%"
            print_in_color(message, is_red=total_bkg_content < 0)

            # Print contributions from each background source
            for bkg_file in background_files:
                if bin_index in bkg_contributions[bkg_file]:
                    contribution_content = bkg_contributions[bkg_file][bin_index]['content']
                    contribution_error = (bkg_contributions[bkg_file][bin_index]['error'])**0.5  # Error is the square root of the sum of squares

                    message = f"[{bkg_file.replace('HNL_SignalRegion_Plotter_', '').replace('.root', '')}] Content: {contribution_content:.2f}, Error: {contribution_error:.2f}"
                    print(f"  {message}")

    # At the end, print all red lines (if any)
    if red_lines:
        print("\nLines with percentage error > 30%:")
        for line in red_lines:
            print(line)

print("Processing completed.")
