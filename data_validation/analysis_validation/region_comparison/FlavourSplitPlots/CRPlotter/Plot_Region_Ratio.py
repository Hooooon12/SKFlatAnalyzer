import ROOT
import os
from pathlib import Path
from array import array
import logging
import sys
import mylib
import math

import canvas_margin

# Assuming CMSStyle.py is in the same directory, otherwise update the path

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


def make_legend(gr_Data_dummy,background_files):

    # Add a legend                                                                                                                                  
    legend = ROOT.TLegend(0.2, 0.5, 0.35, 0.8)
    legend.SetBorderSize(0)
    legend.SetFillStyle(0)
    legend.SetTextSize(0.03)
    legend.AddEntry(gr_Data_dummy, "Data", "ep")

    for bkg_file, label in background_files:
        legend.AddEntry(histograms[bkg_file], label, "f")  # Using label for the legend                                                             
        
    return legend


def fill_hist_labels(canvas, ymax):

    eras = [f"{years[0]}", f"{years[1]}", f"{years[2]}", f"{years[3]}"]
    bin_positions = [4.0, 8.0, 12.0, 16.0]

    line = ROOT.TLine(4.0, 0, 4.0, ymax)  # Vertical line at bin position                                                                          \
                                                                                                                                                    
    line.SetLineColor(ROOT.kBlack)
    line.SetLineStyle(2)  # Dashed line                                                                                                            \
                                                                                                                                                    
    line.Draw("same")

    line2 = ROOT.TLine(8.0, 0, 8.0, ymax)  # Vertical line at bin position                                                                          
    line2.SetLineColor(ROOT.kBlack)
    line2.SetLineStyle(2)  # Dashed line                                                                                                            
    line2.Draw("same")
    line3 = ROOT.TLine(12.0, 0, 12.0, ymax)  # Vertical line at bin position                                                                        
    line3.SetLineColor(ROOT.kBlack)
    line3.SetLineStyle(2)  # Dashed line                                                                                                            
    line3.Draw("same")

    for i, pos in enumerate(bin_positions):
        latex = ROOT.TLatex()
        latex.SetTextSize(0.03)
        latex.SetTextAlign(22)
        latex.DrawLatex(pos - 2, ymax*0.9, eras[i])  # Label near the line                                                                         \

    canvas.Update()
    return line,line2,line3

def create_dummy_pads(bin_labels, y_max):

    dummy_up= ROOT.TH1F("h_dummy_up", "Dummy Histogram for Axis", 16, 0, 16)

    # Set x-axis bin labels for the dummy histogram                                                                                                                                                                                                                             
    for i_bin, bin_channel in enumerate(bin_labels):  # Loop over the 4 channels                                                                                                                                                                                                
        dummy_up.GetXaxis().SetBinLabel(i_bin + 1, bin_channel)  # Set labels for bins 1-4 for 2016preVFP                                                                                                                                                                     
        dummy_up.GetXaxis().SetBinLabel(i_bin + 5, bin_channel)  # Set labels for bins 5-8 for 2016postVFP                                                                                                                                                                    
        dummy_up.GetXaxis().SetBinLabel(i_bin + 9, bin_channel)  # Set labels for bins 9-12 for 2017                                                                                                                                                                          
        dummy_up.GetXaxis().SetBinLabel(i_bin + 13, bin_channel)  # Set labels for bins 13-16 for 2018                                                                                                                                                                        
    dummy_up.LabelsOption("v")

    # Draw the dummy histogram first to set the x-axis labels                                                                                                                                                                                                                   
    dummy_up.SetTitle("Stacked Backgrounds: Channels")
    dummy_up.GetXaxis().SetTitle("Channels")
    dummy_up.GetYaxis().SetTitle("Events")

    ### Draw Dummy ratio plots                                                                                                                                                                                                                                                  
    dummy_down = ROOT.TH1D('h_dumy_down', '',16,0,16)
    dummy_down.GetYaxis().SetRangeUser(0.5,1.5)
    dummy_down.SetNdivisions(504,"Y")

    for i_bin, bin_channel in enumerate(bin_labels):  # Loop over the 4 channels                                                                                                                                                                                                
        dummy_down.GetXaxis().SetBinLabel(i_bin + 1, bin_channel)  # Set labels for bins 1-4 for 2016preVFP                                                                                                                                                                   
        dummy_down.GetXaxis().SetBinLabel(i_bin + 5, bin_channel)  # Set labels for bins 5-8 for 2016postVFP                                                                                                                                                                  
        dummy_down.GetXaxis().SetBinLabel(i_bin + 9, bin_channel)  # Set labels for bins 9-12 for 2017                                                                                                                                                                        
        dummy_down.GetXaxis().SetBinLabel(i_bin + 13, bin_channel)  # Set labels for bins 13-16 for 2018                                                                                                                                                                      

    dummy_down.LabelsOption("v")
    dummy_down.GetXaxis().SetTitle("")
    dummy_down.GetYaxis().SetTitle("#frac{Data}{Sim.}")
    dummy_down.SetFillColor(0)
    dummy_down.SetMarkerSize(0)
    dummy_down.SetMarkerStyle(0)
    dummy_down.SetLineColor(ROOT.kWhite)
    dummy_up, dummy_down = canvas_margin.hist_axis(dummy_up, dummy_down)

    dummy_up.GetYaxis().SetRangeUser(0,y_max)



    for i_bin, bin_channel in enumerate(bin_labels):  
        dummy_up.GetXaxis().SetBinLabel(i_bin + 1, bin_channel)
        dummy_up.GetXaxis().SetBinLabel(i_bin + 5, bin_channel)
        dummy_up.GetXaxis().SetBinLabel(i_bin + 9, bin_channel)
        dummy_up.GetXaxis().SetBinLabel(i_bin + 13, bin_channel)                                                                                                                                                                                                                                             
    dummy_up.LabelsOption("v")

    
    return dummy_up, dummy_down


def MakeDataHist(years, channels,data_files, input_dir, hist_base):
    """
    Creates a data histogram for the specified file and returns it.
    If the histogram is invalid or the file is not found, the program exits with a warning.

    Arguments:
    - data_files: List of data files to process.
    - input_dir: Directory containing the input files.
    - year: Year of the data (used for file path).
    - hist_path: Path to the histogram inside the ROOT file.

    Returns:
    - The data histogram if valid, or exits the program if invalid.
    """

    # Loop over each year and channel, extract the corresponding histogram

    h_Data = ROOT.TH1F(f"hist_data", label, 16, 0, 16)  # 16 bins for 4 eras and 3 channels                      
    h_Data.SetMarkerStyle(20)
    h_Data.SetMarkerSize(1.2)
    h_Data.SetMarkerColor(ROOT.kBlack)
    h_Data.SetLineColor(ROOT.kBlack)

    for year_idx, year in enumerate(years):
        for channel_idx, channel in enumerate(channels):

            hist_path = f"{hist_base}/HNL_ULIDv2/{channel}/NObj/N_BJet"
            
            logging.info(f"Filling bins for {year}:{channel}")

            if len(data_files) > 0:
                data_file = data_files[0]
                data_file_path = os.path.join(input_dir, year, data_file)

                # Check if the file exists
                if not os.path.isfile(data_file_path):
                    logging.warning(f"Skipping {data_file} since file {data_file_path} is missing")
                    sys.exit(1)  # Exit the program with error code 1
                else:
                    logging.info(f"Accessing file {data_file_path}")
                    
                # Open the file and retrieve the histogram
                file = ROOT.TFile.Open(data_file_path, "READ")
                if not file or file.IsZombie():
                    logging.warning(f"Failed to open file {data_file_path}")
                    sys.exit(1)  # Exit if the file cannot be opened

                # Retrieve the histogram
                data_hist = file.Get(hist_path)
                if not data_hist:
                    logging.warning(f"Histogram {hist_path} is missing in file {data_file_path}")
                    file.Close()  # Close the file

                    d_data_err = 1.8
                    data_norm = 0
                    bin_idx = year_idx * 4 + channel_idx  # Mapping year and channel to bins                              \                                                                                           
                    h_Data.SetBinContent(bin_idx + 1, data_norm)
                    h_Data.SetBinError(bin_idx + 1, d_data_err)  # Set the bin error                                                                                                                                
                    
                    
                else:    
                    # Calculate the integral and its error
                    data_err = array('d', [0.0])  # Array to store the error
                    data_norm = data_hist.IntegralAndError(1, data_hist.GetNbinsX(), data_err)
                    
                    bin_idx = year_idx * 4 + channel_idx
                    h_Data.SetBinContent(bin_idx + 1, data_norm)
                    h_Data.SetBinError(bin_idx + 1, data_err[0])  # Set the bin error

                logging.info(f"Setting histogram {data_file} : bin_idx = {bin_idx} : norm = {data_norm}")

                # Close the file
                file.Close()

            else:
                logging.error("No data files provided.")
                sys.exit(1)  # Exit the program with error code 1

    # Return the valid histogram                                                                                                                                                                                                                                    
    return h_Data

                
def process_histograms(years, channels,input_dir, background_files, hist_base):
    """
    Initializes histograms and processes them from ROOT files.
    
    Arguments:
    - input_dir: Directory containing the input files.
    - year: The year to use for the file paths.
    - background_files: List of tuples (file_name, label) for background files.
    - hist_base: Base path in the ROOT file to find the histogram.
    - histograms: Dictionary to store histograms.
    - year_idx: The year index for mapping to bins.
    - channel_idx: The channel index for mapping to bins.
    
    Returns:
    - Updated `histograms` dictionary with the bin contents and errors set.
    """

    # Initialize the histograms in the dictionary if not already done
    for bkg_file, label in background_files:
        # Initialize histogram if it doesn't exist
        if bkg_file not in histograms:
            histograms[bkg_file] = ROOT.TH1F(f"hist_{bkg_file}", label, 16, 0, 16)
            logging.info(f"Creating histogram for {bkg_file}")


    for year_idx, year in enumerate(years):
        for channel_idx, channel in enumerate(channels):
            logging.info(f"Filling bins for {year}:{channel}")
            
            hist_path = f"{hist_base}/HNL_ULIDv2/{channel}/NObj/N_BJet"

            # Loop through background files to process each one
            for bkg_file, label in background_files:
                # Construct the file path
                file_path = os.path.join(input_dir, year, bkg_file)
        
                # Check if the file exists
                if not os.path.isfile(file_path):
                    logging.warning(f"Skipping {bkg_file} since file {file_path} is missing")
                    continue  # Skip if the file does not exist
                else:
                    logging.info(f"Accessing file {file_path}")
                    
                # Open the ROOT file and get the histogram
                file = ROOT.TFile.Open(file_path, "READ")
                hist_path = f"{hist_base}/HNL_ULIDv2/{channel}/NObj/N_BJet"
                hist = file.Get(hist_path)
        
                if not hist:
                    logging.warning(f"Skipping {bkg_file} since histogram {hist_path} is missing")
                    file.Close()  # Close the file after processing
                    continue  # Skip if the histogram is not found

                # Calculate the integral and its error
                err = array('d', [0.0])  # Array to store the error

                norm = hist.IntegralAndError(1, hist.GetNbinsX(), err)
                uncer = 0.1
                if "WW" in file_path:
                    uncer = 0.2
                if "VVV" in file_path:
                    uncer = 0.3
                if "ZZ" in file_path:
                    uncer = 0.15
                
                if "Fake" in file_path:
                    uncer = 0.3
                if "CF" in file_path:
                    uncer = 0.25
                if "Conv" in file_path:
                    uncer = 0.2


                d_err = err[0]  # extract the float value
                d_err = math.sqrt(d_err**2 + (uncer * norm)**2)
                
                # Mapping the year and channel to bins
                bin_idx = year_idx * 4 + channel_idx  # Mapping year and channel to bins
                                
                # Set the bin content and error in the histogram
                histograms[bkg_file].SetBinContent(bin_idx + 1, norm)
                histograms[bkg_file].SetBinError(bin_idx + 1, d_err)  # Set the bin error
                
                logging.info(f"Setting histograms {bkg_file} : bin_idx = {bin_idx} : norm = {norm} : channel = {channel_idx} year = {year}")
        
                # Close the file after processing
                file.Close()

    return histograms  # Return the updated histograms dictionary


def get_ratio_plots(h_Data,h_Bkgd,err_up_tmp,err_down_tmp):

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

        #print '%d - %f + %f'%(N, N-L, U-N)                                                                                                                                                                                                                                     
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

def cleanup_basic(objects):

    """
    Perform cleanup for a list of ROOT objects.
    Arguments:
    - objects: List of ROOT objects (like TCanvas, TPad, TH1, etc.) to be cleaned up.
    """
    print("Performing cleanup before exit...")

    for obj in objects:
        if obj:
            print(f"Deleting {obj.GetName() if hasattr(obj, 'GetName') else 'object'}")
            del obj  # Delete the object
        else:
            print("Object is None, not deleting.")

    
    # Optional: Check for any remaining objects in the cleanup list                                                                                                                                                                                                              
    print("Checking for remaining objects in ROOT's cleanup list...")
    remaining_objects = ROOT.gROOT.GetListOfCleanups()
    remaining_size = remaining_objects.GetSize()

    return
    if remaining_size > 0:
        print(f"Remaining objects to clean up: {remaining_size}")
        # Loop through the remaining objects and print their names                                                                                                                                                                                                              
        for i in range(remaining_size):
            obj = remaining_objects.At(i)
            print(f"Object {i + 1}: {obj.GetName()}")
    else:
        print("No remaining objects in ROOT's cleanup list.")

    # Optional: Reset ROOT to ensure everything is cleared up                                                                                                                                                                                                                   
    print("Resetting ROOT...")
    ROOT.gROOT.Reset()
    print("Cleanup completed.")


###   MAIN SETUP

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

base_channels = ["MuMuMuMu", "EEEE", "EMuLL"]
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


data_files = [
    "HNL_ControlRegion_Plotter_Data.root",
]


data_file = "HNL_ControlRegion_Plotter_Data.root"
all_files = [file[0] for file in background_files] + [data_file]


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


# Add verbosity flags
parser = argparse.ArgumentParser(description="Script with verbosity flags.")

# Add verbosity flags                                                                                                                                                                                             
parser.add_argument('-ll', action='store_true', help='Enable level 1 logging')
parser.add_argument('-lll', action='store_true', help='Enable level 2 logging')
parser.add_argument('-llll', action='store_true', help='Enable level 3 logging')

# Add a flag for selecting the directory
parser.add_argument('--plot-version', type=str, choices=directories, help='Select the directory for Plot_Version')

args = parser.parse_args()

hist_bases = [
    ["HNL_ZZ_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
    ["HNL_ZZ_AK8_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
    ["HNL_ZZVBF_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
    ["HNL_ZZ_OrigKF_FourLepton_CR", "LLLL", ["MuMuMuMu", "EEEE", "EMuLL", "LLLL"]],
]

bin_labels = ["#mu#mu#mu#mu", "eeee", "e#mull", "llll"]
if args.ll:
    bin_labels = ["#mu#mu", "ee", "e#mu", "ll"]
    hist_bases = [
        ["HNL_HighMass1Jet_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassNP_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSR3_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSSZPeak_AK4_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSSZPeak_AK8_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_SSPresel_TwoLepton", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["InclusiveCR3", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassBJet_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSR3_2J_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_WpWpNP2_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_WpWpNP3_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["InclusiveCR2", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR3_HighJet", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSSZPeak_VBF_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HighJet", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["InclusiveCR1", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR3_BJet_HighJet", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSR1_InvBJet_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSR1_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR1", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR1_BJet", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSR2_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR2", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR2_BJet", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR1_MET", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_HighMassSR1_InvMET_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["PassCR2_MET", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_WpWpNP_TwoLepton_CR", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["HNL_WpWp_TwoLepton_CR2", "LL", ["MuMu", "EE", "EMu", "LL"]],
        ["EMU_HighJetsCR1", "LL", ["MuMu", "EE", "EMu", "LL"]],
        

    ]

    
elif args.lll:
    bin_labels = ["#mu#mu#mu", "eee", "e#mul", "lll"]
    hist_bases = [
        ["HNL_WZ_SR3_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_ZG_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_WZVBF2_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_WZVBF_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_WZ_SR1_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_WZB_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_ZNPMu_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_ZNPMu_AK8_ThreeLepton_CR", "LLL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_ZNPEl_ThreeLepton_CR", "LL", ["MuMuMu", "EEE", "EMuL", "LLL"]],
        ["HNL_TopNP_ThreeLepton_CR", "LL", ["MuMuMu", "EEE", "EMuL", "LLL"]],

    ]
    


input_dir ="/data6/Users/jalmond/2020/HL_SKFlatAnalyzer_UL_LONG/SKFlatAnalyzer/data_validation/analysis_validation/merged_samples/"+Plot_Version+"/"


# --- Path and File Output ---
Path("results_ratio/").mkdir(exist_ok=True)
Path("results_ratio/"+Plot_Version).mkdir(exist_ok=True)

# Set TDR style for CMS plotting

tdrstyle.setTDRStyle()

log_section_start("Looping on Regions")


# Main execution with try-except-finally block for cleanup
try:
    
    # Prepare to handle and read histograms for different years and channels
    for hist_base, flavour, channels in hist_bases:
        
        log_section_start(f"Running {hist_base}")

        output_file = f"results_ratio/{Plot_Version}/{hist_base}_{flavour}_ratio_stacked_histogram.png"
        logging.info(f"Running code to make {output_file}")

        Run=True
        if Run:
            # Initialize a histogram for stacking (one per era)
            stacked_hist = ROOT.THStack("stacked_hist", "Stacked Backgrounds")
            
            # Initialize the histograms dictionary
            histograms = {}
        
            for bkg_file, label in background_files:
                histograms[bkg_file] = ROOT.TH1F(f"hist_{bkg_file}", label, 16, 0, 16)  # 16 bins for 4 eras and 3 channels
                logging.info(f"Creating histogram for {bkg_file}")
            
            log_section_start("Making Stack")

            # Loop over each year and channel, extract the corresponding histogram
            h_Data = MakeDataHist(years, channels, data_files,input_dir,hist_base)
            histograms = process_histograms(years, channels, input_dir, background_files, hist_base)
            
            for bkg_file, label in background_files:
                # Stack histograms for all background files with unique colors
                histograms[bkg_file].SetFillColor(colors[label])
                stacked_hist.Add(histograms[bkg_file])

            # Initialize the maximum value to a very small number
            max_value = 0
            # Assuming stacked_hist is your THStack
            total_integral = 0

            # Loop over each histogram in the stack and find the maximum value
            for i in range(stacked_hist.GetHists().GetSize()):
                hist = stacked_hist.GetHists().At(i)  # Get the histogram at index i
                max_value = max(max_value, hist.GetMaximum())  # Update max_value with the maximum of the current histogram
                hist_integral = hist.Integral()  # Get the integral of the histogram
                total_integral += hist_integral  # Add it to the total integral
       
        
            # compare with data 
            max_value = max(max_value, h_Data.GetMaximum())  
            data_integral = h_Data.Integral()
            # Now `max_value` contains the maximum value from all histograms in the stack
            print(f"Maximum value in the THStack: {max_value}")
            
            # Print the integral for each histogram
            print(f"Histogram Bkg   Integral: {total_integral:.2f}")
            print(f"Histogram Data  Integral: {data_integral:.2f}")

            
            # Setup Maximum Y value of hist to setup Range for plots
            ymax=max_value*1.5
            
            # Create dummy histogram for axis labels (we only need it to set the axis labels)                                                                                                                                                                                                             
            h_dummy_up,h_dummy_down = create_dummy_pads(bin_labels,ymax)
                # make hists 
            bkg_hist, scaled_up_hist,scaled_down_hist = get_hists_from_stack(stacked_hist, 0.15)
            print("Bin contents for scaled_up_hist and scaled_down_hist:")
            n_bins = scaled_up_hist.GetNbinsX()
            
            for i in range(1, n_bins + 1):  # Bins start from 1 in ROOT
                up_val = scaled_up_hist.GetBinContent(i)
                down_val = scaled_down_hist.GetBinContent(i)
                logging.debug(f"Bin {i}:  Up = {up_val:.3f},  Down = {down_val:.3f}")
          
            # make Graphs
            gr_Bkgd_TotErr,gr_Data = get_graphs(scaled_up_hist,scaled_down_hist,h_Data)

            # set error for data
            err_up_tmp, err_down_tmp=  set_graph_data_bins(gr_Data)  # Directly modifies gr_Data in place
            
            h_Data_Ratio,gr_Data_Ratio,gr_Bkgd_Ratio = get_ratio_plots(h_Data,bkg_hist,err_up_tmp,err_down_tmp)
            gr_Data_dummy = ROOT.TGraphAsymmErrors(gr_Data)                                                                                             
            gr_Data_dummy.SetMarkerStyle(20)                                                                                                            
            gr_Data_dummy.SetMarkerSize(1.2)                                                                                                            
            
            log_section_start("Making Canvas")
            # Create canvas for plotting                                                                                                                                            
            canvas = ROOT.TCanvas("c_stacked_histogram", f"Stacked Histogram: {hist_base}", 900, 700)
            # Check if the canvas was created successfully                                                                                                                          
            if canvas is None:
                print("Error: canvas could not be created.")
            else:
                print("Canvas created successfully.")

            # Create pads for the top and bottom -sections                                                                                                                           
            c1_up = ROOT.TPad("c1_up", "", 0, 0.25, 1, 1)
            c1_down = ROOT.TPad("c1_down", "", 0, 0, 1, 0.25)
            
            # Check if the pads are created successfully                                                                                                                            
            if c1_up is None or c1_down is None:
                print("Error: One or more pads could not be created.")
            else:
                print("Pads created successfully.")

            canvas, c1_up, c1_down = canvas_margin.canvas_margin(canvas, c1_up, c1_down)
            canvas.Draw()
            c1_up.Draw()
            c1_down.Draw()

            canvas.cd()
            c1_up.cd()
            logging.info("Filling upper canvas")

            # plot bkg hist/stack                                                                                                                                               
            logging.info("Draw upper dummy pad")
            h_dummy_up.Draw("hist")

            line1,line2,line3 = fill_hist_labels( canvas,ymax)
            
            stacked_hist.Draw("histsame")
            gr_Bkgd_TotErr.Draw("sameE2")
            h_dummy_up.Draw("histsame")

            # Drawing data
            h_Data.Draw("phistsame")
            gr_Data.Draw("p0same")
            h_dummy_up.Draw("axissame")

            #legend = make_legend(gr_Data_dummy,background_files)
            # Add a legend                                                                                                                                                                                                                          
            legend = ROOT.TLegend(0.2, 0.5, 0.35, 0.8)
            legend.SetBorderSize(0)
            legend.SetFillStyle(0)
            legend.SetTextSize(0.03)
            legend.AddEntry(gr_Data_dummy, "Data", "lpe")
            
            for bkg_file, label in background_files:
                legend.AddEntry(histograms[bkg_file], label, "f")  # Using label for the legend                                                                                                                                                     
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
            
            # bottom pad
            c1_down.cd()
            logging.info("Draw lower dummy pad")
            h_dummy_down.Draw("hist")

            line_down = ROOT.TLine(0, 1, 16, 1)  # Vertical line at bin position
            line_down.SetLineColor(ROOT.kBlack)
            line_down.Draw("same")

            gr_Bkgd_Ratio.Draw("sameE2")
                    
            h_Data_Ratio.Draw("p9histsame")
            gr_Data_Ratio.Draw("p0same")

            canvas.cd()

            canvas.SaveAs(output_file)

            logging.info(f"Stacked histogram saved to: {output_file}")

        else:
            print("**************")


except Exception as e:
    logging.error(f"An error occurred: {e}")
    sys.exit(1)  # Exit with error code                                                                                                                                                                          
                                              
            
finally:
    # cleanup
    for pad in [c1_up, c1_down]:
        pad.Close()
        del pad
    canvas.Close()
    del canvas
    ROOT.gROOT.GetListOfCanvases().Clear()

    #cleanup_basic([stacked_hist,gr_Bkgd_TotErr,h_dummy_up,h_dummy_down,histograms,gr_Data,h_Data])


