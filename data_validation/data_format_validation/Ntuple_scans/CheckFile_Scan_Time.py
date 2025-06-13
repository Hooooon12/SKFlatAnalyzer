
import ROOT
import sys
import time

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
    time_start = time.time()
    tree.SetBranchStatus("*", 0)  # Disable all branches
    tree.SetBranchStatus(branch_name, 1)  # Enable only this branch
    try:
        tree.GetEntry(event)
        return time.time() - time_start  # Return the time it took to get the entry
    except Exception as e:
        print(f"Error in branch: {branch_name} at event {event} -> {str(e)}")
        return None  # Issue detected, return None

# Dictionary to store times for each branch (key: branch_name, value: list of times)
branch_times = {branch: [] for branch in branches}

# Loop over every event number divisible by 1000 (you can adjust this range as needed)
for problematic_event in range(122410, 122411, 1):

    for branch in branches:
        time_branch = test_branch(branch, problematic_event)
        
        # If the branch loads successfully, store the time
        if time_branch is not None:
            branch_times[branch].append((problematic_event, time_branch))

# Calculate total times for each branch and sort by the total time
branch_total_times = []
for branch, times in branch_times.items():
    total_time = sum(time for event, time in times)
    branch_total_times.append((branch, total_time))

# Sort branches by total time, from highest to lowest
branch_total_times.sort(key=lambda x: x[1], reverse=True)

# Get the maximum branch name length for consistent spacing
max_branch_length = max(len(branch) for branch in branches)

# Print the times for each branch, ordered by total time across events
for branch, _ in branch_total_times:
    # Format the branch name with consistent padding
    padded_branch_name = branch.ljust(max_branch_length)
    
    # Format the times for each event
    times_str = ", ".join([f"Event {event}: {time:.6f} seconds" for event, time in branch_times[branch]])
    
    # Calculate the total time for the branch
    total_time = sum(time for event, time in branch_times[branch])
    
    # Print the formatted output
    print(f"Branch {padded_branch_name}: {times_str} | Total time: {total_time:.6f} seconds")

# Close the file
file.Close()




