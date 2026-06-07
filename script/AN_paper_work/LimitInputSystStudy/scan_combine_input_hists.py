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

PHYSICS_PROCESSES = (
  "fake",
  "cf",
  "wz",
  "ww",
  "zz",
  "zg",
  "wz_ewk",
  #"mc_others",
  "conv_others",
  "prompt_others",
  #"signalDYVBF",
  "signalDY",
  "signalVBF",
  "signalSSWW",
  "total_bkg",
)

TOTAL_BKG_NAME = "total_bkg"
RUN2_TOTAL_BKG_ERA = "Run2"

TOTAL_BKG_PROCESSES = (
  "fake",
  "cf",
  "wz",
  "ww",
  "zz",
  "zg",
  "wz_ewk",
  #"mc_others",
  "conv_others",
  "prompt_others",
)

def is_close(a, b, eps):
  return abs(a - b) <= eps


def safe_ratio(x, nom, eps):
  if abs(nom) <= eps:
    if abs(x) <= eps:
      return "0/0"
    return "+inf" if x > 0 else "-inf"
  return "{:.6g}".format(x / nom)


def fmt_float(x):
  if x is None:
    return "NA"
  if isinstance(x, float) and (math.isnan(x) or math.isinf(x)):
    return str(x)
  return "{:.8g}".format(x)


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


def classify_bin(nom, up, down, eps, rel_threshold, abs_threshold):
  flags = []

  vals = [("nom", nom), ("up", up), ("down", down)]
  for label, val in vals:
    if val is not None and val < -eps:
      flags.append("NEG_" + label.upper())

  if up is None:
    flags.append("MISS_UP")
  if down is None:
    flags.append("MISS_DOWN")
  if up is None or down is None:
    return flags

  nom0 = abs(nom) <= eps
  up0 = abs(up) <= eps
  down0 = abs(down) <= eps

  if nom0 and (not up0 or not down0):
    flags.append("ZERO_NOM_NONZERO_VAR")
  if nom0 and down0 and not up0:
    flags.append("ONLY_UP_SURVIVES")
  if nom0 and up0 and not down0:
    flags.append("ONLY_DOWN_SURVIVES")

  if is_close(up, nom, eps) and is_close(down, nom, eps):
    flags.append("NO_EFFECT")
  elif is_close(up, nom, eps) or is_close(down, nom, eps):
    flags.append("ONE_SIDED_EFFECT")

  if up > nom + eps and down > nom + eps:
    flags.append("BOTH_ABOVE_NOM")
  if up < nom - eps and down < nom - eps:
    flags.append("BOTH_BELOW_NOM")

  if abs(nom) > eps:
    rel_up = abs((up - nom) / nom)
    rel_down = abs((down - nom) / nom)
    if rel_up >= rel_threshold:
      flags.append("HUGE_REL_UP")
    if rel_down >= rel_threshold:
      flags.append("HUGE_REL_DOWN")
  else:
    if abs(up - nom) >= abs_threshold:
      flags.append("HUGE_ABS_UP_AT_ZERO_NOM")
    if abs(down - nom) >= abs_threshold:
      flags.append("HUGE_ABS_DOWN_AT_ZERO_NOM")

  return flags


def classify_integral(nom_int, up_int, down_int, eps, rel_threshold, abs_threshold):
  return classify_bin(nom_int, up_int, down_int, eps, rel_threshold, abs_threshold)


def row_is_problem(flags, include_no_effect):
  non_problem = set(["NO_EFFECT"])
  if include_no_effect:
    return len(flags) > 0
  return len([f for f in flags if f not in non_problem]) > 0

def flags_match(flags, wanted_flags):
  """
  Return True if this row should pass the --flag filter.

  wanted_flags is args.flag, e.g.
    ["ONLY_UP_SURVIVES", "ZERO_NOM_NONZERO_VAR"]

  If no --flag is given, do not filter by flag.
  """
  if not wanted_flags:
    return True

  if flags is None:
    flags = []

  if isinstance(flags, str):
    if flags == "" or flags == "-":
      flags = []
    else:
      flags = flags.split(",")

  return any(flag in wanted_flags for flag in flags)


def row_selected_for_problem_summary(flags, args):
  """
  Decide whether a row should enter the summary problem tables.

  If --flag is given, use only the requested flags.
  Otherwise use the normal problem definition.
  """
  if args.flag:
    return flags_match(flags, args.flag)
  return row_is_problem(flags, args.include_no_effect)


def row_selected_for_detail(flags, args):
  """
  Decide whether a row should be printed in the detailed scan.

  In --full mode:
    print all rows unless --flag restricts them.

  In default problem-only mode:
    print only problem rows unless --flag restricts them.
  """
  if args.flag:
    return flags_match(flags, args.flag)

  if args.full:
    return True

  return row_is_problem(flags, args.include_no_effect)


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

  if error is None:
    return flags

  if abs(content) > args.eps and abs(error) <= args.eps:
    flags.append("ZERO_ERR_NONZERO_" + label)

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
                 args):
  flags = list(base_flags)

  flags.extend(classify_stat_value("NOM", nom, err_nom, args))
  if up is not None:
    flags.extend(classify_stat_value("UP", up, err_up, args))
  if down is not None:
    flags.extend(classify_stat_value("DOWN", down, err_down, args))

  neff_nom = effective_entries(nom, err_nom, args.eps)
  neff_up = effective_entries(up, err_up, args.eps) if up is not None else None
  neff_down = effective_entries(down, err_down, args.eps) if down is not None else None

  return {
    "process": process, "era": era, "syst": syst, "bin": ibin,
    "xlow": low, "xhigh": high,
    "nom": nom, "up": up, "down": down,
    "d_up": None if up is None else up - nom,
    "d_down": None if down is None else down - nom,
    "r_up": safe_ratio(up, nom, args.eps) if up is not None else "NA",
    "r_down": safe_ratio(down, nom, args.eps) if down is not None else "NA",
    "err_nom": err_nom,
    "err_up": err_up,
    "err_down": err_down,
    "neff_nom": neff_nom,
    "neff_up": neff_up,
    "neff_down": neff_down,
    "flags": ",".join(flags),
    "up_name": str(up_name),
    "down_name": str(down_name)
  }

def get_total_bkg_ref_hist(tfile, nominal_map, era):
  """
  Return a reference nominal histogram for total_bkg in this era.
  This is used only for binning and x-axis information.
  """
  for process in TOTAL_BKG_PROCESSES:
    hname = nominal_map.get((process, era))
    hist = get_hist(tfile, hname) if hname else None
    if hist:
      return hist
  return None


def get_total_bkg_systs(variation_map, era):
  """
  Return the union of systematics available for the background processes.
  """
  systs = set()
  for process in TOTAL_BKG_PROCESSES:
    systs.update(variation_map.get((process, era), {}).keys())
  return sorted(systs)


def total_bkg_bin_content(tfile, nominal_map, variation_map, era, syst, side, ibin):
  """
  Sum background bin contents.

  For nominal:
    sum process nominal.

  For Up/Down:
    use process_systUp/Down if it exists;
    otherwise use that process nominal.
  This is important because not every nuisance affects every process.
  """
  total = 0.0

  for process in TOTAL_BKG_PROCESSES:
    nom_name = nominal_map.get((process, era))
    nom_hist = get_hist(tfile, nom_name) if nom_name else None
    if not nom_hist:
      continue

    hist = nom_hist

    if side in ("Up", "Down"):
      var_name = variation_map.get((process, era), {}).get(syst, {}).get(side)
      var_hist = get_hist(tfile, var_name) if var_name else None
      if var_hist:
        hist = var_hist

    if ibin <= hist.GetNbinsX():
      total += float(hist.GetBinContent(ibin))

  return total


def total_bkg_integral(tfile, nominal_map, variation_map, era, syst, side, nbins):
  total = 0.0
  for ibin in range(1, nbins + 1):
    total += total_bkg_bin_content(tfile, nominal_map, variation_map, era, syst, side, ibin)
  return total

def get_total_bkg_ref_hist_for_eras(tfile, nominal_map, eras):
  """
  Return a reference histogram for binning/x-axis.
  For Run2 total_bkg, this just needs to be any valid component hist.
  """
  for era in eras:
    for process in TOTAL_BKG_PROCESSES:
      hname = nominal_map.get((process, era))
      hist = get_hist(tfile, hname) if hname else None
      if hist:
        return hist
  return None


def get_total_bkg_systs_for_eras(variation_map, eras):
  """
  Return the union of systematics for total_bkg from selected era(s).

  Important:
    For Run2 total_bkg, the summed content can use all eras,
    but the syst list can be restricted to the selected era.
    Example: --era 2018 --syst-contains scale_j
      -> Run2 total_bkg is sum of all eras,
         but only 2018 scale_j nuisance is scanned.
  """
  systs = set()
  for era in eras:
    for process in TOTAL_BKG_PROCESSES:
      systs.update(variation_map.get((process, era), {}).keys())
  return sorted(systs)


def total_bkg_bin_value_error(tfile, nominal_map, variation_map,
                              component_eras, syst, side, ibin):
  """
  Sum total_bkg over requested eras and background processes.

  Content:
    sum bin contents.

  Error:
    quadrature sum of component bin errors,
    assuming independent MC/stat samples.
  """
  total = 0.0
  err2 = 0.0

  for era in component_eras:
    for process in TOTAL_BKG_PROCESSES:
      nom_name = nominal_map.get((process, era))
      nom_hist = get_hist(tfile, nom_name) if nom_name else None
      if not nom_hist:
        continue

      hist = nom_hist

      if side in ("Up", "Down"):
        var_name = variation_map.get((process, era), {}).get(syst, {}).get(side)
        var_hist = get_hist(tfile, var_name) if var_name else None
        if var_hist:
          hist = var_hist

      if ibin <= hist.GetNbinsX():
        val = float(hist.GetBinContent(ibin))
        err = float(hist.GetBinError(ibin))
        total += val
        err2 += err * err

  return total, math.sqrt(err2)


def total_bkg_integral_for_eras(tfile, nominal_map, variation_map,
                                component_eras, syst, side, nbins):
  total = 0.0
  err2 = 0.0

  for ibin in range(1, nbins + 1):
    val, err = total_bkg_bin_value_error(
      tfile, nominal_map, variation_map,
      component_eras, syst, side, ibin
    )
    total += val
    err2 += err * err

  return total, math.sqrt(err2)


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


def write_rule(f, char="-", n=160):
  f.write(char * n + "\n")

def write_bin_row_header(f, prefix=""):
  f.write(prefix + "{bin:>4} {xlow:>12} {xhigh:>12} {nom:>14} {up:>14} {down:>14} "
           "{dup:>14} {ddown:>14} {rup:>11} {rdown:>11} "
           "{errn:>11} {erru:>11} {errd:>11} "
           "{neffn:>10} {neffu:>10} {neffd:>10}  {flags}\n".format(
    bin="bin", xlow="xlow", xhigh="xhigh",
    nom="nom", up="up", down="down",
    dup="up-nom", ddown="down-nom",
    rup="up/nom", rdown="down/nom",
    errn="errN", erru="errU", errd="errD",
    neffn="neffN", neffu="neffU", neffd="neffD",
    flags="flags"
  ))


def write_bin_row(f, row, prefix=""):
  f.write(prefix + "{bin:4d} {xlow:12.6g} {xhigh:12.6g} {nom:14.8g} {up:>14} {down:>14} "
           "{dup:>14} {ddown:>14} {rup:>11} {rdown:>11} "
           "{errn:>11} {erru:>11} {errd:>11} "
           "{neffn:>10} {neffu:>10} {neffd:>10}  {flags}\n".format(
    bin=row["bin"],
    xlow=row["xlow"],
    xhigh=row["xhigh"],
    nom=row["nom"],
    up=fmt_float(row["up"]),
    down=fmt_float(row["down"]),
    dup=fmt_float(row["d_up"]),
    ddown=fmt_float(row["d_down"]),
    rup=row["r_up"],
    rdown=row["r_down"],
    errn=fmt_float(row.get("err_nom")),
    erru=fmt_float(row.get("err_up")),
    errd=fmt_float(row.get("err_down")),
    neffn=fmt_float(row.get("neff_nom")),
    neffu=fmt_float(row.get("neff_up")),
    neffd=fmt_float(row.get("neff_down")),
    flags=row["flags"] if row["flags"] else "-"
  ))

def write_total_bkg_block(f, args, tfile, nominal_map, variation_map,
                          era_label, component_eras, syst_source_eras,
                          problem_rows, focus_rows, integral_problem_rows,
                          nbins_seen):
  ref_hist = get_total_bkg_ref_hist_for_eras(tfile, nominal_map, component_eras)
  if not ref_hist:
    return

  nbins = ref_hist.GetNbinsX()
  nbins_seen[(TOTAL_BKG_NAME, era_label)] = nbins

  systs = get_total_bkg_systs_for_eras(variation_map, syst_source_eras)

  f.write("\nPROCESS = {process}   ERA = {era}   NOMINAL = sum({components}) over eras({eras})   NBINS = {nbins}\n".format(
    process=TOTAL_BKG_NAME,
    era=era_label,
    components=",".join(TOTAL_BKG_PROCESSES),
    eras=",".join(component_eras),
    nbins=nbins
  ))
  write_rule(f, "-")

  if not systs:
    f.write("  No shape variations found for total_bkg in this era selection.\n")
    return

  for syst in systs:
    if not keep_syst(args, syst):
      continue

    nom_int, nom_int_err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map, component_eras, syst, "Nominal", nbins
    )
    up_int, up_int_err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map, component_eras, syst, "Up", nbins
    )
    down_int, down_int_err = total_bkg_integral_for_eras(
      tfile, nominal_map, variation_map, component_eras, syst, "Down", nbins
    )

    int_flags = classify_integral(
      nom_int, up_int, down_int,
      args.eps, args.rel_threshold, args.abs_threshold
    )

    # Integral-level stat flags, using the total integral error.
    int_flags.extend(classify_stat_value("NOM", nom_int, nom_int_err, args))
    int_flags.extend(classify_stat_value("UP", up_int, up_int_err, args))
    int_flags.extend(classify_stat_value("DOWN", down_int, down_int_err, args))

    if row_selected_for_problem_summary(int_flags, args):
      integral_problem_rows.append({
        "process": TOTAL_BKG_NAME, "era": era_label, "syst": syst,
        "nom": nom_int, "up": up_int, "down": down_int,
        "r_up": safe_ratio(up_int, nom_int, args.eps),
        "r_down": safe_ratio(down_int, nom_int, args.eps),
        "flags": ",".join(int_flags),
        "up_name": "sum component Up if available else nominal",
        "down_name": "sum component Down if available else nominal"
      })

    rows_this_syst = []

    for ibin in range(1, nbins + 1):
      low, high = bin_edges(ref_hist, ibin)

      nom, err_nom = total_bkg_bin_value_error(
        tfile, nominal_map, variation_map,
        component_eras, syst, "Nominal", ibin
      )
      up, err_up = total_bkg_bin_value_error(
        tfile, nominal_map, variation_map,
        component_eras, syst, "Up", ibin
      )
      down, err_down = total_bkg_bin_value_error(
        tfile, nominal_map, variation_map,
        component_eras, syst, "Down", ibin
      )

      base_flags = classify_bin(
        nom, up, down,
        args.eps, args.rel_threshold, args.abs_threshold
      )

      row = make_bin_row(
        TOTAL_BKG_NAME, era_label, syst, ibin, low, high,
        nom, up, down,
        err_nom, err_up, err_down,
        base_flags,
        "sum component Up if available else nominal",
        "sum component Down if available else nominal",
        args
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
      f.write("    Integral: nom={nom:15.8g} ± {nomerr:<11} up={up:>15} ± {uperr:<11} down={down:>15} ± {downerr:<11} rUp={rup:>12} rDown={rdown:>12} flags={flags}\n".format(
        nom=nom_int,
        nomerr=fmt_float(nom_int_err),
        up=fmt_float(up_int),
        uperr=fmt_float(up_int_err),
        down=fmt_float(down_int),
        downerr=fmt_float(down_int_err),
        rup=safe_ratio(up_int, nom_int, args.eps),
        rdown=safe_ratio(down_int, nom_int, args.eps),
        flags=",".join(int_flags) if int_flags else "-"
      ))
      write_bin_row_header(f, "    ")
      for row in rows_this_syst:
        write_bin_row(f, row, "    ")

def write_report(args, tfile, out_path):
  hist_names = get_key_names(tfile)
  nominal_map, variation_map, unknown_names = make_groups(hist_names)

  all_process_eras = sorted(set(list(nominal_map.keys()) + list(variation_map.keys())))

  problem_rows = []
  focus_rows = []
  integral_problem_rows = []
  missing_pair_rows = []
  nbins_seen = OrderedDict()

  with open(out_path, "w") as f:
    f.write("# Combine input histogram variation scan\n")
    f.write("# input_file      : {}\n".format(args.input_root))
    f.write("# output_file     : {}\n".format(out_path))
    f.write("# focus_bin       : {}\n".format(args.focus_bin if args.focus_bin > 0 else "disabled"))
    f.write("# eps             : {}\n".format(args.eps))
    f.write("# rel_threshold   : {}\n".format(args.rel_threshold))
    f.write("# abs_threshold   : {}\n".format(args.abs_threshold))
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
    f.write("# total_bkg_components : {}\n".format(",".join(TOTAL_BKG_PROCESSES)))
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
    f.write("Legend: flags are assigned per bin. Important ones are:\n")
    f.write("  ZERO_NOM_NONZERO_VAR, ONLY_UP_SURVIVES, ONLY_DOWN_SURVIVES,\n")
    f.write("  BOTH_ABOVE_NOM, BOTH_BELOW_NOM, HUGE_REL_*, NEG_*, MISS_UP/DOWN.\n")
    f.write("  NO_EFFECT is normally harmless and suppressed in problem-only mode.\n")
    write_rule(f)

    for process, era in all_process_eras:
      nom_name = nominal_map.get((process, era))
      nom_hist = get_hist(tfile, nom_name) if nom_name else None

      if not nom_hist:
        missing_pair_rows.append({
          "process": process, "era": era, "syst": "*", "issue": "MISSING_NOMINAL",
          "nom_name": str(nom_name), "up_name": "", "down_name": ""
        })
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

      for syst in systs:
        if not keep_syst(args, syst):
          continue

        pair = variation_map[(process, era)][syst]
        up_name = pair.get("Up")
        down_name = pair.get("Down")
        up_hist = get_hist(tfile, up_name) if up_name else None
        down_hist = get_hist(tfile, down_name) if down_name else None

        if up_hist and up_hist.GetNbinsX() != nbins:
          missing_pair_rows.append({
            "process": process, "era": era, "syst": syst,
            "issue": "UP_NBINS_MISMATCH",
            "nom_name": nom_name, "up_name": up_name, "down_name": down_name
          })
        if down_hist and down_hist.GetNbinsX() != nbins:
          missing_pair_rows.append({
            "process": process, "era": era, "syst": syst,
            "issue": "DOWN_NBINS_MISMATCH",
            "nom_name": nom_name, "up_name": up_name, "down_name": down_name
          })

        nom_int = float(nom_hist.Integral(1, nbins))
        up_int = float(up_hist.Integral(1, min(nbins, up_hist.GetNbinsX()))) if up_hist else None
        down_int = float(down_hist.Integral(1, min(nbins, down_hist.GetNbinsX()))) if down_hist else None
        int_flags = classify_integral(nom_int, up_int, down_int, args.eps, args.rel_threshold, args.abs_threshold)

        if row_selected_for_problem_summary(int_flags, args):
          integral_problem_rows.append({
            "process": process, "era": era, "syst": syst,
            "nom": nom_int, "up": up_int, "down": down_int,
            "r_up": safe_ratio(up_int, nom_int, args.eps) if up_int is not None else "NA",
            "r_down": safe_ratio(down_int, nom_int, args.eps) if down_int is not None else "NA",
            "flags": ",".join(int_flags),
            "up_name": str(up_name), "down_name": str(down_name)
          })

        if (not args.full) and (not row_is_problem(int_flags, args.include_no_effect)):
          # In problem-only mode, still inspect bins below, because a benign
          # integral can hide a pathological individual bin.
          pass

        rows_this_syst = []
        for ibin in range(1, nbins + 1):
          low, high = bin_edges(nom_hist, ibin)
          nom = float(nom_hist.GetBinContent(ibin))
          up = float(up_hist.GetBinContent(ibin)) if up_hist and ibin <= up_hist.GetNbinsX() else None
          down = float(down_hist.GetBinContent(ibin)) if down_hist and ibin <= down_hist.GetNbinsX() else None

          err_nom = hist_bin_error(nom_hist, ibin)
          err_up = hist_bin_error(up_hist, ibin) if up_hist and ibin <= up_hist.GetNbinsX() else None
          err_down = hist_bin_error(down_hist, ibin) if down_hist and ibin <= down_hist.GetNbinsX() else None

          base_flags = classify_bin(
            nom, up, down,
            args.eps, args.rel_threshold, args.abs_threshold
          )

          row = make_bin_row(
            process, era, syst, ibin, low, high,
            nom, up, down,
            err_nom, err_up, err_down,
            base_flags,
            up_name, down_name,
            args
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
          f.write("    Integral: nom={nom:15.8g} up={up:>15} down={down:>15} rUp={rup:>12} rDown={rdown:>12} flags={flags}\n".format(
            nom=nom_int,
            up=fmt_float(up_int),
            down=fmt_float(down_int),
            rup=safe_ratio(up_int, nom_int, args.eps) if up_int is not None else "NA",
            rdown=safe_ratio(down_int, nom_int, args.eps) if down_int is not None else "NA",
            flags=",".join(int_flags) if int_flags else "-"
          ))
          write_bin_row_header(f, "    ")
          for row in rows_this_syst:
            write_bin_row(f, row, "    ")

    if args.add_total_bkg and keep_process(args, TOTAL_BKG_NAME):
      # Era-specific total_bkg. This respects --era.
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
          nbins_seen=nbins_seen
        )

      # Full Run2 total_bkg.
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
        nbins_seen=nbins_seen
      )

    # Summary sections are written at the end because the scan is single-pass.
    f.write("\n")
    write_rule(f, "=")
    f.write("\n[SUMMARY AFTER SCAN]\n")
    f.write("  n_integral_problem_rows : {}\n".format(len(integral_problem_rows)))
    f.write("  n_bin_problem_rows      : {}\n".format(len(problem_rows)))
    f.write("  n_focus_bin_rows        : {}\n".format(len(focus_rows)))
    f.write("  n_missing_pair_rows     : {}\n".format(len(missing_pair_rows)))
    f.write("  nbins by process/era    :\n")
    for key, nb in nbins_seen.items():
      f.write("    {}/{} : {}\n".format(key[0], key[1], nb))

    if missing_pair_rows:
      write_rule(f)
      f.write("\n[MISSING / STRUCTURAL ISSUES]\n")
      f.write("{proc:<18} {era:<12} {syst:<55} {issue:<25} {up:<70} {down:<70}\n".format(
        proc="process", era="era", syst="syst", issue="issue", up="up_name", down="down_name"))
      for row in missing_pair_rows:
        f.write("{process:<18} {era:<12} {syst:<55} {issue:<25} {up_name:<70} {down_name:<70}\n".format(**row))

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
        # the bin under investigation.
        if (not args.full) and args.focus_problem_only and not row_is_problem(row["flags"].split(",") if row["flags"] else [], args.include_no_effect):
          continue
        f.write("{process:<18} {era:<12} {syst:<55} ".format(
          process=row["process"], era=row["era"], syst=row["syst"]))
        write_bin_row(f, row)

    if integral_problem_rows:
      write_rule(f)
      f.write("\n[INTEGRAL-LEVEL PROBLEMS]\n")
      f.write("{proc:<18} {era:<12} {syst:<55} {nom:>14} {up:>14} {down:>14} {rup:>11} {rdown:>11}  {flags}\n".format(
        proc="process", era="era", syst="syst", nom="nom_int", up="up_int",
        down="down_int", rup="up/nom", rdown="down/nom", flags="flags"))
      for row in integral_problem_rows:
        f.write("{process:<18} {era:<12} {syst:<55} {nom:14.8g} {up:>14} {down:>14} {r_up:>11} {r_down:>11}  {flags}\n".format(
          process=row["process"], era=row["era"], syst=row["syst"],
          nom=row["nom"], up=fmt_float(row["up"]), down=fmt_float(row["down"]),
          r_up=row["r_up"], r_down=row["r_down"], flags=row["flags"] if row["flags"] else "-"
        ))

    if problem_rows:
      write_rule(f)
      f.write("\n[ALL BIN-LEVEL PROBLEMS]\n")
      f.write("{proc:<18} {era:<12} {syst:<55} ".format(
        proc="process", era="era", syst="syst"))
      write_bin_row_header(f)
      for row in problem_rows:
        f.write("{process:<18} {era:<12} {syst:<55} ".format(
          process=row["process"], era=row["era"], syst=row["syst"]))
        write_bin_row(f, row)

  return {
    "n_hists": len(hist_names),
    "n_nominal": len(nominal_map),
    "n_unknown": len(unknown_names),
    "n_problem_bins": len(problem_rows),
    "n_problem_integrals": len(integral_problem_rows),
    "n_missing": len(missing_pair_rows),
    "out_path": out_path
  }


def parse_args():
  parser = argparse.ArgumentParser(
    description="Scan Combine input ROOT histograms bin-by-bin for pathological shape variations."
  )
  parser.add_argument("input_root", help="Combine input ROOT file, e.g. M1000_EE_card_input.root")
  parser.add_argument("-o", "--out", default=None, help="Output txt path")
  parser.add_argument("--focus-bin", type=int, default=8, help="Bin to summarize explicitly. Use 0 to disable.")
  parser.add_argument("--full", action="store_true",
                      help="Dump every bin for every variation. Default dumps only problematic rows plus summaries.")
  parser.add_argument("--problem-only", action="store_true",
                      help="Alias for default behavior. Kept for readability.")
  parser.add_argument("--focus-problem-only", action="store_true",
                      help="In the focus-bin summary, print only flagged rows.")
  parser.add_argument("--include-no-effect", action="store_true",
                      help="Treat NO_EFFECT rows as problems. Usually not needed.")
  parser.add_argument("--eps", type=float, default=1.0e-12,
                      help="Absolute epsilon for zero/equality checks.")
  parser.add_argument("--rel-threshold", type=float, default=1.0,
                      help="Flag relative changes >= this value when nominal is nonzero. Default 1.0 = 100%%.")
  parser.add_argument("--abs-threshold", type=float, default=1.0e-6,
                      help="Flag absolute changes >= this value when nominal is zero.")
  parser.add_argument("--physics-processes", action="store_true",
                      help="Keep only the main analysis processes: fake, cf, wz, ww, zz, zg, wz_ewk, mc_others, and signal*.")
  parser.add_argument("--process", action="append", default=[],
                      help="Restrict to an exact process name. Can be given multiple times, e.g. --process wz --process zz")
  parser.add_argument("--era", nargs="+", default=[],
                      help="Restrict to an exact era. Can be given multiple times, e.g. --era 2018")
  parser.add_argument("--syst", action="append", default=[],
                      help="Restrict to an exact systematic name. Can be given multiple times, e.g. --syst CMS_scale_j_2018_sr2")
  parser.add_argument("--syst-contains", action="append", default=[],
                      help="Restrict to systematics whose name contains this substring. Can be given multiple times, e.g. --syst-contains scale_j")
  parser.add_argument("--flag", action="append", default=[],
                      help="Restrict printed rows to a specific flag. Can be given multiple times, e.g. --flag ONLY_UP_SURVIVES --flag ZERO_NOM_NONZERO_VAR")
  parser.add_argument("--add-total-bkg", action="store_true",
                      help="Add a pseudo-process total_bkg by summing fake, cf, wz, ww, zz, zg, wz_ewk, and mc_others.")
  parser.add_argument("--min-neff", type=float, default=10.0,
                      help="Flag bins with effective entries below this value. Use 0 to disable. Default: 10.")
  parser.add_argument("--max-rel-stat", type=float, default=-1.0,
                      help="Flag bins with relative stat error above this value. Disabled by default. Example: 1.0 means 100%%.")
  return parser.parse_args()


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

  return 0


if __name__ == "__main__":
  sys.exit(main())
