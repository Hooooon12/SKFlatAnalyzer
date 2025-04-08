import ROOT
import os
from pathlib import Path
from array import array

ROOT.gROOT.SetBatch(True)

# === Configuration ===
years = ["2016preVFP", "2016postVFP", "2017", "2018"]
luminosities = {
    "2016preVFP": 19517,
    "2016postVFP": 16812,
    "2017": 41477,
    "2018": 59827
}

base_channels = ["MuMu", "EE", "EMu"]
channels = base_channels

background_files = [
    "HNL_ControlRegion_Plotter_CF.root",
    "HNL_ControlRegion_Plotter_Conv.root",
    "HNL_ControlRegion_Plotter_Fake.root",
    "HNL_ControlRegion_Plotter_Other.root",
    "HNL_ControlRegion_Plotter_TTV.root",
    "HNL_ControlRegion_Plotter_VVV.root",
    "HNL_ControlRegion_Plotter_WW.root",
    "HNL_ControlRegion_Plotter_WZ.root",
    "HNL_ControlRegion_Plotter_ggZZ.root",
    "HNL_ControlRegion_Plotter_qqZZ.root"
]

data_file = "HNL_ControlRegion_Plotter_Data.root"
all_files = background_files + [data_file]

hist_bases = [
    "HNL_HighMassSSZPeak_AK4_TwoLepton_CR",
    "HNL_HighMassSSZPeak_VBF_TwoLepton_CR",
    "HNL_HighMassSSZPeak_AK8_TwoLepton_CR",
    "HNL_WpWpNP_TwoLepton_CR",
    "HNL_WpWpNP2_TwoLepton_CR",
    "HNL_WpWpNP3_TwoLepton_CR",
    "PassCR1_BJet",
    "PassCR1_MET",
    "PassCR2_BJet",
    "PassCR2_MET",
    "PassCR3_MET_HighJet",
    "PassCR3_BJet_HighJet"
]

id_versions = ["POGTight", "HNTightV2", "HNL_ULIDv2"]

Path("results").mkdir(exist_ok=True)

# === Systematic Errors for Different Backgrounds ===
systematic_errors = {
    "HNL_ControlRegion_Plotter_Fake.root": 0.30,  # 30% for Fake
    "HNL_ControlRegion_Plotter_CF.root": 0.20,    # 20% for CF
    "HNL_ControlRegion_Plotter_Other.root": 0.15,  # 15% for Other backgrounds
    "HNL_ControlRegion_Plotter_TTV.root": 0.15,    # 15% for Other backgrounds
    "HNL_ControlRegion_Plotter_VVV.root": 0.15,    # 15% for Other backgrounds
    "HNL_ControlRegion_Plotter_WW.root": 0.15,     # 15% for Other backgrounds
    "HNL_ControlRegion_Plotter_WZ.root": 0.15,     # 15% for Other backgrounds
    "HNL_ControlRegion_Plotter_ggZZ.root": 0.15,   # 15% for Other backgrounds
    "HNL_ControlRegion_Plotter_qqZZ.root": 0.15,   # 15% for Other backgrounds
}


for ID in id_versions:
    for hist_base in hist_bases:
        output_file = f"results/{hist_base}_{ID}_summary.txt"
        with open(output_file, "w") as fout:
            def dual_print(*args, **kwargs):
                print(*args, **kwargs)
                print(*args, **kwargs, file=fout)

            raw_integrals = {
                channel: {filename: {} for filename in all_files + ["TotalBackground"]}
                for channel in channels
            }
            normalized = {
                channel: {filename: {} for filename in all_files + ["TotalBackground"]}
                for channel in channels
            }
            errors = {
                channel: {filename: {} for filename in all_files + ["TotalBackground"]}
                for channel in channels
            }

            for channel in channels:
                for filename in all_files:
                    for year in years:
                        lumi = luminosities[year]
                        filepath = os.path.join(year, filename)

                        if not os.path.isfile(filepath):
                            raw_integrals[channel][filename][year] = None
                            normalized[channel][filename][year] = None
                            errors[channel][filename][year] = None
                            continue

                        file = ROOT.TFile.Open(filepath, "READ")
                        if not file or file.IsZombie():
                            raw_integrals[channel][filename][year] = None
                            normalized[channel][filename][year] = None
                            errors[channel][filename][year] = None
                            continue

                        hist_path = f"{hist_base}/{ID}/{channel}/NObj/N_BJet"
                        hist = file.Get(hist_path)
                        if not hist:
                            raw_integrals[channel][filename][year] = None
                            normalized[channel][filename][year] = None
                            errors[channel][filename][year] = None
                            file.Close()
                            continue

                        err = array('d', [0.])
                        integral = hist.IntegralAndError(0, hist.GetNbinsX() + 1, err)
                        raw_integrals[channel][filename][year] = integral
                        normalized[channel][filename][year] = (integral / lumi) * 1000 if lumi > 0 else 0
                        errors[channel][filename][year] = err[0]
                        file.Close()

            for channel in channels:
                for year in years:
                    raw_total = 0
                    norm_total = 0
                    err_total_sq = 0
                    for bkg_file in background_files:
                        raw = raw_integrals[channel][bkg_file].get(year)
                        norm = normalized[channel][bkg_file].get(year)
                        err = errors[channel][bkg_file].get(year)
                        if raw is not None:
                            raw_total += raw
                        if norm is not None:
                            norm_total += norm
                        if err is not None:
                            # Apply systematic error to the existing error
                            sys_error = systematic_errors.get(bkg_file, 0)
                            total_error = (err**2 + (sys_error * raw)**2)**0.5
                            err_total_sq += total_error**2

                    raw_integrals[channel]["TotalBackground"][year] = raw_total if raw_total > 0 else None
                    normalized[channel]["TotalBackground"][year] = norm_total if norm_total > 0 else None
                    errors[channel]["TotalBackground"][year] = (err_total_sq ** 0.5) if err_total_sq > 0 else None

            dual_print(f"\n\n{'='*10} Histogram Base: {hist_base}, ID: {ID} {'='*10}")
            for filename in background_files + ["TotalBackground", data_file]:
                dual_print(f"\n{filename}")
                for year in years:
                    lumi = luminosities[year]
                    line = f"  {year:<12}:"
                    for base in base_channels:
                        raw = raw_integrals[base][filename].get(year)
                        norm = normalized[base][filename].get(year)
                        err = errors[base][filename].get(year)
                        ref_norm = normalized[base][filename].get("2018")

                        if norm is None or ref_norm is None or ref_norm == 0 or raw is None or err is None:
                            line += f"  {base}: --"
                        else:
                            ratio = norm / ref_norm
                            stat_err = (err / lumi) * 1000
                            line += f"  {base}: Raw: {raw:.1f}, Norm: {norm:.2f} +/- {stat_err:.2f} ({ratio:.2f}),"
                    dual_print(line)

            dual_print(f"\n\n{'='*30} TotalBackground / Data (with stat error) {'='*30}")
            for year in years:
                line = f"{year:<12}:"
                lumi = luminosities[year]
                for channel in channels:
                    total_norm = normalized[channel]["TotalBackground"].get(year)
                    data_norm = normalized[channel][data_file].get(year)
                    total_err = errors[channel]["TotalBackground"].get(year)
                    data_raw = raw_integrals[channel][data_file].get(year)

                    if None in (total_norm, data_norm, total_err, data_raw) or data_norm == 0:
                        line += f"  {channel}: --"
                    else:
                        err_total = (total_err / lumi) * 1000
                        err_data = (data_raw ** 0.5 / lumi) * 1000
                        ratio = total_norm / data_norm
                        rel_err = ratio * ((err_total / total_norm) ** 2 + (err_data / data_norm) ** 2) ** 0.5
                        line += f"  {channel}: {ratio:.3f} +/- {rel_err:.3f}"
                dual_print(line)

        print(f"\nOutput written to: {output_file}")

        era_labels = ["2016preVFP", "2016postVFP", "2017", "2018"]
        era_bins = [0, 1, 2, 3]
        era_offsets = {"MuMu": -0.2, "EE": 0.0, "EMu": 0.2}
        colors = {"MuMu": ROOT.kRed+1, "EE": ROOT.kBlue+1, "EMu": ROOT.kGreen+2}
        markers = {"MuMu": 20, "EE": 21, "EMu": 22}

        canvas = ROOT.TCanvas("c_total_background", f"Total Background: {hist_base}, {ID}", 900, 700)
        canvas.cd()

        legend = ROOT.TLegend(0.65, 0.75, 0.88, 0.9)
        legend.SetBorderSize(0)
        legend.SetFillStyle(0)

        max_y = 0
        graphs = {}
        data_graphs = {}

        for channel in channels:
            x_vals = array('d')
            y_vals = array('d')
            x_errs = array('d')
            y_errs = array('d')

            data_x = array('d')
            data_y = array('d')
            data_ex = array('d')
            data_ey = array('d')

            for i, year in enumerate(years):
                x_bkg = era_bins[i] + era_offsets[channel] - 0.05
                x_data = era_bins[i] + era_offsets[channel] + 0.05

                lumi = luminosities[year]
                norm = normalized[channel]["TotalBackground"].get(year)
                err = errors[channel]["TotalBackground"].get(year)
                norm_data = normalized[channel][data_file].get(year)
                raw_data = raw_integrals[channel][data_file].get(year)

                x_vals.append(x_bkg)
                x_errs.append(0)
                y_vals.append(norm if norm is not None else 0)
                y_errs.append((err / lumi) * 1000 if norm and err else 0)

                data_x.append(x_data)
                data_ex.append(0)
                data_y.append(norm_data if norm_data is not None else 0)
                data_ey.append((raw_data**0.5 / lumi) * 1000 if norm_data and raw_data else 0)

                if norm is not None and err is not None:
                    max_y = max(max_y, norm + y_errs[-1])
                if norm_data is not None and raw_data is not None:
                    max_y = max(max_y, norm_data + data_ey[-1])

            graph = ROOT.TGraphErrors(len(x_vals), x_vals, y_vals, x_errs, y_errs)
            graph.SetName(f"TotalBackground_{channel}")
            graph.SetMarkerStyle(markers[channel])
            graph.SetMarkerColor(colors[channel])
            graph.SetLineColor(colors[channel])
            graph.SetMarkerSize(1.3)
            graphs[channel] = graph

            data_graph = ROOT.TGraphErrors(len(data_x), data_x, data_y, data_ex, data_ey)
            data_graph.SetName(f"Data_{channel}")
            data_graph.SetMarkerStyle(markers[channel] + 4)
            data_graph.SetMarkerColor(colors[channel])
            data_graph.SetLineColor(colors[channel])
            data_graph.SetMarkerSize(1.4)
            data_graphs[channel] = data_graph

        frame = canvas.DrawFrame(-0.5, 0, 3.5, max_y * 1.2)
        frame.SetTitle(f"Total Background vs Data: {hist_base}, {ID}")
        frame.GetXaxis().SetTitle("")
        frame.GetYaxis().SetTitle("Events / fb")
        frame.GetXaxis().SetLabelSize(0)
        frame.GetXaxis().SetTickLength(0)

        for channel in channels:
            graphs[channel].Draw("P same")
            data_graphs[channel].Draw("P same")
            legend.AddEntry(graphs[channel], f"{channel} (Bkg)", "p")
            legend.AddEntry(data_graphs[channel], f"{channel} (Data)", "p")

        line = ROOT.TLine()
        line.SetLineStyle(2)
        line.SetLineWidth(1)
        line.SetLineColor(ROOT.kGray + 2)
        for x in [0.5, 1.5, 2.5]:
            line.DrawLine(x, 0, x, max_y * 1.2)

        latex = ROOT.TLatex()
        latex.SetTextSize(0.035)
        latex.SetTextAlign(22)
        for i, label in enumerate(era_labels):
            latex.DrawLatex(i, -0.045 * max_y, label)

        legend.Draw()
        canvas.Update()

        canvas.Print(f"results_LL/{hist_base}_{ID}_TotalBackground_AllChannels.png")
        canvas.Print(f"results_LL/{hist_base}_{ID}_TotalBackground_AllChannels.pdf")

        print(f"Saved canvas and graphs to: results/{hist_base}_{ID}_graphs.root")
