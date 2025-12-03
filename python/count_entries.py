#!/usr/bin/env python3

import os
import sys
import argparse
import ROOT

def count_entries_and_genweights(directory, dir_name, tree_name):
    files = [os.path.join(directory, f)
             for f in os.listdir(directory)
             if f.endswith(".root")]

    if not files:
        print("No ROOT files found in directory: {}".format(directory))
        return

    total_entries = 0
    total_genweight = 0

    print("Scanning directory: {}".format(directory))
    print("Using TDirectory: {}".format(dir_name))
    print("Using TTree name: {}".format(tree_name))
    print("")

    for path in sorted(files):
        f = ROOT.TFile.Open(path)
        if not f or f.IsZombie():
            print("Warning: could not open file: {}".format(path))
            continue

        d = f.Get(dir_name)
        if not d:
            print("Warning: TDirectory '{}' not found in file: {}".format(dir_name, path))
            f.Close()
            continue

        tree = d.Get(tree_name)
        if not tree:
            print("Warning: TTree '{}' not found in directory '{}' in file: {}".format(tree_name, dir_name, path))
            f.Close()
            continue

        n_entries = tree.GetEntries()
        total_entries += n_entries

        print("{}: {} entries".format(os.path.basename(path), n_entries))

        # Get gen_weight branch once
        gen_weight = tree.GetBranch("gen_weight")
        if not gen_weight:
            print("Warning: branch 'gen_weight' not found in file: {}".format(path))
            f.Close()
            continue

        # Loop over events
        for i in range(n_entries):
            tree.GetEntry(i)
            w = tree.gen_weight
            if w > 0:
                total_genweight += 1
            elif w < 0:
                total_genweight -= 1
            # If w == 0, do nothing

        f.Close()

    print("")
    print("Total entries across all files: {}".format(total_entries))
    print("Summed genweight sign count: {}".format(total_genweight))


def main():
    parser = argparse.ArgumentParser(
        description="Sum entries and signed genweights over ROOT files in a directory."
    )
    parser.add_argument("directory", help="Directory with ROOT files")
    parser.add_argument("--dir", dest="dir_name", default="recoTree",
                        help="TDirectory containing the TTree (default: recoTree)")
    parser.add_argument("--tree", dest="tree_name", default="SKFlat",
                        help="TTree name (default: SKFlat)")

    args = parser.parse_args()

    if not os.path.isdir(args.directory):
        print("Error: '{}' is not a valid directory".format(args.directory))
        sys.exit(1)

    count_entries_and_genweights(args.directory, args.dir_name, args.tree_name)


if __name__ == "__main__":
    main()
