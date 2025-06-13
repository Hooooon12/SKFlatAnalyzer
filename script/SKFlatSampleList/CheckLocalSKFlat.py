import os
from os import listdir
from os.path import isfile,isdir, join

def get_dsn_list(_mclist_path):
    arr_dsn = []
    
    try:
        r_localfile = open(_mclist_path, "r")
        for line in r_localfile:
            line_blocks = line.split()
            if len(line_blocks) != 3:
                continue  # Skip lines that don't have exactly 3 blocks
            arr_dsn.append(line_blocks[1])  # Only append the second element
        r_localfile.close()
    except Exception as e:
        print(f"Error reading file {e}")

    return arr_dsn


def get_skflat_on_tamsa(_path, _era):
    path_era = _path + _era + "/MC/"
    arr_skflatdir = [f for f in listdir(path_era) if isdir(join(path_era, f))]
    arr_skflatdir.sort()

    try:
        skip_dsn_list = open(os.getenv("SKFlat_WD") + "/script/SKFlatSampleList/config/DSN_Skip.txt", "r")
        for line in skip_dsn_list:
            parts = line.split()
            if len(parts) == 2:
                if parts[0] == "201*" or parts[0] == _era:
                    print(f"Removing {parts[1]}")
                    if parts[1] in arr_skflatdir:
                        arr_skflatdir.remove(parts[1])
                    else:
                        print(f"Warning: {parts[1]} not found in the directory list")
        skip_dsn_list.close()
    except Exception as e:
        print(f"Error reading DSN_Skip.txt: {e}")

    return arr_skflatdir


                
    
