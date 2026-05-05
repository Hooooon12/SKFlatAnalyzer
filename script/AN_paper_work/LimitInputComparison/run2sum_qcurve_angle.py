#!/usr/bin/env python3
"""
Blind Run2 vs Run2Sum diagnostics for full HNL 1D model.

This script reads LimitInputs/<WP>/<era>/<region>/<mass>_<channel>_card_input.root
and computes two compact diagnostics that are useful for understanding why a
Run2Sum limit can differ from the era-separated Run2 simultaneous fit.

Diagnostic 1: fixed-nuisance background-only Asimov q(r) curves
  q_A(r) = sum_bins 2 * [ mu(r) - b - b * ln(mu(r)/b) ]
  with
    mu(r) = b + r * S_lin + c_ch * r^2 * S_quad

  Here
    S_lin  = signalDY + signalVBF
    S_quad = signalSSWW
    c_ch   = 1 for MuMu / EE
             4 for EMu

  The script computes
    - q_sep(r):    using era-separated bins/channels (quadrature over eras)
    - q_run2sum(r): using the one-category Run2Sum projection

Diagnostic 2: background-weighted sensitivity angle
  u_i = S_lin_i / sqrt(B_i)
  v_i = c_ch * S_quad_i / sqrt(B_i)
  cos(theta) = (u dot v) / (|u| |v|)

Outputs are human-oriented:
  - diagnostics_summary.txt
  - qcurve_<region>_<channel>_<mass>.png/.pdf
  - scan_rqref_<region>_<channel>.png/.pdf
  - scan_costheta_<region>_<channel>.png/.pdf

Typical usage:
  python run2sum_qcurve_angle.py \
    --base-dir /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_L2review_HNL_ULIDv2_FixHessian_AddGluGluTaus_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr \
    --regions sr1 \
    --channels EMu \
    --masses 600 700 800 900 1000 1100 1200 1300 1500 1700 2000 2500 3000 \
    --outdir qcurve_angle_EMu_sr1
"""

from __future__ import annotations

import argparse
import math
import os
import sys
from array import array
from dataclasses import dataclass
from typing import Dict, List, Optional, Sequence, Tuple

try:
    import ROOT  # type: ignore
    ROOT.gROOT.SetBatch(True)
    ROOT.gStyle.SetOptStat(0)
except Exception as exc:
    print("[ERROR] Could not import ROOT. Run this inside your ROOT/CMSSW environment.")
    print("        import error:", exc)
    sys.exit(1)


DEFAULT_ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]
DEFAULT_MASSES = ["600", "700", "800", "900", "1000", "1100", "1200", "1300", "1500", "1700", "2000", "2500", "3000"]
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
LIN_PROCS = ["signalDY", "signalVBF"]
QUAD_PROCS = ["signalSSWW"]


@dataclass
class HistPack:
    hist: Optional[object]
    missing: List[str]


@dataclass
class DiagnosticsRow:
    region: str
    channel: str
    mass: str
    mass_value: float
    bkg_mode: str
    coeff_quad: float
    r_qref_sep: Optional[float]
    r_qref_run2sum: Optional[float]
    r_qref_ratio: Optional[float]
    cos_sep: Optional[float]
    cos_run2sum: Optional[float]
    delta_cos: Optional[float]
    quad_over_lin_sep: Optional[float]
    quad_over_lin_run2sum: Optional[float]


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Compute blind q(r) curves and background-weighted DYVBF-vs-SSWW angle for Run2 vs Run2Sum.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument("--base-dir", required=True, help="LimitInputs WP directory.")
    parser.add_argument("--eras", nargs="+", default=DEFAULT_ERAS, help="Source eras.")
    parser.add_argument("--run2-era", default="Run2", help="Run2Sum input folder label under --base-dir.")
    parser.add_argument("--regions", nargs="+", default=["sr1"], help="Regions to inspect.")
    parser.add_argument("--channels", nargs="+", default=["EMu"], help="Channels to inspect.")
    parser.add_argument("--masses", nargs="+", default=DEFAULT_MASSES, help="Masses. Use either 800 or M800.")
    parser.add_argument("--bkg-mode", choices=["bkgsum", "data_obs"], default="bkgsum", help="Background definition.")
    parser.add_argument("--r-min", type=float, default=0.0, help="Minimum r for q(r) scan.")
    parser.add_argument("--r-max", type=float, default=20.0, help="Maximum r for q(r) scan.")
    parser.add_argument("--n-r", type=int, default=401, help="Number of r points in the q(r) scan.")
    parser.add_argument("--q-ref", type=float, default=2.706, help="Reference q value used for r(q_ref) summary.")
    parser.add_argument("--epsilon", type=float, default=1.0e-12, help="Small positive value for denominators.")
    parser.add_argument("--outdir", default="run2sum_qcurve_angle", help="Output directory.")
    parser.add_argument("--quiet-missing", action="store_true", help="Do not print warnings for missing optional histograms.")
    return parser.parse_args()


def normalize_mass(mass: str) -> str:
    mass = str(mass).strip()
    if mass == "Weinberg":
        return mass
    return mass if mass.startswith("M") else "M" + mass


def mass_value(mass: str) -> float:
    if mass == "Weinberg":
        return float("nan")
    return float(mass.replace("M", ""))


def coeff_quad(channel: str) -> float:
    return 4.0 if channel == "EMu" else 1.0


def fmt_num(x: Optional[float], digits: int = 4) -> str:
    if x is None:
        return "n/a"
    try:
        val = float(x)
    except Exception:
        return str(x)
    if math.isnan(val):
        return "nan"
    ax = abs(val)
    if ax == 0.0:
        return "0"
    if ax >= 1.0e4 or ax < 1.0e-3:
        return f"{val:.3e}"
    if ax >= 100.0:
        return f"{val:.2f}"
    if ax >= 10.0:
        return f"{val:.3f}"
    return f"{val:.4f}"


def safe_filename(*parts: str) -> str:
    out = "_".join(parts)
    for bad in ["/", " ", "(", ")", ",", ":"]:
        out = out.replace(bad, "_")
    while "__" in out:
        out = out.replace("__", "_")
    return out.strip("_")


def card_input_path(base_dir: str, era: str, region: str, mass: str, channel: str) -> str:
    return os.path.join(base_dir, era, region, f"{mass}_{channel}_card_input.root")


def clone_hist(hist: object, name: str) -> object:
    out = hist.Clone(name)
    out.SetDirectory(0)
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


def add_hists(hists: Sequence[object], name: str) -> Optional[object]:
    valid = [h for h in hists if h is not None]
    if not valid:
        return None
    out = zero_like(valid[0], name)
    for h in valid:
        assert_compatible_binning(out, h, name)
        out.Add(h)
    return out


def build_component_hist(base_dir: str,
                         era: str,
                         region: str,
                         mass: str,
                         channel: str,
                         procs: Sequence[str],
                         quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, era, region, mass, channel)
    hists = []
    missing: List[str] = []
    for proc in procs:
        h = read_hist(fpath, proc, quiet=True)
        if h is None:
            missing.append(f"{era}:{proc}")
            continue
        hists.append(h)
    return HistPack(add_hists(hists, f"{era}_{region}_{channel}_{mass}_{'_'.join(procs)}"), missing)


def build_run2_component_hist(base_dir: str,
                              run2_era: str,
                              eras: Sequence[str],
                              region: str,
                              mass: str,
                              channel: str,
                              procs: Sequence[str],
                              quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, run2_era, region, mass, channel)
    hists = []
    missing: List[str] = []
    for era in eras:
        for proc in procs:
            hist_name = f"{proc}_{era}"
            h = read_hist(fpath, hist_name, quiet=True)
            if h is None:
                missing.append(f"{run2_era}:{hist_name}")
                continue
            hists.append(h)
    return HistPack(add_hists(hists, f"{run2_era}_{region}_{channel}_{mass}_{'_'.join(procs)}"), missing)


def build_background_hist(base_dir: str,
                          era: str,
                          region: str,
                          mass: str,
                          channel: str,
                          bkg_mode: str,
                          quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, era, region, mass, channel)
    if bkg_mode == "data_obs":
        h = read_hist(fpath, "data_obs", quiet=quiet)
        return HistPack(h, [] if h is not None else [f"{era}:data_obs"])

    hists = []
    missing: List[str] = []
    for proc in BACKGROUND_PROCS:
        h = read_hist(fpath, proc, quiet=True)
        if h is None:
            missing.append(f"{era}:{proc}")
            continue
        hists.append(h)
    return HistPack(add_hists(hists, f"bkg_{era}_{region}_{channel}_{mass}_{bkg_mode}"), missing)


def build_run2_background_hist(base_dir: str,
                               run2_era: str,
                               eras: Sequence[str],
                               region: str,
                               mass: str,
                               channel: str,
                               bkg_mode: str,
                               quiet: bool) -> HistPack:
    fpath = card_input_path(base_dir, run2_era, region, mass, channel)
    if bkg_mode == "data_obs":
        h = read_hist(fpath, "data_obs", quiet=quiet)
        return HistPack(h, [] if h is not None else [f"{run2_era}:data_obs"])

    hists = []
    missing: List[str] = []
    for era in eras:
        for proc in BACKGROUND_PROCS:
            hist_name = f"{proc}_{era}"
            h = read_hist(fpath, hist_name, quiet=True)
            if h is None:
                missing.append(f"{run2_era}:{hist_name}")
                continue
            hists.append(h)
    return HistPack(add_hists(hists, f"bkg_{run2_era}_{region}_{channel}_{mass}_{bkg_mode}"), missing)


def ensure_hist_or_zero(ref: object, hist: Optional[object], name: str) -> object:
    if hist is None:
        return zero_like(ref, name)
    assert_compatible_binning(ref, hist, name)
    return hist


def build_r_grid(rmin: float, rmax: float, n: int) -> List[float]:
    if n < 2:
        return [rmin, rmax]
    step = (rmax - rmin) / float(n - 1)
    return [rmin + i * step for i in range(n)]


def qA_bin_exclusion(mu: float, b: float, eps: float) -> float:
    mu = max(float(mu), 0.0)
    b = max(float(b), 0.0)
    if mu <= eps:
        return 0.0
    if b <= eps:
        return 2.0 * mu
    val = 2.0 * (mu - b - b * math.log(mu / b))
    return max(val, 0.0)


def qcurve_single_hist(sig_lin: object,
                       sig_quad: object,
                       bkg: object,
                       r_values: Sequence[float],
                       quad_coeff: float,
                       eps: float) -> List[float]:
    assert_compatible_binning(sig_lin, bkg, "qcurve_single_hist:lin")
    assert_compatible_binning(sig_quad, bkg, "qcurve_single_hist:quad")

    qvals: List[float] = []
    nbins = bkg.GetNbinsX()
    for r in r_values:
        qtot = 0.0
        for ibin in range(1, nbins + 1):
            b = max(bkg.GetBinContent(ibin), 0.0)
            s_lin = max(sig_lin.GetBinContent(ibin), 0.0)
            s_quad = max(sig_quad.GetBinContent(ibin), 0.0)
            mu = b + r * s_lin + quad_coeff * (r * r) * s_quad
            qtot += qA_bin_exclusion(mu, b, eps)
        qvals.append(qtot)
    return qvals


def qcurve_separated(era_triples: Sequence[Tuple[object, object, object]],
                     r_values: Sequence[float],
                     quad_coeff: float,
                     eps: float) -> List[float]:
    qsum = [0.0 for _ in r_values]
    for sig_lin, sig_quad, bkg in era_triples:
        q = qcurve_single_hist(sig_lin, sig_quad, bkg, r_values, quad_coeff, eps)
        for i, val in enumerate(q):
            qsum[i] += val
    return qsum


def interpolate_r_at_qref(r_values: Sequence[float], q_values: Sequence[float], q_ref: float) -> Optional[float]:
    if not r_values or not q_values or len(r_values) != len(q_values):
        return None
    if q_values[0] >= q_ref:
        return r_values[0]
    for i in range(1, len(r_values)):
        q0 = q_values[i - 1]
        q1 = q_values[i]
        if q0 <= q_ref <= q1:
            r0 = r_values[i - 1]
            r1 = r_values[i]
            if abs(q1 - q0) < 1e-12:
                return 0.5 * (r0 + r1)
            frac = (q_ref - q0) / (q1 - q0)
            return r0 + frac * (r1 - r0)
    return None


def weighted_vectors_from_hists(sig_lin: object,
                                sig_quad: object,
                                bkg: object,
                                quad_coeff: float,
                                eps: float) -> Tuple[List[float], List[float]]:
    assert_compatible_binning(sig_lin, bkg, "weighted_vectors_from_hists:lin")
    assert_compatible_binning(sig_quad, bkg, "weighted_vectors_from_hists:quad")

    u: List[float] = []
    v: List[float] = []
    nbins = bkg.GetNbinsX()
    for ibin in range(1, nbins + 1):
        b = max(bkg.GetBinContent(ibin), eps)
        rootb = math.sqrt(b)
        s_lin = max(sig_lin.GetBinContent(ibin), 0.0)
        s_quad = max(sig_quad.GetBinContent(ibin), 0.0)
        u.append(s_lin / rootb)
        v.append((quad_coeff * s_quad) / rootb)
    return u, v


def vector_dot(a: Sequence[float], b: Sequence[float]) -> float:
    return sum(x * y for x, y in zip(a, b))


def vector_norm(a: Sequence[float]) -> float:
    return math.sqrt(sum(x * x for x in a))


def cos_theta(u: Sequence[float], v: Sequence[float], eps: float) -> Optional[float]:
    nu = vector_norm(u)
    nv = vector_norm(v)
    if nu <= eps or nv <= eps:
        return None
    c = vector_dot(u, v) / (nu * nv)
    if c > 1.0:
        c = 1.0
    if c < -1.0:
        c = -1.0
    return c


def quad_over_lin_norm(u: Sequence[float], v: Sequence[float], eps: float) -> Optional[float]:
    nu = vector_norm(u)
    nv = vector_norm(v)
    if nu <= eps:
        return None
    return nv / nu


def save_qcurve_plot(outbase: str,
                     region: str,
                     channel: str,
                     mass: str,
                     bkg_mode: str,
                     q_ref: float,
                     r_values: Sequence[float],
                     q_sep: Sequence[float],
                     q_run2sum: Sequence[float],
                     row: DiagnosticsRow) -> None:
    if not r_values:
        return

    xs = array('d', [float(x) for x in r_values])
    ys_sep = array('d', [float(y) for y in q_sep])
    ys_sum = array('d', [float(y) for y in q_run2sum])

    gr_sep = ROOT.TGraph(len(r_values), xs, ys_sep)
    gr_sep.SetLineColor(ROOT.kBlue + 1)
    gr_sep.SetLineWidth(3)

    gr_sum = ROOT.TGraph(len(r_values), xs, ys_sum)
    gr_sum.SetLineColor(ROOT.kRed + 1)
    gr_sum.SetLineWidth(3)

    ymax = max(max(q_sep), max(q_run2sum), q_ref)
    ymax *= 1.18 if ymax > 0 else 1.0

    objtag = safe_filename(region, channel, mass, bkg_mode)
    c = ROOT.TCanvas(f"c_q_{objtag}", f"c_q_{objtag}", 900, 700)
    frame = ROOT.TH1F(f"frame_q_{objtag}", "", 1, float(r_values[0]), float(r_values[-1]))
    frame.SetMinimum(0.0)
    frame.SetMaximum(ymax)
    frame.GetXaxis().SetTitle("r")
    frame.GetYaxis().SetTitle("fixed-nuisance Asimov q_{A}(r)")
    frame.Draw()

    line = ROOT.TLine(float(r_values[0]), q_ref, float(r_values[-1]), q_ref)
    line.SetLineStyle(7)
    line.SetLineColor(ROOT.kGray + 2)
    line.SetLineWidth(2)

    gr_sep.Draw("L SAME")
    gr_sum.Draw("L SAME")
    line.Draw("SAME")

    leg = ROOT.TLegend(0.56, 0.72, 0.88, 0.88)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.AddEntry(gr_sep, "Run2 separate", "l")
    leg.AddEntry(gr_sum, "Run2Sum", "l")
    leg.AddEntry(line, f"q_ref = {q_ref:.3f}", "l")
    leg.Draw()

    txt = ROOT.TPaveText(0.14, 0.62, 0.50, 0.88, "NDC")
    txt.SetBorderSize(0)
    txt.SetFillStyle(0)
    txt.SetTextAlign(12)
    txt.AddText(f"{region}, {channel}, {mass} ({bkg_mode})")
    txt.AddText(f"SSWW coefficient = {fmt_num(row.coeff_quad)}")
    txt.AddText(f"r@q_ref (sep)     = {fmt_num(row.r_qref_sep)}")
    txt.AddText(f"r@q_ref (Run2Sum) = {fmt_num(row.r_qref_run2sum)}")
    txt.AddText(f"Run2Sum / sep     = {fmt_num(row.r_qref_ratio)}")
    txt.AddText(f"cos#theta (sep)   = {fmt_num(row.cos_sep)}")
    txt.AddText(f"cos#theta (sum)   = {fmt_num(row.cos_run2sum)}")
    txt.Draw()

    c.SaveAs(outbase + ".png")
    c.SaveAs(outbase + ".pdf")
    c.Close()


def save_scan_plot(outbase: str,
                   title: str,
                   ylabel: str,
                   masses: Sequence[float],
                   y_sep: Sequence[Optional[float]],
                   y_sum: Sequence[Optional[float]],
                   y_min: Optional[float] = None,
                   y_max: Optional[float] = None) -> None:
    pts_sep = [(m, y) for m, y in zip(masses, y_sep) if y is not None and not math.isnan(float(y))]
    pts_sum = [(m, y) for m, y in zip(masses, y_sum) if y is not None and not math.isnan(float(y))]
    if not pts_sep and not pts_sum:
        return

    all_m = [p[0] for p in pts_sep + pts_sum]
    xmin = min(all_m)
    xmax = max(all_m)
    if xmin == xmax:
        xmin -= 1.0
        xmax += 1.0

    all_y = [float(p[1]) for p in pts_sep + pts_sum]
    ymin_auto = min(all_y)
    ymax_auto = max(all_y)
    if y_min is None:
        y_min = ymin_auto - 0.08 * max(abs(ymax_auto - ymin_auto), 1.0)
    if y_max is None:
        y_max = ymax_auto + 0.15 * max(abs(ymax_auto - ymin_auto), 1.0)
    if abs(y_max - y_min) < 1e-9:
        y_min -= 1.0
        y_max += 1.0

    objtag = safe_filename(title, ylabel)
    c = ROOT.TCanvas(f"c_scan_{objtag}", f"c_scan_{objtag}", 900, 700)
    frame = ROOT.TH1F(f"frame_scan_{objtag}", "", 1, xmin, xmax)
    frame.SetMinimum(y_min)
    frame.SetMaximum(y_max)
    frame.GetXaxis().SetTitle("mass")
    frame.GetYaxis().SetTitle(ylabel)
    frame.Draw()

    leg = ROOT.TLegend(0.58, 0.76, 0.88, 0.88)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)

    if pts_sep:
        xs_sep = array('d', [p[0] for p in pts_sep])
        ys_sep = array('d', [float(p[1]) for p in pts_sep])
        gr_sep = ROOT.TGraph(len(pts_sep), xs_sep, ys_sep)
        gr_sep.SetLineColor(ROOT.kBlue + 1)
        gr_sep.SetMarkerColor(ROOT.kBlue + 1)
        gr_sep.SetMarkerStyle(20)
        gr_sep.SetLineWidth(3)
        gr_sep.Draw("LP SAME")
        leg.AddEntry(gr_sep, "Run2 separate", "lp")

    if pts_sum:
        xs_sum = array('d', [p[0] for p in pts_sum])
        ys_sum = array('d', [float(p[1]) for p in pts_sum])
        gr_sum = ROOT.TGraph(len(pts_sum), xs_sum, ys_sum)
        gr_sum.SetLineColor(ROOT.kRed + 1)
        gr_sum.SetMarkerColor(ROOT.kRed + 1)
        gr_sum.SetMarkerStyle(21)
        gr_sum.SetLineWidth(3)
        gr_sum.Draw("LP SAME")
        leg.AddEntry(gr_sum, "Run2Sum", "lp")

    leg.Draw()

    txt = ROOT.TPaveText(0.15, 0.90, 0.90, 0.96, "NDC")
    txt.SetBorderSize(0)
    txt.SetFillStyle(0)
    txt.SetTextAlign(12)
    txt.AddText(title)
    txt.Draw()

    c.SaveAs(outbase + ".png")
    c.SaveAs(outbase + ".pdf")
    c.Close()


def write_summary(path: str,
                  rows: Sequence[DiagnosticsRow],
                  q_ref: float,
                  r_max: float) -> None:
    os.makedirs(os.path.dirname(path), exist_ok=True)

    by_group: Dict[Tuple[str, str], List[DiagnosticsRow]] = {}
    for row in rows:
        by_group.setdefault((row.region, row.channel), []).append(row)

    with open(path, "w") as f:
        f.write("===============================================================\n")
        f.write("Run2 vs Run2Sum blind diagnostics for full HNL 1D model\n")
        f.write("---------------------------------------------------------------\n")
        f.write(f"q_ref = {q_ref:.3f}\n")
        f.write(f"If r@q_ref is 'n/a', the curve did not cross q_ref before r_max = {fmt_num(r_max)}.\n")
        f.write("SSWW coefficient: MuMu/EE -> 1, EMu -> 4.\n")
        f.write("cos(theta) uses background-weighted vectors u = DYVBF/sqrt(B), v = c*SSWW/sqrt(B).\n")
        f.write("===============================================================\n\n")

        for (region, channel), group_rows in sorted(by_group.items()):
            group_rows = sorted(group_rows, key=lambda r: r.mass_value)
            f.write(f"[{region} / {channel}]\n")
            header = (
                f"{'mass':<8} "
                f"{'r@q_sep':>11} "
                f"{'r@q_sum':>11} "
                f"{'sum/sep':>10} "
                f"{'cos_sep':>10} "
                f"{'cos_sum':>10} "
                f"{'dcos':>10} "
                f"{'|quad|/|lin| sep':>17} "
                f"{'|quad|/|lin| sum':>17}\n"
            )
            f.write(header)
            f.write("-" * (len(header) - 1) + "\n")
            for row in group_rows:
                f.write(
                    f"{row.mass:<8} "
                    f"{fmt_num(row.r_qref_sep):>11} "
                    f"{fmt_num(row.r_qref_run2sum):>11} "
                    f"{fmt_num(row.r_qref_ratio):>10} "
                    f"{fmt_num(row.cos_sep):>10} "
                    f"{fmt_num(row.cos_run2sum):>10} "
                    f"{fmt_num(row.delta_cos):>10} "
                    f"{fmt_num(row.quad_over_lin_sep):>17} "
                    f"{fmt_num(row.quad_over_lin_run2sum):>17}\n"
                )
            f.write("\n")


def main() -> int:
    args = parse_args()
    base_dir = os.path.abspath(args.base_dir)
    os.makedirs(args.outdir, exist_ok=True)

    masses = [normalize_mass(m) for m in args.masses]
    r_values = build_r_grid(args.r_min, args.r_max, args.n_r)

    print("============================================================")
    print("Run2 vs Run2Sum q(r) / cos(theta) diagnostics")
    print("------------------------------------------------------------")
    print("base_dir :", base_dir)
    print("eras     :", ", ".join(args.eras))
    print("run2 era :", args.run2_era)
    print("regions  :", ", ".join(args.regions))
    print("channels :", ", ".join(args.channels))
    print("masses   :", ", ".join(masses))
    print("bkg mode :", args.bkg_mode)
    print("r range  :", f"[{args.r_min}, {args.r_max}] with {args.n_r} points")
    print("q_ref    :", args.q_ref)
    print("outdir   :", args.outdir)
    print("============================================================\n")

    rows: List[DiagnosticsRow] = []

    for region in args.regions:
        for channel in args.channels:
            cquad = coeff_quad(channel)

            for mass in masses:
                if mass == "Weinberg":
                    print(f"[SKIP] {region} {channel} {mass}: this script is for DYVBF + SSWW full HNL diagnostics.")
                    continue

                run2_file = card_input_path(base_dir, args.run2_era, region, mass, channel)
                if not os.path.exists(run2_file):
                    print(f"[SKIP] Missing Run2 file: {run2_file}")
                    continue

                # Build Run2Sum components.
                run2_bkg_pack = build_run2_background_hist(base_dir, args.run2_era, args.eras, region, mass, channel, args.bkg_mode, args.quiet_missing)
                if run2_bkg_pack.hist is None:
                    print(f"[SKIP] No Run2 background for {region} {channel} {mass}")
                    continue

                run2_lin_pack = build_run2_component_hist(base_dir, args.run2_era, args.eras, region, mass, channel, LIN_PROCS, args.quiet_missing)
                run2_quad_pack = build_run2_component_hist(base_dir, args.run2_era, args.eras, region, mass, channel, QUAD_PROCS, args.quiet_missing)

                ref_run2 = run2_bkg_pack.hist
                run2_lin = ensure_hist_or_zero(ref_run2, run2_lin_pack.hist, f"run2_lin_{region}_{channel}_{mass}")
                run2_quad = ensure_hist_or_zero(ref_run2, run2_quad_pack.hist, f"run2_quad_{region}_{channel}_{mass}")

                # Build era-separated components.
                era_triples: List[Tuple[object, object, object]] = []
                u_sep_all: List[float] = []
                v_sep_all: List[float] = []

                for era in args.eras:
                    era_bkg_pack = build_background_hist(base_dir, era, region, mass, channel, args.bkg_mode, args.quiet_missing)
                    if era_bkg_pack.hist is None:
                        continue
                    era_lin_pack = build_component_hist(base_dir, era, region, mass, channel, LIN_PROCS, args.quiet_missing)
                    era_quad_pack = build_component_hist(base_dir, era, region, mass, channel, QUAD_PROCS, args.quiet_missing)

                    ref_era = era_bkg_pack.hist
                    era_lin = ensure_hist_or_zero(ref_era, era_lin_pack.hist, f"{era}_lin_{region}_{channel}_{mass}")
                    era_quad = ensure_hist_or_zero(ref_era, era_quad_pack.hist, f"{era}_quad_{region}_{channel}_{mass}")

                    era_triples.append((era_lin, era_quad, era_bkg_pack.hist))
                    u_part, v_part = weighted_vectors_from_hists(era_lin, era_quad, era_bkg_pack.hist, cquad, args.epsilon)
                    u_sep_all.extend(u_part)
                    v_sep_all.extend(v_part)

                if not era_triples:
                    print(f"[SKIP] No era-separated background for {region} {channel} {mass}")
                    continue

                q_sep = qcurve_separated(era_triples, r_values, cquad, args.epsilon)
                q_run2sum = qcurve_single_hist(run2_lin, run2_quad, run2_bkg_pack.hist, r_values, cquad, args.epsilon)

                r_qref_sep = interpolate_r_at_qref(r_values, q_sep, args.q_ref)
                r_qref_sum = interpolate_r_at_qref(r_values, q_run2sum, args.q_ref)
                r_qref_ratio = None
                if r_qref_sep is not None and r_qref_sum is not None and abs(r_qref_sep) > args.epsilon:
                    r_qref_ratio = r_qref_sum / r_qref_sep

                u_sum, v_sum = weighted_vectors_from_hists(run2_lin, run2_quad, run2_bkg_pack.hist, cquad, args.epsilon)
                cos_sep = cos_theta(u_sep_all, v_sep_all, args.epsilon)
                cos_sum = cos_theta(u_sum, v_sum, args.epsilon)
                delta_cos = None
                if cos_sep is not None and cos_sum is not None:
                    delta_cos = cos_sum - cos_sep

                quad_over_lin_sep = quad_over_lin_norm(u_sep_all, v_sep_all, args.epsilon)
                quad_over_lin_sum = quad_over_lin_norm(u_sum, v_sum, args.epsilon)

                row = DiagnosticsRow(
                    region=region,
                    channel=channel,
                    mass=mass,
                    mass_value=mass_value(mass),
                    bkg_mode=args.bkg_mode,
                    coeff_quad=cquad,
                    r_qref_sep=r_qref_sep,
                    r_qref_run2sum=r_qref_sum,
                    r_qref_ratio=r_qref_ratio,
                    cos_sep=cos_sep,
                    cos_run2sum=cos_sum,
                    delta_cos=delta_cos,
                    quad_over_lin_sep=quad_over_lin_sep,
                    quad_over_lin_run2sum=quad_over_lin_sum,
                )
                rows.append(row)

                print(
                    f"[{region:<4} {channel:<4} {mass:<7}] "
                    f"r@q_ref sep={fmt_num(r_qref_sep):>10}  "
                    f"sum={fmt_num(r_qref_sum):>10}  "
                    f"ratio={fmt_num(r_qref_ratio):>10}  "
                    f"cos_sep={fmt_num(cos_sep):>10}  "
                    f"cos_sum={fmt_num(cos_sum):>10}"
                )

                outbase = os.path.join(args.outdir, safe_filename("qcurve", region, channel, mass))
                save_qcurve_plot(outbase, region, channel, mass, args.bkg_mode, args.q_ref, r_values, q_sep, q_run2sum, row)

    # Write summary text.
    summary_path = os.path.join(args.outdir, "diagnostics_summary.txt")
    write_summary(summary_path, rows, args.q_ref, args.r_max)

    # Region/channel scan plots.
    for region in args.regions:
        for channel in args.channels:
            group = [r for r in rows if r.region == region and r.channel == channel and not math.isnan(r.mass_value)]
            if not group:
                continue
            group = sorted(group, key=lambda r: r.mass_value)
            x = [r.mass_value for r in group]

            outbase_r = os.path.join(args.outdir, safe_filename("scan_rqref", region, channel))
            save_scan_plot(
                outbase_r,
                f"{region}, {channel}: r at q_ref = {args.q_ref:.3f}",
                "r at q_ref",
                x,
                [r.r_qref_sep for r in group],
                [r.r_qref_run2sum for r in group],
            )

            outbase_cos = os.path.join(args.outdir, safe_filename("scan_costheta", region, channel))
            save_scan_plot(
                outbase_cos,
                f"{region}, {channel}: background-weighted cos(theta)",
                "cos(theta)",
                x,
                [r.cos_sep for r in group],
                [r.cos_run2sum for r in group],
                y_min=-1.05,
                y_max=1.05,
            )

    # README / quick guide.
    readme_path = os.path.join(args.outdir, "README.txt")
    with open(readme_path, "w") as f:
        f.write("Run2 vs Run2Sum blind diagnostics\n")
        f.write("================================\n\n")
        f.write("Main files:\n")
        f.write("  diagnostics_summary.txt\n")
        f.write("    aligned summary table with r@q_ref and cos(theta)\n\n")
        f.write("  qcurve_<region>_<channel>_<mass>.png/.pdf\n")
        f.write("    fixed-nuisance Asimov q(r) curves\n\n")
        f.write("  scan_rqref_<region>_<channel>.png/.pdf\n")
        f.write("    approximate sensitivity scan using r where q(r) crosses q_ref\n\n")
        f.write("  scan_costheta_<region>_<channel>.png/.pdf\n")
        f.write("    background-weighted angle scan between DYVBF and SSWW\n\n")
        f.write(f"q_ref = {args.q_ref:.3f}\n")
        f.write(f"r range = [{args.r_min}, {args.r_max}] with {args.n_r} points\n")
        f.write("SSWW coefficient: MuMu/EE -> 1, EMu -> 4\n")

    print("\n[OUTPUT]")
    print("  ", summary_path)
    print("  ", readme_path)
    print("  ", args.outdir)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
