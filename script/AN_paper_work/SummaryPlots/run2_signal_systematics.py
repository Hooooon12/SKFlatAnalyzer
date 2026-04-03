#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import math
import re
from array import array
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kError  # silence noisy ROOT warnings

# ---------------- Utilities ----------------
def file_ok(f):
    return bool(f) and not f.IsZombie()

def safe_integral(h):
    if not h:
        return 0.0
    v = float(h.Integral())
    return v if math.isfinite(v) else 0.0

def safe_integral_and_error(h):
    if not h:
        return 0.0, 0.0
    err = array('d', [0.0])
    v = float(h.IntegralAndError(1, h.GetNbinsX(), err, ""))
    v = v if math.isfinite(v) else 0.0
    e = float(err[0]) if math.isfinite(err[0]) else 0.0
    return v, e

def two_sigfigs(x):
    if not math.isfinite(x) or x == 0.0:
        return "0"
    sign = "-" if x < 0 else ""
    x = abs(x)
    power = int(math.floor(math.log10(x)))
    scale = 10 ** (power - 1)
    rounded = round(x / scale) * scale
    s = f"{rounded:.15g}"
    if "." in s:
        s = s.rstrip("0").rstrip(".")
    return sign + s

def tex_escape(s):
    return s.replace("_", r"\_")


# ------------ I/O helpers -------------
def open_file(base_dir, analysis_version, year, region, flavour, mass):
    path = f"{base_dir}/{analysis_version}/{year}/{region}/M{mass}_{flavour}_card_input.root"
    if not os.path.exists(path):
        return None
    f = ROOT.TFile.Open(path, "READ")
    return f if file_ok(f) else None


# ------------ Syst helpers ------------
def flavours_for_syst(syst_key: str):
    """Choose flavours depending on syst key name."""
    k = (syst_key or "").lower()
    if "_e_" in k:
        return ["EE"]
    if "_m_" in k:
        return ["MuMu"]
    return ["EE", "MuMu", "EMu"]

def should_skip_base(base: str):
    """Skip fake or cf systematics entirely."""
    b = (base or "").lower()
    return ("fake" in b) or ("cf" in b)


# --------- Core calc (per signal, per mass) ---------
def compute_syst_frac_one_signal_sumflavours(
    base_dir, analysis_version, year, flavours, syst_key, region, mass, sig
):
    EPS = 1e-9
    SENT = 0.001

    def is_sentinel(x):
        return (abs(x) < EPS) or (abs(x - SENT) < 1e-9)

    files = {flav: open_file(base_dir, analysis_version, year, region, flav, mass) for flav in flavours}

    nom_sum = 0.0
    up_sum  = 0.0
    dn_sum  = 0.0

    for flav, f in files.items():
        if not f:
            continue
        h_nom = f.Get(sig)
        h_up  = f.Get(f"{sig}_CMS_{syst_key}Up")
        h_dn  = f.Get(f"{sig}_CMS_{syst_key}Down")

        v_nom, _ = safe_integral_and_error(h_nom)
        v_up_raw = safe_integral(h_up) if h_up else 0.0
        v_dn_raw = safe_integral(h_dn) if h_dn else 0.0

        up_real = (not is_sentinel(v_up_raw))
        dn_real = (not is_sentinel(v_dn_raw))

        up_contrib = v_up_raw if up_real else v_nom
        dn_contrib = v_dn_raw if dn_real else v_nom

        nom_sum += v_nom
        up_sum  += up_contrib
        dn_sum  += dn_contrib

    for f in files.values():
        if file_ok(f):
            f.Close()

    if nom_sum <= 0.0:
        return 0.0

    d_up = abs(up_sum - nom_sum) / nom_sum
    d_dn = abs(dn_sum - nom_sum) / nom_sum
    return max(d_up, d_dn)


def compute_stat_total_combo_sumflavours(
    base_dir, analysis_version, year, flavours, region, sr_signals
):
    masses = sorted(set(m for m, _ in sr_signals))
    files = { (flav, m): open_file(base_dir, analysis_version, year, region, flav, m)
              for flav in flavours for m in masses }

    nom_sum = 0.0
    var_sum = 0.0
    for flav in flavours:
        for m, sig in sr_signals:
            f = files.get((flav, m))
            if not f:
                continue
            h_nom = f.Get(sig)
            v_nom, e_nom = safe_integral_and_error(h_nom)
            nom_sum += v_nom
            var_sum += (e_nom * e_nom)

    for f in files.values():
        if file_ok(f):
            f.Close()

    return (math.sqrt(var_sum) / nom_sum) if nom_sum > 0.0 else 0.0


# --------- Auto classification ---------
def classify_systematics_from_file(root_file, signal_names):
    year_pattern = re.compile(r"_(2016preVFP|2016postVFP|2017|2018)")
    sr_pattern   = re.compile(r"_sr[123]")

    sr_split, era_split, global_split = set(), set(), set()

    for key in root_file.GetListOfKeys():
        name = key.GetName()
        if not any(name.startswith(sig + "_CMS_") for sig in signal_names):
            continue

        core = re.sub(r"(Up|Down)$", "", name)
        for sig in signal_names:
            pref = f"{sig}_CMS_"
            if core.startswith(pref):
                core = core.replace(pref, "", 1)
                break

        has_year = bool(year_pattern.search(core))
        has_sr   = bool(sr_pattern.search(core))

        base = year_pattern.sub("", core)
        base = sr_pattern.sub("", base)
        base = re.sub(r"__+", "_", base).strip("_")

        if has_year and has_sr:
            sr_split.add(base)
        elif has_year:
            era_split.add(base)
        else:
            global_split.add(base)

    return sorted(sr_split), sorted(era_split), sorted(global_split)


def find_any_signal_file(base_dir, analysis_version, years_order, flavours, signal_names, masses):
    for year in years_order:
        for sr in ["sr1", "sr2", "sr3"]:
            for flav in flavours:
                for m in masses:
                    f = open_file(base_dir, analysis_version, year, sr, flav, m)
                    if f:
                        if any(bool(f.Get(sig)) for sig in signal_names):
                            return f
                        f.Close()
    return None


# ---------- Formatting helpers ----------
def fmt_range(fracs):
    vals = [max(0.0, float(f)) for f in fracs if math.isfinite(f)]
    if not vals:
        return "-"
    mn, mx = min(vals), max(vals)
    if mx < 0.01:
        return "$<$ 1 \\%"
    mn_s = two_sigfigs(100.0 * mn)
    mx_s = two_sigfigs(100.0 * mx)
    if mn == mx:
        return f"{mn_s} \\%"
    return f"{mn_s}--{mx_s} \\%"

def build_mapping_text(sr_combo):
    parts = []
    for sr in ["sr1", "sr2", "sr3"]:
        entries = sr_combo.get(sr, [])
        by_sig = {}
        for m, sig in entries:
            by_sig.setdefault(sig, []).append(m)
        sig_chunks = []
        for sig, masses in by_sig.items():
            seen = []
            for x in masses:
                if x not in seen:
                    seen.append(x)
            masses_str = ",".join(seen)
            pretty = sig.replace("signal", "")
            sig_chunks.append(f"{pretty}({masses_str})")
        sr_upper = sr.upper()
        parts.append(f"{sr_upper} = " + "+".join(sig_chunks))
    return ", ".join(parts)


# ----------------- Main ----------------
def main():
    base_dir    = "/data9/Users/HNL_public/SUS-24-014/LimitInputs"
    InDir       = "ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"
    analysis_version = InDir

    years_order = ["2016preVFP", "2016postVFP", "2017", "2018"]
    flavours    = ["EE", "MuMu", "EMu"]

    sr_combo = {
        "sr1": [("500", "signalDY"), ("500", "signalVBF"), ("700", "signalDY"), ("700", "signalVBF"), ("1000", "signalDY"), ("1000", "signalVBF"), ("2000", "signalDY"), ("2000", "signalVBF")],
        "sr2": [("1000", "signalSSWW"), ("2000", "signalSSWW"), ("10000", "signalSSWW"), ("20000", "signalSSWW")],
        "sr3": [("100", "signalDY"), ("400", "signalDY"), ("700", "signalDY"), ("1000", "signalDY"), ("2000", "signalDY"), ("700", "signalVBF"), ("1000", "signalVBF"), ("2000", "signalVBF")],
    }

    mapping_text = build_mapping_text(sr_combo)

    masses_in_use = sorted({m for pairs in sr_combo.values() for (m, _) in pairs})
    signal_names_all = ["signalDY", "signalVBF", "signalSSWW"]

    sample_file = find_any_signal_file(base_dir, analysis_version, years_order, flavours, signal_names_all, masses_in_use)
    if not sample_file:
        print("[ERROR] Could not find any input ROOT file with signal histograms to classify systematics.")
        return
    sr_split_bases, era_split_bases, global_bases = classify_systematics_from_file(sample_file, signal_names_all)
    sample_file.Close()

    sr_split_bases   = [b for b in sr_split_bases   if not should_skip_base(b)]
    era_split_bases  = [b for b in era_split_bases  if not should_skip_base(b)]
    global_bases     = [b for b in global_bases     if not should_skip_base(b)]

    results = {}

    # SR-split
    for base in sr_split_bases:
        label = f"{base}_sr-split"
        results[label] = {}
        for year in years_order:
            vals = []
            for sr in ["sr1","sr2","sr3"]:
                syst_key = f"{base}_{year}_{sr}"
                flavs = flavours_for_syst(syst_key)
                sr_vals = []
                for (m, sig) in sr_combo[sr]:
                    s_fr = compute_syst_frac_one_signal_sumflavours(
                        base_dir, analysis_version, year, flavs, syst_key, sr, m, sig
                    )
                    sr_vals.append(s_fr)
                vals.append(sr_vals)
            results[label][year] = vals

    # Era-split
    for base in era_split_bases:
        label = f"{base}_era-split"
        results[label] = {}
        for year in years_order:
            vals = []
            for sr in ["sr1","sr2","sr3"]:
                syst_key = f"{base}_{year}"
                flavs = flavours_for_syst(syst_key)
                sr_vals = []
                for (m, sig) in sr_combo[sr]:
                    s_fr = compute_syst_frac_one_signal_sumflavours(
                        base_dir, analysis_version, year, flavs, syst_key, sr, m, sig
                    )
                    sr_vals.append(s_fr)
                vals.append(sr_vals)
            results[label][year] = vals

    # Global
    for base in global_bases:
        label = f"{base}_global"
        results[label] = {}
        for year in years_order:
            vals = []
            for sr in ["sr1","sr2","sr3"]:
                flavs = flavours_for_syst(base)
                sr_vals = []
                for (m, sig) in sr_combo[sr]:
                    s_fr = compute_syst_frac_one_signal_sumflavours(
                        base_dir, analysis_version, year, flavs, base, sr, m, sig
                    )
                    sr_vals.append(s_fr)
                vals.append(sr_vals)
            results[label][year] = vals

    # Stat-only per year (yield-weighted)
    stat_per_year = {}
    for year in years_order:
        st = []
        for sr in ["sr1","sr2","sr3"]:
            st.append(
                compute_stat_total_combo_sumflavours(base_dir, analysis_version, year, flavours, sr, sr_combo[sr])
            )
        stat_per_year[year] = st

    # ---------- LaTeX output ----------
    out_path = f"tex_signal/{InDir}_signalSystematics_Run2_ranges.tex"
    os.makedirs(os.path.dirname(out_path), exist_ok=True)

    with open(out_path, "w", encoding="utf-8") as tex:
        tex.write("%% Requires \\usepackage{pdflscape}\n")
        tex.write("\\begin{table}[!ht]\n\\centering\n")
        tex.write("\\caption{Run~2 ranges of systematic percentages after summing signals over flavours "
                  "(EE+$\\mu\\mu$+$e\\mu$). Mapping: " + mapping_text +
                  ". Each cell shows the min--max across 2016preVFP, 2016postVFP, 2017, 2018 and across the listed masses per SR.}\n")
        tex.write("\\begin{tabular}{l|ccc}\n\\hline\n")
        tex.write("Source & SR1 & SR2 & SR3 \\\\\n\\hline\n")

        for label in sorted(results.keys()):
            sr1_vals, sr2_vals, sr3_vals = [], [], []
            for y in years_order:
                if y not in results[label]:
                    continue
                v = results[label][y]
                if len(v) >= 1: sr1_vals.extend(v[0])
                if len(v) >= 2: sr2_vals.extend(v[1])
                if len(v) >= 3: sr3_vals.extend(v[2])

            row = f"{tex_escape(label)} & {fmt_range(sr1_vals)} & {fmt_range(sr2_vals)} & {fmt_range(sr3_vals)} \\\\\n"
            tex.write(row)

        sr1_stat = [stat_per_year[y][0] for y in years_order]
        sr2_stat = [stat_per_year[y][1] for y in years_order]
        sr3_stat = [stat_per_year[y][2] for y in years_order]

        tex.write("\\hline\n")
        tex.write(f"stat\\_error (signal, EE+$\\mu\\mu$+$e\\mu$) & "
                  f"{fmt_range(sr1_stat)} & {fmt_range(sr2_stat)} & {fmt_range(sr3_stat)} \\\\\n")
        tex.write("\\hline\n\\end{tabular}\n\\end{table}\n\\n")

    print(f"[INFO] LaTeX Run2 range table written to {out_path}")
    print(os.path.abspath(out_path))


if __name__ == "__main__":
    main()
