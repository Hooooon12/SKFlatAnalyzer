import os
import re
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# Configurations
masses = ['100', '125', '200', '250', '300', '400', '500']
flavours = ['MuMu', 'EE', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']

coarse_dir = "LooseScan/logs"
fine_prefix = "fine_"
tight_dir = "logs"
out_dir = "plots"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

# Patterns
pattern = re.compile(r"\[RESULT\] Best total FOM: ([\d.]+) using (\d+) bins")
v2_pattern = re.compile(r"Summed Azimov FOM from LimitBins:")
def extract_fom_points(directory, prefix, mass, flav, era):
    x_vals = []
    y_vals = []
    for nbin in range(5, 21):
        filename = f"scan_{prefix}{mass}_{flav}_{era}_{nbin}.out"
        filepath = os.path.join(directory, filename)
        if not os.path.exists(filepath):
            continue
        with open(filepath, "r") as f:
            for line in f:
                match = pattern.search(line)
                if match:
                    fom = float(match.group(1))
                    bins = int(match.group(2))
                    x_vals.append(bins)
                    y_vals.append(fom)
                    break
    return sorted(zip(x_vals, y_vals))


def extract_v2_fom(mass, flav,era):

    v2_pattern = re.compile(r"\[([A-Za-z]+)_V2 M(\d+)\] Summed Azimov FOM from LimitBins: ([\d.]+)")

    for directory in [coarse_dir]:
        if not os.path.isdir(directory):
            continue
        for fname in os.listdir(directory):
            path = os.path.join(directory, fname)

            if not os.path.isfile(path):
                continue
            if not mass in path:
                continue
            if not era in path:
                continue
            if not flav in path:
                continue            

            with open(path, "r") as f:
                for line in f:
                    match = v2_pattern.search(line)
                    if match:
                        flav_match, mass_match, fom_str = match.groups()
                        if flav_match == flav and mass_match == mass:
                            return float(fom_str)
    return None




def make_graph(points, color, title):
    graph = ROOT.TGraph(len(points))
    for i, (x, y) in enumerate(points):
        graph.SetPoint(i, x, y)
    graph.SetLineColor(color)
    graph.SetMarkerColor(color)
    graph.SetMarkerStyle(20)
    graph.SetLineWidth(2)
    graph.SetTitle(title)
    return graph

for mass in masses:
    for flav in flavours:
        for era in eras:
            loose_points = extract_fom_points(coarse_dir, "", mass, flav, era)
            fine_points = extract_fom_points(coarse_dir, fine_prefix, mass, flav, era)
            tight_points = extract_fom_points(tight_dir, "", mass, flav, era)
            v2_fom = extract_v2_fom(mass, flav,era)

            all_foms = [y for _, y in loose_points + fine_points + tight_points]
            if v2_fom: all_foms.append(v2_fom)
            if not all_foms:
                continue

            canvas = ROOT.TCanvas("c", "c", 800, 600)
            legend = ROOT.TLegend(0.50, 0.70, 0.88, 0.88)
            legend.SetBorderSize(0)
            legend.SetFillStyle(0)

            plotted = False
            y_max = max(all_foms) * 1.5

            if loose_points:
                g_loose = make_graph(loose_points, ROOT.kBlue, "")
                g_loose.GetYaxis().SetRangeUser(0, y_max)
                g_loose.Draw("APL")
                legend.AddEntry(g_loose, "Loose criteria (coarse scan)", "lp")
                plotted = True

            if fine_points:
                g_fine = make_graph(fine_points, ROOT.kGreen+2, "")
                g_fine.Draw("PL" if plotted else "APL")
                legend.AddEntry(g_fine, "Loose criteria (fine scan)", "lp")
                plotted = True

            if tight_points:
                g_tight = make_graph(tight_points, ROOT.kRed, "")
                g_tight.Draw("PL" if plotted else "APL")
                legend.AddEntry(g_tight, "Tight criteria", "lp")
                plotted = True

            if v2_fom:
                v2_line = ROOT.TLine(5, v2_fom, 20, v2_fom)
                v2_line.SetLineStyle(2)
                v2_line.SetLineColor(ROOT.kMagenta + 2)
                v2_line.SetLineWidth(2)
                v2_line.Draw("same")
                legend.AddEntry(v2_line, "V2 FOM (LimitBins)", "l")

            if plotted:
                g_loose.GetXaxis().SetTitle("Number of Bins")
                g_loose.GetYaxis().SetTitle("Total FOM")
                g_loose.GetYaxis().SetTitleOffset(1.3)
                g_loose.SetTitle(f"FOM Scan: M={mass}, {flav}, {era}")

            canvas.SetGrid()
            legend.Draw()
            canvas.SaveAs(f"{out_dir}/FOMScan_{mass}_{flav}_{era}.pdf")
