#!/usr/bin/env python3

import os, argparse, math, re
import ROOT

ROOT.gROOT.SetBatch(True)

try:
    import tdrstyle
    tdrstyle.setTDRStyle()
except:
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
    "cf": ROOT.TColor.GetColor("#92dadd"),
    "fake": ROOT.TColor.GetColor("#3f90da"),
    "wz": ROOT.TColor.GetColor("#ffa90e"),
    # WZ_EWK is merged into WZ in plots; keep same color for safety
    "zz": ROOT.TColor.GetColor("#bd1f01"),
    "ww": ROOT.TColor.GetColor("#94a4a2"),
    "zg": ROOT.TColor.GetColor("#832db6"),
    "mc_others": ROOT.TColor.GetColor("#b9ac70"),
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
    "presetname1": {
        "default": {
            "DY": 1.0,
            "VBF": 1.0,
            "SSWW": 1.0,
            "Weinberg": 1.0,
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
        required=True,
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
        default="total",
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
        type=float,
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
        type=float,
        default=1.0,
        help=(
            "DY draw scale for --signal-mode separate. "
            "Ignored when a --signal-preset is used."
        ),
    )

    parser.add_argument(
        "--signal-scale-vbf",
        dest="signal_scale_vbf",
        type=float,
        default=1.0,
        help=(
            "Wgamma/VBF draw scale for --signal-mode separate. "
            "Ignored when a --signal-preset is used."
        ),
    )

    parser.add_argument(
        "--signal-scale-ssww",
        dest="signal_scale_ssww",
        type=float,
        default=1.0,
        help=(
            "SSWW draw scale for --signal-mode separate. "
            "Ignored when a --signal-preset is used."
        ),
    )

    parser.add_argument(
        "--signal-scale-weinberg",
        dest="signal_scale_weinberg",
        type=float,
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

    args = parser.parse_args()

    if not args.eras:
        args.eras = ["Run2Sum"]

    mass_dependent_signals = [s for s in args.signals if s != "Weinberg"]
    if mass_dependent_signals and not args.masses:
        parser.error("-m/--masses is required for HNL/DY/VBF/DYVBF/SSWW signals.")

    signal_scale_options = {
        "--signal-scale-total": args.signal_scale_total,
        "--signal-scale-dy": args.signal_scale_dy,
        "--signal-scale-vbf": args.signal_scale_vbf,
        "--signal-scale-ssww": args.signal_scale_ssww,
        "--signal-scale-weinberg": args.signal_scale_weinberg,
    }

    for option_name, scale in signal_scale_options.items():
        if scale <= 0:
            parser.error(f"{option_name} must be greater than 0.")

    if args.signal_preset is not None and args.signal_mode != "separate":
        parser.error(
            "--signal-preset can only be used with --signal-mode separate."
        )

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


def resolve_separate_signal_scales(preset_name, channel, mass):
    """
    Resolve a separate-mode preset for one channel/mass point.

    Overrides are applied from top to bottom. Missing "channels" or
    "masses" means that the rule matches all values of that dimension.
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
            allowed_masses = {str(item) for item in masses}
            if mass_text not in allowed_masses:
                continue

        scales.update(override["scales"])

    required_keys = ["DY", "VBF", "SSWW", "Weinberg"]

    for key in required_keys:
        if key not in scales:
            raise ValueError(
                f"Preset {preset_name!r} does not define a scale for {key}."
            )

        if scales[key] <= 0:
            raise ValueError(
                f"Preset {preset_name!r} has non-positive {key} scale: "
                f"{scales[key]}"
            )

    return scales


def resolve_signal_scales(args, channel, mass):
    """
    Return the signal draw scales for one channel/mass point.

    For prefit and postfit B-only HNL total/both,
    args.signal_scale_total is interpreted as a multiplier S of |V|^2.
    Therefore DY/VBF use S and SSWW uses S^2.

    load_signal_curves() deliberately treats postfit S+B differently:
    the already-fitted HNL curves are all given the same display factor S
    so that the fitted composition is not changed.
    """
    if args.signal_mode == "separate":
        if args.signal_preset is not None:
            component_scales = resolve_separate_signal_scales(
                preset_name=args.signal_preset,
                channel=channel,
                mass=mass,
            )
        else:
            component_scales = {
                "DY": args.signal_scale_dy,
                "VBF": args.signal_scale_vbf,
                "SSWW": args.signal_scale_ssww,
                "Weinberg": args.signal_scale_weinberg,
            }

        return {
            "total": args.signal_scale_total,
            **component_scales,
        }

    hnl_mixing_multiplier = args.signal_scale_total

    return {
        "total": hnl_mixing_multiplier,
        "DY": hnl_mixing_multiplier,
        "VBF": hnl_mixing_multiplier,
        "SSWW": hnl_mixing_multiplier * hnl_mixing_multiplier,
        "Weinberg": args.signal_scale_weinberg,
    }


def get_signal_mode_subdir(signal_mode, signal_preset):
    if signal_mode == "separate" and signal_preset is not None:
        return f"signal_separate_{signal_preset}"

    return f"signal_{signal_mode}"


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


def build_input_file(wp, point_name):
    return os.path.join(
        BASE_DIR,
        wp,
        point_name,
        "FitDiag",
        "Unblind",
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

# ------------------------------------------------------------
# Plot one region
# ------------------------------------------------------------

def plot_region(
    region_dir,
    signal_region_dir,
    fit_type,
    outdir,
    logy,
    signal_mode,
    signal_scales,
    signal_colors,
    draw_data,
    mass,
    point_signal,
):

    region = region_dir.GetName()
    if fit_type == "shapes_prefit":
        fit_text = "Pre-fit"
        
    elif fit_type == "shapes_fit_b":
        fit_text = "Post-fit (B-only)"
        
    elif fit_type == "shapes_fit_s":
        fit_text = "Post-fit (S+B)"
        
    else:
        fit_text = fit_type
    print("")
    print("=" * 70)
    print("Region:", region)
    
    total_bkg_source = region_dir.Get(
        "total_background"
    )

    if not total_bkg_source:
        print(
            "Skipping",
            region,
            ": total_background not found",
        )
        return

    data = None

    if draw_data:

        data = region_dir.Get("data")

        if not data:

            print(
                "[WARNING] data not found in",
                f"{fit_type}/{region}",
                "- using B/B pseudo-data in the ratio pad",
            )

            draw_data = False

    nbins = total_bkg_source.GetNbinsX()

    total_bkg = total_bkg_source.Clone(
        f"total_bkg_{fit_type}_{region}"
    )
    total_bkg.SetDirectory(0)

    # --------------------------------------------------------
    # Ratio numerator
    # --------------------------------------------------------

    if draw_data:

        ratio_input = data

    else:

        # Standard FitDiagnostics output does not provide a separate
        # total_background_stat histogram.
        #
        # Leave this as None to use sqrt(B), i.e. expected
        # observation counting-stat errors.
        stat_only_bkg = None

        # If a separate stat-only histogram is prepared later:
        #
        # stat_only_bkg = region_dir.Get(
        #     "total_background_stat"
        # )

        ratio_input = make_background_pseudodata_graph(
            bkg_hist=total_bkg,
            stat_error_hist=stat_only_bkg,
        )

    stack = ROOT.THStack()

    legend = ROOT.TLegend(0.47, 0.43, 0.89, 0.88)

    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.028)
    legend.SetNColumns(2)

    total_yield = 0.

    for proc in STACK_ORDER:

        h = get_combined_process(region_dir, proc)

        if not h:
            continue

        yld = h.Integral()
        total_yield += yld

        print(f"{proc:12s} {yld:10.3f}")

        h.SetFillColor(PROCESS_COLORS[proc])
        h.SetLineColor(ROOT.kBlack)

        stack.Add(h)
        legend.AddEntry(h, proc, "f")

    print("-" * 30)
    print(f"{'stack sum':12s} {total_yield:10.3f}")
    print(
        f"{'total_bkg':12s}"
        f" {total_bkg.Integral('width'):10.3f}"
    )

    signal_curves = load_signal_curves(
        region_dir=region_dir,
        signal_region_dir=signal_region_dir,
        fit_type=fit_type,
        signal_mode=signal_mode,
        signal_scales=signal_scales,
        signal_colors=signal_colors,
        mass=mass,
        point_signal=point_signal,
    )

    for curve in signal_curves:
        legend.AddEntry(
            curve["hist"],
            curve["label"],
            "l",
        )

    if draw_data:
        legend.AddEntry(data, "Data", "pe")

    # --------------------------------------------------------
    # Canvas
    # --------------------------------------------------------

    canvas = ROOT.TCanvas(f"c_{region}", "", 800, 800)

    pad1 = ROOT.TPad("pad1", "", 0, 0.30, 1, 1)
    pad2 = ROOT.TPad("pad2", "", 0, 0, 1, 0.30)

    pad1.SetBottomMargin(0.02)
    pad2.SetTopMargin(0.05)
    pad2.SetBottomMargin(0.35)

    pad1.Draw()
    pad2.Draw()

    # --------------------------------------------------------
    # Upper pad
    # --------------------------------------------------------

    pad1.cd()
    if logy: pad1.SetLogy()
    
    stack.Draw("hist")
    stack.GetXaxis().SetLimits(0, nbins)
    
    stack.GetXaxis().SetLabelSize(0)
    
    stack.GetXaxis().SetTitleSize(0)
    
    ymax_candidates = [
        stack.GetMaximum(),
        total_bkg.GetMaximum(),
    ]

    for curve in signal_curves:
        ymax_candidates.append(
            curve["hist"].GetMaximum()
        )

    ymax = max(ymax_candidates)

    stack.SetMinimum(0.1)
    
    if logy: stack.SetMaximum(ymax * 500.)
    else: stack.SetMaximum(ymax * 2.)

    stack.GetYaxis().SetTitle("Events")

    stack.GetYaxis().SetTitleSize(0.05)
    
    stack.GetYaxis().SetLabelSize(0.04)

    stack.GetYaxis().SetTitleOffset(0.9)

    unc_band = make_unc_band(total_bkg)
    unc_band.Draw("E2 SAME")

    legend.AddEntry(
        unc_band,
        "Bkg. total unc.",
        "f",
    )

    for curve in signal_curves:
        curve["hist"].Draw("HIST SAME")

    if draw_data:

        data.SetMarkerStyle(20)
        data.SetMarkerSize(1.0)
        data.SetLineColor(ROOT.kBlack)

        data.Draw("PZ SAME")

    legend.Draw()

    latex = ROOT.TLatex()
    latex.SetNDC()
    latex.SetTextSize(0.045)

    latex.DrawLatex(0.15, 0.92, "CMS Work in Progress")
    latex.DrawLatex(0.67, 0.92, "138 fb^{-1} (13 TeV)")
    latex.SetTextFont(62)
    
    latex.DrawLatex(0.15, 0.84, region)
    
    latex.SetTextFont(42)
    
    latex.DrawLatex(0.15, 0.78, fit_text)

    # --------------------------------------------------------
    # Ratio pad
    # --------------------------------------------------------

    pad2.cd()

    ratio_band = make_ratio_band(total_bkg)

    frame = ROOT.TH1D(
        
        "ratio_frame",
        
        "",
        
        nbins,
        
        0,
        
        nbins
        
    )
    frame.SetStats(0)
    
    frame.SetMinimum(0.)
    frame.SetMaximum(2.5)

    if draw_data:
        frame.GetYaxis().SetTitle("Data/Pred.")
    else:
        frame.GetYaxis().SetTitle("Asimov/Pred.")
    frame.GetXaxis().SetTitle("Bin number")

    frame.GetYaxis().SetNdivisions(505)

    frame.GetYaxis().SetTitleSize(0.09)
    frame.GetYaxis().SetLabelSize(0.08)
    frame.GetYaxis().SetTitleOffset(0.45)

    frame.GetXaxis().SetTitleSize(0.12)
    frame.GetXaxis().SetLabelSize(0.10)

    frame.Draw()

    ratio_band.Draw("E2 SAME")

    ratio_graph = make_ratio_graph(
        ratio_input,
        total_bkg,
    )

    ratio_graph.SetMarkerStyle(20)
    ratio_graph.SetMarkerSize(0.9)
    ratio_graph.SetMarkerColor(ROOT.kBlack)
    ratio_graph.SetLineColor(ROOT.kBlack)

    ratio_graph.Draw("PZ SAME")

    line = ROOT.TLine(
        total_bkg.GetXaxis().GetXmin(),
        1.0,
        nbins,
        1.0
    )

    line.SetLineStyle(2)
    line.Draw()

    # --------------------------------------------------------
    # Save
    # --------------------------------------------------------

    canvas.SaveAs(
        os.path.join(
            outdir,
            f"{region}.pdf"
        )
    )
    
    canvas.SaveAs(
        os.path.join(
            outdir,
            f"{region}.png"
        )
    )


# ------------------------------------------------------------
# Main
# ------------------------------------------------------------

def run_one_file(
    input_file,
    output_point_dir,
    era,
    logy,
    signal_mode,
    signal_scales,
    signal_colors,
    draw_data,
    mass,
    point_signal,
):

    global PROCESS_ERAS

    PROCESS_ERAS = get_process_eras(era)

    if not os.path.exists(input_file):
        print("")
        print("[WARNING] Input file does not exist:")
        print(" ", input_file)
        return

    f = ROOT.TFile.Open(input_file)

    if not f or f.IsZombie():
        print("")
        print("[WARNING] Failed to open input file:")
        print(" ", input_file)
        return

    # Keep a handle to shapes_prefit because postfit B-only plots
    # deliberately use the prefit signal as their signal overlay.
    prefit_fit_dir = f.Get("shapes_prefit")

    for fit_type in [
        "shapes_prefit",
        "shapes_fit_b",
        "shapes_fit_s",
    ]:

        fit_dir = f.Get(fit_type)

        if not fit_dir:
            print("")
            print("[WARNING] Missing fit directory:", fit_type)
            continue

        fit_outdir = os.path.join(
            output_point_dir,
            FIT_OUTPUT_DIR[fit_type],
        )

        variant_subdir = get_plot_variant_subdir(
            logy=logy,
            draw_data=draw_data,
        )

        if variant_subdir is not None:

            fit_outdir = os.path.join(
                fit_outdir,
                variant_subdir,
            )

        os.makedirs(fit_outdir, exist_ok=True)

        print("")
        print("=" * 80)
        print(fit_type)
        print("Output:", fit_outdir)
        print("=" * 80)

        for key in fit_dir.GetListOfKeys():

            obj = key.ReadObj()

            if not obj.InheritsFrom("TDirectory"):
                continue

            # ------------------------------------------------
            # Choose the source directory for the signal.
            #
            # Background source:
            #   prefit      -> shapes_prefit
            #   postfit B   -> shapes_fit_b
            #   postfit S+B -> shapes_fit_s
            #
            # Signal source:
            #   prefit      -> shapes_prefit
            #   postfit B   -> shapes_prefit
            #   postfit S+B -> shapes_fit_s
            # ------------------------------------------------
            if fit_type == "shapes_fit_b":

                signal_region_dir = None

                if prefit_fit_dir:

                    candidate = prefit_fit_dir.Get(
                        obj.GetName()
                    )

                    if (
                        candidate
                        and candidate.InheritsFrom("TDirectory")
                    ):
                        signal_region_dir = candidate

                if (
                    signal_region_dir is None
                    and signal_mode != "none"
                ):
                    print(
                        "[WARNING] Matching shapes_prefit region not found:",
                        obj.GetName(),
                        "- postfit B-only will be drawn without signal overlay.",
                    )

            else:

                # For prefit, the current region itself is the
                # prefit signal source.
                #
                # For postfit S+B, the current region contains
                # the genuinely fitted signal.
                signal_region_dir = obj

            plot_region(
                region_dir=obj,
                signal_region_dir=signal_region_dir,
                fit_type=fit_type,
                outdir=fit_outdir,
                logy=logy,
                signal_mode=signal_mode,
                signal_scales=signal_scales,
                signal_colors=signal_colors,
                draw_data=draw_data,
                mass=mass,
                point_signal=point_signal,
            )

    f.Close()


def main():
    args = parse_args()

    signal_colors = {
        "total": args.signal_color_total,
        "DY": args.signal_color_dy,
        "VBF": args.signal_color_vbf,
        "SSWW": args.signal_color_ssww,
        "Weinberg": args.signal_color_weinberg,
    }

    signal_mode_subdir = get_signal_mode_subdir(
        signal_mode=args.signal_mode,
        signal_preset=args.signal_preset,
    )

    for wp in args.InputWPs:
        for era in args.eras:
            for channel in args.channels:
                for signal in args.signals:
                    for tag in args.tags:
                        for tag_suffix in TAG_MAP[tag]:

                            if signal == "Weinberg":
                                mass_loop = [None]
                            else:
                                mass_loop = args.masses

                            for mass in mass_loop:

                                signal_scales = resolve_signal_scales(
                                    args=args,
                                    channel=channel,
                                    mass=mass,
                                )

                                point_name = build_point_name(
                                    era=era,
                                    channel=channel,
                                    mass=mass,
                                    signal=signal,
                                    tag_suffix=tag_suffix,
                                )

                                input_file = build_input_file(
                                    wp=wp,
                                    point_name=point_name,
                                )

                                output_point_dir = os.path.join(
                                    OUTDIR_BASE,
                                    wp,
                                    point_name,
                                    signal_mode_subdir,
                                )

                                print("")
                                print("#" * 100)
                                print("WP     :", wp)
                                print("Point  :", point_name)
                                print("Input  :", input_file)
                                print("Output :", output_point_dir)
                                print("Mode   :", signal_mode_subdir)
                                print("Scales :", signal_scales)
                                print("#" * 100)

                                run_one_file(
                                    input_file=input_file,
                                    output_point_dir=output_point_dir,
                                    era=era,
                                    logy=args.logy,
                                    signal_mode=args.signal_mode,
                                    signal_scales=signal_scales,
                                    signal_colors=signal_colors,
                                    draw_data=not args.no_data,
                                    mass=mass,
                                    point_signal=signal,
                                )


if __name__ == "__main__":
    main()
