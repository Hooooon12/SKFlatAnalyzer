#!/usr/bin/env python3
# -*- coding: utf-8 -*-

"""
Run scan_combine_input_hists.py over many Combine input ROOT files,
then rank the riskiest process/syst/bin entries and write a human-readable
recommendation report.

This wrapper intentionally keeps outputs as aligned text files, not CSV.

Typical usage:
  python3 scan_all_combine_risks.py \
    "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_.../Run2/sr2/*_card_input.root" \
    --scanner ./scan_combine_input_hists.py \
    --out-dir scan_campaign_sr2 \
    --add-total-bkg \
    --check-stored-total-bkg \
    --top-n 50 \
    --make-top-full 20

For all individual MC diagnostic processes too, do NOT pass --physics-processes.

For datacard-level processes only:
  add --physics-processes
"""

import argparse
import glob
import math
import os
import re
import shlex
import subprocess
import sys
from collections import defaultdict, OrderedDict


ERAS = ("2016preVFP", "2016postVFP", "2017", "2018")

SEVERITY_WEIGHT = {
  "fatal": 1000000.0,
  "risk": 10000.0,
  "warning": 1000.0,
  "info": 10.0,
  "ok": 0.0,
}

FLAG_WEIGHT = {
  # fatal / structural
  "NONFINITE_NOM": 500000.0,
  "NONFINITE_UP": 500000.0,
  "NONFINITE_DOWN": 500000.0,
  "NEG_NOM": 300000.0,
  "NEG_UP": 250000.0,
  "NEG_DOWN": 250000.0,
  "MISSING_NOMINAL": 250000.0,
  "MISS_UP": 150000.0,
  "MISS_DOWN": 150000.0,
  "UP_NBINS_MISMATCH": 200000.0,
  "DOWN_NBINS_MISMATCH": 200000.0,
  "UP_BIN_EDGE_MISMATCH": 200000.0,
  "DOWN_BIN_EDGE_MISMATCH": 200000.0,
  "TOTAL_BKG_COMPONENT_BINNING_MISMATCH": 200000.0,
  "STORED_TOTAL_MISMATCH": 100000.0,

  # fit-risk
  "ONLY_UP_SURVIVES": 50000.0,
  "ONLY_DOWN_SURVIVES": 50000.0,
  "UP_KILLS_NOM": 45000.0,
  "DOWN_KILLS_NOM": 45000.0,
  "ZERO_NOM_NONZERO_VAR": 35000.0,
  "HUGE_ABS_UP_AT_ZERO_NOM": 30000.0,
  "HUGE_ABS_DOWN_AT_ZERO_NOM": 30000.0,
  "LARGE_TOTAL_IMPACT_UP": 30000.0,
  "LARGE_TOTAL_IMPACT_DOWN": 30000.0,
  "DATA_PULL_HIGH": 25000.0,
  "BOTH_ABOVE_NOM": 12000.0,
  "BOTH_BELOW_NOM": 12000.0,
  "ASYM_LARGE": 6000.0,
  "LOCAL_SPIKE_UP": 5000.0,
  "LOCAL_SPIKE_DOWN": 5000.0,

  # stat / warning
  "HUGE_REL_UP": 3000.0,
  "HUGE_REL_DOWN": 3000.0,
  "ZERO_CONTENT_NONZERO_ERR_NOM": 8000.0,
  "ZERO_CONTENT_NONZERO_ERR_UP": 8000.0,
  "ZERO_CONTENT_NONZERO_ERR_DOWN": 8000.0,
  "ZERO_ERR_NONZERO_NOM": 5000.0,
  "ZERO_ERR_NONZERO_UP": 5000.0,
  "ZERO_ERR_NONZERO_DOWN": 5000.0,
  "LOW_NEFF_NOM": 1000.0,
  "LOW_NEFF_UP": 1000.0,
  "LOW_NEFF_DOWN": 1000.0,
  "HIGH_RELSTAT_NOM": 1000.0,
  "HIGH_RELSTAT_UP": 1000.0,
  "HIGH_RELSTAT_DOWN": 1000.0,

  # info, useful for explanation but low ranking weight
  "UP_MOVES_TOWARD_DATA": 100.0,
  "DOWN_MOVES_TOWARD_DATA": 100.0,
  "DATA_OUTSIDE_SYST_ENVELOPE": 100.0,
  "ONE_SIDED_EFFECT": 100.0,
  "NO_EFFECT": 0.0,
}


def shell_quote_list(cmd):
  return " ".join(shlex.quote(str(x)) for x in cmd)


def ensure_dir(path):
  os.makedirs(path, exist_ok=True)
  return path


def safe_float(x, default=None):
  if x is None:
    return default
  try:
    if str(x) in ("NA", "-", "0/0", "+inf", "-inf", "inf", "nan"):
      return default
    val = float(x)
    if math.isnan(val) or math.isinf(val):
      return default
    return val
  except Exception:
    return default


def split_flags(s):
  if not s or s == "-":
    return []
  out = []
  for part in str(s).split(","):
    part = part.strip()
    if not part or part.startswith("+") and part.endswith("more"):
      continue
    out.append(part)
  return out


def expand_inputs(patterns):
  out = []
  for pat in patterns:
    if os.path.isdir(pat):
      matches = glob.glob(os.path.join(pat, "**", "*_card_input.root"), recursive=True)
    else:
      matches = glob.glob(pat)
      if not matches and os.path.exists(pat):
        matches = [pat]

    for m in matches:
      if m.endswith(".root") and os.path.exists(m):
        out.append(os.path.abspath(m))

  # preserve order, remove duplicates
  dedup = []
  for x in sorted(out):
    if x not in dedup:
      dedup.append(x)
  return dedup


def make_report_name(root_path):
  base = os.path.basename(root_path)
  if base.endswith(".root"):
    base = base[:-5]

  # Include nearby path context: .../Run2/sr2/M1000_MuMu_card_input.root
  parts = os.path.normpath(root_path).split(os.sep)
  context = []
  for p in parts[-4:-1]:
    if p:
      context.append(p)

  stem = "__".join(context + [base])
  stem = re.sub(r"[^A-Za-z0-9_.+-]+", "_", stem)
  return stem + "__scan.txt"


def run_command(cmd, log_handle=None):
  if log_handle:
    log_handle.write(shell_quote_list(cmd) + "\n")
    log_handle.flush()

  proc = subprocess.run(
    cmd,
    stdout=subprocess.PIPE,
    stderr=subprocess.STDOUT,
    text=True
  )

  if log_handle:
    log_handle.write(proc.stdout)
    if not proc.stdout.endswith("\n"):
      log_handle.write("\n")
    log_handle.write("[exit code] {}\n\n".format(proc.returncode))
    log_handle.flush()

  return proc.returncode, proc.stdout


def run_scanner_for_file(root_file, report_path, args, log_handle):
  cmd = [
    args.python,
    args.scanner,
    root_file,
    "--out", report_path,
    "--focus-bin", str(args.focus_bin),
    "--min-severity", args.min_severity,
    "--rel-threshold", str(args.rel_threshold),
    "--abs-threshold", str(args.abs_threshold),
    "--impact-threshold", str(args.impact_threshold),
    "--asym-threshold", str(args.asym_threshold),
    "--local-spike-threshold", str(args.local_spike_threshold),
    "--data-pull-threshold", str(args.data_pull_threshold),
    "--min-neff", str(args.min_neff),
    "--flag-verbosity", args.flag_verbosity,
    "--max-flags", str(args.max_flags),
  ]

  if args.add_total_bkg:
    cmd.append("--add-total-bkg")
  if args.check_stored_total_bkg:
    cmd.append("--check-stored-total-bkg")
  if args.physics_processes:
    cmd.append("--physics-processes")
  if args.full_reports:
    cmd.append("--full")
  if args.legacy_total_bkg:
    cmd.append("--legacy-total-bkg")

  # Optional extra passthrough arguments, e.g.
  #   --scanner-extra --data-pull-mode poisson --same-side-min-rel 0.02
  if args.scanner_extra:
    cmd.extend(args.scanner_extra)

  code, output = run_command(cmd, log_handle)
  return code == 0, output, cmd


def section_name(line):
  m = re.match(r"^\[(.+)\]\s*$", line.strip())
  if not m:
    return None
  return m.group(1).strip()


def parse_bin_problem_line(parts):
  """
  Expected columns after A-patches:

    process era syst bin xlow xhigh nom up down up-nom down-nom up/nom down/nom
    errN errU errD neffN neffU neffD nom/tot dUp/tot dDn/tot data data-nom pull sev flags
  """
  if len(parts) < 27:
    return None
  if not parts[3].isdigit():
    return None

  return {
    "kind": "bin",
    "process": parts[0],
    "era": parts[1],
    "syst": parts[2],
    "bin": int(parts[3]),
    "xlow": parts[4],
    "xhigh": parts[5],
    "nom": parts[6],
    "up": parts[7],
    "down": parts[8],
    "d_up": parts[9],
    "d_down": parts[10],
    "r_up": parts[11],
    "r_down": parts[12],
    "err_nom": parts[13],
    "err_up": parts[14],
    "err_down": parts[15],
    "neff_nom": parts[16],
    "neff_up": parts[17],
    "neff_down": parts[18],
    "frac_total": parts[19],
    "d_up_total": parts[20],
    "d_down_total": parts[21],
    "data": parts[22],
    "data_minus_nom": parts[23],
    "data_pull": parts[24],
    "severity": parts[25],
    "flags": " ".join(parts[26:]),
  }


def parse_integral_problem_line(parts):
  """
  Expected columns after A-patches:

    process era syst nom_int up_int down_int up-nom down-nom up/nom down/nom
    errN errU errD nom/tot dUp/tot dDn/tot data data-nom pull sev flags
  """
  if len(parts) < 21:
    return None
  if parts[0] == "process" or parts[3] == "nom_int":
    return None

  return {
    "kind": "integral",
    "process": parts[0],
    "era": parts[1],
    "syst": parts[2],
    "bin": None,
    "xlow": None,
    "xhigh": None,
    "nom": parts[3],
    "up": parts[4],
    "down": parts[5],
    "d_up": parts[6],
    "d_down": parts[7],
    "r_up": parts[8],
    "r_down": parts[9],
    "err_nom": parts[10],
    "err_up": parts[11],
    "err_down": parts[12],
    "frac_total": parts[13],
    "d_up_total": parts[14],
    "d_down_total": parts[15],
    "data": parts[16],
    "data_minus_nom": parts[17],
    "data_pull": parts[18],
    "severity": parts[19],
    "flags": " ".join(parts[20:]),
  }


def parse_report(report_path, root_file):
  rows = []
  current_section = None

  with open(report_path, "r", encoding="utf-8", errors="replace") as f:
    for line in f:
      sec = section_name(line)
      if sec:
        current_section = sec
        continue

      if current_section not in ("ALL BIN-LEVEL PROBLEMS", "INTEGRAL-LEVEL PROBLEMS"):
        continue

      stripped = line.strip()
      if not stripped:
        continue
      if stripped.startswith("process ") or stripped.startswith("bin "):
        continue
      if set(stripped) <= set("-="):
        continue

      parts = stripped.split()

      row = None
      if current_section == "ALL BIN-LEVEL PROBLEMS":
        row = parse_bin_problem_line(parts)
      elif current_section == "INTEGRAL-LEVEL PROBLEMS":
        row = parse_integral_problem_line(parts)

      if row:
        row["report"] = report_path
        row["root_file"] = root_file
        rows.append(row)

  return rows


def score_row(row):
  flags = split_flags(row.get("flags", ""))
  sev = row.get("severity", "ok")

  score = SEVERITY_WEIGHT.get(sev, 0.0)

  for flag in flags:
    score += FLAG_WEIGHT.get(flag, 100.0)

  abs_dutot = abs(safe_float(row.get("d_up_total"), 0.0) or 0.0)
  abs_ddtot = abs(safe_float(row.get("d_down_total"), 0.0) or 0.0)
  abs_pull = abs(safe_float(row.get("data_pull"), 0.0) or 0.0)

  # Impact on local/total prediction is usually the most actionable scalar.
  score += 200000.0 * max(abs_dutot, abs_ddtot)

  # Data-related rows matter most for total_bkg.
  if abs_pull > 0:
    score += 5000.0 * abs_pull

  if row.get("process") == "total_bkg":
    score *= 1.5

  if row.get("kind") == "integral":
    score *= 1.2

  return score


def reason_for_row(row):
  flags = split_flags(row.get("flags", ""))
  important = sorted(flags, key=lambda f: FLAG_WEIGHT.get(f, 0.0), reverse=True)[:4]

  bits = []
  if important:
    bits.append("flags=" + ",".join(important))

  d_up_total = safe_float(row.get("d_up_total"))
  d_down_total = safe_float(row.get("d_down_total"))
  pull = safe_float(row.get("data_pull"))

  if d_up_total is not None or d_down_total is not None:
    bits.append("dUp/tot={}, dDn/tot={}".format(
      row.get("d_up_total", "NA"),
      row.get("d_down_total", "NA")
    ))

  if pull is not None:
    bits.append("pull={}".format(row.get("data_pull", "NA")))

  if row.get("kind") == "bin":
    bits.append("bin={}".format(row.get("bin")))

  return "; ".join(bits) if bits else "ranked by severity/impact"


def extract_era_from_syst(syst):
  for era in ERAS:
    if era in syst:
      return era
  return None


def make_top_full_report(row, rank, args, log_handle):
  process = row["process"]
  syst = row["syst"]
  root_file = row["root_file"]
  bin_id = row.get("bin") or args.focus_bin

  base = os.path.basename(root_file)
  if base.endswith(".root"):
    base = base[:-5]

  safe_syst = re.sub(r"[^A-Za-z0-9_.+-]+", "_", syst)
  out_name = "top{rank:03d}__{base}__{proc}__{era}__{syst}__bin{bin}.txt".format(
    rank=rank,
    base=re.sub(r"[^A-Za-z0-9_.+-]+", "_", base),
    proc=re.sub(r"[^A-Za-z0-9_.+-]+", "_", process),
    era=re.sub(r"[^A-Za-z0-9_.+-]+", "_", row["era"]),
    syst=safe_syst,
    bin=bin_id
  )
  out_path = os.path.join(args.out_dir, "top_full_reports", out_name)

  cmd = [
    args.python,
    args.scanner,
    root_file,
    "--out", out_path,
    "--focus-bin", str(bin_id),
    "--syst", syst,
    "--full",
    "--min-severity", "info",
    "--flag-verbosity", "full",
    "--max-flags", "20",
  ]

  if process:
    cmd += ["--process", process]

  if args.add_total_bkg or process == "total_bkg":
    cmd.append("--add-total-bkg")
  if args.check_stored_total_bkg:
    cmd.append("--check-stored-total-bkg")
  if args.legacy_total_bkg:
    cmd.append("--legacy-total-bkg")

  # For process-level rows, era is explicit.
  # For Run2 total_bkg, restrict source era if the nuisance name contains an era.
  if row["era"] in ERAS:
    cmd += ["--era", row["era"]]
  elif process == "total_bkg":
    src_era = extract_era_from_syst(syst)
    if src_era:
      cmd += ["--era", src_era]

  if args.scanner_extra:
    cmd.extend(args.scanner_extra)

  code, output = run_command(cmd, log_handle)
  return out_path if code == 0 else None


def write_recommendations(path, ranked_rows, failed_scans, args, top_full_paths):
  with open(path, "w", encoding="utf-8") as f:
    f.write("# Risk recommendations from Combine input histogram scans\n")
    f.write("# out_dir      : {}\n".format(os.path.abspath(args.out_dir)))
    f.write("# n_ranked_rows: {}\n".format(len(ranked_rows)))
    f.write("# top_n        : {}\n".format(args.top_n))
    f.write("# make_top_full: {}\n".format(args.make_top_full))
    f.write("\n")

    if failed_scans:
      f.write("[FAILED SCANS]\n")
      for root_file, message in failed_scans:
        f.write("  - {} : {}\n".format(root_file, message.strip().splitlines()[-1] if message else "failed"))
      f.write("\n")

    f.write("[TOP RISKY ENTRIES]\n")
    f.write("{rank:>4} {score:>12} {kind:<9} {proc:<18} {era:<12} {syst:<55} {bin:>5} {severity:<8} {report}\n".format(
      rank="rank", score="score", kind="kind", proc="process", era="era",
      syst="syst", bin="bin", severity="severity", report="report"
    ))

    for i, row in enumerate(ranked_rows[:args.top_n], start=1):
      bin_label = str(row["bin"]) if row.get("bin") is not None else "-"
      f.write("{rank:4d} {score:12.1f} {kind:<9} {proc:<18} {era:<12} {syst:<55} {bin:>5} {severity:<8} {report}\n".format(
        rank=i,
        score=row["score"],
        kind=row["kind"],
        proc=row["process"],
        era=row["era"],
        syst=row["syst"],
        bin=bin_label,
        severity=row["severity"],
        report=os.path.relpath(row["report"], args.out_dir),
      ))
      f.write("      reason: {}\n".format(reason_for_row(row)))
      f.write("      root  : {}\n".format(row["root_file"]))

      if i in top_full_paths:
        f.write("      full  : {}\n".format(os.path.relpath(top_full_paths[i], args.out_dir)))

      f.write("\n")

    f.write("\n[HOW TO READ]\n")
    f.write("  1. Open the recommended report path first.\n")
    f.write("  2. Search for the process/era/syst/bin shown above.\n")
    f.write("  3. If a top_full_reports file exists, open that first; it is a targeted --full scan.\n")
    f.write("  4. Prioritize total_bkg rows and LARGE_TOTAL_IMPACT/DATA_PULL rows before tiny-process HUGE_REL rows.\n")


def parse_args():
  p = argparse.ArgumentParser(description="Run all Combine input scans and rank risky bins/processes/systematics.")
  p.add_argument("inputs", nargs="+",
                 help="ROOT files, directories, or glob patterns. Directories are searched recursively for *_card_input.root.")
  p.add_argument("--scanner", default="./scan_combine_input_hists.py",
                 help="Path to scan_combine_input_hists.py")
  p.add_argument("--python", default=sys.executable,
                 help="Python executable to use. Default: current Python.")
  p.add_argument("--out-dir", default="scan_campaign",
                 help="Output directory for reports and recommendations.")

  p.add_argument("--physics-processes", action="store_true",
                 help="Pass --physics-processes to scanner. Omit this to scan individual MC diagnostic processes too.")
  p.add_argument("--add-total-bkg", action="store_true", default=True,
                 help="Pass --add-total-bkg to scanner. Default: on.")
  p.add_argument("--no-add-total-bkg", dest="add_total_bkg", action="store_false",
                 help="Do not pass --add-total-bkg.")
  p.add_argument("--check-stored-total-bkg", action="store_true", default=True,
                 help="Pass --check-stored-total-bkg. Default: on.")
  p.add_argument("--no-check-stored-total-bkg", dest="check_stored_total_bkg", action="store_false",
                 help="Do not pass --check-stored-total-bkg.")
  p.add_argument("--legacy-total-bkg", action="store_true",
                 help="Pass --legacy-total-bkg to scanner.")

  p.add_argument("--focus-bin", type=int, default=0,
                 help="Focus bin for generic scans. Use 0 to disable. Top full reports override this with their bin.")
  p.add_argument("--full-reports", action="store_true",
                 help="Make every base report --full. Usually too large; default is problem-only.")
  p.add_argument("--make-top-full", type=int, default=20,
                 help="Create targeted --full reports for top N entries. Use 0 to disable.")
  p.add_argument("--top-n", type=int, default=50,
                 help="Number of entries to list in recommendations.")

  p.add_argument("--min-severity", choices=["info", "warning", "risk", "fatal"], default="warning")
  p.add_argument("--rel-threshold", default="1.0")
  p.add_argument("--abs-threshold", default="1e-6")
  p.add_argument("--impact-threshold", default="0.05")
  p.add_argument("--asym-threshold", default="5.0")
  p.add_argument("--local-spike-threshold", default="5.0")
  p.add_argument("--data-pull-threshold", default="2.0")
  p.add_argument("--min-neff", default="10.0")
  p.add_argument("--flag-verbosity", choices=["compact", "full", "none"], default="compact")
  p.add_argument("--max-flags", default="4")

  p.add_argument("--scanner-extra", nargs=argparse.REMAINDER, default=[],
                 help="Extra arguments passed to scanner. Put this last, e.g. --scanner-extra --data-pull-mode poisson")

  return p.parse_args()


def main():
  args = parse_args()

  root_files = expand_inputs(args.inputs)
  if not root_files:
    sys.stderr.write("No input ROOT files found.\n")
    return 2

  ensure_dir(args.out_dir)
  ensure_dir(os.path.join(args.out_dir, "reports"))
  ensure_dir(os.path.join(args.out_dir, "top_full_reports"))

  log_path = os.path.join(args.out_dir, "00_commands.log")
  reco_path = os.path.join(args.out_dir, "00_risk_recommendations.txt")

  all_rows = []
  failed = []
  report_paths = []

  with open(log_path, "w", encoding="utf-8") as log:
    log.write("# scan_all_combine_risks command log\n\n")

    for idx, root_file in enumerate(root_files, start=1):
      report_name = make_report_name(root_file)
      report_path = os.path.join(args.out_dir, "reports", report_name)

      print("[{}/{}] scanning {}".format(idx, len(root_files), root_file))
      ok, output, cmd = run_scanner_for_file(root_file, report_path, args, log)

      if not ok:
        failed.append((root_file, output))
        continue

      report_paths.append(report_path)
      rows = parse_report(report_path, root_file)
      all_rows.extend(rows)

    for row in all_rows:
      row["score"] = score_row(row)

    ranked = sorted(all_rows, key=lambda r: r["score"], reverse=True)

    top_full_paths = OrderedDict()
    n_make = min(args.make_top_full, len(ranked))
    seen_full_keys = set()

    for i, row in enumerate(ranked[:n_make], start=1):
      key = (row["root_file"], row["process"], row["era"], row["syst"], row.get("bin"))
      if key in seen_full_keys:
        continue
      seen_full_keys.add(key)

      print("[top-full {}/{}] {} {} {} bin {}".format(
        i, n_make, row["process"], row["syst"], row["era"], row.get("bin")
      ))
      full_path = make_top_full_report(row, i, args, log)
      if full_path:
        top_full_paths[i] = full_path

  write_recommendations(reco_path, ranked, failed, args, top_full_paths)

  print("Done.")
  print("Scanned ROOT files:", len(root_files))
  print("Base reports:", os.path.join(args.out_dir, "reports"))
  print("Top full reports:", os.path.join(args.out_dir, "top_full_reports"))
  print("Recommendations:", reco_path)
  print("Command log:", log_path)

  return 0


if __name__ == "__main__":
  sys.exit(main())
