import os
import re
from collections import defaultdict

# Settings
log_dir = "logs"
masses = ['100', '125', '150', '200', '250', '300', '400', '500']
flavours = ['EE', 'MuMu', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

def extract_bin_edges(filepath, version="V4"):
    try:
        with open(filepath, "r") as f:
            content = f.read()
    except Exception:
        return []

    match = re.search(
        rf"\[RESULT\] Best total FOM {version}:.*?using \d+ bins with strict-bin merge scan:\n"
        r"((?:\s*Bin\s+\d+:.*\n?)*)",
        content,
    )
    if not match:
        return []

    bin_lines = match.group(1).strip().split("\n")
    edges = []
    for line in bin_lines:
        edge_match = re.search(r"X = \[([-\d.]+), ([-\d.]+)\)", line)
        if edge_match:
            hi = float(edge_match.group(1))
            edges.append(hi)
    edges = sorted(set(e for e in edges if e != -1.0 and e != 1.0))
    return edges

def print_bins_grouped_by_flag(version_tag):
    # data[flag][mass][flavour] = list of bin lines
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

    for mass in masses:
        for mode in ["strict_10Bins","strict_15Bins", "strict_20Bins"]:
            if mode == "strict_10Bins":
                bin_configs = [(10, 25, "Strict_BDT", "_Strict_10_Bin", "scan")]
            elif mode == "strict_15Bins":
                bin_configs = [(15, 25, "Strict_BDT", "_Strict_15_Bin", "scan")]
            elif mode == "strict_20Bins":
                bin_configs = [(20, 25, "Strict_BDT", "_Strict_20_Bin", "scan")]
                
            for flav in flavours:
                for era in eras:
                    for n_bins, n_max_bins, flag, suffix, prefix in bin_configs:
                        fname = f"{prefix}_{mass}_{flav}_{era}_{n_max_bins}_{n_bins}.out"
                        fpath = os.path.join(log_dir, fname)
                        if not os.path.exists(fpath):
                            continue

                        edges = extract_bin_edges(fpath, version=version_tag)
                        if not edges:
                            continue

                        edge_str = ",".join(f"{e:.3f}" for e in edges)
                        key = f'SR3BDT_{flav}_{era}_{mass}_{version_tag}{suffix}'
                        line = f'  map_bdt_limit_bins["{key}"] = {{{edge_str}, 1}};'
                        data[flag][mass][flav].append(line)

                        if mass == '100':
                            for new_mass in ['85', '90', '95']:
                                key_alt = f'SR3BDT_{flav}_{era}_{new_mass}_{version_tag}{suffix}'
                                alt_line = f'  map_bdt_limit_bins["{key_alt}"] = {{{edge_str}, 1}};'
                                data[flag][mass][flav].append(alt_line)

    # Print results grouped by flag, then mass, then flavour
    for flag in ["Strict_BDT"]:
        if flag not in data:
            continue
        for mass in masses:
            if mass not in data[flag]:
                continue
            print(f"\n  //========== Mass {mass} ==========")
            for flav in flavours:
                if flav in data[flag][mass]:
                    for line in data[flag][mass][flav]:
                        print(line)
                    print("")  # Blank line between flavours

if __name__ == "__main__":
    print_bins_grouped_by_flag("V4")
    print_bins_grouped_by_flag("V3")

