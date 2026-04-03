import os
import re
import ROOT

# Configuration
masses = ['100', '125', '150', '200', '250', '300', '400', '500']
flavours = ['MuMu', 'EE', 'EMu']
eras = ['2016preVFP', '2016postVFP', '2017', '2018']
log_dir = "logs"
out_dir = "plots_fomratios"
os.makedirs(out_dir, exist_ok=True)

# Regex patterns
unscanned_re = re.compile(r"\[SUMMARY\] \[.*_(V[34]) M(\d+)\] Summed Azimov FOM from LimitBins: ([\d.]+)")
scanned_re = re.compile(r"\[RESULT\] Best total FOM (V[34]): ([\d.]+)")

# Result storage
results = {}

# Loop over all log files
for era in eras:
    for flav in flavours:
        for mass in masses:
            def extract_foms(tag, key):
                fname = f"{tag}_{mass}_{flav}_{era}_25_20.out"
                path = os.path.join(log_dir, fname)
                print(f"[INFO] Checking: {path}")
                if not os.path.exists(path):
                    print(f"[MISSING] File not found: {path}")
                    return {}

                found = {}
                with open(path) as f:
                    for line in f:
                        line = line.strip()
                        if key == "unscanned":
                            match = unscanned_re.search(line)
                            if match:
                                ver, _, fom = match.groups()
                                if ver not in found:
                                    found[ver] = float(fom)
                                    print(f"  [FOUND] {key} {ver}: {fom}")
                                    if len(found) == 2:
                                        break
                        else:
                            match = scanned_re.search(line)
                            if match:
                                ver, fom = match.groups()
                                if ver not in found:
                                    found[ver] = float(fom)
                                    print(f"  [FOUND] {key} {ver}: {fom}")
                                    if len(found) == 2:
                                        break

                if not found:
                    pat = unscanned_re.pattern if key == "unscanned" else scanned_re.pattern
                    print(f"  [WARN] No {key} FOMs found in {fname}")
                    print(f"         Searched with pattern: {pat}")

                for v in ["V3", "V4"]:
                    if v not in found:
                        print(f"  [WARN] {key} {v} not found in {fname}")

                return found

            results.setdefault(era, {}).setdefault(flav, {}).setdefault(mass, {})
            d = results[era][flav][mass]
            d['unscanned'] = extract_foms("scan", "unscanned")
            d['scanned'] = extract_foms("scan", "scanned")
            d['loose'] = extract_foms("scan_loose", "scanned")
            d['very_loose'] = extract_foms("scan_very_loose", "scanned")

# Plot
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

for era in eras:
    for flav in flavours:
        g1 = ROOT.TGraph()
        g2 = ROOT.TGraph()
        g3 = ROOT.TGraph()
        g4 = ROOT.TGraph()
        g5 = ROOT.TGraph()
        idx = 0

        for mass in masses:
            d = results.get(era, {}).get(flav, {}).get(mass, {})
            if not all(k in d for k in ['unscanned', 'scanned']):
                continue
            v3_unscan = d['unscanned'].get('V3')
            v4_unscan = d['unscanned'].get('V4')
            v3_scan = d['scanned'].get('V3')
            v4_scan = d['scanned'].get('V4')
            v4_loose = d['loose'].get('V4')
            v4_very = d['very_loose'].get('V4')

            if not all([v3_unscan, v4_unscan, v3_scan, v4_scan]):
                continue

            x = float(mass)
            g1.SetPoint(idx, x, v4_unscan / v3_unscan)
            g2.SetPoint(idx, x, v4_scan / v3_scan)
            g3.SetPoint(idx, x, v4_scan / v4_unscan)
            if v4_loose is not None:
                g4.SetPoint(idx, x, v4_loose / v4_scan)
                print(f"Setting v4_loose")
            if v4_very is not None:
                g5.SetPoint(idx, x, v4_very / v4_scan)

            idx += 1

        if idx == 0:
            continue

        c = ROOT.TCanvas("c", "", 800, 600)
        colors = [ROOT.kRed, ROOT.kBlue, ROOT.kGreen+2, ROOT.kMagenta+2, ROOT.kOrange+1]
        labels = [
            "V4/V3 (Unscanned)",
            "V4/V3 (Scanned)",
            "V4 Scanned / V4 Unscanned",
            "Loose V4 / V4 Scanned",
            "VeryLoose V4 / V4 Scanned"
        ]
        graphs = [g1, g2, g3, g4, g5]

        legend = ROOT.TLegend(0.52, 0.68, 0.88, 0.88)
        legend.SetBorderSize(0)
        legend.SetFillStyle(0)

        for i, gr in enumerate(graphs):
            if gr.GetN() == 0:
                print(f"[WARN] Empty graph for {labels[i]} - skipping draw.")
                continue
            gr.SetLineColor(colors[i])
            gr.SetMarkerColor(colors[i])
            gr.SetMarkerStyle(20 + i)
            gr.SetLineWidth(2)
            if i == 0:
                gr.SetTitle(f"FOM Ratios: {flav}, {era}")
                gr.GetXaxis().SetTitle("Mass [GeV]")
                gr.GetYaxis().SetTitle("Ratio")
                gr.GetXaxis().SetLimits(80, 520)
                gr.GetYaxis().SetRangeUser(0.2, 1.8)
                gr.Draw("APL")
            else:
                gr.Draw("PL SAME")
            legend.AddEntry(gr, labels[i], "lp")
            print (f"Adding entry {labels[i]}")
        c.SetGrid()
        legend.Draw()
        c.SaveAs(f"{out_dir}/FOMRatios_{flav}_{era}.pdf")

print("[DONE] Ratio plots written to:", out_dir)
