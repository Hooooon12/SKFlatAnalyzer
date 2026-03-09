#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import math
import ROOT

ROOT.gROOT.SetBatch(True)

BASE_DIR = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"
ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]
REGIONS = ["sr1", "sr2", "sr3"]

ROOT_FILES = [
    "M10000_MuMu_card_input.root",
    "M1000_MuMu_card_input.root",
    "M100_MuMu_card_input.root",
    "M1100_MuMu_card_input.root",
    "M1200_MuMu_card_input.root",
    "M125_MuMu_card_input.root",
    "M1300_MuMu_card_input.root",
    "M15000_MuMu_card_input.root",
    "M1500_MuMu_card_input.root",
    "M150_MuMu_card_input.root",
    "M1700_MuMu_card_input.root",
    "M20000_MuMu_card_input.root",
    "M2000_MuMu_card_input.root",
    "M200_MuMu_card_input.root",
    "M25000_MuMu_card_input.root",
    "M2500_MuMu_card_input.root",
    "M250_MuMu_card_input.root",
    "M30000_MuMu_card_input.root",
    "M3000_MuMu_card_input.root",
    "M300_MuMu_card_input.root",
    "M350_MuMu_card_input.root",
    "M400_MuMu_card_input.root",
    "M450_MuMu_card_input.root",
    "M5000_MuMu_card_input.root",
    "M500_MuMu_card_input.root",
    "M600_MuMu_card_input.root",
    "M700_MuMu_card_input.root",
    "M7500_MuMu_card_input.root",
    "M800_MuMu_card_input.root",
    "M85_MuMu_card_input.root",
    "M900_MuMu_card_input.root",
    "M90_MuMu_card_input.root",
    "M95_MuMu_card_input.root",
    "Weinberg_MuMu_card_input.root",
]

SIGNAL_MAP = {
    "default": ["signalDY", "signalVBF", "signalSSWW"],
    "Weinberg_MuMu_card_input.root": ["signalWeinberg"],
}

ALL_SIGNALS = ["signalDY", "signalVBF", "signalSSWW", "signalWeinberg"]

SUMMARY_OUT = "signal_stat_summary.txt"
DETAIL_OUT = "signal_stat_detailed.txt"


def clone_hist(h, new_name):
    out = h.Clone(new_name)
    out.SetDirectory(0)
    return out


def open_root_safely(path):
    if not os.path.exists(path):
        return None
    tf = ROOT.TFile.Open(path)
    if not tf or tf.IsZombie():
        return None
    return tf


def get_signal_names(root_file):
    return SIGNAL_MAP.get(root_file, SIGNAL_MAP["default"])


def parse_mass_label(root_file):
    if root_file.startswith("Weinberg"):
        return "Weinberg"
    tag = root_file.replace("_MuMu_card_input.root", "")
    if tag.startswith("M"):
        return f"{tag[1:]} GeV"
    return tag


def mass_sort_key(mass_label):
    if mass_label == "Weinberg":
        return (10**12,)
    try:
        val = float(mass_label.replace(" GeV", ""))
        return (val,)
    except Exception:
        return (10**12,)


def get_mass_value(mass_label):
    if mass_label == "Weinberg":
        return None
    try:
        return float(mass_label.replace(" GeV", ""))
    except Exception:
        return None


def combine_eras(root_file, region, signal_name):
    """
    data_obs is used as background.
    return combined_signal, combined_bkg
    """
    combined_sig = None
    combined_bkg = None
    found_signal = False
    found_bkg = False

    for era in ERAS:
        path = os.path.join(BASE_DIR, era, region, root_file)
        tf = open_root_safely(path)
        if tf is None:
            continue

        # background: blinded data_obs (= total background)
        hbkg = tf.Get("data_obs")
        if hbkg and hbkg.InheritsFrom("TH1"):
            found_bkg = True
            if combined_bkg is None:
                combined_bkg = clone_hist(hbkg, f"data_obs_{region}_combined")
            else:
                combined_bkg.Add(hbkg)

        # signal
        hsig = tf.Get(signal_name)
        if hsig and hsig.InheritsFrom("TH1"):
            found_signal = True
            if combined_sig is None:
                combined_sig = clone_hist(hsig, f"{signal_name}_{region}_combined")
            else:
                combined_sig.Add(hsig)

        tf.Close()

    if not found_signal:
        combined_sig = None
    if not found_bkg:
        combined_bkg = None

    return combined_sig, combined_bkg


def safe_rel_stat(err, content):
    if content <= 0:
        return None
    return err / content


def get_top3_bins(sig, bkg):
    """
    Return top 3 bins ranked by S/sqrt(B), using:
      S = signal bin content
      B = data_obs bin content
    """
    results = []
    if sig is None or bkg is None:
        return results

    nbins = sig.GetNbinsX()
    for ibin in range(1, nbins + 1):
        s = sig.GetBinContent(ibin)
        b = bkg.GetBinContent(ibin)
        e = sig.GetBinError(ibin)

        if s <= 0 or b <= 0:
            continue

        score = s / math.sqrt(b)
        rel = safe_rel_stat(e, s)

        results.append({
            "bin": ibin,
            "S": s,
            "B": b,
            "score": score,
            "err": e,
            "content": s,
            "rel": rel,
        })

    results.sort(key=lambda x: x["score"], reverse=True)
    return results[:3]


def fmt_percent(x, digits=1):
    if x is None:
        return "-"
    return f"{100.0 * x:.{digits}f}%"


def fmt_float(x):
    if x is None:
        return "-"
    return f"{x:.6g}"


def collect_all_results():
    all_results = {}

    for root_file in ROOT_FILES:
        mass_label = parse_mass_label(root_file)
        available_signals = get_signal_names(root_file)

        all_results[mass_label] = {}

        for region in REGIONS:
            all_results[mass_label][region] = {}

            for sig_name in ALL_SIGNALS:
                if sig_name not in available_signals:
                    all_results[mass_label][region][sig_name] = None
                    continue

                sig, bkg = combine_eras(root_file, region, sig_name)

                if sig is None or bkg is None:
                    all_results[mass_label][region][sig_name] = None
                    continue

                top3 = get_top3_bins(sig, bkg)
                all_results[mass_label][region][sig_name] = top3 if top3 else []

    return all_results


def write_summary_table(results, outpath):
    sorted_masses = sorted(results.keys(), key=mass_sort_key)

    with open(outpath, "w", encoding="utf-8") as f:
        f.write("Signal statistical uncertainty summary (top-3 most sensitive bins)\n")
        f.write("Values shown are GetBinError/GetBinContent for the top-3 bins ranked by S/sqrt(B).\n")
        f.write("Background is taken from combined data_obs.\n\n")

        colw = 28
        header = (
            f"{'signalDY':<{colw}}"
            f"{'signalVBF':<{colw}}"
            f"{'signalSSWW':<{colw}}"
            f"{'signalWeinberg':<{colw}}\n"
        )

        for mass_label in sorted_masses:
            mass_data = results[mass_label]

            f.write(f"mass : {mass_label}\n")

            for region in REGIONS:
                f.write(f"  {region}\n")
                f.write("    " + header)

                row_parts = []
                for sig_name in ALL_SIGNALS:
                    top3 = mass_data[region].get(sig_name, None)

                    if top3 is None or len(top3) == 0:
                        cell = "-"
                    else:
                        cell = ", ".join(fmt_percent(x["rel"], digits=1) for x in top3)

                    row_parts.append(f"{cell:<{colw}}")

                f.write("    " + "".join(row_parts) + "\n")

            f.write("\n")

        # final global summary
        f.write("\n")
        f.write("Global summary (top-1 bin only)\n")
        f.write("---------------------------------------------\n")

        for sig in ALL_SIGNALS:
            vals = []
        
            for mass_label in sorted_masses:
                mass_value = get_mass_value(mass_label)
        
                # Low-mass rule has priority
                if mass_value is not None and mass_value <= 300:
                    regions_to_use = ["sr3"] # SR3 limit is the leading portion
                else:
                    if sig == "signalVBF":
                        regions_to_use = ["sr1"]          # ignore SR2, SR3 for signalVBF
                    elif sig == "signalSSWW":
                        regions_to_use = ["sr2", "sr3"]   # ignore SR1 for signalSSWW
                    else:
                        regions_to_use = ["sr1", "sr3"]   # ignore SR2 for signalDY
        
                for region in regions_to_use:
                    top3 = results[mass_label][region].get(sig, None)
                    if not top3:
                        continue
        
                    rel = top3[0]["rel"]
                    if rel is not None:
                        vals.append(rel)

            if len(vals) == 0:
                f.write(f"{sig:<15} : -\n")
            else:
                mn = min(vals) * 100.0
                mx = max(vals) * 100.0
                f.write(f"{sig:<15} : {mn:.1f}% - {mx:.1f}%\n")


def write_detailed_table(results, outpath):
    sorted_masses = sorted(results.keys(), key=mass_sort_key)

    with open(outpath, "w", encoding="utf-8") as f:
        f.write("Detailed signal statistical uncertainty table\n")
        f.write("Top-3 bins are ranked by S/sqrt(B), with B taken from combined data_obs.\n\n")

        header_fmt = (
            "{:<6}{:<8}{:<15}{:<15}{:<15}{:<15}{:<15}{:<12}\n"
        )
        row_fmt = (
            "{:<6}{:<8}{:<15}{:<15}{:<15}{:<15}{:<15}{:<12}\n"
        )

        for mass_label in sorted_masses:
            mass_data = results[mass_label]

            f.write("=" * 120 + "\n")
            f.write(f"mass : {mass_label}\n")
            f.write("=" * 120 + "\n")

            for region in REGIONS:
                f.write(f"\n  [{region}]\n")

                for sig_name in ALL_SIGNALS:
                    top3 = mass_data[region].get(sig_name, None)

                    f.write(f"    {sig_name}\n")

                    if top3 is None:
                        f.write("      -\n")
                        continue

                    if len(top3) == 0:
                        f.write("      no valid bins with S>0 and B>0\n")
                        continue

                    f.write("      ")
                    f.write(header_fmt.format(
                        "rank",
                        "bin",
                        "S/sqrt(B)",
                        "S",
                        "B",
                        "err",
                        "content",
                        "err/content"
                    ))

                    for i, x in enumerate(top3, 1):
                        f.write("      ")
                        f.write(row_fmt.format(
                            i,
                            x["bin"],
                            f"{x['score']:.6g}",
                            f"{x['S']:.6g}",
                            f"{x['B']:.6g}",
                            f"{x['err']:.6g}",
                            f"{x['content']:.6g}",
                            fmt_percent(x["rel"], digits=1)
                        ))
                f.write("\n")


def main():
    results = collect_all_results()
    write_summary_table(results, SUMMARY_OUT)
    write_detailed_table(results, DETAIL_OUT)

    print(f"Saved summary table  : {SUMMARY_OUT}")
    print(f"Saved detailed table : {DETAIL_OUT}")


if __name__ == "__main__":
    main()
