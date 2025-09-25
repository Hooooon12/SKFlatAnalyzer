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
            fname = f"scan_{mass}_{flav}_{era}_25_20.out"
            path = os.path.join(log_dir, fname)
            if not os.path.exists(path):
                continue

            results.setdefault(era, {}).setdefault(flav, {}).setdefault(mass, {'V3': {}, 'V4': {}})
            found_unscanned, found_scanned = set(), set()

            with open(path, "r") as f:
                for line in f:
                    u_match = unscanned_re.search(line)
                    if u_match:
                        ver, m, fom = u_match.groups()
                        if ver not in found_unscanned:
                            results[era][flav][mass][ver]['unscanned'] = float(fom)
                            found_unscanned.add(ver)
                    s_match = scanned_re.search(line)
                    if s_match:
                        ver, fom = s_match.groups()
                        if ver not in found_scanned:
                            results[era][flav][mass][ver]['scanned'] = float(fom)
                            found_scanned.add(ver)
                    if found_unscanned == {"V3", "V4"} and found_scanned == {"V3", "V4"}:
                        break

# Plot
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

for era in eras:
    for flav in flavours:
        gr_unscanned = ROOT.TGraph()
        gr_scanned = ROOT.TGraph()
        gr_ratio = ROOT.TGraph()
        point = 0

        for mass in masses:
            d = results.get(era, {}).get(flav, {}).get(mass, {})
            if not d or 'V3' not in d or 'V4' not in d:
                continue
            if 'unscanned' not in d['V3'] or 'unscanned' not in d['V4']:
                continue
            if 'scanned' not in d['V3'] or 'scanned' not in d['V4']:
                continue

            unscan_v3 = d['V3']['unscanned']
            unscan_v4 = d['V4']['unscanned']
            scan_v3 = d['V3']['scanned']
            scan_v4 = d['V4']['scanned']

            ratio_unscanned = unscan_v4 / unscan_v3 if unscan_v3 > 0 else 0
            ratio_scanned = scan_v4 / scan_v3 if scan_v3 > 0 else 0
            ratio_ratio = ratio_scanned / ratio_unscanned if ratio_unscanned > 0 else 0

            gr_unscanned.SetPoint(point, float(mass), ratio_unscanned)
            gr_scanned.SetPoint(point, float(mass), ratio_scanned)
            gr_ratio.SetPoint(point, float(mass), ratio_ratio)
            point += 1

        if point == 0:
            continue

        c = ROOT.TCanvas("c", "", 800, 600)
        gr_unscanned.SetLineColor(ROOT.kRed)
        gr_unscanned.SetMarkerColor(ROOT.kRed)
        gr_unscanned.SetMarkerStyle(20)
        gr_unscanned.SetLineWidth(2)

        gr_scanned.SetLineColor(ROOT.kBlue)
        gr_scanned.SetMarkerColor(ROOT.kBlue)
        gr_scanned.SetMarkerStyle(21)
        gr_scanned.SetLineWidth(2)

        gr_ratio.SetLineColor(ROOT.kGreen+2)
        gr_ratio.SetMarkerColor(ROOT.kGreen+2)
        gr_ratio.SetMarkerStyle(22)
        gr_ratio.SetLineWidth(2)

        gr_unscanned.SetTitle(f"FOM Ratio V4/V3: {flav}, {era}")
        gr_unscanned.GetXaxis().SetTitle("Mass [GeV]")
        gr_unscanned.GetYaxis().SetTitle("Ratio")
        gr_unscanned.GetYaxis().SetTitleOffset(1.3)
        gr_unscanned.GetXaxis().SetLimits(80, 520)
        gr_unscanned.GetYaxis().SetRangeUser(0.6, 1.6)
        gr_unscanned.Draw("APL")
        gr_scanned.Draw("PL SAME")
        gr_ratio.Draw("PL SAME")

        leg = ROOT.TLegend(0.55, 0.75, 0.88, 0.88)
        leg.SetBorderSize(0)
        leg.SetFillStyle(0)
        leg.AddEntry(gr_unscanned, "V4/V3 (Unscanned)", "lp")
        leg.AddEntry(gr_scanned, "V4/V3 (Scanned)", "lp")
        leg.AddEntry(gr_ratio, "(V4 Scanned) / (V4 Unscanned)", "lp")
        leg.Draw()

        c.SetGrid()
        c.SaveAs(f"{out_dir}/FOMRatio_{flav}_{era}.pdf")

print("[DONE] Plots saved to:", out_dir)
