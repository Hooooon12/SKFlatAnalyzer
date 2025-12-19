#!/usr/bin/env python3
import os
import argparse
import math
import re
import ROOT

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)

# Fixed eras and flavours
ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]
FLAVOURS = ["EE", "MuMu", "EMu"]

ERA_TOKENS = ["2016preVFP", "2016postVFP", "2017", "2018"]

def match_syst_key(base, keys, era):
    """
    Given a base systematic name (e.g. 'eff_m_reco_syst') and list of keys in the file,
    return the concrete systematic core string that matches this era.
    """
    # Try era-specific first
    for tok in ERA_TOKENS:
        if era.endswith(tok):
            candidate = f"{base}_{tok}"
            if any(k.startswith(candidate) for k in keys):
                return candidate
    # Fallback: plain base
    if any(k.startswith(base) for k in keys):
        return base
    return None


# Default components in order
COMPONENTS_DEFAULT = ["fake", "cf", "zg", "wz","zz","ww", "mc_others", "data_obs", "signalSSWW", "signalDYVBF"]

# Colors for backgrounds
COLORS = {
    "fake": ROOT.kAzure - 9,
    "cf": ROOT.kYellow,
    "zg": ROOT.kOrange - 3,
    "wz": ROOT.kSpring - 1,
    "zz": ROOT.kRed,
    "ww": ROOT.kMagenta - 4, 
    "mc_others": ROOT.kGray + 2,
  
}

SIG1_LINE_COLOR = ROOT.kRed   # first signal (from --file-pattern)
SIG2_LINE_COLOR = ROOT.kBlue  # second signal (from --file-pattern-sig2)

# Sentinel values for "no effect" in syst shapes
SENTINEL_EPS = 1e-9
SENTINEL_VAL = 0.001

# Try to apply tdrstyle if available
try:
    import tdrstyle
    tdrstyle.setTDRStyle()
except Exception:
    pass

def build_path(base, era, subdir, file_pattern, flavour):
    return os.path.join(base, era, subdir, file_pattern.format(flavour=flavour))

def open_root(file_path):
    f = ROOT.TFile.Open(file_path, "READ")
    if not f or f.IsZombie():
        raise IOError("Cannot open ROOT file: " + file_path)
    return f

def get_hist_maybe(f, hname):
    h = f.Get(hname)
    if not h:
        return None
    h = h.Clone()
    h.SetDirectory(0)
    return h

def make_zero_like(template, name):
    h = ROOT.TH1D(name, "", template.GetNbinsX(),
                  template.GetXaxis().GetXmin(), template.GetXaxis().GetXmax())
    h.Sumw2()
    h.SetDirectory(0) 
    return h

def sanity_print_combined_delta(h_nom_all, build_var_hist_fn, skey, b0, b1):
    h_up_all   = build_var_hist_fn(skey, "Up")
    h_down_all = build_var_hist_fn(skey, "Down")
    if not (h_up_all and h_down_all and h_nom_all):
        print(f"[SANITY] Missing histograms for {skey}")
        return
    print(f"\n[SANITY] Combined Delta for {skey} (bins {b0}..{b1})")
    for i in range(b0, b1 + 1):
        N  = h_nom_all.GetBinContent(i)
        u  = h_up_all.GetBinContent(i)
        d  = h_down_all.GetBinContent(i)
        dlt = max(abs(u - N), abs(d - N))
        pct = 100.0 * dlt / N if N > 0 else 0.0
        print(f"  bin {i:>2d}: N={N:.2f}  Delta={dlt:.2f}  ({pct:.2f}%)")

def pad_histogram(h, target_bins):
    # Pad a histogram to target_bins with zeros (uniform 0.5..N+0.5 axis)
    if h.GetNbinsX() == target_bins:
        return h
    new_h = ROOT.TH1D(h.GetName() + "_pad", h.GetTitle(),
                      target_bins, 0.5, target_bins + 0.5)
    new_h.Sumw2()
    new_h.SetDirectory(0) 
    for i in range(1, min(h.GetNbinsX(), target_bins) + 1):
        new_h.SetBinContent(i, h.GetBinContent(i))
        new_h.SetBinError(i, h.GetBinError(i))
    return new_h

def concat_hists_with_labels(h_sr, h_cr1, h_cr2, h_cr3, name):
    # Concatenate SR/CR histos and set bin labels: SR 1.., CR1 1.., CR2 1.., CR3 1..
    nbins_total = h_sr.GetNbinsX() + h_cr1.GetNbinsX() + h_cr2.GetNbinsX() + h_cr3.GetNbinsX()
    h_out = ROOT.TH1D(name, "", nbins_total, 0.5, nbins_total + 0.5)
    h_out.Sumw2()

    def copy_bins(src, start_bin, tag):
        for i in range(1, src.GetNbinsX() + 1):
            h_out.SetBinContent(start_bin, src.GetBinContent(i))
            h_out.SetBinError(start_bin, src.GetBinError(i))
            h_out.GetXaxis().SetBinLabel(start_bin, "{} {}".format(tag, i))
            start_bin += 1
        return start_bin

    next_bin = 1
    next_bin = copy_bins(h_sr,  next_bin, "SR")
    next_bin = copy_bins(h_cr1, next_bin, "CR1")
    next_bin = copy_bins(h_cr2, next_bin, "CR2")
    copy_bins(h_cr3, next_bin, "CR3")
    return h_out

def add_hists(a, b):
    if a is None:
        out = b.Clone()
        out.SetDirectory(0)
        return out
    out = a.Clone()
    out.Add(b)
    out.SetDirectory(0)
    return out

def nbins_fallback(file_obj):
    h = get_hist_maybe(file_obj, "data_obs")
    if h:
        return h.GetNbinsX()
    for cand in COMPONENTS_DEFAULT:
        h = get_hist_maybe(file_obj, cand)
        if h:
            return h.GetNbinsX()
    return 0

def ensure_dir(outf, name):
    d = outf.Get(name)
    if not d:
        d = outf.mkdir(name)
    return d

def parse_mass_from_pattern(file_pattern):
    base = os.path.basename(file_pattern)
    for part in base.split("_"):
        if part.startswith("M") and part[1:].isdigit():
            return part[1:]
    return "?"

def is_sentinel_value(x):
    """Return True if x is effectively a 'no-effect' sentinel."""
    return (abs(x) < SENTINEL_EPS) or (abs(x - SENTINEL_VAL) < 1e-9)

def list_syst_keys_from_file(tf, comp_prefix="prompt_inc"):
    """
    Scan a file and discover unique systematic keys that appear as:
      '<comp_prefix>_CMS_<syst_key>Up' / '...Down'
    We only scan one component to avoid duplicates; keys are shared across comps.
    """
    syst = set()
    if not tf:
        return []
    keys = tf.GetListOfKeys()
    if not keys:
        return []
    for i in range(keys.GetSize()):
        k = keys.At(i)
        name = k.GetName()
        if not name.startswith(f"{comp_prefix}_CMS_"):
            continue
        if name.endswith("Up") or name.endswith("Down"):
            core = re.sub(r"(Up|Down)$", "", name)
            core = core.replace(f"{comp_prefix}_CMS_", "")
            syst.add(core)
    return sorted(syst)

def build_grand_bkg_for_syst(base, eras, flavours, sr_subdir, cr1_subdir, cr2_subdir, cr3_subdir,
                             file_pattern, components_bkg, max_sr, max_cr1, max_cr2, max_cr3,
                             syst_key, up_or_down):
    """
    Build the grand, concatenated background-only histogram for a single syst family and direction.
    Resolves era-specific systematics (e.g. eff_m_reco_syst_2016preVFP, eff_m_reco_syst_2017) automatically.
    """
    grand_var = None

    def _fetch_var_hist(f, comp, core, which_dir):
        """
        Try several naming patterns for shape variations:
        <comp>_CMS_<core><Up/Down>  OR  <comp>_<core><Up/Down>
        Return the first non-sentinel histogram found, else None.
        """
        for patt in (f"{comp}_CMS_{core}{which_dir}", f"{comp}_{core}{which_dir}"):
            h = get_hist_maybe(f, patt)
            if h:
                if not is_sentinel_value(float(h.Integral())):
                    return h
        return None

    def choose_var_or_nom(f, comp, tmpl, syst_base, era, which_dir):
        """
        Try common naming patterns for this era; if none found (or sentinel), use nominal.
        """
        candidates = [
            f"{comp}_CMS_{syst_base}_{era}{which_dir}",
            f"{comp}_{syst_base}_{era}{which_dir}",
            f"{comp}_CMS_{syst_base}{which_dir}",
            f"{comp}_{syst_base}{which_dir}",
        ]
        for patt in candidates:
            h = get_hist_maybe(f, patt)
            if h and not is_sentinel_value(float(h.Integral())):
                return h
            # fallback to nominal
            h_nom = get_hist_maybe(f, comp)
            if h_nom:
                return h_nom
        return make_zero_like(tmpl, f"{comp}_zero_fallback")

    
    for era in eras:
        for flav in flavours:
            # open nominal files
            sr_path  = build_path(base, era, sr_subdir,  file_pattern, flav)
            cr1_path = build_path(base, era, cr1_subdir, file_pattern, flav)
            cr2_path = build_path(base, era, cr2_subdir, file_pattern, flav)
            cr3_path = build_path(base, era, cr3_subdir, file_pattern, flav)
            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception:
                continue

            # templates for binning
            tmpl_sr  = get_hist_maybe(f_sr,  "data_obs") or get_hist_maybe(f_sr,  components_bkg[0])
            tmpl_cr1 = get_hist_maybe(f_cr1, "data_obs") or get_hist_maybe(f_cr1, components_bkg[0])
            tmpl_cr2 = get_hist_maybe(f_cr2, "data_obs") or get_hist_maybe(f_cr2, components_bkg[0])
            tmpl_cr3 = get_hist_maybe(f_cr3, "data_obs") or get_hist_maybe(f_cr3, components_bkg[0])
            if not (tmpl_sr and tmpl_cr1 and tmpl_cr2 and tmpl_cr3):
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue


            # === resolve the right syst core for this era ===
            #keys_sr = [k.GetName() for k in f_sr.GetListOfKeys()]
            #core = match_syst_key(syst_key, keys_sr, era)
            #if not core:
            #    f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
            #    continue

            # Sum backgrounds per region
            sum_sr = sum_cr1 = sum_cr2 = sum_cr3 = None
            for comp in components_bkg:
                hs  = choose_var_or_nom(f_sr,  comp, tmpl_sr,  syst_key, era, up_or_down)
                hc1 = choose_var_or_nom(f_cr1, comp, tmpl_cr1, syst_key, era, up_or_down)
                hc2 = choose_var_or_nom(f_cr2, comp, tmpl_cr2, syst_key, era, up_or_down)
                hc3 = choose_var_or_nom(f_cr3, comp, tmpl_cr3, syst_key, era, up_or_down)
                
                if hs.GetNbinsX()  != max_sr:  hs  = pad_histogram(hs,  max_sr)
                if hc1.GetNbinsX() != max_cr1: hc1 = pad_histogram(hc1, max_cr1)
                if hc2.GetNbinsX() != max_cr2: hc2 = pad_histogram(hc2, max_cr2)
                if hc3.GetNbinsX() != max_cr3: hc3 = pad_histogram(hc3, max_cr3)

                sum_sr  = add_hists(sum_sr,  hs)
                sum_cr1 = add_hists(sum_cr1, hc1)
                sum_cr2 = add_hists(sum_cr2, hc2)
                sum_cr3 = add_hists(sum_cr3, hc3)

            h_concat = concat_hists_with_labels(sum_sr, sum_cr1, sum_cr2, sum_cr3,
                                                f"bkg_{syst_key}{up_or_down}_{era}_{flav}")
            if grand_var is None:
                grand_var = h_concat.Clone(f"bkg_{syst_key}{up_or_down}_grand")
                grand_var.SetDirectory(0)
            else:
                grand_var.Add(h_concat)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    return grand_var



def make_bkg_unc_band_abs(h_bkg_total, syst_keys, build_var_hist_fn):
    """
    Build a TGraphAsymmErrors band for total background:
      per-bin error = sqrt( stat^2 + sum_over_sources( max(|Up-Nom|,|Down-Nom|)^2 ) ).
    """
    n = h_bkg_total.GetNbinsX()

    # Pre-build variations once per source
    var_up   = {k: build_var_hist_fn(k, "Up")   for k in syst_keys}
    var_down = {k: build_var_hist_fn(k, "Down") for k in syst_keys}

    g = ROOT.TGraphAsymmErrors(n)
    for i in range(1, n + 1):
        x  = h_bkg_total.GetBinCenter(i)
        ex = 0.5
        y  = h_bkg_total.GetBinContent(i)
        stat = h_bkg_total.GetBinError(i)

        syst2_sum = 0.0
        for k in syst_keys:
            up  = var_up[k].GetBinContent(i)   if var_up[k]   else y
            dn  = var_down[k].GetBinContent(i) if var_down[k] else y
            delta = max(abs(up - y), abs(dn - y))
            syst2_sum += delta * delta

        etot = math.sqrt(stat*stat + syst2_sum)

        g.SetPoint(i - 1, x, y)
        g.SetPointError(i - 1, ex, ex, etot, etot)

    g.SetFillStyle(3144)
    try:
        g.SetFillColorAlpha(ROOT.kGray + 1, 0.5)
    except Exception:
        g.SetFillColor(ROOT.kGray + 1)
    g.SetLineColor(0)
    return g

def make_bkg_unc_band_ratio(h_bkg_total, band_abs):
    """
    Convert an absolute-error band to a ratio band centered at 1:
      ey_ratio = ey_abs / y_nom (if y_nom>0).
    """
    n = h_bkg_total.GetNbinsX()
    g = ROOT.TGraphAsymmErrors(n)
    for i in range(1, n + 1):
        x  = h_bkg_total.GetBinCenter(i)
        ex = 0.5
        y  = h_bkg_total.GetBinContent(i)
        ey = band_abs.GetErrorYhigh(i - 1)  # symmetric
        eyr = (ey / y) if y > 0 else 0.0
        g.SetPoint(i - 1, x, 1.0)
        g.SetPointError(i - 1, ex, ex, eyr, eyr)

    g.SetFillStyle(3144)
    try:
        g.SetFillColorAlpha(ROOT.kGray + 1, 0.5)
    except Exception:
        g.SetFillColor(ROOT.kGray + 1)
    g.SetLineColor(0)
    return g

def _format_lumi_text(lumi_str):
    """Make 'fb-1' render nicely as 'fb^{-1}' in TLatex if user passed fb-1."""
    if "fb-1" in lumi_str:
        return lumi_str.replace("fb-1", "fb^{-1}")
    return lumi_str


def build_grand_bkg_scaled(base, eras, flavours, sr_subdir, cr1_subdir, cr2_subdir, cr3_subdir,
                           file_pattern, components_bkg, max_sr, max_cr1, max_cr2, max_cr3,
                           scale_map, up_or_down):
    """
    Build background-only grand histogram with simple normalization scaling applied
    to specific components. scale_map is like {'fake': 0.25, 'cf': 0.20}.
    Up applies (1+frac), Down applies max(0, 1-frac).
    """
    grand_var = None

    for era in eras:
        for flav in flavours:
            sr_path  = build_path(base, era, sr_subdir,  file_pattern, flav)
            cr1_path = build_path(base, era, cr1_subdir, file_pattern, flav)
            cr2_path = build_path(base, era, cr2_subdir, file_pattern, flav)
            cr3_path = build_path(base, era, cr3_subdir, file_pattern, flav)
            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception:
                continue

            # templates
            tmpl_sr  = get_hist_maybe(f_sr,  "data_obs") or get_hist_maybe(f_sr,  components_bkg[0])
            tmpl_cr1 = get_hist_maybe(f_cr1, "data_obs") or get_hist_maybe(f_cr1, components_bkg[0])
            tmpl_cr2 = get_hist_maybe(f_cr2, "data_obs") or get_hist_maybe(f_cr2, components_bkg[0])
            tmpl_cr3 = get_hist_maybe(f_cr3, "data_obs") or get_hist_maybe(f_cr3, components_bkg[0])
            if not (tmpl_sr and tmpl_cr1 and tmpl_cr2 and tmpl_cr3):
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue


            # choose scale factors
            def factor_for(comp):
                frac = scale_map.get(comp, 0.0)
                if frac <= 0:
                    return 1.0
                if up_or_down == "Up":
                    return 1.0 + frac
                else:
                    return max(0.0, 1.0 - frac)

            sum_sr = sum_cr1 = sum_cr2 = sum_cr3 = None
            for comp in components_bkg:
                hs  = get_hist_maybe(f_sr,  comp)  or make_zero_like(tmpl_sr,  f"{comp}_sr_zero_norm")
                hc1 = get_hist_maybe(f_cr1, comp)  or make_zero_like(tmpl_cr1, f"{comp}_cr1_zero_norm")
                hc2 = get_hist_maybe(f_cr2, comp)  or make_zero_like(tmpl_cr2, f"{comp}_cr2_zero_norm")
                hc3 = get_hist_maybe(f_cr3, comp)  or make_zero_like(tmpl_cr3, f"{comp}_cr3_zero_norm")

                # pad to global binning
                if hs.GetNbinsX()  != max_sr:  hs  = pad_histogram(hs,  max_sr)
                if hc1.GetNbinsX() != max_cr1: hc1 = pad_histogram(hc1, max_cr1)
                if hc2.GetNbinsX() != max_cr2: hc2 = pad_histogram(hc2, max_cr2)
                if hc3.GetNbinsX() != max_cr3: hc3 = pad_histogram(hc3, max_cr3)

                # apply normalization-only scaling if requested
                fac = factor_for(comp)
                if fac != 1.0:
                    hs.Scale(fac); hc1.Scale(fac); hc2.Scale(fac); hc3.Scale(fac)

                sum_sr  = add_hists(sum_sr,  hs)
                sum_cr1 = add_hists(sum_cr1, hc1)
                sum_cr2 = add_hists(sum_cr2, hc2)
                sum_cr3 = add_hists(sum_cr3, hc3)

            h_concat = concat_hists_with_labels(sum_sr, sum_cr1, sum_cr2, sum_cr3,
                                                f"bkg_normscale_{up_or_down}_{era}_{flav}")
            if grand_var is None:
                grand_var = h_concat.Clone(f"bkg_normscale_{up_or_down}_grand")
                grand_var.SetDirectory(0)
            else:
                grand_var.Add(h_concat)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    return grand_var


def dump_uncertainties_table(h_bkg_total, syst_keys, build_var_hist_fn,
                             start_bin=1, end_bin=None, tsv_path=None, max_sources=None):
    """
    Print per-bin uncertainty breakdown:
      bin, N, stat, each syst (Nom, Up, Down, Delta), and total = sqrt(stat^2 + sum Delta^2).
    If tsv_path is given, also write a TSV file with the same content.
    max_sources: if not None, limit number of syst columns to keep table narrow.
    """
    if h_bkg_total is None:
        print("[WARN] No total background histogram; cannot dump.")
        return

    n = h_bkg_total.GetNbinsX()
    if end_bin is None:
        end_bin = n
    start_bin = max(1, int(start_bin))
    end_bin   = min(n, int(end_bin))

    # Build variation histograms once
    var_up   = {k: build_var_hist_fn(k, "Up")   for k in syst_keys}
    var_down = {k: build_var_hist_fn(k, "Down") for k in syst_keys}

    # Select which systematics to show (optionally truncate for readability)
    show_systs = list(syst_keys)
    if max_sources is not None:
        show_systs = show_systs[:max_sources]

    # Header
    cols = ["bin", "N", "stat"]
    for k in show_systs:
        cols.append(f"{k}_Nom")
        cols.append(f"{k}_Up")
        cols.append(f"{k}_Down")
        cols.append(f"{k}_Delta")
    cols.append("total")
    col_line = "\t".join(cols)
    print("\n=== Uncertainty dump (bins {}..{}) ===".format(start_bin, end_bin))
    print(col_line)

    # Optional TSV write
    ftsv = open(tsv_path, "w") if tsv_path else None
    if ftsv:
        ftsv.write(col_line + "\n")

    # Rows
    for i in range(start_bin, end_bin + 1):
        N = h_bkg_total.GetBinContent(i)
        stat = h_bkg_total.GetBinError(i)
        syst2_sum = 0.0
        values = [str(i), f"{N:.6g}", f"{stat:.6g}"]

        for k in syst_keys:
            h_up   = var_up[k]
            h_down = var_down[k]
            if h_up and h_down:
                up_val = h_up.GetBinContent(i)
                dn_val = h_down.GetBinContent(i)
                delta  = max(abs(up_val - N), abs(dn_val - N))
            else:
                up_val = N
                dn_val = N
                delta  = 0.0


            syst2_sum += delta * delta

            if k in show_systs:
                values.extend([
                    f"{N:.6g}",      # nominal value
                    f"{up_val:.6g}", # up variation
                    f"{dn_val:.6g}", # down variation
                    f"{delta:.6g}"   # delta
                ])

        total = math.sqrt(stat*stat + syst2_sum)
        values.append(f"{total:.6g}")

        line = "\t".join(values)
        print(line)
        if ftsv:
            ftsv.write(line + "\n")

    if ftsv:
        ftsv.close()
        print("[INFO] Wrote TSV:", tsv_path)



def dump_uncertainties_table_old(h_bkg_total, syst_keys, build_var_hist_fn,
                             start_bin=1, end_bin=None, tsv_path=None, max_sources=None):
    """
    Print per-bin uncertainty breakdown:
      bin, N, stat, each syst (Delta_i), and total = sqrt(stat^2 + sum Delta_i^2).
    If tsv_path is given, also write a TSV file with the same content.
    max_sources: if not None, limit number of syst columns to keep table narrow.
    """
    if h_bkg_total is None:
        print("[WARN] No total background histogram; cannot dump.")
        return

    n = h_bkg_total.GetNbinsX()
    if end_bin is None:
        end_bin = n
    start_bin = max(1, int(start_bin))
    end_bin   = min(n, int(end_bin))

    # Build variation histograms once
    var_up   = {k: build_var_hist_fn(k, "Up")   for k in syst_keys}
    var_down = {k: build_var_hist_fn(k, "Down") for k in syst_keys}

    # Select which systematics to show (optionally truncate for readability)
    show_systs = list(syst_keys)
    if max_sources is not None:
        show_systs = show_systs[:max_sources]

    # Header
    cols = ["bin", "N", "stat"]
    cols += [f"Delta[{k}]" for k in show_systs]
    cols += ["total"]
    col_line = "\t".join(cols)
    print("\n=== Uncertainty dump (bins {}..{}) ===".format(start_bin, end_bin))
    print(col_line)

    # Optional TSV write
    ftsv = open(tsv_path, "w") if tsv_path else None
    if ftsv:
        ftsv.write(col_line + "\n")

    # Rows
    for i in range(start_bin, end_bin + 1):
        N = h_bkg_total.GetBinContent(i)
        stat = h_bkg_total.GetBinError(i)
        syst2_sum = 0.0
        deltas = []
        for k in syst_keys:
            up  = var_up[k].GetBinContent(i)   if var_up[k]   else N
            dn  = var_down[k].GetBinContent(i) if var_down[k] else N
            delta = max(abs(up - N), abs(dn - N))
            syst2_sum += delta * delta
            # stash for display only if in show_systs
            if k in show_systs:
                deltas.append(delta)

        total = math.sqrt(stat*stat + syst2_sum)
        row = [str(i), f"{N:.6g}", f"{stat:.6g}"] + [f"{d:.6g}" for d in deltas] + [f"{total:.6g}"]
        line = "\t".join(row)
        print(line)
        if ftsv:
            ftsv.write(line + "\n")

    if ftsv:
        ftsv.close()
        print("[INFO] Wrote TSV:", tsv_path)
        

def make_stack_plot(hists_bkg, h_data, h_sig1, h_sig2, outpath, title, lumi,
                    region_edges, sr_bins, mass1, mass2,
                    g_bkg_band_main=None, g_bkg_band_ratio=None, h_bkg_total_pre=None):
    # Keep references so graphics survive SaveAs
    lines_main = []
    texts_main = []
    keep_misc = []

    # Sum total background (use precomputed one if provided so band & y-range match)
    if h_bkg_total_pre:
        h_bkg_total = h_bkg_total_pre.Clone("bkg_total_for_plot")
        h_bkg_total.SetDirectory(0)
    else:
        h_bkg_total = None
        for _, hb in hists_bkg:
            h_bkg_total = add_hists(h_bkg_total, hb)

    # Prepare and scale signals by powers of two until >= 10% of total bkg
    def prep_sig(hsig, color,scale):
        if not (hsig and h_bkg_total and h_bkg_total.Integral() > 0 and hsig.Integral() > 0):
            return None, 1
        #scale = 1
        #while hsig.Integral() * scale < 0.1 * h_bkg_total.Integral():
        #    scale *= 2
        out = hsig.Clone("signal_draw_" + str(color))
        out.SetDirectory(0)
        out.Scale(scale)
        out.SetLineWidth(3)
        out.SetLineStyle(2)
        out.SetLineColor(color)
        out.SetFillStyle(0)
        return out, scale

    h_sig1_draw, scale1 = prep_sig(h_sig1, SIG1_LINE_COLOR,100)
    h_sig2_draw, scale2 = prep_sig(h_sig2, SIG2_LINE_COLOR,10)

    # Canvas and pads (main wider; legend column; ratio below)
    c = ROOT.TCanvas("c", "c", 2400, 950)
    pad_main  = ROOT.TPad("pad_main",  "pad_main",  0.00, 0.32, 0.84, 1.00)
    pad_ratio = ROOT.TPad("pad_ratio", "pad_ratio", 0.00, 0.00, 0.84, 0.30)
    pad_leg   = ROOT.TPad("pad_leg",   "pad_leg",   0.84, 0.32, 1.00, 1.00)

    for p in (pad_main, pad_leg, pad_ratio):
        p.SetFillStyle(0)

    # Margins
    pad_main.SetRightMargin(0.02)
    pad_main.SetTopMargin( max(0.13, pad_main.GetTopMargin()) )  # a bit more room for labels

    pad_main.SetBottomMargin(0.02)
    pad_main.SetLeftMargin(0.12)   # leave room for y-title

    pad_ratio.SetLeftMargin(0.12)
    pad_ratio.SetRightMargin(0.02)
    pad_ratio.SetTopMargin(0.08)
    pad_ratio.SetBottomMargin(0.32)

    pad_leg.SetLeftMargin(0.02)
    pad_main.Draw(); pad_leg.Draw(); pad_ratio.Draw()

    # MAIN pad
    pad_main.cd()
    pad_main.SetLogy()

    # Stack
    stack = ROOT.THStack("stack", title)
    for label, hb in hists_bkg:
        color = COLORS.get(label, ROOT.kGray + 1)
        hb.SetLineColor(ROOT.kBlack)
        hb.SetFillColor(color)
        hb.SetFillStyle(1001)
        stack.Add(hb, "HIST")

    stack.Draw("HIST")
    stack.GetYaxis().SetTitle("Events/bin")
    stack.GetXaxis().SetLabelSize(0)

    # Y-axis aesthetics
    stack.GetYaxis().SetTitleOffset(0.95)
    stack.GetYaxis().SetTitleSize(0.055)
    stack.GetYaxis().SetLabelSize(0.045)

    # Y range (log-safe)
    ymax_stack = stack.GetMaximum()
    ymax_data  = h_data.GetMaximum() if h_data else 0.0
    ymax_sig1  = h_sig1_draw.GetMaximum() if h_sig1_draw else 0.0
    ymax_sig2  = h_sig2_draw.GetMaximum() if h_sig2_draw else 0.0
    y_max = max(ymax_stack, ymax_data, ymax_sig1, ymax_sig2, 1.0)

    headroom = 100.
    stack.SetMaximum(headroom * y_max)
    stack.SetMinimum(1.0)  # bottom of log frame

    # Draw (stat + syst) background uncertainty band on main
    if g_bkg_band_main:
        g_bkg_band_main.Draw("E2 SAME")
        keep_misc.append(g_bkg_band_main)

    # Draw data & signals
    if h_data:
        h_data.SetMarkerStyle(20)
        h_data.SetMarkerSize(1.0)
        h_data.SetLineColor(ROOT.kBlack)
        h_data.Draw("PE SAME")
    if h_sig1_draw:
        h_sig1_draw.Draw("HIST SAME")

    # CMS and lumi labels
    l = pad_main.GetLeftMargin()
    r = pad_main.GetRightMargin()
    t = pad_main.GetTopMargin()

    cms = ROOT.TLatex()
    cms.SetNDC(True); cms.SetTextFont(61)
    cms.SetTextSize(0.75 * t); cms.SetTextAlign(11)
    cms.DrawLatex(l + 0.01, 1 - t + 0.02, "CMS")

    pre = ROOT.TLatex()
    pre.SetNDC(True); pre.SetTextFont(52)
    pre.SetTextSize(0.76 * 0.75 * t); pre.SetTextAlign(11)
    pre.DrawLatex(l + 0.01, 1 - t + 0.02 - 1.2 * 0.75 * t, "Preliminary")

    lumi_txt = ROOT.TLatex()
    lumi_txt.SetNDC(True); lumi_txt.SetTextFont(42)
    lumi_txt.SetTextSize(0.60 * t); lumi_txt.SetTextAlign(31)  # right, bottom
    x_right = 1 - r
    y_top   = 1 - t + 0.20 * t
    lumi_txt.DrawLatex(x_right, y_top, _format_lumi_text(lumi))

    pad_main.SetTicks(1, 1)

    # ------------------------------------------------------------------
    # Region separators and titles (first edge: masked double-slit)
    # ------------------------------------------------------------------
    pad_main.Update()
    L = pad_main.GetLeftMargin()
    R = pad_main.GetRightMargin()
    B = pad_main.GetBottomMargin()
    T = pad_main.GetTopMargin()
    
    # X axis user range from the stack (0.5 .. N+0.5)
    xmin = stack.GetXaxis().GetXmin()
    xmax = stack.GetXaxis().GetXmax()
    
    # Map user-x (bin coordinates) -> NDC-x
    def x_user_to_ndc(xu):
        if xmax == xmin:
            return 0.5
        return L + (xu - xmin) / (xmax - xmin) * (1.0 - L - R)

    y_ndc_bottom = 0.25
    # put the top at the same vertical level as your region titles

    mask_objs = []
    y_ndc_label = 50000
    if region_edges:
        x_split = region_edges[0] + 0.5
        dx = 0.08
        gap_w = 2.0 * dx
        x_lo = x_split - 0.5 * gap_w
        x_hi = x_split + 0.5 * gap_w

        gap_box = ROOT.TBox(x_lo, y_ndc_bottom, x_hi,y_ndc_label)
        gap_box.SetFillColor(pad_main.GetFillColor())
        gap_box.SetFillStyle(1001)
        gap_box.SetLineColor(pad_main.GetFillColor())
        gap_box.Draw("same")
        mask_objs.append(gap_box)

        for xoff in (-dx, +dx):
            ln = ROOT.TLine(x_split + xoff, y_ndc_bottom, x_split + xoff, y_ndc_label)
            ln.SetLineStyle(1)
            ln.SetLineWidth(3)
            ln.SetLineColor(ROOT.kBlack)
            ln.Draw()
            lines_main.append(ln)

        for edge in region_edges[1:]:
            x = edge + 0.5
            ln = ROOT.TLine(x, y_ndc_bottom, x, y_ndc_label)
            ln.SetLineStyle(9)
            ln.SetLineWidth(3)
            ln.SetLineColor(ROOT.kGray+1)
            ln.Draw()
            lines_main.append(ln)

    
    # Recompute transforms after stack has been drawn
    pad_main.Update()

    t = pad_main.GetTopMargin()
    # Place text within the top margin band; 0.65*t is a good vertical anchor
    xmin = stack.GetXaxis().GetXmin()
    xmax = stack.GetXaxis().GetXmax()
    L = pad_main.GetLeftMargin()
    R = pad_main.GetRightMargin()
    t = pad_main.GetTopMargin()
    
    y_label_ndc = 1.0 - t - 0.2
    text_size_ndc = 0.04      
    
    titles = ["Boosted SR", "BJet CR", "InvMET CR", "WZ CR"]
    starts = [0] + region_edges
    ends   = region_edges + [h_bkg_total.GetNbinsX()]
    last_label_dx = -0.010

    def x_user_to_ndc(xu):
        # Map user x in [xmin, xmax] to NDC, accounting for left/right margins
        if xmax == xmin:
            return 0.5
        return L + (xu - xmin) / (xmax - xmin) * (1.0 - L - R)
    
    
    for idx, (lab, a, b) in enumerate(zip(titles, starts, ends)):
        x_user_center = 0.5 * (a + 1 + b)
        x_ndc = x_user_to_ndc(x_user_center)
        if idx == len(titles) - 1:
            x_ndc += last_label_dx
        tl = ROOT.TLatex()
        tl.SetNDC(True)
        tl.SetTextFont(62)
        tl.SetTextSize(text_size_ndc)

        tl.SetTextAlign(23)
        tl.DrawLatex(x_ndc, y_label_ndc, lab)
        print (f"x_ndc={x_ndc} y_label_ndc = {y_label_ndc} lab = {lab}")
        texts_main.append(tl)

    pad_main._keepalive = getattr(pad_main, "_keepalive", []) \
        + lines_main + texts_main + mask_objs + [cms, pre, lumi_txt] \
        + ([g_bkg_band_main] if g_bkg_band_main else [])
    

    # LEGEND pad
    pad_leg.cd()
    leg = ROOT.TLegend(0.10, 0.08, 0.95, 0.88)
    leg.SetBorderSize(0)
    leg.SetLineWidth(0)
    leg.SetLineColor(ROOT.kWhite)   # not kWhite bare
    leg.SetFillStyle(0)
    leg.SetTextSize(0.055)

    def _add_fill_entry(leg, obj, label):
        e = leg.AddEntry(obj, label, "f")
        e.SetLineWidth(0)          # <- removes the black border around the box
        e.SetLineColor(0)          # <- force transparent border
        e.SetMarkerSize(0)         # (optional) no marker over the box
        return e
    
    def _add_line_entry(leg, obj, label):
        e = leg.AddEntry(obj, label, "l")  # for signals
        # you can also thin these if you like:
        # e.SetLineWidth(3)
        return e
    
    if h_data:
        leg.AddEntry(h_data, "Data", "pe")
        leg.SetLineColor(0) 
    for label, hb in hists_bkg:
        if label == "fake":
            _add_fill_entry(leg, hb, "Nonprompt")
        elif label == "cf":
            _add_fill_entry(leg, hb, "Charge MisID.")
        elif label == "mc_others":
            _add_fill_entry(leg, hb, "others")
        elif label == "ww":
            _add_fill_entry(leg, hb, "W^{#pm}W^{#pm}")
        else:
            _add_fill_entry(leg, hb, label)

    if g_bkg_band_main:
        _add_fill_entry(leg, g_bkg_band_main, "Bkg. unc.(stat #oplus syst)")
        
    if h_sig1_draw:
        lab1 = f"DY+W#gamma M={mass1} GeV" + (f" (x{scale1})" if scale1 != 1 else "")
        _add_line_entry(leg, h_sig1_draw, lab1)

    leg.Draw()

    # RATIO pad
    pad_ratio.cd()
    if h_data and h_bkg_total and h_data.Integral() > 0:
        ratio = h_bkg_total.Clone("ratio_bkg_over_data")
        ratio.Divide(h_data)
        ratio.SetTitle("")
        ratio.GetYaxis().SetRangeUser(0.5,1.5)
        ratio.GetYaxis().SetTitle("#frac{Prediction}{Data}")
        ratio.GetYaxis().SetNdivisions(505)
        ratio.GetYaxis().SetTitleSize(0.10)
        ratio.GetYaxis().SetLabelSize(0.09)
        ratio.GetYaxis().SetTitleOffset(0.55)
        ratio.GetXaxis().SetLabelSize(0.09)
        ratio.GetXaxis().SetTitleSize(0.12)
        ratio.GetXaxis().SetTitleOffset(1.0)
        ratio.GetXaxis().LabelsOption("v")
        
        # Draw ratio band first if available
        if g_bkg_band_ratio:
            ratio.Draw("E")  # create the axes
            g_bkg_band_ratio.Draw("E2 SAME")
            ratio.Draw("E SAME")
            keep_misc.append(g_bkg_band_ratio)
        else:
            ratio.Draw("E")

        one = ROOT.TLine(ratio.GetXaxis().GetXmin(), 1.0, ratio.GetXaxis().GetXmax(), 1.0)
        one.SetLineStyle(2)
        one.Draw()
        pad_ratio.SetTicks(1, 1)
        keep_misc.append(one)
        
    # Save (PNG + PDF)
    os.makedirs(os.path.dirname(outpath), exist_ok=True)

    # Keep misc graphics alive on the canvas object
    c._keepalive = getattr(c, "_keepalive", []) + [obj for obj in keep_misc if obj]

    c.SaveAs(outpath)
    root, ext = os.path.splitext(outpath)
    if ext.lower() != ".pdf":
        c.SaveAs(root + ".pdf")

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--base", required=True)
    ap.add_argument("--sr-subdir", default="sr1")
    ap.add_argument("--cr1-subdir", default="sr1_InvBJet")
    ap.add_argument("--cr2-subdir", default="sr1_InvMET")
    ap.add_argument("--cr3-subdir", default="wz_cr1")
    ap.add_argument("--file-pattern", default="M400_{flavour}_card_input.root")        # backgrounds + signal-1
    ap.add_argument("--file-pattern-sig2", default="M600_{flavour}_card_input.root")    # signal-2 only
    ap.add_argument("--components", nargs="+", default=COMPONENTS_DEFAULT)
    ap.add_argument("--outfile", default="combined_SRCR.root")
    ap.add_argument("--outplot", default="summary_plot_sr1/combined_sr3_all_eras_flavs.png")
    ap.add_argument("--title", default="SR+CR Combined (All Eras & Flavours)")
    ap.add_argument("--lumi", default="137.6 fb-1 (13 TeV)")
    ap.add_argument("--lumi-norm-syst", type=float, default=0.05,
                    help="Relative normalization syst for luminosity (e.g. 0.05 for 5%%). 0 disables.")
    ap.add_argument("--fake-norm-syst", type=float, default=0.25,
                    help="Relative normalization syst for 'fake' (e.g. 0.25 for 25%%). 0 disables.")
    ap.add_argument("--cf-norm-syst", type=float, default=0.2,
                    help="Relative normalization syst for 'cf' (e.g. 0.20 for 20%%). 0 disables.")
    ap.add_argument("--dump-unc", action="store_true",
                    help="Print per-bin uncertainty breakdown after building bands")
    ap.add_argument("--dump-range", default="",
                    help="Optional bin range like '1:40'. Leave empty for all bins.")
    ap.add_argument("--dump-tsv", default="",
                    help="Optional path to write a TSV with the dump.")
    ap.add_argument("--dump-max-sources", type=int, default=0,
                    help="Limit number of syst columns in the table (0 = all).")
    args = ap.parse_args()


    base_tag = os.path.basename(os.path.normpath(args.base))
    base_tag = base_tag.replace("/", "_").replace(":", "_")
    
    out_dir = os.path.dirname(args.outplot) or "summary_plot_sr1"
    os.makedirs(out_dir, exist_ok=True)

    args.outfile = os.path.join(out_dir, f"combined_{args.sr_subdir}_{base_tag}.root")
    args.outplot = os.path.join(out_dir, f"combined_{args.sr_subdir}_{base_tag}_all_eras_flavs.png")


    
    # PASS 1: discover max bins per region and print
    max_sr = max_cr1 = max_cr2 = max_cr3 = 0
    print("\n=== Bin Count Summary per Era / Flavour / Region (pass 1) ===")
    print("{:<12} {:<6} {:>5} {:>5} {:>5} {:>5} | {:>6}".format("Era", "Flav", "SR", "CR1", "CR2", "CR3", "Concat"))
    print("-" * 64)

    for era in ERAS:
        for flav in FLAVOURS:
            sr_path  = build_path(args.base, era, args.sr_subdir,  args.file_pattern, flav)
            cr1_path = build_path(args.base, era, args.cr1_subdir, args.file_pattern, flav)
            cr2_path = build_path(args.base, era, args.cr2_subdir, args.file_pattern, flav)
            cr3_path = build_path(args.base, era, args.cr3_subdir, args.file_pattern, flav)
            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception as e:
                print("{:<12} {:<6} MISSING: {}".format(era, flav, e))
                continue

            sr_bins  = nbins_fallback(f_sr)
            cr1_bins = nbins_fallback(f_cr1)
            cr2_bins = nbins_fallback(f_cr2)
            cr3_bins = nbins_fallback(f_cr3)

            # Special padding quirk

            concat_bins = sr_bins + cr1_bins + cr2_bins + cr3_bins
            print("{:<12} {:<6} {:5d} {:5d} {:5d} {:5d} | {:6d}".format(
                era, flav, sr_bins, cr1_bins, cr2_bins, cr3_bins, concat_bins))

            max_sr  = max(max_sr,  sr_bins)
            max_cr1 = max(max_cr1, cr1_bins)
            max_cr2 = max(max_cr2, cr2_bins)
            max_cr3 = max(max_cr3, cr3_bins)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    total_bins = max_sr + max_cr1 + max_cr2 + max_cr3
    print("[TARGET] SR={} CR1={} CR2={} CR3={} | total={}".format(max_sr, max_cr1, max_cr2, max_cr3, total_bins))

    # Discover available systematic sources by scanning one representative SR file
    syst_keys = []
    for era in ERAS:
        for flav in FLAVOURS:
            try:
                f_scan = open_root(build_path(args.base, era, args.sr_subdir, args.file_pattern, flav))
                syst_keys = list_syst_keys_from_file(f_scan, comp_prefix="prompt_inc")
                f_scan.Close()
                if syst_keys:
                    break
            except Exception:
                continue
        if syst_keys:
            break
    print("[INFO] Found {} syst sources: {}".format(len(syst_keys), ", ".join(syst_keys)))
    print("[DEBUG] Has eff_m_reco_syst? ", any(k.startswith("eff_m_reco_syst") for k in syst_keys))

    def _strip_era_suffix(core):
        for tok in ERA_TOKENS:
            suf = "_" + tok
            if core.endswith(suf):
                return core[: -len(suf)]
        return core

    # NEW: collapse to base keys (no era suffix) and de-duplicate
    syst_keys = sorted({_strip_era_suffix(k) for k in syst_keys})
    
    # Optionally enforce presence of particular keys
    for must in ("eff_m_reco_syst",):
        if must not in syst_keys:
            syst_keys.append(must)
            
    print("[INFO] Using {} base syst sources: {}".format(len(syst_keys), ", ".join(syst_keys)))
        
    if args.fake_norm_syst > 0.0:
        syst_keys.append("FAKE_NORM")
    if args.cf_norm_syst > 0.0:
        syst_keys.append("CF_NORM")

    ### Add lumi systematic
    syst_keys.append("LUMI_NORM")
    
    if any(k in ("FAKE_NORM","CF_NORM") for k in syst_keys):
        print("[INFO] Added virtual norm systs:",
              ("FAKE_NORM" if args.fake_norm_syst > 0 else ""),
              ("CF_NORM" if args.cf_norm_syst > 0 else ""))
    
    # PASS 2: build concatenated histograms and grand sums
    outf = ROOT.TFile.Open(args.outfile, "RECREATE")
    if not outf or outf.IsZombie():
        raise IOError("Cannot create " + args.outfile)

    grand = {}            # bkgs + data + signal-1
    grand_sig2 = None     # aggregate of signal-2

    for era in ERAS:
        for flav in FLAVOURS:
            # Main pattern (backgrounds, data, signal-1)
            sr_path  = build_path(args.base, era, args.sr_subdir,  args.file_pattern, flav)
            cr1_path = build_path(args.base, era, args.cr1_subdir, args.file_pattern, flav)
            cr2_path = build_path(args.base, era, args.cr2_subdir, args.file_pattern, flav)
            cr3_path = build_path(args.base, era, args.cr3_subdir, args.file_pattern, flav)
            # Second signal pattern
            sr_path_s2  = build_path(args.base, era, args.sr_subdir,  args.file_pattern_sig2, flav)
            cr1_path_s2 = build_path(args.base, era, args.cr1_subdir, args.file_pattern_sig2, flav)
            cr2_path_s2 = build_path(args.base, era, args.cr2_subdir, args.file_pattern_sig2, flav)
            cr3_path_s2 = build_path(args.base, era, args.cr3_subdir, args.file_pattern_sig2, flav)

            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception:
                continue

            # Template finder
            def template_from(f):
                h = get_hist_maybe(f, "data_obs")
                if not h:
                    for cand in COMPONENTS_DEFAULT:
                        h = get_hist_maybe(f, cand)
                        if h:
                            break
                if not h:
                    raise KeyError("No template histogram found")
                return h

            try:
                tmpl_sr  = template_from(f_sr)
                tmpl_cr1 = template_from(f_cr1)
                tmpl_cr2 = template_from(f_cr2)
                tmpl_cr3 = template_from(f_cr3)
            except Exception:
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue


            d = ensure_dir(outf, "{}_{}".format(era, flav))
            d.cd()

            # Build concatenated histograms for each component for the main pattern
            for comp in args.components:
                hs  = get_hist_maybe(f_sr, comp)  or make_zero_like(tmpl_sr,  "{}_sr_zero".format(comp))
                hc1 = get_hist_maybe(f_cr1, comp) or make_zero_like(tmpl_cr1, "{}_cr1_zero".format(comp))
                hc2 = get_hist_maybe(f_cr2, comp) or make_zero_like(tmpl_cr2, "{}_cr2_zero".format(comp))
                hc3 = get_hist_maybe(f_cr3, comp) or make_zero_like(tmpl_cr3, "{}_cr3_zero".format(comp))

                if hs.GetNbinsX()  != max_sr:  hs  = pad_histogram(hs,  max_sr)
                if hc1.GetNbinsX() != max_cr1: hc1 = pad_histogram(hc1, max_cr1)
                if hc2.GetNbinsX() != max_cr2: hc2 = pad_histogram(hc2, max_cr2)
                if hc3.GetNbinsX() != max_cr3: hc3 = pad_histogram(hc3, max_cr3)

                h_concat = concat_hists_with_labels(hs, hc1, hc2, hc3, "{}_concat_{}_{}".format(comp, era, flav))
                h_concat.Write()

                if comp not in grand:
                    grand[comp] = h_concat.Clone("{}_grand".format(comp))
                    grand[comp].SetDirectory(0)
                else:
                    grand[comp].Add(h_concat)

            # Optional second signal aggregation
            try:
                f_sr_s2  = open_root(sr_path_s2)
                f_cr1_s2 = open_root(cr1_path_s2)
                f_cr2_s2 = open_root(cr2_path_s2)
                f_cr3_s2 = open_root(cr3_path_s2)
            except Exception:
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue

            # Templates for s2 (optional)
            try:
                tmpl_sr_s2  = template_from(f_sr_s2)
                tmpl_cr1_s2 = template_from(f_cr1_s2)
                tmpl_cr2_s2 = template_from(f_cr2_s2)
                tmpl_cr3_s2 = template_from(f_cr3_s2)
            except Exception:
                tmpl_sr_s2 = tmpl_cr1_s2 = tmpl_cr2_s2 = tmpl_cr3_s2 = None

            if tmpl_sr_s2:
                for comp in ["signalSSWW", "signalDYVBF"]:
                    hs2  = get_hist_maybe(f_sr_s2, comp)  or make_zero_like(tmpl_sr_s2,  "{}_sr_zero_s2".format(comp))
                    hc1s = get_hist_maybe(f_cr1_s2, comp) or make_zero_like(tmpl_cr1_s2, "{}_cr1_zero_s2".format(comp))
                    hc2s = get_hist_maybe(f_cr2_s2, comp) or make_zero_like(tmpl_cr2_s2, "{}_cr2_zero_s2".format(comp))
                    hc3s = get_hist_maybe(f_cr3_s2, comp) or make_zero_like(tmpl_cr3_s2, "{}_cr3_zero_s2".format(comp))

                    if hs2 and hs2.GetNbinsX()  != max_sr:  hs2  = pad_histogram(hs2,  max_sr)
                    if hc1s and hc1s.GetNbinsX() != max_cr1: hc1s = pad_histogram(hc1s, max_cr1)
                    if hc2s and hc2s.GetNbinsX() != max_cr2: hc2s = pad_histogram(hc2s, max_cr2)
                    if hc3s and hc3s.GetNbinsX() != max_cr3: hc3s = pad_histogram(hc3s, max_cr3)

                    if hs2 and hc1s and hc2s and hc3s:
                        h_concat_s2 = concat_hists_with_labels(hs2, hc1s, hc2s, hc3s, "{}_s2_concat_{}_{}".format(comp, era, flav))
                        if grand_sig2 is None:
                            grand_sig2 = h_concat_s2.Clone("signal2_grand")
                            grand_sig2.SetDirectory(0)
                        else:
                            grand_sig2.Add(h_concat_s2)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
            f_sr_s2.Close(); f_cr1_s2.Close(); f_cr2_s2.Close(); f_cr3_s2.Close()

    # Write grand histograms
    outf.cd()
    for comp, h in grand.items():
        h.Write()
    if grand_sig2:
        grand_sig2.Write()

    # Combine signal-1 from grand
    h_sig1 = None
    if ("signalSSWW" in grand) or ("signalDYVBF" in grand):
        if "signalSSWW" in grand:
            h_sig1 = grand["signalSSWW"].Clone("signal1_sum"); h_sig1.SetDirectory(0)
        if "signalDYVBF" in grand:
            if h_sig1 is None:
                h_sig1 = grand["signalDYVBF"].Clone("signal1_sum"); h_sig1.SetDirectory(0)
            else:
                h_sig1.Add(grand["signalDYVBF"])

    # Combine signal-2 (already summed)
    h_sig2 = grand_sig2.Clone("signal2_sum") if grand_sig2 else None

    # Backgrounds for plot (order matches COLORS keys used above where applicable)
    hists_bkg = []
    # --- merge prompt_others + conv_inc into a single "others" component

    for comp in ["fake", "cf", "zg", "wz","zz","ww","mc_others"]:
        if comp in grand:
            hh = grand[comp].Clone(comp + "_plot"); hh.SetDirectory(0)
            hists_bkg.append((comp, hh))

    h_data = grand.get("data_obs")

    # Build total background histogram for band/y-range
    h_bkg_total_plot = None
    for _, hb in hists_bkg:
        h_bkg_total_plot = add_hists(h_bkg_total_plot, hb)

       
    # Closure that builds grand background for a syst source & direction
    components_bkg = [c for c in args.components if c not in ("data_obs", "signalSSWW", "signalDYVBF")]

    def build_var_hist_fn(skey, up_or_down):

        # --- Virtual normalization-only sources (unchanged) ---
        if skey == "FAKE_NORM" and args.fake_norm_syst > 0.0:
            return build_grand_bkg_scaled(
                base=args.base,
                eras=ERAS,
                flavours=FLAVOURS,
                sr_subdir=args.sr_subdir, cr1_subdir=args.cr1_subdir,
                cr2_subdir=args.cr2_subdir, cr3_subdir=args.cr3_subdir,
                file_pattern=args.file_pattern,
                components_bkg=components_bkg,
                max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                scale_map={"fake": args.fake_norm_syst},
                up_or_down=up_or_down
            )
        
        if skey == "CF_NORM" and args.cf_norm_syst > 0.0:
            return build_grand_bkg_scaled(
                base=args.base,
                eras=ERAS,
                flavours=FLAVOURS,
            sr_subdir=args.sr_subdir, cr1_subdir=args.cr1_subdir,
                cr2_subdir=args.cr2_subdir, cr3_subdir=args.cr3_subdir,
                file_pattern=args.file_pattern,
                components_bkg=components_bkg,
                max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                scale_map={"cf": args.cf_norm_syst},
                up_or_down=up_or_down
            )
        if skey == "LUMI_NORM":
            return build_grand_bkg_scaled(
                base=args.base,
                eras=ERAS,
                flavours=FLAVOURS,
                sr_subdir=args.sr_subdir, cr1_subdir=args.cr1_subdir,
                cr2_subdir=args.cr2_subdir, cr3_subdir=args.cr3_subdir,
                file_pattern=args.file_pattern,
                components_bkg=components_bkg,
                max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                # scale all backgrounds equally
                scale_map={comp: args.lumi_norm_syst for comp in components_bkg},
                up_or_down=up_or_down
            )

        
        # --- Default: shape-based source (now era-aware) ---
        return build_grand_bkg_for_syst(
            base=args.base,
            eras=ERAS,
            flavours=FLAVOURS,
            sr_subdir=args.sr_subdir, cr1_subdir=args.cr1_subdir,
            cr2_subdir=args.cr2_subdir, cr3_subdir=args.cr3_subdir,
            file_pattern=args.file_pattern,
            components_bkg=components_bkg,
            max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
            syst_key=skey,        # pass base name, e.g. "eff_m_reco_syst"
            up_or_down=up_or_down
        )
    
    sanity_print_combined_delta(h_bkg_total_plot, build_var_hist_fn, "eff_m_reco_syst", 27, 31)

    # Make stat + syst bands
    g_bkg_band_main = None
    g_bkg_band_ratio = None
    if h_bkg_total_plot and syst_keys:
        g_bkg_band_main  = make_bkg_unc_band_abs(h_bkg_total_plot, syst_keys, build_var_hist_fn)
        g_bkg_band_ratio = make_bkg_unc_band_ratio(h_bkg_total_plot, g_bkg_band_main)
        # Main band style (gray fill, black outline)
        g_bkg_band_main.SetFillColor(ROOT.kGray + 1)            # light gray
        g_bkg_band_main.SetLineColor(ROOT.kBlack)               # black outline
        g_bkg_band_main.SetLineWidth(1)
        
        # Ratio band style (same)
        g_bkg_band_ratio.SetFillColor(ROOT.kGray + 1)
        g_bkg_band_ratio.SetLineColor(ROOT.kBlack)
        g_bkg_band_ratio.SetLineWidth(1)
        
    if args.dump_unc and h_bkg_total_plot and syst_keys:
        start_bin = 1
        end_bin = h_bkg_total_plot.GetNbinsX()
        if args.dump_range:
            try:
                parts = args.dump_range.split(":")
                if len(parts) == 2:
                    start_bin = int(parts[0])
                    end_bin = int(parts[1])
            except Exception:
                pass
        tsv = args.dump_tsv if args.dump_tsv else None
        max_sources = args.dump_max_sources if args.dump_max_sources > 0 else None
        dump_uncertainties_table(
            h_bkg_total_plot, syst_keys, build_var_hist_fn,
            start_bin=start_bin, end_bin=end_bin,
            tsv_path=tsv, max_sources=max_sources
        )

        
    # Region edges: after SR, after CR1, after CR2
    region_edges = [max_sr, max_sr + max_cr1, max_sr + max_cr1 + max_cr2]

    # Mass labels parsed from patterns
    mass1 = parse_mass_from_pattern(args.file_pattern)
    mass2 = parse_mass_from_pattern(args.file_pattern_sig2)

    if hists_bkg and h_data:
        make_stack_plot(
            hists_bkg, h_data, h_sig1, h_sig2, args.outplot,
            args.title, args.lumi, region_edges, max_sr, mass1, mass2,
            g_bkg_band_main=g_bkg_band_main,
            g_bkg_band_ratio=g_bkg_band_ratio,
            h_bkg_total_pre=h_bkg_total_plot
        )
    else:
        print("[WARN] Missing backgrounds or data; not plotting.")

    outf.Close()
    print("Wrote:", args.outfile)
    print("Plot: ", args.outplot, "(+ PDF)")
    print("SR/CR split lines drawn after bin:", region_edges)

if __name__ == "__main__":
    main()
