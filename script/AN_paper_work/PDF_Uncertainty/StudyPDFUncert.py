import os
import math
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetEndErrorSize(0)

# =========================================================
# config
# =========================================================
INDIR = "."
OUTDIR = "pdf_unc_ratio_plots"
os.makedirs(OUTDIR, exist_ok=True)

FILES = {
    "CCDY": {
        500:  "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_DYTypeI_DF_M500_private.root",
        1000: "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_DYTypeI_DF_M1000_private.root",
        3000: "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_DYTypeI_DF_M3000_private.root",
    },
    "WG": {
        500:  "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_VBFTypeI_DF_M500_private.root",
        1000: "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_VBFTypeI_DF_M1000_private.root",
        3000: "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_VBFTypeI_DF_M3000_private.root",
    },
    "SSWW": {
        500:  "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_SSWWTypeI_SF_M500_private.root",
        1000: "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_SSWWTypeI_SF_M1000_private.root",
        3000: "/data6/Users/jihkim/SKFlatOutput/Run2UltraLegacy_v3/HNL_SignalRegion_Plotter_PR183_PDFUncertStudy/2018/HNL_SignalRegion_Plotter_SSWWTypeI_SF_M3000_private.root",
    },
}

PROC_COLORS = {
    "CCDY":   ROOT.kAzure - 3,
    "WG":  ROOT.kMagenta - 4,
    "SSWW": ROOT.kOrange - 3,
}
PROC_MARKERS = {
    "CCDY": 20,
    "WG": 21,
    "SSWW": 22,
}

MASS_COLORS = {
    500:  ROOT.kBlack,
    1000: ROOT.kAzure + 2,
    3000: ROOT.kRed + 1,
}
MASS_MARKERS = {
    500: 20,
    1000: 21,
    3000: 22,
}
MASS_LINESTYLES = {
    500: 1,
    1000: 2,
    3000: 7,
}

# =========================================================
# display control table for 1D plots
# =========================================================
# compare_mode = "mass"  : compare masses within the same process
# compare_mode = "proc"  : compare processes at fixed mass
#
# IMPORTANT:
#   - The log on/off and y-range values below were kept from your current file.
#   - The new "xrange" field is for manual x-axis control of the 1D plots.
#   - If "xrange" is None, the code falls back to the default xrange from
#     PLOTS_COMMON / PLOTS_BY_PROC.
#   - I pre-filled example x-ranges only for the WG / SSWW Q-related mass plots:
#       ("WG",   "NoCut_Q")      -> (0.0, 1500.0)
#       ("WG",   "PDFUnc_vs_Q")  -> (0.0, 1500.0)
#       ("SSWW", "NoCut_Q")      -> (0.0, 1500.0)
#       ("SSWW", "PDFUnc_vs_Q")  -> (0.0, 1500.0)
#     Everything else is left as None.
#
# Format:
#   ("WG", "PDFUnc_vs_Q"): {
#       "top":   {"log": True,  "yrange": (1e-4, 2e-1)},
#       "ratio": {"log": True,  "yrange": (8e-1, 2e1)},
#       "xrange": (0.0, 1500.0),
#   }
#
# Typical usage:
#   1) Keep the log booleans as they are.
#   2) Tune only the yrange numbers by eye.
#   3) Add an xrange only when you want to override the default x-axis range.
#   4) Set xrange to None if you want the built-in default range.
#
def _default_display_entry():
    return {
        "top":   {"log": False, "yrange": None},
        "ratio": {"log": False, "yrange": None},
        "xrange": None,
    }

DISPLAY_SETTINGS = {
    "mass": {
        "default": {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": False, "yrange": None},
            "xrange": None,
        },

        # ---------- CCDY ----------
        ("CCDY", "NoCut_Q"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.001, 100.)},
            "xrange": None,
        },
        ("CCDY", "NoCut_X1"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.01, 100.)},
            "xrange": None,
        },
        ("CCDY", "NoCut_X2"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.01, 100.)},
            "xrange": None,
        },
        ("CCDY", "PDFUnc_vs_Q"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.01, 100.)},
            "xrange": None,
        },
        ("CCDY", "PDFUnc_vs_X1"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.01, 100.)},
            "xrange": None,
        },
        ("CCDY", "PDFUnc_vs_X2"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.01, 100.)},
            "xrange": None,
        },

        # ---------- WG ----------
        ("WG", "NoCut_Q"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.01, 100.)},
            "xrange": (0.0, 1500.0),
        },
        ("WG", "NoCut_X1"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.001, 100.)},
            "xrange": None,
        },
        ("WG", "NoCut_X2"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.001, 100.)},
            "xrange": None,
        },
        ("WG", "NoCut_Xphoton"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.001, 100.)},
            "xrange": None,
        },
        ("WG", "NoCut_Xparton"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.001, 100.)},
            "xrange": None,
        },
        ("WG", "PDFUnc_vs_Q"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 100.)},
            "xrange": (0.0, 1500.0),
        },
        ("WG", "PDFUnc_vs_X1"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 100.)},
            "xrange": None,
        },
        ("WG", "PDFUnc_vs_X2"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 100.)},
            "xrange": None,
        },
        ("WG", "PDFUnc_vs_Xphoton"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 100.)},
            "xrange": None,
        },
        ("WG", "PDFUnc_vs_Xparton"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 100.)},
            "xrange": None,
        },

        # ---------- SSWW ----------
        ("SSWW", "NoCut_Q"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": (0.0, 500.0),
        },
        ("SSWW", "NoCut_X1"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": None,
        },
        ("SSWW", "NoCut_X2"): {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": None,
        },
        ("SSWW", "PDFUnc_vs_Q"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": (0.0, 500.0),
        },
        ("SSWW", "PDFUnc_vs_X1"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": None,
        },
        ("SSWW", "PDFUnc_vs_X2"): {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": None,
        },
    },

    "proc": {
        "default": {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": False, "yrange": None},
            "xrange": None,
        },

        "NoCut_Q": {
            "top":   {"log": True, "yrange": None},
            "ratio": {"log": True, "yrange": None},
            "xrange": None,
        },
        "NoCut_X1": {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.0)},
            "xrange": None,
        },
        "NoCut_X2": {
            "top":   {"log": False, "yrange": None},
            "ratio": {"log": True, "yrange": (0.1, 10.0)},
            "xrange": None,
        },
        "PDFUnc_vs_Q": {
            "top":   {"log": True, "yrange": (0.01, 10.)},
            "ratio": {"log": True, "yrange": (0.1, 100.)},
            "xrange": None,
        },
        "PDFUnc_vs_X1": {
            "top":   {"log": True, "yrange": (0.01, 10.)},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": None,
        },
        "PDFUnc_vs_X2": {
            "top":   {"log": True, "yrange": (0.01, 10.)},
            "ratio": {"log": True, "yrange": (0.1, 10.)},
            "xrange": None,
        },
    },
}


def _merge_display_entry(base, override):
    out = {
        "top": dict(base.get("top", {})),
        "ratio": dict(base.get("ratio", {})),
        "xrange": base.get("xrange", None),
    }
    for pad in ["top", "ratio"]:
        if pad in override:
            out[pad].update(override[pad])
        out[pad].setdefault("log", False)
        out[pad].setdefault("yrange", None)
    if "xrange" in override:
        out["xrange"] = override["xrange"]
    return out

def get_display_setting(compare_mode, proc=None, hname=None):
    cfg = DISPLAY_SETTINGS.get(compare_mode, {})
    base = cfg.get("default", _default_display_entry())

    if compare_mode == "mass":
        override = cfg.get((proc, hname), {})
    elif compare_mode == "proc":
        override = cfg.get(hname, {})
    else:
        override = {}

    return _merge_display_entry(base, override)


# =========================================================
# 1D plot table
# =========================================================
# tuple format:
# (hname, is_profile, normalize_shape, xrange, xtitle, ytitle)
PLOTS_COMMON = [
    ("PDFUnc_vs_Q",   True,  False, (0.0, 3500.0), "PDF scale Q [GeV]", "Mean relative PDF uncertainty"),
    ("PDFUnc_vs_X1",  True,  False, (0.0, 1.0),    "Bjorken x_{1}",  "Mean relative PDF uncertainty"),
    ("PDFUnc_vs_X2",  True,  False, (0.0, 1.0),    "Bjorken x_{2}",  "Mean relative PDF uncertainty"),
    ("NoCut_Q",       False, True,  (0.0, 3500.0), "PDF scale Q [GeV]", "Normalized events"),
    ("NoCut_X1",      False, True,  (0.0, 1.0),    "Bjorken x_{1}",  "Normalized events"),
    ("NoCut_X2",      False, True,  (0.0, 1.0),    "Bjorken x_{2}",  "Normalized events"),
]

# Add process-specific 1D plots here.
# Same tuple format as PLOTS_COMMON.
PLOTS_BY_PROC = {
    "CCDY": [],
    "WG": [
        ("PDFUnc_vs_Xphoton", True,  False, (0.0, 1.0), "Bjorken x_{#gamma}", "Mean relative PDF uncertainty"),
        ("PDFUnc_vs_Xparton", True,  False, (0.0, 1.0), "Bjorken x_{q/g}",    "Mean relative PDF uncertainty"),
        ("NoCut_Xphoton",     False, True,  (0.0, 1.0), "Bjorken x_{#gamma}", "Normalized events"),
        ("NoCut_Xparton",     False, True,  (0.0, 1.0), "Bjorken x_{q/g}",    "Normalized events"),
    ],
    "SSWW": [],
}


def get_1d_plot_list_for_proc(proc):
    return list(PLOTS_COMMON) + list(PLOTS_BY_PROC.get(proc, []))

# =========================================================
# 2D plot table
# =========================================================
# tuple format:
# (hname, xrange, yrange, xtitle, ytitle, ztitle, drawopt)

# Common 2D plots that should exist for all processes.
# NOTE:
#   These require the analyzer/root files to actually contain the histograms.
#   If PDFUnc_X1_Q / NoCut_X1_Q etc. are not in the ROOT files yet,
#   these will just print [WARN] missing and be skipped.
PLOTS_2D_COMMON = [
    ("PDFUnc_X1_Q", (0.0, 1.0), (0.0, 5000.0), "Bjorken x_{1}", "PDF scale Q [GeV]", "Mean relative PDF uncertainty", "COLZ"),
    ("PDFUnc_X2_Q", (0.0, 1.0), (0.0, 5000.0), "Bjorken x_{2}", "PDF scale Q [GeV]", "Mean relative PDF uncertainty", "COLZ"),
    ("NoCut_X1_Q",  (0.0, 1.0), (0.0, 5000.0), "Bjorken x_{1}", "PDF scale Q [GeV]", "Entries", "COLZ"),
    ("NoCut_X2_Q",  (0.0, 1.0), (0.0, 5000.0), "Bjorken x_{2}", "PDF scale Q [GeV]", "Entries", "COLZ"),

    ("PDFUnc_X1_X2", (0.0, 1.0), (0.0, 1.0), "Bjorken x_{1}", "Bjorken x_{2}", "Mean relative PDF uncertainty", "COLZ"),
    ("NoCut_X1_X2",  (0.0, 1.0), (0.0, 1.0), "Bjorken x_{1}", "Bjorken x_{2}", "Entries", "COLZ"),
]

# Process-specific 2D plots
PLOTS_2D_BY_PROC = {
    "CCDY": [],
    "WG": [
        ("PDFUnc_Xphoton_Q", (0.0, 1.0), (0.0, 5000.0), "Bjorken x_{#gamma}", "PDF scale Q [GeV]", "Mean relative PDF uncertainty", "COLZ"),
        ("PDFUnc_Xparton_Q", (0.0, 1.0), (0.0, 5000.0), "Bjorken x_{q/g}",    "PDF scale Q [GeV]", "Mean relative PDF uncertainty", "COLZ"),
    ],
    "SSWW": [],
}

# 2D ratio plots: numerator / CCDY
# tuple format:
# (hname_num, hname_occ, xrange, yrange, xtitle, ytitle, ztitle, drawopt, ratio_mode)
#
# ratio_mode:
#   "content" : direct bin-content ratio
#               use this for PDFUnc_* profile maps
#   "shape"   : normalize each 2D histogram first, then take ratio
#               use this for NoCut_* event maps
PLOTS_2D_RATIO = [
    ("PDFUnc_X1_Q",   "NoCut_X1_Q",   (0., 1.0), (0., 1000.0), "Bjorken x_{1}", "PDF scale Q [GeV]", "PDF uncert. ratio to CCDY", "COLZ", "content"),
    ("PDFUnc_X2_Q",   "NoCut_X2_Q",   (0., 1.0), (0., 1000.0), "Bjorken x_{2}", "PDF scale Q [GeV]", "PDF uncert. ratio to CCDY", "COLZ", "content"),
    ("PDFUnc_X1_X2",  "NoCut_X1_X2",  (0., 1.0), (0., 1.0),    "Bjorken x_{1}", "Bjorken x_{2}",     "PDF uncert. ratio to CCDY", "COLZ", "content"),

    ("NoCut_X1_Q",    "NoCut_X1_Q",   (0., 1.0), (0., 1000.0), "Bjorken x_{1}", "PDF scale Q [GeV]", "Normalized event ratio to CCDY", "COLZ", "shape"),
    ("NoCut_X2_Q",    "NoCut_X2_Q",   (0., 1.0), (0., 1000.0), "Bjorken x_{2}", "PDF scale Q [GeV]", "Normalized event ratio to CCDY", "COLZ", "shape"),
    ("NoCut_X1_X2",   "NoCut_X1_X2",  (0., 1.0), (0., 1.0),    "Bjorken x_{1}", "Bjorken x_{2}",     "Normalized event ratio to CCDY", "COLZ", "shape"),
]

# 2D ratio quad plots:
# top row    = PDF uncertainty ratio
# bottom row = normalized event shape ratio
#
# tuple format:
# (hname_pdfunc, hname_nocut, hname_occ, xrange, yrange, xtitle, ytitle, drawopt)
PLOTS_2D_RATIO_QUAD = [
    (
        "PDFUnc_X1_Q", # 1st row
        "NoCut_X1_Q",  # 2nd row
        "NoCut_X1_Q",  # occupation check
        (0., 1.0),
        (0., 1000.0),
        "Bjorken x_{1}",
        "PDF scale Q [GeV]",
        "COLZ",
    ),
    (
        "PDFUnc_X2_Q",
        "NoCut_X2_Q",
        "NoCut_X2_Q",
        (0., 1.0),
        (0., 1000.0),
        "Bjorken x_{2}",
        "PDF scale Q [GeV]",
        "COLZ",
    ),
    (
        "PDFUnc_X1_X2",
        "NoCut_X1_X2",
        "NoCut_X1_X2",
        (0., 1.0),
        (0., 1.0),
        "Bjorken x_{1}",
        "Bjorken x_{2}",
        "COLZ",
    ),
]

def get_2d_plot_list_for_proc(proc):
    return list(PLOTS_2D_COMMON) + list(PLOTS_2D_BY_PROC.get(proc, []))

def _default_display_entry_2d():
    return {
        "xrange": None,
        "yrange": None,
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": False,
        "drawopt": None,
    }

DISPLAY_SETTINGS_2D_RAW = {
    "default": _default_display_entry_2d(),

    # Common Q maps
    ("CCDY", "PDFUnc_X1_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 3000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("CCDY", "PDFUnc_X2_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 3000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("CCDY", "NoCut_X1_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 3000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("CCDY", "NoCut_X2_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 3000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

    # X1-X2 maps (activate only if histograms exist)
    ("CCDY", "PDFUnc_X1_X2"): {
        "xrange": (0., 1.),
        "yrange": (0., 1.),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("CCDY", "NoCut_X1_X2"): {
        "xrange": (0., 1.),
        "yrange": (0., 1.),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

    # Common Q maps
    ("WG", "PDFUnc_X1_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("WG", "PDFUnc_X2_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("WG", "NoCut_X1_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("WG", "NoCut_X2_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

    # X1-X2 maps (activate only if histograms exist)
    ("WG", "PDFUnc_X1_X2"): {
        "xrange": (0., 1.),
        "yrange": (0., 1.),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("WG", "NoCut_X1_X2"): {
        "xrange": (0., 1.),
        "yrange": (0., 1.),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

    # Common Q maps
    ("SSWW", "PDFUnc_X1_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 500.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("SSWW", "PDFUnc_X2_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 500.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("SSWW", "NoCut_X1_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 500.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("SSWW", "NoCut_X2_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 500.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

    # X1-X2 maps (activate only if histograms exist)
    ("SSWW", "PDFUnc_X1_X2"): {
        "xrange": (0., 1.),
        "yrange": (0., 1.),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("SSWW", "NoCut_X1_X2"): {
        "xrange": (0., 1.),
        "yrange": (0., 1.),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

    # WG-specific photon / parton maps
    ("WG", "PDFUnc_Xphoton_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
    ("WG", "PDFUnc_Xparton_Q"): {
        "xrange": (0., 1.0),
        "yrange": (0., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },
}

DISPLAY_SETTINGS_2D_RATIO = {
    "default": {
        "xrange": None,
        "yrange": None,
        "zrange": (0.5, 2.0),
        "logx": False,
        "logy": False,
        "logz": False,
        "drawopt": None,
    },

    "PDFUnc_X1_Q": {
        "xrange": (0., 1.0),
        "yrange": (1., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": True,
        "logz": True,
        "drawopt": None,
    },
    "PDFUnc_X2_Q": {
        "xrange": (0., 1.0),
        "yrange": (1., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": True,
        "logz": True,
        "drawopt": None,
    },

    "PDFUnc_X1_X2": {
        "xrange": (0., 1.0),
        "yrange": (0., 1.0),
        #"zrange": (0.1, 10.),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

    "NoCut_X1_Q": {
        "xrange": (0.0, 1.0),
        "yrange": (1., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": True,
        "logz": True,
        "drawopt": None,
    },
    "NoCut_X2_Q": {
        "xrange": (0.0, 1.0),
        "yrange": (1., 1000.0),
        "zrange": None,
        "logx": False,
        "logy": True,
        "logz": True,
        "drawopt": None,
    },
    "NoCut_X1_X2": {
        "xrange": (0.0, 1.0),
        "yrange": (0.0, 1.0),
        "zrange": None,
        "logx": False,
        "logy": False,
        "logz": True,
        "drawopt": None,
    },

}

def _merge_display_entry_2d(base, override):
    out = dict(base)
    for key in ["xrange", "yrange", "zrange", "logx", "logy", "logz", "drawopt"]:
        if key in override:
            out[key] = override[key]
    return out

def get_display_setting_2d_raw(proc, hname):
    cfg = DISPLAY_SETTINGS_2D_RAW
    out = _merge_display_entry_2d(cfg.get("default", _default_display_entry_2d()), cfg.get(hname, {}))
    out = _merge_display_entry_2d(out, cfg.get((proc, hname), {}))
    return out

def get_display_setting_2d_ratio(hname):
    cfg = DISPLAY_SETTINGS_2D_RATIO
    out = _merge_display_entry_2d(cfg.get("default", _default_display_entry_2d()), cfg.get(hname, {}))
    return out

# =========================================================
# helpers
# =========================================================
def open_root(path):
    f = ROOT.TFile.Open(path)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open {path}")
    return f


def get_obj(f, hname):
    obj = f.Get(hname)
    if not obj:
        print(f"[WARN] missing: {hname} in {f.GetName()}")
        return None
    out = obj.Clone(f"{obj.GetName()}__{os.path.basename(f.GetName()).replace('.root','')}")
    if out.InheritsFrom("TH1"):
        out.SetDirectory(0)
    return out


def style_obj(h, color, marker=20, linestyle=1, width=3):
    h.SetLineColor(color)
    h.SetMarkerColor(color)
    h.SetLineWidth(width)
    h.SetLineStyle(linestyle)
    h.SetMarkerStyle(marker)
    h.SetMarkerSize(0.9)


def normalize_hist(h):
    if not h or h.InheritsFrom("TProfile"):
        return
    integral = h.Integral(1, h.GetNbinsX())
    if integral > 0:
        h.Scale(1.0 / integral)


def make_empty_hist_like(src, name):
    ax = src.GetXaxis()
    nb = ax.GetNbins()
    xbins = ax.GetXbins()
    if xbins and xbins.GetSize() > 0:
        h = ROOT.TH1D(name, "", nb, xbins.GetArray())
    else:
        h = ROOT.TH1D(name, "", nb, ax.GetXmin(), ax.GetXmax())
    h.SetDirectory(0)
    return h

def make_empty_2d_hist_like(src, name):
    ax = src.GetXaxis()
    ay = src.GetYaxis()

    nbx = ax.GetNbins()
    nby = ay.GetNbins()

    xbins = ax.GetXbins()
    ybins = ay.GetXbins()

    has_var_x = xbins and xbins.GetSize() > 0
    has_var_y = ybins and ybins.GetSize() > 0

    if has_var_x and has_var_y:
        h = ROOT.TH2D(name, "", nbx, xbins.GetArray(), nby, ybins.GetArray())
    elif has_var_x:
        h = ROOT.TH2D(name, "", nbx, xbins.GetArray(), nby, ay.GetXmin(), ay.GetXmax())
    elif has_var_y:
        h = ROOT.TH2D(name, "", nbx, ax.GetXmin(), ax.GetXmax(), nby, ybins.GetArray())
    else:
        h = ROOT.TH2D(name, "", nbx, ax.GetXmin(), ax.GetXmax(), nby, ay.GetXmin(), ay.GetXmax())

    h.SetDirectory(0)
    return h

def make_ratio(num, den, name):
    """
    ratio = num / den, manual error propagation
    works for TH1 / TProfile bin contents
    """
    r = make_empty_hist_like(num, name)
    nb = num.GetNbinsX()

    for ib in range(1, nb + 1):
        a = num.GetBinContent(ib)
        ea = num.GetBinError(ib)
        b = den.GetBinContent(ib)
        eb = den.GetBinError(ib)

        if b <= 0:
            r.SetBinContent(ib, 0.0)
            r.SetBinError(ib, 0.0)
            continue

        val = a / b
        if a > 0:
            rel2 = 0.0
            rel2 += (ea / a) ** 2
            rel2 += (eb / b) ** 2
            err = abs(val) * math.sqrt(rel2)
        else:
            err = ea / b

        r.SetBinContent(ib, val)
        r.SetBinError(ib, err)

    return r

def get_bin_range_for_axis(axis, user_range=None):
    if user_range is None:
        return 1, axis.GetNbins()

    lo, hi = user_range

    # Avoid selecting underflow/overflow.
    eps = 1e-12 * max(abs(hi - lo), 1.0)
    ib1 = axis.FindFixBin(lo + eps)
    ib2 = axis.FindFixBin(hi - eps)

    ib1 = max(1, min(axis.GetNbins(), ib1))
    ib2 = max(1, min(axis.GetNbins(), ib2))

    if ib2 < ib1:
        ib1, ib2 = ib2, ib1

    return ib1, ib2


def normalize_2d_hist(h, name, norm_xrange=None, norm_yrange=None):
    """
    Return a normalized clone of a TH2-like histogram.

    norm_xrange/norm_yrange:
      - If None, normalize over the full histogram range.
      - If given, normalize only over that visible/selected range.

    Do not use this for TProfile2D PDFUnc maps.
    Use this for NoCut_* event-count maps.
    """
    if h is None:
        return None

    if h.InheritsFrom("TProfile") or h.InheritsFrom("TProfile2D"):
        print(f"[WARN] normalize_2d_hist called on profile-like object: {h.GetName()}")
        print("       This is probably not what you want. Returning unnormalized clone.")
        out = h.Clone(name)
        if out.InheritsFrom("TH1"):
            out.SetDirectory(0)
        return out

    out = h.Clone(name)
    if out.InheritsFrom("TH1"):
        out.SetDirectory(0)

    ix1, ix2 = get_bin_range_for_axis(out.GetXaxis(), norm_xrange)
    iy1, iy2 = get_bin_range_for_axis(out.GetYaxis(), norm_yrange)

    integral = out.Integral(ix1, ix2, iy1, iy2)

    if integral <= 0:
        print(f"[WARN] normalize_2d_hist: non-positive integral for {h.GetName()}")
        print(f"       integral = {integral}, xbins=({ix1},{ix2}), ybins=({iy1},{iy2})")
        return out

    out.Scale(1.0 / integral)
    return out

def make_2d_ratio(num, den, name, occ_num=None, occ_den=None, min_entries=0):
    """
    Make a plain TH2D ratio map: num / den.

    Important:
      num and den can be TH2 or TProfile2D.
      The output is always TH2D, not TProfile2D.
      This avoids profile-internal bookkeeping problems.
    """
    if num is None or den is None:
        print(f"[WARN] make_2d_ratio: missing num or den for {name}")
        return None

    h = make_empty_2d_hist_like(num, name)

    if num.GetNbinsX() != den.GetNbinsX() or num.GetNbinsY() != den.GetNbinsY():
        print(f"[WARN] make_2d_ratio: bin mismatch for {name}")
        print(f"       num bins = ({num.GetNbinsX()}, {num.GetNbinsY()})")
        print(f"       den bins = ({den.GetNbinsX()}, {den.GetNbinsY()})")
        return None

    for ix in range(1, num.GetNbinsX() + 1):
        for iy in range(1, num.GetNbinsY() + 1):

            # Optional occupancy cut.
            # If occ histograms are missing, no occupancy cut is applied.
            if occ_num is not None and occ_num.GetBinContent(ix, iy) < min_entries:
                continue
            if occ_den is not None and occ_den.GetBinContent(ix, iy) < min_entries:
                continue

            a = num.GetBinContent(ix, iy)
            b = den.GetBinContent(ix, iy)

            if b <= 0:
                continue

            h.SetBinContent(ix, iy, a / b)
            h.SetBinError(ix, iy, 0.0)

    return h

def auto_top_range(hists, is_profile=False, use_log=False):
    ymax = 0.0
    ymin = 1e9
    for h in hists:
        if not h:
            continue
        ymax = max(ymax, h.GetMaximum())
        for ib in range(1, h.GetNbinsX() + 1):
            y = h.GetBinContent(ib)
            if y > 0:
                ymin = min(ymin, y)

    if ymax <= 0:
        return (1e-3, 1.0) if use_log else (0.0, 1.0)

    if use_log:
        if ymin == 1e9:
            ymin = 1e-3
        return (max(ymin * 0.7, 1e-6), ymax * 3.0)

    if is_profile:
        return (0.0, ymax * 1.35)
    return (0.0, ymax * 1.35)


def auto_ratio_range(ratio_hists, default=(0.7, 1.3), hard_min=0.0, hard_max=100.0):
    vals = []
    for h in ratio_hists:
        if not h:
            continue
        for ib in range(1, h.GetNbinsX() + 1):
            y = h.GetBinContent(ib)
            if y > 0 and y < 50:
                vals.append(y)

    if not vals:
        return default

    vmin = min(vals)
    vmax = max(vals)

    if abs(vmax - vmin) < 0.10:
        center = 0.5 * (vmax + vmin)
        return (max(hard_min, center - 0.15), min(hard_max, center + 0.15))

    pad = 0.15 * (vmax - vmin)
    lo = max(hard_min, vmin - pad)
    hi = min(hard_max, vmax + pad)

    if lo < 0.05:
        lo = 0.0
    return (lo, hi)


def draw_with_ratio(
    objs,
    labels,
    outname,
    xtitle,
    ytitle,
    title="",
    ref_index=0,
    ratio_title="Ratio to ref",
    normalize=False,
    top_range=None,
    ratio_range=None,
    xrange=None,
    SetLogTop=False,
    SetLogRatio=False,
):
    """
    objs: list of TH1 or TProfile clones
    ref_index: reference object for ratio
    """
    hs = []
    for h in objs:
        if not h:
            hs.append(None)
            continue
        hh = h.Clone(f"{h.GetName()}__draw")
        if hh.InheritsFrom("TH1"):
            hh.SetDirectory(0)
        if normalize:
            normalize_hist(hh)
        hs.append(hh)

    ref = hs[ref_index]
    if ref is None:
        print(f"[WARN] ref is None for {outname}")
        return

    is_profile = ref.InheritsFrom("TProfile")

    c = ROOT.TCanvas(f"c_{outname}", "", 800, 800)

    pad1 = ROOT.TPad("pad1", "", 0.0, 0.30, 1.0, 1.0)
    pad2 = ROOT.TPad("pad2", "", 0.0, 0.00, 1.0, 0.30)

    pad1.SetBottomMargin(0.02)
    pad1.SetLeftMargin(0.12)
    pad1.SetRightMargin(0.04)
    pad1.SetTopMargin(0.08)

    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.35)
    pad2.SetLeftMargin(0.12)
    pad2.SetRightMargin(0.04)

    pad1.Draw()
    pad2.Draw()

    # ---------- top pad ----------
    pad1.cd()
    if SetLogTop:
        pad1.SetLogy()

    if top_range is not None:
        top_ymin, top_ymax = top_range
    else:
        top_ymin, top_ymax = auto_top_range([h for h in hs if h], is_profile=is_profile, use_log=SetLogTop)

    if SetLogTop:
        top_ymin = max(top_ymin, 1e-6)

    leg = ROOT.TLegend(0.56, 0.68, 0.88, 0.88)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)

    first = True
    for h, lab in zip(hs, labels):
        if not h:
            continue

        if first:
            h.SetTitle(title)
            h.GetYaxis().SetTitle(ytitle)
            h.GetYaxis().SetTitleSize(0.050)
            h.GetYaxis().SetLabelSize(0.042)
            h.GetYaxis().SetTitleOffset(1.05)
            h.GetXaxis().SetLabelSize(0.0)
            h.GetXaxis().SetTitleSize(0.0)
            h.GetYaxis().SetRangeUser(top_ymin, top_ymax)

            if xrange is not None:
                h.GetXaxis().SetRangeUser(xrange[0], xrange[1])

            draw_opt = "E1" if is_profile else "hist"
            h.Draw(draw_opt)
            first = False
        else:
            draw_opt = "E1 same" if is_profile else "hist same"
            h.Draw(draw_opt)

        leg.AddEntry(h, lab, "lep" if is_profile else "l")

    leg.Draw()

    # ---------- ratio pad ----------
    pad2.cd()
    if SetLogRatio:
        pad2.SetLogy()

    ratio_hists = []
    for i, h in enumerate(hs):
        if i == ref_index or h is None:
            continue
        r = make_ratio(h, ref, f"ratio_{outname}_{i}")
        style_obj(
            r,
            h.GetLineColor(),
            h.GetMarkerStyle() if is_profile else 20,
            h.GetLineStyle(),
            h.GetLineWidth(),
        )
        ratio_hists.append(r)

    if not ratio_hists:
        print(f"[WARN] no ratio hist for {outname}")
        c.Close()
        return

    rr = ratio_range if ratio_range is not None else auto_ratio_range(ratio_hists)
    rr_lo = max(rr[0], 1e-6) if SetLogRatio else rr[0]
    rr_hi = rr[1]

    first = True
    for r in ratio_hists:
        if first:
            r.SetTitle("")
            r.GetYaxis().SetTitle(ratio_title)
            r.GetYaxis().SetTitleSize(0.10)
            r.GetYaxis().SetLabelSize(0.09)
            r.GetYaxis().SetTitleOffset(0.45)
            r.GetYaxis().SetNdivisions(505)

            r.GetXaxis().SetTitle(xtitle)
            r.GetXaxis().SetTitleSize(0.12)
            r.GetXaxis().SetLabelSize(0.10)
            r.GetXaxis().SetTitleOffset(1.05)

            r.GetYaxis().SetRangeUser(rr_lo, rr_hi)

            if xrange is not None:
                r.GetXaxis().SetRangeUser(xrange[0], xrange[1])

            r.Draw("E1")
            first = False
        else:
            r.Draw("E1 same")

    x1 = ref.GetXaxis().GetXmin() if xrange is None else xrange[0]
    x2 = ref.GetXaxis().GetXmax() if xrange is None else xrange[1]
    line = ROOT.TLine(x1, 1.0, x2, 1.0)
    line.SetLineStyle(2)
    line.SetLineWidth(2)
    line.Draw()

    c.SaveAs(os.path.join(OUTDIR, outname + ".pdf"))
    c.SaveAs(os.path.join(OUTDIR, outname + ".png"))
    c.Close()

def draw_2d(
    obj,
    outname,
    xtitle,
    ytitle,
    ztitle,
    title="",
    xrange=None,
    yrange=None,
    zrange=None,
    drawopt="COLZ",
    SetLogX=False,
    SetLogY=False,
    SetLogZ=False,
):
    if not obj:
        print(f"[WARN] 2D object is None for {outname}")
        return

    h = obj.Clone(f"{obj.GetName()}__draw2d")
    if h.InheritsFrom("TH1"):
        h.SetDirectory(0)

    c = ROOT.TCanvas(f"c2d_{outname}", "", 900, 800)
    c.SetLeftMargin(0.12)
    c.SetRightMargin(0.16)
    c.SetBottomMargin(0.12)
    c.SetTopMargin(0.08)

    if SetLogX:
        c.SetLogx()
    if SetLogY:
        c.SetLogy()
    if SetLogZ:
        c.SetLogz()

    h.SetTitle(title)
    h.GetXaxis().SetTitle(xtitle)
    h.GetYaxis().SetTitle(ytitle)
    h.GetZaxis().SetTitle(ztitle)

    h.GetXaxis().SetTitleSize(0.045)
    h.GetYaxis().SetTitleSize(0.045)
    h.GetZaxis().SetTitleSize(0.045)
    h.GetXaxis().SetLabelSize(0.040)
    h.GetYaxis().SetLabelSize(0.040)
    h.GetZaxis().SetLabelSize(0.040)
    h.GetYaxis().SetTitleOffset(1.30)
    h.GetZaxis().SetTitleOffset(1.20)

    if xrange is not None:
        h.GetXaxis().SetRangeUser(xrange[0], xrange[1])
    if yrange is not None:
        h.GetYaxis().SetRangeUser(yrange[0], yrange[1])

    # Manual z-range should win if provided.
    if zrange is not None:
        h.SetMinimum(zrange[0])
        h.SetMaximum(zrange[1])
    elif SetLogZ:
        min_positive = None
        for ix in range(1, h.GetNbinsX() + 1):
            for iy in range(1, h.GetNbinsY() + 1):
                val = h.GetBinContent(ix, iy)
                if val > 0:
                    if min_positive is None or val < min_positive:
                        min_positive = val
        if min_positive is not None:
            h.SetMinimum(max(min_positive * 0.7, 1e-6))

    h.Draw(drawopt)
    c.SaveAs(os.path.join(OUTDIR, outname + ".pdf"))
    c.SaveAs(os.path.join(OUTDIR, outname + ".png"))
    c.Close()

def draw_2d_pair(
    obj_left,
    obj_right,
    outname,
    xtitle,
    ytitle,
    ztitle,
    title_left="",
    title_right="",
    xrange=None,
    yrange=None,
    zrange=None,
    drawopt="COLZ",
    SetLogX=False,
    SetLogY=False,
    SetLogZ=False,
):
    if (obj_left is None) or (obj_right is None):
        print(f"[WARN] one of the pair plots is None for {outname}")
        return

    c = ROOT.TCanvas(f"c2d_pair_{outname}", "", 1800, 800)
    c.Divide(2, 1)

    drawn_hists = []

    for ipad, (obj, this_title) in enumerate([(obj_left, title_left), (obj_right, title_right)], start=1):
        c.cd(ipad)
        pad = ROOT.gPad
        pad.SetLeftMargin(0.12)
        pad.SetRightMargin(0.16)
        pad.SetBottomMargin(0.12)
        pad.SetTopMargin(0.08)

        if SetLogX:
            pad.SetLogx()
        if SetLogY:
            pad.SetLogy()
        if SetLogZ:
            pad.SetLogz()

        h = obj.Clone(f"{obj.GetName()}__pair_{ipad}")
        if h.InheritsFrom("TH1"):
            h.SetDirectory(0)

        h.SetTitle(this_title)
        h.GetXaxis().SetTitle(xtitle)
        h.GetYaxis().SetTitle(ytitle)
        h.GetZaxis().SetTitle(ztitle)

        h.GetXaxis().SetTitleSize(0.045)
        h.GetYaxis().SetTitleSize(0.045)
        h.GetZaxis().SetTitleSize(0.045)
        h.GetXaxis().SetLabelSize(0.040)
        h.GetYaxis().SetLabelSize(0.040)
        h.GetZaxis().SetLabelSize(0.040)
        h.GetYaxis().SetTitleOffset(1.30)
        h.GetZaxis().SetTitleOffset(1.20)

        if xrange is not None:
            h.GetXaxis().SetRangeUser(xrange[0], xrange[1])
        if yrange is not None:
            h.GetYaxis().SetRangeUser(yrange[0], yrange[1])

        if zrange is not None:
            h.SetMinimum(zrange[0])
            h.SetMaximum(zrange[1])
        elif SetLogZ:
            min_positive = None
            for ix in range(1, h.GetNbinsX() + 1):
                for iy in range(1, h.GetNbinsY() + 1):
                    val = h.GetBinContent(ix, iy)
                    if val > 0:
                        if min_positive is None or val < min_positive:
                            min_positive = val
            if min_positive is not None:
                h.SetMinimum(max(min_positive * 0.7, 1e-6))

        h.Draw(drawopt)
        drawn_hists.append(h)

    c.SaveAs(os.path.join(OUTDIR, outname + ".pdf"))
    c.SaveAs(os.path.join(OUTDIR, outname + ".png"))
    c.Close()

def draw_2d_quad(
    pad_specs,
    outname,
):
    """
    Draw four 2D histograms in a 2x2 canvas.

    pad layout:
        1 = top-left
        2 = top-right
        3 = bottom-left
        4 = bottom-right

    pad_specs: list of 4 dictionaries.
      Each dict should contain:
        obj, title, xtitle, ytitle, ztitle,
        xrange, yrange, zrange,
        drawopt, logx, logy, logz
    """
    if len(pad_specs) != 4:
        raise RuntimeError(f"draw_2d_quad expects 4 pad specs, got {len(pad_specs)}")

    if any(spec.get("obj") is None for spec in pad_specs):
        print(f"[WARN] one of the quad plots is None for {outname}")
        return

    c = ROOT.TCanvas(f"c2d_quad_{outname}", "", 1800, 1600)
    c.Divide(2, 2)

    # Important for PyROOT:
    # keep drawn histograms alive until SaveAs finishes.
    drawn_hists = []

    for ipad, spec in enumerate(pad_specs, start=1):
        c.cd(ipad)
        pad = ROOT.gPad

        pad.SetLeftMargin(0.12)
        pad.SetRightMargin(0.17)
        pad.SetBottomMargin(0.12)
        pad.SetTopMargin(0.09)

        if spec.get("logx", False):
            pad.SetLogx()
        if spec.get("logy", False):
            pad.SetLogy()
        if spec.get("logz", False):
            pad.SetLogz()

        obj = spec["obj"]
        h = obj.Clone(f"{obj.GetName()}__quad_{ipad}")
        if h.InheritsFrom("TH1"):
            h.SetDirectory(0)

        h.SetTitle(spec.get("title", ""))
        h.GetXaxis().SetTitle(spec.get("xtitle", ""))
        h.GetYaxis().SetTitle(spec.get("ytitle", ""))
        h.GetZaxis().SetTitle(spec.get("ztitle", ""))

        h.GetXaxis().SetTitleSize(0.045)
        h.GetYaxis().SetTitleSize(0.045)
        h.GetZaxis().SetTitleSize(0.045)

        h.GetXaxis().SetLabelSize(0.040)
        h.GetYaxis().SetLabelSize(0.040)
        h.GetZaxis().SetLabelSize(0.040)

        h.GetYaxis().SetTitleOffset(1.30)
        h.GetZaxis().SetTitleOffset(1.25)

        xrange = spec.get("xrange", None)
        yrange = spec.get("yrange", None)
        zrange = spec.get("zrange", None)

        if xrange is not None:
            h.GetXaxis().SetRangeUser(xrange[0], xrange[1])
        if yrange is not None:
            h.GetYaxis().SetRangeUser(yrange[0], yrange[1])

        if zrange is not None:
            h.SetMinimum(zrange[0])
            h.SetMaximum(zrange[1])
        elif spec.get("logz", False):
            min_positive = None
            for ix in range(1, h.GetNbinsX() + 1):
                for iy in range(1, h.GetNbinsY() + 1):
                    val = h.GetBinContent(ix, iy)
                    if val > 0:
                        if min_positive is None or val < min_positive:
                            min_positive = val
            if min_positive is not None:
                h.SetMinimum(max(min_positive * 0.7, 1e-6))

        h.Draw(spec.get("drawopt", "COLZ"))
        drawn_hists.append(h)

    c.SaveAs(os.path.join(OUTDIR, outname + ".pdf"))
    c.SaveAs(os.path.join(OUTDIR, outname + ".png"))
    c.Close()

# =========================================================
# load files
# =========================================================
TF = {}
for proc, mdict in FILES.items():
    TF[proc] = {}
    for mass, fname in mdict.items():
        TF[proc][mass] = open_root(os.path.join(INDIR, fname))

# =========================================================
# 1) mass dependence: compare masses within the same process
#    ref = M500
# =========================================================
for proc in ["CCDY", "WG", "SSWW"]:
    masses = [500, 1000, 3000]

    for hname, is_profile, do_norm, xrange, xtitle, ytitle in get_1d_plot_list_for_proc(proc):
        display_cfg = get_display_setting("mass", proc=proc, hname=hname)
        SetLogTop = display_cfg["top"]["log"]
        SetLogRatio = display_cfg["ratio"]["log"]
        top_range = display_cfg["top"]["yrange"]
        ratio_range = display_cfg["ratio"]["yrange"]
        draw_xrange = display_cfg["xrange"] if display_cfg["xrange"] is not None else xrange

        objs = []
        labels = []
        for m in masses:
            h = get_obj(TF[proc][m], hname)
            if not h:
                continue
            style_obj(h, MASS_COLORS[m], MASS_MARKERS[m], MASS_LINESTYLES[m])
            objs.append(h)
            labels.append(f"{proc}, m_{{N}}={m} GeV")

        if len(objs) < 2:
            print(f"[WARN] skip mass comparison for {proc} {hname}: not enough objects")
            continue

        draw_with_ratio(
            objs=objs,
            labels=labels,
            outname=f"massCompare_{proc}_{hname}",
            xtitle=xtitle,
            ytitle=ytitle,
            title=f"{proc}: mass dependence",
            ref_index=0,
            ratio_title=" / M500",
            normalize=do_norm,
            top_range=top_range,
            ratio_range=ratio_range,
            xrange=draw_xrange,
            SetLogTop=SetLogTop,
            SetLogRatio=SetLogRatio,
        )

# =========================================================
# 2) process dependence: compare CCDY / WG / SSWW at fixed mass
#    ref = CCDY
# =========================================================
for mass in [500, 1000, 3000]:
    for hname, is_profile, do_norm, xrange, xtitle, ytitle in PLOTS_COMMON:
        display_cfg = get_display_setting("proc", hname=hname)
        SetLogTop = display_cfg["top"]["log"]
        SetLogRatio = display_cfg["ratio"]["log"]
        top_range = display_cfg["top"]["yrange"]
        ratio_range = display_cfg["ratio"]["yrange"]
        draw_xrange = display_cfg["xrange"] if display_cfg["xrange"] is not None else xrange

        objs = []
        labels = []

        for proc in ["CCDY", "WG", "SSWW"]:
            h = get_obj(TF[proc][mass], hname)
            if not h:
                continue
            style_obj(h, PROC_COLORS[proc], PROC_MARKERS[proc], 1)
            objs.append(h)
            labels.append(f"{proc}, m_{{N}}={mass} GeV")

        if len(objs) < 2:
            print(f"[WARN] skip process comparison for M{mass} {hname}: not enough objects")
            continue

        draw_with_ratio(
            objs=objs,
            labels=labels,
            outname=f"procCompare_M{mass}_{hname}",
            xtitle=xtitle,
            ytitle=ytitle,
            title=f"Process comparison at m_{{N}}={mass} GeV",
            ref_index=0,
            ratio_title=" / CCDY",
            normalize=do_norm,
            top_range=top_range,
            ratio_range=ratio_range,
            xrange=draw_xrange,
            SetLogTop=SetLogTop,
            SetLogRatio=SetLogRatio,
        )

    for hname, is_profile, do_norm, xrange, xtitle, ytitle in PLOTS_COMMON:
        display_cfg = get_display_setting("proc", hname=hname)
        SetLogTop = display_cfg["top"]["log"]
        SetLogRatio = display_cfg["ratio"]["log"]
        top_range = display_cfg["top"]["yrange"]
        ratio_range = display_cfg["ratio"]["yrange"]
        draw_xrange = display_cfg["xrange"] if display_cfg["xrange"] is not None else xrange

        objs = []
        labels = []

        for proc in ["CCDY", "WG", "SSWW"]:
            h = get_obj(TF[proc][mass], hname)
            if not h:
                continue
            style_obj(h, PROC_COLORS[proc], PROC_MARKERS[proc], 1)
            objs.append(h)
            labels.append(f"{proc}, m_{{N}}={mass} GeV")

        if len(objs) < 2:
            print(f"[WARN] skip process comparison for M{mass} {hname}: not enough objects")
            continue

        draw_with_ratio(
            objs=objs,
            labels=labels,
            outname=f"procCompare_M{mass}_{hname}",
            xtitle=xtitle,
            ytitle=ytitle,
            title=f"Process comparison at m_{{N}}={mass} GeV",
            ref_index=0,
            ratio_title=" / CCDY",
            normalize=do_norm,
            top_range=top_range,
            ratio_range=ratio_range,
            xrange=draw_xrange,
            SetLogTop=SetLogTop,
            SetLogRatio=SetLogRatio,
        )

# =========================================================
# 3) 2D plots: raw maps
# =========================================================
for proc in ["CCDY", "WG", "SSWW"]:
    for mass in [500, 1000, 3000]:
        for hname, default_xrange, default_yrange, xtitle, ytitle, ztitle, default_drawopt in get_2d_plot_list_for_proc(proc):
            h2 = get_obj(TF[proc][mass], hname)
            if not h2:
                continue

            cfg = get_display_setting_2d_raw(proc, hname)
            draw_xrange = cfg["xrange"] if cfg["xrange"] is not None else default_xrange
            draw_yrange = cfg["yrange"] if cfg["yrange"] is not None else default_yrange
            draw_zrange = cfg["zrange"]
            drawopt = cfg["drawopt"] if cfg["drawopt"] is not None else default_drawopt

            draw_2d(
                obj=h2,
                outname=f"plot2D_{proc}_M{mass}_{hname}",
                xtitle=xtitle,
                ytitle=ytitle,
                ztitle=ztitle,
                title=f"{proc}, m_{{N}}={mass} GeV: {hname}",
                xrange=draw_xrange,
                yrange=draw_yrange,
                zrange=draw_zrange,
                drawopt=drawopt,
                SetLogX=cfg["logx"],
                SetLogY=cfg["logy"],
                SetLogZ=cfg["logz"],
            )

# =========================================================
# 4) 2D ratio plots:
#    top row    = PDF uncertainty ratio to CCDY
#    bottom row = normalized event shape ratio to CCDY
# =========================================================
for mass in [500, 1000, 3000]:
    for hname_unc, hname_nocut, hname_occ, default_xrange, default_yrange, xtitle, ytitle, default_drawopt in PLOTS_2D_RATIO_QUAD:

        # ---------- input histograms: PDF uncertainty maps ----------
        h_unc_ccdy = get_obj(TF["CCDY"][mass], hname_unc)
        h_unc_wg   = get_obj(TF["WG"][mass], hname_unc)
        h_unc_ssww = get_obj(TF["SSWW"][mass], hname_unc)

        # ---------- input histograms: NoCut event maps ----------
        h_evt_ccdy = get_obj(TF["CCDY"][mass], hname_nocut)
        h_evt_wg   = get_obj(TF["WG"][mass], hname_nocut)
        h_evt_ssww = get_obj(TF["SSWW"][mass], hname_nocut)

        # ---------- occupancy histograms ----------
        occ_ccdy = get_obj(TF["CCDY"][mass], hname_occ)
        occ_wg   = get_obj(TF["WG"][mass], hname_occ)
        occ_ssww = get_obj(TF["SSWW"][mass], hname_occ)

        # Need all PDF uncertainty maps and NoCut maps for the 4-pad comparison.
        if any(x is None for x in [h_unc_ccdy, h_unc_wg, h_unc_ssww]):
            print(f"[WARN] skip quad ratio for M{mass} {hname_unc}: missing PDF uncertainty hist")
            print(f"       CCDY: {bool(h_unc_ccdy)}, WG: {bool(h_unc_wg)}, SSWW: {bool(h_unc_ssww)}")
            continue

        if any(x is None for x in [h_evt_ccdy, h_evt_wg, h_evt_ssww]):
            print(f"[WARN] skip quad ratio for M{mass} {hname_nocut}: missing NoCut event hist")
            print(f"       CCDY: {bool(h_evt_ccdy)}, WG: {bool(h_evt_wg)}, SSWW: {bool(h_evt_ssww)}")
            continue

        # Occupancy is useful but not mandatory.
        if any(x is None for x in [occ_ccdy, occ_wg, occ_ssww]):
            print(f"[WARN] occupancy hist missing for M{mass} {hname_occ}; drawing without occupancy cut")
            print(f"       occ CCDY: {bool(occ_ccdy)}, occ WG: {bool(occ_wg)}, occ SSWW: {bool(occ_ssww)}")
            occ_ccdy = None
            occ_wg = None
            occ_ssww = None

        # ---------- display settings ----------
        # Top row uses settings of PDFUnc_*.
        cfg_unc = get_display_setting_2d_ratio(hname_unc)

        # Bottom row uses settings of NoCut_*.
        cfg_evt = get_display_setting_2d_ratio(hname_nocut)

        # Use PDFUnc setting first for common x/y range.
        # If not given, fall back to NoCut setting, then to table default.
        draw_xrange = (
            cfg_unc["xrange"]
            if cfg_unc["xrange"] is not None
            else cfg_evt["xrange"]
            if cfg_evt["xrange"] is not None
            else default_xrange
        )

        draw_yrange = (
            cfg_unc["yrange"]
            if cfg_unc["yrange"] is not None
            else cfg_evt["yrange"]
            if cfg_evt["yrange"] is not None
            else default_yrange
        )

        drawopt_unc = cfg_unc["drawopt"] if cfg_unc["drawopt"] is not None else default_drawopt
        drawopt_evt = cfg_evt["drawopt"] if cfg_evt["drawopt"] is not None else default_drawopt

        # ---------- top row: PDF uncertainty ratio ----------
        ratio_unc_wg = make_2d_ratio(
            h_unc_wg,
            h_unc_ccdy,
            f"ratioPDFUnc_WG_over_CCDY_M{mass}_{hname_unc}",
            occ_num=occ_wg,
            occ_den=occ_ccdy,
            min_entries=0,
        )

        ratio_unc_ssww = make_2d_ratio(
            h_unc_ssww,
            h_unc_ccdy,
            f"ratioPDFUnc_SSWW_over_CCDY_M{mass}_{hname_unc}",
            occ_num=occ_ssww,
            occ_den=occ_ccdy,
            min_entries=0,
        )

        # ---------- bottom row: normalized event shape ratio ----------
        # This keeps your current convention:
        # normalize over the full histogram, not only the drawn x/y window.
        h_evt_ccdy_norm = normalize_2d_hist(
            h_evt_ccdy,
            f"{h_evt_ccdy.GetName()}__shapeNorm_CCDY_M{mass}",
            norm_xrange=None,
            norm_yrange=None,
        )
        h_evt_wg_norm = normalize_2d_hist(
            h_evt_wg,
            f"{h_evt_wg.GetName()}__shapeNorm_WG_M{mass}",
            norm_xrange=None,
            norm_yrange=None,
        )
        h_evt_ssww_norm = normalize_2d_hist(
            h_evt_ssww,
            f"{h_evt_ssww.GetName()}__shapeNorm_SSWW_M{mass}",
            norm_xrange=None,
            norm_yrange=None,
        )

        ratio_evt_wg = make_2d_ratio(
            h_evt_wg_norm,
            h_evt_ccdy_norm,
            f"ratioNoCutShape_WG_over_CCDY_M{mass}_{hname_nocut}",
            occ_num=occ_wg,
            occ_den=occ_ccdy,
            min_entries=0,
        )

        ratio_evt_ssww = make_2d_ratio(
            h_evt_ssww_norm,
            h_evt_ccdy_norm,
            f"ratioNoCutShape_SSWW_over_CCDY_M{mass}_{hname_nocut}",
            occ_num=occ_ssww,
            occ_den=occ_ccdy,
            min_entries=0,
        )

        # ---------- draw 2x2 canvas ----------
        pad_specs = [
            {
                "obj": ratio_unc_wg,
                "title": f"PDF uncert.: WG / CCDY, m_{{N}}={mass} GeV",
                "xtitle": xtitle,
                "ytitle": ytitle,
                "ztitle": "PDF uncert. ratio to CCDY",
                "xrange": draw_xrange,
                "yrange": draw_yrange,
                "zrange": cfg_unc["zrange"],
                "drawopt": drawopt_unc,
                "logx": cfg_unc["logx"],
                "logy": cfg_unc["logy"],
                "logz": cfg_unc["logz"],
            },
            {
                "obj": ratio_unc_ssww,
                "title": f"PDF uncert.: SSWW / CCDY, m_{{N}}={mass} GeV",
                "xtitle": xtitle,
                "ytitle": ytitle,
                "ztitle": "PDF uncert. ratio to CCDY",
                "xrange": draw_xrange,
                "yrange": draw_yrange,
                "zrange": cfg_unc["zrange"],
                "drawopt": drawopt_unc,
                "logx": cfg_unc["logx"],
                "logy": cfg_unc["logy"],
                "logz": cfg_unc["logz"],
            },
            {
                "obj": ratio_evt_wg,
                "title": f"Norm. events: WG / CCDY, m_{{N}}={mass} GeV",
                "xtitle": xtitle,
                "ytitle": ytitle,
                "ztitle": "Normalized event ratio to CCDY",
                "xrange": draw_xrange,
                "yrange": draw_yrange,
                "zrange": cfg_evt["zrange"],
                "drawopt": drawopt_evt,
                "logx": cfg_evt["logx"],
                "logy": cfg_evt["logy"],
                "logz": cfg_evt["logz"],
            },
            {
                "obj": ratio_evt_ssww,
                "title": f"Norm. events: SSWW / CCDY, m_{{N}}={mass} GeV",
                "xtitle": xtitle,
                "ytitle": ytitle,
                "ztitle": "Normalized event ratio to CCDY",
                "xrange": draw_xrange,
                "yrange": draw_yrange,
                "zrange": cfg_evt["zrange"],
                "drawopt": drawopt_evt,
                "logx": cfg_evt["logx"],
                "logy": cfg_evt["logy"],
                "logz": cfg_evt["logz"],
            },
        ]

        draw_2d_quad(
            pad_specs=pad_specs,
            outname=f"ratio2DQuad_M{mass}_{hname_unc}",
        )

# =========================================================
# cleanup
# =========================================================
for proc in TF:
    for mass in TF[proc]:
        TF[proc][mass].Close()

print(f"[DONE] output dir = {OUTDIR}")
