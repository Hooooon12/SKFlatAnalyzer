import ROOT
import os
from pathlib import Path
from array import array
import logging
import sys

# Assuming CMSStyle.py is in the same directory, otherwise update the path
#sys.path.append("CMSStyle.py")  # Update this with the correct path if needed
import CMS_lumi, tdrstyle



# A helper function to add clear breaks in logging
def log_section_start(section_name):
    logging.info("-" * 50)  # Print 50 '=' characters as a separator
    logging.info(f"STARTING SUBSECTION: {section_name}")
    logging.info("-" * 50)

logging.basicConfig(
    level=logging.INFO,
    format='Plotter:  %(asctime)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler()  # Log to console (screen)
    ]
)

ROOT.gROOT.SetBatch(True)
ROOT.gErrorIgnoreLevel = ROOT.kFatal


# === Configuration ===
years = ["2016preVFP", "2016postVFP", "2017", "2018"]
luminosities = {
    "2016preVFP": 19517,
    "2016postVFP": 16812,
    "2017": 41477,
    "2018": 59827
}

base_channels = ["MuMuMu", "EEE", "EMuL"]
background_files = [
    #["HNL_ControlRegion_Plotter_CF.root", "Chargeflip"],
    ["HNL_ControlRegion_Plotter_Conv.root", "X#gamma"],
    ["HNL_ControlRegion_Plotter_Fake.root", "Nonprompt"],
    ["HNL_ControlRegion_Plotter_TTV.root", "TTV"],
    #["HNL_ControlRegion_Plotter_VVV.root", "VVV"],
    ["HNL_ControlRegion_Plotter_WZ.root", "WZ"],
    ["HNL_ControlRegion_Plotter_ZZ.root", "ZZ"],
]

data_files = [
    "HNL_ControlRegion_Plotter_Data.root",
]


data_file = "HNL_ControlRegion_Plotter_Data.root"
all_files = [file[0] for file in background_files] + [data_file]

hist_bases = [
    ["HNL_WZ_SR3_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
]

# --- Path and File Output ---
Path("results").mkdir(exist_ok=True)

# Set TDR style for CMS plotting

tdrstyle.setTDRStyle()

# Prepare to handle and read histograms for different years and channels
for hist_base, flavour, channels in hist_bases:
    output_file = f"results/{hist_base}_{flavour}_stacked_histogram.png"
    logging.info(f"Running code to make {output_file}")
    
    # Create canvas for plotting
    canvas = ROOT.TCanvas("c_stacked_histogram", f"Stacked Histogram: {hist_base}", 900, 700)
    canvas.cd()

    # Initialize a histogram for stacking (one per era)
    stacked_hist = ROOT.THStack("stacked_hist", "Stacked Backgrounds")

    # Create dummy histogram for axis labels (we only need it to set the axis labels)
    dummy_hist = ROOT.TH1F("dummy_hist", "Dummy Histogram for Axis", 16, 0, 16)
    bin_labels = ["#mu#mu#mu", "eee", "E#mul", "lll"]

    # Set x-axis bin labels for the dummy histogram
    for i_bin, bin_channel in enumerate(bin_labels):  # Loop over the 4 channels
        dummy_hist.GetXaxis().SetBinLabel(i_bin + 1, bin_channel)  # Set labels for bins 1-4 for 2016preVFP
        dummy_hist.GetXaxis().SetBinLabel(i_bin + 5, bin_channel)  # Set labels for bins 5-8 for 2016postVFP
        dummy_hist.GetXaxis().SetBinLabel(i_bin + 9, bin_channel)  # Set labels for bins 9-12 for 2017
        dummy_hist.GetXaxis().SetBinLabel(i_bin + 13, bin_channel)  # Set labels for bins 13-16 for 2018
    dummy_hist.LabelsOption("v")

    # Draw the dummy histogram first to set the x-axis labels
    dummy_hist.SetTitle("Stacked Backgrounds: Channels")
    dummy_hist.GetXaxis().SetTitle("Channels")
    dummy_hist.GetYaxis().SetTitle("Events")
    
    histograms = {}
    for bkg_file, label in background_files:
        histograms[bkg_file] = ROOT.TH1F(f"hist_{bkg_file}", label, 16, 0, 16)  # 16 bins for 4 eras and 3 channels
        logging.info(f"Creating histogram for {bkg_file}")

    h_Data = ROOT.TH1F(f"hist_data", label, 16, 0, 16)  # 16 bins for 4 eras and 3 channels               
    colors = {
        "Chargeflip": ROOT.kYellow,
        "W#gamma": ROOT.kBlue,
        "Z#gamma": ROOT.kGreen + 2,
        "X#gamma": ROOT.kMagenta,
        "Nonprompt": 870,
        "Other": ROOT.kOrange,
        "tHq": ROOT.kViolet,
        "TTV": ROOT.kPink + 1,
        "VVV": ROOT.kYellow,
        "WW": ROOT.kGray,
        "WZ": ROOT.kGreen,
        "ggZZ": ROOT.kSpring + 1,
        "qqZZ": ROOT.kTeal,
        "ZZ": ROOT.kTeal,
    }
    
    # Loop over each year and channel, extract the corresponding histogram
    for year_idx, year in enumerate(years):
        for channel_idx, channel in enumesrate(channels):
            logging.info(f"Filling bins for {year}:{channel}")

            input_dir ="/data6/Users/jalmond/2020/HL_SKFlatAnalyzer_UL_LONG/SKFlatAnalyzer/data_validation/analysis_validation/merged_samples/ANv5/"
            hist_path = f"{hist_base}/HNL_ULIDv2/{channel}/NObj/N_BJet"
            
            if len(data_files)  > 0:
                data_file= data_files[0]
                data_file_path = os.path.join(input_dir,year, data_file)
                if not os.path.isfile(data_file_path):
                    logging.warning(f"Skipping bkg {data_file} since file {data_file_path} is missing")
                    continue  # Skip if the file does not exist                                                                                                                              
                else:
                    logging.info(f"Accessing file {data_file_path}")
                # Open the file and get the histogram                                                                                                                                        
                file = ROOT.TFile.Open(data_file_path, "READ")

                data_hist = file.Get(hist_path)
                if not data_hist:
                    logging.warning(f"Skipping bkg {bkg_file} since histogram {hist_path} is missing")
                    file.Close()
                    continue  # Skip if the histogram is not found                                                                                                                           
                # Calculate the integral and its error
                data_err = array('d', [0.0])  # Array to store the error
                data_norm = data_hist.IntegralAndError(1, data_hist.GetNbinsX(), data_err)

                bin_idx = year_idx * 4 + channel_idx  # Mapping year and channel to bins                                                                                                     
                h_Data.SetBinContent(bin_idx + 1, data_norm)
                h_Data.SetBinError(bin_idx + 1, data_err[0])  # Set the bin error                                                                                                   
                logging.info(f"Setting histograms {data_file} : bin_idx = {bin_idx} : norm = {data_norm} :  channel = {channel} year = {year}")
                file.Close()                

            for bkg_file, label in background_files:
                # Construct the file path
            
                file_path = os.path.join(input_dir,year, bkg_file)
                if not os.path.isfile(file_path):
                    logging.warning(f"Skipping bkg {bkg_file} since file {file_path} is missing")
                    continue  # Skip if the file does not exist
                else:
                    logging.info(f"Accessing file {file_path}")
                # Open the file and get the histogram
                file = ROOT.TFile.Open(file_path, "READ")
                hist_path = f"{hist_base}/HNL_ULIDv2/{channel}/NObj/N_BJet"
                hist = file.Get(hist_path)
                if not hist:
                    logging.warning(f"Skipping bkg {bkg_file} since histogram {hist_path} is missing")
                    file.Close()
                    continue  # Skip if the histogram is not found

                    
                # Calculate the integral and its error
                err = array('d', [0.0])  # Array to store the error
                norm = hist.IntegralAndError(1, hist.GetNbinsX(), err)

                bin_idx = year_idx * 4 + channel_idx  # Mapping year and channel to bins
                histograms[bkg_file].SetBinContent(bin_idx + 1, norm)
                histograms[bkg_file].SetBinError(bin_idx + 1, err[0])  # Set the bin error
                logging.info(f"Setting histograms {bkg_file} : bin_idx = {bin_idx} : norm = {norm} :  channel = {channel} year = {year}")
                file.Close()


    
    for bkg_file, label in background_files:
        # Stack histograms for all background files with unique colors
        histograms[bkg_file].SetFillColor(colors[label])
        stacked_hist.Add(histograms[bkg_file])


    # Initialize the maximum value to a very small number
    max_value = 0
    
    # Loop over each histogram in the stack and find the maximum value
    for i in range(stacked_hist.GetHists().GetSize()):
        hist = stacked_hist.GetHists().At(i)  # Get the histogram at index i
        max_value = max(max_value, hist.GetMaximum())  # Update max_value with the maximum of the current histogram
        
    # Now `max_value` contains the maximum value from all histograms in the stack
    print(f"Maximum value in the THStack: {max_value}")

    # Draw the stacked histogram
    ymax=max_value*1.5
    dummy_hist.GetYaxis().SetRangeUser(0,ymax)
    dummy_hist.Draw("hist")

    stacked_hist.Draw("histsame")
    dummy_hist.Draw("histsame")


    h_Data.SetMarkerStyle(20)
    h_Data.SetMarkerSize(1.2)
    h_Data.SetMarkerColor(ROOT.kBlack)
    h_Data.SetLineColor(ROOT.kBlack)

    h_Data.Draw("phistsame")
    gr_Data = ROOT.TGraphAsymmErrors(h_Data)

    
    # Add vertical lines and era labels                                                                                                                                                                             
    eras = [f"{years[0]}", f"{years[1]}", f"{years[2]}", f"{years[3]}"]
    bin_positions = [4.0, 8.0, 12.0, 16.0]

    line = ROOT.TLine(4.0, 0, 4.0, ymax)  # Vertical line at bin position                                                                                                                  
    line.SetLineColor(ROOT.kBlack)
    line.SetLineStyle(2)  # Dashed line                                                                                                                                                                         
    line.Draw("same")
    line2 = ROOT.TLine(8.0, 0, 8.0, ymax)  # Vertical line at bin position
    line2.SetLineColor(ROOT.kBlack)
    line2.SetLineStyle(2)  # Dashed line
    line2.Draw("same")
    line3 = ROOT.TLine(12.0, 0, 12.0, ymax)  # Vertical line at bin position
    line3.SetLineColor(ROOT.kBlack)
    line3.SetLineStyle(2)  # Dashed line
    line3.Draw("same")    


    canvas.Update()
    for i, pos in enumerate(bin_positions):        
        latex = ROOT.TLatex()
        latex.SetTextSize(0.03)
        latex.SetTextAlign(22)
        latex.DrawLatex(pos - 2, ymax*0.9, eras[i])  # Label near the line                                                                                                                  
    gr_Data_dummy = ROOT.TGraphAsymmErrors(gr_Data)
    gr_Data_dummy.SetMarkerStyle(20)
    gr_Data_dummy.SetMarkerSize(1.2)
    canvas.Update()

        
    # Add a legend
    legend = ROOT.TLegend(0.2, 0.5, 0.35, 0.8)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.03)
    legend.AddEntry(gr_Data_dummy, "Data", "ep")

    for bkg_file, label in background_files:
        legend.AddEntry(histograms[bkg_file], label, "f")  # Using label for the legend
    legend.Draw()
    canvas.Update()

    # Add CMS luminosity and logo
    

    latex_CMSPriliminary = ROOT.TLatex()
    latex_Lumi = ROOT.TLatex()
    
    latex_CMSPriliminary.SetNDC()
    latex_Lumi.SetNDC()
    latex_CMSPriliminary.SetTextSize(0.035)
    latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}")
    
    latex_Lumi.SetTextSize(0.035)
    latex_Lumi.SetTextFont(42)
    latex_Lumi.DrawLatex(0.73, 0.96, "137.1 fb^{-1} (13 TeV)")
    

    
    # Save the canvas as a PNG
    canvas.SaveAs(output_file)
    logging.info(f"Stacked histogram saved to: {output_file}")
