#!/usr/bin/env python3
import sys
import os
import math
import re
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.PyConfig.IgnoreCommandLineOptions = True

# -------------------------
# Fixed grid order
# -------------------------
ERAS     = ["2016preVFP", "2016postVFP", "2017", "2018"]
FLAVOURS = ["EE", "MuMu", "EMu"]
SRS      = ["sr1", "sr2", "sr3"]

# -------------------------
# Signals and mass maps
# -------------------------
SIGNAL_PROCS = ["signalSSWW", "signalDYVBF"]

# DYVBF: always 3 bins per SR (bins 4-6)
SR_MASSES_DYVBF = {
    "sr1": [500, 1000, 2000],
    "sr2": [500, 1000, 2000],
    "sr3": [100,  500, 1000],
}

# SSWW: only sr2 and sr3 (bins 1-3)
SR_MASSES_SSWW = {
    "sr2": [500, 1000, 10000],
    "sr3": [500, 1000, 10000],
}

# -------------------------
# Style
# -------------------------
def setTDRStyle():
    tdr = ROOT.TStyle("tdrStyle","Style for P-TDR")
    tdr.SetPaintTextFormat("4.4f")
    tdr.SetCanvasBorderMode(0)
    tdr.SetCanvasColor(ROOT.kWhite)
    tdr.SetCanvasDefH(1200)
    tdr.SetCanvasDefW(1600)
    tdr.SetPadBorderMode(0)
    tdr.SetPadColor(ROOT.kWhite)
    tdr.SetHistLineColor(1)
    tdr.SetMarkerStyle(20)
    tdr.SetOptFile(0)
    tdr.SetOptStat(0)
    tdr.SetPadTopMargin(0.05)
    tdr.SetPadBottomMargin(0.13)
    tdr.SetPadLeftMargin(0.16)
    tdr.SetPadRightMargin(0.06)
    tdr.SetOptTitle(0)
    tdr.SetTitleFont(42)
    tdr.SetTitleSize(0.05)
    tdr.SetLabelFont(42, "XYZ")
    tdr.SetLabelSize(0.05, "XYZ")
    tdr.SetTitleFont(42, "XYZ")
    tdr.SetTitleSize(0.06, "XYZ")
    tdr.SetTitleXOffset(0.9)
    tdr.SetTitleYOffset(1.25)
    tdr.SetPadTickX(1)
    tdr.SetPadTickY(1)
    tdr.cd()
    ROOT.gROOT.ForceStyle()
    ROOT.gStyle.SetCanvasPreferGL(False)

# -------------------------
# Helpers
# -------------------------
def path_for(base_dir, analysis_version, era, sr, mass, flavour):
    return os.path.join(base_dir, analysis_version, era, sr, f"M{mass}_{flavour}_card_input.root")

def file_ok(f):
    return bool(f) and (not f.IsZombie())

def safe_integral(h):
    if not h:
        return 0.0
    v = float(h.Integral())
    return v if math.isfinite(v) else 0.0

def norm_or_zero(num, den):
    return (num/den) if den != 0.0 else 1.0

# -------------------------
# Dynamic systematics scan
# -------------------------
SYST_KEY_RE = re.compile(r"""
    ^(?P<proc>[^;:_]+)
    _CMS_
    (?P<syst>.+?)
    (?:_(?P<era>2016preVFP|2016postVFP|2017|2018))?
    (?:_(?P<sr>sr1|sr2|sr3))?
    (?P<ud>Up|Down)$
""", re.X)

def classify_systematics_from_file(rootfile_path):
    sr_scoped   = set()
    era_scoped  = set()
    global_scoped = set()

    f = ROOT.TFile.Open(rootfile_path, "READ")
    if not file_ok(f):
        raise IOError("Cannot open file for syst discovery: " + rootfile_path)

    seen = {}
    keys = f.GetListOfKeys()
    for i in range(keys.GetSize()):
        key = keys.At(i)
        name = key.GetName()
        m = SYST_KEY_RE.match(name)
        if not m:
            continue
        base = m.group("syst")
        has_era = bool(m.group("era"))
        has_sr  = bool(m.group("sr"))
        tag = ("era_sr" if (has_era and has_sr) else
               "era"    if has_era else
               "sr"     if has_sr  else
               "none")
        seen.setdefault(base, set()).add(tag)
    f.Close()

    for base, tags in seen.items():
        if "era_sr" in tags:
            sr_scoped.add(base)
        elif "era" in tags:
            era_scoped.add(base)
        else:
            global_scoped.add(base)

    return {
        "sr_scoped":  list(sorted(sr_scoped)),
        "era_scoped": list(sorted(era_scoped)),
        "global":     list(sorted(global_scoped)),
    }

def build_var_name(proc, base_syst, era, sr, scope, up_or_down):
    if scope == "sr_scoped":
        return f"{proc}_CMS_{base_syst}_{era}_{sr}{up_or_down}"
    elif scope == "era_scoped":
        return f"{proc}_CMS_{base_syst}_{era}{up_or_down}"
    else:
        return f"{proc}_CMS_{base_syst}{up_or_down}"

def find_probe_file(base_dir, analysis_version):
    print("[probe] searching for a file to discover systematics...")
    search = [
        ("signalDYVBF", SR_MASSES_DYVBF),
        ("signalSSWW",  SR_MASSES_SSWW),
    ]
    for proc_name, mass_map in search:
        for era in ERAS:
            for sr in SRS:
                masses = mass_map.get(sr, [])
                for flavour in FLAVOURS:
                    for m in masses:
                        p = path_for(base_dir, analysis_version, era, sr, m, flavour)
                        if os.path.exists(p):
                            print(f"[probe] found: {p}")
                            return p
    return None

def coerce_variation_to_nominal(v_var, v_nom, eps_abs=1e-12, eps_rel=1e-6):
    # If the variation integral is non-finite, zero, or tiny vs nominal, treat as nominal
    if not math.isfinite(v_var):
        return v_nom
    if v_var <= eps_abs:
        return v_nom
    if v_nom > 0.0 and (v_var / v_nom) < eps_rel:
        return v_nom
    return v_var

# -------------------------
# Ratios per pad (6 bins)
# -------------------------
def compute_signal_ratios_for_pad(base_dir, analysis_version, era, flavour, sr, base_syst, scope):
    up_vals = []
    dn_vals = []

    ssww_masses = SR_MASSES_SSWW.get(sr, [])
    for m in ssww_masses:
        fpath = path_for(base_dir, analysis_version, era, sr, m, flavour)
        f = ROOT.TFile.Open(fpath, "READ")
        if not file_ok(f):
            up_vals.append(1.0); dn_vals.append(1.0)
            continue

        h_nom = f.Get("signalSSWW")
        name_up = build_var_name("signalSSWW", base_syst, era, sr, scope, "Up")
        name_dn = build_var_name("signalSSWW", base_syst, era, sr, scope, "Down")
        h_up = f.Get(name_up) if h_nom else None
        h_dn = f.Get(name_dn) if h_nom else None

        v_nom = safe_integral(h_nom)

        if v_nom == 0.0:
            up_vals.append(1.0)
            dn_vals.append(1.0)
            f.Close()
            continue


        v_up  = safe_integral(h_up) if h_up else v_nom
        v_dn  = safe_integral(h_dn) if h_dn else v_nom

        print(f"[DEBUG] SSWW {era} {flavour} {sr} mass={m} syst={base_syst}")
        print(f"  v_nom={v_nom:.6f}, v_up={v_up:.6f}, v_dn={v_dn:.6f}")
        print(f"  h_nom={bool(h_nom)}, h_up={bool(h_up)}, h_dn={bool(h_dn)}")
        
        # map sentinel to nominal
        if abs(v_up - 0.001) < 1e-12: v_up = v_nom
        if abs(v_dn - 0.001) < 1e-12: v_dn = v_nom

        # Treat very small numbers as sentinel
        if v_up < 1e-4:  # or maybe even 1e-6 if you want stricter
            v_up = v_nom
        if v_dn < 1e-4:
            v_dn = v_nom
        
        # NEW: coerce effectively-empty variations back to nominal
        v_up = coerce_variation_to_nominal(v_up, v_nom)
        v_dn = coerce_variation_to_nominal(v_dn, v_nom)
        
        up_vals.append(norm_or_zero(v_up, v_nom))
        dn_vals.append(norm_or_zero(v_dn, v_nom))
        

        f.Close()

    dyvbf_masses = SR_MASSES_DYVBF.get(sr, [])
    for m in dyvbf_masses:
        fpath = path_for(base_dir, analysis_version, era, sr, m, flavour)
        f = ROOT.TFile.Open(fpath, "READ")
        if not file_ok(f):
            up_vals.append(1.0); dn_vals.append(1.0)
            continue

        h_nom = f.Get("signalDYVBF")
        name_up = build_var_name("signalDYVBF", base_syst, era, sr, scope, "Up")
        name_dn = build_var_name("signalDYVBF", base_syst, era, sr, scope, "Down")
        h_up = f.Get(name_up) if h_nom else None
        h_dn = f.Get(name_dn) if h_nom else None

        v_nom = safe_integral(h_nom)
        if v_nom == 0.0:
            up_vals.append(1.0)
            dn_vals.append(1.0)
            f.Close()
            continue

        v_up  = safe_integral(h_up) if h_up else v_nom
        v_dn  = safe_integral(h_dn) if h_dn else v_nom

        print(f"[DEBUG] DY {era} {flavour} {sr} mass={m} syst={base_syst}")
        print(f"  v_nom={v_nom:.6f}, v_up={v_up:.6f}, v_dn={v_dn:.6f}")
        print(f"  h_nom={bool(h_nom)}, h_up={bool(h_up)}, h_dn={bool(h_dn)}")
        
        # map sentinel to nominal
        if abs(v_up - 0.001) < 1e-12: v_up = v_nom
        if abs(v_dn - 0.001) < 1e-12: v_dn = v_nom

        # Treat very small numbers as sentinel
        if v_up < 1e-4:  # or maybe even 1e-6 if you want stricter
            v_up = v_nom
        if v_dn < 1e-4:
            v_dn = v_nom
        
        print(f"[DEBUG] DY2 {era} {flavour} {sr} mass={m} syst={base_syst}")
        print(f"  v_nom={v_nom:.6f}, v_up={v_up:.6f}, v_dn={v_dn:.6f}")
        print(f"  h_nom={bool(h_nom)}, h_up={bool(h_up)}, h_dn={bool(h_dn)}")

        # NEW: coerce effectively-empty variations back to nominal
        v_up = coerce_variation_to_nominal(v_up, v_nom)
        v_dn = coerce_variation_to_nominal(v_dn, v_nom)

        print(f"[DEBUG] DY3 {era} {flavour} {sr} mass={m} syst={base_syst}")
        print(f"  v_nom={v_nom:.6f}, v_up={v_up:.6f}, v_dn={v_dn:.6f}")
        print(f"  h_nom={bool(h_nom)}, h_up={bool(h_up)}, h_dn={bool(h_dn)}")

        
        up_vals.append(norm_or_zero(v_up, v_nom))
        dn_vals.append(norm_or_zero(v_dn, v_nom))
        
        f.Close()

    if len(ssww_masses) == 0:
        up_vals = [1.0, 1.0, 1.0] + up_vals
        dn_vals = [1.0, 1.0, 1.0] + dn_vals

    while len(up_vals) < 6:  up_vals.append(1.0)
    while len(dn_vals) < 6:  dn_vals.append(1.0)

    return up_vals, dn_vals



# -------------------------
# Drawing
# -------------------------
KEEP = []  # keep strong refs so PyROOT does not GC drawables

def set_band_fill_pdf_safe(graph):
    graph.SetFillColor(ROOT.kGray)
    graph.SetFillStyle(1001)
    graph.SetLineColor(ROOT.kGray+2)

def draw_panel(year, flavour, sr, syst_label, up_vals, dn_vals, ymin, ymax,
               ssww_masses, dyvbf_masses, draw_legend=False):

    labels = [f"SSWW {m}" for m in (ssww_masses if ssww_masses else [None, None, None])]
    labels = [lab if not str(lab).endswith("None") else "SSWW -" for lab in labels] if labels else []
    if ssww_masses == []:
        labels = ["SSWW -", "SSWW -", "SSWW -"]
    labels += [f"DYVBF {m}" for m in dyvbf_masses]
    nbins = len(labels)

    hbase = ROOT.TH1F(f"hbase_{year}_{flavour}_{sr}_{syst_label}", "", nbins, 0.5, nbins+0.5)
    for i, lab in enumerate(labels, start=1):
        hbase.SetBinContent(i, 1.0)
        hbase.GetXaxis().SetBinLabel(i, lab)
    hbase.GetYaxis().SetTitle("Syst / Nominal")
    hbase.SetMinimum(ymin)
    hbase.SetMaximum(ymax)
    hbase.SetLineColor(ROOT.kBlack)
    hbase.SetLineStyle(2)

    gband = ROOT.TGraphAsymmErrors(nbins)
    for i in range(nbins):
        y_up = up_vals[i]
        y_dn = dn_vals[i]
        y_mid = 0.5*(y_up + y_dn)
        err_up = max(0.0, y_up - y_mid)
        err_dn = max(0.0, y_mid - y_dn)
        gband.SetPoint(i, i+1, y_mid)
        gband.SetPointError(i, 0.45, 0.45, err_dn, err_up)
    set_band_fill_pdf_safe(gband)

    gup = ROOT.TGraph(nbins)
    gdn = ROOT.TGraph(nbins)
    for i in range(nbins):
        gup.SetPoint(i, i+1, up_vals[i])
        gdn.SetPoint(i, i+1, dn_vals[i])
    gup.SetLineColor(ROOT.kBlue+1); gup.SetMarkerColor(ROOT.kBlue+1); gup.SetMarkerStyle(20)
    gdn.SetLineColor(ROOT.kRed+1);  gdn.SetMarkerColor(ROOT.kRed+1);  gdn.SetMarkerStyle(20)

    # draw frame and a y=1 reference line
    hbase.Draw("HIST")
    ref = ROOT.TLine(0.5, 1.0, nbins + 0.5, 1.0)
    ref.SetLineStyle(2)
    ref.SetLineColor(ROOT.kBlack)
    ref.SetLineWidth(2)
    ref.Draw("SAME")

    # band and lines
    gband.Draw("E3 SAME")
    gdn.SetLineWidth(2); gdn.SetMarkerSize(1.2)
    gup.SetLineWidth(2); gup.SetMarkerSize(1.2)
    gdn.Draw("LP SAME")
    gup.Draw("LP SAME")

    ref.Draw("SAME")
    # axes on top and force paint
    hbase.Draw("AXIS SAME")
    ROOT.gPad.RedrawAxis()
    ROOT.gPad.Modified(); ROOT.gPad.Update()

    # pad label
    pt = ROOT.TLatex()
    pt.SetNDC(True)
    pt.SetTextFont(42)
    pt.SetTextSize(0.05)
    pt.DrawLatex(0.2, 0.90, f"{year} {flavour} {sr.upper()}")

    # optional legend
    if draw_legend:
        leg = ROOT.TLegend(0.55, 0.72, 0.90, 0.90)
        leg.SetBorderSize(0)
        leg.SetFillStyle(0)
        leg.AddEntry(gband, "Band (Down..Up)", "f")
        leg.AddEntry(gdn,   f"{syst_label} Down", "lp")
        leg.AddEntry(gup,   f"{syst_label} Up", "lp")
        leg.Draw()
        KEEP.append(leg)

    # keep strong refs so objects survive until SaveAs
    KEEP.extend([hbase, ref, gband, gup, gdn, pt])

def plot_canvas_for_sr(syst_base, scope, sr, ratios_map, outdir):

    all_vals = []
    for up_vals, dn_vals in ratios_map.values():
        all_vals.extend(up_vals)
        all_vals.extend(dn_vals)
    if all_vals:
        global_min = min(all_vals)
        global_max = max(all_vals)
        ymin_global = 0.95 * global_min
        ymax_global = 1.05 * global_max
        if abs(ymax_global - ymin_global) < 1e-9:
            ymin_global, ymax_global = 0.8, 1.2
    else:
        ymin_global, ymax_global = 0.8, 1.2

    if abs(ymax_global - ymin_global) < 1e-9:
        ymin_global, ymax_global = 0.9, 1.1

    c = ROOT.TCanvas(f"c_{syst_base}_{sr}", f"{syst_base} / {sr}", 1600, 1200)
    c.Divide(3, 4)

    ssww_masses  = SR_MASSES_SSWW.get(sr, [])
    dyvbf_masses = SR_MASSES_DYVBF.get(sr, [])

    pad_index = 1
    for year in ERAS:
        for flav in FLAVOURS:
            c.cd(pad_index)
            up_vals, dn_vals = ratios_map[(year, flav)]
            draw_panel(year, flav, sr, syst_base, up_vals, dn_vals,
                       ymin_global, ymax_global, ssww_masses, dyvbf_masses,
                       draw_legend=(pad_index==1))
            pad_index += 1

    os.makedirs(outdir, exist_ok=True)
    base = os.path.join(outdir, f"SIGNALS_{syst_base}_{scope}_{sr}_MultiYearGrid")
    print(f"[save] {base}.pdf/png")
    c.Modified(); c.Update()
    c.SaveAs(base + ".pdf")
    c.SaveAs(base + ".png")

# -------------------------
# Main
# -------------------------
def main(analysis_version):
    setTDRStyle()
    base_dir = "/data9/Users/HNL_public/SUS-24-014/LimitInputs"

    # find a real file to classify systematics from
    probe = find_probe_file(base_dir, analysis_version)
    if not probe:
        print("[warn] No probe file found - will use a small default set of systematics.")
        classes = {
            "sr_scoped":  ["res_j_sr1", "scale_j_sr1", "cf_stat_sr1", "fake_stat_sr1", "fake_highpt_sr1"],
            "era_scoped": ["scale_e", "scale_m", "res_e", "res_m", "scale_met", "l1_ecal_prefiring"],
            "global":     ["btag_hf_corr", "btag_lf_corr", "pileup_13TeV", "eff_e_id", "eff_m_id"],
        }
    else:
        classes = classify_systematics_from_file(probe)
        print("[syst] SR-scoped :", classes["sr_scoped"])
        print("[syst] Era-scoped:", classes["era_scoped"])
        print("[syst] Global   :", classes["global"])

        # if everything is empty, provide defaults
        if not (classes["sr_scoped"] or classes["era_scoped"] or classes["global"]):
            print("[warn] No systematics discovered - falling back to defaults.")
            classes = {
                "sr_scoped":  ["res_j_sr1", "scale_j_sr1", "cf_stat_sr1", "fake_stat_sr1", "fake_highpt_sr1"],
                "era_scoped": ["scale_e", "scale_m", "res_e", "res_m", "scale_met", "l1_ecal_prefiring"],
                "global":     ["btag_hf_corr", "btag_lf_corr", "pileup_13TeV", "eff_e_id", "eff_m_id"],
            }

    # loop over discovered (or default) systematics
    for syst_base in classes["sr_scoped"] + classes["era_scoped"] + classes["global"]:
        scope = ("sr_scoped" if syst_base in classes["sr_scoped"]
                 else "era_scoped" if syst_base in classes["era_scoped"]
                 else "global")
        print(f"[run] syst={syst_base:>24s}  scope={scope}")

        for sr in SRS:
            ratios_map = {}
            for year in ERAS:
                for flav in FLAVOURS:
                    updn = compute_signal_ratios_for_pad(
                        base_dir, analysis_version, year, flav, sr, syst_base, scope
                    )
                    ratios_map[(year, flav)] = updn

            outdir = f"Plots_Onebinned/{analysis_version}/SIGNALS/{syst_base}"
            plot_canvas_for_sr(syst_base, scope, sr, ratios_map, outdir)

if __name__ == "__main__":
    # Example:
    # main("ANv5_BDTV2to4_HNL_ULIDv2_WZ_amcatnlo_V3_Strict_10_Bin_RunSyst_Decorr_JetDecorr")
    main("ANv5_BDTV3_AltSR1_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr")
