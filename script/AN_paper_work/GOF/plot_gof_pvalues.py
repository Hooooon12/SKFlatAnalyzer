#!/usr/bin/env python3

import os
import re
import json
import glob
import argparse
import warnings

warnings.filterwarnings(
  "ignore",
  message=r"The value of the smallest subnormal.*",
  category=UserWarning,
)

import matplotlib.pyplot as plt

def set_cms_style():
  plt.rcParams.update({
    "font.size": 14,
    "axes.linewidth": 1.2,
    "axes.labelsize": 16,
    "xtick.labelsize": 12,
    "ytick.labelsize": 13,
    "legend.fontsize": 12,
    "xtick.direction": "in",
    "ytick.direction": "in",
    "xtick.top": True,
    "ytick.right": True,
    "xtick.major.size": 6,
    "ytick.major.size": 6,
    "xtick.minor.size": 3,
    "ytick.minor.size": 3,
  })


def draw_cms_label(ax, lumi_text=r"137.6 fb$^{-1}$ (13 TeV)", extra_text="Preliminary"):
  ax.text(
    0.0,
    1.02,
    "CMS",
    transform=ax.transAxes,
    fontsize=20,
    fontweight="bold",
    ha="left",
    va="bottom",
    clip_on=False,
  )

  if extra_text:
    ax.text(
      0.105,
      1.02,
      extra_text,
      transform=ax.transAxes,
      fontsize=16,
      fontstyle="italic",
      ha="left",
      va="bottom",
      clip_on=False,
    )

  ax.text(
    1.0,
    1.02,
    lumi_text,
    transform=ax.transAxes,
    fontsize=14,
    ha="right",
    va="bottom",
    clip_on=False,
  )

def find_p_value(obj):
  if isinstance(obj, dict):
    if "p" in obj and isinstance(obj["p"], (int, float)):
      return float(obj["p"])
    for v in obj.values():
      p = find_p_value(v)
      if p is not None:
        return p
  elif isinstance(obj, list):
    for v in obj:
      p = find_p_value(v)
      if p is not None:
        return p
  return None


def parse_files(args):
  pattern = os.path.join(args.inputDir, "gof_Run2Sum_*.json")
  files = glob.glob(pattern)
  #print(pattern)
  #print(files)

  data = {
    "EE": {},
    "EMu": {},
    "MuMu": {}
  }

  #print(args.region)
  if args.region in {"sr1", "sr2", "sr3"}:
    regex = re.compile(
      rf"^gof_Run2Sum_(EE|EMu|MuMu)_(M(\d+)|Weinberg)_.*_{args.region}_.*\.json$"
    )
  else:
    regex = re.compile(
      r"^gof_Run2Sum_(EE|EMu|MuMu)_(M(\d+)|Weinberg)_(?!.*sr\d+).+\.json$" # ^ --> beginning of the string, $ --> end of the string, ?! --> negative search, .* --> any character geq 0, .+ --> any characer gt 0, \. --> escape . (real ".")
    )

  for f in files:
    base = os.path.basename(f)
    #print(f, base)
    m = regex.match(base)
    if not m:
      continue

    channel = m.group(1)
    mass_token = m.group(2)
    mass_num = m.group(3)

    with open(f, "r") as jf:
      content = json.load(jf)

    pval = find_p_value(content)
    if pval is None:
      print("Warning: could not find p-value in " + base)
      continue

    if mass_token == "Weinberg":
      mass_key = "Weinberg"
    else:
      mass_key = int(mass_num)

    data[channel][mass_key] = pval

  return data


def get_sorted_mass_keys(channel_dict):
  numeric_keys = sorted([k for k in channel_dict if isinstance(k, int)])
  out = numeric_keys[:]
  if "Weinberg" in channel_dict:
    out.append("Weinberg")
  return out

def make_plot(
  channel,
  channel_dict,
  output_dir,
  logy=False,
  ymin=1e-3,
  lumi_text=r"137.6 fb$^{-1}$ (13 TeV)",
  cms_extra="Preliminary",
  region="",
):
  if len(channel_dict) == 0:
    print("No entries found for " + channel)
    return

  ordered_keys = get_sorted_mass_keys(channel_dict)

  x = list(range(len(ordered_keys)))
  y = [channel_dict[k] for k in ordered_keys]
  labels = [str(k) if isinstance(k, int) else "Weinberg" for k in ordered_keys]

  fig, ax = plt.subplots(figsize=(16, 6))

  y_plot = y[:]

  if logy:
    if ymin <= 0.0:
      raise ValueError("--ymin must be positive when using --logy")

    for i, val in enumerate(y_plot):
      if val <= 0.0:
        print(
          "Warning: non-positive p-value found in "
          + channel
          + " at "
          + labels[i]
          + ". It will be drawn at ymin = "
          + str(ymin)
        )
        y_plot[i] = ymin

    ax.set_yscale("log")
    ax.set_ylim(ymin, 1.0)
  else:
    ax.set_ylim(0.0, 1.0)

  ax.plot(
    x,
    y_plot,
    marker="o",
    linewidth=1.5,
    markersize=5,
    label="Observed",
  )

  ax.axhline(0.05, linestyle="--", linewidth=1.0, label=r"$p = 0.05$")
  ax.axhline(0.01, linestyle=":", linewidth=1.0, label=r"$p = 0.01$")

  if "Weinberg" in ordered_keys:
    ax.axvline(
      len(ordered_keys) - 1.5,
      linestyle="--",
      linewidth=1.5,
      color="black",
    )

  ax.set_xticks(x)
  ax.set_xticklabels(labels, rotation=60, ha="right")
  ax.set_xlim(-0.6, len(x) - 0.4)

  ax.set_xlabel(r"$m_{N}$ [GeV]")
  ax.set_ylabel(r"GOF $p$-value")

  channel_label = {
    "EE": r"$ee$ channel",
    "EMu": r"$e\mu$ channel",
    "MuMu": r"$\mu\mu$ channel",
  }

  ax.text(
    0.03,
    0.94,
    channel_label.get(channel, channel),
    transform=ax.transAxes,
    fontsize=16,
    ha="left",
    va="top",
  )

  draw_cms_label(
    ax,
    lumi_text=lumi_text,
    extra_text=cms_extra,
  )

  if logy:
    ax.grid(True, which="both", axis="y", alpha=0.3)
  else:
    ax.grid(True, axis="y", alpha=0.3)

  ax.legend(
    loc="upper right",
    bbox_to_anchor=(0.88, 0.86), # can be adjusted later
    frameon=False,
  )

  plt.tight_layout(rect=[0, 0, 1, 0.95])

  suffix = "_"+region
  if logy: suffix += "_logy"

  out_png = os.path.join(output_dir+"/"+region, "gof_pvalues_" + channel + suffix + ".png")
  out_pdf = os.path.join(output_dir+"/"+region, "gof_pvalues_" + channel + suffix + ".pdf")

  fig.savefig(out_png, dpi=200, bbox_inches="tight")
  fig.savefig(out_pdf, bbox_inches="tight")
  plt.close(fig)

  print("Saved: " + out_png)
  print("Saved: " + out_pdf)

def main():
  parser = argparse.ArgumentParser()
  parser.add_argument(
    "-i", "--inputDir",
    required=True,
    help="Directory containing gof_Run2Sum_*.json files"
  )
  parser.add_argument(
    "-o", "--outputDir",
    default="gof_pvalue_plots",
    help="Output directory"
  )
  parser.add_argument(
    "--logy",
    action="store_true",
    help="Use logarithmic y-axis for p-values"
  )
  parser.add_argument(
    "--ymin",
    type=float,
    default=1e-3,
    help="Minimum y-axis value when using --logy"
  )
  parser.add_argument(
    "--lumiText",
    default=r"137.6 fb$^{-1}$ (13 TeV)",
    help="Luminosity text shown at top right"
  )
  parser.add_argument(
    "--cmsExtra",
    default="Preliminary",
    help='Extra text after CMS. For example: "Preliminary", "Work in progress", or "".'
  )
  parser.add_argument(
    "--region",
    default="Combined",
    choices=['Combined','sr1','sr2','sr3'],
    help='GOF from specific signal region'
  )
  args = parser.parse_args()

  set_cms_style()

  if not os.path.isdir(args.outputDir+"/"+args.region):
    os.makedirs(args.outputDir+"/"+args.region)

  data = parse_files(args)

  make_plot(
    "EE",
    data["EE"],
    args.outputDir,
    logy=args.logy,
    ymin=args.ymin,
    lumi_text=args.lumiText,
    cms_extra=args.cmsExtra,
    region=args.region,
  )
  
  make_plot(
    "EMu",
    data["EMu"],
    args.outputDir,
    logy=args.logy,
    ymin=args.ymin,
    lumi_text=args.lumiText,
    cms_extra=args.cmsExtra,
    region=args.region,
  )
  
  make_plot(
    "MuMu",
    data["MuMu"],
    args.outputDir,
    logy=args.logy,
    ymin=args.ymin,
    lumi_text=args.lumiText,
    cms_extra=args.cmsExtra,
    region=args.region,
  )

if __name__ == "__main__":
  main()
