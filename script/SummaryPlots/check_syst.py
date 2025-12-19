#!/usr/bin/env python3
import os
import math
import argparse
import ROOT

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)

# Eras and flavours to scan
#ERAS     = ["2016preVFP", "2016postVFP", "2017", "2018"]
#FLAVOURS = ["EE", "MuMu", "EMu"]
ERAS     = ["2018"]
FLAVOURS = ["MuMu"]

# Background components to include in the summed background
# Adjust to your file content if needed
COMPONENTS_BKG = ["fake", "cf", "conv_inc", "wz", "zz", "ww", "prompt_others"]

# Regions (directories)
SR_SUBDIR  = "sr1"
CR1_SUBDIR = "sr1_InvBJet"
CR2_SUBDIR = "sr1_InvMET"
CR3_SUBDIR = "wz_cr1"

# File pattern
FILE_PATTERN = "M500_{flavour}_card_input.root"

# Systematics that should NOT be applied to certain components.
# Extend as needed for your setup.
SYST_DENY = {
    "eff_m_reco_stat": {"fake", "cf"},
    "eff_m_id": {"fake", "cf"},
    "eff_m_trigger": {"fake", "cf"},
    "eff_e_reco": {"fake", "cf"},
    "eff_e_id": {"fake", "cf"},
    "eff_e_trigger": {"fake", "cf"},
    # add other efficiency-like sources as appropriate
}

def build_path(base, era, subdir, flavour):
    return os.path.join(base, era, subdir, FILE_PATTERN.format(flavour=flavour))

def open_root(path):
    f = ROOT.TFile.Open(path, "READ")
    if not f or f.IsZombie():
        raise IOError("Cannot open file: " + path)
    return f

def get_hist_maybe(f, name):
    h = f.Get(name)
    if not h:
        return None
    h = h.Clone()
    h.SetDirectory(0)
    return h

def add_in_place(accum, h):
    if h is None:
        return accum
    if accum is None:
        a = h.Clone()
        a.SetDirectory(0)
        return a
    accum.Add(h)
    return accum

def concat_sr_cr(h_sr, h_cr1, h_cr2, h_cr3):
    nb = 0
    for h in (h_sr, h_cr1, h_cr2, h_cr3):
        if h:
            nb += h.GetNbinsX()
    out = ROOT.TH1D("h_concat", "", nb, 0.5, nb + 0.5)
    out.Sumw2()
    iout = 1
    for h in (h_sr, h_cr1, h_cr2, h_cr3):
        if not h:
            continue
        for ib in range(1, h.GetNbinsX() + 1):
            out.SetBinContent(iout, out.GetBinContent(iout) + h.GetBinContent(ib))
            out.SetBinError(iout, math.hypot(out.GetBinError(iout), h.GetBinError(ib)))
            iout += 1
    return out

def try_variation_hist(f, comp, syst_core, up_or_down):
    """
    Try to fetch a variation histogram with several naming patterns.
    Returns (hist, used_name) or (None, None).
    """
    candidates = [
        f"{comp}_CMS_{syst_core}{up_or_down}",
        f"{comp}_{syst_core}{up_or_down}",
    ]
    for name in candidates:
        h = get_hist_maybe(f, name)
        if h:
            return h, name
    return None, None

def try_nominal_hist(f, comp):
    h = get_hist_maybe(f, comp)
    if h:
        return h, comp
    return None, None

def is_sentinel_variation(h_var, h_nom):
    """
    Detect near-empty or placeholder variation histograms.
    Falls back if var integral is tiny compared to nominal.
    """
    if h_var is None or h_nom is None:
        return True
    i_var = float(h_var.Integral())
    i_nom = float(h_nom.Integral())
    # absolute tiny
    if i_var < 1e-6:
        return True
    # relative tiny (e.g. 0.01 vs O(1-1e3))
    if i_nom > 0 and i_var < 0.005 * i_nom:
        return True
    return False

def build_sum_for_region(f, components, syst_base, era, up_or_down):
    """
    Sum components in one region file for:
      - nominal if up_or_down is None
      - variation otherwise (era-aware), with denylist and sentinel fallback
    Returns (hist, debug_info_list) where entries are (comp, used_name, tag)
    """
    debug = []
    h_sum = None
    for comp in components:
        used = None
        tag = "NOM"
        if up_or_down is None:
            h, used = try_nominal_hist(f, comp)
        else:
            # skip syst for denylisted components
            deny = SYST_DENY.get(syst_base, set())
            if comp in deny:
                h, used = try_nominal_hist(f, comp)
                tag = "FORCED_NOM(no_syst_for_comp)"
            else:
                # era-specific variant first
                core_era = f"{syst_base}_{era}"
                h, used = try_variation_hist(f, comp, core_era, up_or_down)
                tag = f"VAR({core_era}{up_or_down})"
                # base (no era) as fallback
                if h is None:
                    core_base = syst_base
                    h, used = try_variation_hist(f, comp, core_base, up_or_down)
                    tag = f"VAR({core_base}{up_or_down})"
                # if found, validate sentinel vs nominal
                if h is not None:
                    h_nom_for_comp, _ = try_nominal_hist(f, comp)
                    if is_sentinel_variation(h, h_nom_for_comp):
                        h, used = h_nom_for_comp, (used + " -> FALLBACK_NOM_SENTINEL") if used else "FALLBACK_NOM_SENTINEL"
                        tag = "FALLBACK_NOM_SENTINEL"
                # if still missing, use nominal
                if h is None:
                    h, used = try_nominal_hist(f, comp)
                    tag = "FALLBACK_NOM"

        if h is None:
            debug.append((comp, "(missing)", tag))
            continue

        h_sum = add_in_place(h_sum, h)
        debug.append((comp, used, tag))
    return h_sum, debug

def make_bkg_concat_for(era, flav, syst_base=None, up_or_down=None):
    """
    Build concatenated SR+CR1+CR2+CR3 background for era/flav and variation.
    """
    f_sr  = open_root(build_path(args.base, era, SR_SUBDIR,  flav))
    f_cr1 = open_root(build_path(args.base, era, CR1_SUBDIR, flav))
    f_cr2 = open_root(build_path(args.base, era, CR2_SUBDIR, flav))
    f_cr3 = open_root(build_path(args.base, era, CR3_SUBDIR, flav))

    if up_or_down is None:
        h_sr,  dbg_sr  = build_sum_for_region(f_sr,  COMPONENTS_BKG, None, era, None)
        h_c1,  dbg_c1  = build_sum_for_region(f_cr1, COMPONENTS_BKG, None, era, None)
        h_c2,  dbg_c2  = build_sum_for_region(f_cr2, COMPONENTS_BKG, None, era, None)
        h_c3,  dbg_c3  = build_sum_for_region(f_cr3, COMPONENTS_BKG, None, era, None)
    else:
        h_sr,  dbg_sr  = build_sum_for_region(f_sr,  COMPONENTS_BKG, args.syst, era, up_or_down)
        h_c1,  dbg_c1  = build_sum_for_region(f_cr1, COMPONENTS_BKG, args.syst, era, up_or_down)
        h_c2,  dbg_c2  = build_sum_for_region(f_cr2, COMPONENTS_BKG, args.syst, era, up_or_down)
        h_c3,  dbg_c3  = build_sum_for_region(f_cr3, COMPONENTS_BKG, args.syst, era, up_or_down)

    f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    h_concat = concat_sr_cr(h_sr, h_c1, h_c2, h_c3)
    debug = {"SR": dbg_sr, "CR1": dbg_c1, "CR2": dbg_c2, "CR3": dbg_c3}
    return h_concat, debug

def print_debug_map(debug_map, label):
    print("  {} component histograms used:".format(label))
    for region, entries in debug_map.items():
        print("    [{}]".format(region))
        for comp, used_name, tag in entries:
            print("      - {:14s} {:40s}  {}".format(comp, used_name if used_name else "(none)", tag))

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--base", required=True, help="Base directory containing era subdirs")
    parser.add_argument("--syst", default="eff_m_reco_stat", help="Base syst key, e.g. eff_m_reco_stat")
    parser.add_argument("--era", default="", help="If set, only run this era")
    parser.add_argument("--flav", default="", help="If set, only run this flavour (EE, MuMu, EMu)")
    parser.add_argument("--bins", default="", help="Optional bin range like 1:31")
    args_local = parser.parse_args()
    global args
    args = args_local

    eras = [args.era] if args.era else ERAS
    flvs = [args.flav] if args.flav else FLAVOURS

    b0, b1 = 1, None
    if args.bins:
        try:
            parts = [int(x) for x in args.bins.split(":")]
            if len(parts) == 2:
                b0, b1 = parts[0], parts[1]
        except Exception:
            pass

    for era in eras:
        for flav in flvs:
            print("\n=== {} {} ===".format(era, flav))
            try:
                h_nom, dbg_nom = make_bkg_concat_for(era, flav, None, None)
                h_up,  dbg_up  = make_bkg_concat_for(era, flav, args.syst, "Up")
                h_dn,  dbg_dn  = make_bkg_concat_for(era, flav, args.syst, "Down")
            except Exception as e:
                print("  [ERROR] could not build histograms:", e)
                continue

            print_debug_map(dbg_nom, "NOM")
            print_debug_map(dbg_up,  "UP")
            print_debug_map(dbg_dn,  "DOWN")

            i_nom = h_nom.Integral() if h_nom else 0.0
            i_up  = h_up.Integral()  if h_up  else 0.0
            i_dn  = h_dn.Integral()  if h_dn  else 0.0
            r_up  = (i_up / i_nom) if i_nom > 0 else 0.0
            r_dn  = (i_dn / i_nom) if i_nom > 0 else 0.0
            print("  Integrals: Nom = {:.3f}, Up = {:.3f} (ratio {:.3f}), Down = {:.3f} (ratio {:.3f})"
                  .format(i_nom, i_up, r_up, i_dn, r_dn))

            nb = h_nom.GetNbinsX()
            b_start = max(1, b0)
            b_end   = min(nb, b1 if b1 is not None else nb)
            print("  bin\tNominal\tUp\tDown\tDelta")
            for ib in range(b_start, b_end + 1):
                n = h_nom.GetBinContent(ib)
                u = h_up.GetBinContent(ib) if h_up else n
                d = h_dn.GetBinContent(ib) if h_dn else n
                delta = max(abs(u - n), abs(d - n))
                print("  {}\t{:.2f}\t{:.2f}\t{:.2f}\t{:.2f}".format(ib, n, u, d, delta))

            # Optional note if up/down both on same side and far from nominal
            if i_nom > 0:
                same_side = (i_up < i_nom and i_dn < i_nom) or (i_up > i_nom and i_dn > i_nom)
                far_from_nom = (abs(i_up - i_nom) / i_nom > 0.05) and (abs(i_dn - i_nom) / i_nom > 0.05)
                close_to_each_other = abs(i_up - i_dn) / i_nom < 0.01
                if same_side and far_from_nom and close_to_each_other:
                    print("  [NOTE] Up and Down integrals are on the same side and close to each other, both far from nominal.")
                    print("         This can happen if the central SF is not midway between variations or if many comps fallback to nominal.")

if __name__ == "__main__":
    main()
