#!/usr/bin/env python3
"""
combined_srcr_plot_syst_refactored.py

Refactor of the original combined SR+CR plotting script to support:
  - Region presets (SR1/SR2/SR3) with automatic CR subdir wiring
    (SR3BDT is treated as an alias of SR3: inputs live under the same 'sr3' directory/hist naming)
  - Automatic region titles based on selected SR
  - Multiple signals and multiple masses (overlay when binning is compatible)
  - Robust padding when SR/CR bin counts differ
  - Signal auto-scaling for visibility
    (legend shows: total scale = input pre-scale * extra visual scale)

This script still sums over all eras and flavours by default.

Note:
  - ROOT histograms in inputs are assumed to already include your analysis normalization.
    The legend scale is the *total* factor: (pre-scale already baked into inputs) * (extra visual scale applied here).
"""

import os
import argparse
import math
import re
import json
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

import ROOT

ROOT.PyConfig.IgnoreCommandLineOptions = True
ROOT.gROOT.SetBatch(True)

# -------------------------------------------------------------------------
# Constants
# -------------------------------------------------------------------------

ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]
FLAVOURS = ["EE", "MuMu", "EMu"]

DEFAULT_FAKE = {
    "EE": 0.25,
    "MuMu": 0.20,
    "EMu": 0.30
}

ERA_TOKENS = ["2016preVFP", "2016postVFP", "2017", "2018"]

# Available mass tokens in your card-input naming
MASS_CHOICES = [
    "M85","M90","M95","M100","M125","M150","M200","M250","M300","M350","M400","M450",
    "M500","M600","M700","M800","M900","M1000","M1100","M1200","M1300","M1500","M1700",
    "M2000","M2500","M3000","M5000","M7500","M10000","M15000","M20000","M25000","M30000",
    "M40000","M50000","M60000",
    "Weinberg",
]

# Default backgrounds (stack order is defined by this list)
BKG_COMPONENTS_DEFAULT = ["fake", "cf", "zg", "wz", "wz_ewk", "zz", "ww", "mc_others"]
DATA_COMPONENT_DEFAULT = "data_obs"

# Colors for backgrounds
COLORS_BKG = {
    # Recommended palette (hex)
    "cf": ROOT.TColor.GetColor("#b9ac70"),
    "fake": ROOT.TColor.GetColor("#3f90da"),
    "wz": ROOT.TColor.GetColor("#ffa90e"),
    # WZ_EWK is merged into WZ in plots; keep same color for safety
    "wz_ewk": ROOT.TColor.GetColor("#ffa90e"),
    "zz": ROOT.TColor.GetColor("#bd1f01"),
    "ww": ROOT.TColor.GetColor("#94a4a2"),
    "zg": ROOT.TColor.GetColor("#832db6"),
    "mc_others": ROOT.TColor.GetColor("#92dadd"),
}

# Sentinel values for "no effect" in syst shapes
SENTINEL_EPS = 1e-9
SENTINEL_VAL = 0.001

# Try to apply tdrstyle if available
try:
    import tdrstyle
    tdrstyle.setTDRStyle()
except Exception:
    pass

# PostScript/PDF line scaling + custom dashed styles
ROOT.gStyle.SetLineScalePS(1.0)
ROOT.gStyle.SetLineStyleString(9, "12 6")  # dash 12, gap 6
ROOT.gStyle.SetLineStyleString(11, "8 4")  # dash 8, gap 4

# -------------------------------------------------------------------------
# Region + Signal configuration
# -------------------------------------------------------------------------

@dataclass(frozen=True)
class RegionPreset:
    key: str
    sr_subdir: str
    cr1_subdir: str
    cr2_subdir: str
    cr3_subdir: str
    sr_title: str
    # Whether the SR binning depends on the signal mass.
    # NOTE: SR3 is special in this analysis: low-mass (<=500 GeV) is mass-dependent,
    # while high-mass (>500 GeV) and Weinberg share the same binning.
    mass_dependent_binning: bool

def _norm_region_key(x: str) -> str:
    """Normalize region keys like 'SR3BDT', 'sr3_bdt' -> 'sr3bdt'."""
    x = (x or "").strip().lower()
    x = re.sub(r"[^a-z0-9]+", "", x)
    return x

REGION_PRESETS: Dict[str, RegionPreset] = {
    # If your directory names differ, you can override with --sr-subdir / --cr*-subdir.
    "sr1": RegionPreset(
        key="sr1",
        sr_subdir="sr1",
        cr1_subdir="cr1_InvBJet",
        cr2_subdir="cr1_InvMET",
        cr3_subdir="wz_cr1",
        sr_title="SR1",
        mass_dependent_binning=True,
    ),
    "sr2": RegionPreset(
        key="sr2",
        sr_subdir="sr2",
        cr1_subdir="cr2_InvBJet",
        cr2_subdir="cr2_InvMET",
        cr3_subdir="wz_cr2",
        sr_title="SR2",
        mass_dependent_binning=False,
    ),
    "sr3": RegionPreset(
        key="sr3",
        sr_subdir="sr3",
        cr1_subdir="cr3_InvBJet",
        cr2_subdir="cr3_InvMET",
        cr3_subdir="wz_cr3",
        sr_title="Resolved SR",
        mass_dependent_binning=False,
    ),
}

def _is_sr3_lowmass_mass_dependent(mass: str) -> bool:
    """SR3 low-mass (<=500 GeV) has mass-dependent binning; high-mass and Weinberg do not."""
    if not mass:
        return False
    if mass == "Weinberg":
        return False
    m = _mass_to_int(mass)
    if m is None:
        return False
    return m <= 500

def is_mass_dependent_binning(region_key: str, sr_subdir: str, base_mass: str) -> bool:
    """Decide whether binning is mass-dependent for the given region and base mass."""
    r = _norm_region_key(region_key)
    s = _norm_region_key(sr_subdir)
    if r == "sr1" or s == "sr1":
        return True
    # SR3 special rule
    if r == "sr3" or s == "sr3":
        return _is_sr3_lowmass_mass_dependent(base_mass)
    return False

# Region key aliases (backward-compatibility)
#
# In this analysis setup, low-mass "SR3BDT" content is stored under the same
# directory/histogram naming as SR3. So we treat SR3BDT as an alias of SR3.
REGION_ALIASES: Dict[str, str] = {
    "sr3bdt": "sr3",
}
def _resolve_existing_subdir(base: str, candidates: List[str]) -> str:
    """Pick the first subdir candidate that exists under base/<era>/<subdir>. Fallback to the first candidate."""
    for cand in candidates:
        if not cand:
            continue
        for era in ERAS:
            if os.path.isdir(os.path.join(base, era, cand)):
                return cand
    return candidates[0] if candidates else ""

@dataclass(frozen=True)
class SignalTemplate:
    key: str
    # primary histogram(s) to try in the ROOT file
    hnames: Tuple[str, ...]
    # optional fallback histogram(s); if provided and primary missing, sum these
    fallback_sum_hnames: Tuple[str, ...] = ()
    # legend label template (can include {mass})
    label_tmpl: str = ""
    # "family" used for default color selection
    family: str = "HNL"

SIGNAL_LIBRARY: Dict[str, SignalTemplate] = {
    "DY": SignalTemplate(
        key="DY",
        hnames=("signalDY",),
        label_tmpl="DY M={mass} GeV",
        family="HNL",
    ),
    "VBF": SignalTemplate(
        key="VBF",
        hnames=("signalVBF",),
        label_tmpl="W#gamma M={mass} GeV",
        family="HNL",
    ),
    "DYVBF": SignalTemplate(
        key="DYVBF",
        hnames=("signalDYVBF",),
        fallback_sum_hnames=("signalDY", "signalVBF"),
        label_tmpl="DY+W#gamma M={mass} GeV",
        family="HNL",
    ),
    "SSWW": SignalTemplate(
        key="SSWW",
        hnames=("signalSSWW",),
        label_tmpl="SSWW M={mass} GeV",
        family="HNL",
    ),
    "Weinberg": SignalTemplate(
        key="Weinberg",
        hnames=("signalWeinberg",),
        #label_tmpl="#splitline{Weinberg op.}{c^{#mu}_{5}=1, #Lambda=200 TeV}",
        label_tmpl="Weinberg op.",
        family="Weinberg",
    ),
}

def _mass_to_int(m: str) -> Optional[int]:
    if not m:
        return None
    if m == "Weinberg":
        return None
    mm = m.strip()
    if mm.startswith("M") and mm[1:].isdigit():
        return int(mm[1:])
    return None

def _parse_color(s: str, default: int) -> int:
    """Parse a ROOT color specification.

    Supported:
      - integer literal (ROOT color index), e.g. "632"
      - hex "#RRGGBB" (via ROOT.TColor.GetColor)
      - ROOT color expressions like "kBlue", "kRed+2", "kAzure-9"
        (optional "ROOT." prefix, whitespace is ignored)

    Notes:
      - We intentionally avoid `eval` for safety.
      - If parsing fails, returns `default`.
    """
    if s is None:
        return default
    ss = str(s).strip()
    if ss == "":
        return default

    # int literal?
    try:
        return int(ss)
    except Exception:
        pass

    # hex?
    if re.fullmatch(r"#?[0-9a-fA-F]{6}", ss):
        if not ss.startswith("#"):
            ss = "#" + ss
        return ROOT.TColor.GetColor(ss)

    # ROOT constant expression: kBlue, kRed+2, kGray+1, ...
    expr = re.sub(r"\s+", "", ss)
    if expr.startswith("ROOT."):
        expr = expr[len("ROOT."):]

    m = re.match(r"^(k[A-Za-z0-9_]+)(.*)$", expr)
    if not m:
        # common names (backward-compat)
        name = ss.lower()
        mapping = {
            "red": ROOT.kRed,
            "blue": ROOT.kBlue,
            "black": ROOT.kBlack,
            "gray": ROOT.kGray + 1,
            "grey": ROOT.kGray + 1,
            "green": ROOT.kGreen + 2,
            "orange": ROOT.kOrange + 7,
            "magenta": ROOT.kMagenta + 2,
            "cyan": ROOT.kCyan + 2,
            "yellow": ROOT.kYellow + 2,
            "azure": ROOT.kAzure + 2,
            "spring": ROOT.kSpring + 1,
        }
        return mapping.get(name, default)

    base_name, tail = m.group(1), m.group(2)
    if not hasattr(ROOT, base_name):
        return default
    try:
        val = int(getattr(ROOT, base_name))
    except Exception:
        return default

    if tail:
        # must be a sequence like +2-1+10
        if not re.fullmatch(r"([+-]\d+)+", tail):
            return default
        for tok in re.findall(r"[+-]\d+", tail):
            val += int(tok)
    return val


# -------------------------------------------------------------------------
# Default signal pre-scaling in the *inputs*
# -------------------------------------------------------------------------

# The signal histograms in the ROOT inputs are already pre-scaled by these factors.
# We do NOT apply these again (they are only used to annotate the legend scale).
SIGNAL_INPUT_PRESCALE = {
    # Weinberg operator samples are pre-scaled by 1e4 in the inputs
    "Weinberg": 1.0e4,
    # DY/VBF(/DYVBF): threshold at 100 GeV
    "HNL_MLE100": 1.0e-3,
    "HNL_MGT100": 1.0e-2,
    # SSWW: threshold at 3000 GeV
    "SSWW_MLE3000": 1.0e-4,
    "SSWW_MGT3000": 1.0e-2,
}

def signal_input_prescale(sig_key: str, mass: str) -> float:
    """Return the pre-scale factor already applied in the input histogram.

    Rules provided by the analysis setup:
      - HNL (DY/VBF/DYVBF):
          m <= 100  -> 1e-3
          m >  100  -> 1e-2
        (samples exist only up to 3000 GeV)

      - SSWW:
          m <= 3000 -> 1e-4
          m >  3000 -> 1e-2

      - Weinberg: always 1e4
    """
    if sig_key == "Weinberg":
        return float(SIGNAL_INPUT_PRESCALE["Weinberg"])

    m = _mass_to_int(mass)
    if m is None:
        return 1.0

    if sig_key in ("DY", "VBF", "DYVBF"):
        return float(SIGNAL_INPUT_PRESCALE["HNL_MLE100" if m <= 100 else "HNL_MGT100"])

    if sig_key == "SSWW":
        return float(SIGNAL_INPUT_PRESCALE["SSWW_MLE3000" if m <= 3000 else "SSWW_MGT3000"])

    return 1.0

# -------------------------------------------------------------------------
# ROOT helpers
# -------------------------------------------------------------------------

def build_path(base: str, era: str, subdir: str, file_pattern: str, mass: str, flavour: str) -> str:
    return os.path.join(base, era, subdir, file_pattern.format(mass=mass, flavour=flavour))

def open_root(file_path: str) -> ROOT.TFile:
    f = ROOT.TFile.Open(file_path, "READ")
    if not f or f.IsZombie():
        raise IOError("Cannot open ROOT file: " + file_path)
    return f

def get_hist_maybe(f: ROOT.TFile, hname: str) -> Optional[ROOT.TH1]:
    h = f.Get(hname) if f else None
    if not h:
        return None
    h = h.Clone()
    h.SetDirectory(0)
    return h

def make_zero_like(template: ROOT.TH1, name: str) -> ROOT.TH1:
    h = ROOT.TH1D(name, "", template.GetNbinsX(),
                  template.GetXaxis().GetXmin(), template.GetXaxis().GetXmax())
    h.Sumw2()
    h.SetDirectory(0)
    return h

def pad_histogram(h: ROOT.TH1, target_bins: int) -> ROOT.TH1:
    """Pad (or truncate) to target_bins with a simple 0.5..N+0.5 axis."""
    if h.GetNbinsX() == target_bins:
        return h
    new_h = ROOT.TH1D(h.GetName() + "_pad", h.GetTitle(), target_bins, 0.5, target_bins + 0.5)
    new_h.Sumw2()
    new_h.SetDirectory(0)
    for i in range(1, min(h.GetNbinsX(), target_bins) + 1):
        new_h.SetBinContent(i, h.GetBinContent(i))
        new_h.SetBinError(i, h.GetBinError(i))
    return new_h

def concat_hists_with_labels(h_sr: ROOT.TH1, h_cr1: ROOT.TH1, h_cr2: ROOT.TH1, h_cr3: ROOT.TH1, name: str) -> ROOT.TH1:
    nbins_total = h_sr.GetNbinsX() + h_cr1.GetNbinsX() + h_cr2.GetNbinsX() + h_cr3.GetNbinsX()
    h_out = ROOT.TH1D(name, "", nbins_total, 0.5, nbins_total + 0.5)
    h_out.Sumw2()
    h_out.SetDirectory(0)

    def copy_bins(src: ROOT.TH1, start_bin: int, tag: str) -> int:
        for i in range(1, src.GetNbinsX() + 1):
            h_out.SetBinContent(start_bin, src.GetBinContent(i))
            h_out.SetBinError(start_bin, src.GetBinError(i))
            h_out.GetXaxis().SetBinLabel(start_bin, f"{tag} {i}")
            start_bin += 1
        return start_bin

    next_bin = 1
    next_bin = copy_bins(h_sr,  next_bin, "SR")
    next_bin = copy_bins(h_cr1, next_bin, "IB")
    next_bin = copy_bins(h_cr2, next_bin, "IM")
    copy_bins(h_cr3, next_bin, "WZ")
    return h_out

def add_hists(a: Optional[ROOT.TH1], b: ROOT.TH1) -> ROOT.TH1:
    if a is None:
        out = b.Clone()
        out.SetDirectory(0)
        return out
    out = a.Clone()
    out.Add(b)
    out.SetDirectory(0)
    return out

def nbins_fallback(file_obj: ROOT.TFile, preferred: Optional[List[str]] = None) -> int:
    """Try data_obs first, then the provided preferred list, else 0."""
    h = get_hist_maybe(file_obj, DATA_COMPONENT_DEFAULT)
    if h:
        return h.GetNbinsX()
    for cand in (preferred or []):
        h = get_hist_maybe(file_obj, cand)
        if h:
            return h.GetNbinsX()
    return 0

def ensure_dir(outf: ROOT.TFile, name: str) -> ROOT.TDirectory:
    d = outf.Get(name)
    if not d:
        d = outf.mkdir(name)
    return d

# -------------------------------------------------------------------------
# Systematics helpers (mostly carried from original)
# -------------------------------------------------------------------------

def is_sentinel_value(x: float) -> bool:
    return (abs(x) < SENTINEL_EPS) or (abs(x - SENTINEL_VAL) < 1e-9)

def list_syst_keys_from_file(tf: ROOT.TFile, comp_prefix: str = "wz") -> List[str]:
    """
    Discover unique systematic keys that appear as:
      '<comp_prefix>_CMS_<syst_key>Up/Down' or '<comp_prefix>_pdf*' or '<comp_prefix>_QCD*'
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
        if not (name.startswith(f"{comp_prefix}_CMS_")
                or name.startswith(f"{comp_prefix}_CMS_SUS24014_")
                or name.startswith(f"{comp_prefix}_pdf")
                or name.startswith(f"{comp_prefix}_QCD")):
            continue
        if name.endswith("Up") or name.endswith("Down"):
            core = re.sub(r"(Up|Down)$", "", name)
            # remove leading comp_ and CMS tags, but keep pdf/QCD raw suffixes consistent
            if ("pdf" not in core) and ("QCDscale" not in core):
                core = core.replace(f"{comp_prefix}_CMS_SUS24014_", "").replace(f"{comp_prefix}_CMS_", "")
            else:
                core = core.replace(f"{comp_prefix}_", "")
            syst.add(core)
    return sorted(syst)

def _strip_era_suffix(core: str) -> str:
    for tok in ERA_TOKENS:
        suf = "_" + tok
        if core.endswith(suf):
            return core[: -len(suf)]
    return core

def _syst_region_tokens_for(sr_key: str, sr_subdir: str) -> List[str]:
    """
    Build a list of region tokens to try inside systematic histogram names.
    This is intentionally permissive because naming conventions differ.
    """
    tokens = []
    # canonical
    if sr_key:
        tokens.append(sr_key)
    # common analysis tokens
    # e.g. "sr1", "sr2", "sr3", "sr3bdt"
    # from subdir too (might be "sr3_bdt", "sr3BDT", etc)
    if sr_subdir:
        norm = _norm_region_key(sr_subdir)
        if norm and norm not in tokens:
            tokens.append(norm)
        # also add raw subdir if it already looks like "srX..."
        raw = sr_subdir.strip()
        if raw and raw not in tokens:
            tokens.append(raw)

    # Also try standard sr1/sr2/sr3 always, since many files embed them independent of chosen region
    for t in ("sr1", "sr2", "sr3"):
        if t not in tokens:
            tokens.append(t)

    return tokens

def build_grand_bkg_for_syst(
    base: str,
    eras: List[str],
    flavours: List[str],
    mass: str,
    sr_subdir: str,
    cr1_subdir: str,
    cr2_subdir: str,
    cr3_subdir: str,
    file_pattern: str,
    components_bkg: List[str],
    max_sr: int,
    max_cr1: int,
    max_cr2: int,
    max_cr3: int,
    syst_key: str,
    up_or_down: str,
    sr_key_for_syst: str = "",
) -> Optional[ROOT.TH1]:
    """
    Build concatenated background-only grand histogram for a single syst family and direction.
    Tries multiple naming patterns (including SR token variants) and falls back to nominal.
    """
    grand_var = None
    region_tokens = _syst_region_tokens_for(sr_key_for_syst, sr_subdir)

    def choose_var_or_nom(f: ROOT.TFile, comp: str, tmpl: ROOT.TH1, syst_base: str, era: str, which_dir: str) -> ROOT.TH1:
        # Try era+region-specific patterns, then era-only, then base-only
        candidates = []
        for regtok in region_tokens:
            candidates.extend([
                f"{comp}_CMS_SUS24014_{syst_base}_{era}_{regtok}{which_dir}",
                f"{comp}_CMS_{syst_base}_{era}_{regtok}{which_dir}",
                f"{comp}_{syst_base}_{era}_{regtok}{which_dir}",
            ])
        candidates.extend([
            f"{comp}_CMS_SUS24014_{syst_base}_{era}{which_dir}",
            f"{comp}_CMS_{syst_base}_{era}{which_dir}",
            f"{comp}_{syst_base}_{era}{which_dir}",
            f"{comp}_CMS_SUS24014_{syst_base}{which_dir}",
            f"{comp}_CMS_{syst_base}{which_dir}",
            f"{comp}_{syst_base}{which_dir}",
        ])
        for patt in candidates:
            h = get_hist_maybe(f, patt)
            if h and not is_sentinel_value(float(h.Integral())):
                return h

        # fallback to nominal component
        h_nom = get_hist_maybe(f, comp)
        if h_nom:
            return h_nom

        return make_zero_like(tmpl, f"{comp}_zero_fallback")

    for era in eras:
        for flav in flavours:
            sr_path  = build_path(base, era, sr_subdir,  file_pattern, mass, flav)
            cr1_path = build_path(base, era, cr1_subdir, file_pattern, mass, flav)
            cr2_path = build_path(base, era, cr2_subdir, file_pattern, mass, flav)
            cr3_path = build_path(base, era, cr3_subdir, file_pattern, mass, flav)
            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception:
                continue

            tmpl_sr  = get_hist_maybe(f_sr,  DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_sr,  components_bkg[0])
            tmpl_cr1 = get_hist_maybe(f_cr1, DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_cr1, components_bkg[0])
            tmpl_cr2 = get_hist_maybe(f_cr2, DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_cr2, components_bkg[0])
            tmpl_cr3 = get_hist_maybe(f_cr3, DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_cr3, components_bkg[0])
            if not (tmpl_sr and tmpl_cr1 and tmpl_cr2 and tmpl_cr3):
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue

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

            h_concat = concat_hists_with_labels(sum_sr, sum_cr1, sum_cr2, sum_cr3, f"bkg_{syst_key}{up_or_down}_{era}_{flav}")
            if grand_var is None:
                grand_var = h_concat.Clone(f"bkg_{syst_key}{up_or_down}_grand")
                grand_var.SetDirectory(0)
            else:
                grand_var.Add(h_concat)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    return grand_var

def build_grand_bkg_scaled(
    base: str,
    eras: List[str],
    flavours: List[str],
    mass: str,
    sr_subdir: str,
    cr1_subdir: str,
    cr2_subdir: str,
    cr3_subdir: str,
    file_pattern: str,
    components_bkg: List[str],
    max_sr: int,
    max_cr1: int,
    max_cr2: int,
    max_cr3: int,
    scale_map: Optional[Dict[str, float]] = None,
    scale_map_by_flavour: Optional[Dict[str, Dict[str, float]]] = None,
    up_or_down: str = "Up",
) -> Optional[ROOT.TH1]:
    """
    Build background-only grand histogram with normalization scaling:
      Up: (1+frac), Down: max(0, 1-frac)
    """
    grand_var = None

    for era in eras:
        for flav in flavours:
            sr_path  = build_path(base, era, sr_subdir,  file_pattern, mass, flav)
            cr1_path = build_path(base, era, cr1_subdir, file_pattern, mass, flav)
            cr2_path = build_path(base, era, cr2_subdir, file_pattern, mass, flav)
            cr3_path = build_path(base, era, cr3_subdir, file_pattern, mass, flav)
            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception:
                continue

            tmpl_sr  = get_hist_maybe(f_sr,  DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_sr,  components_bkg[0])
            tmpl_cr1 = get_hist_maybe(f_cr1, DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_cr1, components_bkg[0])
            tmpl_cr2 = get_hist_maybe(f_cr2, DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_cr2, components_bkg[0])
            tmpl_cr3 = get_hist_maybe(f_cr3, DATA_COMPONENT_DEFAULT) or get_hist_maybe(f_cr3, components_bkg[0])
            if not (tmpl_sr and tmpl_cr1 and tmpl_cr2 and tmpl_cr3):
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue

            def factor_for(comp: str, flav: str) -> float:
                if scale_map_by_flavour is not None:
                    frac = scale_map_by_flavour.get(flav, {}).get(comp, 0.0)
                else:
                    frac = (scale_map or {}).get(comp, 0.0)
                if frac <= 0:
                    return 1.0
                if up_or_down == "Up":
                    return 1.0 + frac
                return max(0.0, 1.0 - frac)

            sum_sr = sum_cr1 = sum_cr2 = sum_cr3 = None
            for comp in components_bkg:
                hs  = get_hist_maybe(f_sr,  comp) or make_zero_like(tmpl_sr,  f"{comp}_sr_zero_norm")
                hc1 = get_hist_maybe(f_cr1, comp) or make_zero_like(tmpl_cr1, f"{comp}_cr1_zero_norm")
                hc2 = get_hist_maybe(f_cr2, comp) or make_zero_like(tmpl_cr2, f"{comp}_cr2_zero_norm")
                hc3 = get_hist_maybe(f_cr3, comp) or make_zero_like(tmpl_cr3, f"{comp}_cr3_zero_norm")

                if hs.GetNbinsX()  != max_sr:  hs  = pad_histogram(hs,  max_sr)
                if hc1.GetNbinsX() != max_cr1: hc1 = pad_histogram(hc1, max_cr1)
                if hc2.GetNbinsX() != max_cr2: hc2 = pad_histogram(hc2, max_cr2)
                if hc3.GetNbinsX() != max_cr3: hc3 = pad_histogram(hc3, max_cr3)

                fac = factor_for(comp, flav)
                if fac != 1.0:
                    hs.Scale(fac); hc1.Scale(fac); hc2.Scale(fac); hc3.Scale(fac)

                sum_sr  = add_hists(sum_sr,  hs)
                sum_cr1 = add_hists(sum_cr1, hc1)
                sum_cr2 = add_hists(sum_cr2, hc2)
                sum_cr3 = add_hists(sum_cr3, hc3)

            h_concat = concat_hists_with_labels(sum_sr, sum_cr1, sum_cr2, sum_cr3, f"bkg_normscale_{up_or_down}_{era}_{flav}")
            if grand_var is None:
                grand_var = h_concat.Clone(f"bkg_normscale_{up_or_down}_grand")
                grand_var.SetDirectory(0)
            else:
                grand_var.Add(h_concat)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    return grand_var

def make_bkg_unc_band_abs(h_bkg_total: ROOT.TH1, syst_keys: List[str], build_var_hist_fn) -> ROOT.TGraphAsymmErrors:
    """Absolute uncertainty band for total background (stat ⊕ syst)."""
    n = h_bkg_total.GetNbinsX()
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

        etot = math.sqrt(stat * stat + syst2_sum)

        g.SetPoint(i - 1, x, y)
        g.SetPointError(i - 1, ex, ex, etot, etot)

    g.SetFillStyle(3144)
    try:
        g.SetFillColorAlpha(ROOT.kGray + 1, 0.5)
    except Exception:
        g.SetFillColor(ROOT.kGray + 1)
    g.SetLineColor(0)
    return g

def make_bkg_unc_band_ratio(h_bkg_total: ROOT.TH1, band_abs: ROOT.TGraphAsymmErrors) -> ROOT.TGraphAsymmErrors:
    """Ratio uncertainty band centered at 1."""
    n = h_bkg_total.GetNbinsX()
    g = ROOT.TGraphAsymmErrors(n)
    for i in range(1, n + 1):
        x  = h_bkg_total.GetBinCenter(i)
        ex = 0.5
        y  = h_bkg_total.GetBinContent(i)
        ey = band_abs.GetErrorYhigh(i - 1)
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

def _format_lumi_text(lumi_str: str) -> str:
    return lumi_str.replace("fb-1", "fb^{-1}") if "fb-1" in lumi_str else lumi_str

# -------------------------------------------------------------------------
# Region titles + bin scanning
# -------------------------------------------------------------------------

def infer_region_titles(sr_title: str, cr1_subdir: str, cr2_subdir: str, cr3_subdir: str) -> List[str]:
    """
    Return [SR_title, CR1_title, CR2_title, CR3_title].
    CR titles are inferred from subdir names (InvBJet/InvMET/WZ), with safe fallbacks.
    """
    def cr_title_from_subdir(s: str, fallback: str) -> str:
        s_low = (s or "").lower()
        if "invbjet" in s_low or "bjet" in s_low:
            return "IB"
        if "invmet" in s_low or "met" in s_low:
            return "IM"
        if "wz" in s_low:
            return "WZ"
        return fallback

    return [
        sr_title or "SR",
        cr_title_from_subdir(cr1_subdir, "CR1"),
        cr_title_from_subdir(cr2_subdir, "CR2"),
        cr_title_from_subdir(cr3_subdir, "CR3"),
    ]

def scan_max_bins(
    base: str,
    masses: List[str],
    sr_subdir: str,
    cr1_subdir: str,
    cr2_subdir: str,
    cr3_subdir: str,
    file_pattern: str,
    bkg_components: List[str],
    data_component: str,
    verbose: bool = True,
) -> Tuple[int, int, int, int, Dict[str, Dict[str, Dict[str, int]]]]:
    """
    Scan era/flavour/region files and return max bins for SR/CR1/CR2/CR3 across given masses.
    Also returns a nested dict with per-(mass,era,flav) bin counts for diagnostics.
    """
    max_sr = max_cr1 = max_cr2 = max_cr3 = 0
    info: Dict[str, Dict[str, Dict[str, int]]] = {}

    preferred = [data_component] + list(bkg_components)

    if verbose:
        print("\n=== Bin Count Summary (scan) ===")
        hdr = "{:<10} {:<12} {:<6} {:>5} {:>5} {:>5} {:>5} | {:>6}".format("Mass", "Era", "Flav", "SR", "CR1", "CR2", "CR3", "Concat")
        print(hdr)
        print("-" * len(hdr))

    for mass in masses:
        info[mass] = {}
        for era in ERAS:
            info[mass][era] = {}
            for flav in FLAVOURS:
                sr_path  = build_path(base, era, sr_subdir,  file_pattern, mass, flav)
                cr1_path = build_path(base, era, cr1_subdir, file_pattern, mass, flav)
                cr2_path = build_path(base, era, cr2_subdir, file_pattern, mass, flav)
                cr3_path = build_path(base, era, cr3_subdir, file_pattern, mass, flav)

                try:
                    f_sr  = open_root(sr_path)
                    f_cr1 = open_root(cr1_path)
                    f_cr2 = open_root(cr2_path)
                    f_cr3 = open_root(cr3_path)
                except Exception as e:
                    if verbose:
                        print("{:<10} {:<12} {:<6} MISSING: {}".format(mass, era, flav, e))
                    continue

                sr_bins  = nbins_fallback(f_sr, preferred=preferred)
                cr1_bins = nbins_fallback(f_cr1, preferred=preferred)
                cr2_bins = nbins_fallback(f_cr2, preferred=preferred)
                cr3_bins = nbins_fallback(f_cr3, preferred=preferred)

                info[mass][era][flav] = {
                    "SR": sr_bins, "CR1": cr1_bins, "CR2": cr2_bins, "CR3": cr3_bins,
                }

                concat_bins = sr_bins + cr1_bins + cr2_bins + cr3_bins
                if verbose:
                    print("{:<10} {:<12} {:<6} {:5d} {:5d} {:5d} {:5d} | {:6d}".format(
                        mass, era, flav, sr_bins, cr1_bins, cr2_bins, cr3_bins, concat_bins))

                max_sr  = max(max_sr,  sr_bins)
                max_cr1 = max(max_cr1, cr1_bins)
                max_cr2 = max(max_cr2, cr2_bins)
                max_cr3 = max(max_cr3, cr3_bins)

                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    if verbose:
        total_bins = max_sr + max_cr1 + max_cr2 + max_cr3
        print(f"[TARGET] SR={max_sr} CR1={max_cr1} CR2={max_cr2} CR3={max_cr3} | total={total_bins}")

    return max_sr, max_cr1, max_cr2, max_cr3, info

# -------------------------------------------------------------------------
# Build "grand" histograms (sum over eras/flavours)
# -------------------------------------------------------------------------

def _template_from_file(f: ROOT.TFile, data_component: str, preferred_components: List[str]) -> ROOT.TH1:
    h = get_hist_maybe(f, data_component)
    if h:
        return h
    for cand in preferred_components:
        h = get_hist_maybe(f, cand)
        if h:
            return h
    raise KeyError("No template histogram found (data or preferred components)")

def build_grand_component(
    base: str,
    mass: str,
    sr_subdir: str,
    cr1_subdir: str,
    cr2_subdir: str,
    cr3_subdir: str,
    file_pattern: str,
    comp: str,
    max_sr: int,
    max_cr1: int,
    max_cr2: int,
    max_cr3: int,
    data_component: str,
    preferred_components: List[str],
    write_per_era_flav: bool,
    outf: Optional[ROOT.TFile],
) -> Optional[ROOT.TH1]:
    """Sum one component over all eras/flavours, concatenating SR+CR bins with padding."""
    grand = None

    for era in ERAS:
        for flav in FLAVOURS:
            sr_path  = build_path(base, era, sr_subdir,  file_pattern, mass, flav)
            cr1_path = build_path(base, era, cr1_subdir, file_pattern, mass, flav)
            cr2_path = build_path(base, era, cr2_subdir, file_pattern, mass, flav)
            cr3_path = build_path(base, era, cr3_subdir, file_pattern, mass, flav)

            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception:
                continue

            try:
                tmpl_sr  = _template_from_file(f_sr,  data_component, preferred_components)
                tmpl_cr1 = _template_from_file(f_cr1, data_component, preferred_components)
                tmpl_cr2 = _template_from_file(f_cr2, data_component, preferred_components)
                tmpl_cr3 = _template_from_file(f_cr3, data_component, preferred_components)
            except Exception:
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue

            hs  = get_hist_maybe(f_sr,  comp) or make_zero_like(tmpl_sr,  f"{comp}_sr_zero")
            hc1 = get_hist_maybe(f_cr1, comp) or make_zero_like(tmpl_cr1, f"{comp}_cr1_zero")
            hc2 = get_hist_maybe(f_cr2, comp) or make_zero_like(tmpl_cr2, f"{comp}_cr2_zero")
            hc3 = get_hist_maybe(f_cr3, comp) or make_zero_like(tmpl_cr3, f"{comp}_cr3_zero")

            if hs.GetNbinsX()  != max_sr:  hs  = pad_histogram(hs,  max_sr)
            if hc1.GetNbinsX() != max_cr1: hc1 = pad_histogram(hc1, max_cr1)
            if hc2.GetNbinsX() != max_cr2: hc2 = pad_histogram(hc2, max_cr2)
            if hc3.GetNbinsX() != max_cr3: hc3 = pad_histogram(hc3, max_cr3)

            h_concat = concat_hists_with_labels(hs, hc1, hc2, hc3, f"{comp}_concat_{era}_{flav}")

            # optionally write per era/flavour dir
            if write_per_era_flav and outf:
                d = ensure_dir(outf, f"{era}_{flav}")
                d.cd()
                h_concat.Write()

            if grand is None:
                grand = h_concat.Clone(f"{comp}_grand")
                grand.SetDirectory(0)
            else:
                grand.Add(h_concat)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    return grand

def build_grand_signal(
    base: str,
    mass: str,
    sr_subdir: str,
    cr1_subdir: str,
    cr2_subdir: str,
    cr3_subdir: str,
    file_pattern: str,
    templ: SignalTemplate,
    max_sr: int,
    max_cr1: int,
    max_cr2: int,
    max_cr3: int,
    data_component: str,
    preferred_components: List[str],
) -> Optional[ROOT.TH1]:
    """
    Build a grand concatenated signal histogram for a given mass.
    If the primary histogram name is missing, optionally fall back to summing fallback_sum_hnames.
    """
    # Decide which histogram(s) we will attempt for this template
    primary_names = list(templ.hnames)
    fallback_names = list(templ.fallback_sum_hnames)

    def fetch_comp_hist(f: ROOT.TFile, comp: str, tmpl: ROOT.TH1, suffix: str) -> ROOT.TH1:
        h = get_hist_maybe(f, comp)
        return h if h else make_zero_like(tmpl, f"{comp}_{suffix}_zero")

    grand = None

    for era in ERAS:
        for flav in FLAVOURS:
            sr_path  = build_path(base, era, sr_subdir,  file_pattern, mass, flav)
            cr1_path = build_path(base, era, cr1_subdir, file_pattern, mass, flav)
            cr2_path = build_path(base, era, cr2_subdir, file_pattern, mass, flav)
            cr3_path = build_path(base, era, cr3_subdir, file_pattern, mass, flav)

            try:
                f_sr  = open_root(sr_path)
                f_cr1 = open_root(cr1_path)
                f_cr2 = open_root(cr2_path)
                f_cr3 = open_root(cr3_path)
            except Exception:
                continue

            try:
                tmpl_sr  = _template_from_file(f_sr,  data_component, preferred_components)
                tmpl_cr1 = _template_from_file(f_cr1, data_component, preferred_components)
                tmpl_cr2 = _template_from_file(f_cr2, data_component, preferred_components)
                tmpl_cr3 = _template_from_file(f_cr3, data_component, preferred_components)
            except Exception:
                f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()
                continue

            # Try primary first (single hist), else fallback sum
            def build_region_hist(f: ROOT.TFile, tmpl: ROOT.TH1, suffix: str) -> ROOT.TH1:
                # primary: take first existing among primary_names
                for hname in primary_names:
                    h = get_hist_maybe(f, hname)
                    if h:
                        return h
                # fallback sum
                if fallback_names:
                    hsum = None
                    for hname in fallback_names:
                        h = get_hist_maybe(f, hname)
                        if not h:
                            h = make_zero_like(tmpl, f"{hname}_{suffix}_zero")
                        hsum = add_hists(hsum, h)
                    return hsum
                # default: zeros like template
                return make_zero_like(tmpl, f"{templ.key}_{suffix}_zero")

            hs  = build_region_hist(f_sr,  tmpl_sr,  "sr")
            hc1 = build_region_hist(f_cr1, tmpl_cr1, "cr1")
            hc2 = build_region_hist(f_cr2, tmpl_cr2, "cr2")
            hc3 = build_region_hist(f_cr3, tmpl_cr3, "cr3")

            if hs.GetNbinsX()  != max_sr:  hs  = pad_histogram(hs,  max_sr)
            if hc1.GetNbinsX() != max_cr1: hc1 = pad_histogram(hc1, max_cr1)
            if hc2.GetNbinsX() != max_cr2: hc2 = pad_histogram(hc2, max_cr2)
            if hc3.GetNbinsX() != max_cr3: hc3 = pad_histogram(hc3, max_cr3)

            h_concat = concat_hists_with_labels(hs, hc1, hc2, hc3, f"{templ.key}_concat_{mass}_{era}_{flav}")

            if grand is None:
                grand = h_concat.Clone(f"{templ.key}_{mass}_grand")
                grand.SetDirectory(0)
            else:
                grand.Add(h_concat)

            f_sr.Close(); f_cr1.Close(); f_cr2.Close(); f_cr3.Close()

    return grand

# -------------------------------------------------------------------------
# Signal scaling + styling for plotting
# -------------------------------------------------------------------------

def _nice_scale_factor(target: float) -> float:
    """
    Round *up* to a human-friendly scale factor.
    Example: 260 -> 300, 5500 -> 6000, 820 -> 1000, 1.2 -> 2
    """
    if target <= 1.0:
        return 1.0
    exp = int(math.floor(math.log10(target)))
    base = target / (10 ** exp)
    # include 6 to allow 6000-like factors
    nice = [1.0, 2.0, 3.0, 5.0, 6.0, 10.0]
    for n in nice:
        if base <= n:
            return n * (10 ** exp)
    return 10.0 * (10 ** exp)

def _pow2_scale_factor(target: float) -> float:
    if target <= 1.0:
        return 1.0
    n = int(math.ceil(math.log(target, 2)))
    return float(2 ** n)

def _pow10_scale_factor(target: float) -> float:
    if target <= 1.0:
        return 1.0
    n = int(math.ceil(math.log10(target)))
    return float(10 ** n)

def autoscale_signal(
    hsig: ROOT.TH1,
    h_bkg_total: ROOT.TH1,
    sr_bins: int,
    target_frac: float = 0.20,
    mode: str = "nice",
    max_scale: float = 1e6,
) -> float:
    """
    Compute an *additional* scale factor for drawing.

    Strategy:
      - Look only in SR bins [1..sr_bins]
      - Find signal peak bin, compare to background content at that bin
      - Choose scale to bring signal peak to ~ target_frac * bkg_at_peak
      - Round scale up to a "nice" value (or pow2/pow10)
      - Never scale down below 1
    """
    if not (hsig and h_bkg_total):
        return 1.0
    if sr_bins <= 0:
        return 1.0

    # Peak in SR
    sig_max = 0.0
    sig_peak_bin = 1
    for i in range(1, min(sr_bins, hsig.GetNbinsX()) + 1):
        v = hsig.GetBinContent(i)
        if v > sig_max:
            sig_max = v
            sig_peak_bin = i
    if sig_max <= 0:
        return 1.0

    bkg_at_peak = h_bkg_total.GetBinContent(sig_peak_bin) if sig_peak_bin <= h_bkg_total.GetNbinsX() else 0.0
    if bkg_at_peak <= 0:
        # fallback to bkg max in SR
        bkg_at_peak = 0.0
        for i in range(1, min(sr_bins, h_bkg_total.GetNbinsX()) + 1):
            bkg_at_peak = max(bkg_at_peak, h_bkg_total.GetBinContent(i))
    if bkg_at_peak <= 0:
        return 1.0

    desired = (target_frac * bkg_at_peak) / sig_max
    desired = max(1.0, desired)
    desired = min(max_scale, desired)

    mode = (mode or "nice").lower()
    if mode == "pow2":
        return min(max_scale, _pow2_scale_factor(desired))
    if mode == "pow10":
        return min(max_scale, _pow10_scale_factor(desired))
    # default: nice
    return min(max_scale, _nice_scale_factor(desired))

@dataclass
class SignalDrawItem:
    key: str
    mass: str
    hist: ROOT.TH1
    color: int
    line_style: int
    line_width: int
    # Extra scale applied by this plotting script (for visibility)
    scale: float
    # Pre-scale already applied in the input histograms (for legend annotation only)
    base_scale: float
    label: str
    draw_sr_only: bool = True

def style_and_scale_signal(h: ROOT.TH1, color: int, line_style: int, line_width: int, scale: float) -> ROOT.TH1:
    out = h.Clone(f"{h.GetName()}_draw")
    out.SetDirectory(0)
    if scale != 1.0:
        out.Scale(scale)
    out.SetLineWidth(line_width)
    out.SetLineStyle(line_style)
    out.SetLineColor(color)
    out.SetFillStyle(0)
    return out

# -------------------------------------------------------------------------
# Plotting
# -------------------------------------------------------------------------

def make_stack_plot(
    hists_bkg: List[Tuple[str, ROOT.TH1]],
    h_data: Optional[ROOT.TH1],
    signals: List[SignalDrawItem],
    outpath: str,
    title: str,
    lumi: str,
    region_edges: List[int],
    region_titles: List[str],
    sr_bins: int,
    logy: bool = True,
    g_bkg_band_main: Optional[ROOT.TGraphAsymmErrors] = None,
    g_bkg_band_ratio: Optional[ROOT.TGraphAsymmErrors] = None,
    h_bkg_total_pre: Optional[ROOT.TH1] = None,
):
    """Draw stack + data + multiple signal overlays + (stat⊕syst) band."""
    lines_main = []
    texts_main = []
    keep_misc = []

    # Total background (use precomputed one if provided)
    if h_bkg_total_pre:
        h_bkg_total = h_bkg_total_pre.Clone("bkg_total_for_plot")
        h_bkg_total.SetDirectory(0)
    else:
        h_bkg_total = None
        for _, hb in hists_bkg:
            h_bkg_total = add_hists(h_bkg_total, hb)

    # Canvas / pads
    c = ROOT.TCanvas("c", "c", 2400, 950)
    pad_main  = ROOT.TPad("pad_main",  "pad_main",  0.00, 0.32, 0.84, 1.00)
    pad_ratio = ROOT.TPad("pad_ratio", "pad_ratio", 0.00, 0.00, 0.84, 0.30)
    pad_leg   = ROOT.TPad("pad_leg",   "pad_leg",   0.84, 0.32, 1.00, 1.00)

    for p in (pad_main, pad_leg, pad_ratio):
        p.SetFillStyle(0)

    pad_main.SetRightMargin(0.02)
    pad_main.SetTopMargin(max(0.13, pad_main.GetTopMargin()))
    pad_main.SetBottomMargin(0.02)
    pad_main.SetLeftMargin(0.12)

    pad_ratio.SetLeftMargin(0.12)
    pad_ratio.SetRightMargin(0.02)
    pad_ratio.SetTopMargin(0.08)
    pad_ratio.SetBottomMargin(0.32)

    pad_leg.SetLeftMargin(0.02)

    pad_main.Draw(); pad_leg.Draw(); pad_ratio.Draw()

    # MAIN pad
    pad_main.cd()
    pad_main.SetLogy(1 if logy else 0)

    stack = ROOT.THStack("stack", title)
    for label, hb in hists_bkg:
        color = COLORS_BKG.get(label, ROOT.kGray + 1)
        hb.SetLineColor(ROOT.kBlack)
        hb.SetFillColor(color)
        hb.SetFillStyle(1001)
        stack.Add(hb, "HIST")

    stack.Draw("HIST")
    stack.GetYaxis().SetTitle("Events/bin")
    stack.GetXaxis().SetLabelSize(0)

    stack.GetYaxis().SetTitleOffset(0.95)
    stack.GetYaxis().SetTitleSize(0.055)
    stack.GetYaxis().SetLabelSize(0.045)

    # Y range (log-safe) using SR maxima (signals drawn in SR only by default)
    ymax_stack = stack.GetMaximum()
    ymax_data  = h_data.GetMaximum() if h_data else 0.0

    ymax_sig = 0.0
    for s in signals:
        if not s.hist:
            continue
        # SR-only max after scaling
        smax = 0.0
        for i in range(1, min(sr_bins, s.hist.GetNbinsX()) + 1):
            smax = max(smax, s.hist.GetBinContent(i))
        ymax_sig = max(ymax_sig, smax)

    y_max = max(ymax_stack, ymax_data, ymax_sig, 1.0)
    if logy:
        # Add generous headroom so region titles/labels never collide with the tallest bins
        stack.SetMaximum(200.0 * y_max)
        stack.SetMinimum(1.0)
    else:
        stack.SetMaximum(1.25 * y_max)
        stack.SetMinimum(0.0)

    # Uncertainty band
    if g_bkg_band_main:
        g_bkg_band_main.Draw("E2 SAME")
        keep_misc.append(g_bkg_band_main)

    # Data
    if h_data:
        h_data.SetMarkerStyle(20)
        h_data.SetMarkerSize(1.0)
        h_data.SetLineColor(ROOT.kBlack)
        h_data.Draw("PE SAME")

    # Signals
    split_bin = region_edges[0] if region_edges else sr_bins
    for s in signals:
        if not s.hist:
            continue
        if s.draw_sr_only:
            h_sr = s.hist.Clone(f"{s.hist.GetName()}_sr_range")
            h_sr.SetDirectory(0)
            h_sr.GetXaxis().SetRange(1, split_bin)
            h_sr.Draw("HIST SAME")
            keep_misc.append(h_sr)
        else:
            s.hist.Draw("HIST SAME")
            keep_misc.append(s.hist)

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
    lumi_txt.SetTextSize(0.60 * t); lumi_txt.SetTextAlign(31)
    x_right = 1 - r
    y_top   = 1 - t + 0.20 * t
    lumi_txt.DrawLatex(x_right, y_top, _format_lumi_text(lumi))

    pad_main.SetTicks(1, 1)

    # Region separator lines + titles
    pad_main.Update()

    # --- Robust SR/CR separator lines (drawn in NDC) ---
    #
    # We want the separator lines to touch the plot frame *exactly* in every region.
    # In log-y, relying on user-y coordinates (or GetUymin/GetUymax) can lead to
    # "floating" lines that do not reach the top/bottom.
    #
    # Solution: draw the vertical separators in pad-NDC coordinates, spanning
    # [bottomMargin .. 1-topMargin]. This is independent of log/lin scaling.
    xmin = stack.GetXaxis().GetXmin()
    xmax = stack.GetXaxis().GetXmax()
    L = pad_main.GetLeftMargin()
    R = pad_main.GetRightMargin()

    def x_user_to_ndc(xu: float) -> float:
        if xmax == xmin:
            return 0.5
        return L + (xu - xmin) / (xmax - xmin) * (1.0 - L - R)

    y1_ndc = pad_main.GetBottomMargin()
    y2_ndc = 1.0 - pad_main.GetTopMargin()

    # Expand the mask vertically by a few pixels to fully cover thick signal lines
    extra_py = 2  # <- typically 2~6 pixels
    try:
        pad_h_px = float(getattr(pad_main, "GetWh", lambda: 0)() or 0)
        if pad_h_px > 0:
            dy_ndc = extra_py / pad_h_px
            y1_ndc = max(0.0, y1_ndc - dy_ndc)
            y2_ndc = min(1.0, y2_ndc + dy_ndc)
    except Exception:
        pass

    def _draw_vline_ndc(x_ndc: float, color: int, style: int, width: int) -> Optional[ROOT.TLine]:
        # Preferred: DrawLineNDC exists in most ROOT versions.
        ln = None
        try:
            tmp = ROOT.TLine()
            if hasattr(tmp, "DrawLineNDC"):
                ln = tmp.DrawLineNDC(x_ndc, y1_ndc, x_ndc, y2_ndc)
        except Exception:
            ln = None

        if not ln:
            # Fallback: create a TLine and mark as NDC if supported
            try:
                ln = ROOT.TLine(x_ndc, y1_ndc, x_ndc, y2_ndc)
                if hasattr(ln, "SetNDC"):
                    ln.SetNDC(True)
                ln.Draw()
            except Exception:
                return None

        ln.SetLineColor(color)
        ln.SetLineStyle(style)
        ln.SetLineWidth(width)
        return ln

    # First edge: "double" black line with a clean gap
    mask_objs = []
    if region_edges:
        x_split_user = region_edges[0] + 0.5
        dx_user = 0.08

        x_split_ndc = x_user_to_ndc(x_split_user)
        x_lo_ndc    = x_user_to_ndc(x_split_user - dx_user)
        x_hi_ndc    = x_user_to_ndc(x_split_user + dx_user)

        # Mask the gap region with a filled rectangle in NDC so the stacked fills/lines
        # do not show through between the two black separator lines.
        #
        # IMPORTANT: do *not* use a fixed pixel-width "eraser" line here. The bin-to-pixel
        # mapping changes with the total number of bins, so a fixed pixel width can be
        # too narrow (histogram leaks into the gap) in some regions and too wide (white
        # covers outside the black lines) in others. Using an NDC rectangle whose x-range
        # is defined by the same (x_split ± dx_user) mapping as the black lines makes the
        # gap stable across SR1/SR2/SR3.
        eraser_color = int(getattr(pad_main, "GetFillColor", lambda: 0)() or 0)

        x1_ndc = min(x_lo_ndc, x_hi_ndc)
        x2_ndc = max(x_lo_ndc, x_hi_ndc)

        # Expand by ~0.5 pixel in NDC to avoid 1-pixel leaks due to rounding.
        eps_ndc = 0.0
        try:
            ww = float(getattr(pad_main, "GetWw", lambda: 0)() or 0)
            if ww > 0:
                eps_ndc = 0.5 / ww
        except Exception:
            eps_ndc = 0.0003
        if eps_ndc > 0.0:
            x1_ndc = max(0.0, x1_ndc - eps_ndc)
            x2_ndc = min(1.0, x2_ndc + eps_ndc)


        gap_box = None
        # Prefer TPave (supports NDC option robustly), then TPaveText, then TBox with SetNDC if available.
        try:
            gap_box = ROOT.TPave(x1_ndc, y1_ndc, x2_ndc, y2_ndc, 0, "NDC")
        except Exception:
            try:
                gap_box = ROOT.TPaveText(x1_ndc, y1_ndc, x2_ndc, y2_ndc, "NDC")
            except Exception:
                try:
                    gap_box = ROOT.TBox(x1_ndc, y1_ndc, x2_ndc, y2_ndc)
                    if hasattr(gap_box, "SetNDC"):
                        gap_box.SetNDC(True)
                except Exception:
                    gap_box = None

        if gap_box:
            gap_box.SetFillColor(eraser_color)
            gap_box.SetFillStyle(1001)
            gap_box.SetLineColor(eraser_color)
            if hasattr(gap_box, "SetBorderSize"):
                gap_box.SetBorderSize(0)
            if hasattr(gap_box, "SetShadowColor"):
                gap_box.SetShadowColor(0)
            gap_box.Draw("same")
            mask_objs.append(gap_box)

        ln1 = _draw_vline_ndc(x_lo_ndc, color=ROOT.kBlack, style=1, width=3)
        ln2 = _draw_vline_ndc(x_hi_ndc, color=ROOT.kBlack, style=1, width=3)
        if ln1:
            lines_main.append(ln1)
        if ln2:
            lines_main.append(ln2)

        # Other edges: single dashed grey line
        for edge in region_edges[1:]:
            x_ndc = x_user_to_ndc(edge + 0.5)
            ln = _draw_vline_ndc(x_ndc, color=ROOT.kGray + 1, style=9, width=3)
            if ln:
                lines_main.append(ln)

    pad_main.Modified()
    pad_main.Update()

    # Place region titles in top band
    t = pad_main.GetTopMargin()
    y_label_ndc = 1.0 - t - 0.2
    text_size_ndc = 0.045

    starts = [0] + region_edges
    ends   = region_edges + [h_bkg_total.GetNbinsX() if h_bkg_total else (region_edges[-1] if region_edges else 0)]

    last_label_dx = -0.010
    for idx, (lab, a, b) in enumerate(zip(region_titles, starts, ends)):
        x_user_center = 0.5 * (a + 1 + b)
        x_ndc = x_user_to_ndc(x_user_center)
        if idx == len(region_titles) - 1:
            x_ndc += last_label_dx
        tl = ROOT.TLatex()
        tl.SetNDC(True)
        tl.SetTextFont(62)
        tl.SetTextSize(text_size_ndc)
        tl.SetTextAlign(23)
        tl.DrawLatex(x_ndc, y_label_ndc, lab)
        texts_main.append(tl)

    pad_main._keepalive = getattr(pad_main, "_keepalive", []) \
        + lines_main + texts_main + mask_objs + [cms, pre, lumi_txt] \
        + ([g_bkg_band_main] if g_bkg_band_main else [])

    # LEGEND pad
    pad_leg.cd()
    leg = ROOT.TLegend(0.10, 0.08, 0.95, 0.88)
    leg.SetBorderSize(0)
    leg.SetLineWidth(0)
    leg.SetLineColor(0)
    leg.SetFillStyle(0)
    leg.SetTextSize(0.055)

    def _add_fill_entry(leg: ROOT.TLegend, obj, label: str):
        e = leg.AddEntry(obj, label, "f")
        e.SetLineWidth(0)
        e.SetLineColor(0)
        e.SetMarkerSize(0)
        return e

    def _add_line_entry(leg: ROOT.TLegend, obj, label: str):
        return leg.AddEntry(obj, label, "l")

    if h_data:
        leg.AddEntry(h_data, "Data", "pe")

    # Background labels
    for label, hb in hists_bkg:
        if label == "fake":
            _add_fill_entry(leg, hb, "Nonprompt")
        elif label == "cf":
            _add_fill_entry(leg, hb, "Charge MisID.")
        elif label == "mc_others":
            _add_fill_entry(leg, hb, "others")
        elif label == "ww":
            _add_fill_entry(leg, hb, "W^{#pm}W^{#pm}")
        elif label == "wz":
            _add_fill_entry(leg, hb, "WZ")
        elif label == "wz_ewk":
            _add_fill_entry(leg, hb, "WZ_EWK")
        elif label == "zz":
            _add_fill_entry(leg, hb, "ZZ")
        elif label == "zg":
            _add_fill_entry(leg, hb, "Z#gamma")
        else:
            _add_fill_entry(leg, hb, label)

    if g_bkg_band_main:
        _add_fill_entry(leg, g_bkg_band_main, "Bkg. unc.(stat #oplus syst)")

    def _fmt_scale(x: float) -> str:
        """Pretty formatting for legend scale factors."""
        if x == 0.0:
            return "0"
        if abs(x - round(x)) < 1e-9 and abs(x) < 1e12:
            return str(int(round(x)))
        ax = abs(x)
        # Use scientific notation for very small/large values
        if ax < 0.01 or ax >= 10000.0:
            ss = f"{x:.0e}"
            ss = ss.replace("e+0", "e").replace("e-0", "e-").replace("e+", "e")
            return ss
        return f"{x:g}"

    # Signals
    for s in signals:
        if not s.hist:
            continue
        # Legend shows: total scale = (input pre-scale) * (extra visual scale)
        total_scale = float(getattr(s, "base_scale", 1.0)) * float(getattr(s, "scale", 1.0))
        scale_txt = f" (x{_fmt_scale(total_scale)})" if abs(total_scale - 1.0) > 1e-12 else ""
        _add_line_entry(leg, s.hist, f"{s.label}{scale_txt}")

    leg.Draw()

    # RATIO pad (Prediction / Data)
    pad_ratio.cd()
    if h_data and h_bkg_total and h_data.Integral() > 0:
        ratio = h_bkg_total.Clone("ratio_bkg_over_data")
        ratio.SetStats(0)
        ratio.SetLineWidth(1)
        ratio.SetLineColor(ROOT.kBlack)
        ratio.SetMarkerStyle(20)
        ratio.SetMarkerSize(1.0)

        ratio.Divide(h_data)
        ratio.SetTitle("")
        ratio.GetYaxis().SetRangeUser(0.5, 1.5)
        ratio.GetYaxis().SetTitle("#frac{Prediction}{Data}")
        ratio.GetYaxis().SetNdivisions(505)
        ratio.GetYaxis().SetTitleSize(0.10)
        ratio.GetYaxis().SetLabelSize(0.09)
        ratio.GetYaxis().SetTitleOffset(0.55)
        ratio.GetXaxis().SetLabelSize(0.09)
        ratio.GetXaxis().SetTitleSize(0.12)
        ratio.GetXaxis().SetTitleOffset(1.0)
        # Horizontal bin labels (more readable than vertical)
        ratio.GetXaxis().LabelsOption("h")
        ratio.GetXaxis().SetLabelOffset(0.02)
        # If there are many bins, a small tilt helps readability more than shrinking the font.
        if ratio.GetNbinsX() >= 40:
            ratio.GetXaxis().SetLabelAngle(25)

        if g_bkg_band_ratio:
            ratio.Draw("E")
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

    # Save
    os.makedirs(os.path.dirname(outpath), exist_ok=True)
    c._keepalive = getattr(c, "_keepalive", []) + [obj for obj in keep_misc if obj]
    c.SaveAs(outpath)
    root, ext = os.path.splitext(outpath)
    if ext.lower() != ".pdf":
        c.SaveAs(root + ".pdf")

# -------------------------------------------------------------------------
# Main
# -------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser()

    ap.add_argument("--base", required=True, help="Base directory containing era/subdir/rootfiles")

    # Region selection
    ap.add_argument(
        "--region",
        default="",
        help="Region preset key: SR1/SR2/SR3 (case-insensitive). "
             "SR3BDT is treated as an alias of SR3 (inputs are stored under sr3).",
    )
    ap.add_argument("--sr-subdir", default="", help="Override SR subdir (relative to base/era).")
    ap.add_argument("--cr1-subdir", default="", help="Override CR1 subdir (InvBJet).")
    ap.add_argument("--cr2-subdir", default="", help="Override CR2 subdir (InvMET).")
    ap.add_argument("--cr3-subdir", default="", help="Override CR3 subdir (WZ).")

    # Masses
    ap.add_argument("--mass", default="", choices=MASS_CHOICES, help="Single mass token (legacy).")
    ap.add_argument("--masses", nargs="*", default=None, choices=MASS_CHOICES,
                    help="Multiple mass tokens for HNL signals (e.g. --masses M500 M1000).")

    ap.add_argument("--bkg-mass", default="", choices=MASS_CHOICES,
                    help="Which mass file to use for backgrounds/data. If empty, derived automatically.")

    # Input file naming
    ap.add_argument("--file-pattern", default="{mass}_{flavour}_card_input.root",
                    help="ROOT file pattern under each subdir (supports {mass}, {flavour}).")

    # Components (background + data)
    ap.add_argument("--bkg-components", nargs="+", default=BKG_COMPONENTS_DEFAULT,
                    help="Background components to read/stack.")
    ap.add_argument("--data-component", default=DATA_COMPONENT_DEFAULT,
                    help="Data histogram name (usually data_obs).")

    # Signals
    ap.add_argument("--signals", nargs="+", default=["DYVBF", "SSWW"],
                    help="Signal keys to overlay. Choices: " + ", ".join(SIGNAL_LIBRARY.keys()))
    ap.add_argument(
        "--hnl-color",
        default="kRed",
        help="Default color for HNL signals (ROOT expr/int/#RRGGBB). Examples: kRed+2, 632, #1f77b4",
    )
    ap.add_argument(
        "--weinberg-color",
        default="kBlue",
        help="Default color for Weinberg signal (ROOT expr/int/#RRGGBB).",
    )
    ap.add_argument(
        "--sig-color-map",
        type=json.loads,
        default=None,
        help=(
            "Optional JSON map overriding signal colors. Keys can be 'SIG' or 'SIG:MASS'. "
            "Values can be ROOT color expr/int/#RRGGBB. "
            "Example: '{\"DYVBF\":\"kRed+1\", \"SSWW\":\"kGreen+2\", \"Weinberg\":\"kBlack\"}'"
        ),
    )
    ap.add_argument("--sig-line-styles", default="11,9,2,7",
                    help="Comma-separated ROOT line styles cycled for multiple signals/masses.")
    ap.add_argument("--sig-line-width", type=int, default=3, help="Signal line width.")

    ap.add_argument("--sig-autoscale", action="store_true", default=True,
                    help="Enable automatic signal scaling for visibility (default: on).")
    ap.add_argument("--no-sig-autoscale", dest="sig_autoscale", action="store_false",
                    help="Disable signal autoscaling (use scale=1 unless overridden).")
    ap.add_argument("--sig-target-frac", type=float, default=0.20,
                    help="Target signal peak fraction of background at signal peak bin (SR only).")
    ap.add_argument("--sig-scale-mode", choices=["nice", "pow2", "pow10"], default="nice",
                    help="Rounding mode for auto-scale.")
    ap.add_argument("--sig-max-scale", type=float, default=1e6, help="Max auto scale factor.")
    ap.add_argument("--sig-scale-map", type=json.loads, default=None,
                    help="Optional JSON map overriding extra scales per signal key. "
                         "Example: '{\"Weinberg\":6000, \"SSWW\":1000}'")

    # Plot cosmetics
    ap.add_argument("--outfile", default="", help="Output ROOT file (grand hists). If empty, auto-named.")
    ap.add_argument("--outplot", default="", help="Output plot PNG path. If empty, auto-named.")
    ap.add_argument("--title", default="", help="Plot title (THStack title).")
    ap.add_argument("--lumi", default="137.6 fb-1 (13 TeV)", help="Lumi label (TLatex).")

    ap.add_argument("--logy", dest="logy", action="store_true", default=True,
                    help="Use log scale on the main pad y-axis (default: on).")
    ap.add_argument("--no-logy", dest="logy", action="store_false",
                    help="Disable log scale on the main pad y-axis (linear y).")

    ap.add_argument("--titles", default="",
                    help="Override region titles as 'SR,CR1,CR2,CR3' (comma-separated).")

    # Systematics band options
    ap.add_argument("--syst-scan-comp", default="wz",
                    help="Component prefix to scan for available systematic shapes (default: wz).")
    ap.add_argument("--lumi-norm-syst", type=float, default=0.02, help="Relative lumi norm syst (0 disables).")
    ap.add_argument("--fake-norm-syst", type=json.loads, default=DEFAULT_FAKE,
                    help="Relative fake norm syst by flavour JSON (null disables).")
    ap.add_argument("--cf-norm-syst", type=float, default=0.17, help="Relative cf norm syst (0 disables).")

    ap.add_argument("--dump-unc", action="store_true", help="Dump per-bin uncertainty breakdown (optional).")
    ap.add_argument("--dump-range", default="", help="Optional bin range like '1:40'.")
    ap.add_argument("--dump-tsv", default="", help="Optional TSV output path.")
    ap.add_argument("--dump-max-sources", type=int, default=0, help="Limit syst columns in dump (0=all).")

    args = ap.parse_args()

    # -------------------------
    # Resolve region preset
    # -------------------------
    region_key_raw = _norm_region_key(args.region)
    region_key = REGION_ALIASES.get(region_key_raw, region_key_raw)
    if region_key_raw and region_key_raw != region_key:
        print(f"[INFO] Region '{args.region}' is treated as alias '{region_key}'.")

    preset = REGION_PRESETS.get(region_key) if region_key else None

    # If no explicit region, try infer from sr_subdir
    if not preset and args.sr_subdir:
        inferred_raw = _norm_region_key(args.sr_subdir)
        inferred = REGION_ALIASES.get(inferred_raw, inferred_raw)
        preset = REGION_PRESETS.get(inferred)

    # Use preset defaults, then apply explicit overrides
    sr_subdir  = args.sr_subdir  or (preset.sr_subdir  if preset else "sr3")
    cr1_subdir = args.cr1_subdir or (preset.cr1_subdir if preset else "cr3_InvBJet")
    cr2_subdir = args.cr2_subdir or (preset.cr2_subdir if preset else "cr3_InvMET")
    cr3_subdir = args.cr3_subdir or (preset.cr3_subdir if preset else "wz_cr3")

    # If the user did not override subdir names explicitly, confirm that the chosen
    # subdirectories exist on disk (fall back to the first candidate otherwise).
    if not args.sr_subdir:
        sr_subdir = _resolve_existing_subdir(args.base, [sr_subdir])
    if not args.cr1_subdir:
        cr1_subdir = _resolve_existing_subdir(args.base, [cr1_subdir])
    if not args.cr2_subdir:
        cr2_subdir = _resolve_existing_subdir(args.base, [cr2_subdir])
    if not args.cr3_subdir:
        cr3_subdir = _resolve_existing_subdir(args.base, [cr3_subdir])

    sr_title = preset.sr_title if preset else (f"{sr_subdir}" if sr_subdir else "SR")

    region_titles = infer_region_titles(sr_title, cr1_subdir, cr2_subdir, cr3_subdir)
    if args.titles:
        parts = [p.strip() for p in args.titles.split(",")]
        if len(parts) == 4 and all(parts):
            region_titles = parts

    # -------------------------
    # Resolve masses and plotting jobs
    # -------------------------
    user_masses = None
    if args.masses is not None:
        user_masses = list(args.masses)
    elif args.mass:
        user_masses = [args.mass]
    else:
        user_masses = []

    # Filter out "Weinberg" from HNL masses; Weinberg is selected via --signals
    hnl_masses = [m for m in user_masses if m != "Weinberg"]

    # Resolve signal list (validate keys)
    signals_req = []
    for s in args.signals:
        if s not in SIGNAL_LIBRARY:
            raise ValueError(f"Unknown signal key '{s}'. Allowed: {list(SIGNAL_LIBRARY.keys())}")
        signals_req.append(s)

    want_weinberg = ("Weinberg" in signals_req)

    # Determine background mass ("base mass" used for background/data inputs)
    bkg_mass_forced = bool(args.bkg_mass)
    if args.bkg_mass:
        bkg_mass = args.bkg_mass
    else:
        if hnl_masses:
            bkg_mass = hnl_masses[0]
        elif want_weinberg:
            bkg_mass = "Weinberg"
        else:
            raise ValueError("No mass provided. Use --mass or --masses, or include Weinberg in --signals.")

    # Determine per-plot jobs
    # Rules in this analysis:
    #   - SR1: mass-dependent binning -> never overlay different masses
    #   - SR3: low mass (<=500 GeV) is mass-dependent; high mass (>500 GeV) and Weinberg share binning
    #   - SR2: mass-independent -> overlay allowed
    plot_jobs: List[Dict] = []

    region_for_logic = preset.key if preset else (region_key or "")
    region_for_logic = region_for_logic or _norm_region_key(sr_subdir)
    is_sr1_like = (_norm_region_key(region_for_logic) == "sr1") or (_norm_region_key(sr_subdir) == "sr1")
    is_sr3_like = (_norm_region_key(region_for_logic) == "sr3") or (_norm_region_key(sr_subdir) == "sr3")

    if is_sr1_like:
        if len(hnl_masses) > 1:
            print("[INFO] SR1 has mass-dependent binning; producing one plot per HNL mass instead of overlay.")
            for m in hnl_masses:
                plot_jobs.append({"bkg_mass": m, "hnl_masses": [m]})
        else:
            plot_jobs.append({"bkg_mass": bkg_mass, "hnl_masses": hnl_masses[:]})

    elif is_sr3_like:
        low_masses = [m for m in hnl_masses if _is_sr3_lowmass_mass_dependent(m)]
        high_masses = [m for m in hnl_masses if m not in low_masses]

        if len(low_masses) > 1:
            print("[INFO] SR3 low-mass (<=500 GeV) has mass-dependent binning; producing one plot per low mass instead of overlay.")

        # One job per low mass
        for m in low_masses:
            plot_jobs.append({"bkg_mass": m, "hnl_masses": [m]})

        # One combined job for the high-binning regime (high masses and/or Weinberg)
        if high_masses or want_weinberg:
            if bkg_mass_forced and not _is_sr3_lowmass_mass_dependent(bkg_mass):
                base_high = bkg_mass
            elif bkg_mass_forced and _is_sr3_lowmass_mass_dependent(bkg_mass) and (high_masses or want_weinberg):
                # User forced a low-mass bkg file but also requested high-binning signals.
                # Use a compatible base mass for the high-binning plot.
                print("[WARN] --bkg-mass is a low SR3 mass (<=500 GeV) but high-mass/Weinberg signals were requested; using a high-binning base mass for the high-mass plot.")
                base_high = high_masses[0] if high_masses else "Weinberg"
            else:
                base_high = high_masses[0] if high_masses else "Weinberg"
            plot_jobs.append({"bkg_mass": base_high, "hnl_masses": high_masses[:]})

        # If the user only provided a single low mass and nothing else, we already have the low-mass job.
        if not plot_jobs:
            plot_jobs.append({"bkg_mass": bkg_mass, "hnl_masses": hnl_masses[:]})

    else:
        # SR2 (and unknown regions): assume mass-independent binning
        plot_jobs.append({"bkg_mass": bkg_mass, "hnl_masses": hnl_masses[:]})

    # If no HNL masses, still make one job (for Weinberg-only plot)
    if not plot_jobs:
        plot_jobs = [{"bkg_mass": bkg_mass, "hnl_masses": []}]

    # Base tag for output naming
    base_tag = os.path.basename(os.path.normpath(args.base)).replace("/", "_").replace(":", "_")

    # Parse line styles list
    line_styles = []
    for tok in (args.sig_line_styles.split(",") if args.sig_line_styles else []):
        tok = tok.strip()
        if not tok:
            continue
        try:
            line_styles.append(int(tok))
        except Exception:
            pass
    if not line_styles:
        line_styles = [11, 9, 2, 7]

    # Default colors
    hnl_color = _parse_color(args.hnl_color, ROOT.kRed)
    weinberg_color = _parse_color(args.weinberg_color, ROOT.kBlue)

    sig_color_map: Dict[str, object] = args.sig_color_map if isinstance(args.sig_color_map, dict) else {}

    def resolve_signal_color(sig_key: str, mass: str) -> int:
        """Resolve signal color with priority:

        1) --sig-color-map with key "SIG:MASS"
        2) --sig-color-map with key "SIG"
        3) family defaults: --weinberg-color / --hnl-color
        """
        fallback = weinberg_color if sig_key == "Weinberg" else hnl_color
        if sig_color_map:
            k_mass = f"{sig_key}:{mass}"
            if k_mass in sig_color_map:
                return _parse_color(sig_color_map[k_mass], fallback)
            if sig_key in sig_color_map:
                return _parse_color(sig_color_map[sig_key], fallback)
        return fallback

    # Loop jobs
    for job in plot_jobs:
        job_bkg_mass = job["bkg_mass"]
        job_hnl_masses = job["hnl_masses"]

        # Job-specific SR3L/SR3H naming based on the base mass (for titles/output tags)
        job_sr_title = sr_title
        job_region_titles = list(region_titles)
        job_sr_name_tag = _norm_region_key(sr_subdir) or "sr"
        if _norm_region_key(sr_subdir) == "sr3":
            _m_int = _mass_to_int(job_bkg_mass)
            if (_m_int is not None) and (_m_int <= 500):
                job_sr_title = "SR3L"
                job_sr_name_tag = "sr3l"
            else:
                job_sr_title = "SR3H"
                job_sr_name_tag = "sr3h"
            job_region_titles[0] = job_sr_title


        # Per-job mass-dependent binning decision (SR3 low-mass is special)
        job_mass_dependent_binning = is_mass_dependent_binning(
            preset.key if preset else region_key,
            sr_subdir,
            job_bkg_mass,
        )

        # Only draw Weinberg when binning is compatible with the base mass
        draw_weinberg_this_job = want_weinberg and (not job_mass_dependent_binning or job_bkg_mass == "Weinberg")
        if want_weinberg and (not draw_weinberg_this_job) and job_bkg_mass != "Weinberg":
            print(f"[INFO] Mass-dependent binning mode for base mass {job_bkg_mass}: skipping Weinberg overlay (different binning).")

        # Choose which masses to scan for binning:
        #  - Always include background mass
        #  - Include all HNL masses we will draw (if binning is supposed to be mass-independent)
        #  - Include Weinberg mass file if we will draw Weinberg
        scan_masses = [job_bkg_mass]
        if not job_mass_dependent_binning:
            for m in job_hnl_masses:
                if m not in scan_masses:
                    scan_masses.append(m)
        if draw_weinberg_this_job and "Weinberg" not in scan_masses:
            scan_masses.append("Weinberg")

        # Pass 1: scan max bins
        max_sr, max_cr1, max_cr2, max_cr3, _bin_info = scan_max_bins(
            base=args.base,
            masses=scan_masses,
            sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
            file_pattern=args.file_pattern,
            bkg_components=args.bkg_components,
            data_component=args.data_component,
            verbose=True,
        )
        total_bins = max_sr + max_cr1 + max_cr2 + max_cr3
        region_edges = [max_sr, max_sr + max_cr1, max_sr + max_cr1 + max_cr2]

        # Prepare output paths
        out_dir = None
        if args.outplot:
            out_dir = os.path.dirname(args.outplot) or "."
        else:
            out_dir = f"summary_plot_{job_sr_name_tag}"
        os.makedirs(out_dir, exist_ok=True)

        # auto-name outputs per job
        # IMPORTANT: output names must contain exactly one mass token, determined by the
        # base mass used to read the background/data inputs (job_bkg_mass).
        mass_tag = job_bkg_mass

        need_suffix = (len(plot_jobs) > 1)

        if args.outfile:
            if need_suffix:
                _r, _e = os.path.splitext(args.outfile)
                _e = _e or ".root"
                outfile = f"{_r}_{mass_tag}{_e}"
            else:
                outfile = args.outfile
        else:
            outfile = os.path.join(out_dir, f"combined_{job_sr_name_tag}_{mass_tag}_{base_tag}.root")

        if args.outplot:
            if need_suffix:
                _r, _e = os.path.splitext(args.outplot)
                _e = _e or ".png"
                outplot = f"{_r}_{mass_tag}{_e}"
            else:
                outplot = args.outplot
        else:
            outplot = os.path.join(out_dir, f"combined_{job_sr_name_tag}_{mass_tag}_{base_tag}_all_eras_flavs.png")

        # PASS 2: build grand backgrounds + data
        outf = ROOT.TFile.Open(outfile, "RECREATE")
        if not outf or outf.IsZombie():
            raise IOError("Cannot create " + outfile)

        preferred_for_template = list(args.bkg_components)  # plus data is tried first internally

        grand: Dict[str, ROOT.TH1] = {}

        # Build and write per-component hists (background + data)
        for comp in list(args.bkg_components) + [args.data_component]:
            h_grand = build_grand_component(
                base=args.base,
                mass=job_bkg_mass,
                sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
                file_pattern=args.file_pattern,
                comp=comp,
                max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                data_component=args.data_component,
                preferred_components=preferred_for_template,
                write_per_era_flav=True,
                outf=outf,
            )
            if h_grand:
                grand[comp] = h_grand
                outf.cd()
                h_grand.Write()

        # Build bkg list for plot
        #
        # Analysis convention: plot should show (wz + wz_ewk) as a single stacked background.
        # They are kept separate in the inputs because some uncertainties (e.g. QCDscale)
        # apply only to 'wz'. The uncertainty band is still computed using the full component
        # list, so nothing is lost by merging only for the visible stack/legend.
        hists_bkg: List[Tuple[str, ROOT.TH1]] = []
        _wz_merged_done = False
        for comp in args.bkg_components:
            if comp in ("wz", "wz_ewk"):
                if _wz_merged_done:
                    continue
                # Merge any available WZ pieces into a single histogram labelled 'wz'
                h_wz = None
                for _c in ("wz", "wz_ewk"):
                    if _c not in grand:
                        continue
                    hh = grand[_c].Clone(f"{_c}_plot_tmp")
                    hh.SetDirectory(0)
                    hh.SetLineWidth(0)
                    if h_wz is None:
                        h_wz = hh.Clone("wz_plot")
                        h_wz.SetDirectory(0)
                    else:
                        h_wz.Add(hh)
                if h_wz:
                    hists_bkg.append(("wz", h_wz))
                _wz_merged_done = True
                continue

            if comp in grand:
                hh = grand[comp].Clone(comp + "_plot")
                hh.SetDirectory(0)
                hh.SetLineWidth(0)
                hists_bkg.append((comp, hh))

        h_data = grand.get(args.data_component)

        # total bkg for plot
        h_bkg_total_plot = None
        for _, hb in hists_bkg:
            h_bkg_total_plot = add_hists(h_bkg_total_plot, hb)

        # -------------------------
        # Systematics keys discovery (scan one representative file)
        # -------------------------
        syst_keys: List[str] = []
        for era in ERAS:
            for flav in FLAVOURS:
                try:
                    f_scan = open_root(build_path(args.base, era, sr_subdir, args.file_pattern, job_bkg_mass, flav))
                    # Scan syst keys from the requested component prefix, and (if applicable)
                    # also from wz_ewk so that merging WZ pieces doesn't accidentally drop
                    # any shape sources that exist only for one of them.
                    prefixes = [args.syst_scan_comp]
                    if args.syst_scan_comp == "wz" and ("wz_ewk" in args.bkg_components):
                        prefixes.append("wz_ewk")
                    if args.syst_scan_comp == "wz_ewk" and ("wz" in args.bkg_components):
                        prefixes.append("wz")

                    _keys = []
                    for pfx in prefixes:
                        _keys.extend(list_syst_keys_from_file(f_scan, comp_prefix=pfx))
                    syst_keys = sorted(set(_keys))
                    f_scan.Close()
                    if syst_keys:
                        break
                except Exception:
                    continue
            if syst_keys:
                break

        # Collapse to base keys (strip era suffix) and de-duplicate
        syst_keys = sorted({_strip_era_suffix(k) for k in syst_keys})

        # Add virtual norm systs
        if args.fake_norm_syst:
            syst_keys.append("FAKE_NORM")
        if args.cf_norm_syst and args.cf_norm_syst > 0.0:
            syst_keys.append("CF_NORM")
        if args.lumi_norm_syst and args.lumi_norm_syst > 0.0:
            syst_keys.append("LUMI_NORM")

        print("[INFO] Using {} syst sources: {}".format(len(syst_keys), ", ".join(syst_keys)))

        # Closure to build grand background for a given systematic
        components_bkg_for_syst = list(args.bkg_components)

        # Cache variations to avoid rebuilding multiple times
        var_cache: Dict[Tuple[str, str], Optional[ROOT.TH1]] = {}

        def build_var_hist_fn(skey: str, up_or_down: str) -> Optional[ROOT.TH1]:
            cache_key = (skey, up_or_down)
            if cache_key in var_cache:
                return var_cache[cache_key]

            # Virtual normalization-only sources
            if skey == "FAKE_NORM" and args.fake_norm_syst:
                scale_map_by_flavour = {
                    "EE":   {"fake": float(args.fake_norm_syst.get("EE",   0.0))},
                    "MuMu": {"fake": float(args.fake_norm_syst.get("MuMu", 0.0))},
                    "EMu":  {"fake": float(args.fake_norm_syst.get("EMu",  0.0))},
                }
                out = build_grand_bkg_scaled(
                    base=args.base, eras=ERAS, flavours=FLAVOURS, mass=job_bkg_mass,
                    sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
                    file_pattern=args.file_pattern, components_bkg=components_bkg_for_syst,
                    max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                    scale_map_by_flavour=scale_map_by_flavour,
                    up_or_down=up_or_down,
                )
                var_cache[cache_key] = out
                return out

            if skey == "CF_NORM" and args.cf_norm_syst and args.cf_norm_syst > 0.0:
                out = build_grand_bkg_scaled(
                    base=args.base, eras=ERAS, flavours=FLAVOURS, mass=job_bkg_mass,
                    sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
                    file_pattern=args.file_pattern, components_bkg=components_bkg_for_syst,
                    max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                    scale_map={"cf": float(args.cf_norm_syst)},
                    up_or_down=up_or_down,
                )
                var_cache[cache_key] = out
                return out

            if skey == "LUMI_NORM" and args.lumi_norm_syst and args.lumi_norm_syst > 0.0:
                scale_map = {comp: float(args.lumi_norm_syst) for comp in components_bkg_for_syst if comp not in ("fake", "cf")}
                out = build_grand_bkg_scaled(
                    base=args.base, eras=ERAS, flavours=FLAVOURS, mass=job_bkg_mass,
                    sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
                    file_pattern=args.file_pattern, components_bkg=components_bkg_for_syst,
                    max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                    scale_map=scale_map,
                    up_or_down=up_or_down,
                )
                var_cache[cache_key] = out
                return out

            # Default: shape-based
            out = build_grand_bkg_for_syst(
                base=args.base, eras=ERAS, flavours=FLAVOURS, mass=job_bkg_mass,
                sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
                file_pattern=args.file_pattern, components_bkg=components_bkg_for_syst,
                max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                syst_key=skey, up_or_down=up_or_down,
                sr_key_for_syst=(preset.key if preset else _norm_region_key(sr_subdir)),
            )
            var_cache[cache_key] = out
            return out

        # Make stat+syst bands
        g_bkg_band_main = None
        g_bkg_band_ratio = None
        if h_bkg_total_plot and syst_keys:
            g_bkg_band_main = make_bkg_unc_band_abs(h_bkg_total_plot, syst_keys, build_var_hist_fn)
            g_bkg_band_ratio = make_bkg_unc_band_ratio(h_bkg_total_plot, g_bkg_band_main)
            g_bkg_band_main.SetFillColor(ROOT.kGray + 1)
            g_bkg_band_main.SetLineColor(ROOT.kBlack)
            g_bkg_band_main.SetLineWidth(1)
            g_bkg_band_ratio.SetFillColor(ROOT.kGray + 1)
            g_bkg_band_ratio.SetLineColor(ROOT.kBlack)
            g_bkg_band_ratio.SetLineWidth(1)

        # Optional uncertainty dump (kept as-is conceptually)
        if args.dump_unc and h_bkg_total_plot and syst_keys:
            start_bin = 1
            end_bin = h_bkg_total_plot.GetNbinsX()
            if args.dump_range:
                try:
                    a, b = args.dump_range.split(":")
                    start_bin = int(a); end_bin = int(b)
                except Exception:
                    pass
            tsv = args.dump_tsv if args.dump_tsv else None
            max_sources = args.dump_max_sources if args.dump_max_sources and args.dump_max_sources > 0 else None
            dump_uncertainties_table(
                h_bkg_total_plot, syst_keys, build_var_hist_fn,
                start_bin=start_bin, end_bin=end_bin,
                tsv_path=tsv, max_sources=max_sources,
            )

        # -------------------------
        # Build signals to draw (grand sums)
        # -------------------------
        draw_items: List[SignalDrawItem] = []

        # Cycle styles across (signal key, mass)
        style_index = 0

        for sig_key in signals_req:
            templ = SIGNAL_LIBRARY[sig_key]

            if sig_key == "Weinberg":
                if not draw_weinberg_this_job:
                    continue
                # single mass point, read from Weinberg files
                hsig = build_grand_signal(
                    base=args.base, mass="Weinberg",
                    sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
                    file_pattern=args.file_pattern,
                    templ=templ,
                    max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                    data_component=args.data_component,
                    preferred_components=preferred_for_template,
                )
                if not hsig:
                    print("[WARN] Weinberg signal histogram not found (or files missing); skipping.")
                    continue

                # Skip empty signals (avoid legend clutter)
                _sr_int = 0.0
                for _i in range(1, min(max_sr, hsig.GetNbinsX()) + 1):
                    _sr_int += hsig.GetBinContent(_i)
                if _sr_int <= 0.0:
                    print("[INFO] Weinberg signal is empty in SR; skipping.")
                    continue

                # compute scale
                scale = 1.0
                if args.sig_scale_map and sig_key in args.sig_scale_map:
                    scale = float(args.sig_scale_map[sig_key])
                elif args.sig_autoscale and h_bkg_total_plot:
                    scale = autoscale_signal(
                        hsig, h_bkg_total_plot, sr_bins=max_sr,
                        target_frac=args.sig_target_frac,
                        mode=args.sig_scale_mode,
                        max_scale=args.sig_max_scale,
                    )

                line_style = line_styles[style_index % len(line_styles)]
                style_index += 1

                base_scale = signal_input_prescale(sig_key, "Weinberg")
                color = resolve_signal_color(sig_key, "Weinberg")
                h_draw = style_and_scale_signal(hsig, color=color, line_style=line_style, line_width=args.sig_line_width, scale=scale)

                draw_items.append(SignalDrawItem(
                    key=sig_key, mass="Weinberg", hist=h_draw,
                    color=color, line_style=line_style, line_width=args.sig_line_width,
                    scale=scale,
                    base_scale=base_scale,
                    label=templ.label_tmpl,
                    draw_sr_only=True,
                ))
                # write to output
                outf.cd()
                h_draw.Write(f"{sig_key}_Weinberg_draw")
                continue

            # HNL-like signals: build per requested HNL mass
            for m in job_hnl_masses:
                # Skip if user accidentally passed Weinberg as HNL mass
                if m == "Weinberg":
                    continue
                hsig = build_grand_signal(
                    base=args.base, mass=m,
                    sr_subdir=sr_subdir, cr1_subdir=cr1_subdir, cr2_subdir=cr2_subdir, cr3_subdir=cr3_subdir,
                    file_pattern=args.file_pattern,
                    templ=templ,
                    max_sr=max_sr, max_cr1=max_cr1, max_cr2=max_cr2, max_cr3=max_cr3,
                    data_component=args.data_component,
                    preferred_components=preferred_for_template,
                )
                if not hsig:
                    print(f"[WARN] Signal '{sig_key}' for mass {m} not found (or files missing); skipping.")
                    continue

                # Skip empty signals (avoid legend clutter)
                _sr_int = 0.0
                for _i in range(1, min(max_sr, hsig.GetNbinsX()) + 1):
                    _sr_int += hsig.GetBinContent(_i)
                if _sr_int <= 0.0:
                    print(f"[INFO] Signal '{sig_key}' mass {m} is empty in SR; skipping.")
                    continue

                scale = 1.0
                # Allow scale override by key, or by "key:mass"
                if args.sig_scale_map:
                    if f"{sig_key}:{m}" in args.sig_scale_map:
                        scale = float(args.sig_scale_map[f"{sig_key}:{m}"])
                    elif sig_key in args.sig_scale_map:
                        scale = float(args.sig_scale_map[sig_key])
                if scale == 1.0 and args.sig_autoscale and h_bkg_total_plot:
                    scale = autoscale_signal(
                        hsig, h_bkg_total_plot, sr_bins=max_sr,
                        target_frac=args.sig_target_frac,
                        mode=args.sig_scale_mode,
                        max_scale=args.sig_max_scale,
                    )

                line_style = line_styles[style_index % len(line_styles)]
                style_index += 1

                base_scale = signal_input_prescale(sig_key, m)
                color = resolve_signal_color(sig_key, m)
                h_draw = style_and_scale_signal(hsig, color=color, line_style=line_style, line_width=args.sig_line_width, scale=scale)

                mass_num = _mass_to_int(m)
                mass_label = str(mass_num) if mass_num is not None else m.replace("M", "")
                label = templ.label_tmpl.format(mass=mass_label)

                draw_items.append(SignalDrawItem(
                    key=sig_key, mass=m, hist=h_draw,
                    color=color, line_style=line_style, line_width=args.sig_line_width,
                    scale=scale,
                    base_scale=base_scale,
                    label=label,
                    draw_sr_only=True,
                ))
                outf.cd()
                h_draw.Write(f"{sig_key}_{m}_draw")

        # Make plot
        if hists_bkg and h_data:
            make_stack_plot(
                hists_bkg=hists_bkg,
                h_data=h_data,
                signals=draw_items,
                outpath=outplot,
                title=args.title,
                lumi=args.lumi,
                region_edges=region_edges,
                region_titles=job_region_titles,
                sr_bins=max_sr,
                g_bkg_band_main=g_bkg_band_main,
                g_bkg_band_ratio=g_bkg_band_ratio,
                h_bkg_total_pre=h_bkg_total_plot,
            )
        else:
            print("[WARN] Missing backgrounds or data; not plotting.")

        outf.Close()
        print("Wrote:", outfile)
        print("Plot: ", outplot, "(+ PDF)")
        print("SR/CR split lines drawn after bin:", region_edges)

# -------------------------------------------------------------------------
# Optional uncertainty table dumper (kept from original for convenience)
# -------------------------------------------------------------------------

def dump_uncertainties_table(h_bkg_total, syst_keys, build_var_hist_fn,
                             start_bin=1, end_bin=None, tsv_path=None, max_sources=None):
    if h_bkg_total is None:
        print("[WARN] No total background histogram; cannot dump.")
        return

    n = h_bkg_total.GetNbinsX()
    if end_bin is None:
        end_bin = n
    start_bin = max(1, int(start_bin))
    end_bin   = min(n, int(end_bin))

    var_up   = {k: build_var_hist_fn(k, "Up")   for k in syst_keys}
    var_down = {k: build_var_hist_fn(k, "Down") for k in syst_keys}

    show_systs = list(syst_keys)
    if max_sources is not None:
        show_systs = show_systs[:max_sources]

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

    ftsv = open(tsv_path, "w") if tsv_path else None
    if ftsv:
        ftsv.write(col_line + "\n")

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
                    f"{N:.6g}",
                    f"{up_val:.6g}",
                    f"{dn_val:.6g}",
                    f"{delta:.6g}"
                ])

        total = math.sqrt(stat * stat + syst2_sum)
        values.append(f"{total:.6g}")

        line = "\t".join(values)
        print(line)
        if ftsv:
            ftsv.write(line + "\n")

    if ftsv:
        ftsv.close()
        print("[INFO] Wrote TSV:", tsv_path)

if __name__ == "__main__":
    main()
