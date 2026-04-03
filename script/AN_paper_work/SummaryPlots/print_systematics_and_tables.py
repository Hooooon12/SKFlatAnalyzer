
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import math
import re
from array import array
import ROOT

ROOT.gROOT.SetBatch(True)

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

# -------------- Core calc -----------------
def compute_syst_stats_for_key(base_dir, analysis_version, year, flavour, syst_key):
    regions = ["sr1", "sr2", "sr3"]
    bkgs = ["cf","mc_others","fake","ww","wz","zg","zz"]
    signal = "1000"

    EPS = 1e-9
    SENT = 0.001
    def is_sentinel(x):
        return (abs(x) < EPS) or (abs(x - SENT) < 1e-9)

    file_base = f"{base_dir}/{analysis_version}/{year}"
    paths = {r: f"{file_base}/{r}/M{signal}_{flavour}_card_input.root" for r in regions}
    files = {r: ROOT.TFile.Open(p, "READ") for r,p in paths.items()}

    syst_frac = [0.0, 0.0, 0.0]
    stat_frac = [0.0, 0.0, 0.0]

    for i, region in enumerate(regions):
        f = files.get(region)
        if not file_ok(f):
            continue

        nom_aff = 0.0
        up_sum  = 0.0
        dn_sum  = 0.0
        var_nom_aff = 0.0

        for bkg in bkgs:

            if ("fake" in syst_key) != (bkg == "fake"):
                continue
            if ("cf"   in syst_key) != (bkg == "cf"):
                continue

            h_nom = f.Get(bkg)
            h_up  = f.Get(f"{bkg}_CMS_{syst_key}Up")
            h_dn  = f.Get(f"{bkg}_CMS_{syst_key}Down")
            
            v_nom, e_nom = safe_integral_and_error(h_nom)
            v_up_raw = safe_integral(h_up) if h_up else 0.0
            v_dn_raw = safe_integral(h_dn) if h_dn else 0.0

            up_real = (not is_sentinel(v_up_raw))
            dn_real = (not is_sentinel(v_dn_raw))
            affected = up_real or dn_real
            if not affected:
                continue

            up_contrib = v_up_raw if up_real else v_nom
            dn_contrib = v_dn_raw if dn_real else v_nom

            nom_aff     += v_nom
            up_sum      += up_contrib
            dn_sum      += dn_contrib
            var_nom_aff += (e_nom * e_nom)

        if nom_aff > 0.0:
            d_up = abs(up_sum - nom_aff) / nom_aff
            d_dn = abs(dn_sum - nom_aff) / nom_aff
            syst_frac[i] = max(d_up, d_dn)
            stat_frac[i] = math.sqrt(var_nom_aff) / nom_aff
        else:
            syst_frac[i] = 0.0
            stat_frac[i] = 0.0

    for r,f in files.items():
        if file_ok(f):
            f.Close()

    return syst_frac, stat_frac

def compute_stat_total_all_bkgs(base_dir, analysis_version, year, flavour):
    regions = ["sr1", "sr2", "sr3"]
    bkgs = ["cf","mc_others","fake","ww","wz","zg","zz"]
    signal = "1000"
    file_base = f"{base_dir}/{analysis_version}/{year}"
    paths = {r: f"{file_base}/{r}/M{signal}_{flavour}_card_input.root" for r in regions}
    files = {r: ROOT.TFile.Open(p, "READ") for r,p in paths.items()}

    stat_frac = [0.0, 0.0, 0.0]

    for i, region in enumerate(regions):
        f = files.get(region)
        if not file_ok(f):
            continue
        nom_sum = 0.0
        var_sum = 0.0
        for bkg in bkgs:
            h_nom = f.Get(bkg)
            v_nom, e_nom = safe_integral_and_error(h_nom)
            nom_sum += v_nom
            var_sum += (e_nom * e_nom)
        if nom_sum > 0.0:
            stat_frac[i] = math.sqrt(var_sum) / nom_sum
        else:
            stat_frac[i] = 0.0

    for r,f in files.items():
        if file_ok(f):
            f.Close()

    return stat_frac

# --------- Auto classification ---------
def classify_systematics_from_file(root_file):
    year_pattern = re.compile(r"_(2016preVFP|2016postVFP|2017|2018)")
    sr_pattern   = re.compile(r"_sr[123]")

    sr_split, era_split, global_split = set(), set(), set()

    for key in root_file.GetListOfKeys():
        name = key.GetName()
        if not name.startswith("prompt_inc_CMS"):
            continue

        core = re.sub(r"(Up|Down)$", "", name)
        core = core.replace("prompt_inc_CMS_", "")

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

def find_any_prompt_file(base_dir, analysis_version, years_order, flavours):
    signal = "1000"
    for year in years_order:
        for sr in ["sr1", "sr2", "sr3"]:
            for flav in flavours:
                path = f"{base_dir}/{analysis_version}/{year}/{sr}/M{signal}_{flav}_card_input.root"
                f = ROOT.TFile.Open(path, "READ")
                if file_ok(f):
                    return f, path
    return None, None

# ---------- Printing helpers -----------
def fmt_pct_sig2(frac):
    pct = 100.0 * max(0.0, frac)
    if pct <= 0.0:
        return "0 %"
    if pct < 1.0:
        return "$<$ 1 %"
    return two_sigfigs(pct) + " %"

def fmt_or_dash(frac):
    if not math.isfinite(frac) or abs(frac) < 1e-9:
        return "-"
    return fmt_pct_sig2(frac)

def print_line_per_flavour_year(label, year, flavour, syst_frac):
    sr_labels = ["SR1", "SR2", "SR3"]
    parts = [f"{sr_labels[i]}: {fmt_or_dash(syst_frac[i])}" for i in range(3)]
    line = f"[{label}] {flavour} {year} | " + " | ".join(parts)
    print(line)

# ----------------- Main ----------------
def main():
    base_dir    = "/data9/Users/HNL_public/SUS-24-014/LimitInputs"
    InDir="ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"
    analysis_version =InDir
    years_order = ["2016preVFP", "2016postVFP", "2017", "2018"]
    flavours    = ["EE", "MuMu", "EMu"]

    sample_file, _ = find_any_prompt_file(base_dir, analysis_version, years_order, flavours)
    if not sample_file:
        print("[ERROR] Could not find any input ROOT file to classify systematics.")
        return
    sr_split_bases, era_split_bases, global_bases = classify_systematics_from_file(sample_file)
    sample_file.Close()

    results = {}

    for base in sr_split_bases:
        label = f"{base}_sr-split"
        results[label] = {}
        for year in years_order:
            results[label][year] = {}
            for flav in flavours:
                syst_frac = [0.0, 0.0, 0.0]
                for i, sr in enumerate(["sr1","sr2","sr3"]):
                    syst_key = f"{base}_{year}_{sr}"
                    s_fr, _ = compute_syst_stats_for_key(base_dir, analysis_version, year, flav, syst_key)
                    syst_frac[i] = s_fr[i]
                results[label][year][flav] = syst_frac
                print_line_per_flavour_year(label, year, flav, syst_frac)

    for base in era_split_bases:
        label = f"{base}_era-split"
        results[label] = {}
        for year in years_order:
            results[label][year] = {}
            for flav in flavours:
                syst_key = f"{base}_{year}"
                syst_frac, _ = compute_syst_stats_for_key(base_dir, analysis_version, year, flav, syst_key)
                results[label][year][flav] = syst_frac
                print_line_per_flavour_year(label, year, flav, syst_frac)

    for base in global_bases:
        label = f"{base}_global"
        results[label] = {}
        for year in years_order:
            results[label][year] = {}
            for flav in flavours:
                syst_frac, _ = compute_syst_stats_for_key(base_dir, analysis_version, year, flav, base)
                results[label][year][flav] = syst_frac
                print_line_per_flavour_year(label, year, flav, syst_frac)

    # Stat totals per (year, flavour)
    stat_totals = {}
    for year in years_order:
        stat_totals[year] = {}
        for flav in flavours:
            st = compute_stat_total_all_bkgs(base_dir, analysis_version, year, flav)
            stat_totals[year][flav] = st
            sr_labels = ["SR1", "SR2", "SR3"]
            parts = [f"{sr_labels[i]}: {fmt_or_dash(st[i])}" for i in range(3)]
            print(f"[stat_error] {flav} {year} | " + " | ".join(parts))

    # Additional bottom rows: Nonprompt background and Charge mismeasurement
    nonprompt_pct = {"EE": 0.30, "MuMu": 0.20, "EMu": 0.25}
    charge_pct    = {"EE": 0.30, "MuMu": None, "EMu": None}

    # LaTeX-friendly flavour labels
    def flabel(f):
        return {"EE": r"$ee$", "MuMu": r"$\mu\mu$", "EMu": r"$e\mu$"}[f]

    out_path = f"tex/{InDir}_systematics_tables.tex"
    os.system("mkdir -p tex")
    with open(out_path, "w", encoding="utf-8") as tex:
        tex.write("%% Requires \\usepackage{pdflscape} in your preamble\n")
        for year in years_order:
            tex.write("\\begin{landscape}\n")
            tex.write(f"\\begin{{table}}[h]\n\\centering\n")

            tex.write(
                f"\\caption{{Systematic variations on background yields in percentages for {year}. "
                f"Columns are {flabel('EE')}-SR1, {flabel('MuMu')}-SR1, {flabel('EMu')}-SR1, "
            f"{flabel('EE')}-SR2, {flabel('MuMu')}-SR2, {flabel('EMu')}-SR2, "
                f"{flabel('EE')}-SR3, {flabel('MuMu')}-SR3, {flabel('EMu')}-SR3.}}\n"
            )
            tex.write(f"\\label{{tab:systematics_bkg_{year}}}\n")
            tex.write("\\begin{tabular}{l|ccc|ccc|ccc}\n")
            tex.write("\\hline\n")
            
            tex.write("Source & " + " & ".join([
                f"{flabel('EE')}-SR1", f"{flabel('MuMu')}-SR1", f"{flabel('EMu')}-SR1",
                f"{flabel('EE')}-SR2", f"{flabel('MuMu')}-SR2", f"{flabel('EMu')}-SR2",
                f"{flabel('EE')}-SR3", f"{flabel('MuMu')}-SR3", f"{flabel('EMu')}-SR3"
            ]) + " \\\\\n\\hline\n")
            for label in sorted(results.keys()):
                lbl = tex_escape(label)
                row_vals = []
                for sr_idx in [0,1,2]:
                    for flav in ["EE","MuMu","EMu"]:
                        v = results.get(label, {}).get(year, {}).get(flav, None)
                        if v is None:
                            row_vals.append("-")
                        else:
                            s = fmt_or_dash(v[sr_idx])
                            row_vals.append(s.replace(" %","\\%") if s != "-" else s)
                tex.write(f"{lbl} & " + " & ".join(row_vals) + " \\\\\n")
            # stat_error row
            ee_st = stat_totals.get(year, {}).get("EE", [0,0,0])
            mm_st = stat_totals.get(year, {}).get("MuMu", [0,0,0])
            em_st = stat_totals.get(year, {}).get("EMu", [0,0,0])
            def val(st, i):
                s = fmt_or_dash(st[i])
                return s.replace(" %","\\%") if s != "-" else s
            stat_vals = [val(ee_st,0), val(mm_st,0), val(em_st,0),
                         val(ee_st,1), val(mm_st,1), val(em_st,1),
                         val(ee_st,2), val(mm_st,2), val(em_st,2)]
            tex.write("\\hline\n")
            tex.write("stat\\_error (total bkg) & " + " & ".join(stat_vals) + " \\\\\n")
            # Nonprompt and Charge rows
            def rep(v):
                if v is None:
                    return "-"
                return (two_sigfigs(100.0 * v) + " \\%")
            np_vals = [rep(nonprompt_pct["EE"]), rep(nonprompt_pct["MuMu"]), rep(nonprompt_pct["EMu"])] * 3
            ch_vals = [rep(charge_pct["EE"]), rep(charge_pct["MuMu"]), rep(charge_pct["EMu"])] * 3
            tex.write("Nonprompt background & " + " & ".join(np_vals) + " \\\\\n")
            tex.write("Charge mismeasurement & " + " & ".join(ch_vals) + " \\\\\n")
            tex.write("\\hline\n\\end{tabular}\n\\end{table}\n\\end{landscape}\n\n")
    print(f"[INFO] LaTeX tables written to {out_path}")
    print(os.path.abspath(out_path))

if __name__ == "__main__":
    main()
