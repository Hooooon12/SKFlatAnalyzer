#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Scan CMS Combine input ROOT histograms bin-by-bin.

The script groups histograms by
  <process>_<era>                      nominal
  <process>_<era>_<systematic>Up       up variation
  <process>_<era>_<systematic>Down     down variation

and writes a human-readable aligned text report comparing nominal / Up / Down
for every process, era, systematic, and bin.

Example:
  python scan_combine_input_hists.py M1000_EE_card_input.root \
    --out scan_M1000_EE_sr2.txt \
    --focus-bin 8 \
    --problem-only

For a full dump:
  python scan_combine_input_hists.py M1000_EE_card_input.root \
    --out scan_M1000_EE_sr2_full.txt \
    --focus-bin 8 \
    --full
"""

from __future__ import print_function

import argparse
import math
import os
import sys
from collections import defaultdict, OrderedDict

ERAS = ("2016preVFP", "2016postVFP", "2017", "2018")

# Datacard-rate background definition.  This should match the Combine model.
CARD_BKG_PROCESSES = (
  "fake",
  "cf",
  "zg",
  "zz",
  "wz",
  "wz_ewk",
  "ww",
  "mc_others",
)

# Backward-compatible total-bkg definition for older ROOT files that do not
# contain mc_others but do contain conv_others/prompt_others.
LEGACY_TOTAL_BKG_PROCESSES = (
  "fake",
  "cf",
  "zg",
  "zz",
  "wz",
  "wz_ewk",
  "ww",
  "conv_others",
  "prompt_others",
)

SIGNAL_PROCESSES = (
  "signalDYVBF",   # diagnostic only in the split-signal datacard setup
  "signalDY",
  "signalVBF",
  "signalSSWW",
  "signalWeinberg",
)

SUMMARY_PROCESSES = (
  "conv_inc",
  "conv_others",
  "prompt_inc",
  "prompt_others",
  "mc_inc",
)

# --physics-processes means "main things I normally want to inspect".
# Individual MC samples are still scanned when --physics-processes is not used,
# or when they are requested explicitly with --process.
PHYSICS_PROCESSES = CARD_BKG_PROCESSES + SIGNAL_PROCESSES + (
  "total_bkg",     # computed pseudo-process
)

TOTAL_BKG_NAME = "total_bkg"       # computed pseudo-process in this scanner
STORED_TOTAL_BKG_NAME = "tot_bkg"  # expected histogram name in ROOT, if MakeInput writes it
RUN2_TOTAL_BKG_ERA = "Run2"

# Default total background components.  resolve_total_bkg_processes() can switch
# to the legacy conv_others+prompt_others definition if mc_others is absent.
TOTAL_BKG_PROCESSES = CARD_BKG_PROCESSES


def is_finite_number(x):
  return x is not None and not (math.isnan(float(x)) or math.isinf(float(x)))


def is_close(a, b, eps):
  if a is None or b is None:
    return False
  if not is_finite_number(a) or not is_finite_number(b):
    return False
  return abs(a - b) <= eps


def safe_ratio(x, nom, eps):
  if x is None or nom is None:
    return "NA"
  if not is_finite_number(x) or not is_finite_number(nom):
    return "NA"
  if abs(nom) <= eps:
    if abs(x) <= eps:
      return "0/0"
    return "+inf" if x > 0 else "-inf"
  return "{:.6g}".format(x / nom)


def safe_divide(x, denom, eps):
  if x is None or denom is None:
    return None
  if not is_finite_number(x) or not is_finite_number(denom):
    return None
  if abs(denom) <= eps:
    return None
  return x / denom


def fmt_scalar(x, precision=10, trim=False):
  """
  Human-readable scalar formatter.

  precision=10 means fixed decimal up to 10 digits after the decimal point.
  trim=True removes trailing zeros for short inline messages.
  """
  if x is None:
    return "NA"

  try:
    x = float(x)
  except Exception:
    return str(x)

  if math.isnan(x) or math.isinf(x):
    return str(x)

  # Avoid ugly -0.0000000000
  if abs(x) < 0.5 * (10.0 ** (-precision)):
    x = 0.0

  s = ("{0:." + str(precision) + "f}").format(x)

  if trim:
    s = s.rstrip("0").rstrip(".")
    if s == "-0":
      s = "0"

  return s


def fmt_float(x):
  # Backward-compatible short formatter used in inline messages.
  return fmt_scalar(x, precision=10, trim=True)


def fmt_col(x, width=17, precision=10):
  """
  Fixed-width table column.

  Use fixed decimal normally.  If a number is too long for the column,
  fall back to compact scientific/general notation rather than breaking alignment.
  """
  if x is None:
    return "NA".rjust(width)

  try:
    xf = float(x)
  except Exception:
    return str(x).rjust(width)

  if math.isnan(xf) or math.isinf(xf):
    return str(xf).rjust(width)

  if abs(xf) < 0.5 * (10.0 ** (-precision)):
    xf = 0.0

  s = ("{0:." + str(precision) + "f}").format(xf)

  if len(s) > width:
    s = ("{0:." + str(max(3, precision - 4)) + "g}").format(xf)

  return s.rjust(width)


def find_process_era_rest(hist_name):
  """
  Return (process, era, rest_after_process_era).

  For example:
    fake_2018 -> ("fake", "2018", "")
    fake_2018_CMS_res_j_2018_sr2Up
      -> ("fake", "2018", "CMS_res_j_2018_sr2Up")
  """
  best = None
  for era in ERAS:
    marker = "_" + era
    idx = hist_name.find(marker)
    if idx < 0:
      continue
    # Require that the era occurrence is followed by end or "_".
    end = idx + len(marker)
    if end != len(hist_name) and hist_name[end] != "_":
      continue
    if best is None or idx < best[0]:
      best = (idx, era, end)

  if best is None:
    return None, None, None

  idx, era, end = best
  process = hist_name[:idx]
  rest = hist_name[end:]
  if rest.startswith("_"):
    rest = rest[1:]
  return process, era, rest


def split_variation_rest(rest):
  if rest == "":
    return None, "Nominal"
  if rest.endswith("Down"):
    return rest[:-4], "Down"
  if rest.endswith("Up"):
    return rest[:-2], "Up"
  return rest, "Unknown"


def get_key_names(tfile):
  names = []
  for key in tfile.GetListOfKeys():
    obj = key.ReadObj()
    if obj.InheritsFrom("TH1"):
      names.append(key.GetName())
  return names


def get_hist(tfile, name):
  obj = tfile.Get(name)
  if not obj:
    return None
  if not obj.InheritsFrom("TH1"):
    return None
  return obj


def bin_edges(hist, ibin):
  xaxis = hist.GetXaxis()
  return xaxis.GetBinLowEdge(ibin), xaxis.GetBinUpEdge(ibin)


def integral_and_error(hist):
  err = ROOT.Double(0.0) if "ROOT" in globals() else 0.0
  # PyROOT type conversion differs between versions. IntegralAndError is not
  # essential for this diagnostic, so fall back to Integral().
  try:
    val = hist.IntegralAndError(1, hist.GetNbinsX(), err)
    return float(val), float(err)
  except Exception:
    return float(hist.Integral(1, hist.GetNbinsX())), None


SEVERITY_LEVELS = OrderedDict([
  ("info", 0),
  ("warning", 1),
  ("risk", 2),
  ("fatal", 3),
])

INFO_FLAGS = set([
  "NO_EFFECT",
  "ONE_SIDED_EFFECT",
  "UP_MOVES_TOWARD_DATA",
  "DOWN_MOVES_TOWARD_DATA",
  "DATA_OUTSIDE_SYST_ENVELOPE",
])

WARNING_FLAGS = set([
  "HUGE_REL_UP",
  "HUGE_REL_DOWN",
  "LOW_NEFF_NOM",
  "LOW_NEFF_UP",
  "LOW_NEFF_DOWN",
  "HIGH_RELSTAT_NOM",
  "HIGH_RELSTAT_UP",
  "HIGH_RELSTAT_DOWN",
  "ZERO_CONTENT_NONZERO_ERR_NOM",
  "ZERO_CONTENT_NONZERO_ERR_UP",
  "ZERO_CONTENT_NONZERO_ERR_DOWN",
  "ZERO_ERR_NONZERO_NOM",
  "ZERO_ERR_NONZERO_UP",
  "ZERO_ERR_NONZERO_DOWN",
  "LOCAL_SPIKE_UP",
  "LOCAL_SPIKE_DOWN",
])

RISK_FLAGS = set([
  "ZERO_NOM_NONZERO_VAR",
  "ONLY_UP_SURVIVES",
  "ONLY_DOWN_SURVIVES",
  "UP_KILLS_NOM",
  "DOWN_KILLS_NOM",
  "BOTH_ABOVE_NOM",
  "BOTH_BELOW_NOM",
  "HUGE_ABS_UP_AT_ZERO_NOM",
  "HUGE_ABS_DOWN_AT_ZERO_NOM",
  "ASYM_LARGE",
  "LARGE_TOTAL_IMPACT_UP",
  "LARGE_TOTAL_IMPACT_DOWN",
  "DATA_PULL_HIGH",
  "STORED_TOTAL_MISMATCH",
])

FATAL_FLAGS = set([
  "NEG_NOM",
  "NEG_UP",
  "NEG_DOWN",
  "MISS_UP",
  "MISS_DOWN",
  "MISSING_NOMINAL",
  "UP_NBINS_MISMATCH",
  "DOWN_NBINS_MISMATCH",
  "UP_BIN_EDGE_MISMATCH",
  "DOWN_BIN_EDGE_MISMATCH",
  "TOTAL_BKG_COMPONENT_BINNING_MISMATCH",
  "STORED_TOTAL_NBINS_MISMATCH",
  "STORED_TOTAL_BIN_EDGE_MISMATCH",
  "NONFINITE_NOM",
  "NONFINITE_UP",
  "NONFINITE_DOWN",
])


def normalize_flags(flags):
  if flags is None:
    return []

  if isinstance(flags, str):
    if flags == "" or flags == "-":
      return []
    flags = flags.split(",")

  out = []
  for flag in flags:
    flag = str(flag).strip()
    if flag and flag != "-":
      out.append(flag)
  return out


def flag_severity(flags):
  flags = normalize_flags(flags)
  if not flags:
    return "ok"

  if any(flag in FATAL_FLAGS or flag.startswith("NONFINITE_") for flag in flags):
    return "fatal"
  if any(flag in RISK_FLAGS for flag in flags):
    return "risk"
  if any(flag in WARNING_FLAGS for flag in flags):
    return "warning"
  if any(flag in INFO_FLAGS for flag in flags):
    return "info"

  # Unknown new flags should not be silently ignored.
  return "warning"


FLAG_DISPLAY_PRIORITY = [
  # fatal / structural
  "NONFINITE_NOM", "NONFINITE_UP", "NONFINITE_DOWN",
  "NEG_NOM", "NEG_UP", "NEG_DOWN",
  "MISSING_NOMINAL", "MISS_UP", "MISS_DOWN",
  "UP_NBINS_MISMATCH", "DOWN_NBINS_MISMATCH",
  "UP_BIN_EDGE_MISMATCH", "DOWN_BIN_EDGE_MISMATCH",
  "TOTAL_BKG_COMPONENT_BINNING_MISMATCH",
  "STORED_TOTAL_NBINS_MISMATCH", "STORED_TOTAL_BIN_EDGE_MISMATCH",

  # high fit-risk
  "ONLY_UP_SURVIVES", "ONLY_DOWN_SURVIVES",
  "UP_KILLS_NOM", "DOWN_KILLS_NOM",
  "ZERO_NOM_NONZERO_VAR",
  "HUGE_ABS_UP_AT_ZERO_NOM", "HUGE_ABS_DOWN_AT_ZERO_NOM",
  "LARGE_TOTAL_IMPACT_UP", "LARGE_TOTAL_IMPACT_DOWN",
  "DATA_PULL_HIGH",
  "STORED_TOTAL_MISMATCH",

  # medium fit-risk / warnings
  "BOTH_ABOVE_NOM", "BOTH_BELOW_NOM",
  "ASYM_LARGE",
  "LOCAL_SPIKE_UP", "LOCAL_SPIKE_DOWN",
  "HUGE_REL_UP", "HUGE_REL_DOWN",
  "ZERO_CONTENT_NONZERO_ERR_NOM",
  "ZERO_CONTENT_NONZERO_ERR_UP",
  "ZERO_CONTENT_NONZERO_ERR_DOWN",
  "ZERO_ERR_NONZERO_NOM",
  "ZERO_ERR_NONZERO_UP",
  "ZERO_ERR_NONZERO_DOWN",
  "LOW_NEFF_NOM", "LOW_NEFF_UP", "LOW_NEFF_DOWN",
  "HIGH_RELSTAT_NOM", "HIGH_RELSTAT_UP", "HIGH_RELSTAT_DOWN",

  # info
  "DATA_OUTSIDE_SYST_ENVELOPE",
  "UP_MOVES_TOWARD_DATA", "DOWN_MOVES_TOWARD_DATA",
  "ONE_SIDED_EFFECT", "NO_EFFECT",
]

FLAG_PRIORITY_MAP = {flag: i for i, flag in enumerate(FLAG_DISPLAY_PRIORITY)}


def sort_flags_for_display(flags):
  flags = normalize_flags(flags)
  return sorted(flags, key=lambda f: FLAG_PRIORITY_MAP.get(f, 9999))


def compact_flags_for_output(flags, args=None):
  """
  Keep raw row['flags'] unchanged for filtering/ranking,
  but print only the most useful flags by default.
  """
  flags = normalize_flags(flags)
  if not flags:
    return "-"

  verbosity = getattr(args, "flag_verbosity", "compact") if args is not None else "compact"
  max_flags = getattr(args, "max_flags", 4) if args is not None else 4

  if verbosity == "none":
    return "-"

  if verbosity == "full":
    return ",".join(sort_flags_for_display(flags))

  # compact mode:
  # hide purely informational flags unless there is nothing else to show.
  non_info = [f for f in flags if f not in INFO_FLAGS and f != "DATA_OUTSIDE_SYST_ENVELOPE"]

  if non_info:
    show_flags = non_info[:]
  else:
    show_flags = flags[:]

  # If DATA_PULL_HIGH is already present, DATA_OUTSIDE_SYST_ENVELOPE adds little
  # visual value in compact mode.
  if "DATA_PULL_HIGH" in show_flags and "DATA_OUTSIDE_SYST_ENVELOPE" in show_flags:
    show_flags.remove("DATA_OUTSIDE_SYST_ENVELOPE")

  show_flags = sort_flags_for_display(show_flags)

  if max_flags > 0 and len(show_flags) > max_flags:
    n_more = len(show_flags) - max_flags
    show_flags = show_flags[:max_flags] + ["+{}more".format(n_more)]

  return ",".join(show_flags) if show_flags else "-"

def severity_passes_min(severity, min_severity):
  if severity == "ok":
    return False
  if min_severity == "ok":
    return True
  return SEVERITY_LEVELS.get(severity, 1) >= SEVERITY_LEVELS.get(min_severity, 1)


def classify_bin(nom, up, down, args):
  flags = []

  vals = [("nom", nom), ("up", up), ("down", down)]
  for label, val in vals:
    if val is None:
      continue
    if not is_finite_number(val):
      flags.append("NONFINITE_" + label.upper())
    elif val < -args.eps:
      flags.append("NEG_" + label.upper())

  if up is None:
    flags.append("MISS_UP")
  if down is None:
    flags.append("MISS_DOWN")
  if up is None or down is None:
    return flags

  if any(not is_finite_number(v) for v in [nom, up, down]):
    return flags

  nom0 = abs(nom) <= args.eps
  up0 = abs(up) <= args.eps
  down0 = abs(down) <= args.eps

  if nom0 and (not up0 or not down0):
    flags.append("ZERO_NOM_NONZERO_VAR")
  if nom0 and down0 and not up0:
    flags.append("ONLY_UP_SURVIVES")
  if nom0 and up0 and not down0:
    flags.append("ONLY_DOWN_SURVIVES")

  if (not nom0) and up0:
    flags.append("UP_KILLS_NOM")
  if (not nom0) and down0:
    flags.append("DOWN_KILLS_NOM")

  if is_close(up, nom, args.eps) and is_close(down, nom, args.eps):
    flags.append("NO_EFFECT")
  elif is_close(up, nom, args.eps) or is_close(down, nom, args.eps):
    flags.append("ONE_SIDED_EFFECT")

  du_abs = abs(up - nom)
  dd_abs = abs(down - nom)
  max_abs_effect = max(du_abs, dd_abs)

  if abs(nom) > args.eps:
    max_rel_effect = max_abs_effect / abs(nom)
  else:
    max_rel_effect = float("inf") if max_abs_effect > args.eps else 0.0

  # Same-side variations are interesting only if the effect is not tiny.
  if max_rel_effect >= args.same_side_min_rel or max_abs_effect >= args.abs_threshold:
    if up > nom + args.eps and down > nom + args.eps:
      flags.append("BOTH_ABOVE_NOM")
    if up < nom - args.eps and down < nom - args.eps:
      flags.append("BOTH_BELOW_NOM")

  if abs(nom) > args.eps:
    rel_up = abs((up - nom) / nom)
    rel_down = abs((down - nom) / nom)
    if rel_up >= args.rel_threshold:
      flags.append("HUGE_REL_UP")
    if rel_down >= args.rel_threshold:
      flags.append("HUGE_REL_DOWN")
  else:
    if abs(up - nom) >= args.abs_threshold:
      flags.append("HUGE_ABS_UP_AT_ZERO_NOM")
    if abs(down - nom) >= args.abs_threshold:
      flags.append("HUGE_ABS_DOWN_AT_ZERO_NOM")

  if args.asym_threshold > 0:
    du = abs(up - nom)
    dd = abs(down - nom)
    if du > args.eps and dd > args.eps:
      asym = max(du, dd) / max(min(du, dd), args.eps)

      # Do not flag tiny numerical asymmetries.
      if (
        asym >= args.asym_threshold
        and (
          max_rel_effect >= args.asym_min_rel
          or max_abs_effect >= args.abs_threshold
        )
      ):
        flags.append("ASYM_LARGE")

  return flags


def classify_integral(nom_int, up_int, down_int, args):
  return classify_bin(nom_int, up_int, down_int, args)


def classify_total_impact(nom, up, down, total_nom, args):
  flags = []
  if args.impact_threshold <= 0:
    return flags
  if total_nom is None or not is_finite_number(total_nom) or abs(total_nom) <= args.eps:
    return flags

  if up is not None and is_finite_number(up):
    if abs((up - nom) / total_nom) >= args.impact_threshold:
      flags.append("LARGE_TOTAL_IMPACT_UP")

  if down is not None and is_finite_number(down):
    if abs((down - nom) / total_nom) >= args.impact_threshold:
      flags.append("LARGE_TOTAL_IMPACT_DOWN")

  return flags


def classify_data_alignment(nom, up, down, data, ref_err, args):
  flags = []
  if data is None:
    return flags
  if up is None or down is None:
    return flags
  if any(not is_finite_number(v) for v in [nom, up, down, data]):
    return flags

  dist_nom = abs(data - nom)
  if dist_nom <= args.eps:
    return flags

  if abs(data - up) < dist_nom - args.eps:
    flags.append("UP_MOVES_TOWARD_DATA")
  if abs(data - down) < dist_nom - args.eps:
    flags.append("DOWN_MOVES_TOWARD_DATA")

  lo = min(nom, up, down)
  hi = max(nom, up, down)
  if data < lo - args.eps or data > hi + args.eps:
    flags.append("DATA_OUTSIDE_SYST_ENVELOPE")

  pull_denom = None

  if args.data_pull_mode == "none":
    pull_denom = None

  elif args.data_pull_mode == "mcstat":
    if ref_err is not None and is_finite_number(ref_err) and abs(ref_err) > args.eps:
      pull_denom = ref_err

  elif args.data_pull_mode == "poisson":
    # More realistic for data-vs-background visual diagnostics:
    # data fluctuation scale is roughly sqrt(expected), plus MC stat.
    mc_err2 = 0.0
    if ref_err is not None and is_finite_number(ref_err):
      mc_err2 = ref_err * ref_err
    pull_denom = math.sqrt(max(nom, 0.0) + mc_err2)

  else:
    raise ValueError("Unknown data_pull_mode: " + str(args.data_pull_mode))

  if pull_denom is not None and pull_denom > args.eps:
    pull = (data - nom) / pull_denom
    if abs(pull) >= args.data_pull_threshold:
      flags.append("DATA_PULL_HIGH")

  return flags


def classify_local_spike(nom, up, down, int_nom, int_up, int_down, args):
  flags = []
  if args.local_spike_threshold <= 0:
    return flags
  if int_nom is None or not is_finite_number(int_nom) or abs(int_nom) <= args.eps:
    return flags
  if nom is None or not is_finite_number(nom) or abs(nom) <= args.eps:
    return flags

  if up is not None and int_up is not None and is_finite_number(up) and is_finite_number(int_up):
    bin_rel = abs((up - nom) / nom)
    int_rel = abs((int_up - int_nom) / int_nom)
    if bin_rel >= args.rel_threshold and bin_rel >= args.local_spike_threshold * max(int_rel, args.eps):
      flags.append("LOCAL_SPIKE_UP")

  if down is not None and int_down is not None and is_finite_number(down) and is_finite_number(int_down):
    bin_rel = abs((down - nom) / nom)
    int_rel = abs((int_down - int_nom) / int_nom)
    if bin_rel >= args.rel_threshold and bin_rel >= args.local_spike_threshold * max(int_rel, args.eps):
      flags.append("LOCAL_SPIKE_DOWN")

  return flags


def row_is_problem(flags, args):
  flags = normalize_flags(flags)
  if not flags:
    return False

  if not args.include_no_effect:
    flags = [f for f in flags if f != "NO_EFFECT"]
    if not flags:
      return False

  severity = flag_severity(flags)
  return severity_passes_min(severity, args.min_severity)


def flags_match(flags, wanted_flags):
  """
  Return True if this row should pass the --flag filter.

  wanted_flags is args.flag, e.g.
    ["ONLY_UP_SURVIVES", "ZERO_NOM_NONZERO_VAR"]

  If no --flag is given, do not filter by flag.
  """
  if not wanted_flags:
    return True

  flags = normalize_flags(flags)
  return any(flag in wanted_flags for flag in flags)


def row_selected_for_problem_summary(flags, args):
  """
  Decide whether a row should enter the summary problem tables.

  If --flag is given, use only the requested flags.
  Otherwise use the severity-aware problem definition.
  """
  if args.flag:
    return flags_match(flags, args.flag)
  return row_is_problem(flags, args)


def row_selected_for_detail(flags, args):
  """
  Decide whether a row should be printed in the detailed scan.

  In --full mode:
    print all rows unless --flag restricts them.

  In default problem-only mode:
    print only severity-selected rows unless --flag restricts them.
  """
  if args.flag:
    return flags_match(flags, args.flag)

  if args.full:
    return True

  return row_is_problem(flags, args)

def keep_process(args, process):
  if args.physics_processes and process not in PHYSICS_PROCESSES:
    return False

  if args.process and process not in args.process:
    return False

  return True


def keep_era(args, era):
  if args.era and era not in args.era:
    return False

  return True


def keep_syst(args, syst):
  if args.syst and syst not in args.syst:
    return False

  if args.syst_contains:
    if not any(token in syst for token in args.syst_contains):
      return False

  return True

def hist_bin_error(hist, ibin):
  if not hist:
    return None
  if ibin > hist.GetNbinsX():
    return None
  return float(hist.GetBinError(ibin))


def effective_entries(content, error, eps):
  """
  Effective entries:
    N_eff = (sum w)^2 / sum(w^2)
          = content^2 / error^2

  This is meaningful when bin errors are stored as sqrt(sum w^2),
  which is the usual Sumw2 convention.
  """
  if error is None:
    return None

  if abs(error) <= eps:
    if abs(content) <= eps:
      return 0.0
    return float("inf")

  return (content * content) / (error * error)


def rel_stat_error(content, error, eps):
  if error is None:
    return None

  if abs(content) <= eps:
    if abs(error) <= eps:
      return 0.0
    return float("inf")

  return abs(error / content)


def classify_stat_value(label, content, error, args):
  """
  Add stat-related flags using GetBinError and effective entries.

  label should be one of:
    NOM, UP, DOWN
  """
  flags = []

  if error is None or content is None:
    return flags

  if not is_finite_number(content) or not is_finite_number(error):
    flags.append("NONFINITE_" + label)
    return flags

  if abs(content) > args.eps and abs(error) <= args.eps:
    flags.append("ZERO_ERR_NONZERO_" + label)

  if abs(content) <= args.eps and abs(error) > args.eps:
    flags.append("ZERO_CONTENT_NONZERO_ERR_" + label)

  neff = effective_entries(content, error, args.eps)
  relerr = rel_stat_error(content, error, args.eps)

  if args.min_neff > 0 and neff is not None:
    if abs(content) > args.eps and neff < args.min_neff:
      flags.append("LOW_NEFF_" + label)

  if args.max_rel_stat > 0 and relerr is not None:
    if abs(content) > args.eps and relerr > args.max_rel_stat:
      flags.append("HIGH_RELSTAT_" + label)

  return flags


def make_bin_row(process, era, syst, ibin, low, high,
                 nom, up, down,
                 err_nom, err_up, err_down,
                 base_flags,
                 up_name, down_name,
                 args,
                 total_nom=None,
                 data=None,
                 data_ref_err=None,
                 int_nom=None,
                 int_up=None,
                 int_down=None):
  flags = normalize_flags(base_flags)

  flags.extend(classify_stat_value("NOM", nom, err_nom, args))
  if up is not None:
    flags.extend(classify_stat_value("UP", up, err_up, args))
  if down is not None:
    flags.extend(classify_stat_value("DOWN", down, err_down, args))

  flags.extend(classify_total_impact(nom, up, down, total_nom, args))
  flags.extend(classify_data_alignment(nom, up, down, data, data_ref_err, args))
  flags.extend(classify_local_spike(nom, up, down, int_nom, int_up, int_down, args))

  flags = normalize_flags(flags)
  severity = flag_severity(flags)

  neff_nom = effective_entries(nom, err_nom, args.eps)
  neff_up = effective_entries(up, err_up, args.eps) if up is not None else None
  neff_down = effective_entries(down, err_down, args.eps) if down is not None else None

  d_up = None if up is None else up - nom
  d_down = None if down is None else down - nom

  frac_total = safe_divide(nom, total_nom, args.eps)
  d_up_total = safe_divide(d_up, total_nom, args.eps)
  d_down_total = safe_divide(d_down, total_nom, args.eps)

  data_minus_nom = None if data is None else data - nom
  data_pull = safe_divide(data_minus_nom, data_ref_err, args.eps)

  return {
    "process": process, "era": era, "syst": syst, "bin": ibin,
    "xlow": low, "xhigh": high,
    "nom": nom, "up": up, "down": down,
    "d_up": d_up,
    "d_down": d_down,
    "r_up": safe_ratio(up, nom, args.eps),
    "r_down": safe_ratio(down, nom, args.eps),
    "err_nom": err_nom,
    "err_up": err_up,
    "err_down": err_down,
    "neff_nom": neff_nom,
    "neff_up": neff_up,
    "neff_down": neff_down,
    "total_nom": total_nom,
    "frac_total": frac_total,
    "d_up_total": d_up_total,
    "d_down_total": d_down_total,
    "data": data,
    "data_minus_nom": data_minus_nom,
    "data_pull": data_pull,
    "severity": severity,
    "flags": ",".join(flags),
    "up_name": str(up_name),
    "down_name": str(down_name)
  }


def make_integral_row(process, era, syst,
                      nom, up, down,
                      err_nom, err_up, err_down,
                      base_flags,
                      up_name, down_name,
                      args,
                      total_nom=None,
                      data=None,
                      data_ref_err=None):
  flags = normalize_flags(base_flags)

  flags.extend(classify_stat_value("NOM", nom, err_nom, args))
  if up is not None:
    flags.extend(classify_stat_value("UP", up, err_up, args))
  if down is not None:
    flags.extend(classify_stat_value("DOWN", down, err_down, args))

  flags.extend(classify_total_impact(nom, up, down, total_nom, args))
  flags.extend(classify_data_alignment(nom, up, down, data, data_ref_err, args))

  flags = normalize_flags(flags)
  severity = flag_severity(flags)

  d_up = None if up is None else up - nom
  d_down = None if down is None else down - nom

  return {
    "process": process, "era": era, "syst": syst,
    "nom": nom, "up": up, "down": down,
    "d_up": d_up,
    "d_down": d_down,
    "r_up": safe_ratio(up, nom, args.eps),
    "r_down": safe_ratio(down, nom, args.eps),
    "err_nom": err_nom,
    "err_up": err_up,
    "err_down": err_down,
    "total_nom": total_nom,
    "frac_total": safe_divide(nom, total_nom, args.eps),
    "d_up_total": safe_divide(d_up, total_nom, args.eps),
    "d_down_total": safe_divide(d_down, total_nom, args.eps),
    "data": data,
    "data_minus_nom": None if data is None else data - nom,
    "data_pull": safe_divide(None if data is None else data - nom, data_ref_err, args.eps),
    "severity": severity,
    "flags": ",".join(flags),
    "up_name": str(up_name),
    "down_name": str(down_name)
  }


def hist_integral_content_error(hist, nbins=None):
  if not hist:
    return None, None

  if nbins is None:
    nbins = hist.GetNbinsX()

  total = 0.0
  err2 = 0.0
  for ibin in range(1, min(nbins, hist.GetNbinsX()) + 1):
    total += float(hist.GetBinContent(ibin))
    err = float(hist.GetBinError(ibin))
    err2 += err * err
  return total, math.sqrt(err2)


def binning_signature(hist):
  if not hist:
    return None
  ax = hist.GetXaxis()
  return tuple(round(ax.GetBinLowEdge(i), 12) for i in range(1, hist.GetNbinsX() + 2))


def same_binning(h_ref, h_new, eps):
  if not h_ref or not h_new:
    return False
  if h_ref.GetNbinsX() != h_new.GetNbinsX():
    return False

  ax_ref = h_ref.GetXaxis()
  ax_new = h_new.GetXaxis()
  for ibin in range(1, h_ref.GetNbinsX() + 2):
    if abs(ax_ref.GetBinLowEdge(ibin) - ax_new.GetBinLowEdge(ibin)) > eps:
      return False
  return True


def add_structural_issue(rows, process, era, syst, issue, nom_name="", up_name="", down_name=""):
  rows.append({
    "process": process,
    "era": era,
    "syst": syst,
    "issue": issue,
    "nom_name": str(nom_name),
    "up_name": str(up_name),
    "down_name": str(down_name),
  })


def resolve_total_bkg_processes(args, nominal_map, component_eras):
  """
  Decide which components define computed total_bkg.

  Default:
    fake + cf + zg + zz + wz + wz_ewk + ww + mc_others

  Fallback:
    fake + cf + zg + zz + wz + wz_ewk + ww + conv_others + prompt_others
    if mc_others is absent and legacy components are present.

  Manual override:
    --total-bkg-process can specify the exact process list.
  """
  if args.total_bkg_process:
    return tuple(args.total_bkg_process)

  if args.legacy_total_bkg:
    return LEGACY_TOTAL_BKG_PROCESSES

  has_mc_others = any(nominal_map.get(("mc_others", era)) for era in component_eras)
  if has_mc_others:
    return CARD_BKG_PROCESSES

  # If there's no mc_others, then make up with conv_others + prompt_others
  has_legacy = any(nominal_map.get(("conv_others", era)) for era in component_eras) or \
               any(nominal_map.get(("prompt_others", era)) for era in component_eras)
  if has_legacy:
    return LEGACY_TOTAL_BKG_PROCESSES

  return CARD_BKG_PROCESSES


def get_hist_for_process_side(tfile, nominal_map, variation_map,
                              process, era, syst, side):
  nom_name = nominal_map.get((process, era))
  nom_hist = get_hist(tfile, nom_name) if nom_name else None

  if side == "Nominal":
    return nom_hist, nom_name

  hist = nom_hist
  hname = nom_name

  if side in ("Up", "Down"):
    var_name = variation_map.get((process, era), {}).get(syst, {}).get(side)
    var_hist = get_hist(tfile, var_name) if var_name else None
    if var_hist:
      hist = var_hist
      hname = var_name

  return hist, hname


def get_total_bkg_ref_hist_for_eras(tfile, nominal_map, eras, component_processes):
  """
  Return a reference histogram for binning/x-axis.
  """
  for era in eras:
    for process in component_processes:
      hname = nominal_map.get((process, era))
      hist = get_hist(tfile, hname) if hname else None
      if hist:
        return hist
  return None


def get_total_bkg_systs_for_eras(variation_map, eras, component_processes):
  """
  Return the union of systematics for total_bkg from selected era(s).

  Important:
    For Run2 total_bkg, the summed content can use all eras,
    but the syst list can be restricted to selected era(s).
  """
  systs = set()
  for era in eras:
    for process in component_processes:
      systs.update(variation_map.get((process, era), {}).keys())
  return sorted(systs)


def total_bkg_bin_value_error(tfile, nominal_map, variation_map,
                              component_eras, component_processes,
                              syst, side, ibin):
  """
  Sum total_bkg over requested eras and background processes.

  Content:
    sum bin contents.

  Error:
    quadrature sum of component bin errors,
    assuming independent MC/stat samples.

  For Up/Down:
    use process_systUp/Down if it exists;
    otherwise use that process nominal.
  """
  total = 0.0
  err2 = 0.0

  for era in component_eras:
    for process in component_processes:
      hist, _ = get_hist_for_process_side(
        tfile, nominal_map, variation_map,
        process, era, syst, side
      )
      if not hist:
        continue

      if ibin <= hist.GetNbinsX():
        val = float(hist.GetBinContent(ibin))
        err = float(hist.GetBinError(ibin))
        total += val
        err2 += err * err

  return total, math.sqrt(err2)


def total_bkg_integral_for_eras(tfile, nominal_map, variation_map,
                                component_eras, component_processes,
                                syst, side, nbins):
  total = 0.0
  err2 = 0.0

  for ibin in range(1, nbins + 1):
    val, err = total_bkg_bin_value_error(
      tfile, nominal_map, variation_map,
      component_eras, component_processes,
      syst, side, ibin
    )
    total += val
    err2 += err * err

  return total, math.sqrt(err2)


def check_total_bkg_component_binning(tfile, nominal_map,
                                      component_eras, component_processes,
                                      ref_hist, structural_rows,
                                      era_label):
  if not ref_hist:
    return

  for era in component_eras:
    for process in component_processes:
      hname = nominal_map.get((process, era))
      hist = get_hist(tfile, hname) if hname else None
      if not hist:
        continue
      if not same_binning(ref_hist, hist, 1.0e-9):
        add_structural_issue(
          structural_rows,
          TOTAL_BKG_NAME,
          era_label,
          "*",
          "TOTAL_BKG_COMPONENT_BINNING_MISMATCH",
          "reference=" + str(ref_hist.GetName()),
          hname,
          ""
        )


def data_obs_bin_value(data_obs, ibin):
  if not data_obs:
    return None
  if ibin > data_obs.GetNbinsX():
    return None
  return float(data_obs.GetBinContent(ibin))


def data_obs_integral(data_obs, nbins):
  if not data_obs:
    return None
  return float(data_obs.Integral(1, min(nbins, data_obs.GetNbinsX())))


def stored_total_bkg_bin_value_error(tfile, nominal_map, component_eras, ibin):
  total = 0.0
  err2 = 0.0
  missing_eras = []

  for era in component_eras:
    hname = nominal_map.get((STORED_TOTAL_BKG_NAME, era))
    hist = get_hist(tfile, hname) if hname else None
    if not hist:
      missing_eras.append(era)
      continue

    if ibin <= hist.GetNbinsX():
      total += float(hist.GetBinContent(ibin))
      err = float(hist.GetBinError(ibin))
      err2 += err * err

  if len(missing_eras) == len(component_eras):
    return None, None, missing_eras

  return total, math.sqrt(err2), missing_eras


def stored_total_bkg_integral_for_eras(tfile, nominal_map, component_eras, nbins):
  total = 0.0
  err2 = 0.0
  missing_any = set()
  found_any = False

  for ibin in range(1, nbins + 1):
    val, err, missing = stored_total_bkg_bin_value_error(
      tfile, nominal_map, component_eras, ibin
    )
    for era in missing:
      missing_any.add(era)
    if val is None:
      continue
    found_any = True
    total += val
    err2 += err * err

  if not found_any:
    return None, None, sorted(missing_any)

  return total, math.sqrt(err2), sorted(missing_any)


def make_groups(hist_names):
  nominal = {}
  variations = defaultdict(lambda: defaultdict(dict))
  unknown = []

  for name in hist_names:
    if name == "data_obs":
      continue

    process, era, rest = find_process_era_rest(name)
    if process is None:
      unknown.append(name)
      continue

    syst, side = split_variation_rest(rest)

    if side == "Nominal":
      nominal[(process, era)] = name
    elif side in ("Up", "Down"):
      variations[(process, era)][syst][side] = name
    else:
      unknown.append(name)

  return nominal, variations, unknown


def write_rule(f, char="-", n=220):
  f.write(char * n + "\n")


def write_bin_row_header(f, prefix=""):
  cols = [
    ("bin", 4),
    ("xlow", 10),
    ("xhigh", 10),
    ("nom", 17),
    ("up", 17),
    ("down", 17),
    ("up-nom", 17),
    ("down-nom", 17),
    ("up/nom", 13),
    ("down/nom", 13),
    ("errN", 17),
    ("errU", 17),
    ("errD", 17),
    ("neffN", 13),
    ("neffU", 13),
    ("neffD", 13),
    ("nom/tot", 13),
    ("dUp/tot", 13),
    ("dDn/tot", 13),
    ("data", 17),
    ("data-nom", 17),
    ("pull", 13),
    ("sev", 7),
  ]
  f.write(prefix + " ".join(name.rjust(width) for name, width in cols) + "  flags\n")


def write_bin_row(f, row, prefix="", args=None):
  vals = [
    str(row["bin"]).rjust(4),
    fmt_col(row["xlow"], 10, 4),
    fmt_col(row["xhigh"], 10, 4),
    fmt_col(row["nom"], 17, 10),
    fmt_col(row["up"], 17, 10),
    fmt_col(row["down"], 17, 10),
    fmt_col(row["d_up"], 17, 10),
    fmt_col(row["d_down"], 17, 10),
    str(row["r_up"]).rjust(13),
    str(row["r_down"]).rjust(13),
    fmt_col(row.get("err_nom"), 17, 10),
    fmt_col(row.get("err_up"), 17, 10),
    fmt_col(row.get("err_down"), 17, 10),
    fmt_col(row.get("neff_nom"), 13, 5),
    fmt_col(row.get("neff_up"), 13, 5),
    fmt_col(row.get("neff_down"), 13, 5),
    fmt_col(row.get("frac_total"), 13, 6),
    fmt_col(row.get("d_up_total"), 13, 6),
    fmt_col(row.get("d_down_total"), 13, 6),
    fmt_col(row.get("data"), 17, 10),
    fmt_col(row.get("data_minus_nom"), 17, 10),
    fmt_col(row.get("data_pull"), 13, 5),
    str(row.get("severity", "ok")).rjust(7),
  ]

  f.write(prefix + " ".join(vals) + "  " + compact_flags_for_output(row.get("flags", ""), args) + "\n")


def write_integral_row_header(f, prefix=""):
  cols = [
    ("process", 18),
    ("era", 12),
    ("syst", 55),
    ("nom_int", 17),
    ("up_int", 17),
    ("down_int", 17),
    ("up-nom", 17),
    ("down-nom", 17),
    ("up/nom", 13),
    ("down/nom", 13),
    ("errN", 17),
    ("errU", 17),
    ("errD", 17),
    ("nom/tot", 13),
    ("dUp/tot", 13),
    ("dDn/tot", 13),
    ("data", 17),
    ("data-nom", 17),
    ("pull", 13),
    ("sev", 7),
  ]
  f.write(prefix + " ".join(name.rjust(width) if i >= 3 else name.ljust(width)
                            for i, (name, width) in enumerate(cols)) + "  flags\n")


def write_integral_row(f, row, prefix="", args=None):
  vals = [
    str(row["process"]).ljust(18),
    str(row["era"]).ljust(12),
    str(row["syst"]).ljust(55),
    fmt_col(row["nom"], 17, 10),
    fmt_col(row["up"], 17, 10),
    fmt_col(row["down"], 17, 10),
    fmt_col(row.get("d_up"), 17, 10),
    fmt_col(row.get("d_down"), 17, 10),
    str(row["r_up"]).rjust(13),
    str(row["r_down"]).rjust(13),
    fmt_col(row.get("err_nom"), 17, 10),
    fmt_col(row.get("err_up"), 17, 10),
    fmt_col(row.get("err_down"), 17, 10),
    fmt_col(row.get("frac_total"), 13, 6),
    fmt_col(row.get("d_up_total"), 13, 6),
    fmt_col(row.get("d_down_total"), 13, 6),
    fmt_col(row.get("data"), 17, 10),
    fmt_col(row.get("data_minus_nom"), 17, 10),
    fmt_col(row.get("data_pull"), 13, 5),
    str(row.get("severity", "ok")).rjust(7),
  ]

  f.write(prefix + " ".join(vals) + "  " + compact_flags_for_output(row.get("flags", ""), args) + "\n")


def write_total_bkg_block(f, args, tfile, nominal_map, variation_map,
                          era_label, component_eras, syst_source_eras,
                          problem_rows, focus_rows, integral_problem_rows,
                          structural_rows, stored_total_check_rows,
                          nbins_seen):
  component_processes = resolve_total_bkg_processes(args, nominal_map, component_eras)
  ref_hist = get_total_bkg_ref_hist_for_eras(
    tfile, nominal_map, component_eras, component_processes
  )
  if not ref_hist:
    return

  nbins = ref_hist.GetNbinsX()
  nbins_seen[(TOTAL_BKG_NAME, era_label)] = nbins

  check_total_bkg_component_binning(
    tfile, nominal_map,
    component_eras, component_processes,
    ref_hist, structural_rows, era_label
  )

  systs = get_total_bkg_systs_for_eras(
    variation_map, syst_source_eras, component_processes
  )

  data_obs = get_hist(tfile, "data_obs")
  use_data_alignment = (era_label == RUN2_TOTAL_BKG_ERA and data_obs is not None)

  f.write("\nPROCESS = {process}   ERA = {era}   NOMINAL = sum({components}) over eras({eras})   NBINS = {nbins}\n".format(
    process=TOTAL_BKG_NAME,
    era=era_label,
    components=",".join(component_processes),
    eras=",".join(component_eras),
    nbins=nbins
  ))
  write_rule(f, "-")

  # Compare computed total_bkg nominal with stored tot_bkg, if present.
  if args.check_stored_total_bkg:
    comp_int, comp_int_err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map,
      component_eras, component_processes,
      None, "Nominal", nbins
    )
    stored_int, stored_int_err, missing_stored = stored_total_bkg_integral_for_eras(
      tfile, nominal_map, component_eras, nbins
    )

    if stored_int is None:
      f.write("  Stored {} check: no stored histograms found for eras({}).\n".format(
        STORED_TOTAL_BKG_NAME, ",".join(component_eras)
      ))
    else:
      diff = stored_int - comp_int
      rel = safe_divide(diff, comp_int, args.eps)
      flags = []
      if abs(diff) > args.stored_total_abs_threshold:
        flags.append("STORED_TOTAL_MISMATCH")
      row = {
        "process": STORED_TOTAL_BKG_NAME,
        "era": era_label,
        "syst": "computed-vs-stored-nominal",
        "computed": comp_int,
        "stored": stored_int,
        "diff": diff,
        "rel": rel,
        "missing_eras": ",".join(missing_stored) if missing_stored else "-",
        "flags": ",".join(flags),
        "severity": flag_severity(flags),
      }
      stored_total_check_rows.append(row)

      f.write("  Stored {stored_name} nominal check: computed={comp:.8g} ± {compe} stored={stored:.8g} ± {storede} diff={diff:.8g} rel={rel} missing_eras={missing} flags={flags}\n".format(
        stored_name=STORED_TOTAL_BKG_NAME,
        comp=comp_int,
        compe=fmt_float(comp_int_err),
        stored=stored_int,
        storede=fmt_float(stored_int_err),
        diff=diff,
        rel=fmt_float(rel),
        missing=",".join(missing_stored) if missing_stored else "-",
        flags=",".join(flags) if flags else "-"
      ))

  if not systs:
    f.write("  No shape variations found for total_bkg in this era selection.\n")
    return

  for syst in systs:
    if not keep_syst(args, syst):
      continue

    nom_int, nom_int_err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map,
      component_eras, component_processes,
      syst, "Nominal", nbins
    )
    up_int, up_int_err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map,
      component_eras, component_processes,
      syst, "Up", nbins
    )
    down_int, down_int_err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map,
      component_eras, component_processes,
      syst, "Down", nbins
    )

    data_int = data_obs_integral(data_obs, nbins) if use_data_alignment else None

    int_flags = classify_integral(nom_int, up_int, down_int, args)
    int_row = make_integral_row(
      TOTAL_BKG_NAME, era_label, syst,
      nom_int, up_int, down_int,
      nom_int_err, up_int_err, down_int_err,
      int_flags,
      "sum component Up if available else nominal",
      "sum component Down if available else nominal",
      args,
      total_nom=nom_int,
      data=data_int,
      data_ref_err=nom_int_err
    )

    if row_selected_for_problem_summary(int_row["flags"], args):
      integral_problem_rows.append(int_row)

    rows_this_syst = []

    for ibin in range(1, nbins + 1):
      low, high = bin_edges(ref_hist, ibin)

      nom, err_nom = total_bkg_bin_value_error(
        tfile, nominal_map, variation_map,
        component_eras, component_processes,
        syst, "Nominal", ibin
      )
      up, err_up = total_bkg_bin_value_error(
        tfile, nominal_map, variation_map,
        component_eras, component_processes,
        syst, "Up", ibin
      )
      down, err_down = total_bkg_bin_value_error(
        tfile, nominal_map, variation_map,
        component_eras, component_processes,
        syst, "Down", ibin
      )

      data = data_obs_bin_value(data_obs, ibin) if use_data_alignment else None

      base_flags = classify_bin(nom, up, down, args)

      row = make_bin_row(
        TOTAL_BKG_NAME, era_label, syst, ibin, low, high,
        nom, up, down,
        err_nom, err_up, err_down,
        base_flags,
        "sum component Up if available else nominal",
        "sum component Down if available else nominal",
        args,
        total_nom=nom,
        data=data,
        data_ref_err=err_nom,
        int_nom=nom_int,
        int_up=up_int,
        int_down=down_int
      )

      if args.focus_bin > 0 and ibin == args.focus_bin:
        focus_rows.append(row)

      if row_selected_for_problem_summary(row["flags"], args):
        problem_rows.append(row)

      if row_selected_for_detail(row["flags"], args):
        rows_this_syst.append(row)

    if args.full or rows_this_syst:
      f.write("\n  SYST = {syst}\n".format(syst=syst))
      f.write("    Up   = total_bkg: sum component Up if available, otherwise nominal\n")
      f.write("    Down = total_bkg: sum component Down if available, otherwise nominal\n")
      f.write("    Integral: nom={nom:15.8g} ± {nomerr:<11} up={up:>15} ± {uperr:<11} down={down:>15} ± {downerr:<11} rUp={rup:>12} rDown={rdown:>12} severity={sev} flags={flags}\n".format(
        nom=nom_int,
        nomerr=fmt_float(nom_int_err),
        up=fmt_float(up_int),
        uperr=fmt_float(up_int_err),
        down=fmt_float(down_int),
        downerr=fmt_float(down_int_err),
        rup=safe_ratio(up_int, nom_int, args.eps),
        rdown=safe_ratio(down_int, nom_int, args.eps),
        sev=int_row.get("severity", "ok"),
        flags=int_row["flags"] if int_row["flags"] else "-"
      ))
      write_bin_row_header(f, "    ")
      for row in rows_this_syst:
        write_bin_row(f, row, "    ", args=args)

def write_report(args, tfile, out_path):
  hist_names = get_key_names(tfile)
  nominal_map, variation_map, unknown_names = make_groups(hist_names)

  all_process_eras = sorted(set(list(nominal_map.keys()) + list(variation_map.keys())))

  problem_rows = []
  focus_rows = []
  integral_problem_rows = []
  structural_rows = []
  stored_total_check_rows = []
  nbins_seen = OrderedDict()

  total_nom_bin_cache = {}
  total_nom_int_cache = {}

  def get_total_nom_bin_cached(era, ibin):
    key = (era, ibin)
    if key in total_nom_bin_cache:
      return total_nom_bin_cache[key]

    component_processes = resolve_total_bkg_processes(args, nominal_map, (era,))
    val, err = total_bkg_bin_value_error(
      tfile, nominal_map, variation_map,
      (era,), component_processes,
      None, "Nominal", ibin
    )
    total_nom_bin_cache[key] = (val, err)
    return val, err

  def get_total_nom_int_cached(era, nbins):
    key = (era, nbins)
    if key in total_nom_int_cache:
      return total_nom_int_cache[key]

    component_processes = resolve_total_bkg_processes(args, nominal_map, (era,))
    val, err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map,
      (era,), component_processes,
      None, "Nominal", nbins
    )
    total_nom_int_cache[key] = (val, err)
    return val, err

  with open(out_path, "w") as f:
    f.write("# Combine input histogram variation scan\n")
    f.write("# input_file      : {}\n".format(args.input_root))
    f.write("# output_file     : {}\n".format(out_path))
    f.write("# focus_bin       : {}\n".format(args.focus_bin if args.focus_bin > 0 else "disabled"))
    f.write("# eps             : {}\n".format(args.eps))
    f.write("# rel_threshold   : {}\n".format(args.rel_threshold))
    f.write("# abs_threshold   : {}\n".format(args.abs_threshold))
    f.write("# asym_threshold  : {}\n".format(args.asym_threshold))
    f.write("# impact_threshold: {}\n".format(args.impact_threshold))
    f.write("# local_spike_threshold : {}\n".format(args.local_spike_threshold))
    f.write("# data_pull_threshold   : {}\n".format(args.data_pull_threshold))
    f.write("# min_severity    : {}\n".format(args.min_severity))
    f.write("# mode            : {}\n".format("full" if args.full else "problem-only"))
    f.write("# physics_processes : {}\n".format(args.physics_processes))
    f.write("# process_filter  : {}\n".format(",".join(args.process) if args.process else "all"))
    f.write("# era_filter      : {}\n".format(",".join(args.era) if args.era else "all"))
    f.write("# syst_filter     : exact={} contains={}\n".format(
      ",".join(args.syst) if args.syst else "all",
      ",".join(args.syst_contains) if args.syst_contains else "all"
    ))
    f.write("# flag_filter     : {}\n".format(",".join(args.flag) if args.flag else "all"))
    f.write("# add_total_bkg   : {}\n".format(args.add_total_bkg))
    f.write("# total_bkg_default_components : {}\n".format(",".join(TOTAL_BKG_PROCESSES)))
    f.write("# total_bkg_manual_components  : {}\n".format(",".join(args.total_bkg_process) if args.total_bkg_process else "auto"))
    f.write("# legacy_total_bkg : {}\n".format(args.legacy_total_bkg))
    f.write("# check_stored_total_bkg : {}\n".format(args.check_stored_total_bkg))
    f.write("# min_neff       : {}\n".format(args.min_neff))
    f.write("# max_rel_stat   : {}\n".format(args.max_rel_stat))
    write_rule(f, "=")

    f.write("\n[KEY SUMMARY]\n")
    f.write("  n_TH1_keys          : {}\n".format(len(hist_names)))
    f.write("  n_nominal_groups    : {}\n".format(len(nominal_map)))
    f.write("  n_process_era_total : {}\n".format(len(all_process_eras)))
    f.write("  n_unknown_names     : {}\n".format(len(unknown_names)))
    if unknown_names:
      f.write("  unknown examples    : {}\n".format(", ".join(unknown_names[:10])))
    write_rule(f)

    f.write("\n[DATA_OBS]\n")
    data_obs = get_hist(tfile, "data_obs")
    if data_obs:
      f.write("  nbins = {}, integral = {:.8g}\n".format(data_obs.GetNbinsX(), data_obs.Integral(1, data_obs.GetNbinsX())))
      f.write("  {bin:>4} {low:>13} {high:>13} {content:>15}\n".format(
        bin="bin", low="xlow", high="xhigh", content="data_obs"))
      for ibin in range(1, data_obs.GetNbinsX() + 1):
        low, high = bin_edges(data_obs, ibin)
        f.write("  {bin:4d} {low:13.6g} {high:13.6g} {content:15.8g}\n".format(
          bin=ibin, low=low, high=high, content=data_obs.GetBinContent(ibin)))
    else:
      f.write("  data_obs not found\n")
    write_rule(f)

    # First pass: collect rows and immediately write detailed blocks if requested.
    f.write("\n[DETAILED SCAN]\n")
    f.write("Legend: flags are assigned per bin. Severity order is fatal > risk > warning > info.\n")
    f.write("  fatal   : NEG_*, MISS_UP/DOWN, missing nominal, binning mismatch, non-finite values.\n")
    f.write("  risk    : one-sided surviving templates, both variations same side, large total-bkg impact, data-pull issues.\n")
    f.write("  warning : huge relative changes, low Neff/high rel stat, zero-content nonzero-error, local spikes.\n")
    f.write("  info    : NO_EFFECT, ONE_SIDED_EFFECT, nuisance direction moves total_bkg toward data.\n")
    f.write("  NO_EFFECT is normally harmless and suppressed unless --include-no-effect is used.\n")
    write_rule(f)

    for process, era in all_process_eras:
      nom_name = nominal_map.get((process, era))
      nom_hist = get_hist(tfile, nom_name) if nom_name else None

      if not nom_hist:
        add_structural_issue(
          structural_rows,
          process, era, "*", "MISSING_NOMINAL",
          nom_name, "", ""
        )
        continue

      nbins = nom_hist.GetNbinsX()
      nbins_seen[(process, era)] = nbins

      systs = sorted(variation_map.get((process, era), {}).keys())

      if not keep_process(args, process):
        continue
      if not keep_era(args, era):
        continue

      f.write("\nPROCESS = {process}   ERA = {era}   NOMINAL = {nom}   NBINS = {nbins}\n".format(
        process=process, era=era, nom=nom_name, nbins=nbins))
      write_rule(f, "-")

      if not systs:
        f.write("  No shape variations found for this process/era.\n")
        continue

      total_nom_int = None
      if args.add_total_bkg:
        total_nom_int, _ = get_total_nom_int_cached(era, nbins)

      for syst in systs:
        if not keep_syst(args, syst):
          continue

        pair = variation_map[(process, era)][syst]
        up_name = pair.get("Up")
        down_name = pair.get("Down")
        up_hist = get_hist(tfile, up_name) if up_name else None
        down_hist = get_hist(tfile, down_name) if down_name else None

        if up_hist and up_hist.GetNbinsX() != nbins:
          add_structural_issue(
            structural_rows,
            process, era, syst, "UP_NBINS_MISMATCH",
            nom_name, up_name, down_name
          )
        if down_hist and down_hist.GetNbinsX() != nbins:
          add_structural_issue(
            structural_rows,
            process, era, syst, "DOWN_NBINS_MISMATCH",
            nom_name, up_name, down_name
          )
        if up_hist and not same_binning(nom_hist, up_hist, 1.0e-9):
          add_structural_issue(
            structural_rows,
            process, era, syst, "UP_BIN_EDGE_MISMATCH",
            nom_name, up_name, down_name
          )
        if down_hist and not same_binning(nom_hist, down_hist, 1.0e-9):
          add_structural_issue(
            structural_rows,
            process, era, syst, "DOWN_BIN_EDGE_MISMATCH",
            nom_name, up_name, down_name
          )

        nom_int, nom_int_err = hist_integral_content_error(nom_hist, nbins)
        up_int, up_int_err = hist_integral_content_error(up_hist, min(nbins, up_hist.GetNbinsX())) if up_hist else (None, None)
        down_int, down_int_err = hist_integral_content_error(down_hist, min(nbins, down_hist.GetNbinsX())) if down_hist else (None, None)

        int_flags = classify_integral(nom_int, up_int, down_int, args)
        int_row = make_integral_row(
          process, era, syst,
          nom_int, up_int, down_int,
          nom_int_err, up_int_err, down_int_err,
          int_flags,
          up_name, down_name,
          args,
          total_nom=total_nom_int
        )

        if row_selected_for_problem_summary(int_row["flags"], args):
          integral_problem_rows.append(int_row)

        rows_this_syst = []
        for ibin in range(1, nbins + 1):
          low, high = bin_edges(nom_hist, ibin)
          nom = float(nom_hist.GetBinContent(ibin))
          up = float(up_hist.GetBinContent(ibin)) if up_hist and ibin <= up_hist.GetNbinsX() else None
          down = float(down_hist.GetBinContent(ibin)) if down_hist and ibin <= down_hist.GetNbinsX() else None

          err_nom = hist_bin_error(nom_hist, ibin)
          err_up = hist_bin_error(up_hist, ibin) if up_hist and ibin <= up_hist.GetNbinsX() else None
          err_down = hist_bin_error(down_hist, ibin) if down_hist and ibin <= down_hist.GetNbinsX() else None

          total_nom_bin = None
          if args.add_total_bkg:
            total_nom_bin, _ = get_total_nom_bin_cached(era, ibin)

          base_flags = classify_bin(nom, up, down, args)

          row = make_bin_row(
            process, era, syst, ibin, low, high,
            nom, up, down,
            err_nom, err_up, err_down,
            base_flags,
            up_name, down_name,
            args,
            total_nom=total_nom_bin,
            int_nom=nom_int,
            int_up=up_int,
            int_down=down_int
          )

          if args.focus_bin > 0 and ibin == args.focus_bin:
            focus_rows.append(row)

          if row_selected_for_problem_summary(row["flags"], args):
            problem_rows.append(row)

          if row_selected_for_detail(row["flags"], args):
            rows_this_syst.append(row)

        if args.full or rows_this_syst:
          f.write("\n  SYST = {syst}\n".format(syst=syst))
          f.write("    Up   = {}\n".format(up_name if up_name else "MISSING"))
          f.write("    Down = {}\n".format(down_name if down_name else "MISSING"))
          f.write("    Integral: nom={nom:15.8g} ± {nomerr:<11} up={up:>15} ± {uperr:<11} down={down:>15} ± {downerr:<11} rUp={rup:>12} rDown={rdown:>12} severity={sev} flags={flags}\n".format(
            nom=nom_int,
            nomerr=fmt_float(nom_int_err),
            up=fmt_float(up_int),
            uperr=fmt_float(up_int_err),
            down=fmt_float(down_int),
            downerr=fmt_float(down_int_err),
            rup=safe_ratio(up_int, nom_int, args.eps),
            rdown=safe_ratio(down_int, nom_int, args.eps),
            sev=int_row.get("severity", "ok"),
            flags=int_row["flags"] if int_row["flags"] else "-"
          ))
          write_bin_row_header(f, "    ")
          for row in rows_this_syst:
            write_bin_row(f, row, "    ", args=args)

    if args.add_total_bkg and keep_process(args, TOTAL_BKG_NAME):
      # Era-specific computed total_bkg. This respects --era.
      for era in ERAS:
        if not keep_era(args, era):
          continue

        write_total_bkg_block(
          f, args, tfile, nominal_map, variation_map,
          era_label=era,
          component_eras=(era,),
          syst_source_eras=(era,),
          problem_rows=problem_rows,
          focus_rows=focus_rows,
          integral_problem_rows=integral_problem_rows,
          structural_rows=structural_rows,
          stored_total_check_rows=stored_total_check_rows,
          nbins_seen=nbins_seen
        )

      # Full Run2 computed total_bkg.
      # This does NOT respect --era for the summed components:
      # it always sums all eras, so it can be compared to data_obs.
      #
      # However, if --era is specified, use that era only to choose
      # which nuisance names to scan. Example:
      #   --era 2018 --syst-contains scale_j
      # gives Run2 total_bkg for the 2018 scale_j nuisance:
      #   2018 varied, other eras nominal.
      selected_syst_eras = tuple([era for era in args.era if era in ERAS]) if args.era else ERAS
      if not selected_syst_eras:
        selected_syst_eras = ERAS

      write_total_bkg_block(
        f, args, tfile, nominal_map, variation_map,
        era_label=RUN2_TOTAL_BKG_ERA,
        component_eras=ERAS,
        syst_source_eras=selected_syst_eras,
        problem_rows=problem_rows,
        focus_rows=focus_rows,
        integral_problem_rows=integral_problem_rows,
        structural_rows=structural_rows,
        stored_total_check_rows=stored_total_check_rows,
        nbins_seen=nbins_seen
      )

    # Summary sections are written at the end because the scan is single-pass.
    f.write("\n")
    write_rule(f, "=")
    f.write("\n[SUMMARY AFTER SCAN]\n")
    f.write("  n_integral_problem_rows : {}\n".format(len(integral_problem_rows)))
    f.write("  n_bin_problem_rows      : {}\n".format(len(problem_rows)))
    f.write("  n_focus_bin_rows        : {}\n".format(len(focus_rows)))
    f.write("  n_structural_rows       : {}\n".format(len(structural_rows)))
    f.write("  n_stored_total_checks   : {}\n".format(len(stored_total_check_rows)))
    f.write("  nbins by process/era    :\n")
    for key, nb in nbins_seen.items():
      f.write("    {}/{} : {}\n".format(key[0], key[1], nb))

    if structural_rows:
      write_rule(f)
      f.write("\n[MISSING / STRUCTURAL ISSUES]\n")
      f.write("{proc:<18} {era:<12} {syst:<55} {issue:<35} {up:<70} {down:<70}\n".format(
        proc="process", era="era", syst="syst", issue="issue", up="up_name", down="down_name"))
      for row in structural_rows:
        f.write("{process:<18} {era:<12} {syst:<55} {issue:<35} {up_name:<70} {down_name:<70}\n".format(**row))

    if stored_total_check_rows:
      write_rule(f)
      f.write("\n[STORED tot_bkg CHECK]\n")
      f.write("{proc:<18} {era:<12} {syst:<30} {computed:>14} {stored:>14} {diff:>14} {rel:>11} {missing:<30} {sev:>8} {flags}\n".format(
        proc="process", era="era", syst="check", computed="computed", stored="stored",
        diff="stored-comp", rel="rel", missing="missing_eras", sev="severity", flags="flags"
      ))
      for row in stored_total_check_rows:
        if args.flag and not flags_match(row["flags"], args.flag):
          continue
        if row["flags"] and not row_is_problem(row["flags"], args):
          continue
        f.write("{process:<18} {era:<12} {syst:<30} {computed:14.8g} {stored:14.8g} {diff:14.8g} {rel:>11} {missing_eras:<30} {severity:>8} {flags}\n".format(
          process=row["process"], era=row["era"], syst=row["syst"],
          computed=row["computed"], stored=row["stored"], diff=row["diff"],
          rel=fmt_float(row["rel"]),
          missing_eras=row["missing_eras"],
          severity=row["severity"],
          flags=row["flags"] if row["flags"] else "-"
        ))

    if args.focus_bin > 0:
      write_rule(f)
      f.write("\n[FOCUS BIN {}]\n".format(args.focus_bin))
      f.write("{proc:<18} {era:<12} {syst:<55} ".format(
        proc="process", era="era", syst="syst"))
      write_bin_row_header(f)
      for row in focus_rows:
        if not flags_match(row["flags"], args.flag):
          continue

        # In problem-only mode, show all focus-bin rows anyway, because this is
        # the bin under investigation.  --focus-problem-only restores filtering.
        if (not args.full) and args.focus_problem_only and not row_is_problem(row["flags"], args):
          continue
        f.write("{process:<18} {era:<12} {syst:<55} ".format(
          process=row["process"], era=row["era"], syst=row["syst"]))
        write_bin_row(f, row, args=args)

    if integral_problem_rows:
      write_rule(f)
      f.write("\n[INTEGRAL-LEVEL PROBLEMS]\n")
      write_integral_row_header(f)
      for row in integral_problem_rows:
        write_integral_row(f, row, args=args)

    if problem_rows:
      write_rule(f)
      f.write("\n[ALL BIN-LEVEL PROBLEMS]\n")
      f.write("{proc:<18} {era:<12} {syst:<55} ".format(
        proc="process", era="era", syst="syst"))
      write_bin_row_header(f)
      for row in problem_rows:
        f.write("{process:<18} {era:<12} {syst:<55} ".format(
          process=row["process"], era=row["era"], syst=row["syst"]))
        write_bin_row(f, row, args=args)

  return {
    "n_hists": len(hist_names),
    "n_nominal": len(nominal_map),
    "n_unknown": len(unknown_names),
    "n_problem_bins": len(problem_rows),
    "n_problem_integrals": len(integral_problem_rows),
    "n_missing": len(structural_rows),
    "n_stored_total_checks": len(stored_total_check_rows),
    "out_path": out_path
  }


def flatten_option_values(values):
  """
  Normalize argparse list options.

  Supports all of these:
    --process wz zz fake
    --process wz --process zz --process fake
    --process wz,zz,fake
  """
  out = []

  if not values:
    return out

  for group in values:
    if group is None:
      continue

    if isinstance(group, str):
      group = [group]

    for item in group:
      if item is None:
        continue
      for token in str(item).split(","):
        token = token.strip()
        if token:
          out.append(token)

  # preserve order, remove duplicates
  dedup = []
  for x in out:
    if x not in dedup:
      dedup.append(x)

  return dedup


def parse_args():
  parser = argparse.ArgumentParser(
    description="Scan Combine input ROOT histograms bin-by-bin for pathological shape variations."
  )
  parser.add_argument("input_root", help="Combine input ROOT file, e.g. M1000_EE_card_input.root")
  parser.add_argument("-o", "--out", default=None, help="Output txt path")
  parser.add_argument("--focus-bin", type=int, default=8, help="Bin to summarize explicitly. Use 0 to disable.")
  parser.add_argument("--full", action="store_true",
                      help="Dump every bin for every variation. Default dumps only severity-selected rows plus summaries.")
  parser.add_argument("--problem-only", action="store_true",
                      help="Alias for default behavior. Kept for readability.")
  parser.add_argument("--focus-problem-only", action="store_true",
                      help="In the focus-bin summary, print only flagged rows.")
  parser.add_argument("--include-no-effect", action="store_true",
                      help="Treat NO_EFFECT rows as info-level problems. Usually not needed.")

  parser.add_argument("--eps", type=float, default=1.0e-12,
                      help="Absolute epsilon for zero/equality checks.")
  parser.add_argument("--rel-threshold", type=float, default=1.0,
                      help="Flag relative changes >= this value when nominal is nonzero. Default 1.0 = 100%%.")
  parser.add_argument("--abs-threshold", type=float, default=1.0e-6,
                      help="Flag absolute changes >= this value when nominal is zero.")
  parser.add_argument("--asym-threshold", type=float, default=5.0,
                      help="Flag ASYM_LARGE if |Up-nom| and |Down-nom| differ by at least this factor. Use 0 to disable.")
  parser.add_argument("--impact-threshold", type=float, default=0.05,
                      help="Flag LARGE_TOTAL_IMPACT_* if |variation-nominal|/total_bkg >= this value. Use 0 to disable.")
  parser.add_argument("--local-spike-threshold", type=float, default=5.0,
                      help="Flag LOCAL_SPIKE_* if bin-level relative effect is this factor larger than integral-level effect. Use 0 to disable.")
  parser.add_argument("--data-pull-threshold", type=float, default=2.0,
                      help="For computed Run2 total_bkg, flag DATA_PULL_HIGH when |data-total_bkg|/err(total_bkg) exceeds this value.")
  parser.add_argument("--stored-total-abs-threshold", type=float, default=1.0e-8,
                      help="Absolute threshold for computed total_bkg vs stored tot_bkg mismatch.")
  parser.add_argument("--same-side-min-rel", type=float, default=0.02,
                      help="Only flag BOTH_ABOVE_NOM/BOTH_BELOW_NOM if max |var-nom|/|nom| is at least this value. Default: 0.02.")
  parser.add_argument("--asym-min-rel", type=float, default=0.02,
                      help="Only flag ASYM_LARGE if max |var-nom|/|nom| is at least this value. Default: 0.02.")
  parser.add_argument("--data-pull-mode", choices=["poisson", "mcstat", "none"], default="poisson",
                      help="Denominator for DATA_PULL_HIGH. poisson uses sqrt(expected + MCstat^2).")
  parser.add_argument("--flag-verbosity", choices=["compact", "full", "none"], default="compact",
                      help="How many flags to print. Raw flags are still used internally.")
  parser.add_argument("--max-flags", type=int, default=4,
                      help="Maximum number of displayed flags in compact mode. Use <=0 to show all compact-selected flags.")

  parser.add_argument("--min-severity", choices=["info", "warning", "risk", "fatal"], default="warning",
                      help="Minimum severity printed in problem-only summaries. Default: warning.")
  parser.add_argument("--physics-processes", action="store_true",
                      help="Keep only main analysis processes: datacard backgrounds, signal*, and computed total_bkg.")
  parser.add_argument("--process", action="append", nargs="+", default=[],
                      help="Restrict to process names, e.g. --process wz zz fake or --process wz,zz,fake")
  parser.add_argument("--era", action="append", nargs="+", default=[],
                      help="Restrict to eras, e.g. --era 2016preVFP 2017 2018 or --era 2016preVFP,2018")
  parser.add_argument("--syst", action="append", nargs="+", default=[],
                      help="Restrict to exact systematic names, e.g. --syst CMS_scale_j_2018_sr2 CMS_res_j_2018_sr2")
  parser.add_argument("--syst-contains", action="append", nargs="+", default=[],
                      help="Restrict to systematics containing substrings, e.g. --syst-contains scale_j res_j")
  parser.add_argument("--flag", action="append", nargs="+", default=[],
                      help="Restrict printed rows to flags, e.g. --flag ONLY_UP_SURVIVES DATA_PULL_HIGH")

  parser.add_argument("--add-total-bkg", action="store_true",
                      help="Add computed total_bkg pseudo-process. Default components are fake, cf, zg, zz, wz, wz_ewk, ww, mc_others.")
  parser.add_argument("--legacy-total-bkg", action="store_true",
                      help="For older inputs, compute total_bkg using conv_others+prompt_others instead of mc_others.")
  parser.add_argument("--total-bkg-process", action="append", nargs="+", default=[],
                      help="Manually specify total_bkg components, e.g. --total-bkg-process fake cf zg zz wz wz_ewk ww mc_others")
  parser.add_argument("--check-stored-total-bkg", action="store_true",
                      help="When --add-total-bkg is used, compare computed total_bkg nominal with stored tot_bkg histograms if present.")

  parser.add_argument("--min-neff", type=float, default=10.0,
                      help="Flag bins with effective entries below this value. Use 0 to disable. Default: 10.")
  parser.add_argument("--max-rel-stat", type=float, default=-1.0,
                      help="Flag bins with relative stat error above this value. Disabled by default. Example: 1.0 means 100%%.")
  args = parser.parse_args()

  args.process = flatten_option_values(args.process)
  args.era = flatten_option_values(args.era)
  args.syst = flatten_option_values(args.syst)
  args.syst_contains = flatten_option_values(args.syst_contains)
  args.flag = flatten_option_values(args.flag)
  args.total_bkg_process = flatten_option_values(args.total_bkg_process)

  return args


def main():
  global ROOT

  args = parse_args()

  if not os.path.exists(args.input_root):
    sys.stderr.write("ERROR: input ROOT file does not exist: {}\n".format(args.input_root))
    return 2

  try:
    import ROOT
    ROOT.gROOT.SetBatch(True)
  except Exception as exc:
    sys.stderr.write("ERROR: failed to import ROOT/PyROOT. Run inside a ROOT/CMSSW environment.\n")
    sys.stderr.write("       {}\n".format(exc))
    return 2

  tfile = ROOT.TFile.Open(args.input_root, "READ")
  if not tfile or tfile.IsZombie():
    sys.stderr.write("ERROR: failed to open ROOT file: {}\n".format(args.input_root))
    return 2

  out_path = args.out
  if out_path is None:
    base = os.path.basename(args.input_root)
    if base.endswith(".root"):
      base = base[:-5]
    suffix = "_full" if args.full else "_problems"
    out_path = base + "_histVariationScan" + suffix + ".txt"

  result = write_report(args, tfile, out_path)
  tfile.Close()

  print("Wrote:", result["out_path"])
  print("TH1 keys:", result["n_hists"])
  print("nominal groups:", result["n_nominal"])
  print("bin-level problem rows:", result["n_problem_bins"])
  print("integral-level problem rows:", result["n_problem_integrals"])
  print("missing/structural rows:", result["n_missing"])
  print("stored total checks:", result.get("n_stored_total_checks", 0))

  return 0


if __name__ == "__main__":
  sys.exit(main())
