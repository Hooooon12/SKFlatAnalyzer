import ROOT
import os
from pathlib import Path
from array import array
import logging
import sys
import mylib
import math
import canvas_margin
import ctypes
import numpy as np

import CMS_lumi, tdrstyle


Analyzer = "HNL_ControlRegionTwo"
result_dir = "results_ratio_" +Analyzer

# A helper function to add clear breaks in logging
def log_section_start(section_name):
    logging.info("-" * 50)  # Print 50 '=' characters as a separator
    logging.info(f"STARTING SUBSECTION: {section_name}")
    logging.info("-" * 50)

logging.basicConfig(
    level=logging.INFO,
    format='Plotter:  %(asctime)s - %(levelname)s - %(message)s',
    handlers=[logging.StreamHandler()]  # Log to console (screen)
)

def create_dummy_pads(datahist, y_max,xmin,xmax):

    dummy_up= GetNullHist(datahist)
    dummy_up.SetTitle("Stacked Backgrounds: Channels")
    dummy_up.GetYaxis().SetTitle("Events")

    dummy_up.GetXaxis().SetRangeUser(xmin,xmax)

    dummy_down = GetNullHist(datahist)

    dummy_down.GetXaxis().SetRangeUser(xmin,xmax)
    dummy_down.GetYaxis().SetRangeUser(0.5,1.5)
    dummy_down.SetNdivisions(504,"Y")

    dummy_down.LabelsOption("v")
    dummy_down.GetXaxis().SetTitle("")
    dummy_down.GetYaxis().SetTitle("#frac{Data}{Sim.}")
    dummy_down.SetFillColor(0)
    dummy_down.SetMarkerSize(0)
    dummy_down.SetMarkerStyle(0)
    dummy_down.SetLineColor(ROOT.kWhite)
    dummy_up, dummy_down = canvas_margin.hist_axis(dummy_up, dummy_down)

    dummy_up.GetYaxis().SetRangeUser(0,y_max)

    return dummy_up, dummy_down


def CustomRebin(hist, new_bin_edges):

    #new_bin_edges = [0, 100, 400, 1000, float('inf')]

    new_bin_edges_array = np.array(new_bin_edges, dtype='d')

    # Create a new histogram with the custom binning
    # The number of bins is len(new_bin_edges) - 1, and the range is from min to max of your custom bins
    new_hist = ROOT.TH1F("new_hist", "Rebinned Histogram", len(new_bin_edges) - 1, new_bin_edges_array)

    
    # Loop through the old histogram and rebin the data
    for bin_idx in range(1, hist.GetNbinsX() + 1):
        # Get the content and error of the original bin
        content = hist.GetBinContent(bin_idx)
        error = hist.GetBinError(bin_idx)
        
        # Get the x-axis value for the current bin
        x_value = hist.GetBinCenter(bin_idx)

        # Find the correct bin in the new histogram based on the custom bin edges
        for i in range(len(new_bin_edges) - 1):
            if new_bin_edges[i] <= x_value < new_bin_edges[i + 1]:
                new_hist.Fill(x_value, content)  # Add the content to the new histogram
                break  # Exit the loop once the correct bin is found


    # Now you have the new histogram with the custom binning
    return new_hist



def GetNullHist(data_hist):

    # Clone the histogram
    h_clone = data_hist.Clone()

    # Set all bins to 0
    for bin_idx in range(1, h_clone.GetNbinsX() + 1):
        h_clone.SetBinContent(bin_idx, 0)
        h_clone.SetBinError(bin_idx, 0)        
        
    return h_clone

def GetDataHist(year, channel, data_files, input_dir, hist_base, hist_config, histname, hist_axis, hist_rebin):
    h_data = None
    hist_path = f"{hist_base}/{hist_config}/{channel}/{histname}"

    if len(data_files) > 0:
        data_file = data_files[0]
        data_file_path = os.path.join(input_dir, year, data_file)
        logging.info(f"Getting data histogram {hist_path} from {data_file_path}")

        # Check if the file exists
        if not os.path.isfile(data_file_path):
            raise FileNotFoundError(f"Skipping {data_file} since file {data_file_path} is missing")

        # Open the file and retrieve the histogram
        file = ROOT.TFile.Open(data_file_path, "READ")
        if not file or file.IsZombie():
            raise FileNotFoundError(f"Failed to open file {data_file_path}")

        # Retrieve the histogram from the file
        h_DATA = file.Get(hist_path)
        if not h_DATA:
            raise ValueError(f"Skipping {data_file} since histogram {hist_path} is missing")
        h_DATA = mylib.MakeOverflowBin(h_DATA)
        
        # Clone the histogram immediately (while the file is still open)
        h_data = h_DATA.Clone()
        
        # Now detach the histogram from the file explicitly to prevent dependency on the file
        h_data.SetDirectory(0)  # This ensures the histogram is no longer tied to the file

        # Logging and setting styles
        logging.info(f"Successfully accessed histogram, integral={h_data.Integral()}")
        h_data.GetXaxis().SetTitle(hist_axis)
        h_data.SetMarkerStyle(20)
        h_data.SetMarkerSize(1.2)
        h_data.SetMarkerColor(ROOT.kBlack)
        h_data.SetLineColor(ROOT.kBlack)

        if len(hist_rebin) > 1:
            h_data=CustomRebin(h_data,hist_rebin)
        else:
            h_data.Rebin(hist_rebin[0])
            logging.info(f"Rebinning hist {hist_rebin[0]}")
        
        # Close the file after you are done with the histogram
        file.Close()
    else:
        raise ValueError("No data files provided.")

    # Final logging
    logging.info(f"Successfully accessed histogram, integral={h_data.Integral()}")

    return h_data


def process_histograms(NULLHist,year, channel, input_dir, background_files, hist_base, hist_config, histname, hist_axis, hist_rebin,colors):
    histograms = {}  # Initialize histograms dictionary
    hist_path = f"{hist_base}/{hist_config}/{channel}/{histname}"

    for bkg_file, label in background_files:
        file_path = os.path.join(input_dir, year, bkg_file)

        # Check if the file exists
        if not os.path.isfile(file_path):
            logging.warning(f"Skipping {bkg_file} since file {file_path} is missing")
            continue  # Skip if the file doesn't exist

        logging.info(f"Accessing file {file_path}")
        file = ROOT.TFile.Open(file_path, "READ")

        if not file or file.IsZombie():
            logging.warning(f"Failed to open file {file_path}")
            continue  # Skip if file cannot be opened

        hist = file.Get(hist_path)
        if not hist:
            logging.warning(f"Skipping {bkg_file} since histogram {hist_path} is missing")
            file.Close()
            histograms[bkg_file] = NULLHist
            continue  # Skip if histogram not found

        hist = mylib.MakeOverflowBin(hist)
        hist_clone = hist.Clone()
        hist_clone.SetDirectory(0) 
        
        if len(hist_rebin) > 1:
            hist_clone=CustomRebin(hist_clone,hist_rebin)
        else:
            hist_clone.Rebin(hist_rebin[0])
            
        logging.info(f"Setting hist color : {bkg_file} --> {colors[label]} ")
        hist_clone.SetFillColor(colors[label])        
        histograms[bkg_file] = hist_clone  # Add the histogram to the dictionary

        file.Close()

    return histograms  # Return the populated dictionary






def get_ratio_plots(h_Data,h_Bkgd,gr_Bkgd_TotErr, gr_Data,err_up_tmp,err_down_tmp):

    # set ratio hists and graphs
    # clone data

    h_Data_Ratio = h_Data.Clone('h_Data_Ratio')
    tmp_h_Data_Ratio = h_Data.Clone()

    tmp_h_Data_Ratio.Divide(h_Bkgd)

    gr_Data_Ratio = ROOT.TGraphAsymmErrors(tmp_h_Data_Ratio)
    gr_Data_Ratio.SetName('gr_Data_Ratio')
    gr_Data_Ratio.SetLineWidth(2)
    gr_Data_Ratio.SetMarkerSize(0.)
    gr_Data_Ratio.SetLineColor(ROOT.kBlack)

    gr_Bkgd_Ratio = gr_Bkgd_TotErr.Clone('gr_Bkgd_Ratio')

    for i in range(1,h_Data_Ratio.GetXaxis().GetNbins()+1):

        if h_Bkgd.GetBinContent(i)!=0:

            h_Data_Ratio.SetBinContent( i, h_Data_Ratio.GetBinContent(i) / h_Bkgd.GetBinContent(i) )
            h_Data_Ratio.SetBinError ( i, h_Data_Ratio.GetBinError(i) / h_Bkgd.GetBinContent(i) )

            if err_down_tmp[i-1]!=0.:
                gr_Data_Ratio.SetPointEYlow(i-1, err_down_tmp[i-1] / h_Bkgd.GetBinContent(i) )
                gr_Data_Ratio.SetPointEYhigh(i-1, err_up_tmp[i-1] / h_Bkgd.GetBinContent(i))
            else:
                gr_Data_Ratio.SetPointEYlow(i-1, 0)
                gr_Data_Ratio.SetPointEYhigh(i-1, 1.8 / h_Bkgd.GetBinContent(i))

            gr_Bkgd_Ratio.SetPoint(i-1,h_Bkgd.GetXaxis().GetBinCenter(i), 1.)
            gr_Bkgd_Ratio.SetPointEYhigh( i-1, gr_Bkgd_Ratio.GetErrorYhigh(i-1) / h_Bkgd.GetBinContent(i) )
            gr_Bkgd_Ratio.SetPointEYlow( i-1,  gr_Bkgd_Ratio.GetErrorYlow(i-1) / h_Bkgd.GetBinContent(i) )

        elif h_Bkgd.GetBinContent(i)==0. and h_Data_Ratio.GetBinContent(i)==0.:

            h_Data_Ratio.SetBinContent( i, 0 )
            h_Data_Ratio.SetBinError ( i, 0 )
            gr_Data_Ratio.SetPoint(i-1, 0, 0)
            gr_Data_Ratio.SetPointEYlow(i-1, 0)
            gr_Data_Ratio.SetPointEYhigh(i-1, 0)

            gr_Bkgd_Ratio.SetPoint(i-1,h_Bkgd.GetXaxis().GetBinCenter(i), 1.)
            gr_Bkgd_Ratio.SetPointEYhigh( i-1, 0. )
            gr_Bkgd_Ratio.SetPointEYlow( i-1, 0. )

        else:
            this_max_ratio = 20.0
            this_data = h_Data_Ratio.GetBinContent(i)
            this_data_err = h_Data_Ratio.GetBinError(i)

            h_Data_Ratio.SetBinContent( i, this_max_ratio )
            h_Data_Ratio.SetBinError ( i, this_data_err*this_max_ratio/this_data )

            tmp_x = ctypes.c_double(0.)
            tmp_y = ctypes.c_double(0.)

            gr_Data_Ratio.GetPoint(i-1, tmp_x, tmp_y)
            gr_Data_Ratio.SetPoint(i-1, tmp_x, this_max_ratio)
            gr_Data_Ratio.SetPointEYlow(i-1, err_down_tmp[i-1]*this_max_ratio/this_data)
            gr_Data_Ratio.SetPointEYhigh(i-1, err_up_tmp[i-1]*this_max_ratio/this_data)

            gr_Bkgd_Ratio.SetPoint(i-1,h_Bkgd.GetXaxis().GetBinCenter(i), 1.)
            gr_Bkgd_Ratio.SetPointEYhigh( i-1, 0. )
            gr_Bkgd_Ratio.SetPointEYlow( i-1, 0. )

        gr_Bkgd_Ratio.SetMarkerColor(0)
        gr_Bkgd_Ratio.SetMarkerSize(0)
        gr_Bkgd_Ratio.SetFillStyle(3013)
        gr_Bkgd_Ratio.SetFillColor(ROOT.kBlack)
        gr_Bkgd_Ratio.SetLineColor(0)

    return h_Data_Ratio,gr_Data_Ratio,gr_Bkgd_Ratio

def get_graphs(scaled_up_hist,scaled_down_hist,h_Data):

    gr_Bkgd_TotErr = mylib.GetAsymmError(scaled_up_hist,scaled_down_hist)
    gr_Bkgd_TotErr.SetMarkerColor(0)
    gr_Bkgd_TotErr.SetMarkerSize(0)
    gr_Bkgd_TotErr.SetFillStyle(3013)
    gr_Bkgd_TotErr.SetFillColor(ROOT.kBlack)
    gr_Bkgd_TotErr.SetLineColor(0)

    gr_Data = ROOT.TGraphAsymmErrors(h_Data)
    gr_Data.SetLineWidth(2)
    gr_Data.SetMarkerSize(0.)
    gr_Data.SetMarkerColor(ROOT.kBlack)
    gr_Data.SetLineColor(ROOT.kBlack)


    return gr_Bkgd_TotErr,gr_Data

def get_hists_from_stack(stacked_hist, uncertainty):

    # Make bkg hist (sum of all backgrounds)                                                                                                                                               
    bkg_hist = stacked_hist.GetHists().At(0).Clone("sum_hist")

    for i in range(1, stacked_hist.GetHists().GetSize()):
        hist = stacked_hist.GetHists().At(i)
        bkg_hist.Add(hist)

    print(f"Sum histogram name: {bkg_hist.GetName()}")

    # Create up/down variations using bin errors                                                                                                                                           
    scaled_up_hist = bkg_hist.Clone("scaled_up_hist")
    scaled_down_hist = bkg_hist.Clone("scaled_down_hist")

    for i in range(1, bkg_hist.GetNbinsX() + 1):
        content = bkg_hist.GetBinContent(i)
        error = bkg_hist.GetBinError(i)

        scaled_up_hist.SetBinContent(i, content + error)
        scaled_down_hist.SetBinContent(i, max(0, content - error))  # Avoid negative values                                                                                                

    return bkg_hist, scaled_up_hist, scaled_down_hist
def set_graph_data_bins(gr_Data):

    alpha = 1. - 0.6827

    err_down_tmp = []
    err_up_tmp = []
    for i in range(0, gr_Data.GetN()):
        N = gr_Data.GetY()[i]

        L = 0.                                          if (N==0.) else (ROOT.Math.gamma_quantile(alpha/2.,N,1.))
        U = ( ROOT.Math.gamma_quantile_c(alpha,N+1,1) ) if (N==0.) else (ROOT.Math.gamma_quantile_c(alpha/2.,N+1.,1.))

        #print '%d - %f + %f'%(N, N-L, U-N)                                                                                                                                               \
                                                                                                                                                                                           
        if N!=0:
            gr_Data.SetPointEYlow(i, N-L )
            gr_Data.SetPointEYhigh(i, U-N )
            err_down_tmp.append(N-L)
            err_up_tmp.append(U-N)
        else:
            zerodata_err_low = 0.1
            zerodata_err_high = 1.8

            xlow = gr_Data.GetX()[i]-gr_Data.GetEXlow()[i]
            xhigh = gr_Data.GetX()[i]+gr_Data.GetEXhigh()[i]

            gr_Data.SetPointEYlow(i, zerodata_err_low)
            gr_Data.SetPointEYhigh(i, zerodata_err_high)
            err_down_tmp.append(zerodata_err_low)
            err_up_tmp.append(zerodata_err_high)

    return err_up_tmp,err_down_tmp



def main():

    ###   MAIN SETUP                                                                                                     

    ROOT.gROOT.SetBatch(True)
    ROOT.gErrorIgnoreLevel = ROOT.kFatal
    
    
    # === Configuration ===                                                                                              
    years = ["2016preVFP", "2016postVFP", "2017" ,"2018"]
    luminosities = {
        "2016preVFP": 19517,
        "2016postVFP": 16812,
        "2017": 41477,
        "2018": 59827
    }
    
    background_files = [
        [f"{Analyzer}_CF.root", "Chargeflip"],
        [f"{Analyzer}_Conv.root", "X#gamma"],
        [f"{Analyzer}_Fake.root", "Nonprompt"],
        [f"{Analyzer}_TTV.root", "TTV"],
        [f"{Analyzer}_VVV.root", "VVV"],
        [f"{Analyzer}_WZ.root", "WZ"],
        [f"{Analyzer}_WW.root", "WW"],
        [f"{Analyzer}_ZZ.root", "ZZ"],
        [f"{Analyzer}_Other.root", "Other"],
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


    data_files = [
        f"{Analyzer}_Data.root",
    ]


    data_file = f"{Analyzer}_Data.root"

    import argparse


    # Define the path containing the directories                                                                         
    directory_path = '/data6/Users/jalmond/2020/HL_SKFlatAnalyzer_UL_LONG/SKFlatAnalyzer/data_validation/analysis_validation/merged_samples/'
    
    # Get a list of directories in the specified path                                                                    
    directories = [d for d in os.listdir(directory_path) if os.path.isdir(os.path.join(directory_path, d))]
    
    # Check if there are directories available                                                                           
    if not directories:
        raise ValueError("No directories found in the specified path!")
    
    # Print the available directories and ask the user to pick one                                                       
    print("Available directories:")
    for idx, directory in enumerate(directories, 1):
        print(f"{idx}. {directory}")

    # Prompt the user to choose a directory                                                                              
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


    parser = argparse.ArgumentParser(description="Script with verbosity flags.")
    parser.add_argument('--plot-version', type=str, choices=directories, help='Select the directory for Plot_Version')
    
    args = parser.parse_args()

    #Plots = [["ExtraLep/All_El_NonMatched_CBVeto","All_El_NonMatched CB",[1]], ["ExtraLep/All_El_NonMatched_MVA","All_El_NonMatched MVA",[1]], ["ExtraLep/All_Muon_NonMatched","All_Muon_NonMatched",[1]], ["ExtraLep/All_Tau_NonMatched","All_Tau_NonMatched",[1]], ["Standard/Lepton_pt","Lepton_pt",[2]]]
    Plots = [
        
        #["ExtraLep/All_El_NonMatched","All_El_NonMatched CB",[1],0,5],
        #["ExtraLep/All_Muon_NonMatched","All_Muon_NonMatched",[1],0,5],
        #["ExtraLep/All_Tau_NonMatched","All_Tau_NonMatched",[1],0,5],
        ["Leptons/Lep_1_pt","Lepton_pt",[2],0,100],
        ["Leptons/Lep_2_pt","Lepton_pt",[2],0,100],
        ["Standard/M_ll","Lepton mass",[2],0,400],
        ["MainPlots/HT_PT1","Lepton mass",[1],0,10],
        ["VBF/Lead_MJJ","MJJ",[1],0,3000],
        ["VBF/MaxDEtaJets_MJJ","MJJ",[1],0,2000],
        ["VBF/Lead_zeppenfeld","Z",[5],0,2],
        ["VBF/MaxDEta_Jets_zeppenfeld","Z",[5],0,2]
    ]

    IDs = ["HNL_ULIDv2"]
    #","HNL_ULIDv2_Pt20","HNL_ULIDv2_Pt25","HNL_ULIDv2_Pt30","HNL_ULIDv2_pileup_medium","HNL_ULIDv2_presel","HNL_ULIDv2_presel_mod"]
    

    Flavours = ["MuMu","EE","EMu","LL"]

    
    hist_bases = [
    ]

    for j_histname, j_label,j_rebin,j_min,j_max in Plots:
        for j_id in IDs:
            for j_flavour in Flavours:
                hist_bases.append(["HNL_HighMassSR2_M750_CR", j_id,j_histname, j_label, j_rebin, j_flavour,j_min,j_max])
                hist_bases.append(["HNL_HighMassSR2_M500_CR", j_id,j_histname, j_label, j_rebin, j_flavour,j_min,j_max])
                hist_bases.append(["HNL_HighMassSR2_Inclusive_CR", j_id,j_histname, j_label, j_rebin, j_flavour,j_min,j_max])
                hist_bases.append(["HNL_HighMassSR2_InvBJet_CR", j_id,j_histname, j_label, j_rebin, j_flavour,j_min,j_max])
                hist_bases.append(["HNL_HighMassSR2_InvMET_CR",  j_id,j_histname, j_label, j_rebin, j_flavour,j_min,j_max])
    
    input_dir ="/data6/Users/jalmond/2020/HL_SKFlatAnalyzer_UL_LONG/SKFlatAnalyzer/data_validation/analysis_validation/merged_samples/"+Plot_Version+"/"


            
    # Path and File Output
    Path(result_dir).mkdir(exist_ok=True)

    # Set TDR style for CMS plotting
    tdrstyle.setTDRStyle()

    # Main execution loop
    try:
        for hist_base, hist_config, histname, hist_axis, hist_rebin, flavour,xmin,xmax in hist_bases:
            log_section_start(f"Running {hist_base}")
            histname_syntaxfix = histname.replace("/", "_")
            Path(f"{result_dir}/{histname_syntaxfix}").mkdir(exist_ok=True)

            histname_syntaxfix
            for year_idx, year in enumerate(years):
                output_file = f"{result_dir}/{histname_syntaxfix}/{year}_{hist_base}_{hist_config}_{histname_syntaxfix}_{flavour}_ratio_stacked_histogram.png"
                logging.info(f"Running code to make {output_file}")
                
                stacked_hist = ROOT.THStack(f"stacked_hist_{flavour}_{year}", "Stacked Backgrounds")

                log_section_start("Making Data")

                # Extract data histogram
                h_Data = GetDataHist(year, flavour, data_files, input_dir, hist_base, hist_config, histname, hist_axis, hist_rebin)
                if not h_Data:
                    logging.warning("h_Data is NULL")

                NULLHist = GetNullHist(h_Data)
               
                log_section_start("Making Stack")
                histograms = process_histograms(NULLHist,year, flavour, input_dir, background_files, hist_base, hist_config, histname, hist_axis, hist_rebin,colors)

                # Stack background histograms
                for bkg_file, label in background_files:
                    histograms[bkg_file].SetFillColor(colors[label])
                    stacked_hist.Add(histograms[bkg_file])

                # Find maximum values for the plot's y-axis range
                max_value = 0
                total_integral = 0
                for i in range(stacked_hist.GetHists().GetSize()):
                    hist = stacked_hist.GetHists().At(i)
                    max_value = max(max_value, hist.GetMaximum())
                    total_integral += hist.Integral()

                # compare with data
                max_value = max(max_value, h_Data.GetMaximum())
                data_integral = h_Data.Integral()

                # Setup Maximum Y value of hist to setup Range for plots
                ymax = max_value * 1.5

                # Create dummy histograms for axis labels
                h_dummy_up, h_dummy_down = create_dummy_pads(h_Data, ymax,xmin,xmax)

                # Create background and error histograms
                bkg_hist, scaled_up_hist, scaled_down_hist = get_hists_from_stack(stacked_hist, 0.15)

                # Generate graphs
                gr_Bkgd_TotErr, gr_Data = get_graphs(scaled_up_hist, scaled_down_hist, h_Data)

                # Set error for data
                err_up_tmp, err_down_tmp = set_graph_data_bins(gr_Data)

                # Generate ratio plots
                h_Data_Ratio, gr_Data_Ratio, gr_Bkgd_Ratio = get_ratio_plots(h_Data, bkg_hist,gr_Bkgd_TotErr, gr_Data, err_up_tmp, err_down_tmp)

                # Set up the canvas and pads for plotting
                log_section_start("Making Canvas")
                canvas = ROOT.TCanvas("c_stacked_histogram", f"Stacked Histogram: {hist_base}", 900, 700)

                # Check if the canvas is created
                if canvas is None:
                    raise RuntimeError("Error: canvas could not be created.")
                logging.info("Canvas created successfully.")

                # Create pads for the top and bottom
                c1_up = ROOT.TPad("c1_up", "", 0, 0.25, 1, 1)
                c1_down = ROOT.TPad("c1_down", "", 0, 0, 1, 0.25)

                # Check if the pads are created
                if c1_up is None or c1_down is None:
                    raise RuntimeError("Error: One or more pads could not be created.")

                canvas, c1_up, c1_down = canvas_margin.canvas_margin(canvas, c1_up, c1_down)
                canvas.Draw()
                c1_up.Draw()
                c1_down.Draw()

                # Upper canvas
                canvas.cd()
                c1_up.cd()
                logging.info("Filling upper canvas")

                # Plot background histograms
                logging.info("Draw upper dummy pad")
                h_dummy_up.Draw("hist")

                stacked_hist.Draw("histsame")
                gr_Bkgd_TotErr.Draw("sameE2")
                h_dummy_up.Draw("histsame")

                # Drawing data
                h_Data.Draw("p same")
                gr_Data.Draw("p0same")
                h_dummy_up.Draw("axissame")

                # Add a legend
                legend = ROOT.TLegend(0.2, 0.7, 0.35, 0.9)
                legend.SetBorderSize(0)
                legend.SetFillStyle(0)
                legend.SetTextSize(0.03)
                
                gr_Data_dummy = ROOT.TGraphAsymmErrors(gr_Data)
                gr_Data_dummy.SetMarkerStyle(20)
                gr_Data_dummy.SetMarkerSize(1.2)

                legend.AddEntry(gr_Data_dummy, "Data", "lpe")
                legend2 = ROOT.TLegend(0.4, 0.7, 0.55, 0.9)
                legend2.SetBorderSize(0)
                legend2.SetFillStyle(0)
                legend2.SetTextSize(0.03)

                ileg=1
                for bkg_file, label in background_files:
                    if ileg < len(histograms)/2:
                        legend.AddEntry(histograms[bkg_file], label, "f")  # Using label for the legend
                    else:
                        legend2.AddEntry(histograms[bkg_file], label, "f")  # Using label for the legend

                    ileg=ileg+1
                    
                legend.Draw()
                legend2.Draw()

                # Add CMS and luminosity labels
                latex_CMSPriliminary = ROOT.TLatex()
                latex_Lumi = ROOT.TLatex()

                latex_CMSPriliminary.SetNDC()
                latex_Lumi.SetNDC()
                latex_CMSPriliminary.SetTextSize(0.035)
                latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}")

                latex_Lumi.SetTextSize(0.035)
                latex_Lumi.SetTextFont(42)
                latex_Lumi.DrawLatex(0.73, 0.96, "137.1 fb^{-1} (13 TeV)")

                # Bottom canvas
                c1_down.cd()
                logging.info("Draw lower dummy pad")
                h_dummy_down.Draw("hist")

                line_down = ROOT.TLine(0, 1, h_Data.GetXaxis().GetXmax(), 1)
                line_down.SetLineColor(ROOT.kBlack)
                line_down.Draw("same")

                gr_Bkgd_Ratio.Draw("sameE2")
                h_Data_Ratio.Draw("p9histsame")
                gr_Data_Ratio.Draw("p0same")

                # Save canvas
                canvas.SaveAs(output_file)
                logging.info(f"Stacked histogram saved to: {output_file}")

    except Exception as e:
        logging.error(f"An error occurred: {e}")
        sys.exit(1)

    finally:
        # Cleanup
        canvas.Close()
        del canvas
        ROOT.gROOT.GetListOfCanvases().Clear()

if __name__ == "__main__":
    main()
