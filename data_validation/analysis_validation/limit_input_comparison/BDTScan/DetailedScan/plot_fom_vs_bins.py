import os
import re
import ROOT

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

# Configurations
masses = ['100', '125', '200', '250', '300', '400', '500']
flavours = ['MuMu', 'EE', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']
log_dir = "LooseScan/logs"
out_dir = "plots"

if not os.path.exists(out_dir):
    os.makedirs(out_dir)

# Regex pattern to extract bin and FOM from result line
pattern = re.compile(r"\[RESULT\] Best total FOM: ([\d.]+) using (\d+) bins")

# Loop over all scan sets
for mass in masses:
    for flav in flavours:
        for era in eras:
            x_vals = []
            y_vals = []
            for nbin in range(5, 21):
                filename = f"scan_{mass}_{flav}_{era}_{nbin}.out"
                filepath = os.path.join(log_dir, filename)

            
                if not os.path.exists(filepath):
                    continue

                with open(filepath, "r") as f:
                    for line in f:
                        if "[RESULT] Best total FOM:" in line:
                            match = pattern.search(line)
                            if match:
                                fom = float(match.group(1))
                                bins = int(match.group(2))
                                x_vals.append(bins)
                                y_vals.append(fom)
                            break  # Only take the first result line

            if len(x_vals) < 2:
                continue  # Skip if not enough points to plot

            # Create ROOT graph
            graph = ROOT.TGraph(len(x_vals))
            for i, (x, y) in enumerate(sorted(zip(x_vals, y_vals))):
                graph.SetPoint(i, x, y)

            graph.SetTitle(f"FOM vs Bins: {mass} {flav} {era}")
            graph.GetXaxis().SetTitle("Number of Bins")
            graph.GetYaxis().SetTitle("Total FOM")

            canvas = ROOT.TCanvas("c", "c", 800, 600)
            graph.SetMarkerStyle(20)
            graph.SetLineWidth(2)
            graph.Draw("APL")
            canvas.SaveAs(f"{out_dir}/FOMScan_{mass}_{flav}_{era}.pdf")
            del canvas  # Prevent overwriting issues            
