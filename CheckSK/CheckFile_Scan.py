import ROOT
import sys

# Ensure a file path is provided as an argument
if len(sys.argv) < 2:
    print("Usage: python script.py <path_to_root_file>")
    sys.exit(1)

# Open the ROOT file
file_path = sys.argv[1]
file = ROOT.TFile.Open(file_path)

if not file or file.IsZombie():
    print(f"Error: Cannot open file {file_path}")
    sys.exit(1)

# Get the TTree
tree = file.Get("recoTree/SKFlat")  # Update with the correct tree name if necessary
if not tree:
    print("Error: Tree 'recoTree/SKFlat' not found in the file.")
    sys.exit(1)

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
        print(f"Error in branch: {branch_name} at event {event} -> {str(e)}")
        return True  # Issue detected

# Loop over every event number divisible by 1000
for problematic_event in range(0, nentries, 100):

    bad_branches = []
    
    for branch in branches:
        if test_branch(branch, problematic_event):
            bad_branches.append(branch)
    
    if bad_branches:
        print("\nProblematic branches detected for event", problematic_event)
        for b in bad_branches:
            print(f"- {b}")
# Close the file
file.Close()
