#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import glob
import argparse
from math import isnan
import ROOT

ROOT.gROOT.SetBatch(True)

DEFAULT_BASE = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr"
DEFAULT_ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]
DEFAULT_FLAVOURS = ["MuMu", "EE", "EMu"]
DEFAULT_MASSES = [
    100,125,150,200,250,300,400,500,600,700,800,900,
    1000,1500,2000,5000,10000,15000,25000,30000
]
DEFAULT_SCALE_TYPES = ["RenScale", "FacScale"]
SCALE_LABELS = {
    "RenScale": "RenScale",
    "FacScale": "FacScale",
}

# ----------------- helpers -----------------

def integral_of_hist(h):
    if not h:
        return None
    if isinstance(h, ROOT.TH1):
        return h.Integral(1, h.GetNbinsX())
    try:
        return h.Integral()
    except Exception:
        return None

def list_all_keys(tfile):
    keys = []
    def recurse(dir_obj, prefix=""):
        lst = dir_obj.GetListOfKeys()
        if not lst:
            return
        for key in lst:
            name = key.GetName()
            full_name = prefix + name
            keys.append(full_name)
            obj = dir_obj.Get(name)
            if obj and obj.InheritsFrom("TDirectory"):
                recurse(obj, prefix=full_name + "/")
    recurse(tfile, "")
    return sorted(keys)

def resolve_hist_path(all_keys, basename):
    base_lower = basename.lower()
    for full in all_keys:
        if full.lower() == base_lower:
            return full
    for full in all_keys:
        if full.split("/")[-1].lower() == base_lower:
            return full
    return None

def read_ratio_band_with_details(tfile, all_keys, base_name, up_name, down_name):
    nom_path  = resolve_hist_path(all_keys, base_name)
    up_path   = resolve_hist_path(all_keys, up_name)
    dn_path   = resolve_hist_path(all_keys, down_name)
    paths = {"nom": nom_path, "up": up_path, "dn": dn_path}

    if not nom_path or not up_path or not dn_path:
        return (None, None, None, None, None, paths, "hist_missing")

    h_nom = tfile.Get(nom_path)
    h_up  = tfile.Get(up_path)
    h_dn  = tfile.Get(dn_path)

    i_nom = integral_of_hist(h_nom)
    i_up  = integral_of_hist(h_up)
    i_dn  = integral_of_hist(h_dn)

    if i_nom is None or i_up is None or i_dn is None:
        return (None, None, i_nom, i_up, i_dn, paths, "integral_none")
    if i_nom == 0:
        return (None, None, i_nom, i_up, i_dn, paths, "nominal_zero")

    r_up = i_up / i_nom
    r_dn = i_dn / i_nom
    if any(isnan(x) for x in [r_up, r_dn]):
        return (None, None, i_nom, i_up, i_dn, paths, "nan_ratio")

    return (min(r_dn, r_up), max(r_dn, r_up), i_nom, i_up, i_dn, paths, None)

def ensure_dir(path):
    os.makedirs(path, exist_ok=True)

def autodetect_sr(base_dir, era, flavour, masses):
    era_dir = os.path.join(base_dir, era)
    if not os.path.isdir(era_dir):
        return None
    cand = [d for d in glob.glob(os.path.join(era_dir, "*")) if os.path.isdir(d)]
    for sr_dir in sorted(cand):
        for m in masses:
            pattern = os.path.join(sr_dir, f"M{m}_{flavour}_card_input.root")
            if glob.glob(pattern):
                return os.path.basename(sr_dir)
    return None

# --------- frame + drawing helpers ---------
def make_frame_hist(name, nbins, labels, ymin, ymax, title, ytitle="Scale ratio to nominal"):
    """
    Unique, directory-detached frame histogram to avoid:
    TROOT::Append: Replacing existing TH1: frame (Potential memory leak).
    """
    hframe = ROOT.TH1F(name, title, nbins, 0.5, nbins + 0.5)
    hframe.SetDirectory(0)  # detach from gDirectory
    hframe.GetYaxis().SetRangeUser(ymin, ymax)
    hframe.GetYaxis().SetTitle(ytitle)
    hframe.GetXaxis().SetTitle("Mass [GeV]")
    for i, lab in enumerate(labels, start=1):
        hframe.GetXaxis().SetBinLabel(i, str(lab))
    hframe.LabelsOption("h", "X")
    hframe.SetStats(0)
    return hframe

def graph_band_from_points(xs, ymins, ymaxs, fill_color, alpha=0.35, line_color=None):
    n = len(xs)
    g = ROOT.TGraphAsymmErrors(n)
    for i, (x, ylo, yhi) in enumerate(zip(xs, ymins, ymaxs)):
        if ylo is None or yhi is None:
            g.SetPoint(i, x, 0.0)
            g.SetPointError(i, 0.0, 0.0, 0.0, 0.0)
        else:
            y = 0.5*(ylo + yhi)
            el = y - ylo
            eu = yhi - y
            g.SetPoint(i, x, y)
            g.SetPointError(i, 0.45, 0.45, el, eu)
    color = fill_color if line_color is None else line_color
    g.SetFillStyle(1001)
    g.SetFillColorAlpha(fill_color, alpha)
    g.SetLineColor(color)
    g.SetLineWidth(1)
    g.SetMarkerStyle(0)
    return g

def avg_band_height(ymins, ymaxs):
    vals = [yhi - ylo for ylo, yhi in zip(ymins, ymaxs) if ylo is not None and yhi is not None]
    return sum(vals)/len(vals) if vals else 0.0

def has_any_pair(ymins, ymaxs):
    return any((ylo is not None and yhi is not None) for ylo, yhi in zip(ymins, ymaxs))

def draw_edge_lines(xs, ymins, ymaxs, color, style=1, width=3):
    g_lo = ROOT.TGraph(); g_lo.SetName("")  # avoid default name collisions
    g_hi = ROOT.TGraph(); g_hi.SetName("")
    ilo = ihi = 0
    for x, ylo, yhi in zip(xs, ymins, ymaxs):
        if ylo is not None and yhi is not None:
            g_lo.SetPoint(ilo, x, ylo); ilo += 1
            g_hi.SetPoint(ihi, x, yhi); ihi += 1
    for g in (g_lo, g_hi):
        g.SetLineColor(color)
        g.SetLineStyle(style)
        g.SetLineWidth(width)
        g.Draw("L same")
    return [g_lo, g_hi]  # keep alive

def draw_per_bin_lines(xs, dy_lo, dy_hi, wg_lo, wg_hi, ss_lo, ss_hi, wb_lo=None, wb_hi=None):
    drawn = []
    col_dy = ROOT.kAzure - 9
    col_wg = ROOT.kMagenta - 4
    col_ss = ROOT.kOrange - 3
    col_wb = ROOT.kGray + 1
    lw = 3
    style_dy = 1  # solid
    style_wg = 1  # solid
    style_ss = 2  # dashed
    style_wb = 3  # dotted

    if wb_lo is None:
        wb_lo = [None] * len(xs)
    if wb_hi is None:
        wb_hi = [None] * len(xs)

    for i, x in enumerate(xs):
        x1 = x - 0.45
        x2 = x + 0.45

        if dy_lo[i] is not None:
            l1 = ROOT.TLine(x1, dy_lo[i], x2, dy_lo[i])
            l1.SetLineColor(col_dy); l1.SetLineStyle(style_dy); l1.SetLineWidth(lw); l1.Draw()
            drawn.append(l1)
        if dy_hi[i] is not None:
            l2 = ROOT.TLine(x1, dy_hi[i], x2, dy_hi[i])
            l2.SetLineColor(col_dy); l2.SetLineStyle(style_dy); l2.SetLineWidth(lw); l2.Draw()
            drawn.append(l2)

        if wg_lo[i] is not None:
            l3 = ROOT.TLine(x1, wg_lo[i], x2, wg_lo[i])
            l3.SetLineColor(col_wg); l3.SetLineStyle(style_wg); l3.SetLineWidth(lw); l3.Draw()
            drawn.append(l3)
        if wg_hi[i] is not None:
            l4 = ROOT.TLine(x1, wg_hi[i], x2, wg_hi[i])
            l4.SetLineColor(col_wg); l4.SetLineStyle(style_wg); l4.SetLineWidth(lw); l4.Draw()
            drawn.append(l4)

        if ss_lo[i] is not None:
            l5 = ROOT.TLine(x1, ss_lo[i], x2, ss_lo[i])
            l5.SetLineColor(col_ss); l5.SetLineStyle(style_ss); l5.SetLineWidth(lw); l5.Draw()
            drawn.append(l5)
        if ss_hi[i] is not None:
            l6 = ROOT.TLine(x1, ss_hi[i], x2, ss_hi[i])
            l6.SetLineColor(col_ss); l6.SetLineStyle(style_ss); l6.SetLineWidth(lw); l6.Draw()
            drawn.append(l6)

        if wb_lo[i] is not None:
            l7 = ROOT.TLine(x1, wb_lo[i], x2, wb_lo[i])
            l7.SetLineColor(col_wb); l7.SetLineStyle(style_wb); l7.SetLineWidth(lw); l7.Draw()
            drawn.append(l7)
        if wb_hi[i] is not None:
            l8 = ROOT.TLine(x1, wb_hi[i], x2, wb_hi[i])
            l8.SetLineColor(col_wb); l8.SetLineStyle(style_wb); l8.SetLineWidth(lw); l8.Draw()
            drawn.append(l8)

    return drawn

# --------- SR / mass filters ---------

def maybe_filter_masses_by_sr(masses, sr):
    if sr is None:
        return masses
    s = str(sr).lower()
    if s == "sr1":
        return [m for m in masses if 500 <= m <= 3000]
    if s == "sr2":
        return [m for m in masses if m >= 500]
    if s == "sr3":
        return [m for m in masses if m <= 3000]
    return masses

def add_weinberg_first_if_needed(labels, base_dir, era, sr, flavour):
    """
    For SR2 and SR3 only, add 'Weinberg' as first label.
    """
    s = str(sr).lower()
    if s in ("sr2", "sr3"):
        return ["Weinberg"] + labels
    return labels

# ----------------- main drawing -----------------

def draw_one_plot(base_dir, era, flavour, masses, sr, outdir, print_all_keys=False, scale_key="RenScale"):
    assert isinstance(flavour, str), "flavour must be a string like 'MuMu', 'EE', or 'EMu'"
    if scale_key not in SCALE_LABELS:
        raise ValueError(f"Unsupported scale_key={scale_key}. Use one of {sorted(SCALE_LABELS)}")

    scale_label = SCALE_LABELS[scale_key]
    ytitle = f"{scale_label} ratio to nominal"

    masses_filtered = maybe_filter_masses_by_sr(masses, sr)
    labels = [str(m) for m in masses_filtered]
    labels = add_weinberg_first_if_needed(labels, base_dir, era, sr, flavour)

    if not labels:
        print(f"[WARN] After SR filtering, no bins left to plot for {era} {flavour} (SR={sr}, scale={scale_key})")
        return

    nbins = len(labels)
    xs = list(range(1, nbins + 1))
    dy_lo, dy_hi = [], []
    wg_lo, wg_hi = [], []
    ss_lo, ss_hi = [], []
    wb_lo, wb_hi = [], []

    print(f"\n=== DEBUG: {era} {flavour} (SR={sr}, scale={scale_key}) ===")
    for lab in labels:
        if lab == "Weinberg":
            fname = f"Weinberg_{flavour}_card_input.root"
        else:
            fname = f"M{lab}_{flavour}_card_input.root"
        fpath = os.path.join(base_dir, era, sr, fname)

        if not os.path.isfile(fpath):
            print(f"[{lab}] FILE missing: {fpath} -> CCDY=None, Wgamma=None, SSWW=None, Weinberg=None")
            dy_lo.append(None); dy_hi.append(None)
            wg_lo.append(None); wg_hi.append(None)
            ss_lo.append(None); ss_hi.append(None)
            wb_lo.append(None); wb_hi.append(None)
            continue

        tf = ROOT.TFile.Open(fpath, "READ")
        if not tf or tf.IsZombie():
            print(f"[{lab}] FILE unreadable: {fpath} -> CCDY=None, Wgamma=None, SSWW=None, Weinberg=None")
            dy_lo.append(None); dy_hi.append(None)
            wg_lo.append(None); wg_hi.append(None)
            ss_lo.append(None); ss_hi.append(None)
            wb_lo.append(None); wb_hi.append(None)
            continue

        all_keys = list_all_keys(tf)
        print(f"[{lab}] Found {len(all_keys)} objects in ROOT file")
        if print_all_keys:
            for k in all_keys:
                print("    {}".format(k))

        if lab == "Weinberg":
            w_lo, w_hi, w_inom, w_iup, w_idn, _, w_status = read_ratio_band_with_details(
                tf, all_keys,
                "signalWeinberg",
                f"signalWeinberg_{scale_key}_WeinbergUp",
                f"signalWeinberg_{scale_key}_WeinbergDown"
            )
            if w_status is None:
                print(f"[Weinberg] {scale_key}: i_nom={w_inom:.6g}, i_up={w_iup:.6g}, i_dn={w_idn:.6g} "
                      f"-> r_up={max(w_lo, w_hi):.6f}, r_dn={min(w_lo, w_hi):.6f}")
                wb_lo.append(w_lo); wb_hi.append(w_hi)
            else:
                print(f"[Weinberg] {scale_key} missing: {w_status} -> None")
                wb_lo.append(None); wb_hi.append(None)

            dy_lo.append(None); dy_hi.append(None)
            wg_lo.append(None); wg_hi.append(None)
            ss_lo.append(None); ss_hi.append(None)

            tf.Close()
            continue

        # CCDY/DY: skip for SR2 due to limited statistics

        # CCDY/DY: skip for SR2 due to limited statistics
        if str(sr).lower() == "sr2":
            print(f"[{lab}] CCDY : skipped for SR2")
            dy_lo.append(None); dy_hi.append(None)
        else:
            d_lo, d_hi, d_inom, d_iup, d_idn, d_paths, d_status = read_ratio_band_with_details(
                tf, all_keys,
                "signalDY",
                f"signalDY_{scale_key}_DYUp",
                f"signalDY_{scale_key}_DYDown"
            )
            if d_status is None:
                print(f"[{lab}] CCDY: i_nom={d_inom:.6g}, i_up={d_iup:.6g}, i_dn={d_idn:.6g} "
                      f"-> r_up={max(d_lo, d_hi):.6f}, r_dn={min(d_lo, d_hi):.6f}")
                dy_lo.append(d_lo); dy_hi.append(d_hi)
            else:
                print(f"[{lab}] CCDY missing: {d_status} -> None")
                dy_lo.append(None); dy_hi.append(None)

        # Wgamma/VBF: skip for SR2 due to limited statistics
        if str(sr).lower() == "sr2":
            print(f"[{lab}] Wgamma : skipped for SR2")
            wg_lo.append(None); wg_hi.append(None)
        else:
            w_lo, w_hi, w_inom, w_iup, w_idn, w_paths, w_status = read_ratio_band_with_details(
                tf, all_keys,
                "signalVBF",
                f"signalVBF_{scale_key}_VBFUp",
                f"signalVBF_{scale_key}_VBFDown"
            )
            if w_status is None:
                print(f"[{lab}] Wgamma: i_nom={w_inom:.6g}, i_up={w_iup:.6g}, i_dn={w_idn:.6g} "
                      f"-> r_up={max(w_lo, w_hi):.6f}, r_dn={min(w_lo, w_hi):.6f}")
                wg_lo.append(w_lo); wg_hi.append(w_hi)
            else:
                print(f"[{lab}] Wgamma missing: {w_status} -> None")
                wg_lo.append(None); wg_hi.append(None)

        if str(sr).lower() == "sr1":
            print(f"[{lab}] SSWW : skipped for SR1")
            ss_lo.append(None); ss_hi.append(None)
        else:
            s_lo, s_hi, s_inom, s_iup, s_idn, s_paths, s_status = read_ratio_band_with_details(
                tf, all_keys,
                "signalSSWW",
                f"signalSSWW_{scale_key}_SSWWUp",
                f"signalSSWW_{scale_key}_SSWWDown"
            )
            if s_status is None:
                print(f"[{lab}] SSWW : i_nom={s_inom:.6g}, i_up={s_iup:.6g}, i_dn={s_idn:.6g} "
                      f"-> r_up={max(s_lo, s_hi):.6f}, r_dn={min(s_lo, s_hi):.6f}")
                ss_lo.append(s_lo); ss_hi.append(s_hi)
            else:
                print(f"[{lab}] SSWW missing: {s_status} -> None")
                ss_lo.append(None); ss_hi.append(None)

        wb_lo.append(None); wb_hi.append(None)

        tf.Close()

    vals = [v for pair in zip(dy_lo, dy_hi) for v in pair if v is not None] + \
           [v for pair in zip(wg_lo, wg_hi) for v in pair if v is not None] + \
           [v for pair in zip(ss_lo, ss_hi) for v in pair if v is not None] + \
           [v for pair in zip(wb_lo, wb_hi) for v in pair if v is not None]
    if not vals:
        print(f"[WARN] No valid ratios for {era} {flavour} (SR={sr}, scale={scale_key}). Skipping plots.")
        return

    ymin_raw = min(vals)
    ymax_raw = max(vals)
    span = (ymax_raw - ymin_raw) if ymax_raw > ymin_raw else 1.0
    ymin = max(0.0, ymin_raw - 0.05 * span)
    ymax = ymax_raw + 0.25 * span

    title_band = f"{era} {flavour}, SR={sr}, {scale_label}"
    cname_band = f"c_band_{scale_key}_{era}_{flavour}_{sr}"
    c1 = ROOT.TCanvas(cname_band, title_band, 1100, 700)
    c1.SetMargin(0.11, 0.04, 0.16, 0.08)

    frame1_name = f"frame_band_{scale_key}_{era}_{flavour}_{sr}"
    hframe1 = make_frame_hist(frame1_name, nbins, labels, ymin, ymax, title_band, ytitle=ytitle)
    hframe1.Draw("AXIS")

    unity1 = ROOT.TLine(0.5, 1.0, nbins + 0.5, 1.0)
    unity1.SetLineStyle(2)
    unity1.SetLineColor(ROOT.kGray+2)
    unity1.Draw()

    g_dy = graph_band_from_points(xs, dy_lo, dy_hi, ROOT.kAzure-9, alpha=0.40)
    have_dy = has_any_pair(dy_lo, dy_hi)
    dy_w = avg_band_height(dy_lo, dy_hi)

    g_wg = graph_band_from_points(xs, wg_lo, wg_hi, ROOT.kMagenta-4, alpha=0.40)
    have_wg = has_any_pair(wg_lo, wg_hi)
    wg_w = avg_band_height(wg_lo, wg_hi)

    is_sr1 = (str(sr).lower() == "sr1")
    g_ss = None; have_ss = False; ss_w = 0.0
    if not is_sr1:
        g_ss = graph_band_from_points(xs, ss_lo, ss_hi, ROOT.kOrange-3, alpha=0.35)
        have_ss = has_any_pair(ss_lo, ss_hi)
        ss_w = avg_band_height(ss_lo, ss_hi)

    g_wb = graph_band_from_points(xs, wb_lo, wb_hi, ROOT.kGray+1, alpha=0.25)
    have_wb = has_any_pair(wb_lo, wb_hi)

    bands_keep = []
    if have_wb:
        g_wb.Draw("E2 same")
        bands_keep.append(g_wb)

    bands_to_draw = []
    if have_dy:
        bands_to_draw.append((dy_w, g_dy))
    if have_wg:
        bands_to_draw.append((wg_w, g_wg))
    if (not is_sr1) and have_ss:
        bands_to_draw.append((ss_w, g_ss))

    bands_to_draw.sort(key=lambda x: x[0], reverse=True)

    for _, g in bands_to_draw:
        g.Draw("E2 same")
        bands_keep.append(g)

    bands_keep += draw_edge_lines(xs, dy_lo, dy_hi, ROOT.kAzure-3, style=1, width=3)
    bands_keep += draw_edge_lines(xs, wg_lo, wg_hi, ROOT.kMagenta-4, style=1, width=3)
    if not is_sr1:
        bands_keep += draw_edge_lines(xs, ss_lo, ss_hi, ROOT.kOrange+7, style=2, width=3)
    if have_wb:
        bands_keep += draw_edge_lines(xs, wb_lo, wb_hi, ROOT.kGray+2, style=3, width=3)

    hframe1.Draw("AXIS same")

    leg1 = ROOT.TLegend(0.60, 0.74, 0.90, 0.92)
    leg1.SetBorderSize(0); leg1.SetFillStyle(0)
    if have_dy:
        leg1.AddEntry(g_dy, f"CCDY {scale_label} band", "f")
    if have_wg:
        leg1.AddEntry(g_wg, f"W#gamma {scale_label} band", "f")
    if (not is_sr1) and have_ss:
        leg1.AddEntry(g_ss, f"SSWW {scale_label} band", "f")
    if have_wb:
        leg1.AddEntry(g_wb, f"Weinberg {scale_label} band", "f")
    leg1.Draw()

    lab1 = ROOT.TLatex()
    lab1.SetNDC(True)
    lab1.SetTextFont(42)
    lab1.SetTextSize(0.045)
    lab1.SetTextAlign(13)
    lab1.DrawLatex(0.14, 0.96, f"{era}, {flavour}")

    if labels[0] == "Weinberg":
        sep_line = ROOT.TLine(1.5, ymin, 1.5, ymax)
        sep_line.SetLineStyle(2)
        sep_line.SetLineWidth(2)
        sep_line.SetLineColor(ROOT.kBlack)
        sep_line.Draw()
        bands_keep.append(sep_line)

    c1._keep_bands = bands_keep + [leg1, lab1, unity1, hframe1]

    ensure_dir(outdir)
    base = f"{era}_{flavour}_SR-{sr}_{scale_key}Bands"
    full_base_band = os.path.join(outdir, f"{scale_key}_bands_plots_" + base)
    for ext in ("png", "pdf"):
        c1.SaveAs(f"{full_base_band}.{ext}")
    c1.Close()

    title_lines = f"{era} {flavour}, SR={sr}, {scale_label}"
    cname_lines = f"c_lines_{scale_key}_{era}_{flavour}_{sr}"
    c2 = ROOT.TCanvas(cname_lines, title_lines, 1100, 700)
    c2.SetMargin(0.11, 0.04, 0.16, 0.08)

    frame2_name = f"frame_lines_{scale_key}_{era}_{flavour}_{sr}"
    hframe2 = make_frame_hist(frame2_name, nbins, labels, ymin, ymax, title_lines, ytitle=ytitle)
    hframe2.Draw("AXIS")

    unity2 = ROOT.TLine(0.5, 1.0, nbins + 0.5, 1.0)
    unity2.SetLineStyle(2)
    unity2.SetLineColor(ROOT.kGray+2)
    unity2.Draw()

    keep = []
    keep += draw_per_bin_lines(xs, dy_lo, dy_hi,
                               wg_lo, wg_hi,
                               ss_lo if not is_sr1 else [None]*nbins,
                               ss_hi if not is_sr1 else [None]*nbins,
                               wb_lo, wb_hi)

    hframe2.Draw("AXIS same")

    leg2 = ROOT.TLegend(0.58, 0.70, 0.92, 0.92)
    leg2.SetBorderSize(0); leg2.SetFillStyle(0)

    demo_dy = ROOT.TGraph(); demo_dy.SetLineColor(ROOT.kAzure-9); demo_dy.SetLineStyle(1); demo_dy.SetLineWidth(3)
    leg2.AddEntry(demo_dy, f"CCDY {scale_label} up/down lines", "l")
    keep.append(demo_dy)

    demo_wg = ROOT.TGraph(); demo_wg.SetLineColor(ROOT.kMagenta-4); demo_wg.SetLineStyle(1); demo_wg.SetLineWidth(3)
    leg2.AddEntry(demo_wg, f"W#gamma {scale_label} up/down lines", "l")
    keep.append(demo_wg)

    if not is_sr1:
        demo_ss = ROOT.TGraph(); demo_ss.SetLineColor(ROOT.kOrange-3); demo_ss.SetLineStyle(2); demo_ss.SetLineWidth(3)
        leg2.AddEntry(demo_ss, f"SSWW {scale_label} up/down lines", "l")
        keep.append(demo_ss)

    if have_wb:
        demo_wb = ROOT.TGraph(); demo_wb.SetLineColor(ROOT.kGray+1); demo_wb.SetLineStyle(3); demo_wb.SetLineWidth(3)
        leg2.AddEntry(demo_wb, f"Weinberg {scale_label} up/down lines", "l")
        keep.append(demo_wb)

    leg2.Draw()

    leg_info2 = ROOT.TLegend(0.15, 0.88, 0.45, 0.94)
    leg_info2.SetBorderSize(0); leg_info2.SetFillStyle(0); leg_info2.SetTextFont(42); leg_info2.SetTextSize(0.04)
    leg_info2.AddEntry(0, f"{era}, {flavour}", "")
    leg_info2.Draw()
    keep.append(leg_info2)

    c2._keepalive = keep + [leg2, unity2, hframe2]

    # If you want to save these too, uncomment below:
    # full_base_lines = os.path.join(outdir, f"{scale_key}_bands_plots_" + f"{era}_{flavour}_SR-{sr}_Lines")
    # for ext in ("png", "pdf"):
    #     c2.SaveAs(f"{full_base_lines}.{ext}")

    c2.Close()

# ----------------- CLI -----------------

def main():
    ap = argparse.ArgumentParser(description="Plot renormalization/factorization scale variation bands vs mass using ROOT.")
    ap.add_argument("--base-dir", default=DEFAULT_BASE, help="Base directory before {era}/{SR}")
    ap.add_argument("--eras", nargs="+", default=DEFAULT_ERAS, help="Eras to include")
    ap.add_argument("--flavours", nargs="+", default=DEFAULT_FLAVOURS, help="Flavours to include")
    ap.add_argument("--masses", nargs="+", type=int, default=DEFAULT_MASSES, help="Mass points")
    ap.add_argument("--scale-types", nargs="+", default=DEFAULT_SCALE_TYPES,
                    choices=DEFAULT_SCALE_TYPES, help="Scale variations to plot")
    ap.add_argument("--sr", default=None, help="SR subdirectory name; if omitted, auto-detect per era")
    ap.add_argument("--outdir", default="scale_bands_plots", help="Output directory for plots")
    ap.add_argument("--print-keys", action="store_true", help="Print all object paths found in each ROOT file")
    args = ap.parse_args()

    for scale_key in args.scale_types:
        for era in args.eras:
            sr_to_use = args.sr or autodetect_sr(args.base_dir, era, args.flavours[0], args.masses)
            if sr_to_use is None:
                print(f"[WARN] Could not auto-detect SR under {args.base_dir}/{era}. Use --sr to set it explicitly.")
                continue
            print(f"[INFO] Scale {scale_key}, Era {era}: using SR = {sr_to_use}")
            for flav in args.flavours:
                draw_one_plot(args.base_dir, era, flav, args.masses, sr_to_use,
                              args.outdir, print_all_keys=args.print_keys, scale_key=scale_key)

if __name__ == "__main__":
    main()
