import ROOT
import os
import argparse

def count_events_in_trees(directory):
    total_events = 0

    # Loop over all files in the directory
    for filename in os.listdir(directory):
        if filename.endswith(".root"):  # Ensure we're only working with ROOT files
            filepath = os.path.join(directory, filename)
            # Open the ROOT file
            with ROOT.TFile(filepath, "READ") as file:
                # Check if the TTree exists
                tree = file.Get("recoTree/SKFlat")
                if tree:
                    total_events += tree.GetEntries()
                    print(f"File: {filename}, Events: {tree.GetEntries()}")
                else:
                    print(f"File: {filename} does not contain the TTree 'SKFlat'.")
    return total_events

if __name__ == "__main__":
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Count the total number of events in all ROOT files in a directory.")
    parser.add_argument("directory", type=str, help="Path to the directory containing ROOT files.")
    args = parser.parse_args()

    # Count events in all TFiles in the given directory
    total = count_events_in_trees(args.directory)
    print(f"Total events across all files: {total}")
