import ROOT
import os
from pathlib import Path
from array import array
import logging
import sys
import mylib
import math

import canvas_margin
import CMS_lumi, tdrstyle

# ---------------- Logging ----------------
def log_section_start(section_name):
    logging.info("-" * 50)
    logging.info(f"STARTING SUBSECTION: {section_name}")
    logging.info("-" * 50)

logging.basicConfig(
    level=logging.INFO,
    format="Plotter:  %(asctime)s - %(levelname)s - %(message)s",
    handlers=[logging.StreamHandler()]
)

# ---------------- Helpers ----------------
def draw_fixed_boundaries(pad, x_positions):
    pad.cd()
    pad.Update()
    y_min = pad.GetUymin()
    y_max = pad.GetUymax()
    lines = []
    for x in x_positions:
        ln = ROOT.TLine(x, y_min, x, y_max)
        ln.SetLineColor(ROOT.kBlack)
        ln.SetLineStyle(2)
        ln.SetLineWidth(2)
        ln.Draw("same")
        lines.append(ln)
    pad.RedrawAxis()
    pad.Update()
    return lines

def make_legend(gr_Data_dummy, background_files):
    legend = ROOT.TLegend(0.2, 0.5, 0.35, 0.8)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.03)
    legend.AddEntry(gr_Data_dummy, "Data", "ep")
    for bkg_file, label in background_files:
        legend.AddEntry(histograms[bkg_file], label, "f")
    return legend

def fill_hist_labels(canvas, ymax):
    eras = [f"{years[0]}", f"{years[1]}", f"{years[2]}", f"{years[3]}"]
    bin_positions = [4.0, 8.0, 12.0, 16.0]

    line = ROOT.TLine(4.0, 0, 4.0, ymax)
    line.SetLineColor(ROOT.kBlack)
    line.SetLineStyle(2)
    line.Draw("same")

    line2 = ROOT.TLine(8.0, 0, 8.0, ymax)
    line2.SetLineColor(ROOT.kBlack)
    line2.SetLineStyle(2)
    line2.Draw("same")

    line3 = ROOT.TLine(12.0, 0, 12.0, ymax)
    line3.SetLineColor(ROOT.kBlack)
    line3.SetLineStyle(2)
    line3.Draw("same")

    for i, pos in enumerate(bin_positions):
        latex = ROOT.TLatex()
        latex.SetTextSize(0.03)
        latex.SetTextAlign(22)
        latex.DrawLatex(pos - 2, ymax * 0.9, eras[i])

    canvas.Update()
    return line, line2, line3

def create_dummy_pads(bin_labels, y_max):
    dummy_up = ROOT.TH1F("h_dummy_up", "Dummy Histogram for Axis", 16, 0, 16)
    for i_bin, bin_channel in enumerate(bin_labels):
        dummy_up.GetXaxis().SetBinLabel(i_bin + 1, bin_channel)
        dummy_up.GetXaxis().SetBinLabel(i_bin + 5, bin_channel)
        dummy_up.GetXaxis().SetBinLabel(i_bin + 9, bin_channel)
        dummy_up.GetXaxis().SetBinLabel(i_bin + 13, bin_channel)
    dummy_up.LabelsOption("v")
    dummy_up.SetTitle("Stacked Backgrounds: Channels")
    dummy_up.GetXaxis().SetTitle("Channels")
    dummy_up.GetYaxis().SetTitle("Events")

    dummy_down = ROOT.TH1D("h_dumy_down", "", 16, 0, 16)
    dummy_down.GetYaxis().SetRangeUser(0.5, 1.5)
    dummy_down.SetNdivisions(504, "Y")
    for i_bin, bin_channel in enumerate(bin_labels):
        dummy_down.GetXaxis().SetBinLabel(i_bin + 1, bin_channel)
        dummy_down.GetXaxis().SetBinLabel(i_bin + 5, bin_channel)
        dummy_down.GetXaxis().SetBinLabel(i_bin + 9, bin_channel)
        dummy_down.GetXaxis().SetBinLabel(i_bin + 13, bin_channel)
    dummy_down.LabelsOption("v")
    dummy_down.GetXaxis().SetTitle("")
    dummy_down.GetYaxis().SetTitle("#frac{Data}{Sim.}")
    dummy_down.SetFillColor(0)
    dummy_down.SetMarkerSize(0)
    dummy_down.SetMarkerStyle(0)
    dummy_down.SetLineColor(ROOT.kWhite)

    dummy_up, dummy_down = canvas_margin.hist_axis(dummy_up, dummy_down)
    dummy_up.GetYaxis().SetRangeUser(0, y_max)
    return dummy_up, dummy_down

def MakeDataHist(years, channels, data_files, input_dir, hist_base):
    h_Data = ROOT.TH1F("hist_data", "Data", 16, 0, 16)
    h_Data.SetMarkerStyle(20)
    h_Data.SetMarkerSize(1.2)
    h_Data.SetMarkerColor(ROOT.kBlack)
    h_Data.SetLineColor(ROOT.kBlack)

    if len(data_files) == 0:
        logging.error("No data files provided.")
        sys.exit(1)

    for year_idx, year in enumerate(years):
        for channel_idx, channel in enumerate(channels):
            hist_path = f"{hist_base}/HNL_ULIDv2/{channel}/NObj/N_BJet"
            logging.info(f"Filling bins for {year}:{channel}")
            data_file = data_files[0]
            data_file_path = os.path.join(input_dir, year, data_file)
            if not os.path.isfile(data_file_path):
                logging.warning(f"Missing file {data_file_path}")
                sys.exit(1)
            file = ROOT.TFile.Open(data_file_path, "READ")
            if not file or file.IsZombie():
                logging.warning(f"Failed to open file {data_file_path}")
                sys.exit(1)
            data_hist = file.Get(hist_path)
            bin_idx = year_idx * 4 + channel_idx
            if not data_hist:
                logging.warning(f"Histogram {hist_path} missing in {data_file_path}")
                d_data_err = 1.8
                data_norm = 0.0
                h_Data.SetBinContent(bin_idx + 1, data_norm)
                h_Data.SetBinError(bin_idx + 1, d_data_err)
            else:
                data_err = array("d", [0.0])
                data_norm = data_hist.IntegralAndError(1, data_hist.GetNbinsX(), data_err)
                h_Data.SetBinContent(bin_idx + 1, data_norm)
                h_Data.SetBinError(bin_idx + 1, data_err[0])
            logging.info(f"Setting Data bin_idx={bin_idx} norm={data_norm}")
            file.Close()
    return h_Data

def process_histograms(years, channels, input_dir, background_files, hist_base):
    for bkg_file, label in background_files:
        if bkg_file not in histograms:
            histograms[bkg_file] = ROOT.TH1F(f"hist_{bkg_file}", label, 16, 0, 16)
            logging.info(f"Creating histogram for {bkg_file}")

    for year_idx, year in enumerate(years):
        for channel_idx, channel in enumerate(channels):
            logging.info(f"Filling bins for {year}:{channel}")
            hist_path = f"{hist_base}/HNL_ULIDv2/{channel}/NObj/N_BJet"
            for bkg_file, label in background_files:
                file_path = os.path.join(input_dir, year, bkg_file)
                if not os.path.isfile(file_path):
                    logging.warning(f"Skipping {bkg_file}, missing {file_path}")
                    continue
                file = ROOT.TFile.Open(file_path, "READ")
                hist = file.Get(hist_path)
                if not hist:
                    logging.warning(f"Skipping {bkg_file}, missing hist {hist_path}")
                    file.Close()
                    continue
                err = array("d", [0.0])
                norm = hist.IntegralAndError(1, hist.GetNbinsX(), err)
                uncer = 0.10
                if "WW" in file_path:
                    uncer = 0.20
                if "VVV" in file_path:
                    uncer = 0.30
                if "ZZ" in file_path:
                    uncer = 0.15
                if "Fake" in file_path:
                    uncer = 0.30
                if "CF" in file_path:
                    uncer = 0.25
                if "Conv" in file_path:
                    uncer = 0.20
                d_err = math.sqrt(err[0] ** 2 + (uncer * norm) ** 2)
                bin_idx = year_idx * 4 + channel_idx
                histograms[bkg_file].SetBinContent(bin_idx + 1, norm)
                histograms[bkg_file].SetBinError(bin_idx + 1, d_err)
                logging.info(f"Set {bkg_file} bin={bin_idx} norm={norm} ch={channel_idx} year={year}")
                file.Close()
    return histograms

def get_ratio_plots(h_Data, h_Bkgd, err_up_tmp, err_down_tmp, gr_Bkgd_TotErr):
    h_Data_Ratio = h_Data.Clone("h_Data_Ratio")
    tmp_h_Data_Ratio = h_Data.Clone()
    tmp_h_Data_Ratio.Divide(h_Bkgd)

    gr_Data_Ratio = ROOT.TGraphAsymmErrors(tmp_h_Data_Ratio)
    gr_Data_Ratio.SetName("gr_Data_Ratio")
    gr_Data_Ratio.SetLineWidth(2)
    gr_Data_Ratio.SetMarkerSize(0.0)
    gr_Data_Ratio.SetLineColor(ROOT.kBlack)

    gr_Bkgd_Ratio = gr_Bkgd_TotErr.Clone("gr_Bkgd_Ratio")

    for i in range(1, h_Data_Ratio.GetXaxis().GetNbins() + 1):
        if h_Bkgd.GetBinContent(i) != 0:
            h_Data_Ratio.SetBinContent(i, h_Data_Ratio.GetBinContent(i) / h_Bkgd.GetBinContent(i))
            h_Data_Ratio.SetBinError(i, h_Data_Ratio.GetBinError(i) / h_Bkgd.GetBinContent(i))
            if err_down_tmp[i - 1] != 0.0:
                gr_Data_Ratio.SetPointEYlow(i - 1, err_down_tmp[i - 1] / h_Bkgd.GetBinContent(i))
                gr_Data_Ratio.SetPointEYhigh(i - 1, err_up_tmp[i - 1] / h_Bkgd.GetBinContent(i))
            else:
                gr_Data_Ratio.SetPointEYlow(i - 1, 0)
                gr_Data_Ratio.SetPointEYhigh(i - 1, 1.8 / h_Bkgd.GetBinContent(i))
            gr_Bkgd_Ratio.SetPoint(i - 1, h_Bkgd.GetXaxis().GetBinCenter(i), 1.0)
            gr_Bkgd_Ratio.SetPointEYhigh(i - 1, gr_Bkgd_Ratio.GetErrorYhigh(i - 1) / h_Bkgd.GetBinContent(i))
            gr_Bkgd_Ratio.SetPointEYlow(i - 1, gr_Bkgd_Ratio.GetErrorYlow(i - 1) / h_Bkgd.GetBinContent(i))
        elif h_Bkgd.GetBinContent(i) == 0. and h_Data_Ratio.GetBinContent(i) == 0.:
            h_Data_Ratio.SetBinContent(i, 0)
            h_Data_Ratio.SetBinError(i, 0)
            gr_Data_Ratio.SetPoint(i - 1, 0, 0)
            gr_Data_Ratio.SetPointEYlow(i - 1, 0)
            gr_Data_Ratio.SetPointEYhigh(i - 1, 0)
            gr_Bkgd_Ratio.SetPoint(i - 1, h_Bkgd.GetXaxis().GetBinCenter(i), 1.0)
            gr_Bkgd_Ratio.SetPointEYhigh(i - 1, 0.0)
            gr_Bkgd_Ratio.SetPointEYlow(i - 1, 0.0)
        else:
            this_max_ratio = 20.0
            this_data = h_Data_Ratio.GetBinContent(i)
            this_data_err = h_Data_Ratio.GetBinError(i)
            h_Data_Ratio.SetBinContent(i, this_max_ratio)
            h_Data_Ratio.SetBinError(i, this_data_err * this_max_ratio / this_data if this_data != 0 else 0.0)
            gr_Bkgd_Ratio.SetPoint(i - 1, h_Bkgd.GetXaxis().GetBinCenter(i), 1.0)
            gr_Bkgd_Ratio.SetPointEYhigh(i - 1, 0.0)
            gr_Bkgd_Ratio.SetPointEYlow(i - 1, 0.0)

        gr_Bkgd_Ratio.SetMarkerColor(0)
        gr_Bkgd_Ratio.SetMarkerSize(0)
        gr_Bkgd_Ratio.SetFillStyle(3013)
        gr_Bkgd_Ratio.SetFillColor(ROOT.kBlack)
        gr_Bkgd_Ratio.SetLineColor(0)

    return h_Data_Ratio, gr_Data_Ratio, gr_Bkgd_Ratio

def get_graphs(scaled_up_hist, scaled_down_hist, h_Data):
    gr_Bkgd_TotErr = mylib.GetAsymmError(scaled_up_hist, scaled_down_hist)
    gr_Bkgd_TotErr.SetMarkerColor(0)
    gr_Bkgd_TotErr.SetMarkerSize(0)
    gr_Bkgd_TotErr.SetFillStyle(3013)
    gr_Bkgd_TotErr.SetFillColor(ROOT.kBlack)
    gr_Bkgd_TotErr.SetLineColor(0)

    gr_Data = ROOT.TGraphAsymmErrors(h_Data)
    gr_Data.SetLineWidth(2)
    gr_Data.SetMarkerSize(0.0)
    gr_Data.SetMarkerColor(ROOT.kBlack)
    gr_Data.SetLineColor(ROOT.kBlack)
    return gr_Bkgd_TotErr, gr_Data

def get_hists_from_stack(stacked_hist, uncertainty):
    bkg_hist = stacked_hist.GetHists().At(0).Clone("sum_hist")
    for i in range(1, stacked_hist.GetHists().GetSize()):
        hist = stacked_hist.GetHists().At(i)
        bkg_hist.Add(hist)
    scaled_up_hist = bkg_hist.Clone("scaled_up_hist")
    scaled_down_hist = bkg_hist.Clone("scaled_down_hist")
    for i in range(1, bkg_hist.GetNbinsX() + 1):
        content = bkg_hist.GetBinContent(i)
        error = bkg_hist.GetBinError(i)
        scaled_up_hist.SetBinContent(i, content + error)
        scaled_down_hist.SetBinContent(i, max(0.0, content - error))
    return bkg_hist, scaled_up_hist, scaled_down_hist

def set_graph_data_bins(gr_Data):
    alpha = 1.0 - 0.6827
    err_down_tmp = []
    err_up_tmp = []
    for i in range(0, gr_Data.GetN()):
        N = gr_Data.GetY()[i]
        L = 0.0 if (N == 0.0) else (ROOT.Math.gamma_quantile(alpha / 2.0, N, 1.0))
        U = (ROOT.Math.gamma_quantile_c(alpha, N + 1, 1.0)) if (N == 0.0) else (ROOT.Math.gamma_quantile_c(alpha / 2.0, N + 1.0, 1.0))
        if N != 0:
            gr_Data.SetPointEYlow(i, N - L)
            gr_Data.SetPointEYhigh(i, U - N)
            err_down_tmp.append(N - L)
            err_up_tmp.append(U - N)
        else:
            zerodata_err_low = 0.1
            zerodata_err_high = 1.8
            gr_Data.SetPointEYlow(i, zerodata_err_low)
            gr_Data.SetPointEYhigh(i, zerodata_err_high)
            err_down_tmp.append(zerodata_err_low)
            err_up_tmp.append(zerodata_err_high)
    return err_up_tmp, err_down_tmp

def cleanup_basic(objects):
    print("Performing cleanup before exit...")
    for obj in objects:
        if obj:
            print(f"Deleting {obj.GetName() if hasattr(obj, 'GetName') else 'object'}")
            del obj
        else:
            print("Object is None, not deleting.")
    print("Checking for remaining objects in ROOT's cleanup list...")
    remaining_objects = ROOT.gROOT.GetListOfCleanups()
    remaining_size = remaining_objects.GetSize()
    return

# ---------------- Main setup ----------------
ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kFatal

years = ["2016preVFP", "2016postVFP", "2017", "2018"]
luminosities = {
    "2016preVFP": 19517,
    "2016postVFP": 16812,
    "2017": 41477,
    "2018": 59827
}

# LLLL-only mode
hist_bases = [
    ["HNL_ZZ_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
    ["HNL_ZZ_AK8_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
    ["HNL_ZZVBF_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
    ["HNL_ZZ_OrigKF_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
]
bin_labels = ["#mu#mu#mu#mu", "eeee", "e#mull", "llll"]

background_files = [
    ["HNL_ControlRegion_Plotter_CF.root", "Chargeflip"],
    ["HNL_ControlRegion_Plotter_Conv.root", "X#gamma"],
    ["HNL_ControlRegion_Plotter_Fake.root", "Nonprompt"],
    ["HNL_ControlRegion_Plotter_TTV.root", "TTV"],
    ["HNL_ControlRegion_Plotter_VVV.root", "VVV"],
    ["HNL_ControlRegion_Plotter_WZ.root", "WZ"],
    ["HNL_ControlRegion_Plotter_WW.root", "WW"],
    ["HNL_ControlRegion_Plotter_ZZ.root", "ZZ"],
    ["HNL_ControlRegion_Plotter_Other.root", "Other"],
]

colors = {
    "Chargeflip": ROOT.kYellow,
    "W#gamma": ROOT.kBlue,
    "Z#gamma": ROOT.kGreen + 2,
    "X#gamma": ROOT.kMagenta,
    "Nonprompt": 870,
    "Other": ROOT.kOrange,
    "tHq": ROOT.kViolet,
    "TTV": ROOT.kPink + 1,
    "VVV": ROOT.kMagenta,
    "WW": ROOT.kGray,
    "WZ": ROOT.kGreen,
    "ggZZ": ROOT.kSpring + 1,
    "qqZZ": ROOT.kTeal,
    "ZZ": ROOT.kTeal,
}

data_files = ["HNL_ControlRegion_Plotter_Data.root"]
data_file = "HNL_ControlRegion_Plotter_Data.root"
all_files = [file[0] for file in background_files] + [data_file]

import argparse
parser = argparse.ArgumentParser(description="LLLL-only ratio plotter")
# Keep only the plot-version override; no LL/LLL toggles anymore
directory_path = "/data6/Users/jalmond/2020/HL_SKFlatAnalyzer_UL_LONG/SKFlatAnalyzer/data_validation/analysis_validation/merged_samples/"
directories = [d for d in os.listdir(directory_path) if os.path.isdir(os.path.join(directory_path, d))]
if not directories:
    raise ValueError("No directories found in the specified path!")
parser.add_argument("--plot-version", type=str, choices=directories, help="Select the directory for Plot_Version")
args = parser.parse_args()

if args.plot_version:
    Plot_Version = args.plot_version
else:
    print("Available directories:")
    for idx, directory in enumerate(directories, 1):
        print(f"{idx}. {directory}")
    while True:
        try:
            choice = int(input(f"Select a directory by entering the number (1-{len(directories)}): "))
            if 1 <= choice <= len(directories):
                Plot_Version = directories[choice - 1]
                print(f"Selected Plot_Version: {Plot_Version}")
                break
            else:
                print("Invalid choice. Please select a valid number.")
        except ValueError:
            print("Invalid input. Please enter a number.")

input_dir = os.path.join(directory_path, Plot_Version) + "/"

Path("results_ratio/").mkdir(exist_ok=True)
Path(f"results_ratio/{Plot_Version}").mkdir(exist_ok=True)

tdrstyle.setTDRStyle()
log_section_start("Looping on Regions")

def print_era_ratios(h_data, h_bkg, years_list):
    logging.info("Per-era Data/Bkg ratios:")
    for era_idx, era_name in enumerate(years_list):
        start_bin = era_idx * 4 + 1
        end_bin = start_bin + 3
        d_sum = h_data.Integral(start_bin, end_bin)
        b_sum = h_bkg.Integral(start_bin, end_bin)
        ratio = d_sum / b_sum if b_sum > 0 else float("nan")
        logging.info(f"  {era_name}: Data={d_sum:.3f}, Bkg={b_sum:.3f}, Data/Bkg={ratio:.3f}")
        print(f"[RATIO] Era {era_name}: Data={d_sum:.3f}, Bkg={b_sum:.3f}, Data/Bkg={ratio:.3f}")

try:
    for hist_base, flavour, channels in hist_bases:
        log_section_start(f"Running {hist_base}")
        output_file = f"results_ratio/{Plot_Version}/{hist_base}_{flavour}_ratio_stacked_histogram.png"
        logging.info(f"Will write: {output_file}")

        stacked_hist = ROOT.THStack("stacked_hist", "Stacked Backgrounds")
        histograms = {}
        for bkg_file, label in background_files:
            histograms[bkg_file] = ROOT.TH1F(f"hist_{bkg_file}", label, 16, 0, 16)

        log_section_start("Making Stack")
        h_Data = MakeDataHist(years, channels, data_files, input_dir, hist_base)
        histograms = process_histograms(years, channels, input_dir, background_files, hist_base)

        for bkg_file, label in background_files:
            histograms[bkg_file].SetFillColor(colors[label])
            stacked_hist.Add(histograms[bkg_file])

        max_value = 0.0
        total_integral = 0.0
        for i in range(stacked_hist.GetHists().GetSize()):
            hist = stacked_hist.GetHists().At(i)
            max_value = max(max_value, hist.GetMaximum())
            total_integral += hist.Integral()
        max_value = max(max_value, h_Data.GetMaximum())
        data_integral = h_Data.Integral()

        print(f"Maximum value in the THStack: {max_value}")
        print(f"Histogram Bkg   Integral: {total_integral:.2f}")
        print(f"Histogram Data  Integral: {data_integral:.2f}")

        ymax = max_value * 1.5
        h_dummy_up, h_dummy_down = create_dummy_pads(bin_labels, ymax)

        bkg_hist, scaled_up_hist, scaled_down_hist = get_hists_from_stack(stacked_hist, 0.15)
        gr_Bkgd_TotErr, gr_Data = get_graphs(scaled_up_hist, scaled_down_hist, h_Data)
        err_up_tmp, err_down_tmp = set_graph_data_bins(gr_Data)
        h_Data_Ratio, gr_Data_Ratio, gr_Bkgd_Ratio = get_ratio_plots(
            h_Data, bkg_hist, err_up_tmp, err_down_tmp, gr_Bkgd_TotErr
        )

        # Print per-era ratios (LLLL mode request)
        print_era_ratios(h_Data, bkg_hist, years)

        gr_Data_dummy = ROOT.TGraphAsymmErrors(gr_Data)
        gr_Data_dummy.SetMarkerStyle(20)
        gr_Data_dummy.SetMarkerSize(1.2)

        log_section_start("Making Canvas")
        canvas = ROOT.TCanvas("c_stacked_histogram", f"Stacked Histogram: {hist_base}", 900, 700)
        if canvas is None:
            print("Error: canvas could not be created.")
        c1_up = ROOT.TPad("c1_up", "", 0, 0.25, 1, 1)
        c1_down = ROOT.TPad("c1_down", "", 0, 0, 1, 0.25)
        canvas, c1_up, c1_down = canvas_margin.canvas_margin(canvas, c1_up, c1_down)
        canvas.Draw()
        c1_up.Draw()
        c1_down.Draw()

        canvas.cd()
        c1_up.cd()
        logging.info("Filling upper canvas")
        h_dummy_up.Draw("hist")
        line1, line2, line3 = fill_hist_labels(canvas, ymax)
        stacked_hist.Draw("histsame")
        gr_Bkgd_TotErr.Draw("sameE2")
        h_dummy_up.Draw("histsame")
        h_Data.Draw("phistsame")
        gr_Data.Draw("p0same")
        h_dummy_up.Draw("axissame")

        legend = ROOT.TLegend(0.2, 0.5, 0.35, 0.8)
        legend.SetBorderSize(0)
        legend.SetFillStyle(0)
        legend.SetTextSize(0.03)
        legend.AddEntry(gr_Data_dummy, "Data", "lpe")
        for bkg_file, label in background_files:
            legend.AddEntry(histograms[bkg_file], label, "f")
        legend.Draw()

        latex_CMSPriliminary = ROOT.TLatex()
        latex_Lumi = ROOT.TLatex()
        latex_CMSPriliminary.SetNDC()
        latex_Lumi.SetNDC()
        latex_CMSPriliminary.SetTextSize(0.035)
        latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}")
        latex_Lumi.SetTextSize(0.035)
        latex_Lumi.SetTextFont(42)
        latex_Lumi.DrawLatex(0.73, 0.96, "137.1 fb^{-1} (13 TeV)")

        # Was pad1 before; use the actual upper pad
        c1_up.Update()
        _era_lines = draw_fixed_boundaries(c1_up, [4.0, 8.0, 12.0])

        c1_down.cd()
        logging.info("Draw lower dummy pad")
        h_dummy_down.Draw("hist")
        line_down = ROOT.TLine(0, 1, 16, 1)
        line_down.SetLineColor(ROOT.kBlack)
        line_down.Draw("same")
        gr_Bkgd_Ratio.Draw("sameE2")
        h_Data_Ratio.Draw("p9histsame")
        gr_Data_Ratio.Draw("p0same")

        canvas.cd()
        canvas.SaveAs(output_file)
        logging.info(f"Saved: {output_file}")

except Exception as e:
    logging.error(f"An error occurred: {e}")
    sys.exit(1)

finally:
    try:
        for pad in [c1_up, c1_down]:
            pad.Close()
            del pad
        canvas.Close()
        del canvas
        ROOT.gROOT.GetListOfCanvases().Clear()
    except Exception:
        pass
