#!/usr/bin/env python3
"""
Run2 vs Run2Sum histogram/FOM diagnostics.

This script compares the information kept by era-separated templates against
one-category Run2Sum templates.

Main outputs:
  1) summary TSV
     - era-quadrature FOM: sqrt(sum_era sum_bin Z(S_era_bin, B_era_bin)^2)
     - Run2Sum FOM:       sqrt(sum_bin Z(sum_era S_era_bin, sum_era B_era_bin)^2)
     - predicted limit ratio ~= FOM_era_quad / FOM_Run2Sum
  2) era-yield TSV
  3) per-bin breakdown TSV
  4) process-copy sanity TSV

Typical usage:
  python compare_run2sum_fom.py \
    --base-dir /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr \
    --regions sr1 \
    --channels EMu \
    --masses 600 700 800 900 1000 1100 1200 1300 1500 1700 2000 2500 3000 \
    --signal-modes HNL DYVBF SSWW DY VBF \
    --outdir run2sum_fom_EMu_sr1
"""

from __future__ import annotations

import argparse
import csv
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
    #"HNL": ["signalDY", "signalVBF", "signalSSWW"],
    "DYVBF": ["signalDY", "signalVBF"],
    "DY": ["signalDY"],
    "VBF": ["signalVBF"],
    "SSWW": ["signalSSWW"],
    "Weinberg": ["signalWeinberg"],
}

BKG_MODES = ["data_obs", "bkgsum"]


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


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compare era-separated FOM and Run2Sum FOM from LimitInputs ROOT files.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--base-dir",
        required=True,
        help="LimitInputs WP directory containing 2016preVFP/.../ and Run2/.../ card_input.root files.",
    )
    parser.add_argument("--eras", nargs="+", default=DEFAULT_ERAS, help="Source eras.")
    parser.add_argument("--run2-era", default="Run2", help="Run2Sum input folder label under --base-dir.")
    parser.add_argument("--regions", nargs="+", default=["sr1"], help="Regions to inspect, e.g. sr1 sr2 sr3.")
    parser.add_argument("--channels", nargs="+", default=["EMu"], help="Channels to inspect.")
    parser.add_argument(
        "--masses",
        nargs="+",
        default=["600", "700", "800", "900", "1000", "1100", "1200", "1300", "1500", "1700", "2000", "2500", "3000"],
        help="Masses. Use either 800 or M800. Weinberg is also allowed.",
    )
    parser.add_argument(
        "--signal-modes",
        nargs="+",
        default=["HNL", "DYVBF", "SSWW", "DY", "VBF"],
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
        help="Output directory for TSV files.",
    )
    parser.add_argument(
        "--top-bins",
        type=int,
        default=8,
        help="Number of largest-loss bins to print per mass/region/channel/mode.",
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


def normalize_mass(mass: str) -> str:
    mass = str(mass).strip()
    if mass == "Weinberg":
        return mass
    return mass if mass.startswith("M") else "M" + mass


def card_input_path(base_dir: str, era: str, region: str, mass: str, channel: str) -> str:
    return os.path.join(base_dir, era, region, f"{mass}_{channel}_card_input.root")


def clone_hist(hist: object, name: str) -> object:
    out = hist.Clone(name)
    out.SetDirectory(0)
    return out


def read_hist(file_path: str, hist_name: str, quiet: bool = False) -> Optional[object]:
    if not os.path.exists(file_path):
        if not quiet:
            print(f"[MISSING FILE] {file_path}")
        return None

    root_file = ROOT.TFile.Open(file_path, "READ")
    if not root_file or root_file.IsZombie():
        if not quiet:
            print(f"[BAD FILE] {file_path}")
        return None

    hist = root_file.Get(hist_name)
    if not hist:
        if not quiet:
            print(f"[MISSING HIST] {hist_name} in {file_path}")
        root_file.Close()
        return None

    out = clone_hist(hist, f"{hist_name}__clone")
    root_file.Close()
    return out


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
        if abs(ax.GetBinLowEdge(i) - bx.GetBinLowEdge(i)) > 1e-9:
            raise RuntimeError(
                f"Incompatible bin edge for {context}, edge {i}: "
                f"{ax.GetBinLowEdge(i)} vs {bx.GetBinLowEdge(i)}"
            )


def add_hists(hists: Sequence[object], name: str) -> Optional[object]:
    valid = [h for h in hists if h is not None]
    if not valid:
        return None
    out = zero_like(valid[0], name)
    for h in valid:
        assert_compatible_binning(out, h, name)
        out.Add(h)
    return out


def relative_diff(a: float, b: float, eps: float = 1e-12) -> float:
    denom = max(abs(a), eps)
    return (b - a) / denom


def asimov_z2(s: float, b: float, eps: float = 1e-12) -> float:
    s = max(float(s), 0.0)
    b = max(float(b), eps)
    if s <= 0:
        return 0.0
    val = 2.0 * ((s + b) * math.log(1.0 + s / b) - s)
    return max(val, 0.0)


def simple_z2(s: float, b: float, eps: float = 1e-12) -> float:
    s = max(float(s), 0.0)
    b = max(float(b), eps)
    return (s * s) / b


def compute_fom(sig: object, bkg: object, eps: float = 1e-12) -> FOMResult:
    assert_compatible_binning(sig, bkg, "compute_fom")
    z2_a: List[float] = []
    z2_s: List[float] = []
    total_s = 0.0
    total_b = 0.0
    for ibin in range(1, sig.GetNbinsX() + 1):
        s = max(sig.GetBinContent(ibin), 0.0)
        b = max(bkg.GetBinContent(ibin), 0.0)
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


def build_era_signal(base_dir: str, era: str, region: str, mass: str, channel: str, signal_mode: str, quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, era, region, mass, channel)
    hists = []
    missing = []
    for proc in SIGNAL_MODE_MAP[signal_mode]:
        h = read_hist(fpath, proc, quiet=True)
        if h is None:
            missing.append(f"{era}:{proc}")
            continue
        hists.append(h)
    return HistPack(add_hists(hists, f"sig_{era}_{signal_mode}"), missing)


def build_era_bkg(base_dir: str, era: str, region: str, mass: str, channel: str, bkg_mode: str, quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, era, region, mass, channel)
    if bkg_mode == "data_obs":
        h = read_hist(fpath, "data_obs", quiet=quiet)
        return HistPack(h, [] if h is not None else [f"{era}:data_obs"])

    hists = []
    missing = []
    for proc in BACKGROUND_PROCS:
        h = read_hist(fpath, proc, quiet=True)
        if h is None:
            missing.append(f"{era}:{proc}")
            continue
        hists.append(h)
    return HistPack(add_hists(hists, f"bkg_{era}_{bkg_mode}"), missing)


def build_run2_signal(base_dir: str, run2_era: str, eras: Sequence[str], region: str, mass: str, channel: str, signal_mode: str, quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, run2_era, region, mass, channel)
    hists = []
    missing = []
    for era in eras:
        for proc in SIGNAL_MODE_MAP[signal_mode]:
            hist_name = f"{proc}_{era}"
            h = read_hist(fpath, hist_name, quiet=True)
            if h is None:
                missing.append(f"{run2_era}:{hist_name}")
                continue
            hists.append(h)
    return HistPack(add_hists(hists, f"sig_{run2_era}_{signal_mode}"), missing)


def build_run2_bkg(base_dir: str, run2_era: str, eras: Sequence[str], region: str, mass: str, channel: str, bkg_mode: str, quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, run2_era, region, mass, channel)
    if bkg_mode == "data_obs":
        h = read_hist(fpath, "data_obs", quiet=quiet)
        return HistPack(h, [] if h is not None else [f"{run2_era}:data_obs"])

    hists = []
    missing = []
    for era in eras:
        for proc in BACKGROUND_PROCS:
            hist_name = f"{proc}_{era}"
            h = read_hist(fpath, hist_name, quiet=True)
            if h is None:
                missing.append(f"{run2_era}:{hist_name}")
                continue
            hists.append(h)
    return HistPack(add_hists(hists, f"bkg_{run2_era}_{bkg_mode}"), missing)


def sum_era_hists(packs: Sequence[HistPack], name: str) -> Optional[object]:
    return add_hists([p.hist for p in packs if p.hist is not None], name)


def bin_edges(hist: object, ibin: int) -> Tuple[float, float]:
    ax = hist.GetXaxis()
    return ax.GetBinLowEdge(ibin), ax.GetBinUpEdge(ibin)


def weighted_sb_heterogeneity(era_s: Sequence[float], era_b: Sequence[float], eps: float) -> Tuple[float, float, float]:
    """
    Small-signal decomposition:
      sum_e S_e^2/B_e - (sum_e S_e)^2/(sum_e B_e)
      = sum_e B_e * (S_e/B_e - weighted_mean)^2

    Returns:
      loss_z2_simple, weighted_mean_s_over_b, weighted_cv_s_over_b
    """
    bsum = sum(max(b, 0.0) for b in era_b)
    ssum = sum(max(s, 0.0) for s in era_s)
    if bsum <= eps:
        return 0.0, 0.0, 0.0
    mean = ssum / bsum
    loss = 0.0
    for s, b in zip(era_s, era_b):
        b_eff = max(b, 0.0)
        if b_eff <= eps:
            continue
        r = max(s, 0.0) / b_eff
        loss += b_eff * (r - mean) ** 2
    cv = math.sqrt(loss / bsum) / max(abs(mean), eps) if mean > eps else 0.0
    return loss, mean, cv


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
        rel = diff / max(abs(av), 1e-12)
        max_abs = max(max_abs, diff)
        max_rel = max(max_rel, rel)
    return max_abs, max_rel


def check_process_copy_sanity(
    base_dir: str,
    run2_era: str,
    eras: Sequence[str],
    region: str,
    mass: str,
    channel: str,
    quiet: bool,
) -> List[Dict[str, object]]:
    rows: List[Dict[str, object]] = []
    procs = BACKGROUND_PROCS + ["data_obs"] + sorted({p for mode in SIGNAL_MODE_MAP.values() for p in mode})

    for era in eras:
        era_file = card_input_path(base_dir, era, region, mass, channel)
        run2_file = card_input_path(base_dir, run2_era, region, mass, channel)
        for proc in procs:
            h_era = read_hist(era_file, proc, quiet=True)
            if proc == "data_obs":
                # data_obs is summed in the Run2 file, not copied per era.
                continue
            else:
                h_run2 = read_hist(run2_file, f"{proc}_{era}", quiet=True)
            if h_era is None and h_run2 is None:
                continue
            if h_era is None or h_run2 is None:
                rows.append({
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
                "region": region,
                "channel": channel,
                "mass": mass,
                "era": era,
                "process": proc,
                "status": "ok" if abs(relative_diff(era_int, run2_int)) < 1e-9 and max_rel < 1e-9 else "diff",
                "era_integral": era_int,
                "run2_integral": run2_int,
                "rel_integral_diff": relative_diff(era_int, run2_int),
                "max_bin_abs_diff": max_abs,
                "max_bin_rel_diff": max_rel,
            })
    return rows


def write_tsv(path: str, rows: Sequence[Dict[str, object]], fieldnames: Sequence[str]) -> None:
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames, delimiter="\t", extrasaction="ignore")
        writer.writeheader()
        for row in rows:
            writer.writerow(row)

TEXT_FIELDS = {
    "region",
    "channel",
    "mass",
    "signal_mode",
    "bkg_mode",
    "era",
    "process",
    "status",
    "dominant_era_by_z2",
    "missing_signal_hists",
    "missing_bkg_hists",
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

    if field == "bin":
        return f"{int(round(x))}"

    ax = abs(x)

    if ax == 0.0:
        return "0"

    # 너무 크거나 너무 작은 값은 scientific notation
    if ax >= 1.0e4 or ax < 1.0e-3:
        return f"{x:.3e}"

    # 나머지는 적당히 fixed-point
    if ax >= 1.0e3:
        return f"{x:.1f}"
    if ax >= 1.0e2:
        return f"{x:.2f}"
    if ax >= 10.0:
        return f"{x:.3f}"
    return f"{x:.4f}"


def write_pretty_table(path: str,
                       rows: Sequence[Dict[str, object]],
                       fieldnames: Sequence[str]) -> None:
    os.makedirs(os.path.dirname(path), exist_ok=True)

    formatted_rows = []
    widths = {field: len(field) for field in fieldnames}

    for row in rows:
        out_row = {}
        for field in fieldnames:
            cell = format_for_human(row.get(field, ""), field)
            out_row[field] = cell
            widths[field] = max(widths[field], len(cell))
        formatted_rows.append(out_row)

    with open(path, "w") as f:
        # header
        header_cells = []
        for field in fieldnames:
            if field in TEXT_FIELDS:
                header_cells.append(field.ljust(widths[field]))
            else:
                header_cells.append(field.rjust(widths[field]))
        f.write("  ".join(header_cells) + "\n")

        # separator
        rule_cells = ["-" * widths[field] for field in fieldnames]
        f.write("  ".join(rule_cells) + "\n")

        # rows
        for row in formatted_rows:
            cells = []
            for field in fieldnames:
                cell = row[field]
                if field in TEXT_FIELDS:
                    cells.append(cell.ljust(widths[field]))
                else:
                    cells.append(cell.rjust(widths[field]))
            f.write("  ".join(cells) + "\n")

def fmt_float(x: object, digits: int = 4) -> str:
    return format_for_human(x)


def main() -> int:
    args = parse_args()
    base_dir = os.path.abspath(args.base_dir)
    os.makedirs(args.outdir, exist_ok=True)

    masses = [normalize_mass(m) for m in args.masses]

    summary_rows: List[Dict[str, object]] = []
    era_yield_rows: List[Dict[str, object]] = []
    bin_rows: List[Dict[str, object]] = []
    sanity_rows: List[Dict[str, object]] = []

    print("============================================================")
    print("Run2 vs Run2Sum histogram/FOM diagnostics")
    print("------------------------------------------------------------")
    print("base_dir :", base_dir)
    print("eras     :", ", ".join(args.eras))
    print("run2 era :", args.run2_era)
    print("regions  :", ", ".join(args.regions))
    print("channels :", ", ".join(args.channels))
    print("masses   :", ", ".join(masses))
    print("signals  :", ", ".join(args.signal_modes))
    print("bkg modes:", ", ".join(args.bkg_modes))
    print("outdir   :", args.outdir)
    print("============================================================\n")

    for region in args.regions:
        for channel in args.channels:
            for mass in masses:
                run2_file = card_input_path(base_dir, args.run2_era, region, mass, channel)
                if not os.path.exists(run2_file):
                    print(f"[SKIP] Missing Run2 file: {run2_file}")
                    continue

                sanity_rows.extend(
                    check_process_copy_sanity(base_dir, args.run2_era, args.eras, region, mass, channel, args.quiet_missing)
                )

                for signal_mode in args.signal_modes:
                    era_sig_packs = [
                        build_era_signal(base_dir, era, region, mass, channel, signal_mode, args.quiet_missing)
                        for era in args.eras
                    ]
                    run2_sig_pack = build_run2_signal(
                        base_dir, args.run2_era, args.eras, region, mass, channel, signal_mode, args.quiet_missing
                    )
                    era_sig_sum = sum_era_hists(era_sig_packs, f"era_sum_sig_{region}_{channel}_{mass}_{signal_mode}")

                    if run2_sig_pack.hist is None or era_sig_sum is None or era_sig_sum.Integral() <= args.epsilon:
                        continue

                    for bkg_mode in args.bkg_modes:
                        era_bkg_packs = [
                            build_era_bkg(base_dir, era, region, mass, channel, bkg_mode, args.quiet_missing)
                            for era in args.eras
                        ]
                        run2_bkg_pack = build_run2_bkg(
                            base_dir, args.run2_era, args.eras, region, mass, channel, bkg_mode, args.quiet_missing
                        )
                        era_bkg_sum = sum_era_hists(era_bkg_packs, f"era_sum_bkg_{region}_{channel}_{mass}_{bkg_mode}")

                        if run2_bkg_pack.hist is None or era_bkg_sum is None:
                            continue

                        # Era-quadrature FOM.
                        era_foms: Dict[str, FOMResult] = {}
                        era_z2_asimov_by_bin: Optional[List[float]] = None
                        era_z2_simple_by_bin: Optional[List[float]] = None

                        for era, sig_pack, bkg_pack in zip(args.eras, era_sig_packs, era_bkg_packs):
                            if sig_pack.hist is None or bkg_pack.hist is None:
                                continue
                            fom = compute_fom(sig_pack.hist, bkg_pack.hist, args.epsilon)
                            era_foms[era] = fom
                            if era_z2_asimov_by_bin is None:
                                era_z2_asimov_by_bin = [0.0] * len(fom.z2_asimov_by_bin)
                                era_z2_simple_by_bin = [0.0] * len(fom.z2_simple_by_bin)
                            for i, z2 in enumerate(fom.z2_asimov_by_bin):
                                era_z2_asimov_by_bin[i] += z2
                            for i, z2 in enumerate(fom.z2_simple_by_bin):
                                era_z2_simple_by_bin[i] += z2

                        if era_z2_asimov_by_bin is None or era_z2_simple_by_bin is None:
                            continue

                        z_era_quad_asimov = math.sqrt(sum(era_z2_asimov_by_bin))
                        z_era_quad_simple = math.sqrt(sum(era_z2_simple_by_bin))

                        run2_fom = compute_fom(run2_sig_pack.hist, run2_bkg_pack.hist, args.epsilon)
                        merged_from_eras_fom = compute_fom(era_sig_sum, era_bkg_sum, args.epsilon)

                        pred_limit_ratio_asimov = (
                            z_era_quad_asimov / run2_fom.z_asimov
                            if run2_fom.z_asimov > args.epsilon
                            else float("inf")
                        )
                        pred_limit_ratio_simple = (
                            z_era_quad_simple / run2_fom.z_simple
                            if run2_fom.z_simple > args.epsilon
                            else float("inf")
                        )

                        max_s_abs, max_s_rel = max_bin_abs_diff(era_sig_sum, run2_sig_pack.hist)
                        max_b_abs, max_b_rel = max_bin_abs_diff(era_bkg_sum, run2_bkg_pack.hist)

                        simple_loss_total = max(z_era_quad_simple ** 2 - run2_fom.z_simple ** 2, 0.0)
                        simple_loss_frac = simple_loss_total / max(z_era_quad_simple ** 2, args.epsilon)
                        asimov_loss_total = max(z_era_quad_asimov ** 2 - run2_fom.z_asimov ** 2, 0.0)
                        asimov_loss_frac = asimov_loss_total / max(z_era_quad_asimov ** 2, args.epsilon)

                        summary_rows.append({
                            "region": region,
                            "channel": channel,
                            "mass": mass,
                            "signal_mode": signal_mode,
                            "bkg_mode": bkg_mode,
                            "z_era_quad_asimov": z_era_quad_asimov,
                            "z_run2sum_asimov": run2_fom.z_asimov,
                            "z_merged_from_eras_asimov": merged_from_eras_fom.z_asimov,
                            "pred_limit_ratio_asimov": pred_limit_ratio_asimov,
                            "asimov_info_loss_frac": asimov_loss_frac,
                            "z_era_quad_simple": z_era_quad_simple,
                            "z_run2sum_simple": run2_fom.z_simple,
                            "z_merged_from_eras_simple": merged_from_eras_fom.z_simple,
                            "pred_limit_ratio_simple": pred_limit_ratio_simple,
                            "simple_info_loss_frac": simple_loss_frac,
                            "sumS_eras": era_sig_sum.Integral(),
                            "sumS_run2sum": run2_sig_pack.hist.Integral(),
                            "rel_diff_sumS_run2_vs_eras": relative_diff(era_sig_sum.Integral(), run2_sig_pack.hist.Integral(), args.epsilon),
                            "sumB_eras": era_bkg_sum.Integral(),
                            "sumB_run2sum": run2_bkg_pack.hist.Integral(),
                            "rel_diff_sumB_run2_vs_eras": relative_diff(era_bkg_sum.Integral(), run2_bkg_pack.hist.Integral(), args.epsilon),
                            "max_bin_rel_diff_S_run2_vs_eras": max_s_rel,
                            "max_bin_rel_diff_B_run2_vs_eras": max_b_rel,
                            "missing_signal_hists": ",".join(sum((p.missing for p in era_sig_packs), []) + run2_sig_pack.missing),
                            "missing_bkg_hists": ",".join(sum((p.missing for p in era_bkg_packs), []) + run2_bkg_pack.missing),
                        })

                        # Era yield rows.
                        for era in args.eras:
                            fom = era_foms.get(era)
                            if fom is None:
                                continue
                            era_yield_rows.append({
                                "region": region,
                                "channel": channel,
                                "mass": mass,
                                "signal_mode": signal_mode,
                                "bkg_mode": bkg_mode,
                                "era": era,
                                "S": fom.total_s,
                                "B": fom.total_b,
                                "S_over_B": fom.total_s / max(fom.total_b, args.epsilon),
                                "z_asimov": fom.z_asimov,
                                "z_simple": fom.z_simple,
                                "z2_asimov_fraction": (fom.z_asimov ** 2) / max(z_era_quad_asimov ** 2, args.epsilon),
                                "z2_simple_fraction": (fom.z_simple ** 2) / max(z_era_quad_simple ** 2, args.epsilon),
                            })

                        # Per-bin breakdown.
                        nbins = run2_sig_pack.hist.GetNbinsX()
                        for ibin in range(1, nbins + 1):
                            era_s = []
                            era_b = []
                            era_sb = []
                            era_z2a = []
                            era_z2s = []
                            for era, sig_pack, bkg_pack in zip(args.eras, era_sig_packs, era_bkg_packs):
                                if sig_pack.hist is None or bkg_pack.hist is None:
                                    s = 0.0
                                    b = 0.0
                                else:
                                    s = max(sig_pack.hist.GetBinContent(ibin), 0.0)
                                    b = max(bkg_pack.hist.GetBinContent(ibin), 0.0)
                                era_s.append(s)
                                era_b.append(b)
                                era_sb.append(s / max(b, args.epsilon))
                                era_z2a.append(asimov_z2(s, b, args.epsilon))
                                era_z2s.append(simple_z2(s, b, args.epsilon))

                            sum_s = sum(era_s)
                            sum_b = sum(era_b)
                            z2_sep_a = sum(era_z2a)
                            z2_sep_s = sum(era_z2s)
                            z2_sum_a = asimov_z2(sum_s, sum_b, args.epsilon)
                            z2_sum_s = simple_z2(sum_s, sum_b, args.epsilon)
                            loss_simple, weighted_mean_sb, weighted_cv_sb = weighted_sb_heterogeneity(era_s, era_b, args.epsilon)
                            low, high = bin_edges(run2_sig_pack.hist, ibin)

                            row: Dict[str, object] = {
                                "region": region,
                                "channel": channel,
                                "mass": mass,
                                "signal_mode": signal_mode,
                                "bkg_mode": bkg_mode,
                                "bin": ibin,
                                "bin_low": low,
                                "bin_high": high,
                                "sumS": sum_s,
                                "sumB": sum_b,
                                "run2_S_over_B": sum_s / max(sum_b, args.epsilon),
                                "z2_sep_asimov": z2_sep_a,
                                "z2_run2sum_asimov": z2_sum_a,
                                "z2_loss_asimov": max(z2_sep_a - z2_sum_a, 0.0),
                                "z2_loss_asimov_frac_total": max(z2_sep_a - z2_sum_a, 0.0) / max(z_era_quad_asimov ** 2, args.epsilon),
                                "z2_sep_simple": z2_sep_s,
                                "z2_run2sum_simple": z2_sum_s,
                                "z2_loss_simple": max(z2_sep_s - z2_sum_s, 0.0),
                                "z2_loss_simple_exact_heterogeneity": loss_simple,
                                "weighted_mean_S_over_B": weighted_mean_sb,
                                "weighted_cv_S_over_B": weighted_cv_sb,
                                "dominant_era_by_z2": args.eras[max(range(len(args.eras)), key=lambda i: era_z2a[i])] if args.eras else "",
                                "dominant_era_z2_frac": max(era_z2a) / max(z2_sep_a, args.epsilon) if z2_sep_a > args.epsilon else 0.0,
                            }
                            for era, s, b, sb, z2a, z2s in zip(args.eras, era_s, era_b, era_sb, era_z2a, era_z2s):
                                row[f"S_{era}"] = s
                                row[f"B_{era}"] = b
                                row[f"S_over_B_{era}"] = sb
                                row[f"z2_asimov_{era}"] = z2a
                                row[f"z2_simple_{era}"] = z2s
                            bin_rows.append(row)

                        # Print compact per-case result and top bins only for data_obs mode.
                        if bkg_mode == "data_obs":
                            print(
                                f"[{region:<4} {channel:<4} {mass:<7} {signal_mode:<7}] "
                                f"Z_era={fmt_float(z_era_quad_asimov):>10}  "
                                f"Z_Run2Sum={fmt_float(run2_fom.z_asimov):>10}  "
                                f"pred_limit_ratio={fmt_float(pred_limit_ratio_asimov):>10}  "
                                f"loss={100.0 * asimov_loss_frac:>7.2f}%"
                            )

                            related_bins = [
                                r for r in bin_rows
                                if r["region"] == region
                                and r["channel"] == channel
                                and r["mass"] == mass
                                and r["signal_mode"] == signal_mode
                                and r["bkg_mode"] == bkg_mode
                            ]
                            related_bins = sorted(related_bins, key=lambda r: float(r["z2_loss_asimov"]), reverse=True)
                            for r in related_bins[: args.top_bins]:
                                print(
                                    "  bin {bin:>2} [{bin_low:>8}, {bin_high:>8}]  "
                                    "lossZ2={loss:>10}  S/B Run2={sb:>10}  dom={dom:<12} ({domf:.2f})".format(
                                        bin=int(r["bin"]),
                                        bin_low=fmt_float(float(r["bin_low"])),
                                        bin_high=fmt_float(float(r["bin_high"])),
                                        loss=fmt_float(float(r["z2_loss_asimov"])),
                                        sb=fmt_float(float(r["run2_S_over_B"])),
                                        dom=str(r["dominant_era_by_z2"]),
                                        domf=float(r["dominant_era_z2_frac"]),
                                    )
                                )
                            print("")

    # Write outputs.
    summary_fields = [
        "region", "channel", "mass", "signal_mode", "bkg_mode",
        "z_era_quad_asimov", "z_run2sum_asimov", "z_merged_from_eras_asimov", "pred_limit_ratio_asimov", "asimov_info_loss_frac",
        "z_era_quad_simple", "z_run2sum_simple", "z_merged_from_eras_simple", "pred_limit_ratio_simple", "simple_info_loss_frac",
        "sumS_eras", "sumS_run2sum", "rel_diff_sumS_run2_vs_eras",
        "sumB_eras", "sumB_run2sum", "rel_diff_sumB_run2_vs_eras",
        "max_bin_rel_diff_S_run2_vs_eras", "max_bin_rel_diff_B_run2_vs_eras",
        "missing_signal_hists", "missing_bkg_hists",
    ]
    era_yield_fields = [
        "region", "channel", "mass", "signal_mode", "bkg_mode", "era",
        "S", "B", "S_over_B", "z_asimov", "z_simple", "z2_asimov_fraction", "z2_simple_fraction",
    ]
    bin_fields = [
        "region", "channel", "mass", "signal_mode", "bkg_mode", "bin", "bin_low", "bin_high",
        "sumS", "sumB", "run2_S_over_B",
        "z2_sep_asimov", "z2_run2sum_asimov", "z2_loss_asimov", "z2_loss_asimov_frac_total",
        "z2_sep_simple", "z2_run2sum_simple", "z2_loss_simple", "z2_loss_simple_exact_heterogeneity",
        "weighted_mean_S_over_B", "weighted_cv_S_over_B", "dominant_era_by_z2", "dominant_era_z2_frac",
    ]
    for era in args.eras:
        bin_fields.extend([f"S_{era}", f"B_{era}", f"S_over_B_{era}", f"z2_asimov_{era}", f"z2_simple_{era}"])
    sanity_fields = [
        "region", "channel", "mass", "era", "process", "status",
        "era_integral", "run2_integral", "rel_integral_diff", "max_bin_abs_diff", "max_bin_rel_diff",
    ]

    summary_pretty_fields = [
        "region",
        "channel",
        "mass",
        "signal_mode",
        "bkg_mode",
        "z_era_quad_asimov",
        "z_run2sum_asimov",
        "pred_limit_ratio_asimov",
        "asimov_info_loss_frac",
        "z_era_quad_simple",
        "z_run2sum_simple",
        "pred_limit_ratio_simple",
        "simple_info_loss_frac",
        "sumS_eras",
        "sumS_run2sum",
        "sumB_eras",
        "sumB_run2sum",
        "max_bin_rel_diff_S_run2_vs_eras",
        "max_bin_rel_diff_B_run2_vs_eras",
    ]
    
    era_yield_pretty_fields = era_yield_fields[:]
    
    bin_pretty_fields = [
        "region",
        "channel",
        "mass",
        "signal_mode",
        "bkg_mode",
        "bin",
        "bin_low",
        "bin_high",
        "sumS",
        "sumB",
        "run2_S_over_B",
        "z2_sep_asimov",
        "z2_run2sum_asimov",
        "z2_loss_asimov",
        "weighted_mean_S_over_B",
        "weighted_cv_S_over_B",
        "dominant_era_by_z2",
        "dominant_era_z2_frac",
    ]
    
    for era in args.eras:
        bin_pretty_fields.extend([
            f"S_over_B_{era}",
            f"z2_asimov_{era}",
        ])
    
    sanity_pretty_fields = sanity_fields[:]

    write_tsv(os.path.join(args.outdir, "run2sum_fom_summary.tsv"), summary_rows, summary_fields)
    write_tsv(os.path.join(args.outdir, "run2sum_era_yields.tsv"), era_yield_rows, era_yield_fields)
    write_tsv(os.path.join(args.outdir, "run2sum_bin_breakdown.tsv"), bin_rows, bin_fields)
    write_tsv(os.path.join(args.outdir, "run2sum_process_sanity.tsv"), sanity_rows, sanity_fields)

    write_pretty_table(
        os.path.join(args.outdir, "run2sum_fom_summary.txt"),
        summary_rows,
        summary_pretty_fields,
    )
    
    write_pretty_table(
        os.path.join(args.outdir, "run2sum_era_yields.txt"),
        era_yield_rows,
        era_yield_pretty_fields,
    )
    
    write_pretty_table(
        os.path.join(args.outdir, "run2sum_bin_breakdown_compact.txt"),
        bin_rows,
        bin_pretty_fields,
    )
    
    write_pretty_table(
        os.path.join(args.outdir, "run2sum_process_sanity.txt"),
        sanity_rows,
        sanity_pretty_fields,
    )

    print("\n================ Largest predicted limit-ratio rows ================")
    ranked = sorted(
        [r for r in summary_rows if r["bkg_mode"] == "data_obs"],
        key=lambda r: float(r["pred_limit_ratio_asimov"]),
        reverse=True,
    )
    header = f"{'region':<6} {'ch':<5} {'mass':<7} {'sig':<7} {'Z_era':>10} {'Z_R2S':>10} {'limRatio':>10} {'loss%':>9}"
    print(header)
    print("-" * len(header))
    for r in ranked[: args.top_summary]:
        print(
            f"{r['region']:<6} {r['channel']:<5} {r['mass']:<7} {r['signal_mode']:<7} "
            f"{fmt_float(r['z_era_quad_asimov'], 5):>10} "
            f"{fmt_float(r['z_run2sum_asimov'], 5):>10} "
            f"{fmt_float(r['pred_limit_ratio_asimov'], 5):>10} "
            f"{100.0 * float(r['asimov_info_loss_frac']):>8.2f}%"
        )

    print("\n[OUTPUT: raw TSV]")
    print("  ", os.path.join(args.outdir, "run2sum_fom_summary.tsv"))
    print("  ", os.path.join(args.outdir, "run2sum_era_yields.tsv"))
    print("  ", os.path.join(args.outdir, "run2sum_bin_breakdown.tsv"))
    print("  ", os.path.join(args.outdir, "run2sum_process_sanity.tsv"))
    
    print("\n[OUTPUT: pretty TXT]")
    print("  ", os.path.join(args.outdir, "run2sum_fom_summary.txt"))
    print("  ", os.path.join(args.outdir, "run2sum_era_yields.txt"))
    print("  ", os.path.join(args.outdir, "run2sum_bin_breakdown_compact.txt"))
    print("  ", os.path.join(args.outdir, "run2sum_process_sanity.txt"))        

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
