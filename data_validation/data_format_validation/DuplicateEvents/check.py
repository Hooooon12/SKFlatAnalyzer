import re
import sys

def extract_event_numbers(file_path):
    # List to store event numbers
    event_numbers = []

    # Regular expression to match event number pattern
    event_pattern = re.compile(r"SS Preselection EMu Event number = (\d+)")

    try:
        with open(file_path, 'r') as file:
            for line in file:
                # Search for the event number in each line
                match = event_pattern.search(line)
                if match:
                    event_number = match.group(1)
                    event_numbers.append(event_number)
    
        # Check for duplicates
        duplicates = set([event for event in event_numbers if event_numbers.count(event) > 1])

        # Print event numbers and duplicates
        if event_numbers:
            print("Event numbers found:")
            print("\n".join(event_numbers))
        
        if duplicates:
            print("\nDuplicate event numbers found:")
            print("\n".join(duplicates))
        else:
            print("\nNo duplicates found.")
    
    except FileNotFoundError:
        print(f"File '{file_path}' not found. Please check the file path and try again.")
        
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python script.py <file_path>")
    else:
        file_path = sys.argv[1]
        extract_event_numbers(file_path)
