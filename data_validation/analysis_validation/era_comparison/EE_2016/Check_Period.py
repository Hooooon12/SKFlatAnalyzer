import ROOT
import os
from pathlib import Path
from array import array

ROOT.gROOT.SetBatch(True)

# === Configuration ===
eras = ["2016preVFP", "2016postVFP", "2017", "2018"]

# Data file periods and their corresponding luminosities
all_files = [
    ["2016preVFP", "B_ver2", 5.75],
    ["2016preVFP", "C", 2.57],
    ["2016preVFP", "D", 4.24],
    ["2016preVFP", "E", 4.02],
    ["2016postVFP", "G", 7.58],
    ["2016postVFP", "H", 8.65],
    ["2017", "B", 4.79],
    ["2017", "C", 9.63],
    ["2017", "D", 4.25],
    ["2017", "E", 9.31],
    ["2017", "F", 13.54],
    ["2018", "A", 14.03],
    ["2018", "B", 7.06],
    ["2018", "C", 6.90],
    ["2018", "D", 31.74]
]

hist_bases = [
    ["HNL_WZ_SR1_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_WZVBF_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_WZVBF2_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_WZ_SR3_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_WZB_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_ZNPEl_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_ZNPMu_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_WG_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
    ["HNL_ZG_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL"]],
]

id_versions = ["POGTight", "HNTightV2", "HNL_ULIDv2"]

Path("results_period").mkdir(exist_ok=True)

# === Process Only Data Files ===
for ID in id_versions:
    for hist_base, flavour, channels in hist_bases:
        Path(f"results_period_{flavour}").mkdir(exist_ok=True)
        output_file = f"results_period/{hist_base}_{ID}_summary.txt"
        with open(output_file, "w") as fout:
            def dual_print(*args, **kwargs):
                print(*args, **kwargs)
                print(*args, **kwargs, file=fout)

            # Create histograms for each channel with bins for each file
            histograms = {
                channel: ROOT.TH1F(f"{channel}_hist", f"{channel}_hist", len(all_files), 0, len(all_files))
                for channel in channels
            }

            # Process the data files for each split year and period
            for channel in channels:
                for i, (era, period, lumi) in enumerate(all_files):
                    filepath = os.path.join(era, f"HNL_ControlRegion_Plotter_{period}_Data.root")

                    if not os.path.isfile(filepath):
                        continue

                    file = ROOT.TFile.Open(filepath, "READ")
                    if not file or file.IsZombie():
                        continue

                    hist_path = f"{hist_base}/{ID}/{channel}/NObj/N_BJet"
                    hist = file.Get(hist_path)
                    if not hist:
                        file.Close()
                        continue

                    # Calculate integral and error for each file
                    err = array('d', [0.])
                    integral = hist.IntegralAndError(0, hist.GetNbinsX() + 1, err)
                    norm_integral = (integral / lumi) if lumi > 0 else 0

                    # Fill the histogram for the current channel and period
                    histograms[channel].SetBinContent(i + 1, norm_integral)  # +1 because ROOT histograms are 1-based
                    histograms[channel].SetBinError(i + 1, err[0] / lumi if lumi > 0 else 0)

                    file.Close()

            # Save histograms and print summary
            dual_print(f"\n\n{'='*10} Histogram Base: {hist_base}, ID: {ID} {'='*10}")
            for channel in channels:
                dual_print(f"\nHistogram for {channel}:")
                for i, (era, period, _) in enumerate(all_files):
                    norm_data = histograms[channel].GetBinContent(i + 1)
                    error_data = histograms[channel].GetBinError(i + 1)
                    dual_print(f"  {era}_{period}: {norm_data:.2f} +/- {error_data:.2f}")

                # Canvas setup for plotting each channel's histogram
                canvas = ROOT.TCanvas(f"canvas_{channel}", f"Canvas for {channel}", 900, 700)
                canvas.SetTopMargin(0.05)
                canvas.SetBottomMargin(0.15)

                histograms[channel].SetTitle(f"{channel} Normalized Integral")
                histograms[channel].GetXaxis().SetTitle("Era + Period")
                histograms[channel].GetYaxis().SetTitle("Normalized Integral")
                histograms[channel].SetStats(False)  # Remove the stats box
                histograms[channel].GetXaxis().SetNdivisions(len(all_files), ROOT.kTRUE)

                # Set custom labels for x-axis
                for i, (era, period, _) in enumerate(all_files):
                    histograms[channel].GetXaxis().SetBinLabel(i + 1, f"{era} {period}")

                histograms[channel].Draw()

                # Draw vertical lines to separate eras (positions at x = 4, 6, 11)
                line = ROOT.TLine()
                line.SetLineStyle(2)  # Dashed line
                line.SetLineWidth(1)
                line.SetLineColor(ROOT.kGray + 2)
                x_positions = [4, 6, 11]  # Positions between eras
                max_y = histograms[channel].GetMaximum() * 1.2  # Set the top to be 20% above the maximum Y value
                for x in x_positions:
                    line.DrawLine(x, 0, x, max_y)  # Extend the line to the top of the Y-axis

                # Save histogram as PDF
                pdf_filename = f"results_period_{flavour}/{hist_base}_{ID}_{channel}_histogram.pdf"
                canvas.SaveAs(pdf_filename)
                print(f"Saved {pdf_filename}")

        print(f"\nOutput written to: {output_file}")
