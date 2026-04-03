import os
import re
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# Config
masses = ['100', '125', '150', '200', '250', '300', '400', '500']
flavours = ['MuMu', 'EE', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']
out_dir = "plots"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

# Compile regex
result_pattern = re.compile(r"\[RESULT\] Best total FOM (V[34]): ([\d.]+)")
summary_pattern = re.compile(r"\[SUMMARY\] \[.*_(V[34]) M(\d+)\] Summed Azimov FOM from LimitBins: ([\d.]+)")

# Source patterns and labels
configs = [
    ("logs", "scan_{mass}_{flavour}_{era}_40_{X}.out", "Strict", ROOT.kRed),
    ("logs", "scan_{mass}_{flavour}_{era}_35_{X}.out", "Strict", ROOT.kRed),
    ("logs", "scan_{mass}_{flavour}_{era}_25_{X}.out", "Strict", ROOT.kRed),
    ("logs", "scan_loose_{mass}_{flavour}_{era}_40_{X}.out", "Loose", ROOT.kMagenta+2),
    ("logs", "scan_loose_{mass}_{flavour}_{era}_35_{X}.out", "Loose", ROOT.kMagenta+2),
    ("logs", "scan_loose_{mass}_{flavour}_{era}_25_{X}.out", "Loose", ROOT.kMagenta+2),
    ("logs", "scan_very_loose_{mass}_{flavour}_{era}_40_{X}.out", "Very Loose", ROOT.kOrange),
    ("logs", "scan_very_loose_{mass}_{flavour}_{era}_35_{X}.out", "Very Loose", ROOT.kOrange),
    ("logs", "scan_very_loose_{mass}_{flavour}_{era}_25_{X}.out", "Very Loose", ROOT.kOrange),
]

# Helpers
def extract_fom_points(dirpath, pattern_template, mass, flav, era):
    points_v3 = []
    points_v4 = []
    for nbin in range(5, 51):
        fname = pattern_template.format(mass=mass, flavour=flav, era=era, X=nbin)
        fpath = os.path.join(dirpath, fname)
        if not os.path.exists(fpath):
            continue
        with open(fpath, "r") as f:
            for line in f:
                match = result_pattern.search(line)
                if match:
                    version, fom = match.groups()
                    fom = float(fom)
                    if version == "V3":
                        points_v3.append((nbin, fom))
                    elif version == "V4":
                        points_v4.append((nbin, fom))

    return sorted(points_v3), sorted(points_v4)

def extract_v2_fom(mass, flav, era):
    summary_file = f"logs_v0/scan_{mass}_{flav}_{era}_35_30.out"
    if not os.path.exists(summary_file):
        return None
    with open(summary_file, "r") as f:
        for line in f:
            match = summary_pattern.search(line)
            if match:
                version, _, fom = match.groups()
                if version == "V3":  # Only show V3 legacy FOM
                    return float(fom)
    return None

def make_graph(points, color, marker_style=20):
    graph = ROOT.TGraph(len(points))
    for i, (x, y) in enumerate(points):
        graph.SetPoint(i, x, y)
    graph.SetLineColor(color)
    graph.SetMarkerColor(color)
    graph.SetMarkerStyle(marker_style)
    graph.SetLineWidth(2)
    return graph


# Main loop
for mass in masses:
    for flav in flavours:
        for era in eras:
            graphs = []
            legend = ROOT.TLegend(0.50, 0.70, 0.88, 0.88)
            legend.SetBorderSize(0)
            legend.SetFillStyle(0)

            all_foms = []

            for dirpath, pattern, label, color in configs:
                points_v3, points_v4 = extract_fom_points(dirpath, pattern, mass, flav, era)
                if points_v3:
                    g_v3 = make_graph(points_v3, color + 1, marker_style=24)  # e.g. open triangle up
                    graphs.append((g_v3, f"{label} (V3)"))
                    all_foms.extend(y for _, y in points_v3)
                    
                if points_v4:
                    g_v4 = make_graph(points_v4, color, marker_style=20)  # e.g. solid circle
                    graphs.append((g_v4, f"{label} (V4)"))
                    all_foms.extend(y for _, y in points_v4)


            v2_fom = extract_v2_fom(mass, flav, era)
            if v2_fom:
                all_foms.append(v2_fom)

            if not all_foms:
                continue  # Skip empty plots

            ymax = max(all_foms) * 1.5

            c = ROOT.TCanvas("c", "c", 800, 600)
            plotted = False

            leg_labels = []
            for graph, label in graphs:
                graph.GetXaxis().SetLimits(5, 45)
                graph.GetYaxis().SetRangeUser(0, ymax)
                graph.GetXaxis().SetTitle("Number of bins (X)")
                graph.GetYaxis().SetTitle("FOM")
                graph.GetYaxis().SetTitleOffset(1.3)
                graph.SetTitle(f"FOM Scan: M={mass}, {flav}, {era}")
                graph.Draw("APL" if not plotted else "PL")
                if label not in leg_labels:
                    legend.AddEntry(graph, label, "lp")
                    leg_labels.append(label)
                plotted = True

            if v2_fom:
                line = ROOT.TLine(5, v2_fom, 45, v2_fom)
                line.SetLineStyle(2)
                line.SetLineColor(ROOT.kGray + 2)
                line.SetLineWidth(2)
                line.Draw()
                legend.AddEntry(line, "V2 (Legacy)", "l")

            c.SetGrid()
            legend.Draw()
            c.SaveAs(f"{out_dir}/FOMScan_V3V4_{mass}_{flav}_{era}.pdf")

print(f"[DONE] All plots written to: {out_dir}")
