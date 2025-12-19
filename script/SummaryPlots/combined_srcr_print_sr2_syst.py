#!/usr/bin/env python3
# print_bins_standalone.py
import os
import argparse
import ROOT

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)

ERAS_DEFAULT     = ["2016preVFP", "2016postVFP", "2017", "2018"]
FLAVOURS_DEFAULT = ["EE", "MuMu", "EMu"]

# Background components expected in the files (adjust if yours differ)
BKG_COMPONENTS = ["fake", "cf", "zg", "wz", "zz", "ww", "mc_others"]

def build_path(base, era, subdir, file_pattern, flavour):
    return os.path.join(base, era, subdir, file_pattern.format(flavour=flavour))

def open_root(path):
    f = ROOT.TFile.Open(path, "READ")
    if not f or f.IsZombie():
        raise IOError("Cannot open ROOT file: " + path)
    return f

def get_hist(f, name):
    h = f.Get(name)
    if not h:
        return None
    h = h.Clone()
    h.SetDirectory(0)
    return h

def make_zero_like(template, name):
    h = ROOT.TH1D(name, "", template.GetNbinsX(),
                  template.GetXaxis().GetXmin(), template.GetXaxis().GetXmax())
    h.Sumw2(); h.SetDirectory(0)
    return h

def pad_to(h, nbins):
    if h.GetNbinsX() == nbins:
        return h
    out = ROOT.TH1D(h.GetName()+"_pad", h.GetTitle(), nbins, 0.5, nbins+0.5)
    out.Sumw2(); out.SetDirectory(0)
    for i in range(1, min(h.GetNbinsX(), nbins)+1):
        out.SetBinContent(i, h.GetBinContent(i))
        out.SetBinError(i,   h.GetBinError(i))
    return out

def concat_sr_cr(h_sr, h_cr1, h_cr2, h_cr3, name):
    nb = h_sr.GetNbinsX() + h_cr1.GetNbinsX() + h_cr2.GetNbinsX() + h_cr3.GetNbinsX()
    out = ROOT.TH1D(name, "", nb, 0.5, nb + 0.5)
    out.Sumw2(); out.SetDirectory(0)
    def copy(src, start, tag):
        for i in range(1, src.GetNbinsX()+1):
            out.SetBinContent(start, src.GetBinContent(i))
            out.SetBinError(start,   src.GetBinError(i))
            out.GetXaxis().SetBinLabel(start, f"{tag} {i}")
            start += 1
        return start
    k = 1
    k = copy(h_sr,  k, "SR")
    k = copy(h_cr1, k, "CR1")
    k = copy(h_cr2, k, "CR2")
    copy(h_cr3, k, "CR3")
    return out

def add(a, b):
    if a is None:
        c = b.Clone(); c.SetDirectory(0); return c
    c = a.Clone(); c.Add(b); c.SetDirectory(0); return c

def nbins_from_any(f):
    for name in ["data_obs"] + BKG_COMPONENTS + ["signalSSWW", "signalWeinberg"]:
        h = get_hist(f, name)
        if h: return h.GetNbinsX()
    return 0

def first_template(f):
    h = get_hist(f, "data_obs")
    if not h:
        for n in BKG_COMPONENTS + ["signalSSWW", "signalWeinberg"]:
            h = get_hist(f, n)
            if h: break
    if not h:
        raise KeyError("No template histogram found in file")
    return h

def parse_csv_list(s, default_list):
    if not s: return default_list
    return [x.strip() for x in s.split(",") if x.strip()]

def main():
    ap = argparse.ArgumentParser(description="Print per-bin background, stat error, data, and signals (no plots).")
    ap.add_argument("--base", required=True, help="Base folder containing ERA/subdir/*.root")
    ap.add_argument("--sr-subdir",  default="sr2")
    ap.add_argument("--cr1-subdir", default="sr2_InvBJet")
    ap.add_argument("--cr2-subdir", default="sr2_InvMET")
    ap.add_argument("--cr3-subdir", default="wz_cr2")
    ap.add_argument("--file-pattern",      default="M2000_{flavour}_card_input.root", help="Bkgs+data+signalSSWW")
    ap.add_argument("--file-pattern-sig2", default="Weinberg_{flavour}_card_input.root", help="Weinberg signal only")
    ap.add_argument("--eras",     default="", help="Comma list (e.g. 2017,2018). Default uses all.")
    ap.add_argument("--flavours", default="", help="Comma list (e.g. EE,MuMu). Default uses all.")
    ap.add_argument("--summary-tsv", default="", help="Optional TSV output")
    args = ap.parse_args()

    ERAS     = parse_csv_list(args.eras, ERAS_DEFAULT)
    FLAVOURS = parse_csv_list(args.flavours, FLAVOURS_DEFAULT)

    # PASS 1: discover max SR/CR bin counts to harmonize concatenation
    max_sr = max_cr1 = max_cr2 = max_cr3 = 0
    for era in ERAS:
        for flav in FLAVOURS:
            try:
                f_sr  = open_root(build_path(args.base, era, args.sr_subdir,  args.file_pattern, flav))
                f_cr1 = open_root(build_path(args.base, era, args.cr1_subdir, args.file_pattern, flav))
                f_cr2 = open_root(build_path(args.base, era, args.cr2_subdir, args.file_pattern, flav))
                f_cr3 = open_root(build_path(args.base, era, args.cr3_subdir, args.file_pattern, flav))
            except Exception:
                continue
            max_sr  = max(max_sr,  nbins_from_any(f_sr))
            max_cr1 = max(max_cr1, nbins_from_any(f_cr1))
            max_cr2 = max(max_cr2, nbins_from_any(f_cr2))
            max_cr3 = max(max_cr3, nbins_from_any(f_cr3))
            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    total_bins = max_sr + max_cr1 + max_cr2 + max_cr3
    if total_bins == 0:
        raise RuntimeError("Could not determine total bins from inputs")

    # PASS 2: build concatenated grand histograms across eras/flavours
    grand = {}        # backgrounds + data + signalSSWW
    grand_sig2 = None # signalWeinberg (separate inputs)

    for era in ERAS:
        for flav in FLAVOURS:
            # main pattern (bkgs + data + signalSSWW)
            try:
                f_sr  = open_root(build_path(args.base, era, args.sr_subdir,  args.file_pattern, flav))
                f_cr1 = open_root(build_path(args.base, era, args.cr1_subdir, args.file_pattern, flav))
                f_cr2 = open_root(build_path(args.base, era, args.cr2_subdir, args.file_pattern, flav))
                f_cr3 = open_root(build_path(args.base, era, args.cr3_subdir, args.file_pattern, flav))
            except Exception:
                continue

            try:
                tmpl_sr  = first_template(f_sr)
                tmpl_cr1 = first_template(f_cr1)
                tmpl_cr2 = first_template(f_cr2)
                tmpl_cr3 = first_template(f_cr3)
            except Exception:
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close(); continue

            for comp in BKG_COMPONENTS + ["data_obs", "signalSSWW"]:
                h_sr  = get_hist(f_sr,  comp) or make_zero_like(tmpl_sr,  f"{comp}_sr_zero")
                h_cr1 = get_hist(f_cr1, comp) or make_zero_like(tmpl_cr1, f"{comp}_cr1_zero")
                h_cr2 = get_hist(f_cr2, comp) or make_zero_like(tmpl_cr2, f"{comp}_cr2_zero")
                h_cr3 = get_hist(f_cr3, comp) or make_zero_like(tmpl_cr3, f"{comp}_cr3_zero")

                if h_sr.GetNbinsX()  != max_sr:  h_sr  = pad_to(h_sr,  max_sr)
                if h_cr1.GetNbinsX() != max_cr1: h_cr1 = pad_to(h_cr1, max_cr1)
                if h_cr2.GetNbinsX() != max_cr2: h_cr2 = pad_to(h_cr2, max_cr2)
                if h_cr3.GetNbinsX() != max_cr3: h_cr3 = pad_to(h_cr3, max_cr3)

                h_concat = concat_sr_cr(h_sr, h_cr1, h_cr2, h_cr3, f"{comp}_concat_{era}_{flav}")
                if comp not in grand:
                    grand[comp] = h_concat.Clone(f"{comp}_grand"); grand[comp].SetDirectory(0)
                else:
                    grand[comp].Add(h_concat)

            # second signal (Weinberg) from separate files
            try:
                f_sr_s2  = open_root(build_path(args.base, era, args.sr_subdir,  args.file_pattern_s2 if hasattr(args, "file_pattern_s2") else args.file_pattern_sig2, flav))
                f_cr1_s2 = open_root(build_path(args.base, era, args.cr1_subdir, args.file_pattern_sig2, flav))
                f_cr2_s2 = open_root(build_path(args.base, era, args.cr2_subdir, args.file_pattern_sig2, flav))
                f_cr3_s2 = open_root(build_path(args.base, era, args.cr3_subdir, args.file_pattern_sig2, flav))
            except Exception:
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close(); continue

            try:
                tmpl_sr_s2  = first_template(f_sr_s2)
                tmpl_cr1_s2 = first_template(f_cr1_s2)
                tmpl_cr2_s2 = first_template(f_cr2_s2)
                tmpl_cr3_s2 = first_template(f_cr3_s2)
            except Exception:
                tmpl_sr_s2 = tmpl_cr1_s2 = tmpl_cr2_s2 = tmpl_cr3_s2 = None

            if tmpl_sr_s2:
                comp = "signalWeinberg"
                h_sr2  = get_hist(f_sr_s2,  comp) or make_zero_like(tmpl_sr_s2,  f"{comp}_sr_zero")
                h_c12  = get_hist(f_cr1_s2, comp) or make_zero_like(tmpl_cr1_s2, f"{comp}_cr1_zero")
                h_c22  = get_hist(f_cr2_s2, comp) or make_zero_like(tmpl_cr2_s2, f"{comp}_cr2_zero")
                h_c32  = get_hist(f_cr3_s2, comp) or make_zero_like(tmpl_cr3_s2, f"{comp}_cr3_zero")

                if h_sr2.GetNbinsX()  != max_sr:  h_sr2  = pad_to(h_sr2,  max_sr)
                if h_c12.GetNbinsX() != max_cr1: h_c12 = pad_to(h_c12, max_cr1)
                if h_c22.GetNbinsX() != max_cr2: h_c22 = pad_to(h_c22, max_cr2)
                if h_c32.GetNbinsX() != max_cr3: h_c32 = pad_to(h_c32, max_cr3)

                h_concat_s2 = concat_sr_cr(h_sr2, h_c12, h_c22, h_c32, f"{comp}_concat_{era}_{flav}")
                if grand_sig2 is None:
                    grand_sig2 = h_concat_s2.Clone("signalWeinberg_grand"); grand_sig2.SetDirectory(0)
                else:
                    grand_sig2.Add(h_concat_s2)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
            f_sr_s2.Close(); f_cr1_s2.Close(); f_cr2_s2.Close(); f_cr3_s2.Close()

    # Sum total background
    h_bkg = None
    for comp in BKG_COMPONENTS:
        if comp in grand:
            h_bkg = add(h_bkg, grand[comp])

    if h_bkg is None:
        raise RuntimeError("No background histograms found in inputs")

    h_data = grand.get("data_obs")
    h_sig1 = grand.get("signalSSWW")
    h_sig2 = grand_sig2

    # Print table
    header = ["bin_idx", "bin_label", "bkg", "stat_err", "data", "sig1", "sig2"]
    print("\n=== Bin-by-bin summary (no plots) ===")
    print("\t".join(header))
    ftsv = open(args.summary_tsv, "w") if args.summary_tsv else None
    if ftsv: ftsv.write("\t".join(header) + "\n")

    n = h_bkg.GetNbinsX()
    for i in range(1, n+1):
        label = h_bkg.GetXaxis().GetBinLabel(i)
        bkg   = h_bkg.GetBinContent(i)
        est   = h_bkg.GetBinError(i)
        data  = h_data.GetBinContent(i) if h_data else 0.0
        s1    = h_sig1.GetBinContent(i) if h_sig1 else 0.0
        s2    = h_sig2.GetBinContent(i) if h_sig2 else 0.0
        row = [str(i), label, f"{bkg:.6g}", f"{est:.6g}", f"{data:.6g}", f"{s1:.6g}", f"{s2:.6g}"]
        line = "\t".join(row)
        print(line)
        if ftsv: ftsv.write(line + "\n")

    if ftsv:
        ftsv.close()
        print("[INFO] Wrote TSV:", args.summary_tsv)

if __name__ == "__main__":
    main()
