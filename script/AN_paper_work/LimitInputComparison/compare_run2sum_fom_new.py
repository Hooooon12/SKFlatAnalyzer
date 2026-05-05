#!/usr/bin/env python3
"""
Flexible LimitInputs FOM comparison.

This script supports two workflows.

1) Legacy single-WP mode
   Compare the same WP in two representations:
     - era-sum : use era-separated templates and combine their sensitivities in quadrature
     - run2    : use the merged Run2 representation

   Example:
     python compare_run2sum_fom.py \
       --base-dir /path/to/WP \
       --regions sr1 sr2 sr3 \
       --channels EMu \
       --masses 800 \
       --signal-modes DYVBF SSWW \
       --bkg-modes bkgsum \
       --outdir run2sum_fom_EMu_sr123

2) Multi-WP mode
   Compare arbitrary WPs, each represented either as era-sum or as run2.

   Example:
     python compare_run2sum_fom.py \
       --wp WP1 era-sum /path/to/WP1 \
       --wp WP2 run2    /path/to/WP2 \
       --wp WP3 era-sum /path/to/WP3 \
       --reference-wp WP1 \
       --regions sr1 sr2 sr3 \
       --channels EMu \
       --masses 800 \
       --signal-modes DYVBF SSWW \
       --bkg-modes bkgsum \
       --outdir wp_fom_compare

Important conventions in this version:
  - User-facing outputs are TXT only. No TSV files are written.
  - Exposed loss/fraction quantities are reported on Z itself, not on Z^2.
  - S/B columns are replaced by S/sqrt(B).
  - The old weighted_mean / weighted_cv columns were removed from the output because
    they described an S/B-based Z^2 heterogeneity decomposition, not a directly
    reported sensitivity observable.
"""

from __future__ import annotations

import argparse
import itertools
import math
import os
import sys
from dataclasses import dataclass
from typing import Dict, Iterable, List, Optional, Sequence, Tuple

try:
    import ROOT  # type: ignore
    ROOT.gROOT.SetBatch(True)
except Exception as exc:  # pragma: no cover - ROOT is expected in the analysis env.
    print("[ERROR] Could not import ROOT. Run this inside your ROOT/CMSSW environment.")
    print("        import error:", exc)
    sys.exit(1)


DEFAULT_ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]

BACKGROUND_PROCS = [
    "fake",
    "cf",
    "zg",
    "mc_others",
    "wz",
    "wz_ewk",
    "zz",
    "ww",
]

SIGNAL_MODE_MAP = {
    "DYVBF": ["signalDY", "signalVBF"],
    "DY": ["signalDY"],
    "VBF": ["signalVBF"],
    "SSWW": ["signalSSWW"],
    "Weinberg": ["signalWeinberg"],
}

BKG_MODES = ["data_obs", "bkgsum"]

SOURCE_KIND_ALIASES = {
    "era": "era-sum",
    "erasum": "era-sum",
    "era-sum": "era-sum",
    "era_sum": "era-sum",
    "run2": "run2",
    "run2sum": "run2",
    "run2-sum": "run2",
    "run2_sum": "run2",
}

_HIST_CACHE: Dict[Tuple[str, str], Optional[object]] = {}
_CLONE_COUNTER = itertools.count()


@dataclass(frozen=True)
class ViewSpec:
    label: str
    source_kind: str
    base_dir: str

    @property
    def source_id(self) -> str:
        return f"{self.label}[{self.source_kind}]"


@dataclass
class HistPack:
    hist: Optional[object]
    missing: List[str]


@dataclass
class FOMResult:
    z_asimov: float
    z_simple: float
    total_s: float
    total_b: float
    z2_asimov_by_bin: List[float]
    z2_simple_by_bin: List[float]


@dataclass
class ViewCase:
    spec: ViewSpec
    region: str
    channel: str
    mass: str
    signal_mode: str
    bkg_mode: str
    merged_sig: object
    merged_bkg: object
    merged_fom: FOMResult
    total_s: float
    total_b: float
    era_sig_packs: List[HistPack]
    era_bkg_packs: List[HistPack]
    era_foms: Dict[str, FOMResult]
    era_local_z_simple: Dict[str, List[float]]
    era_local_z_asimov: Dict[str, List[float]]
    component_quad_local_z_simple: List[float]
    component_quad_local_z_asimov: List[float]
    component_quad_total_z_simple: float
    component_quad_total_z_asimov: float
    effective_local_z_simple: List[float]
    effective_local_z_asimov: List[float]
    effective_total_z_simple: float
    effective_total_z_asimov: float
    missing_signal_hists: List[str]
    missing_bkg_hists: List[str]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare FOM/sensitivity across LimitInputs WPs and/or era-sum vs Run2 representations.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--base-dir",
        default=None,
        help=(
            "Legacy single-WP mode. Compare the same WP in era-sum and Run2 forms. "
            "Ignored when --wp is provided."
        ),
    )
    parser.add_argument(
        "--base-label",
        default=None,
        help="Optional label to use in legacy single-WP mode.",
    )
    parser.add_argument(
        "--wp",
        action="append",
        nargs=3,
        metavar=("LABEL", "KIND", "BASE_DIR"),
        help=(
            "Multi-WP mode. Repeat this option as needed. "
            "KIND must be one of: era-sum, run2."
        ),
    )
    parser.add_argument(
        "--reference-wp",
        default=None,
        help=(
            "Reference WP label used as the baseline in pairwise comparisons. "
            "Default: the first resolved WP."
        ),
    )
    parser.add_argument(
        "--eras",
        nargs="+",
        default=DEFAULT_ERAS,
        help="Source eras.",
    )
    parser.add_argument(
        "--run2-era",
        default="Run2",
        help="Run2 input folder label under each base dir.",
    )
    parser.add_argument(
        "--regions",
        nargs="+",
        default=["sr1"],
        help="Regions to inspect, e.g. sr1 sr2 sr3.",
    )
    parser.add_argument(
        "--channels",
        nargs="+",
        default=["EMu"],
        help="Channels to inspect.",
    )
    parser.add_argument(
        "--masses",
        nargs="+",
        default=["600", "700", "800", "900", "1000", "1100", "1200", "1300", "1500", "1700", "2000", "2500", "3000"],
        help="Masses. Use either 800 or M800. Weinberg is also allowed.",
    )
    parser.add_argument(
        "--signal-modes",
        nargs="+",
        default=["DYVBF", "SSWW", "DY", "VBF"],
        choices=sorted(SIGNAL_MODE_MAP.keys()),
        help="Signal combinations to inspect.",
    )
    parser.add_argument(
        "--bkg-modes",
        nargs="+",
        default=["data_obs", "bkgsum"],
        choices=BKG_MODES,
        help="Background definition: data_obs or explicit sum of background processes.",
    )
    parser.add_argument(
        "--outdir",
        default="run2sum_fom_diagnostics",
        help="Output directory for TXT files.",
    )
    parser.add_argument(
        "--top-bins",
        type=int,
        default=8,
        help="Number of largest-loss bins to print per comparison.",
    )
    parser.add_argument(
        "--top-summary",
        type=int,
        default=30,
        help="Number of largest predicted limit-ratio rows to print at the end.",
    )
    parser.add_argument(
        "--epsilon",
        type=float,
        default=1.0e-12,
        help="Small positive value for denominators.",
    )
    parser.add_argument(
        "--quiet-missing",
        action="store_true",
        help="Do not print warnings for missing optional histograms.",
    )
    return parser.parse_args()


def normalize_source_kind(kind: str) -> str:
    key = str(kind).strip().lower()
    if key not in SOURCE_KIND_ALIASES:
        allowed = ", ".join(sorted(set(SOURCE_KIND_ALIASES.values())))
        raise ValueError(f"Unknown source kind '{kind}'. Allowed: {allowed}")
    return SOURCE_KIND_ALIASES[key]


def normalize_mass(mass: str) -> str:
    mass = str(mass).strip()
    if mass == "Weinberg":
        return mass
    return mass if mass.startswith("M") else "M" + mass


def card_input_path(base_dir: str, era: str, region: str, mass: str, channel: str) -> str:
    return os.path.join(base_dir, era, region, f"{mass}_{channel}_card_input.root")


def unique_hist_name(prefix: str) -> str:
    return f"{prefix}__clone_{next(_CLONE_COUNTER)}"


def clone_hist(hist: object, name: str) -> object:
    out = hist.Clone(name)
    out.SetDirectory(0)
    return out


def read_hist(file_path: str, hist_name: str, quiet: bool = False) -> Optional[object]:
    abs_path = os.path.abspath(file_path)
    key = (abs_path, hist_name)

    if key in _HIST_CACHE:
        cached = _HIST_CACHE[key]
        if cached is None:
            return None
        return clone_hist(cached, unique_hist_name(hist_name))

    if not os.path.exists(abs_path):
        if not quiet:
            print(f"[MISSING FILE] {abs_path}")
        _HIST_CACHE[key] = None
        return None

    root_file = ROOT.TFile.Open(abs_path, "READ")
    if not root_file or root_file.IsZombie():
        if not quiet:
            print(f"[BAD FILE] {abs_path}")
        _HIST_CACHE[key] = None
        return None

    hist = root_file.Get(hist_name)
    if not hist:
        if not quiet:
            print(f"[MISSING HIST] {hist_name} in {abs_path}")
        root_file.Close()
        _HIST_CACHE[key] = None
        return None

    cached = clone_hist(hist, unique_hist_name(hist_name))
    root_file.Close()
    _HIST_CACHE[key] = cached
    return clone_hist(cached, unique_hist_name(hist_name))


def zero_like(ref: object, name: str) -> object:
    out = clone_hist(ref, name)
    out.Reset("ICES")
    return out


def assert_compatible_binning(a: object, b: object, context: str) -> None:
    if a.GetNbinsX() != b.GetNbinsX():
        raise RuntimeError(f"Incompatible nbins for {context}: {a.GetNbinsX()} vs {b.GetNbinsX()}")

    ax = a.GetXaxis()
    bx = b.GetXaxis()
    for i in range(1, a.GetNbinsX() + 2):
        low_a = ax.GetBinLowEdge(i)
        low_b = bx.GetBinLowEdge(i)
        if abs(low_a - low_b) > 1e-9:
            raise RuntimeError(
                f"Incompatible bin edge for {context}, edge {i}: {low_a} vs {low_b}"
            )


def binning_status(a: Optional[object], b: Optional[object]) -> str:
    if a is None or b is None:
        return "missing_hist"
    if a.GetNbinsX() != b.GetNbinsX():
        return f"nbins:{a.GetNbinsX()}vs{b.GetNbinsX()}"

    ax = a.GetXaxis()
    bx = b.GetXaxis()
    for i in range(1, a.GetNbinsX() + 2):
        low_a = ax.GetBinLowEdge(i)
        low_b = bx.GetBinLowEdge(i)
        if abs(low_a - low_b) > 1e-9:
            return f"edge_{i}:{low_a}vs{low_b}"
    return "ok"


def add_hists(hists: Sequence[object], name: str) -> Optional[object]:
    valid = [h for h in hists if h is not None]
    if not valid:
        return None

    out = zero_like(valid[0], name)
    for h in valid:
        assert_compatible_binning(out, h, name)
        out.Add(h)
    return out


def relative_diff(a: float, b: float, eps: float = 1.0e-12) -> float:
    denom = max(abs(a), eps)
    return (b - a) / denom


def safe_positive(value: float) -> float:
    return max(float(value), 0.0)


def asimov_z2(s: float, b: float, eps: float = 1.0e-12) -> float:
    s = safe_positive(s)
    b = max(float(b), eps)
    if s <= 0.0:
        return 0.0
    val = 2.0 * ((s + b) * math.log(1.0 + s / b) - s)
    return max(val, 0.0)


def simple_z2(s: float, b: float, eps: float = 1.0e-12) -> float:
    s = safe_positive(s)
    b = max(float(b), eps)
    return (s * s) / b


def local_asimov_z(s: float, b: float, eps: float = 1.0e-12) -> float:
    return math.sqrt(asimov_z2(s, b, eps))


def local_simple_z(s: float, b: float, eps: float = 1.0e-12) -> float:
    return math.sqrt(simple_z2(s, b, eps))


def total_s_over_sqrt_b(s: float, b: float, eps: float = 1.0e-12) -> float:
    s = safe_positive(s)
    if s <= 0.0:
        return 0.0
    return s / math.sqrt(max(float(b), eps))


def compute_fom(sig: object, bkg: object, eps: float = 1.0e-12) -> FOMResult:
    assert_compatible_binning(sig, bkg, "compute_fom")

    z2_a: List[float] = []
    z2_s: List[float] = []
    total_s = 0.0
    total_b = 0.0

    for ibin in range(1, sig.GetNbinsX() + 1):
        s = safe_positive(sig.GetBinContent(ibin))
        b = safe_positive(bkg.GetBinContent(ibin))
        total_s += s
        total_b += b
        z2_a.append(asimov_z2(s, b, eps))
        z2_s.append(simple_z2(s, b, eps))

    return FOMResult(
        z_asimov=math.sqrt(sum(z2_a)),
        z_simple=math.sqrt(sum(z2_s)),
        total_s=total_s,
        total_b=total_b,
        z2_asimov_by_bin=z2_a,
        z2_simple_by_bin=z2_s,
    )


def safe_limit_ratio(z_ref: float, z_cmp: float, eps: float = 1.0e-12) -> float:
    if z_cmp <= eps:
        return float("inf")
    return z_ref / z_cmp


def positive_loss_fraction(z_ref: float, z_cmp: float, eps: float = 1.0e-12) -> float:
    if z_ref <= eps:
        return 0.0
    return max(z_ref - z_cmp, 0.0) / z_ref


def build_signal_from_era_file(
    base_dir: str,
    era: str,
    region: str,
    mass: str,
    channel: str,
    signal_mode: str,
    quiet: bool,
) -> HistPack:
    fpath = card_input_path(base_dir, era, region, mass, channel)
    hists: List[object] = []
    missing: List[str] = []

    for proc in SIGNAL_MODE_MAP[signal_mode]:
        h = read_hist(fpath, proc, quiet=True)
        if h is None:
            missing.append(f"{era}:{proc}")
            continue
        hists.append(h)

    return HistPack(add_hists(hists, unique_hist_name(f"sig_{era}_{signal_mode}")), missing)


def build_signal_from_run2_file(
    base_dir: str,
    run2_era: str,
    era: str,
    region: str,
    mass: str,
    channel: str,
    signal_mode: str,
    quiet: bool,
) -> HistPack:
    fpath = card_input_path(base_dir, run2_era, region, mass, channel)
    hists: List[object] = []
    missing: List[str] = []

    for proc in SIGNAL_MODE_MAP[signal_mode]:
        hist_name = f"{proc}_{era}"
        h = read_hist(fpath, hist_name, quiet=True)
        if h is None:
            missing.append(f"{run2_era}:{hist_name}")
            continue
        hists.append(h)

    return HistPack(add_hists(hists, unique_hist_name(f"sig_{run2_era}_{era}_{signal_mode}")), missing)


def build_bkg_from_era_file(
    base_dir: str,
    era: str,
    region: str,
    mass: str,
    channel: str,
    bkg_mode: str,
    quiet: bool,
) -> HistPack:
    fpath = card_input_path(base_dir, era, region, mass, channel)

    if bkg_mode == "data_obs":
        h = read_hist(fpath, "data_obs", quiet=quiet)
        return HistPack(h, [] if h is not None else [f"{era}:data_obs"])

    hists: List[object] = []
    missing: List[str] = []

    for proc in BACKGROUND_PROCS:
        h = read_hist(fpath, proc, quiet=True)
        if h is None:
            missing.append(f"{era}:{proc}")
            continue
        hists.append(h)

    return HistPack(add_hists(hists, unique_hist_name(f"bkg_{era}_{bkg_mode}")), missing)


def build_bkg_from_run2_file(
    base_dir: str,
    run2_era: str,
    era: str,
    region: str,
    mass: str,
    channel: str,
    bkg_mode: str,
    quiet: bool,
) -> HistPack:
    if bkg_mode == "data_obs":
        # data_obs is stored as a merged histogram in the Run2 file, not copied per era.
        return build_bkg_from_era_file(base_dir, era, region, mass, channel, bkg_mode, quiet)

    fpath = card_input_path(base_dir, run2_era, region, mass, channel)
    hists: List[object] = []
    missing: List[str] = []

    for proc in BACKGROUND_PROCS:
        hist_name = f"{proc}_{era}"
        h = read_hist(fpath, hist_name, quiet=True)
        if h is None:
            missing.append(f"{run2_era}:{hist_name}")
            continue
        hists.append(h)

    return HistPack(add_hists(hists, unique_hist_name(f"bkg_{run2_era}_{era}_{bkg_mode}")), missing)


def sum_era_hists(packs: Sequence[HistPack], name: str) -> Optional[object]:
    return add_hists([p.hist for p in packs if p.hist is not None], name)


def bin_edges(hist: object, ibin: int) -> Tuple[float, float]:
    ax = hist.GetXaxis()
    return ax.GetBinLowEdge(ibin), ax.GetBinUpEdge(ibin)


def max_bin_abs_diff(a: Optional[object], b: Optional[object]) -> Tuple[float, float]:
    if a is None or b is None:
        return float("nan"), float("nan")

    assert_compatible_binning(a, b, "max_bin_abs_diff")
    max_abs = 0.0
    max_rel = 0.0

    for ibin in range(1, a.GetNbinsX() + 1):
        av = a.GetBinContent(ibin)
        bv = b.GetBinContent(ibin)
        diff = abs(bv - av)
        rel = diff / max(abs(av), 1.0e-12)
        max_abs = max(max_abs, diff)
        max_rel = max(max_rel, rel)

    return max_abs, max_rel


def check_process_copy_sanity(
    spec: ViewSpec,
    run2_era: str,
    eras: Sequence[str],
    region: str,
    mass: str,
    channel: str,
    quiet: bool,
) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    procs = BACKGROUND_PROCS + ["data_obs"] + sorted({p for mode in SIGNAL_MODE_MAP.values() for p in mode})

    run2_file = card_input_path(spec.base_dir, run2_era, region, mass, channel)
    if not os.path.exists(run2_file):
        return rows

    for era in eras:
        era_file = card_input_path(spec.base_dir, era, region, mass, channel)
        for proc in procs:
            h_era = read_hist(era_file, proc, quiet=True)

            if proc == "data_obs":
                continue

            h_run2 = read_hist(run2_file, f"{proc}_{era}", quiet=True)

            if h_era is None and h_run2 is None:
                continue

            if h_era is None or h_run2 is None:
                rows.append({
                    "wp": spec.label,
                    "region": region,
                    "channel": channel,
                    "mass": mass,
                    "era": era,
                    "process": proc,
                    "status": "missing_one_side",
                    "era_integral": "nan" if h_era is None else h_era.Integral(),
                    "run2_integral": "nan" if h_run2 is None else h_run2.Integral(),
                    "rel_integral_diff": "nan",
                    "max_bin_abs_diff": "nan",
                    "max_bin_rel_diff": "nan",
                })
                continue

            era_int = h_era.Integral()
            run2_int = h_run2.Integral()
            max_abs, max_rel = max_bin_abs_diff(h_era, h_run2)

            rows.append({
                "wp": spec.label,
                "region": region,
                "channel": channel,
                "mass": mass,
                "era": era,
                "process": proc,
                "status": "ok" if abs(relative_diff(era_int, run2_int)) < 1.0e-9 and max_rel < 1.0e-9 else "diff",
                "era_integral": era_int,
                "run2_integral": run2_int,
                "rel_integral_diff": relative_diff(era_int, run2_int),
                "max_bin_abs_diff": max_abs,
                "max_bin_rel_diff": max_rel,
            })

    return rows


def build_view_case(
    spec: ViewSpec,
    eras: Sequence[str],
    run2_era: str,
    region: str,
    mass: str,
    channel: str,
    signal_mode: str,
    bkg_mode: str,
    eps: float,
    quiet: bool,
) -> Optional[ViewCase]:
    era_sig_packs: List[HistPack] = []
    era_bkg_packs: List[HistPack] = []
    missing_signal: List[str] = []
    missing_bkg: List[str] = []

    for era in eras:
        if spec.source_kind == "era-sum":
            sig_pack = build_signal_from_era_file(spec.base_dir, era, region, mass, channel, signal_mode, quiet)
            bkg_pack = build_bkg_from_era_file(spec.base_dir, era, region, mass, channel, bkg_mode, quiet)
        elif spec.source_kind == "run2":
            sig_pack = build_signal_from_run2_file(spec.base_dir, run2_era, era, region, mass, channel, signal_mode, quiet)
            bkg_pack = build_bkg_from_run2_file(spec.base_dir, run2_era, era, region, mass, channel, bkg_mode, quiet)
        else:
            raise RuntimeError(f"Unhandled source kind: {spec.source_kind}")

        era_sig_packs.append(sig_pack)
        era_bkg_packs.append(bkg_pack)
        missing_signal.extend(sig_pack.missing)
        missing_bkg.extend(bkg_pack.missing)

    merged_sig = sum_era_hists(era_sig_packs, unique_hist_name(f"merged_sig_{spec.label}_{region}_{channel}_{mass}_{signal_mode}"))
    merged_bkg: Optional[object]

    if spec.source_kind == "run2" and bkg_mode == "data_obs":
        run2_file = card_input_path(spec.base_dir, run2_era, region, mass, channel)
        merged_bkg = read_hist(run2_file, "data_obs", quiet=quiet)
        if merged_bkg is None:
            merged_bkg = sum_era_hists(era_bkg_packs, unique_hist_name(f"fallback_sum_data_obs_{spec.label}_{region}_{channel}_{mass}"))
            if merged_bkg is not None:
                missing_bkg.append(f"{run2_era}:data_obs")
    else:
        merged_bkg = sum_era_hists(era_bkg_packs, unique_hist_name(f"merged_bkg_{spec.label}_{region}_{channel}_{mass}_{bkg_mode}"))

    if merged_sig is None or merged_bkg is None:
        return None

    merged_fom = compute_fom(merged_sig, merged_bkg, eps)
    if merged_fom.total_s <= eps:
        return None
    nbins = merged_sig.GetNbinsX()

    era_foms: Dict[str, FOMResult] = {}
    era_local_z_simple: Dict[str, List[float]] = {}
    era_local_z_asimov: Dict[str, List[float]] = {}

    component_quad_local_z_simple = [0.0] * nbins
    component_quad_local_z_asimov = [0.0] * nbins

    for era, sig_pack, bkg_pack in zip(eras, era_sig_packs, era_bkg_packs):
        local_simple = [0.0] * nbins
        local_asimov = [0.0] * nbins
        era_local_z_simple[era] = local_simple
        era_local_z_asimov[era] = local_asimov

        if sig_pack.hist is None or bkg_pack.hist is None:
            continue

        assert_compatible_binning(sig_pack.hist, merged_sig, f"sig_view_component_{spec.source_id}_{era}")
        assert_compatible_binning(bkg_pack.hist, merged_bkg, f"bkg_view_component_{spec.source_id}_{era}")

        fom = compute_fom(sig_pack.hist, bkg_pack.hist, eps)
        era_foms[era] = fom

        for i in range(nbins):
            z_simple = math.sqrt(fom.z2_simple_by_bin[i])
            z_asimov = math.sqrt(fom.z2_asimov_by_bin[i])
            local_simple[i] = z_simple
            local_asimov[i] = z_asimov
            component_quad_local_z_simple[i] += z_simple * z_simple
            component_quad_local_z_asimov[i] += z_asimov * z_asimov

    component_quad_local_z_simple = [math.sqrt(v) for v in component_quad_local_z_simple]
    component_quad_local_z_asimov = [math.sqrt(v) for v in component_quad_local_z_asimov]
    component_quad_total_z_simple = math.sqrt(sum(v * v for v in component_quad_local_z_simple))
    component_quad_total_z_asimov = math.sqrt(sum(v * v for v in component_quad_local_z_asimov))

    if spec.source_kind == "era-sum":
        effective_local_z_simple = list(component_quad_local_z_simple)
        effective_local_z_asimov = list(component_quad_local_z_asimov)
        effective_total_z_simple = component_quad_total_z_simple
        effective_total_z_asimov = component_quad_total_z_asimov
    else:
        effective_local_z_simple = [math.sqrt(v) for v in merged_fom.z2_simple_by_bin]
        effective_local_z_asimov = [math.sqrt(v) for v in merged_fom.z2_asimov_by_bin]
        effective_total_z_simple = merged_fom.z_simple
        effective_total_z_asimov = merged_fom.z_asimov

    return ViewCase(
        spec=spec,
        region=region,
        channel=channel,
        mass=mass,
        signal_mode=signal_mode,
        bkg_mode=bkg_mode,
        merged_sig=merged_sig,
        merged_bkg=merged_bkg,
        merged_fom=merged_fom,
        total_s=merged_fom.total_s,
        total_b=merged_fom.total_b,
        era_sig_packs=era_sig_packs,
        era_bkg_packs=era_bkg_packs,
        era_foms=era_foms,
        era_local_z_simple=era_local_z_simple,
        era_local_z_asimov=era_local_z_asimov,
        component_quad_local_z_simple=component_quad_local_z_simple,
        component_quad_local_z_asimov=component_quad_local_z_asimov,
        component_quad_total_z_simple=component_quad_total_z_simple,
        component_quad_total_z_asimov=component_quad_total_z_asimov,
        effective_local_z_simple=effective_local_z_simple,
        effective_local_z_asimov=effective_local_z_asimov,
        effective_total_z_simple=effective_total_z_simple,
        effective_total_z_asimov=effective_total_z_asimov,
        missing_signal_hists=sorted(set(missing_signal)),
        missing_bkg_hists=sorted(set(missing_bkg)),
    )


def dominant_era(local_map: Dict[str, List[float]], eras: Sequence[str], ibin0: int) -> Tuple[str, float]:
    if not eras:
        return "", 0.0

    best_era = ""
    best_val = -1.0
    sum_quad = 0.0

    for era in eras:
        vals = local_map.get(era, [])
        z = vals[ibin0] if ibin0 < len(vals) else 0.0
        sum_quad += z * z
        if z > best_val:
            best_val = z
            best_era = era

    denom = math.sqrt(sum_quad) if sum_quad > 0.0 else 0.0
    frac = best_val / denom if denom > 0.0 else 0.0
    return best_era, frac


def build_summary_row(
    ref_case: ViewCase,
    cmp_case: ViewCase,
    eps: float,
) -> Dict[str, object]:
    return {
        "ref_wp": ref_case.spec.label,
        "ref_kind": ref_case.spec.source_kind,
        "cmp_wp": cmp_case.spec.label,
        "cmp_kind": cmp_case.spec.source_kind,
        "region": ref_case.region,
        "channel": ref_case.channel,
        "mass": ref_case.mass,
        "signal_mode": ref_case.signal_mode,
        "bkg_mode": ref_case.bkg_mode,
        "ref_sumS": ref_case.total_s,
        "ref_sumB": ref_case.total_b,
        "ref_S_over_sqrtB": total_s_over_sqrt_b(ref_case.total_s, ref_case.total_b, eps),
        "cmp_sumS": cmp_case.total_s,
        "cmp_sumB": cmp_case.total_b,
        "cmp_S_over_sqrtB": total_s_over_sqrt_b(cmp_case.total_s, cmp_case.total_b, eps),
        "ref_z_simple": ref_case.effective_total_z_simple,
        "cmp_z_simple": cmp_case.effective_total_z_simple,
        "pred_limit_ratio_simple": safe_limit_ratio(ref_case.effective_total_z_simple, cmp_case.effective_total_z_simple, eps),
        "sensitivity_loss_frac_simple": positive_loss_fraction(ref_case.effective_total_z_simple, cmp_case.effective_total_z_simple, eps),
        "ref_z_asimov": ref_case.effective_total_z_asimov,
        "cmp_z_asimov": cmp_case.effective_total_z_asimov,
        "pred_limit_ratio_asimov": safe_limit_ratio(ref_case.effective_total_z_asimov, cmp_case.effective_total_z_asimov, eps),
        "sensitivity_loss_frac_asimov": positive_loss_fraction(ref_case.effective_total_z_asimov, cmp_case.effective_total_z_asimov, eps),
        "ref_component_quad_total_z_simple": ref_case.component_quad_total_z_simple,
        "cmp_component_quad_total_z_simple": cmp_case.component_quad_total_z_simple,
        "ref_component_quad_total_z_asimov": ref_case.component_quad_total_z_asimov,
        "cmp_component_quad_total_z_asimov": cmp_case.component_quad_total_z_asimov,
        "binning_status": binning_status(ref_case.merged_sig, cmp_case.merged_sig),
        "ref_missing_signal_hists": ",".join(ref_case.missing_signal_hists),
        "ref_missing_bkg_hists": ",".join(ref_case.missing_bkg_hists),
        "cmp_missing_signal_hists": ",".join(cmp_case.missing_signal_hists),
        "cmp_missing_bkg_hists": ",".join(cmp_case.missing_bkg_hists),
    }


def build_era_yield_rows(
    case: ViewCase,
    eras: Sequence[str],
    eps: float,
) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []

    for era in eras:
        fom = case.era_foms.get(era)
        if fom is None:
            continue

        rows.append({
            "wp": case.spec.label,
            "source_kind": case.spec.source_kind,
            "region": case.region,
            "channel": case.channel,
            "mass": case.mass,
            "signal_mode": case.signal_mode,
            "bkg_mode": case.bkg_mode,
            "era": era,
            "S": fom.total_s,
            "B": fom.total_b,
            "S_over_sqrtB": total_s_over_sqrt_b(fom.total_s, fom.total_b, eps),
            "z_asimov": fom.z_asimov,
            "z_simple": fom.z_simple,
            "component_quad_total_z_asimov": case.component_quad_total_z_asimov,
            "component_quad_total_z_simple": case.component_quad_total_z_simple,
            "view_effective_z_asimov": case.effective_total_z_asimov,
            "view_effective_z_simple": case.effective_total_z_simple,
            "component_sensitivity_fraction_asimov": (
                fom.z_asimov / max(case.component_quad_total_z_asimov, eps)
                if case.component_quad_total_z_asimov > eps else 0.0
            ),
            "component_sensitivity_fraction_simple": (
                fom.z_simple / max(case.component_quad_total_z_simple, eps)
                if case.component_quad_total_z_simple > eps else 0.0
            ),
        })

    return rows


def build_bin_rows(
    ref_case: ViewCase,
    cmp_case: ViewCase,
    eras: Sequence[str],
    eps: float,
) -> List[Dict[str, object]]:
    if binning_status(ref_case.merged_sig, cmp_case.merged_sig) != "ok":
        return []

    rows: List[Dict[str, object]] = []
    nbins = ref_case.merged_sig.GetNbinsX()

    for ibin in range(1, nbins + 1):
        i0 = ibin - 1
        low, high = bin_edges(ref_case.merged_sig, ibin)

        ref_s = safe_positive(ref_case.merged_sig.GetBinContent(ibin))
        ref_b = safe_positive(ref_case.merged_bkg.GetBinContent(ibin))
        cmp_s = safe_positive(cmp_case.merged_sig.GetBinContent(ibin))
        cmp_b = safe_positive(cmp_case.merged_bkg.GetBinContent(ibin))

        ref_dom_era, ref_dom_frac = dominant_era(ref_case.era_local_z_simple, eras, i0)
        cmp_dom_era, cmp_dom_frac = dominant_era(cmp_case.era_local_z_simple, eras, i0)

        row: Dict[str, object] = {
            "ref_wp": ref_case.spec.label,
            "ref_kind": ref_case.spec.source_kind,
            "cmp_wp": cmp_case.spec.label,
            "cmp_kind": cmp_case.spec.source_kind,
            "region": ref_case.region,
            "channel": ref_case.channel,
            "mass": ref_case.mass,
            "signal_mode": ref_case.signal_mode,
            "bkg_mode": ref_case.bkg_mode,
            "bin": ibin,
            "bin_low": low,
            "bin_high": high,
            "ref_S": ref_s,
            "ref_B": ref_b,
            "ref_S_over_sqrtB": total_s_over_sqrt_b(ref_s, ref_b, eps),
            "ref_z_simple": ref_case.effective_local_z_simple[i0],
            "ref_z_asimov": ref_case.effective_local_z_asimov[i0],
            "ref_component_quad_bin_z_simple": ref_case.component_quad_local_z_simple[i0],
            "cmp_S": cmp_s,
            "cmp_B": cmp_b,
            "cmp_S_over_sqrtB": total_s_over_sqrt_b(cmp_s, cmp_b, eps),
            "cmp_z_simple": cmp_case.effective_local_z_simple[i0],
            "cmp_z_asimov": cmp_case.effective_local_z_asimov[i0],
            "cmp_component_quad_bin_z_simple": cmp_case.component_quad_local_z_simple[i0],
            "bin_limit_ratio_simple": safe_limit_ratio(ref_case.effective_local_z_simple[i0], cmp_case.effective_local_z_simple[i0], eps),
            "bin_sensitivity_loss_abs_simple": max(ref_case.effective_local_z_simple[i0] - cmp_case.effective_local_z_simple[i0], 0.0),
            "bin_sensitivity_loss_frac_simple": positive_loss_fraction(ref_case.effective_local_z_simple[i0], cmp_case.effective_local_z_simple[i0], eps),
            "bin_limit_ratio_asimov": safe_limit_ratio(ref_case.effective_local_z_asimov[i0], cmp_case.effective_local_z_asimov[i0], eps),
            "bin_sensitivity_loss_abs_asimov": max(ref_case.effective_local_z_asimov[i0] - cmp_case.effective_local_z_asimov[i0], 0.0),
            "bin_sensitivity_loss_frac_asimov": positive_loss_fraction(ref_case.effective_local_z_asimov[i0], cmp_case.effective_local_z_asimov[i0], eps),
            "ref_dominant_era_simple": ref_dom_era,
            "ref_dominant_era_component_frac_simple": ref_dom_frac,
            "cmp_dominant_era_simple": cmp_dom_era,
            "cmp_dominant_era_component_frac_simple": cmp_dom_frac,
        }

        for era in eras:
            row[f"ref_z_simple_{era}"] = ref_case.era_local_z_simple.get(era, [0.0] * nbins)[i0]
            row[f"cmp_z_simple_{era}"] = cmp_case.era_local_z_simple.get(era, [0.0] * nbins)[i0]

        rows.append(row)

    return rows


TEXT_FIELDS = {
    "wp",
    "ref_wp",
    "ref_kind",
    "cmp_wp",
    "cmp_kind",
    "region",
    "channel",
    "mass",
    "signal_mode",
    "bkg_mode",
    "era",
    "process",
    "status",
    "binning_status",
    "ref_missing_signal_hists",
    "ref_missing_bkg_hists",
    "cmp_missing_signal_hists",
    "cmp_missing_bkg_hists",
    "ref_dominant_era_simple",
    "cmp_dominant_era_simple",
    "source_kind",
}


def format_for_human(value: object, field: str = "") -> str:
    if value is None:
        return ""

    if isinstance(value, str):
        return value

    try:
        x = float(value)
    except Exception:
        return str(value)

    if math.isnan(x):
        return "nan"
    if math.isinf(x):
        return "inf" if x > 0 else "-inf"

    if field == "bin":
        return f"{int(round(x))}"

    ax = abs(x)
    if ax == 0.0:
        return "0"

    if ax >= 1.0e4 or ax < 1.0e-3:
        return f"{x:.3e}"
    if ax >= 1.0e3:
        return f"{x:.1f}"
    if ax >= 1.0e2:
        return f"{x:.2f}"
    if ax >= 10.0:
        return f"{x:.3f}"
    return f"{x:.4f}"


def write_pretty_table(
    path: str,
    rows: Sequence[Dict[str, object]],
    fieldnames: Sequence[str],
    notes: Optional[Sequence[str]] = None,
) -> None:
    os.makedirs(os.path.dirname(path), exist_ok=True)

    formatted_rows: List[Dict[str, str]] = []
    widths = {field: len(field) for field in fieldnames}

    for row in rows:
        out_row: Dict[str, str] = {}
        for field in fieldnames:
            cell = format_for_human(row.get(field, ""), field)
            out_row[field] = cell
            widths[field] = max(widths[field], len(cell))
        formatted_rows.append(out_row)

    with open(path, "w") as f:
        if notes:
            for line in notes:
                f.write(line.rstrip() + "\n")
            f.write("\n")

        header_cells = []
        for field in fieldnames:
            if field in TEXT_FIELDS:
                header_cells.append(field.ljust(widths[field]))
            else:
                header_cells.append(field.rjust(widths[field]))
        f.write("  ".join(header_cells) + "\n")

        rule_cells = ["-" * widths[field] for field in fieldnames]
        f.write("  ".join(rule_cells) + "\n")

        for row in formatted_rows:
            cells = []
            for field in fieldnames:
                cell = row[field]
                if field in TEXT_FIELDS:
                    cells.append(cell.ljust(widths[field]))
                else:
                    cells.append(cell.rjust(widths[field]))
            f.write("  ".join(cells) + "\n")


def fmt_float(x: object) -> str:
    return format_for_human(x)


def resolve_specs(args: argparse.Namespace) -> List[ViewSpec]:
    if args.wp:
        specs: List[ViewSpec] = []
        labels_seen = set()

        for label, kind, base_dir in args.wp:
            if label in labels_seen:
                raise ValueError(f"Duplicated --wp label '{label}'. Labels must be unique.")
            labels_seen.add(label)
            specs.append(
                ViewSpec(
                    label=label,
                    source_kind=normalize_source_kind(kind),
                    base_dir=os.path.abspath(base_dir),
                )
            )
        return specs

    if args.base_dir:
        base_dir = os.path.abspath(args.base_dir)
        base_label = args.base_label or os.path.basename(base_dir.rstrip("/")) or "WP"
        return [
            ViewSpec(label=f"{base_label}_era", source_kind="era-sum", base_dir=base_dir),
            ViewSpec(label=f"{base_label}_run2", source_kind="run2", base_dir=base_dir),
        ]

    raise ValueError("Either --base-dir or at least one --wp must be provided.")


def main() -> int:
    args = parse_args()
    try:
        specs = resolve_specs(args)
    except ValueError as exc:
        print("[ERROR]", exc)
        return 2

    masses = [normalize_mass(m) for m in args.masses]
    if not specs:
        print("[ERROR] No valid WP specifications were resolved.")
        return 2

    reference_label = args.reference_wp or specs[0].label
    label_to_spec = {spec.label: spec for spec in specs}
    if reference_label not in label_to_spec:
        print(f"[ERROR] --reference-wp '{reference_label}' was not found in the resolved WP labels.")
        return 2

    os.makedirs(args.outdir, exist_ok=True)

    summary_rows: List[Dict[str, object]] = []
    era_yield_rows: List[Dict[str, object]] = []
    bin_rows: List[Dict[str, object]] = []
    sanity_rows: List[Dict[str, object]] = []

    sanity_specs_by_base_dir: Dict[str, ViewSpec] = {}
    for spec in specs:
        prev = sanity_specs_by_base_dir.get(spec.base_dir)
        if prev is None or (prev.source_kind != "run2" and spec.source_kind == "run2"):
            sanity_specs_by_base_dir[spec.base_dir] = spec
    sanity_specs = list(sanity_specs_by_base_dir.values())

    print("============================================================")
    print("Flexible LimitInputs FOM comparison")
    print("------------------------------------------------------------")
    print("WPs      :")
    for spec in specs:
        print(f"  - {spec.label:<20}  kind={spec.source_kind:<7}  base_dir={spec.base_dir}")
    print("reference:", reference_label)
    print("eras     :", ", ".join(args.eras))
    print("run2 era :", args.run2_era)
    print("regions  :", ", ".join(args.regions))
    print("channels :", ", ".join(args.channels))
    print("masses   :", ", ".join(masses))
    print("signals  :", ", ".join(args.signal_modes))
    print("bkg modes:", ", ".join(args.bkg_modes))
    print("outdir   :", args.outdir)
    print("============================================================\n")

    sanity_done_keys = set()

    for region in args.regions:
        for channel in args.channels:
            for mass in masses:
                for spec in sanity_specs:
                    sanity_key = (spec.base_dir, region, channel, mass)
                    if sanity_key in sanity_done_keys:
                        continue
                    sanity_done_keys.add(sanity_key)
                    sanity_rows.extend(
                        check_process_copy_sanity(spec, args.run2_era, args.eras, region, mass, channel, args.quiet_missing)
                    )

                for signal_mode in args.signal_modes:
                    for bkg_mode in args.bkg_modes:
                        view_cases: Dict[str, ViewCase] = {}

                        for spec in specs:
                            case = build_view_case(
                                spec=spec,
                                eras=args.eras,
                                run2_era=args.run2_era,
                                region=region,
                                mass=mass,
                                channel=channel,
                                signal_mode=signal_mode,
                                bkg_mode=bkg_mode,
                                eps=args.epsilon,
                                quiet=args.quiet_missing,
                            )
                            if case is None:
                                if not args.quiet_missing:
                                    print(
                                        f"[SKIP VIEW] {spec.source_id} "
                                        f"{region} {channel} {mass} {signal_mode} {bkg_mode}"
                                    )
                                continue

                            view_cases[spec.label] = case
                            era_yield_rows.extend(build_era_yield_rows(case, args.eras, args.epsilon))

                        ref_case = view_cases.get(reference_label)
                        if ref_case is None:
                            continue

                        for spec in specs:
                            if spec.label == reference_label:
                                continue
                            cmp_case = view_cases.get(spec.label)
                            if cmp_case is None:
                                continue

                            summary_row = build_summary_row(ref_case, cmp_case, args.epsilon)
                            summary_rows.append(summary_row)

                            case_bin_rows = build_bin_rows(ref_case, cmp_case, args.eras, args.epsilon)
                            bin_rows.extend(case_bin_rows)

                            if bkg_mode == "data_obs":
                                print(
                                    f"[{region:<4} {channel:<4} {mass:<7} {signal_mode:<7}] "
                                    f"{ref_case.spec.source_id} vs {cmp_case.spec.source_id}  "
                                    f"Zs(ref)={fmt_float(ref_case.effective_total_z_simple):>10}  "
                                    f"Zs(cmp)={fmt_float(cmp_case.effective_total_z_simple):>10}  "
                                    f"limRatio={fmt_float(summary_row['pred_limit_ratio_simple']):>10}  "
                                    f"loss={100.0 * float(summary_row['sensitivity_loss_frac_simple']):>7.2f}%"
                                )

                                related_bins = [
                                    r for r in case_bin_rows
                                ]
                                related_bins = sorted(
                                    related_bins,
                                    key=lambda r: float(r["bin_sensitivity_loss_abs_simple"]),
                                    reverse=True,
                                )
                                for r in related_bins[: args.top_bins]:
                                    print(
                                        "  bin {bin:>2} [{bin_low:>8}, {bin_high:>8}]  "
                                        "lossZ={loss:>10}  "
                                        "z_ref={zref:>10}  z_cmp={zcmp:>10}  "
                                        "dom_ref={domr:<12} ({domrf:.2f})  "
                                        "dom_cmp={domc:<12} ({domcf:.2f})".format(
                                            bin=int(r["bin"]),
                                            bin_low=fmt_float(float(r["bin_low"])),
                                            bin_high=fmt_float(float(r["bin_high"])),
                                            loss=fmt_float(float(r["bin_sensitivity_loss_abs_simple"])),
                                            zref=fmt_float(float(r["ref_z_simple"])),
                                            zcmp=fmt_float(float(r["cmp_z_simple"])),
                                            domr=str(r["ref_dominant_era_simple"]),
                                            domrf=float(r["ref_dominant_era_component_frac_simple"]),
                                            domc=str(r["cmp_dominant_era_simple"]),
                                            domcf=float(r["cmp_dominant_era_component_frac_simple"]),
                                        )
                                    )
                                print("")

    summary_fields = [
        "ref_wp", "ref_kind", "cmp_wp", "cmp_kind",
        "region", "channel", "mass", "signal_mode", "bkg_mode",
        "ref_sumS", "ref_sumB", "ref_S_over_sqrtB",
        "cmp_sumS", "cmp_sumB", "cmp_S_over_sqrtB",
        "ref_z_simple", "cmp_z_simple", "pred_limit_ratio_simple", "sensitivity_loss_frac_simple",
        "ref_z_asimov", "cmp_z_asimov", "pred_limit_ratio_asimov", "sensitivity_loss_frac_asimov",
        "ref_component_quad_total_z_simple", "cmp_component_quad_total_z_simple",
        "ref_component_quad_total_z_asimov", "cmp_component_quad_total_z_asimov",
        "binning_status",
        "ref_missing_signal_hists", "ref_missing_bkg_hists",
        "cmp_missing_signal_hists", "cmp_missing_bkg_hists",
    ]

    era_yield_fields = [
        "wp", "source_kind",
        "region", "channel", "mass", "signal_mode", "bkg_mode", "era",
        "S", "B", "S_over_sqrtB",
        "z_asimov", "z_simple",
        "component_quad_total_z_asimov", "component_quad_total_z_simple",
        "view_effective_z_asimov", "view_effective_z_simple",
        "component_sensitivity_fraction_asimov", "component_sensitivity_fraction_simple",
    ]

    bin_fields = [
        "ref_wp", "ref_kind", "cmp_wp", "cmp_kind",
        "region", "channel", "mass", "signal_mode", "bkg_mode",
        "bin", "bin_low", "bin_high",
        "ref_S", "ref_B", "ref_S_over_sqrtB", "ref_z_simple", "ref_z_asimov", "ref_component_quad_bin_z_simple",
        "cmp_S", "cmp_B", "cmp_S_over_sqrtB", "cmp_z_simple", "cmp_z_asimov", "cmp_component_quad_bin_z_simple",
        "bin_limit_ratio_simple", "bin_sensitivity_loss_abs_simple", "bin_sensitivity_loss_frac_simple",
        "bin_limit_ratio_asimov", "bin_sensitivity_loss_abs_asimov", "bin_sensitivity_loss_frac_asimov",
        "ref_dominant_era_simple", "ref_dominant_era_component_frac_simple",
        "cmp_dominant_era_simple", "cmp_dominant_era_component_frac_simple",
    ]
    for era in args.eras:
        bin_fields.extend([f"ref_z_simple_{era}", f"cmp_z_simple_{era}"])

    sanity_fields = [
        "wp", "region", "channel", "mass", "era", "process", "status",
        "era_integral", "run2_integral", "rel_integral_diff", "max_bin_abs_diff", "max_bin_rel_diff",
    ]

    summary_notes = [
        "# ref_wp is the baseline. pred_limit_ratio ~= Z_ref / Z_cmp. Values > 1 mean cmp_wp is expected to give a weaker limit.",
        "# sensitivity_loss_frac = max(Z_ref - Z_cmp, 0) / Z_ref. All exposed loss/fraction quantities are on Z, not on Z^2.",
        "# S_over_sqrtB is shown as a simple yield-level readability metric. The actual FOM comparison uses Z_simple / Z_asimov.",
        "# component_quad_total_z_* = sqrt(sum_era Z_era^2) for that same WP/case; for run2 inputs this is diagnostic and can exceed the actual run2 Z.",
    ]

    era_notes = [
        "# S_over_sqrtB = S / sqrt(B) using the integrated S and B of that era.",
        "# component_sensitivity_fraction_* = Z_era / sqrt(sum_era Z_era^2) for that same WP/case.",
        "# view_effective_z_* is the actual FOM used for comparison; for run2 inputs it can differ from component_quad_total_z_*.",
    ]

    bin_notes = [
        "# ref_z_simple / cmp_z_simple are the local bin sensitivities used directly for the pairwise comparison.",
        "# bin_sensitivity_loss_frac_simple = max(ref_z_simple - cmp_z_simple, 0) / ref_z_simple.",
        "# component_quad_bin_z_simple = sqrt(sum_era z_era_bin^2) for that same WP; for run2 inputs this shows the within-WP loss from merging eras in that bin.",
        "# The old weighted_mean / weighted_cv output was removed because it was tied to an S/B-based Z^2 decomposition, not a directly reported sensitivity observable.",
    ]

    sanity_notes = [
        "# For each WP base_dir, compares era-file histograms against the copied *_era histograms inside its Run2 file when available.",
        "# data_obs is omitted here because the Run2 file stores it as a merged histogram rather than as per-era copies.",
    ]

    write_pretty_table(
        os.path.join(args.outdir, "run2sum_fom_summary.txt"),
        summary_rows,
        summary_fields,
        notes=summary_notes,
    )
    write_pretty_table(
        os.path.join(args.outdir, "run2sum_era_yields.txt"),
        era_yield_rows,
        era_yield_fields,
        notes=era_notes,
    )
    write_pretty_table(
        os.path.join(args.outdir, "run2sum_bin_breakdown_compact.txt"),
        bin_rows,
        bin_fields,
        notes=bin_notes,
    )
    write_pretty_table(
        os.path.join(args.outdir, "run2sum_process_sanity.txt"),
        sanity_rows,
        sanity_fields,
        notes=sanity_notes,
    )

    print("\n================ Largest predicted limit-ratio rows ================")
    ranked = sorted(
        [r for r in summary_rows if r["bkg_mode"] == "data_obs"],
        key=lambda r: float(r["pred_limit_ratio_simple"]),
        reverse=True,
    )
    header = (
        f"{'ref_wp':<20} {'cmp_wp':<20} {'region':<6} {'ch':<5} {'mass':<7} {'sig':<7} "
        f"{'Z_ref':>10} {'Z_cmp':>10} {'limRatio':>10} {'loss%':>9}"
    )
    print(header)
    print("-" * len(header))
    for r in ranked[: args.top_summary]:
        print(
            f"{str(r['ref_wp']):<20} {str(r['cmp_wp']):<20} "
            f"{str(r['region']):<6} {str(r['channel']):<5} {str(r['mass']):<7} {str(r['signal_mode']):<7} "
            f"{fmt_float(r['ref_z_simple']):>10} "
            f"{fmt_float(r['cmp_z_simple']):>10} "
            f"{fmt_float(r['pred_limit_ratio_simple']):>10} "
            f"{100.0 * float(r['sensitivity_loss_frac_simple']):>8.2f}%"
        )

    print("\n[OUTPUT: pretty TXT]")
    print("  ", os.path.join(args.outdir, "run2sum_fom_summary.txt"))
    print("  ", os.path.join(args.outdir, "run2sum_era_yields.txt"))
    print("  ", os.path.join(args.outdir, "run2sum_bin_breakdown_compact.txt"))
    print("  ", os.path.join(args.outdir, "run2sum_process_sanity.txt"))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
