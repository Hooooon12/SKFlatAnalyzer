import os
import re
from collections import defaultdict

# Settings
log_dir = "logs"
masses = ['100', '125', '150', '200', '250', '300', '400', '500']
flavours = ['EE', 'MuMu', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

def extract_best_version_and_also(filepath):
    fom_v3 = None
    fom_v4 = None

    try:
        with open(filepath, "r") as f:
            for line in f:
                if "Best total FOM V3" in line:
                    m = re.search(r"FOM V3: ([\d.]+)", line)
                    if m:
                        fom_v3 = float(m.group(1))
                elif "Best total FOM V4" in line:
                    m = re.search(r"FOM V4: ([\d.]+)", line)
                    if m:
                        fom_v4 = float(m.group(1))
    except Exception:
        return None, False

    if fom_v3 is not None and fom_v4 is not None:
        if fom_v3 > fom_v4 * 1.05:
            return "V3", True  # V3 forced by 5% rule
        else:
            return "V4", False
    elif fom_v3 is not None:
        return "V3", False
    elif fom_v4 is not None:
        return "V4", False
    else:
        return None, False

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

def print_bins_grouped_by_flag():
    data = defaultdict(lambda: defaultdict(lambda: defaultdict(list)))

    for mass in masses:
        for mode in ["strict", "loose", "very_loose"]:
            if mode == "strict":
                bin_configs = [(15, 15, "Strict_BDT", "", "scan")]
            elif mode == "loose":
                bin_configs = [(15, 15, "Loose_BDT", "", "scan_loose")]
            else:
                bin_configs = [(25, 25, "VeryLoose_BDT", "", "scan_very_loose")]

            for flav in flavours:
                for era in eras:
                    for n_bins, n_max_bins, flag, suffix, prefix in bin_configs:
                        fname = f"{prefix}_{mass}_{flav}_{era}_{n_max_bins}_{n_bins}.out"
                        fpath = os.path.join(log_dir, fname)
                        if not os.path.exists(fpath):
                            continue

                        chosen_version, v3_won_by_5pct = extract_best_version_and_also(fpath)
                        if chosen_version is None:
                            continue

                        versions_to_print = [chosen_version]
                        if v3_won_by_5pct:
                            versions_to_print.append("V4")  # Also print V4 for comparison

                        for version in versions_to_print:
                            edges = extract_bin_edges(fpath, version=version)
                            if not edges:
                                continue
                            edge_str = ",".join(f"{e:.3f}" for e in edges)
                            key = f'SR3BDT_{flav}_{era}_{mass}_{version}{suffix}'
                            line = f'  map_bdt_limit_bins["{key}"] = {{{edge_str}, 1}};'
                            data[flag][mass][flav].append(line)

                            if mass == '100':
                                for new_mass in ['85', '90', '95']:
                                    key_alt = f'SR3BDT_{flav}_{era}_{new_mass}_{version}{suffix}'
                                    alt_line = f'  map_bdt_limit_bins["{key_alt}"] = {{{edge_str}, 1}};'
                                    data[flag][mass][flav].append(alt_line)

    for flag in ["Strict_BDT", "Loose_BDT", "VeryLoose_BDT"]:
        if flag not in data:
            continue
        print(f"\nif(HasFlag(\"{flag}\")) {{")
        for mass in masses:
            if mass not in data[flag]:
                continue
            print(f"  //========== Mass {mass} ===========")
            for flav in flavours:
                if flav in data[flag][mass]:
                    for line in data[flag][mass][flav]:
                        print(line)
                    print("")  # Blank line between flavours
        print("}")

if __name__ == "__main__":
    print_bins_grouped_by_flag()
