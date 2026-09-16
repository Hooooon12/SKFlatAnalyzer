#!/usr/bin/env python3
"""Standalone cosmetic alternative to make_post_fit_plots(8).py.

Original CLI, signal scaling, process stacking and uncertainty helpers retained.
Added: region-specific bin metadata, cuts/codes/both axes, bin-key TXT export,
explicit already-merged input maps, separate outputs, direct ROOT-file input.
Sources: AN2019_206_v7 Tables 39-46 and 63-64.
See postfit_cosmetic_guide.md for commands and configuration examples.
"""

import os, argparse, math, re, json, fnmatch, copy, sys

try:
    import ROOT
except ImportError:
    ROOT = None  # --describe-bins and metadata utilities work without PyROOT.

if ROOT is not None:
    ROOT.PyConfig.IgnoreCommandLineOptions = True
    ROOT.gROOT.SetBatch(True)
    ROOT.TH1.AddDirectory(False)
    try:
        import tdrstyle
        tdrstyle.setTDRStyle()
    except ImportError:
        pass
    ROOT.gStyle.SetOptStat(0)
    ROOT.gStyle.SetOptTitle(0)

# ------------------------------------------------------------
# Configuration
# ------------------------------------------------------------

BASE_DIR = "/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN"

OUTDIR_BASE = "plots"

MASS_CHOICES = [
    "85","90","95","100","125","150","200","250","300","350","400","450","500",
    "600","700","800","900","1000","1100","1200","1300","1500","1700","2000",
    "2500","3000","5000","7500","10000","15000","20000","25000","30000",
    "40000","50000","60000",
]

TAG_MAP = {
    "AllSR": ["_syst"],
    "SR1": ["_sr1_syst_Combined"],
    "SR2": ["_sr2_syst_Combined"],
    "SR3": ["_sr3_syst_Combined"],
}

FIT_OUTPUT_DIR = {
    "shapes_prefit": "prefit",
    "shapes_fit_b": "postfit_b",
    "shapes_fit_s": "postfit_sb",
}

PROCESS_ERAS = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018",
]

STACK_ORDER = [
    "cf",
    "fake",
    "wz",
    "zz",
    "ww",
    "zg",
    "mc_others",
]

PROCESS_COLORS = {

    # Recommended palette (hex)
    "cf": "#92dadd",
    "fake": "#3f90da",
    "wz": "#ffa90e",
    # WZ_EWK is merged into WZ in plots; keep same color for safety
    "zz": "#bd1f01",
    "ww": "#94a4a2",
    "zg": "#832db6",
    "mc_others": "#b9ac70",
}

SIGNAL_COMPONENTS = {
    "DY": {
        "process": "signalDY",
        "label": "DY signal",
        "line_style": 1,
    },
    "VBF": {
        "process": "signalVBF",
        "label": "W#gamma signal",
        "line_style": 1,
    },
    "SSWW": {
        "process": "signalSSWW",
        "label": "SSWW signal",
        "line_style": 1,
    },
    "Weinberg": {
        "process": "signalWeinberg",
        "label": "Weinberg signal",
        "line_style": 1,
    },
}

# HNL input-template normalization.
# The analyzer output is produced at |V|^2 = 1, and these factors are
# applied when the Combine input histograms are built.
HNL_INPUT_MIXING_SQUARED_DEFAULT = 0.01
HNL_INPUT_MIXING_SQUARED_LOW_MASS = 0.001
HNL_INPUT_MIXING_SQUARED_HIGH_MASS = 0.1

# Weinberg input-template metadata.
WEINBERG_GENERATED_C5 = 1.0
WEINBERG_GENERATED_LAMBDA_TEV = 200.0
WEINBERG_INPUT_SCALE = 10000.0

# ------------------------------------------------------------
# Automatic signal scaling
# ------------------------------------------------------------

# Target:
#
#     max(signal) / max(total_background)
#
# In separate mode, each signal component can have a different
# target ratio so that DY, Wgamma and SSWW curves do not pile up
# at the same vertical height.
#
# In total/both mode, one common physical HNL benchmark must be
# preserved, so the "total" target is used.
#
# "VBF" here is the internal key for the Wgamma component.
#
# Tune these values cosmetically as desired.
AUTO_SIGNAL_TARGET_RATIO_LINEAR = {
    "total":    1.0,
    "DY":       1.0,
    "VBF":      0.6,
    "SSWW":     0.3,
    "Weinberg": 1.0,
}

AUTO_SIGNAL_TARGET_RATIO_LOGY = {
    "total":    1.0,
    "DY":       1.0,
    "VBF":      0.6,
    "SSWW":     0.3,
    "Weinberg": 1.0,
}


def auto_target_ratio(component_key, logy):
    table = (
        AUTO_SIGNAL_TARGET_RATIO_LOGY
        if logy
        else AUTO_SIGNAL_TARGET_RATIO_LINEAR
    )

    if component_key not in table:
        raise ValueError(
            f"No auto target ratio configured for {component_key!r}."
        )

    value = float(table[component_key])

    if not math.isfinite(value) or value <= 0:
        raise ValueError(
            f"Auto target ratio for {component_key} must be positive."
        )

    return value


def normalize_scale_spec(value, name="signal scale"):
    """
    Accept either:
        - a positive number
        - the literal string "auto"
    """
    if isinstance(value, str):
        text = value.strip()

        if text.lower() == "auto":
            return "auto"

    else:
        text = str(value)

    try:
        number = float(text)
    except (TypeError, ValueError):
        raise ValueError(
            f"{name} must be a positive number or 'auto', got {value!r}."
        )

    if not math.isfinite(number) or number <= 0:
        raise ValueError(
            f"{name} must be positive, got {value!r}."
        )

    return number


def parse_scale_spec(value):
    try:
        return normalize_scale_spec(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError(str(exc))


def is_auto_scale(value):
    return (
        isinstance(value, str)
        and value.strip().lower() == "auto"
    )


def round_one_significant(value):
    """
    Round a positive number to one significant digit.

    Examples:
        0.247 -> 0.2
        0.25  -> 0.3
        0.48  -> 0.5
        1830  -> 2000
        4860  -> 5000
    """
    if not math.isfinite(value) or value <= 0:
        raise ValueError(
            f"Cannot round non-positive/non-finite value: {value}"
        )

    exponent = math.floor(math.log10(value))
    unit = 10.0 ** exponent
    mantissa = value / unit

    # half-up rather than Python's bankers rounding
    digit = int(math.floor(mantissa + 0.5))

    if digit >= 10:
        return 10.0 * unit

    return digit * unit

# Separate-mode presets.
#
# Matching rules:
#   - "default" applies to every channel/mass.
#   - each item in "overrides" is applied from top to bottom.
#   - omit "channels" to match every channel.
#   - omit "masses" to match every mass.
#   - later matching rules override earlier ones.
#
# The numbers here are DRAW scale factors for the already-built input
# histograms. In prefit and postfit B-only plots they are converted to
# the corresponding |V|^2 value in the legend. In postfit S+B plots
# they are only display factors on the fitted component histograms.
SIGNAL_SCALE_PRESETS = {
    "Unblind_Step2": {
        "default": {
            "DY": "auto",
            "VBF": "auto",
            "SSWW": "auto",
            "Weinberg": "auto",
        },
        "overrides": [
            # Example: same scales for several masses, all channels.
            # {
            #     "masses": ["85", "90", "95", "100"],
            #     "scales": {
            #         "DY": 10.0,
            #         "VBF": 10.0,
            #         "SSWW": 100.0,
            #     },
            # },

            # Example: same scales for several channels, all masses.
            # {
            #     "channels": ["EE", "MuMu"],
            #     "scales": {
            #         "DY": 5.0,
            #         "VBF": 5.0,
            #         "SSWW": 25.0,
            #     },
            # },

            # Example: most specific override.
            # {
            #     "channels": ["MuMu"],
            #     "masses": ["150", "200"],
            #     "scales": {
            #         "DY": 8.0,
            #         "VBF": 6.0,
            #         "SSWW": 50.0,
            #     },
            # },
        ],
    },
}

ERAS = [
    "2016preVFP",
    "2016postVFP",
    "2017",
    "2018",
]

# ------------------------------------------------------------
# Helpers
# ------------------------------------------------------------

def parse_args():
    parser = argparse.ArgumentParser(
        description="Draw pre-fit and post-fit plots from Combine fitDiagnostics output."
    )

    parser.add_argument(
        "-wp",
        dest="InputWPs",
        required=False,
        default=[],
        nargs="+",
        help="List of LimitInput working points",
    )

    parser.add_argument(
        "-e",
        dest="eras",
        default=[],
        choices=["2016preVFP", "2016postVFP", "2017", "2018", "Run2", "Run2Sum"],
        nargs="+",
    )

    parser.add_argument(
        "-c",
        dest="channels",
        default=["MuMu", "EE", "EMu"],
        choices=["MuMu", "EE", "EMu", "3ch"],
        nargs="+",
    )

    parser.add_argument(
        "-m",
        dest="masses",
        default=[],
        choices=MASS_CHOICES,
        nargs="+",
    )

    parser.add_argument(
        "-s",
        dest="signals",
        default=["HNL", "Weinberg"],
        choices=["HNL", "DY", "VBF", "DYVBF", "SSWW", "Weinberg"],
        nargs="+",
    )

    parser.add_argument(
        "-t",
        dest="tags",
        default=["AllSR"],
        choices=["AllSR", "SR1", "SR2", "SR3"],
        nargs="+",
    )

    parser.add_argument(
        "--logy",
        action='store_true',
    )

    parser.add_argument(
        "--signal-mode",
        choices=["total", "separate", "both", "none"],
        default="none",
        help=(
            "How to draw signals: "
            "'total' draws one total signal curve, "
            "'separate' draws signal components, "
            "'both' draws total and component signals, "
            "and 'none' draws no signal. "
            "For prefit and postfit B-only HNL total/both, DY and Wgamma "
            "scale as S while SSWW scales as S^2. For postfit S+B, fitted "
            "signals are kept as fitted and S is only a common display multiplier."
        ),
    )

    parser.add_argument(
        "--signal-preset",
        default=None,
        choices=sorted(SIGNAL_SCALE_PRESETS.keys()),
        help=(
            "Scale preset for --signal-mode separate. "
            "The preset may contain channel- and mass-dependent overrides."
        ),
    )

    parser.add_argument(
        "--signal-scale-total",
        dest="signal_scale_total",
        type=parse_scale_spec,
        default=1.0,
        help=(
            "For prefit and postfit B-only HNL total/both, multiplier S "
            "of |V|^2 relative to the input template: DY/Wgamma are drawn "
            "xS and SSWW xS^2. For postfit S+B total/both, this is only "
            "a common display factor applied to the already-fitted HNL signals."
        ),
    )

    parser.add_argument(
        "--signal-scale-dy",
        dest="signal_scale_dy",
        type=parse_scale_spec,
        default=1.0,
        help=(
            "DY draw scale for --signal-mode separate. "
            "Ignored when a --signal-preset is used."
        ),
    )

    parser.add_argument(
        "--signal-scale-vbf",
        dest="signal_scale_vbf",
        type=parse_scale_spec,
        default=1.0,
        help=(
            "Wgamma/VBF draw scale for --signal-mode separate. "
            "Ignored when a --signal-preset is used."
        ),
    )

    parser.add_argument(
        "--signal-scale-ssww",
        dest="signal_scale_ssww",
        type=parse_scale_spec,
        default=1.0,
        help=(
            "SSWW draw scale for --signal-mode separate. "
            "Ignored when a --signal-preset is used."
        ),
    )

    parser.add_argument(
        "--signal-scale-weinberg",
        dest="signal_scale_weinberg",
        type=parse_scale_spec,
        default=1.0,
        help=(
            "Additional plotter scale for the Weinberg signal. "
            "For prefit plots the legend reports the generated point "
            "c5=1, Lambda=200 TeV and the total factor "
            "WEINBERG_INPUT_SCALE times this value."
        ),
    )

    parser.add_argument(
        "--signal-color-total",
        default="kMagenta",
        help=(
            "Total-signal color. Accepted formats include "
            "'#CC79A7', 'kRed', 'kRed+1', 'red', or '632'."
        ),
    )

    parser.add_argument(
        "--signal-color-dy",
        default="kRed",
        help=(
            "DY-signal color. Accepted formats include "
            "'#D55E00', 'kRed', 'kRed+1', 'red', or '632'."
        ),
    )

    parser.add_argument(
        "--signal-color-vbf",
        default="kOrange",
        help=(
            "VBF-signal color. Accepted formats include "
            "'#0072B2', 'kBlue', 'kAzure-9', or a numeric index."
        ),
    )

    parser.add_argument(
        "--signal-color-ssww",
        default="kBlue",
        help=(
            "SSWW-signal color. Accepted formats include "
            "'#009E73', 'kGreen+2', or a numeric index."
        ),
    )

    parser.add_argument(
        "--signal-color-weinberg",
        default="kMagenta",
        help=(
            "Weinberg signal color. Accepted formats include "
            "'#CC79A7', 'kMagenta', 'kMagenta+1', "
            "'red', or '632'."
        ),
    )

    parser.add_argument(
        "--no-data",
        action="store_true",
        help=(
            "Do not draw observed data in the upper pad. "
            "The ratio pad is kept and uses a B/B pseudo-data graph."
        ),
    )

    parser.add_argument("--axis-style", choices=["cuts", "codes", "both"], default="cuts",
                        help="EXO-21-003-like cut intervals, EXO-22-011-like bin IDs, or both.")
    parser.add_argument("--base-dir", default=BASE_DIR)
    parser.add_argument("--outdir", default=OUTDIR_BASE)
    parser.add_argument("--input-file", help="Read one explicit FitDiagnostics ROOT file instead of -wp.")
    parser.add_argument("--fit-subdir", default="Unblind",
                        help="Subdirectory below FitDiag; e.g. Unblind/mask-cr3_InvMET.")
    parser.add_argument("--fit-types", nargs="+", choices=list(FIT_OUTPUT_DIR),
                        default=list(FIT_OUTPUT_DIR))
    parser.add_argument("--regions", nargs="+", default=[],
                        help="Region names or shell globs; quote patterns containing '*'.")
    parser.add_argument("--bin-config", help="JSON file containing ordered region overrides (see guide).")
    parser.add_argument("--nbins", action="append", default=[], metavar="REGION=N",
                        help="Explicit count; use N=1 for an already inclusive CR. Repeatable.")
    parser.add_argument("--sr2-merge78", action="store_true",
                        help="The INPUT has already merged SR2 bins 7+8; adjust labels only.")
    parser.add_argument("--sr3-mode", choices=["auto", "low", "high"], default="auto",
                        help="Auto: BDT through 500 GeV, high-mass above 500 / Weinberg.")
    parser.add_argument("--sr1-binning-mass", type=int,
                        help="Explicit Table 39 mass key when it differs from the signal mass.")
    parser.add_argument("--describe-bins", action="store_true",
                        help="Print the selected bin definitions and exit; does not require ROOT.")
    parser.add_argument("--cms-label", default="Work in Progress",
                        help="Text after CMS; pass an empty string for CMS only.")
    parser.add_argument("--lumi", type=float, default=None,
                        help="Luminosity in fb^-1; override the era-based display default.")
    parser.add_argument("--canvas-width", type=int, default=0, help="0: adapt width to bin count.")
    parser.add_argument("--ratio-max", type=float, default=2.5)
    parser.add_argument("--label-angle", type=float, default=None,
                        help="Override bin-label rotation, in degrees (normally automatic).")
    args = parser.parse_args()

    if not args.eras:
        args.eras = ["Run2Sum"]

    mass_dependent_signals = [s for s in args.signals if s != "Weinberg"]
    if mass_dependent_signals and not args.masses:
        parser.error("-m/--masses is required for HNL/DY/VBF/DYVBF/SSWW signals.")

    if args.signal_preset is not None and args.signal_mode != "separate":
        parser.error(
            "--signal-preset can only be used with --signal-mode separate."
        )

    if not args.InputWPs and not args.input_file and not args.describe_bins:
        parser.error("Supply -wp or --input-file (or use --describe-bins).")
    if args.InputWPs and args.input_file:
        parser.error("Choose either -wp or --input-file.")
    if args.input_file and (len(args.eras) != 1 or len(args.channels) != 1
                            or len(args.signals) != 1 or len(args.masses) > 1):
        parser.error("--input-file needs one -e, -c and -s, and at most one -m.")
    if args.ratio_max <= 0 or (args.canvas_width and args.canvas_width < 650):
        parser.error("--ratio-max must be positive; --canvas-width must be 0 or >= 650.")
    if args.lumi is not None and args.lumi <= 0:
        parser.error("--lumi must be positive.")
    if args.sr1_binning_mass is not None and args.sr1_binning_mass not in SR1_EDGES["MuMu"]:
        parser.error("--sr1-binning-mass must be a mass listed in AN Table 39.")
    return args

def get_process_eras(era):
    if era in ["Run2", "Run2Sum"]:
        return ["2016preVFP", "2016postVFP", "2017", "2018"]
    return [era]


def get_hnl_input_mixing_squared(mass):
    """
    Return the |V|^2 value already encoded in the HNL input histogram.

    The analyzer output itself is at |V|^2 = 1. The Combine-input builder
    then applies the mass-dependent DY/VBF scaler below, while SSWW gets
    the square of the same scaler.
    """
    if mass is None:
        raise ValueError(
            "HNL input mixing cannot be determined without a mass."
        )

    mass_int = int(mass)

    if mass_int <= 100:
        return HNL_INPUT_MIXING_SQUARED_LOW_MASS

    if mass_int > 3000:
        return HNL_INPUT_MIXING_SQUARED_HIGH_MASS

    return HNL_INPUT_MIXING_SQUARED_DEFAULT


def resolve_separate_signal_scales(
    preset_name,
    channel,
    mass,
):
    """
    Resolve a separate-mode preset for one channel/mass point.

    Each scale may be either:
        - a positive number
        - "auto"

    Overrides are applied from top to bottom.
    """
    preset = SIGNAL_SCALE_PRESETS[preset_name]
    scales = dict(preset["default"])

    mass_text = None if mass is None else str(mass)

    for override in preset.get("overrides", []):
        channels = override.get("channels")
        masses = override.get("masses")

        if channels is not None and channel not in channels:
            continue

        if masses is not None:
            allowed_masses = {
                str(item)
                for item in masses
            }

            if mass_text not in allowed_masses:
                continue

        scales.update(
            override["scales"]
        )

    required_keys = [
        "DY",
        "VBF",
        "SSWW",
        "Weinberg",
    ]

    for key in required_keys:

        if key not in scales:
            raise ValueError(
                f"Preset {preset_name!r} does not define "
                f"a scale for {key}."
            )

        scales[key] = normalize_scale_spec(
            scales[key],
            name=(
                f"Preset {preset_name!r} "
                f"scale for {key}"
            ),
        )

    return scales


def resolve_signal_scales(
    args,
    channel,
    mass,
):
    """
    Return signal scale specifications.

    Each specification is either:
        - a positive numeric draw scale
        - "auto"

    For total/both HNL with a numeric S:
        DY/VBF -> xS
        SSWW   -> xS^2

    If total is "auto", the common physical HNL benchmark is
    resolved region-by-region after total_background is known.
    """

    # ========================================================
    # Separate mode
    # ========================================================
    if args.signal_mode == "separate":

        if args.signal_preset is not None:

            component_scales = (
                resolve_separate_signal_scales(
                    preset_name=args.signal_preset,
                    channel=channel,
                    mass=mass,
                )
            )

        else:

            component_scales = {
                "DY": normalize_scale_spec(
                    args.signal_scale_dy,
                    "--signal-scale-dy",
                ),

                "VBF": normalize_scale_spec(
                    args.signal_scale_vbf,
                    "--signal-scale-vbf",
                ),

                "SSWW": normalize_scale_spec(
                    args.signal_scale_ssww,
                    "--signal-scale-ssww",
                ),

                "Weinberg": normalize_scale_spec(
                    args.signal_scale_weinberg,
                    "--signal-scale-weinberg",
                ),
            }

        return {
            "total": normalize_scale_spec(
                args.signal_scale_total,
                "--signal-scale-total",
            ),

            **component_scales,
        }

    # ========================================================
    # Total / both / none
    # ========================================================

    total_spec = normalize_scale_spec(
        args.signal_scale_total,
        "--signal-scale-total",
    )

    if is_auto_scale(total_spec):

        # Actual numeric values will be resolved later,
        # region by region.
        dy_spec = "auto"
        vbf_spec = "auto"
        ssww_spec = "auto"

    else:

        dy_spec = total_spec
        vbf_spec = total_spec
        ssww_spec = (
            total_spec * total_spec
        )

    return {
        "total": total_spec,
        "DY": dy_spec,
        "VBF": vbf_spec,
        "SSWW": ssww_spec,

        "Weinberg": normalize_scale_spec(
            args.signal_scale_weinberg,
            "--signal-scale-weinberg",
        ),
    }


def get_signal_mode_subdir(args):

    # A named preset is already a unique scaling recipe.
    if (
        args.signal_mode == "separate"
        and args.signal_preset is not None
    ):
        return (
            f"signal_separate_"
            f"{args.signal_preset}"
        )

    base = f"signal_{args.signal_mode}"

    if args.signal_mode == "separate":

        relevant_scales = [
            args.signal_scale_dy,
            args.signal_scale_vbf,
            args.signal_scale_ssww,
            args.signal_scale_weinberg,
        ]

    elif args.signal_mode in [
        "total",
        "both",
    ]:

        relevant_scales = [
            args.signal_scale_total,
            args.signal_scale_weinberg,
        ]

    else:

        relevant_scales = []

    if any(
        is_auto_scale(value)
        for value in relevant_scales
    ):
        base += "_auto"

    return base


def make_prefit_hnl_component_label(component_key, mass, draw_scale):
    input_mixing_squared = get_hnl_input_mixing_squared(mass)

    if component_key in ["DY", "VBF"]:
        displayed_mixing_squared = input_mixing_squared * draw_scale
    elif component_key == "SSWW":
        displayed_mixing_squared = (
            input_mixing_squared * math.sqrt(draw_scale)
        )
    else:
        raise ValueError(
            f"Not an HNL component: {component_key}"
        )

    return (
        f"{SIGNAL_COMPONENTS[component_key]['label']}, "
        f"|V|^{{2}} = {displayed_mixing_squared:g}"
    )


def make_prefit_hnl_total_label(mass, mixing_multiplier):
    input_mixing_squared = get_hnl_input_mixing_squared(mass)
    displayed_mixing_squared = (
        input_mixing_squared * mixing_multiplier
    )

    return (
        f"Total HNL signal, |V|^{{2}} = "
        f"{displayed_mixing_squared:g}"
    )


def make_prefit_weinberg_label(plotter_scale):
    total_display_scale = WEINBERG_INPUT_SCALE * plotter_scale

    return (
        f"Weinberg signal, c_{{5}} = {WEINBERG_GENERATED_C5:g}, "
        f"#Lambda = {WEINBERG_GENERATED_LAMBDA_TEV:g} TeV "
        f"(x{total_display_scale:g})"
    )


def make_fitted_signal_label(base_label, display_scale):
    return f"{base_label} fitted (x{display_scale:g})"


def build_point_name(era, channel, mass, signal, tag_suffix):
    if signal == "Weinberg":
        return f"{era}_{channel}_{signal}{tag_suffix}"

    return f"{era}_{channel}_M{mass}_{signal}{tag_suffix}"


def build_input_file(wp, point_name, fit_subdir="Unblind"):
    return os.path.join(
        BASE_DIR,
        wp,
        point_name,
        "FitDiag",
        fit_subdir,
        f"fitDiagnostics_{point_name}.root",
    )

def get_plot_variant_subdir(
    logy,
    draw_data,
):
    """
    Return the output subdirectory for the plotting variant.

    Nominal:
        linear y-axis with observed data
        -> no extra subdirectory
    """
    if draw_data:

        if logy:
            return "logy"

        return None

    if logy:
        return "nodata_logy"

    return "nodata"


def get_combined_process(region_dir, proc):
    hsum = None

    for era in PROCESS_ERAS:

        if proc == "wz":
            names = [f"wz_{era}", f"wz_ewk_{era}"]
        else:
            names = [f"{proc}_{era}"]

        for name in names:

            h = region_dir.Get(name)
            if not h:
                continue

            if hsum is None:
                hsum = h.Clone(f"{proc}_combined")
                hsum.SetDirectory(0)
            else:
                hsum.Add(h)

    return hsum


def resolve_effective_signal_scales(
    signal_region_dir,
    fit_type,
    signal_mode,
    mass,
    point_signal,
    total_bkg,
    n,
    logy,
    scale_specs,
):
    """
    Convert any "auto" scale specifications into numeric,
    region-specific draw scales.

    Numeric scale specifications are kept unchanged.

    Separate HNL:
        DY / VBF / SSWW may independently be numeric or auto.

    Total / both HNL:
        one physical HNL benchmark is kept, so only the
        common total target ratio is used.

    Weinberg:
        may independently be numeric or auto.
    """

    scales = dict(scale_specs)

    if (
        signal_mode == "none"
        or signal_region_dir is None
    ):
        return scales

    bkg_peak = max(
        total_bkg.GetBinContent(i)
        for i in range(1, n + 1)
    )

    # Extremely defensive fallback.
    # Normally this should never be relevant for real plotted regions.
    if bkg_peak <= 0:

        print(
            "[AUTO SCALE] total background has no positive bins; "
            "unresolved auto scales fall back to x1."
        )

        for key in list(scales):
            if is_auto_scale(scales[key]):
                scales[key] = 1.0

        return scales

    uses_prefit_signal = (
        fit_type
        in [
            "shapes_prefit",
            "shapes_fit_b",
        ]
    )

    is_postfit_sb = (
        fit_type == "shapes_fit_s"
    )

    is_weinberg_point = (
        point_signal == "Weinberg"
    )

    plot_kind = (
        "logy"
        if logy
        else "linear"
    )

    def target_peak(component_key):
        return (
            auto_target_ratio(
                component_key,
                logy,
            )
            * bkg_peak
        )

    def hist_peak(hist):

        if not hist:
            return 0.0

        return max(
            max(
                0.0,
                hist.GetBinContent(i),
            )
            for i in range(1, n + 1)
        )

    def component_hist(component_key):

        return get_combined_process(
            signal_region_dir,
            SIGNAL_COMPONENTS[
                component_key
            ]["process"],
        )

    # ========================================================
    # Weinberg
    # ========================================================

    if is_weinberg_point:

        if not is_auto_scale(
            scales["Weinberg"]
        ):
            return scales

        if signal_mode == "separate":

            hist = component_hist(
                "Weinberg"
            )

        else:

            hist = signal_region_dir.Get(
                "total_signal"
            )

            if not hist:
                hist = component_hist(
                    "Weinberg"
                )

        raw_peak = hist_peak(hist)

        if raw_peak <= 0:

            scales["Weinberg"] = 1.0

            print(
                f"[AUTO SCALE] Weinberg has no positive bins "
                f"({fit_type}, {plot_kind}); using x1."
            )

            return scales

        desired_peak = target_peak(
            "Weinberg"
        )

        continuous_scale = (
            desired_peak / raw_peak
        )

        if uses_prefit_signal:

            # Legend reports:
            #
            #   x(
            #       WEINBERG_INPUT_SCALE
            #       * plotter_scale
            #     )
            #
            # so round the displayed quantity.
            desired_display_scale = (
                WEINBERG_INPUT_SCALE
                * continuous_scale
            )

            rounded_display_scale = (
                round_one_significant(
                    desired_display_scale
                )
            )

            auto_scale = (
                rounded_display_scale
                / WEINBERG_INPUT_SCALE
            )

            display_text = (
                f"x{rounded_display_scale:g}"
            )

        else:

            # Fitted S+B signal:
            # only a display multiplier.
            auto_scale = (
                round_one_significant(
                    continuous_scale
                )
            )

            display_text = (
                f"x{auto_scale:g}"
            )

        scales["Weinberg"] = (
            auto_scale
        )

        print(
            f"[AUTO SCALE] Weinberg "
            f"{fit_type} {plot_kind}: "
            f"Bmax={bkg_peak:g}, "
            f"target ratio="
            f"{auto_target_ratio('Weinberg', logy):g}, "
            f"target={desired_peak:g}, "
            f"{display_text}"
        )

        return scales

    # ========================================================
    # HNL: postfit S+B
    # ========================================================

    if is_postfit_sb:

        # ----------------------------------------------------
        # Separate:
        # each component may independently be auto/manual.
        # ----------------------------------------------------
        if signal_mode == "separate":

            for key in [
                "DY",
                "VBF",
                "SSWW",
            ]:

                if not is_auto_scale(
                    scales[key]
                ):
                    continue

                hist = component_hist(key)
                raw_peak = hist_peak(hist)

                if raw_peak <= 0:

                    scales[key] = 1.0
                    continue

                desired_peak = target_peak(
                    key
                )

                continuous_scale = (
                    desired_peak
                    / raw_peak
                )

                auto_scale = (
                    round_one_significant(
                        continuous_scale
                    )
                )

                scales[key] = auto_scale

                print(
                    f"[AUTO SCALE] {key} "
                    f"{fit_type} {plot_kind}: "
                    f"Bmax={bkg_peak:g}, "
                    f"target ratio="
                    f"{auto_target_ratio(key, logy):g}, "
                    f"target={desired_peak:g}, "
                    f"x{auto_scale:g}"
                )

            return scales

        # ----------------------------------------------------
        # Total/both:
        # one common fitted display multiplier.
        # ----------------------------------------------------

        if not is_auto_scale(
            scales["total"]
        ):
            return scales

        total_source = (
            signal_region_dir.Get(
                "total_signal"
            )
        )

        raw_peak = hist_peak(
            total_source
        )

        if raw_peak <= 0:

            scales["total"] = 1.0
            scales["DY"] = 1.0
            scales["VBF"] = 1.0
            scales["SSWW"] = 1.0

            return scales

        desired_peak = target_peak(
            "total"
        )

        continuous_scale = (
            desired_peak
            / raw_peak
        )

        auto_scale = (
            round_one_significant(
                continuous_scale
            )
        )

        scales["total"] = auto_scale
        scales["DY"] = auto_scale
        scales["VBF"] = auto_scale
        scales["SSWW"] = auto_scale

        print(
            f"[AUTO SCALE] total HNL "
            f"{fit_type} {plot_kind}: "
            f"Bmax={bkg_peak:g}, "
            f"target ratio="
            f"{auto_target_ratio('total', logy):g}, "
            f"target={desired_peak:g}, "
            f"x{auto_scale:g}"
        )

        return scales

    # ========================================================
    # HNL: prefit or B-only + prefit signal
    # ========================================================

    input_v2 = (
        get_hnl_input_mixing_squared(
            mass
        )
    )

    # --------------------------------------------------------
    # Separate:
    #
    # each process may have an independent physical |V|^2
    # benchmark and an independent target ratio.
    # --------------------------------------------------------

    if signal_mode == "separate":

        for key in [
            "DY",
            "VBF",
            "SSWW",
        ]:

            if not is_auto_scale(
                scales[key]
            ):
                continue

            hist = component_hist(key)
            raw_peak = hist_peak(hist)

            if raw_peak <= 0:

                scales[key] = 1.0
                continue

            desired_peak = target_peak(
                key
            )

            desired_draw_scale = (
                desired_peak
                / raw_peak
            )

            if key in [
                "DY",
                "VBF",
            ]:

                # DY/Wgamma ~ |V|^2
                desired_v2 = (
                    input_v2
                    * desired_draw_scale
                )

                rounded_v2 = (
                    round_one_significant(
                        desired_v2
                    )
                )

                auto_scale = (
                    rounded_v2
                    / input_v2
                )

            else:

                # SSWW ~ |V|^4
                #
                # hist draw scale
                #   = (V2_display / V2_input)^2
                desired_v2 = (
                    input_v2
                    * math.sqrt(
                        desired_draw_scale
                    )
                )

                rounded_v2 = (
                    round_one_significant(
                        desired_v2
                    )
                )

                auto_scale = (
                    rounded_v2
                    / input_v2
                ) ** 2

            scales[key] = auto_scale

            print(
                f"[AUTO SCALE] {key} "
                f"{fit_type} {plot_kind}: "
                f"Bmax={bkg_peak:g}, "
                f"target ratio="
                f"{auto_target_ratio(key, logy):g}, "
                f"target={desired_peak:g}, "
                f"|V|^2={rounded_v2:g}, "
                f"hist x{auto_scale:g}"
            )

        return scales

    # ========================================================
    # Total / both:
    #
    # A single physical |V|^2 benchmark must be maintained.
    # ========================================================

    if not is_auto_scale(
        scales["total"]
    ):
        return scales

    component_values = {}

    for key in [
        "DY",
        "VBF",
        "SSWW",
    ]:

        hist = component_hist(key)

        if hist:

            component_values[key] = [
                max(
                    0.0,
                    hist.GetBinContent(i),
                )
                for i in range(
                    1,
                    n + 1,
                )
            ]

        else:

            component_values[key] = (
                [0.0] * n
            )

    if not any(
        value > 0
        for values
        in component_values.values()
        for value in values
    ):

        scales["total"] = 1.0
        scales["DY"] = 1.0
        scales["VBF"] = 1.0
        scales["SSWW"] = 1.0

        print(
            f"[AUTO SCALE] no positive HNL components "
            f"({fit_type}, {plot_kind}); using x1."
        )

        return scales

    def total_peak_at(
        mixing_multiplier,
    ):

        return max(
            (
                component_values["DY"][i]
                + component_values["VBF"][i]
            )
            * mixing_multiplier
            +
            component_values["SSWW"][i]
            * mixing_multiplier
            * mixing_multiplier

            for i in range(n)
        )

    desired_peak = target_peak(
        "total"
    )

    # Bracket the solution.
    low = 0.0
    high = 1.0

    while (
        total_peak_at(high)
        < desired_peak
    ):

        high *= 10.0

        if high > 1.0e12:

            scales["total"] = 1.0
            scales["DY"] = 1.0
            scales["VBF"] = 1.0
            scales["SSWW"] = 1.0

            print(
                "[AUTO SCALE] could not bracket "
                "HNL total scale; using x1."
            )

            return scales

    # Find continuous S.
    for _ in range(100):

        middle = (
            0.5
            * (low + high)
        )

        if (
            total_peak_at(middle)
            < desired_peak
        ):

            low = middle

        else:

            high = middle

    continuous_multiplier = (
        0.5
        * (low + high)
    )

    # The displayed physical quantity is |V|^2.
    # Round that, not the raw histogram multiplier.
    desired_v2 = (
        input_v2
        * continuous_multiplier
    )

    rounded_v2 = (
        round_one_significant(
            desired_v2
        )
    )

    auto_multiplier = (
        rounded_v2
        / input_v2
    )

    scales["total"] = (
        auto_multiplier
    )

    scales["DY"] = (
        auto_multiplier
    )

    scales["VBF"] = (
        auto_multiplier
    )

    scales["SSWW"] = (
        auto_multiplier
        * auto_multiplier
    )

    final_peak = (
        total_peak_at(
            auto_multiplier
        )
    )

    print(
        f"[AUTO SCALE] total HNL "
        f"{fit_type} {plot_kind}: "
        f"Bmax={bkg_peak:g}, "
        f"target ratio="
        f"{auto_target_ratio('total', logy):g}, "
        f"target={desired_peak:g}, "
        f"|V|^2={rounded_v2:g}, "
        f"S={auto_multiplier:g}, "
        f"final signal max={final_peak:g}"
    )

    return scales


def root_color(color_spec):
    """
    Convert a color specification to a ROOT color index.

    Accepted examples:
        "#D55E00"
        "red"
        "kRed"
        "kRed+1"
        "kAzure-9"
        "632"
    """
    spec = str(color_spec).strip()

    if not spec:
        raise ValueError(
            "Empty ROOT color specification."
        )

    # --------------------------------------------------------
    # Numeric ROOT color index, for example "632"
    # --------------------------------------------------------
    try:
        return int(spec)
    except ValueError:
        pass

    # --------------------------------------------------------
    # ROOT symbolic constant:
    #   kRed
    #   kRed+1
    #   kAzure-9
    # Spaces around +/- are also accepted.
    # --------------------------------------------------------
    match = re.fullmatch(
        r"(k[A-Za-z_][A-Za-z0-9_]*)"
        r"(?:\s*([+-])\s*(\d+))?",
        spec,
    )

    if match:

        constant_name = match.group(1)
        operator = match.group(2)
        offset_text = match.group(3)

        if not hasattr(ROOT, constant_name):
            raise ValueError(
                f"Unknown ROOT color constant: "
                f"{constant_name}"
            )

        color_index = int(
            getattr(ROOT, constant_name)
        )

        if offset_text is not None:

            offset = int(offset_text)

            if operator == "+":
                color_index += offset
            else:
                color_index -= offset

        return color_index

    # --------------------------------------------------------
    # Hex string or a color name understood by TColor,
    # for example "#D55E00" or "red".
    # --------------------------------------------------------
    color_index = ROOT.TColor.GetColor(spec)

    if color_index < 0:
        raise ValueError(
            f"Could not interpret ROOT color: "
            f"{color_spec!r}. "
            "Use a hex color, a ROOT constant such as "
            "'kRed+1', a color name, or a numeric index."
        )

    return color_index


def prepare_signal_hist(
    hist,
    unique_name,
    label,
    scale,
    color_spec,
    line_style,
    line_width,
):
    """
    Apply only the requested drawing scale/style.

    The label is already fully formatted by the caller. In particular,
    this function no longer appends a generic "xN" suffix, because prefit
    HNL labels are expressed as |V|^2 and postfit S+B labels are expressed
    as fitted (xN).
    """
    hist.SetName(unique_name)
    hist.SetDirectory(0)

    raw_yield = hist.Integral()

    hist.Scale(scale)

    hist.SetLineColor(root_color(color_spec))
    hist.SetLineStyle(line_style)
    hist.SetLineWidth(line_width)
    hist.SetFillStyle(0)

    return {
        "hist": hist,
        "label": label,
        "raw_yield": raw_yield,
        "draw_yield": hist.Integral(),
    }


def load_signal_curves(
    region_dir,
    signal_region_dir,
    fit_type,
    signal_mode,
    signal_scales,
    signal_colors,
    mass,
    point_signal,
):
    """
    Return a list of signal curves to draw.

    Prefit and postfit B-only benchmark HNL:
      - the signal source is shapes_prefit.
      - separate: each component uses its own requested draw scale.
      - total/both: DY and Wgamma use S, SSWW uses S^2, and the total
        HNL curve is reconstructed from those scaled components.

    Postfit S+B:
      - the signal source is shapes_fit_s, so the histograms are already
        fitted signals.
      - total/both therefore use the Combine-provided fitted total_signal.
      - in both mode all fitted HNL components receive the same common
        display factor S so that their fitted relative composition is kept.
      - separate mode may still display-scale fitted components independently.
    """
    if signal_mode == "none":
        return []

    if fit_type not in [
        "shapes_prefit",
        "shapes_fit_b",
        "shapes_fit_s",
    ]:
        return []

    uses_prefit_signal = fit_type in [
        "shapes_prefit",
        "shapes_fit_b",
    ]

    is_postfit_sb = fit_type == "shapes_fit_s"

    is_weinberg_point = point_signal == "Weinberg"

    if signal_region_dir is None:

        if fit_type == "shapes_fit_b":
            print(
                "[WARNING] No matching shapes_prefit region was found for",
                region_dir.GetName(),
                "- no signal overlay will be drawn in postfit B-only.",
            )
            return []

        signal_region_dir = region_dir

    region = region_dir.GetName()
    safe_region = region.replace("/", "_")

    curves = []

    component_raw_yield = 0.0
    total_raw_yield = None

    # --------------------------------------------------------
    # Individual signal components
    # --------------------------------------------------------
    if signal_mode in ["separate", "both"]:

        for key, config in SIGNAL_COMPONENTS.items():

            # Do not mix HNL and Weinberg components in the same point.
            if is_weinberg_point and key != "Weinberg":
                continue

            if not is_weinberg_point and key == "Weinberg":
                continue

            hist = get_combined_process(
                signal_region_dir,
                config["process"],
            )

            if not hist:
                print(
                    f"[INFO] {config['process']}_<era> not found in",
                    f"signal source for {fit_type}/{region}",
                )
                continue

            # ------------------------------------------------
            # Scale to use for the component
            # ------------------------------------------------
            if is_postfit_sb and signal_mode == "both":

                # In shapes_fit_s these are already fitted components.
                # Use one common display multiplier so that the fitted
                # DY/Wgamma/SSWW composition is not distorted.
                if key == "Weinberg":
                    component_scale = signal_scales["Weinberg"]
                else:
                    component_scale = signal_scales["total"]

            else:
                component_scale = signal_scales[key]

            # ------------------------------------------------
            # Legend label
            # ------------------------------------------------
            if uses_prefit_signal:

                # Both prefit and postfit B-only use the same
                # physical benchmark signal from shapes_prefit.
                if key == "Weinberg":
                    draw_label = make_prefit_weinberg_label(
                        component_scale
                    )
                else:
                    draw_label = make_prefit_hnl_component_label(
                        component_key=key,
                        mass=mass,
                        draw_scale=component_scale,
                    )

            else:

                # shapes_fit_s contains a genuinely fitted signal.
                draw_label = make_fitted_signal_label(
                    base_label=config["label"],
                    display_scale=component_scale,
                )

            curve = prepare_signal_hist(
                hist=hist,
                unique_name=(
                    f"{config['process']}_{fit_type}_{safe_region}"
                ),
                label=draw_label,
                scale=component_scale,
                color_spec=signal_colors[key],
                line_style=config["line_style"],
                line_width=3,
            )

            curves.append(curve)
            component_raw_yield += curve["raw_yield"]

            print(
                f"{(key + ' raw'):16s}"
                f" {curve['raw_yield']:10.3f}"
            )

            print(
                f"{(key + ' draw'):16s}"
                f" {curve['draw_yield']:10.3f}"
                f"  (x{component_scale:g})"
            )

    # --------------------------------------------------------
    # Combine-provided unscaled total_signal from the selected
    # signal source.
    #
    # For postfit B-only this deliberately comes from
    # shapes_prefit, not shapes_fit_b.
    # --------------------------------------------------------
    total_source = signal_region_dir.Get("total_signal")

    if total_source:
        total_raw_yield = total_source.Integral()

    # --------------------------------------------------------
    # Total signal
    # --------------------------------------------------------
    if signal_mode in ["total", "both"]:

        # ----------------------------------------------------
        # PREFIT-BASED HNL BENCHMARK TOTAL
        #
        # Used for BOTH:
        #   - prefit
        #   - postfit B-only
        #
        # A single Scale() on Combine total_signal would be wrong:
        #
        #   DY/Wgamma scale with |V|^2
        #   SSWW scales with |V|^4
        #
        # Therefore rebuild the total from the individually
        # scaled components.
        # ----------------------------------------------------
        if uses_prefit_signal and not is_weinberg_point:

            total_hist = None

            for key in ["DY", "VBF", "SSWW"]:

                component_hist = get_combined_process(
                    signal_region_dir,
                    SIGNAL_COMPONENTS[key]["process"],
                )

                if not component_hist:
                    continue

                component_hist.Scale(
                    signal_scales[key]
                )

                if total_hist is None:

                    total_hist = component_hist.Clone(
                        f"total_hnl_rebuilt_{fit_type}_{safe_region}"
                    )

                    total_hist.SetDirectory(0)

                else:

                    total_hist.Add(
                        component_hist
                    )

            if total_hist:

                curve = prepare_signal_hist(
                    hist=total_hist,
                    unique_name=(
                        f"total_hnl_rebuilt_{fit_type}_{safe_region}"
                    ),
                    label=make_prefit_hnl_total_label(
                        mass=mass,
                        mixing_multiplier=signal_scales["total"],
                    ),
                    scale=1.0,
                    color_spec=signal_colors["total"],
                    line_style=1,
                    line_width=4,
                )

                curves.append(curve)

                print(
                    f"{'total sig. raw':16s}"
                    f" {total_raw_yield if total_raw_yield is not None else 0.0:10.3f}"
                )

                print(
                    f"{'total sig. draw':16s}"
                    f" {curve['draw_yield']:10.3f}"
                    "  (rebuilt with DY/VBF xS, SSWW xS^2)"
                )

            else:

                print(
                    "[INFO] No HNL components found for rebuilt total in",
                    f"signal source for {fit_type}/{region}",
                )

        # ----------------------------------------------------
        # PREFIT-BASED WEINBERG BENCHMARK TOTAL
        #
        # Again used for BOTH:
        #   - prefit
        #   - postfit B-only
        # ----------------------------------------------------
        elif uses_prefit_signal and is_weinberg_point:

            if total_source:

                hist = total_source.Clone(
                    f"total_signal_{fit_type}_{safe_region}"
                )

                hist.SetDirectory(0)

                total_scale = signal_scales["Weinberg"]

                curve = prepare_signal_hist(
                    hist=hist,
                    unique_name=(
                        f"total_signal_{fit_type}_{safe_region}"
                    ),
                    label=make_prefit_weinberg_label(
                        total_scale
                    ),
                    scale=total_scale,
                    color_spec=signal_colors["total"],
                    line_style=1,
                    line_width=4,
                )

                curves.append(curve)

                print(
                    f"{'total sig. raw':16s}"
                    f" {curve['raw_yield']:10.3f}"
                )

                print(
                    f"{'total sig. draw':16s}"
                    f" {curve['draw_yield']:10.3f}"
                    f"  (x{total_scale:g})"
                )

            else:

                print(
                    "[INFO] total_signal not found in",
                    f"signal source for {fit_type}/{region}",
                )

        # ----------------------------------------------------
        # POSTFIT S+B TOTAL
        #
        # This is a genuinely fitted signal.
        #
        # Do NOT reinterpret its normalization as a new |V|^2
        # point. Keep Combine total_signal and apply only a
        # common display multiplier.
        # ----------------------------------------------------
        elif is_postfit_sb:

            if total_source:

                hist = total_source.Clone(
                    f"total_signal_{fit_type}_{safe_region}"
                )

                hist.SetDirectory(0)

                if is_weinberg_point:

                    total_scale = signal_scales["Weinberg"]
                    base_label = "Weinberg signal"

                else:

                    total_scale = signal_scales["total"]
                    base_label = "Total signal"

                curve = prepare_signal_hist(
                    hist=hist,
                    unique_name=(
                        f"total_signal_{fit_type}_{safe_region}"
                    ),
                    label=make_fitted_signal_label(
                        base_label=base_label,
                        display_scale=total_scale,
                    ),
                    scale=total_scale,
                    color_spec=signal_colors["total"],
                    line_style=1,
                    line_width=4,
                )

                curves.append(curve)

                print(
                    f"{'total sig. raw':16s}"
                    f" {curve['raw_yield']:10.3f}"
                )

                print(
                    f"{'total sig. draw':16s}"
                    f" {curve['draw_yield']:10.3f}"
                    f"  (fitted x{total_scale:g})"
                )

            else:

                print(
                    "[INFO] total_signal not found in",
                    f"{fit_type}/{region}",
                )

    # --------------------------------------------------------
    # Diagnostic check
    #
    # Compare the unscaled component sum to total_signal from
    # the SAME signal source.
    #
    # Therefore:
    #
    #   prefit      -> prefit components vs prefit total
    #   postfit B   -> prefit components vs prefit total
    #   postfit S+B -> fitted components vs fitted total
    # --------------------------------------------------------
    if (
        signal_mode == "both"
        and total_raw_yield is not None
    ):

        difference = (
            component_raw_yield
            - total_raw_yield
        )

        if abs(total_raw_yield) > 1.0e-12:

            relative_difference = (
                difference
                / total_raw_yield
            )

        else:

            relative_difference = 0.0

        print(
            f"{'signal check':16s}"
            f" components-total = {difference:+.6g}"
            f"  ({relative_difference:+.3%})"
        )

    return curves


def make_unc_band(hist):
    n = hist.GetNbinsX()

    g = ROOT.TGraphAsymmErrors(n)

    for i in range(1, n + 1):

        x = hist.GetBinCenter(i)
        ex = hist.GetBinWidth(i) / 2.

        y = hist.GetBinContent(i)
        ey = hist.GetBinError(i)

        g.SetPoint(i - 1, x, y)
        g.SetPointError(i - 1, ex, ex, ey, ey)

    g.SetFillColor(ROOT.kGray + 2)
    g.SetFillStyle(3344)
    g.SetLineColor(ROOT.kGray + 2)

    return g


def make_ratio_graph(data_graph, bkg_hist):

    g = ROOT.TGraphAsymmErrors()

    n = data_graph.GetN()

    ip = 0

    for i in range(n):

        x = data_graph.GetPointX(i)
        y = data_graph.GetPointY(i)

        bkg_bin = bkg_hist.GetXaxis().FindFixBin(x)

        if (
            bkg_bin < 1
            or bkg_bin > bkg_hist.GetNbinsX()
        ):
            continue

        bkg = bkg_hist.GetBinContent(bkg_bin)

        if bkg <= 0:
            continue

        ratio = y / bkg

        exl = data_graph.GetErrorXlow(i)
        exh = data_graph.GetErrorXhigh(i)

        eyl = data_graph.GetErrorYlow(i) / bkg
        eyh = data_graph.GetErrorYhigh(i) / bkg

        g.SetPoint(ip, x, ratio)
        g.SetPointError(ip, exl, exh, eyl, eyh)

        ip += 1

    return g

def make_background_pseudodata_graph(
    bkg_hist,
    stat_error_hist=None,
):
    """
    Make pseudo-data points centered exactly on the background prediction.

    Central value:
        D_i = B_i

    Error:
        - if stat_error_hist is given:
              use stat_error_hist.GetBinError(i)
        - otherwise:
              use sqrt(B_i), interpreted as expected counting-stat error

    The central values always come from bkg_hist, so the ratio is exactly one.
    """
    n = bkg_hist.GetNbinsX()

    if (
        stat_error_hist is not None
        and stat_error_hist.GetNbinsX() != n
    ):
        raise RuntimeError(
            "stat_error_hist and bkg_hist have different binning."
        )

    graph = ROOT.TGraphAsymmErrors(n)
    graph.SetName(
        f"pseudo_data_{bkg_hist.GetName()}"
    )

    for i in range(1, n + 1):

        x = bkg_hist.GetBinCenter(i)
        y = bkg_hist.GetBinContent(i)

        if stat_error_hist is not None:
            stat_error = stat_error_hist.GetBinError(i)
        else:
            stat_error = math.sqrt(max(y, 0.0))

        graph.SetPoint(
            i - 1,
            x,
            y,
        )

        # No horizontal error bar for the pseudo-data marker.
        graph.SetPointError(
            i - 1,
            0.0,
            0.0,
            stat_error,
            stat_error,
        )

    graph.SetMarkerStyle(20)
    graph.SetMarkerSize(0.9)
    graph.SetMarkerColor(ROOT.kBlack)
    graph.SetLineColor(ROOT.kBlack)

    return graph

def make_ratio_band(hist):

    n = hist.GetNbinsX()

    g = ROOT.TGraphAsymmErrors(n)

    for i in range(1, n + 1):

        x = hist.GetBinCenter(i)

        ex = hist.GetBinWidth(i) / 2.

        y = hist.GetBinContent(i)

        ey = hist.GetBinError(i)

        ratio_err = 0.

        if y > 0:

            ratio_err = ey / y

        g.SetPoint(i - 1, x, 1.0)

        g.SetPointError(i - 1, ex, ex, ratio_err, ratio_err)

    g.SetFillColor(ROOT.kGray + 2)

    g.SetFillStyle(3344)

    g.SetLineColor(ROOT.kGray + 2)

    return g


# AN2019_206_v7, Tables 39 and 44-46; transcribed with row/column checks.
SR1_EDGES = {'MuMu': {400: [0, 365, 395, 450, 555, 5000],
          450: [0, 415, 440, 495, 755, 5000],
          500: [0, 440, 485, 535, 860, 5000],
          600: [0, 480, 555, 580, 655, 5000],
          700: [0, 555, 620, 665, 830, 5000],
          800: [0, 555, 615, 760, 910, 5000],
          900: [0, 610, 760, 860, 1015, 5000],
          1000: [0, 755, 860, 965, 1135, 5000],
          1100: [0, 555, 755, 885, 1000, 5000],
          1200: [0, 650, 850, 960, 1115, 5000],
          1500: [0, 590, 760, 970, 1185, 5000],
          2000: [0, 595, 785, 965, 1185, 5000]},
 'EE': {400: [0, 330, 390, 440, 740, 5000],
        450: [0, 410, 450, 495, 905, 5000],
        500: [0, 455, 490, 560, 905, 5000],
        600: [0, 570, 600, 655, 905, 5000],
        700: [0, 630, 685, 765, 905, 5000],
        800: [0, 670, 735, 780, 870, 5000],
        900: [0, 740, 810, 875, 990, 5000],
        1000: [0, 750, 850, 925, 1070, 5000],
        1100: [0, 770, 905, 1060, 1230, 5000],
        1200: [0, 905, 1075, 1150, 1290, 5000],
        1500: [0, 750, 905, 1185, 1365, 5000],
        2000: [0, 750, 905, 1185, 1420, 5000]},
 'EMu': {400: [0, 355, 385, 440, 665, 5000],
         450: [0, 410, 445, 490, 665, 5000],
         500: [0, 460, 495, 565, 665, 5000],
         600: [0, 540, 595, 640, 680, 5000],
         700: [0, 595, 655, 680, 770, 5000],
         800: [0, 650, 715, 760, 890, 5000],
         900: [0, 665, 740, 870, 990, 5000],
         1000: [0, 810, 915, 965, 1095, 5000],
         1100: [0, 805, 960, 1035, 1250, 5000],
         1200: [0, 760, 965, 1075, 1425, 5000],
         1500: [0, 735, 915, 1075, 1405, 5000],
         2000: [0, 700, 935, 1080, 1500, 5000]}}

SR3_BDT_UPPER_EDGES = {'MuMu': {100: [-0.115, -0.045, -0.005, 0.025, 0.06, 0.1, 0.125, 0.145, 0.165, 0.18, 0.22, 0.25, 0.27, 0.275, 1.0],
          125: [-0.13, -0.08, -0.035, 0.01, 0.055, 0.085, 0.115, 0.14, 0.165, 0.19, 0.225, 0.245, 0.265, 0.32, 1.0],
          150: [-0.12, -0.075, -0.025, 0.01, 0.035, 0.06, 0.085, 0.105, 0.135, 0.15, 0.175, 0.2, 0.235, 0.265, 1.0],
          200: [-0.08, -0.03, 0.01, 0.035, 0.06, 0.075, 0.1, 0.11, 0.13, 0.15, 0.17, 0.19, 0.21, 0.235, 1.0],
          250: [-0.05, 0.0, 0.02, 0.04, 0.06, 0.07, 0.09, 0.1, 0.11, 0.13, 0.14, 0.155, 0.175, 0.21, 1.0],
          300: [-0.095, -0.04, -0.005, 0.025, 0.045, 0.065, 0.08, 0.09, 0.11, 0.125, 0.15, 0.175, 0.205, 0.24, 1.0],
          350: [-0.05, 0.0, 0.03, 0.055, 0.065, 0.075, 0.095, 0.11, 0.125, 0.135, 0.16, 0.18, 0.2, 0.245, 1.0],
          400: [-0.04, 0.01, 0.04, 0.06, 0.065, 0.08, 0.095, 0.105, 0.12, 0.135, 0.155, 0.19, 0.215, 0.245, 1.0],
          450: [-0.05, -0.005, 0.025, 0.04, 0.045, 0.065, 0.08, 0.1, 0.11, 0.125, 0.145, 0.165, 0.185, 0.22, 1.0],
          500: [-0.095, -0.035, -0.005, 0.01, 0.035, 0.055, 0.065, 0.085, 0.1, 0.115, 0.13, 0.15, 0.175, 0.215, 1.0]},
 'EE': {100: [-0.135, -0.08, -0.015, 0.03, 0.085, 0.105, 0.125, 0.15, 0.185, 0.235, 0.26, 0.305, 0.325, 0.35, 1.0],
        125: [-0.125, -0.07, -0.03, 0.015, 0.06, 0.095, 0.12, 0.14, 0.155, 0.19, 0.21, 0.235, 0.255, 0.28, 1.0],
        150: [-0.155, -0.085, -0.04, 0.01, 0.04, 0.07, 0.105, 0.13, 0.165, 0.2, 0.225, 0.255, 0.275, 0.3, 1.0],
        200: [-0.11, -0.045, -0.01, 0.015, 0.04, 0.075, 0.09, 0.11, 0.125, 0.145, 0.17, 0.2, 0.23, 0.245, 1.0],
        250: [-0.05, 0.0, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12, 0.13, 0.145, 0.165, 0.185, 0.2, 0.235, 1.0],
        300: [-0.065, -0.02, 0.005, 0.03, 0.05, 0.065, 0.09, 0.105, 0.11, 0.13, 0.145, 0.165, 0.18, 0.2, 1.0],
        350: [-0.07, -0.015, 0.025, 0.05, 0.075, 0.095, 0.11, 0.14, 0.155, 0.165, 0.18, 0.205, 0.225, 0.255, 1.0],
        400: [-0.06, 0.0, 0.035, 0.065, 0.09, 0.105, 0.125, 0.135, 0.155, 0.165, 0.19, 0.2, 0.23, 0.275, 1.0],
        450: [-0.05, 0.02, 0.04, 0.06, 0.08, 0.1, 0.11, 0.12, 0.13, 0.14, 0.16, 0.18, 0.205, 0.255, 1.0],
        500: [-0.09, -0.03, 0.0, 0.03, 0.045, 0.06, 0.08, 0.085, 0.095, 0.105, 0.12, 0.135, 0.16, 0.21, 1.0]},
 'EMu': {100: [-0.14, -0.075, -0.03, -0.005, 0.03, 0.06, 0.09, 0.13, 0.16, 0.185, 0.21, 0.235, 0.265, 0.31, 1.0],
         125: [-0.13, -0.085, -0.045, -0.005, 0.02, 0.05, 0.08, 0.11, 0.14, 0.17, 0.2, 0.225, 0.245, 0.285, 1.0],
         150: [-0.11, -0.06, -0.03, 0.0, 0.025, 0.05, 0.075, 0.1, 0.125, 0.15, 0.165, 0.185, 0.205, 0.24, 1.0],
         200: [-0.085, -0.025, 0.01, 0.035, 0.055, 0.075, 0.09, 0.105, 0.13, 0.16, 0.175, 0.185, 0.205, 0.235, 1.0],
         250: [-0.055, -0.005, 0.03, 0.055, 0.075, 0.095, 0.115, 0.14, 0.15, 0.165, 0.18, 0.195, 0.215, 0.25, 1.0],
         300: [-0.05, 0.0, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12, 0.13, 0.15, 0.17, 0.19, 0.205, 0.23, 1.0],
         350: [-0.04, 0.02, 0.05, 0.065, 0.085, 0.105, 0.12, 0.14, 0.155, 0.165, 0.185, 0.2, 0.22, 0.255, 1.0],
         400: [-0.06, -0.01, 0.025, 0.05, 0.07, 0.08, 0.095, 0.115, 0.13, 0.14, 0.15, 0.17, 0.19, 0.235, 1.0],
         450: [0.0, 0.04, 0.06, 0.08, 0.1, 0.11, 0.12, 0.125, 0.14, 0.155, 0.17, 0.19, 0.21, 0.255, 1.0],
         500: [-0.05, 0.0, 0.02, 0.04, 0.06, 0.07, 0.08, 0.1, 0.12, 0.13, 0.145, 0.165, 0.185, 0.21, 1.0]}}

# ------------------------------------------------------------
# Region metadata. Display bins are categorical slots of equal width.
# No rebinning, density conversion, or nuisance-error recomputation is done.
# ------------------------------------------------------------

class BinningError(ValueError):
    pass


SR3H_THRESHOLDS = {
    "MuMu": [[270,340,420,550], [260,340,460,600], [210,270,360,460], [210,270,340,460]],
    "EE":   [[360,600,800,1100], [280,420,550,800], [300,440,650,1000], [220,300,420,550]],
    "EMu":  [[380,550,750,900], [320,460,600,900], [260,380,500,750], [250,360,460,700]],
}
SR2_EDGES = {
    "MuMu": [[0,2.2,3.6,4.5,10], [0,0.9,1.5,2.3,10]],
    "EE":   [[0,0.9,2.4,3.2,10], [0,0.7,1.3,2.5,10]],
    "EMu":  [[0,0.9,2.1,3.2,10], [0,0.7,1.3,2.5,10]],
}
PROCESS_LABELS = {"cf":"Charge misid.", "fake":"Nonprompt", "wz":"WZ", "zz":"ZZ",
                  "ww":"W^{#pm}W^{#pm}", "zg":"Z#gamma", "mc_others":"Other"}
REGION_TITLES = {"sr1":"SR1", "sr2":"SR2", "sr3":"SR3",
                 "wz_cr1":"WZ CR1", "wz_cr2":"WZ CR2", "wz_cr3":"WZ CR3",
                 "zg_cr":"Z#gamma CR", "zz_cr":"ZZ CR"}
LUMI = {"2016preVFP":19.5, "2016postVFP":16.8, "2017":41.5, "2018":59.8,
        "Run2":138.0, "Run2Sum":138.0}


def canonical_region(raw):
    """Recognise names even when Combine prepends era/channel prefixes."""
    text = raw.lower().replace("-", "_")
    for pattern, replacement in (
        (r"(?:^|_)wz_?cr_?([123])(?:_|$)", lambda m: "wz_cr"+m.group(1)),
        (r"(?:^|_)(zg|zz)_?cr(?:_|$)", lambda m: m.group(1)+"_cr"),
        (r"(?:^|_)cr([123])(?:low|high|l|h)?_?(invbjet|invbtag|invmet|ib|im)(?:_|$)",
         lambda m: "cr"+m.group(1)+("_InvBJet" if m.group(2) in ("invbjet","invbtag","ib") else "_InvMET")),
        (r"(?:^|_)sr([123])(?:low|high|l|h)?(?:_|$)", lambda m: "sr"+m.group(1)),
    ):
        found = re.search(pattern, text)
        if found:
            return replacement(found)
    return raw


def region_title(key):
    if key in REGION_TITLES:
        return REGION_TITLES[key]

    # Compact CR naming used in the analysis plots:
    #   cr1_InvBJet -> CR1 IB
    #   cr1_InvMET  -> CR1 IM
    match = re.fullmatch(r"cr([123])_(InvBJet|InvMET)", key, re.I)
    if match:
        suffix = "IB" if match.group(2).lower() == "invbjet" else "IM"
        return f"CR{match.group(1)} {suffix}"

    return key.replace("cr", "CR", 1)


def resolve_channel(raw, requested):
    found = []
    for pattern, ch in ((r"(?:^|_)(?:mumu|mm)(?:_|$)", "MuMu"),
                        (r"(?:^|_)ee(?:_|$)", "EE"),
                        (r"(?:^|_)(?:emu|em)(?:_|$)", "EMu")):
        if re.search(pattern, raw.lower()):
            found.append(ch)
    if len(found) == 1:
        return found[0]
    if len(found) > 1:
        raise BinningError("Ambiguous flavour in region name: "+raw)
    return requested


def resolve_era(raw, requested):
    found = [e for e in ERAS if re.search(r"(?:^|_)"+re.escape(e)+r"(?:_|$)", raw, re.I)]
    if len(found) > 1:
        raise BinningError("Ambiguous era in region name: "+raw)
    return found[0] if found else requested


def sr3_low_mass(raw, mass, point_signal, mode):
    if mode != "auto":
        return mode == "low"
    if re.search(r"(?:sr|cr)3(?:low|l)(?:_|$)", raw.lower()):
        return True
    if re.search(r"(?:sr|cr)3(?:high|h)(?:_|$)", raw.lower()):
        return False
    return point_signal != "Weinberg" and mass is not None and int(mass) <= 500


def number_text(value, decimals=None):
    return f"{value:.{decimals}f}" if decimals is not None else f"{value:g}"


def interval_label(low, high, closure="left", decimals=None):
    fmt = lambda x: number_text(x, decimals)
    if low is None:
        return ("#leq " if closure == "right" else "< ") + fmt(high)
    if high is None:
        return ("> " if closure == "right" else "#geq ") + fmt(low)
    if decimals is not None:
        return f"({fmt(low)}, {fmt(high)}]" if closure == "right" else f"[{fmt(low)}, {fmt(high)})"
    # The axis shows interval endpoints; exact boundary conventions are in the TXT key.
    return fmt(low) + "#minus " + fmt(high)


def new_spec(key, axis_title, source):
    return {"region_key":key, "title":region_title(key), "axis_title":axis_title,
            "source":source, "bins":[], "groups":[], "notes":[]}


def add_intervals(spec, edges, prefix, group="", closure="left", decimals=None, open_last=False):
    start = len(spec["bins"])+1
    count = len(edges)-1
    for j in range(count):
        lo, hi = edges[j:j+2]
        if open_last and j == count-1:
            hi = None
        label = interval_label(lo,hi,closure,decimals)
        spec["bins"].append({"code":prefix+chr(97+j), "cut_label":label,
                             "low":lo, "high":hi, "closure":closure, "decimals":decimals,
                             "group":group, "table_bins":[start+j]})
    if group:
        spec["groups"].append({"first":start, "last":start+count-1, "label":group})


def inclusive_spec(key, source):
    spec = new_spec(key, "", source)
    spec["bins"] = [{"code":region_title(key), "cut_label":region_title(key),
                      "group":"", "table_bins":[], "definition":"Inclusive control-region yield"}]
    return spec


def builtin_spec(key, channel, mass, low_mass, sr1_binning_mass=None):
    if key in ("zg_cr", "zz_cr"):
        return inclusive_spec(key, "AN v7, Tables 63-64")
    if key.startswith("cr1_"):
        return inclusive_spec(key, "AN v7, Sec. 7.4.1, lines 1074-1075")
    if key == "cr2_InvBJet":
        spec = inclusive_spec(key, "AN v7, Table 40")
        spec["bins"][0]["definition"] = "Inclusive CR2 IB; 0 < HT/pT(l1) < 10"
        return spec
    if key == "cr2_InvMET":
        spec = new_spec(key, "H_{T}/p_{T}(#ell_{1})", "AN v7, Table 40")
        add_intervals(spec, [0,3,5,10], "C2I", closure="open")
        return spec
    if key in ("wz_cr1", "wz_cr2", "wz_cr3"):
        axis, edges, prefix = {
            "wz_cr1":("m(#ell_{1}J) [GeV]", [None,750,None], "W1"),
            "wz_cr2":("H_{T}/p_{T}(#ell_{1})", [None,2,4,None], "W2"),
            "wz_cr3":("L_{T} [GeV]", [None,100,200,300,400,None], "W3"),
        }[key]
        spec = new_spec(key, axis, "AN v7, Table 64; common to eras/flavours/masses")
        add_intervals(spec, edges, prefix)
        return spec
    if key.startswith("cr3_"):
        # CR3 follows exactly the same low/high-mass split as SR3.
        # Keep the IB/IM qualifier but make the low/high regime explicit
        # in the publication title: CR3L IB, CR3L IM, CR3H IB, CR3H IM.
        suffix = "IB" if key.lower().endswith("invbjet") else "IM"
        if low_mass:
            spec = new_spec(key, "BDT score", "AN v7, Table 43")
            spec["title"] = f"CR3L {suffix}"
            add_intervals(spec, [None,-.35,-.25,-.15,-.1,-.05,0,.05,1], "C3L", decimals=2)
        else:
            spec = new_spec(key, "L_{T} [GeV]", "AN v7, Table 42")
            spec["title"] = f"CR3H {suffix}"
            add_intervals(spec, [None,150,200,300,None], "C3H")
        return spec
    if key not in ("sr1", "sr2", "sr3"):
        raise BinningError(f"No bin definitions for {key!r}; supply a --bin-config rule.")
    if channel not in SR2_EDGES:
        raise BinningError("Flavour-dependent cuts require EE/EMu/MuMu. For -c 3ch, "
                           "use named region prefixes or a --bin-config channel override.")
    if key == "sr1":
        chosen = sr1_binning_mass
        if chosen is None and mass is not None:
            # SR1 templates reuse the nearest lower available Table-39 mass
            # binning, with under/overflow clamped to the first/last entry.
            # In particular, all mN <= 400 GeV points use the 400 GeV binning.
            m = int(mass)
            available = sorted(SR1_EDGES[channel])
            if m <= available[0]:
                chosen = available[0]
            elif m >= available[-1]:
                chosen = available[-1]
            else:
                chosen = max(value for value in available if value <= m)
        if chosen not in SR1_EDGES[channel]:
            raise BinningError(f"Table 39 has no SR1 binning for mass {mass}. "
                               "Specify --sr1-binning-mass using the actual template's choice, "
                               "or provide a full bin-config rule.")
        spec = new_spec(key, "m(#ell_{1}J) [GeV]", f"AN v7, Table 39; binning mass {chosen} GeV")
        add_intervals(spec, SR1_EDGES[channel][chosen], "B", open_last=True)
        spec["notes"].append("SR1 values above 5000 GeV are capped at 4999 (Sec. 7.4.1); final bin includes them.")
        return spec
    if key == "sr2":
        spec = new_spec(key, "H_{T}/p_{T}(#ell_{1})", "AN v7, Table 40")
        add_intervals(spec, SR2_EDGES[channel][0], "V1", "#Delta#phi(#ell,#ell) > 2", closure="open")
        add_intervals(spec, SR2_EDGES[channel][1], "V2", "#Delta#phi(#ell,#ell) #leq 2", closure="open")
        spec["notes"].append("The upper edge 10 is finite in Table 40; it is not silently relabelled as infinity.")
        return spec
    if low_mass:
        if mass is None:
            raise BinningError("SR3 low-mass BDT binning requires a mass hypothesis.")
        m = int(mass)
        chosen = 100 if 85 <= m <= 100 else m
        if chosen not in SR3_BDT_UPPER_EDGES[channel]:
            raise BinningError(f"No Table 44-46 BDT binning at mN={mass} GeV.")
        spec = new_spec(key, "BDT score", f"AN v7, Table {dict(MuMu=44,EE=45,EMu=46)[channel]}; binning mass {chosen} GeV")
        spec["title"] = "SR3L"
        add_intervals(spec, [None]+SR3_BDT_UPPER_EDGES[channel][chosen], "L", decimals=3)
        spec["notes"].append("Entries in Tables 44-46 are sequential upper thresholds, not 15 independent inclusive cuts.")
        return spec
    spec = new_spec(key, "L_{T} [GeV]", "AN v7, Table 41")
    spec["title"] = "SR3H"
    for idx, edges in enumerate(SR3H_THRESHOLDS[channel]):
        jets = "N(j) < 2" if idx < 2 else "N(j) #geq 2"
        met = "#leq" if idx % 2 == 0 else ">"
        group = f"#splitline{{{jets}}}{{(E_{{T}}^{{miss}})^{{2}}/S_{{T}} {met} 4 GeV}}"
        add_intervals(spec, [None]+edges+[None], f"H{idx+1}", group, closure="right")
    spec["notes"].append("LT = pT(l1)+pT(l2); the jet and MET^2/ST categories remain in the original order.")
    return spec


def merged_metadata(spec, mapping):
    """Describe bins ALREADY merged in the input. Never sum fitted bins/errors here."""
    flat = [x for group in mapping for x in group]
    if flat != list(range(1, len(spec["bins"])+1)):
        raise BinningError("table_bins must partition all original bins in order, with no omissions or duplicates.")
    old = spec["bins"]
    new = []
    for positions in mapping:
        if not positions:
            raise BinningError("Empty table_bins group.")
        selected = [old[i-1] for i in positions]
        b = copy.deepcopy(selected[0])
        if len(selected) > 1:
            if len({x["group"] for x in selected}) != 1 or not all("low" in x for x in selected):
                raise BinningError("Merging across categories needs explicit cut_labels and groups.")
            b["high"] = selected[-1]["high"]
            b["cut_label"] = interval_label(b["low"], b["high"], b["closure"], b["decimals"])
            b["code"] = "+".join(x["code"] for x in selected)
        b["table_bins"] = positions
        new.append(b)
    spec = copy.deepcopy(spec)
    spec["bins"] = new
    spec["groups"] = []
    for i, b in enumerate(new,1):
        if b["group"]:
            if spec["groups"] and spec["groups"][-1]["label"] == b["group"]:
                spec["groups"][-1]["last"] = i
            else:
                spec["groups"].append({"first":i,"last":i,"label":b["group"]})
    spec["source"] += "; explicitly supplied merge mapping for the input templates"
    return spec


def load_bin_rules(args):
    rules = []
    if args.bin_config:
        with open(args.bin_config) as handle:
            payload = json.load(handle)
        if not isinstance(payload, dict) or not isinstance(payload.get("overrides"), list):
            raise BinningError("--bin-config must contain an 'overrides' list.")
        rules = payload["overrides"]
    for text in args.nbins:
        try:
            name, n = text.rsplit("=",1)
            n = int(n)
            if not name or n < 1: raise ValueError()
        except ValueError:
            raise BinningError("--nbins expects REGION=positive_integer, got "+repr(text))
        rules.append({"region":name, "nbins":n})
    allowed = {"region","channels","masses","eras","region_key","channel","nbins","cut_labels",
               "codes","groups","axis_title","title","source","table_bins"}
    for rule in rules:
        if not isinstance(rule, dict) or "region" not in rule:
            raise BinningError("Every bin override needs a 'region' match pattern.")
        if set(rule)-allowed:
            raise BinningError("Unknown bin-config fields: "+str(sorted(set(rule)-allowed)))
    return rules


def rule_matches(rule, raw, key, channel, mass, era):
    pattern = rule["region"].lower()
    if not any(fnmatch.fnmatchcase(name.lower(), pattern) for name in (raw,key)):
        return False
    for field, value in (("channels",channel),("masses",mass),("eras",era)):
        if field in rule and str(value) not in {str(x) for x in rule[field]}:
            return False
    return True


def resolve_bin_spec(raw, channel, mass, point_signal, era, args, rules):
    key = canonical_region(raw)
    channel = resolve_channel(raw, channel)
    era = resolve_era(raw, era)
    config = {}
    for rule in rules:
        if rule_matches(rule,raw,key,channel,mass,era):
            config.update(rule)
    key = config.get("region_key",key)
    channel = config.get("channel",channel)
    low = sr3_low_mass(raw,mass,point_signal,args.sr3_mode)
    if config.get("nbins") == 1 and "cut_labels" not in config:
        if key.startswith("sr"):
            raise BinningError("An inclusive SR requires explicit cut_labels; --nbins=1 is reserved for CRs.")
        spec = inclusive_spec(key, config.get("source","Explicit inclusive CR input supplied by user"))
    elif "cut_labels" in config:
        if not config["cut_labels"]:
            raise BinningError("cut_labels must be a nonempty list.")
        spec = new_spec(key,config.get("axis_title",""),config.get("source","User-supplied bin definitions"))
        prefix = re.sub(r"[^A-Za-z0-9]","",key)
        spec["bins"] = [{"code":f"{prefix}{i}", "cut_label":x, "group":"", "table_bins":[]}
                         for i,x in enumerate(config["cut_labels"],1)]
    else:
        spec = builtin_spec(key,channel,mass,low,args.sr1_binning_mass)
    if args.sr2_merge78 and key == "sr2" and "table_bins" not in config and "cut_labels" not in config:
        spec = merged_metadata(spec,[[1],[2],[3],[4],[5],[6],[7,8]])
    if "table_bins" in config:
        if "cut_labels" in config:
            raise BinningError("Choose table_bins or cut_labels in a rule, not both.")
        spec = merged_metadata(spec,config["table_bins"])
    if "nbins" in config and config["nbins"] != len(spec["bins"]):
        raise BinningError(f"Requested {config['nbins']} bins but labels define {len(spec['bins'])}; "
                           "supply table_bins for existing merges, or explicit cut_labels.")
    if "codes" in config:
        if len(config["codes"]) != len(spec["bins"]):
            raise BinningError("codes and cut_labels have different lengths.")
        for b,code in zip(spec["bins"],config["codes"]): b["code"] = code
    for field in ("axis_title","title","source","groups"):
        if field in config: spec[field] = config[field]
    previous = 0
    for g in spec["groups"]:
        if not (previous < g["first"] <= g["last"] <= len(spec["bins"])):
            raise BinningError("groups must be ordered non-overlapping inclusive 1-based ranges.")
        previous = g["last"]
    if len({b["code"] for b in spec["bins"]}) != len(spec["bins"]):
        raise BinningError("Bin codes must be distinct within each plot.")
    spec.update({"raw_region":raw, "channel":channel, "mass":mass, "era":era})
    return spec


def plain_text(text):
    # Human-readable TXT; retain uncommon TLatex commands verbatim.
    text = re.sub(r"#splitline\{([^{}]*)\}\{(.*)\}",r"\1; \2",str(text))
    for a,b in (("#geq",">="),("#leq","<="),("#minus"," to "),("#Delta#phi","DeltaPhi"),
                ("#ell","l"),("#gamma","gamma"),("#mu","mu"),("#pm","+/-")):
        text = text.replace(a,b)
    return text


def bin_key_text(spec):
    lines = [f"Region: {spec['raw_region']}  |  {spec['channel']}  |  mass: {spec['mass']}  |  era: {spec['era']}",
             "Source: "+spec["source"], "Axis variable: "+plain_text(spec["axis_title"]),
             "Bin IDs are plotting labels proposed here, not IDs already adopted in the AN.",
             "Display slots have equal width. Input yields and fitted per-bin errors are copied unchanged.",
             "No event-yield trimming heuristic and no post-fit rebinning are used."]
    lines.extend(spec["notes"])
    rows = []
    for i,b in enumerate(spec["bins"],1):
        group = b["group"]
        if not group:
            group = next((g["label"] for g in spec["groups"] if g["first"] <= i <= g["last"]), "")
        if "low" in b:
            lo, hi = b["low"], b["high"]
            left = "<" if b["closure"] in ("right","open") else "<="
            right = "<=" if b["closure"] == "right" else "<"
            condition = ((f"{lo:g} {left} " if lo is not None else "")+"x"+
                         (f" {right} {hi:g}" if hi is not None else ""))
        else:
            condition = b.get("definition",plain_text(b["cut_label"]))
        rows.append([str(i),plain_text(b["code"]),",".join(map(str,b["table_bins"])) or "custom",
                     condition,plain_text(group)])
    header = ["Input bin","ID","AN table bins","Cut on x","Category"]
    widths = [max(len(row[j]) for row in [header]+rows) for j in range(5)]
    lines += ["", "  ".join(s.ljust(w) for s,w in zip(header,widths)),
              "  ".join("-"*w for w in widths)]
    lines += ["  ".join(s.ljust(w) for s,w in zip(row,widths)).rstrip() for row in rows]
    return "\n".join(lines)+"\n"


def nonzero(value):
    # Exact zero, not a yield threshold: never drop a small fitted tail.
    return not math.isfinite(float(value)) or float(value) != 0.0


def validate_hist_padding(hist, n, what):
    if hist.GetNbinsX() < n:
        raise BinningError(f"{what}: histogram has {hist.GetNbinsX()} bins, labels require {n}.")
    for i in [0]+list(range(n+1,hist.GetNbinsX()+2)):
        if nonzero(hist.GetBinContent(i)) or nonzero(hist.GetBinError(i)):
            raise BinningError(f"{what}: nonempty discarded/flow bin {i}; check the selected bin map. "
                               "Use --nbins or --bin-config to describe the actual fit input.")


def validate_region_padding(directory, n):
    if directory is None:
        return
    # Include processes not in STACK_ORDER and prefitted signals. A background
    # sum can be zero while individual processes or signal still carry content.
    for key in directory.GetListOfKeys():
        obj = key.ReadObj()
        if obj.InheritsFrom("TH1") and obj.GetDimension() == 1:
            validate_hist_padding(obj,n,directory.GetName()+"/"+obj.GetName())


def categorical_hist(source, n, name):
    validate_hist_padding(source,n,name)
    target = ROOT.TH1D(name,"",n,0.,float(n))
    target.SetDirectory(0)
    for i in range(1,n+1):
        target.SetBinContent(i,source.GetBinContent(i))
        target.SetBinError(i,source.GetBinError(i))
    for prop in ("LineColor","LineStyle","LineWidth","FillColor","FillStyle","MarkerColor","MarkerStyle","MarkerSize"):
        getattr(target,"Set"+prop)(getattr(source,"Get"+prop)())
    return target


def categorical_data(source, reference, n, name):
    """Map graph points by their ORIGINAL x-bin, then move to bin centres.

    Padded graph points with y=0 may have a Poisson upper error; the metadata
    defines which bins exist. Nonzero out-of-range data always causes an error.
    """
    if source is None:
        return None
    target = ROOT.TGraphAsymmErrors()
    target.SetName(name)
    seen = set()
    for i in range(source.GetN()):
        old_x, y = source.GetPointX(i), source.GetPointY(i)
        b = reference.GetXaxis().FindFixBin(old_x)
        if b < 1 or b > n:
            if nonzero(y):
                raise BinningError(f"{name}: nonzero data in omitted bin (x={old_x}, y={y}).")
            continue
        if b in seen:
            raise BinningError(f"{name}: multiple graph points map to input bin {b}.")
        seen.add(b)
        j = target.GetN()
        target.SetPoint(j,b-.5,y)
        target.SetPointError(j,0.,0.,source.GetErrorYlow(i),source.GetErrorYhigh(i))
    return target
# ------------------------------------------------------------
# Plotting: original signal and uncertainty helpers above are reused.
# ------------------------------------------------------------

def pdf_safe_tlatex(text):
    """Return ROOT-TLatex text that is safe for both PNG and PDF output.

    ROOT TLatex does not implement the LaTeX \\ell glyph.  Using \\ell invokes
    TMathText, whose PDF backend is incomplete, while '#ell' is not a TLatex
    command and can be printed literally.  Use a PDF-safe italic l glyph
    instead.  Greek symbols are normalized to native TLatex commands so that
    the same string is used by the raster and vector backends.
    """
    text = str(text)
    text = text.replace(r"\ell", "#font[12]{l}")
    text = text.replace("#ell", "#font[12]{l}")
    text = text.replace("Δ", "#Delta").replace("∆", "#Delta")
    text = text.replace("φ", "#phi")
    # ROOT's PDF text bounding box can shave the left bearing of a leading
    # Greek glyph.  An explicit tiny text group gives #Delta a little vector
    # bounding-box padding without changing the visible label appreciably.
    if text.startswith("#Delta"):
        text = "#font[42]{ }" + text
    return text


def draw_text(x, y, text, size=22, align=11, font=43, angle=0):
    obj = ROOT.TLatex(x,y,pdf_safe_tlatex(text))
    obj.SetNDC(True)
    obj.SetTextFont(font)
    obj.SetTextSize(size)
    obj.SetTextAlign(align)
    obj.SetTextAngle(angle)
    obj.Draw()
    return obj


def render_region(total_bkg, processes, data, ratio_input, curves, spec,
                  fit_type, output_dir, style, logy, args):
    n = len(spec["bins"])
    safe = re.sub(r"[^A-Za-z0-9_]", "_", spec["raw_region"])
    uid = f"{safe}_{fit_type}_{style}"
    width = args.canvas_width or (900 if n <= 8 else 1200 if n <= 15 else 1400)
    height = 900
    canvas = ROOT.TCanvas("c_"+uid,"",width,height)
    upper = ROOT.TPad("upper_"+uid,"",0.,.34,1.,1.)
    ratio = ROOT.TPad("ratio_"+uid,"",0.,0.,1.,.34)
    left, right = .12, .97
    grouped = bool(spec["groups"])

    # --------------------------------------------------------
    # CMS-style upper-pad layout
    # --------------------------------------------------------
    # CMS/WIP and luminosity remain in the top margin.  Everything else is
    # inside the upper frame.  Background/data/uncertainty entries are kept
    # together in one vertical legend on the right, like the reference
    # trilepton plot, rather than spread horizontally across the frame.
    top_margin = .090
    bottom_margin = .025
    frame_top_ndc = 1.0 - top_margin
    frame_bottom_ndc = bottom_margin

    # Move in-frame text a little away from the left/right ticks.
    #title_x = left + .015
    title_x = left + .035
    title_y = .865
    fit_y = .825

    # One-column component legend, inset from the right-side ticks.
    component_x1 = .715 if width <= 900 else .735
    component_x2 = right - .025
    component_top = .865
    component_items = len(processes) + 1 + (1 if data is not None else 0)
    component_row = .039
    component_bottom = component_top - component_row * max(1, component_items)

    # Signal entries live on the left/middle, leaving the component legend
    # visually grouped on the right.  Keep them vertical as well when present.
    signal_x1 = title_x
    signal_x2 = component_x1 - .025
    signal_top = .785
    signal_row = .041
    signal_bottom = signal_top - signal_row * len(curves)

    # Category labels (e.g. the two SR2 Delta-phi categories) must sit below
    # whichever legend extends farther down.  For plots without categories,
    # the same floor is used only to reserve y-axis headroom.
    annotation_bottom = min(component_bottom, signal_bottom if curves else fit_y)
    category_y = annotation_bottom - (.028 if grouped else 0.0)
    annotation_floor = category_y - (.060 if grouped else .025)

    # Keep the highest data/background point below the annotation block by
    # extending the y range if needed.  This is preferable to moving legends
    # outside the plot frame.
    usable_fraction = (annotation_floor - frame_bottom_ndc) / (frame_top_ndc - frame_bottom_ndc)
    usable_fraction = max(.34, min(.78, usable_fraction))
    boundary_fraction = min(.90, usable_fraction + .03,)

    if args.label_angle is not None:
        angle = args.label_angle
    elif style == "cuts" and n > 10:
        #angle = 65
        angle = 40
    elif style == "cuts" and spec.get("axis_title") == "BDT score" and n >= 8:
        # The 8-bin CR3L BDT labels otherwise collide at the default 900 px width.
        #angle = 55
        angle = 30
    else:
        angle = 0

    for pad in (upper,ratio):
        pad.SetLeftMargin(left)
        pad.SetRightMargin(1-right)
        pad.SetFillColor(0)
        pad.SetBorderMode(0)
        pad.SetTicks(1,1)
    upper.SetTopMargin(top_margin)
    upper.SetBottomMargin(bottom_margin)
    ratio.SetTopMargin(.045)
    #ratio.SetBottomMargin(.62 if angle else .34)
    ratio.SetBottomMargin(.48 if angle else .34)
    upper.Draw()
    ratio.Draw()
    keep = [upper,ratio]

    upper.cd()
    if logy:
        upper.SetLogy()

    stack = ROOT.THStack("stack_"+uid,"")
    for _,hist in processes:
        stack.Add(hist)

    ymax_candidates = [total_bkg.GetBinContent(i)+total_bkg.GetBinError(i) for i in range(1,n+1)]
    ymax_candidates += [c["hist"].GetMaximum() for c in curves]
    if data is not None:
        ymax_candidates += [data.GetPointY(i)+data.GetErrorYhigh(i) for i in range(data.GetN())]
    data_top = max([1.] + ymax_candidates)

    positive = [total_bkg.GetBinContent(i) for i in range(1,n+1) if total_bkg.GetBinContent(i)>0]
    ymin = max(1.e-6,min([.1]+[v*.2 for v in positive])) if logy else 0.

    if logy:
        safe_data_top = max(data_top, ymin * 1.01)
        ymax = ymin * (safe_data_top / ymin) ** (1.0 / usable_fraction)
        ymax = max(ymax, safe_data_top * 3.0)
        boundary_top = ymin * (ymax / ymin) ** boundary_fraction
    else:
        ymax = data_top / usable_fraction
        boundary_top = ymin + (ymax - ymin) * boundary_fraction

    frame = ROOT.TH1D("frame_"+uid,"",n,0.,float(n))
    frame.SetDirectory(0)
    frame.SetMinimum(ymin)
    frame.SetMaximum(ymax)
    frame.GetXaxis().SetLabelSize(0)
    frame.GetXaxis().SetTickLength(0)
    frame.GetYaxis().SetTitle("Events / bin")
    frame.GetYaxis().SetTitleFont(43)
    frame.GetYaxis().SetTitleSize(24)
    frame.GetYaxis().SetLabelFont(43)
    frame.GetYaxis().SetLabelSize(21)
    #frame.GetYaxis().SetTitleOffset(1.45)
    frame.GetYaxis().SetTitleOffset(1.85)
    frame.Draw("AXIS")

    if processes:
        # NOCLEAR avoids the ROOT 6.30 THStack SAME-paint crash seen when an
        # external categorical frame has already been drawn.
        stack.Draw("HIST NOCLEAR SAME")

    band = make_unc_band(total_bkg)
    band.Draw("E2 SAME")

    for curve in curves:
        curve["hist"].Draw("HIST SAME")

    if data is not None:
        data.SetMarkerStyle(20)
        data.SetMarkerSize(.9)
        data.SetMarkerColor(ROOT.kBlack)
        data.SetLineColor(ROOT.kBlack)
        data.Draw("PZ SAME")
    keep += [frame,stack,band]

    # Region and fit labels are inside the plotting frame.
    channel_label = {"EE":"ee","EMu":"e#mu","MuMu":"#mu#mu","3ch":"3 channels"}.get(spec["channel"],spec["channel"])
    title = spec["title"]+", "+channel_label
    if spec["mass"] is not None:
        title += f", m_{{N}} = {spec['mass']} GeV"
    keep.append(draw_text(title_x,title_y,title,22,font=63))

    fit_label = {"shapes_prefit":"Pre-fit", "shapes_fit_b":"Post-fit (B-only)",
                 "shapes_fit_s":"Post-fit (S+B)"}[fit_type]
    keep.append(draw_text(title_x,fit_y,fit_label,19,align=11))

    # Data/background/uncertainty: one vertical legend on the right.
    legend = ROOT.TLegend(component_x1,component_bottom,component_x2,component_top)
    legend.SetNColumns(1)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextFont(43)
    legend.SetTextSize(18)
    legend.SetMargin(.24)
    if data is not None:
        legend.AddEntry(data,"Data","pe")
    for proc,hist in processes:
        legend.AddEntry(hist,PROCESS_LABELS[proc],"f")
    legend.AddEntry(band,"Bkg. total unc.","f")
    legend.Draw()
    keep.append(legend)

    if curves:
        signal_legend = ROOT.TLegend(signal_x1,signal_bottom,signal_x2,signal_top)
        signal_legend.SetNColumns(1)
        signal_legend.SetBorderSize(0)
        signal_legend.SetFillStyle(0)
        signal_legend.SetTextFont(43)
        signal_legend.SetTextSize(18)
        signal_legend.SetMargin(.18)
        for curve in curves:
            signal_legend.AddEntry(curve["hist"],curve["label"],"l")
        signal_legend.Draw()
        keep.append(signal_legend)

    # Standard CMS top-margin labels, moved slightly closer to the frame than
    # in v2 while remaining outside the plotting area.
    #cms_y = .944
    cms_y = .914
    keep.append(draw_text(left+.005,cms_y,"CMS",29,font=63))
    if args.cms_label:
        keep.append(draw_text(left+.005+74./width,cms_y+.002,args.cms_label,20,font=53))
    lumi = args.lumi if args.lumi is not None else LUMI[spec["era"]]
    keep.append(draw_text(right-.005,cms_y,f"{lumi:g} fb^{{-1}} (13 TeV)",21,align=31))

    boundaries = sorted({g["first"]-1 for g in spec["groups"] if g["first"] > 1})
    for x in boundaries:
        line = ROOT.TLine(x,ymin,x,boundary_top)
        line.SetLineStyle(2)
        line.SetLineColor(ROOT.kGray+2)
        line.Draw()
        keep.append(line)
    for g in spec["groups"]:
        x = left+(right-left)*((g["first"]-1+g["last"])/2)/n
        keep.append(draw_text(x,category_y,g["label"],20,align=23))
    upper.RedrawAxis()

    ratio.cd()
    ratio_frame = ROOT.TH1D("ratio_frame_"+uid,"",n,0.,float(n))
    ratio_frame.SetDirectory(0)
    ratio_frame.SetMinimum(0.)
    ratio_frame.SetMaximum(args.ratio_max)
    ratio_frame.GetXaxis().SetLabelSize(0)
    ratio_frame.GetXaxis().SetTickLength(0)
    ratio_frame.GetYaxis().SetTitle("Data / Pred." if data is not None else "Asimov / Pred.")
    ratio_frame.GetYaxis().SetNdivisions(505)
    ratio_frame.GetYaxis().SetTitleFont(43)
    ratio_frame.GetYaxis().SetTitleSize(22)
    ratio_frame.GetYaxis().SetLabelFont(43)
    ratio_frame.GetYaxis().SetLabelSize(20)
    ratio_frame.GetYaxis().SetTitleOffset(1.45)
    ratio_frame.Draw("AXIS")
    ratio_band = make_ratio_band(total_bkg)
    ratio_band.Draw("E2 SAME")
    line = ROOT.TLine(0.,1.,float(n),1.)
    line.SetLineStyle(2)
    line.Draw()
    ratio_graph = make_ratio_graph(ratio_input,total_bkg)
    ratio_graph.SetMarkerStyle(20)
    ratio_graph.SetMarkerSize(.85)
    ratio_graph.SetMarkerColor(ROOT.kBlack)
    ratio_graph.SetLineColor(ROOT.kBlack)
    ratio_graph.Draw("PZ SAME")
    keep += [ratio_frame,ratio_band,ratio_graph,line]
    for x in boundaries:
        line = ROOT.TLine(x,0.,x,args.ratio_max)
        line.SetLineStyle(2)
        line.SetLineColor(ROOT.kGray+2)
        line.Draw()
        keep.append(line)

    # Manual TLatex labels: supports inequalities and preserves one label per bin.
    label_y = ratio.GetBottomMargin()-.035
    for i,b in enumerate(spec["bins"]):
        x = left+(right-left)*(i+.5)/n
        text = b["cut_label"] if style == "cuts" else b["code"]
        keep.append(draw_text(x,label_y,text,
                              18 if angle else 20,align=33 if angle else 23,angle=angle))

    # Keep the x-axis title much closer to the labels, comparable to the old
    # ROOT-axis layout.  A one-bin inclusive region gets the explicit title
    # "Region" instead of leaving a large blank lower margin.
    if n == 1:
        axis_title = "Region"
    elif style == "cuts":
        axis_title = spec["axis_title"]
    else:
        axis_title = "Bin ID"
    if axis_title:
        #axis_title_y = .080 if angle else .155
        axis_title_y = .1 if angle else .155
        keep.append(draw_text(right-.005,axis_title_y,axis_title,23,align=31))
    ratio.RedrawAxis()

    canvas.cd()
    canvas.Modified()
    canvas.Update()
    os.makedirs(output_dir,exist_ok=True)
    for ext in ("pdf","png"):
        canvas.SaveAs(os.path.join(output_dir,spec["raw_region"]+"."+ext))
    with open(os.path.join(output_dir,spec["raw_region"]+"_bins.txt"),"w") as handle:
        handle.write(bin_key_text(spec))
    canvas.Close()

def plot_region(region_dir, signal_region_dir, fit_type, outdir, logy,
                signal_mode, signal_scales, signal_colors, draw_data, mass,
                point_signal, channel, era, args, rules):
    region = region_dir.GetName()
    source = region_dir.Get("total_background")
    if not source:
        raise BinningError(region+": total_background not found")
    spec = resolve_bin_spec(region,channel,mass,point_signal,era,args,rules)
    n = len(spec["bins"])
    validate_region_padding(region_dir,n)
    validate_region_padding(signal_region_dir,n)
    total_bkg = categorical_hist(source,n,f"bkg_{region}_{fit_type}")
    raw_data = region_dir.Get("data") if draw_data else None
    data = categorical_data(raw_data,source,n,f"data_{region}_{fit_type}") if raw_data else None
    if draw_data and data is None:
        print(f"[WARNING] {region}: data missing; drawing expected B/B in the ratio pad.")
    ratio_input = data if data is not None else make_background_pseudodata_graph(total_bkg)
    processes = []
    for proc in STACK_ORDER:
        h = get_combined_process(region_dir,proc)
        if not h: continue
        h = categorical_hist(h,n,f"{proc}_{region}_{fit_type}")
        h.SetFillColor(root_color(PROCESS_COLORS[proc]))
        h.SetLineColor(ROOT.kBlack)
        h.SetLineWidth(1)
        processes.append((proc,h))
    # Report a discrepancy; never replace Combine's fitted total error with
    # quadrature of process errors, which would discard correlations.
    max_diff = max(abs(sum(h.GetBinContent(i) for _,h in processes)-total_bkg.GetBinContent(i))
                   for i in range(1,n+1))
    if max_diff > 1e-5*max(1.,max(total_bkg.GetBinContent(i) for i in range(1,n+1))):
        raise BinningError(f"{region}: drawn processes do not sum to total_background "
                           f"(largest bin difference {max_diff:g}); check process names / selected era.")

    effective_signal_scales = (
        resolve_effective_signal_scales(
            signal_region_dir=signal_region_dir,
            fit_type=fit_type,
            signal_mode=signal_mode,
            mass=mass,
            point_signal=point_signal,
            total_bkg=total_bkg,
            n=n,
            logy=logy,
            scale_specs=signal_scales,
        )
    )

    curves = load_signal_curves(
        region_dir,
        signal_region_dir,
        fit_type,
        signal_mode,
        effective_signal_scales,
        signal_colors,
        mass,
        point_signal,
    )

    for i,curve in enumerate(curves):
        curve["hist"] = categorical_hist(curve["hist"],n,f"sig_{i}_{region}_{fit_type}")
    print(f"[BINS] {region}: {source.GetNbinsX()} stored -> {n} displayed; {spec['source']}")
    print(f"[YIELD] total_background = {total_bkg.Integral():.6g}; style = {args.axis_style}")
    styles = ("cuts","codes") if args.axis_style == "both" else (args.axis_style,)
    for style in styles:
        render_region(total_bkg,processes,data,ratio_input,curves,spec,fit_type,
                      os.path.join(outdir,"axis_"+style),style,logy,args)


def selected_region(raw, patterns):
    key = canonical_region(raw)
    return not patterns or any(fnmatch.fnmatchcase(v.lower(),p.lower())
                               for v in (raw,key) for p in patterns)


def run_one_file(input_file, output_point_dir, era, logy, signal_mode, signal_scales,
                 signal_colors, draw_data, mass, point_signal, channel, args, rules):
    global PROCESS_ERAS
    PROCESS_ERAS = get_process_eras(era)
    if not os.path.isfile(input_file):
        print("[ERROR] Input file does not exist:",input_file)
        return 0,1
    f = ROOT.TFile.Open(input_file)
    if not f or f.IsZombie():
        print("[ERROR] Failed to open:",input_file)
        return 0,1
    made, failed = 0,0
    try:
        prefit = f.Get("shapes_prefit")
        for fit_type in args.fit_types:
            fit_dir = f.Get(fit_type)
            if not fit_dir:
                print("[WARNING] Missing fit directory:",fit_type)
                continue
            fit_outdir = os.path.join(output_point_dir,FIT_OUTPUT_DIR[fit_type])
            variant = get_plot_variant_subdir(logy,draw_data)
            if variant: fit_outdir = os.path.join(fit_outdir,variant)
            for key in fit_dir.GetListOfKeys():
                obj = key.ReadObj()
                if not obj.InheritsFrom("TDirectory") or not selected_region(obj.GetName(),args.regions):
                    continue
                signal_dir = obj
                if fit_type == "shapes_fit_b":
                    signal_dir = prefit.Get(obj.GetName()) if prefit else None
                    if not signal_dir: signal_dir = None
                try:
                    plot_region(obj,signal_dir,fit_type,fit_outdir,logy,signal_mode,signal_scales,
                                signal_colors,draw_data,mass,point_signal,channel,era,args,rules)
                    made += 1
                except BinningError as exc:
                    failed += 1
                    print(f"[BINNING ERROR] {fit_type}/{obj.GetName()}: {exc}")
    finally:
        f.Close()
    if not made and not failed:
        print("[ERROR] No matching regions with saved shapes in",input_file)
        failed += 1
    return made,failed


def main():
    global BASE_DIR
    args = parse_args()
    BASE_DIR = args.base_dir
    rules = load_bin_rules(args)

    if args.describe_bins:
        if not args.regions or any(any(c in r for c in "*?[") for r in args.regions):
            raise BinningError("--describe-bins requires explicit --regions names (no globs).")
        for era in args.eras:
            for channel in args.channels:
                for signal in args.signals:
                    for mass in ([None] if signal == "Weinberg" else args.masses):
                        for region in args.regions:
                            print(bin_key_text(resolve_bin_spec(region,channel,mass,signal,era,args,rules)))
        return 0
    if ROOT is None:
        print("PyROOT is required for plotting. Run in your CMSSW environment after cmsenv.",file=sys.stderr)
        return 2
    colors = {"total":args.signal_color_total, "DY":args.signal_color_dy,
              "VBF":args.signal_color_vbf, "SSWW":args.signal_color_ssww,
              "Weinberg":args.signal_color_weinberg}
    signal_subdir = get_signal_mode_subdir(args)
    jobs = []
    if args.input_file:
        mass = args.masses[0] if args.masses else None
        name = os.path.basename(args.input_file).removesuffix(".root")
        jobs.append((args.input_file,os.path.join(args.outdir,name,signal_subdir),
                     args.eras[0],args.channels[0],args.signals[0],mass))
    else:
        for wp in args.InputWPs:
            for era in args.eras:
                for channel in args.channels:
                    for signal in args.signals:
                        for tag in args.tags:
                            for suffix in TAG_MAP[tag]:
                                for mass in ([None] if signal == "Weinberg" else args.masses):
                                    name = build_point_name(era,channel,mass,signal,suffix)
                                    input_file = build_input_file(wp,name,args.fit_subdir)
                                    # Absolute WPs must not discard the chosen output directory.
                                    wp_out = wp if not os.path.isabs(wp) else os.path.basename(wp.rstrip(os.sep))
                                    out = os.path.join(args.outdir,wp_out,name,signal_subdir)
                                    jobs.append((input_file,out,era,channel,signal,mass))
    made,failed = 0,0
    for input_file,out,era,channel,signal,mass in jobs:
        scales = resolve_signal_scales(args,channel,mass)
        print("[INPUT]",input_file)
        m,f = run_one_file(input_file,out,era,args.logy,args.signal_mode,scales,colors,
                           not args.no_data,mass,signal,channel,args,rules)
        made += m
        failed += f
    print(f"[DONE] {made} region/fit plots completed; {failed} failed. Output: {args.outdir}")
    return 2 if failed or not made else 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (BinningError, json.JSONDecodeError) as exc:
        print("[ERROR]",exc,file=sys.stderr)
        sys.exit(2)
