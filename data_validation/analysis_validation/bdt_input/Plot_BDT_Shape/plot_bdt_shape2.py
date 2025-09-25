import os
import ROOT
from array import array
from collections import defaultdict
import tdrstyle

ROOT.gROOT.SetBatch(True)
tdrstyle.setTDRStyle()

# --- Configurations ---
eras = ["2016preVFP", "2016postVFP", "2017", "2018"]
flavours = ["MuMu", "EE", "EMu"]
masses = [85, 90, 95, 100, 125, 150, 200, 300, 400, 500]
base_dir = "/data6/Users/jalmond/2020/Plotter/HNDiLeptonWorskspace/InputFiles/MergedFiles/HNL_SignalRegion_Plotter/HNL_SignalRegion_Plotter_2025-07-23"

min_stat = 25
normalize_to = "bkg"

era_lumi_map = {
    "2016preVFP": 19.5,
    "2016postVFP": 16.8,
    "2017": 41.5,
    "2018": 59.8
}

def draw_cms_label(lumi):
    latex = ROOT.TLatex()
    latex.SetNDC(True)
    latex.SetTextFont(42)
    latex.SetTextSize(0.04)
    latex.DrawLatex(0.15, 0.96, "CMS Preliminary")
    latex.SetTextAlign(31)
    latex.DrawLatex(0.95, 0.96, "%.1f fb^{-1} (13 TeV)" % lumi)

def open_root_file(path):
    if not os.path.isfile(path):
        print("[SKIP] Missing file:", path)
        return None
    return ROOT.TFile.Open(path)

def is_valid_hist(name, mass):
    return name.startswith("M%d_" % mass) and "_Binned" not in name and ("V3" in name or "V4" in name)

def get_matching_hists(file, dir_path, mass):
    hists = {"V3_bkg": [], "V4_bkg": [], "V3_sig": [], "V4_sig": []}
    tdir = file.Get(dir_path)
    if not tdir:
        return hists
    for key in tdir.GetListOfKeys():
        name = key.GetName()
        if not is_valid_hist(name, mass):
            continue
        hist = tdir.Get(name).Clone()
        hist.SetDirectory(0)
        hist.Rebin(4)
        if hist.GetSumw2N() == 0:
            hist.Sumw2()
        filename = os.path.basename(file.GetName())
        tag = "bkg" if "Bkg" in filename else "sig"
        if "V3" in name:
            hists["V3_" + tag].append((name, hist))
        elif "V4" in name:
            hists["V4_" + tag].append((name, hist))
    return hists

def merge_hists(dict1, dict2):
    out = {}
    for key in dict1:
        out[key] = dict1[key] + dict2[key]
    return out


def get_smart_bin_edges(hist, min_stat):
    bin_edges = []

    # Keep outer bins untouched
    nbins = hist.GetNbinsX()
    x_min = -0.4
    x_max = 0.4

    total = 0
    start = None

    for i in range(1, nbins + 1):
        low = hist.GetBinLowEdge(i)
        high = hist.GetBinLowEdge(i+1)
        center = hist.GetBinCenter(i)

        if center < x_min or center > x_max:
            bin_edges.append(low)
            continue

        if start is None:
            start = low

        total += hist.GetBinContent(i)
        if total >= min_stat:
            bin_edges.append(start)
            start = high
            total = 0

    if start is not None:
        bin_edges.append(start)
    bin_edges.append(hist.GetBinLowEdge(nbins + 1))

    # Remove duplicates and sort
    bin_edges = sorted(set(bin_edges))
    return bin_edges if len(bin_edges) > 1 else None


def rebin_with_edges(hist, bin_edges):
    if not bin_edges:
        return hist
    rebinned = hist.Rebin(len(bin_edges) - 1, hist.GetName() + "_rebinned", array('d', bin_edges))
    rebinned.SetDirectory(0)
    return rebinned

def normalize_hist(hist, target_integral):
    if hist.Integral() > 0:
        hist.Scale(target_integral / hist.Integral())

def plot_comparison(hists, flavour, era, mass):
    os.makedirs("plots", exist_ok=True)

    ref_hist = None
    for key in ["V3_bkg", "V4_bkg", "V3_sig", "V4_sig"]:
        if hists[key]:
            ref_hist = hists[key][0][1]
            break

    if not ref_hist:
        print("[SKIP] No reference histogram for binning:", era, flavour, mass)
        return

    bin_edges = get_smart_bin_edges(ref_hist, min_stat)
    if not bin_edges:
        bin_edges = []
        x = -0.4
        step = 0.05
        while x < 0.4:
            bin_edges.append(x)
            x += step
        bin_edges.append(0.4)

    c = ROOT.TCanvas("c", "", 800, 600)
    legend = ROOT.TLegend(0.18, 0.65, 0.48, 0.89)
    legend.SetTextSize(0.022)

    color_map = {"V3_bkg": ROOT.kBlue, "V4_bkg": ROOT.kRed, "V3_sig": ROOT.kBlue, "V4_sig": ROOT.kRed}
    style_map = {"V3_bkg": ROOT.kSolid, "V4_bkg": ROOT.kSolid, "V3_sig": ROOT.kDashed, "V4_sig": ROOT.kDashed}

    bkg_yield = sum(h.Integral() for _, h in hists["V3_bkg"] + hists["V4_bkg"]) or 1.0
    all_hists = []
    for key in ["V3_bkg", "V4_bkg", "V3_sig", "V4_sig"]:
        for name, hist in hists[key]:
            hist = rebin_with_edges(hist, bin_edges)
            if normalize_to == "bkg" and "sig" in key:
                normalize_hist(hist, bkg_yield)
            elif normalize_to == "unity":
                normalize_hist(hist, 1.0)
            hist.SetLineWidth(2)
            hist.SetLineColor(color_map[key])
            hist.SetLineStyle(style_map[key])
            hist.GetXaxis().SetTitle("BDT score")
            hist.GetYaxis().SetTitle("Entries (normalized)")
            hist.GetXaxis().SetRangeUser(-0.4, 0.4)
            label = f"{'V3' if 'V3' in key else 'V4'} ({'sig' if 'sig' in key else 'bkg'})"
            legend.AddEntry(hist, label, "l")
            all_hists.append(hist)

    if not all_hists:
        return

    max_y = max(h.GetMaximum() for h in all_hists)
    for i, h in enumerate(all_hists):
        h.SetMaximum(max_y * 1.3)
        h.Draw("HIST" if i == 0 else "HIST SAME")

    legend.Draw()
    draw_cms_label(era_lumi_map.get(era, 0.0))
    outname = f"plots/Compare_M{mass}_{flavour}_{era}.png"
    c.SaveAs(outname)
    print("[OK]", outname)

def plot_era_overlay(hists_by_era, flavour, mass, kind="sig"):
    os.makedirs("plots", exist_ok=True)
    c = ROOT.TCanvas("c", "", 800, 600)
    legend = ROOT.TLegend(0.18, 0.65, 0.48, 0.89)
    legend.SetTextSize(0.022)

    color_map = {
        "2016preVFP": ROOT.kRed,
        "2016postVFP": ROOT.kBlue,
        "2017": ROOT.kGreen+2,
        "2018": ROOT.kMagenta+2
    }
    style = ROOT.kDashed if kind == "sig" else ROOT.kSolid

    ref_hist = None
    for era in eras:
        if hists_by_era[era]:
            ref_hist = hists_by_era[era][0][1]
            break

    if not ref_hist:
        return

    bin_edges = get_smart_bin_edges(ref_hist, min_stat)
    if not bin_edges:
        bin_edges = []
        x = -0.4
        step = 0.05
        while x < 0.5:
            bin_edges.append(x)
            x += step
        bin_edges.append(0.5)

    all_hists = []
    for era in eras:
        for name, hist in hists_by_era[era]:
            hist = rebin_with_edges(hist, bin_edges)
            if normalize_to == "unity":
                normalize_hist(hist, 1.0)
            hist.SetLineWidth(2)
            hist.SetLineColor(color_map.get(era, ROOT.kBlack))
            hist.SetLineStyle(style)
            hist.GetXaxis().SetTitle("BDT score")
            hist.GetYaxis().SetTitle("Entries (normalized)")
            hist.GetXaxis().SetRangeUser(-0.4, 0.4)
            label = f"{era} M{mass} ({kind})"
            legend.AddEntry(hist, label, "l")
            all_hists.append(hist)

    if not all_hists:
        return

    max_y = max(h.GetMaximum() for h in all_hists)
    for i, h in enumerate(all_hists):
        h.SetMaximum(max_y * 1.3)
        h.Draw("HIST" if i == 0 else "HIST SAME")

    legend.Draw()
    draw_cms_label(sum(era_lumi_map.values()) if kind == "sig" else 0.0)
    outname = f"plots/AllEras_V4_{kind.upper()}_M{mass}_{flavour}.png"
    c.SaveAs(outname)
    print("[OK]", outname)

def main():
    for flavour in flavours:
        for mass in masses:
            v4_sig = defaultdict(list)
            v4_bkg = defaultdict(list)
            for era in eras:
                bkg_path = os.path.join(base_dir, era, "HNL_SignalRegion_Plotter_Bkg.root")
                sig_path = os.path.join(base_dir, era, f"HNL_SignalRegion_Plotter_HNL_{mass}.root")
                bkg_file = open_root_file(bkg_path)
                sig_file = open_root_file(sig_path)
                if not (bkg_file and sig_file):
                    continue
                dir_path = f"LimitExtraction/HNL_ULIDv2/{flavour}/SR3BDTBDT"
                bkg_hists = get_matching_hists(bkg_file, dir_path, mass)
                sig_hists = get_matching_hists(sig_file, dir_path, mass)

                print(f"\n[INFO] Histograms for era: {era} | flavour: {flavour} | mass: {mass}")
                for key in ["V3_bkg", "V4_bkg"]:
                    if bkg_hists[key]:
                        print(f"  [OK] Found {len(bkg_hists[key])} background hists in {key}:")
                        for name, _ in bkg_hists[key]:
                            print("       -", name)
                    else:
                        print("  [--] No background hists in", key)

                for key in ["V3_sig", "V4_sig"]:
                    if sig_hists[key]:
                        print(f"  [OK] Found {len(sig_hists[key])} signal hists in {key}:")
                        for name, _ in sig_hists[key]:
                            print("       -", name)
                    else:
                        print("  [--] No signal hists in", key)

                merged = merge_hists(bkg_hists, sig_hists)
                plot_comparison(merged, flavour, era, mass)
                v4_sig[era].extend(sig_hists["V4_sig"])
                v4_bkg[era].extend(bkg_hists["V4_bkg"])

            plot_era_overlay(v4_sig, flavour, mass, kind="sig")
            plot_era_overlay(v4_bkg, flavour, mass, kind="bkg")

if __name__ == "__main__":
    main()
