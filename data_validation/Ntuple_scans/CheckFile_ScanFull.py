import ROOT
import sys
import os

# Ensure a file path is provided as an argument
if len(sys.argv) < 2:
    print("Usage: python script.py <path_to_root_file_or_directory>")
    sys.exit(1)

# Get input path
input_path = sys.argv[1]

# Prepare log file
log_file = "branch_check_log.txt"
with open(log_file, "w") as log:
    def log_print(message):
        print(message)
        log.write(message + "\n")
    
    # Function to process a single ROOT file
    def process_file(file_path):
        log_print(f"Processing file: {file_path}")
        file = ROOT.TFile.Open(file_path)
        
        if not file or file.IsZombie():
            print(f"Error: Cannot open file {file_path}")
            return
        
        # Get the TTree
        tree = file.Get("recoTree/SKFlat")  # Update with the correct tree name if necessary
        if not tree:
            print("Error: Tree 'recoTree/SKFlat' not found in the file.")
            file.Close()
            return
        
        # Get the total number of entries
        nentries = tree.GetEntries()
        print(f"Total entries in the tree: {nentries}")
        
        # Get a list of all branches
        branches = [b.GetName() for b in tree.GetListOfBranches()]
        print(f"Total branches: {len(branches)}")
        
        # Function to check if a specific branch causes an error
        def test_branch(branch_name, event):
            tree.SetBranchStatus("*", 0)  # Disable all branches
            tree.SetBranchStatus(branch_name, 1)  # Enable only this branch
            try:
                tree.GetEntry(event)
                return False  # No issue
            except Exception as e:
                log_print(f"Error in branch: {branch_name} at event {event} -> {str(e)}")
                return True  # Issue detected
        
        # Loop over every event number divisible by 1000
        for problematic_event in range(0, nentries, 1000):
            #log_print(f"\nChecking event: {problematic_event}")
            bad_branches = []
            
            for branch in branches:
                if test_branch(branch, problematic_event):
                    bad_branches.append(branch)
            
            if bad_branches:
                log_print("\nProblematic branches detected for event " + str(problematic_event))
                for b in bad_branches:
                    log_print(f"- {b}")

        
        # Close the file
        file.Close()
    
    # Process each file if input is a directory
    if os.path.isdir(input_path):
        for root, _, files in os.walk(input_path):
            for file_name in files:
                if file_name.endswith(".root"):
                    process_file(os.path.join(root, file_name))
    elif os.path.isfile(input_path) and input_path.endswith(".root"):
        process_file(input_path)
    else:
        log_print("Error: Input path is neither a ROOT file nor a directory containing ROOT files.")

print(f"Processing complete. Log saved to {log_file}")
