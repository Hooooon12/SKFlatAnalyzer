#!/usr/bin/env python3
"""Plot b-only pulls of CMS_SUS24014_fake_m_syst_[era]_sr3 from diffNuisances txt files.

Expected directory structure looks like
  Pulls/<WP>/Unblind/
    pulls_Run2Sum_MuMu_M150_HNL_sr3_syst_Combined_Unblind.txt
    mask-sr3/
      pulls_Run2Sum_MuMu_M150_HNL_sr3_syst_Combined_Unblind_mask-sr3.txt

Pass the Pulls directory as ``input_dir`` and select ``<WP>`` with ``--wp``.
For backward compatibility, omitting ``--wp`` scans ``input_dir`` itself.

By default, files with zz_cr/zg_cr masked are excluded from the main plot.  Those
files are useful as a rateParam cross-check, but they duplicate the fake-sensitive
region selections and can be misleading if ZZ/ZG rateParams were not frozen.
"""

from __future__ import annotations

import argparse
import re
from pathlib import Path
from collections import defaultdict

import matplotlib.pyplot as plt

ERAS = ["2016preVFP", "2016postVFP", "2017", "2018"]
ERA_LABEL = {
    "2016preVFP": "2016 preVFP",
    "2016postVFP": "2016 postVFP",
    "2017": "2017",
    "2018": "2018",
}
MARKERS = ["o", "s", "^", "D"]

# Region labels describe what is USED in the fit, not what is masked.
REGION_ORDER_ALL = [
    "InvBJet only",
    "InvMET only",
    "InvBJet + InvMET",
    "SR3 only",
    "SR3 + InvBJet + InvMET",
]
REGION_ORDER_CR = [
    "InvBJet only",
    "InvMET only",
    "InvBJet + InvMET",
]

NUIS_RE = re.compile(
    r"^CMS_SUS24014_fake_m_syst_"
    r"(2016preVFP|2016postVFP|2017|2018)_sr3\s+(.*)$"
)
PAIR_RE = re.compile(r"!?([+-]?\d+(?:\.\d+)?)\s+\+/-\s+(\d+(?:\.\d+)?)")
MASS_RE = re.compile(r"_M(\d+)_HNL_")


def masked_regions(path: Path, input_dir: Path) -> set[str]:
    """Return masked regions, preferring the ``mask-*`` directory name."""
    try:
        relative = path.relative_to(input_dir)
    except ValueError:
        relative = path

    for directory in relative.parts[:-1]:
        if directory.startswith("mask-"):
            return set(directory.removeprefix("mask-").split("-"))

    # Backward compatibility with the old flat layout.
    m = re.search(r"_mask-(.+?)(?:\(\d+\))?\.txt$", path.name)
    if m:
        return set(m.group(1).split("-"))
    return set()


def used_region_label(masked: set[str]) -> str | None:
    """Translate masked regions into the fake-sensitive regions used in the fit."""
    masked = set(masked)
    # Ignore ZZ/ZG here: they are normalization-CR bookkeeping, not fake-sensitive regions.
    masked.discard("zz_cr")
    masked.discard("zg_cr")

    alive = []
    if "sr3" not in masked:
        alive.append("SR3")
    if "cr3_InvBJet" not in masked:
        alive.append("InvBJet")
    if "cr3_InvMET" not in masked:
        alive.append("InvMET")

    key = tuple(alive)
    mapping = {
        ("InvBJet",): "InvBJet only",
        ("InvMET",): "InvMET only",
        ("InvBJet", "InvMET"): "InvBJet + InvMET",
        ("SR3",): "SR3 only",
        ("SR3", "InvBJet", "InvMET"): "SR3 + InvBJet + InvMET",
    }
    return mapping.get(key)


def parse_bonly_pulls(path: Path) -> dict[str, tuple[float, float]]:
    """Return {era: (b-only central value, postfit sigma)} for the 20% fake syst."""
    out = {}
    for line in path.read_text(errors="ignore").splitlines():
        m = NUIS_RE.match(line)
        if not m:
            continue
        era, rest = m.groups()
        pairs = PAIR_RE.findall(rest)
        # diffNuisances line contains: prefit, b-only, then s+b (or b-only copy).
        if len(pairs) < 2:
            raise RuntimeError(f"Could not parse b-only fit from {path.name}:\n{line}")
        val, err = map(float, pairs[1])
        out[era] = (val, err)
    return out


def collect(input_dir: Path, masses: set[int] | None, include_normcr_masked: bool):
    data = defaultdict(dict)
    chosen_file = {}

    pattern = "pulls_Run2Sum_MuMu_M*_HNL_sr3_syst_Combined_Unblind*.txt"
    # Full-fit files live directly in input_dir; masked fits live in mask-* subdirectories.
    paths = list(input_dir.glob(pattern))
    paths.extend(input_dir.glob(f"mask-*/{pattern}"))
    for path in sorted(paths):
        mm = MASS_RE.search(path.name)
        if not mm:
            continue
        mass = int(mm.group(1))
        if masses and mass not in masses:
            continue

        masked = masked_regions(path, input_dir)
        has_normcr_mask = bool({"zz_cr", "zg_cr"} & masked)
        if has_normcr_mask and not include_normcr_masked:
            continue

        region = used_region_label(masked)
        if region is None:
            continue

        pulls = parse_bonly_pulls(path)
        if not pulls:
            continue

        key = (mass, region)
        # Prefer the standard fit (ZZ/ZG CRs retained) if duplicates exist.
        if key in chosen_file:
            old = chosen_file[key]
            old_masked = masked_regions(old, input_dir)
            old_has_normcr_mask = bool({"zz_cr", "zg_cr"} & old_masked)
            if old_has_normcr_mask and not has_normcr_mask:
                data[mass][region] = pulls
                chosen_file[key] = path
            continue

        data[mass][region] = pulls
        chosen_file[key] = path

    return data, chosen_file


def resolve_input_dir(input_dir: Path, wp: str | None, fit_dir: str) -> Path:
    """Resolve the directory containing full-fit txt files and mask-* directories."""
    if wp is None:
        return input_dir

    wp_path = Path(wp)
    if not wp_path.is_absolute():
        wp_path = input_dir / wp_path
    return wp_path / fit_dir


def print_table(data, region_order):
    for mass in sorted(data):
        print(f"\nM{mass}")
        print(f"{'region set':26s}" + "".join(f"{ERA_LABEL[e]:>22s}" for e in ERAS))
        for region in region_order:
            if region not in data[mass]:
                continue
            cells = []
            for era in ERAS:
                if era in data[mass][region]:
                    v, s = data[mass][region][era]
                    cells.append(f"{v:+.2f} +/- {s:.2f}")
                else:
                    cells.append("missing")
            print(f"{region:26s}" + "".join(f"{c:>22s}" for c in cells))


def plot(data, region_order, output: Path, title: str, xlim: tuple[float, float] | None):
    masses = sorted(data)
    rows = []
    for mass in masses:
        for region in region_order:
            if region in data[mass]:
                rows.append((mass, region))

    if not rows:
        raise RuntimeError("No matching pulls found.")

    # One forest plot (single axes): masses are blocks, region choices are rows.
    fig_h = max(5.0, 0.55 * len(rows) + 2.2)
    fig, ax = plt.subplots(figsize=(10.5, fig_h))

    y_base = list(range(len(rows)))[::-1]
    offsets = [-0.21, -0.07, 0.07, 0.21]

    # Prefit reference: nuisance has mean 0 and sigma 1.
    ax.axvspan(-1.0, 1.0, alpha=0.08, zorder=0)
    ax.axvline(0.0, linewidth=1.1, zorder=0)
    ax.axvline(-1.0, linewidth=0.8, linestyle="--", alpha=0.5, zorder=0)
    ax.axvline(+1.0, linewidth=0.8, linestyle="--", alpha=0.5, zorder=0)

    for iera, era in enumerate(ERAS):
        xs, xerrs, ys = [], [], []
        for y, (mass, region) in zip(y_base, rows):
            entry = data[mass][region].get(era)
            if entry is None:
                continue
            val, err = entry
            xs.append(val)
            xerrs.append(err)
            ys.append(y + offsets[iera])
        ax.errorbar(
            xs, ys, xerr=xerrs,
            fmt=MARKERS[iera], linestyle="none", capsize=2.5,
            label=ERA_LABEL[era], markersize=5.5,
        )

    labels = [f"M{mass}   {region}" for mass, region in rows]
    ax.set_yticks(y_base)
    ax.set_yticklabels(labels)
    ax.set_xlabel("Post-fit nuisance value [prefit sigma units]")
    ax.set_title(title)
    ax.grid(axis="x", alpha=0.2)
    ax.legend(ncol=4, loc="upper center", bbox_to_anchor=(0.5, -0.10), frameon=False)

    # Separate mass blocks.
    for i in range(1, len(rows)):
        if rows[i][0] != rows[i - 1][0]:
            ax.axhline((y_base[i] + y_base[i - 1]) / 2.0, linewidth=0.8, alpha=0.35)

    if xlim is not None:
        ax.set_xlim(*xlim)

    ax.text(
        0.16, -0.16,
        "horizontal error bars: postfit 1 sigma / shaded band: prefit 1 sigma",
        transform=ax.transAxes, ha="left", va="top", fontsize=9,
    )

    fig.tight_layout()
    output.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(output, bbox_inches="tight")
    plt.close(fig)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("input_dir", nargs="?", default=".", type=Path,
                    help="Pulls root directory (or the txt directory itself when --wp is omitted)")
    ap.add_argument("--wp", "--WP", default=None,
                    help="main working-point directory name under input_dir")
    ap.add_argument("--fit-dir", default="Unblind",
                    help="fit-state directory under the WP (default: Unblind)")
    ap.add_argument("--masses", nargs="*", type=int, default=None,
                    help="masses to plot, e.g. --masses 150 200 (default: all found)")
    ap.add_argument("--regions", choices=["all", "cr"], default="all",
                    help="all: CR-only + SR3-only + combined; cr: CR-only rows only")
    ap.add_argument("--output", type=Path, default=Path("fake_syst_pulls_MuMu.png"))
    ap.add_argument("--title", default="20% inclusive fake systematic pull by b-only fit (MuMu)")
    ap.add_argument("--xlim", nargs=2, type=float, default=None, metavar=("XMIN", "XMAX"))
    ap.add_argument("--include-normcr-masked", action="store_true",
                    help="allow files where zz_cr/zg_cr are masked (not recommended for main plot)")
    args = ap.parse_args()

    input_dir = resolve_input_dir(args.input_dir, args.wp, args.fit_dir)
    if not input_dir.is_dir():
        raise FileNotFoundError(f"Pull directory does not exist: {input_dir}")

    masses = set(args.masses) if args.masses else None
    data, chosen = collect(input_dir, masses, args.include_normcr_masked)
    region_order = REGION_ORDER_CR if args.regions == "cr" else REGION_ORDER_ALL

    print(f"Scanning: {input_dir}")
    print_table(data, region_order)
    print("\nFiles used:")
    for (mass, region), path in sorted(chosen.items()):
        if region in region_order:
            print(f"  M{mass:>4}  {region:26s} <- {path.relative_to(input_dir)}")

    plot(data, region_order, args.output, args.title,
         tuple(args.xlim) if args.xlim else None)
    print(f"\nSaved: {args.output}")


if __name__ == "__main__":
    main()
