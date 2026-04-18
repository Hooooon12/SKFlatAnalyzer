import ROOT
import math
import sys
import argparse

parser = argparse.ArgumentParser(description='script for printing uncertainty breakdown.',formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-wp', dest='InputWPs', nargs='+', help='List of LimitInput working points')
parser.add_argument('-e', dest='eras', default=["Run2"], choices=["2016preVFP","2016postVFP","2017","2018","Run2"], nargs='+')
parser.add_argument('-c', dest='channels', default=["MuMu","EE","EMu"], choices=["MuMu","EE","EMu"], nargs='+') # store [] if nothing is fed
parser.add_argument('-m', dest='masses', default=["100","1000","10000","Weinberg"], nargs='+')
parser.add_argument('-s', dest='signals', default=["HNL"], choices=["","HNL","DY","VBF","DYVBF","SSWW","Weinberg"], nargs='+') # "" : to handle old file name convention
parser.add_argument('--InjectSignal', default='0', help='inject signals to asimov')
args = parser.parse_args()

AsimovName = "s"+args.InjectSignal

BasePath = "/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN"

# Set ROOT to batch mode (no graphical window)
ROOT.gROOT.SetBatch(True)

def get_precision_width(filename, tree_name="limit"):
    """
    Opens a ROOT file and calculates the precise width at 2*deltaNLL = 1.0 
    using TSpline3 interpolation.
    """
    f = ROOT.TFile.Open(filename)
    if not f or f.IsZombie():
        print(f"[Error] Cannot open file: {filename}")
        return None

    t = f.Get(tree_name)
    if not t:
        print(f"[Error] Tree '{tree_name}' not found in: {filename}")
        f.Close()
        return None

    # Load NLL scan data
    # Combine typically stores deltaNLL. We use 2*deltaNLL for the cut.
    n = t.Draw("2*deltaNLL:r", "2*deltaNLL < 10", "goff")
    if n < 3:
        print(f"[Warning] Not enough data points (n={n}) in: {filename}")
        f.Close()
        return 0.0
    
    # Create TGraph and sort it
    gr = ROOT.TGraph(n, t.GetV2(), t.GetV1())
    gr.Sort() 

    # Create TSpline3 for smooth interpolation
    spline = ROOT.TSpline3("spline", gr)

    # Find crossing points at 2*deltaNLL = 1.0
    r_min_val = t.GetMinimum("r")
    r_max_val = t.GetMaximum("r")
    
    # Fine-grained scan to find the crossing
    step = (r_max_val - r_min_val) / 20000.0
    r_vals = [r_min_val + i*step for i in range(20001)]
    
    crossings = []
    
    for i in range(len(r_vals)-1):
        r1 = r_vals[i]
        r2 = r_vals[i+1]
        y1 = spline.Eval(r1)
        y2 = spline.Eval(r2)
        
        # Check if the line crosses y = 1.0
        if (y1 - 1.0) * (y2 - 1.0) < 0:
            # Linear interpolation within the small step for better precision
            r_cross = r1 + (1.0 - y1) * (r2 - r1) / (y2 - y1)
            crossings.append(r_cross)
            
    f.Close()

    if len(crossings) >= 2:
        # Width = (Upper - Lower) / 2
        width = (crossings[-1] - crossings[0]) / 2.0
        return width
    elif len(crossings) == 1:
        # Handle cases where only one side crosses (e.g., limits)
        return crossings[0]
    else:
        return 0.0

# ------------------------------------------------------------------------------
# File Configuration
# Tuple: (Label used in logic, Filename)
# The order MUST follow the cumulative freezing sequence.
# ------------------------------------------------------------------------------
for InputWP, era, channel, mass, signal in [(InputWP, era, channel, mass, signal) for InputWP in args.InputWPs for era in args.eras for channel in args.channels for mass in args.masses for signal in args.signals]:

  if (signal!="Weinberg" and mass=="Weinberg") or (signal=="Weinberg" and mass!="Weinberg"): continue
  mass_signal = "Weinberg" if signal=="Weinberg" else f"M{mass}_{signal}"

  WorkPath = f"{BasePath}/{InputWP}/{era}_{channel}_{mass_signal}_syst/Breakdown/{AsimovName}"

  file_structure = [
      # 0. Total
      ("Total",            f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_total.MultiDimFit.mH120.root"),
      # 1. Freeze Jet
      ("Freeze Jet",       f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet.MultiDimFit.mH120.root"),
      # 2. Freeze +Theory
      ("Freeze PDF",       f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf.MultiDimFit.mH120.root"),
      ("Freeze Scale",     f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale.MultiDimFit.mH120.root"),
      # 3. Freeze +Fake
      ("Freeze Fake",      f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake.MultiDimFit.mH120.root"),
      # 4. Freeze +Lep
      ("Freeze Lep",       f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep.MultiDimFit.mH120.root"),
      # 5. Freeze +Pileup
      ("Freeze Pileup",    f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup.MultiDimFit.mH120.root"),
      # 6. Freeze +Lumi
      ("Freeze Lumi",      f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi.MultiDimFit.mH120.root"),
      # 7. Freeze +Btag
      ("Freeze Btag",      f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root"),
      # 8. Freeze +Prefire
      ("Freeze Prefire",   f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root"),
      # 9. Freeze +Met
      ("Freeze MET",       f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root"),
      # 10. Freeze +Xsec
      ("Freeze Xsec",      f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root"),
      # 11. Freeze +HEM
      ("Freeze HEM",       f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM.MultiDimFit.mH120.root"),
      # 12. Freeze +MCstat
      ("Freeze MCstat",    f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat.MultiDimFit.mH120.root"),
  ]
  if "E" in channel: file_structure.append(("Freeze CF",f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat_cf.MultiDimFit.mH120.root"))
  file_structure.append(("Stat Only",f"{WorkPath}/higgsCombine.{era}_{channel}_{mass_signal}_syst_{AsimovName}_freeze_all.MultiDimFit.mH120.root"))
  
  # Output Labels corresponding to the steps above
  group_labels = [
      "Jet uncert.",
      "PDF",
      "Scale",
      "Fake rate",
      "Lepton uncert.",
      "Pileup",
      "Luminosity",
      "b-tagging",
      "Prefire",
      "MET scale",
      "Cross section",
      "HEM",
      "Template statistical",
  ]
  if "E" in channel: group_labels.append("Charge Flip")
  
  print(f"<{InputWP} {era} {channel} {mass} {signal}>")
  #print("Processing Uncertainty Breakdown...")
  print("=" * 105)
  print(f"{'Source':<20} | {'Uncertainty':<12} | {'Relative %':<12} | {'% Variance':<12} | {'Raw Width':<10}")
  print(f"{'':<20} | {'(absolute)':<12} | {'(linear)':<12} | {'(quadrature)':<12} | {'(cumulative)':<10}")
  print("-" * 105)
  
  # 1. Calculate all widths first
  widths = []
  for label, fname in file_structure:
      w = get_precision_width(fname)
      if w is None: w = 0.0
      widths.append(w)
  
  total_uncert = widths[0]
  stat_uncert = widths[-1] # The last file is 'Freeze All'
  
  # Check for Total=0 to avoid division by zero
  if total_uncert == 0:
      print("Error: Total uncertainty is 0. Check the input files.")
      sys.exit()
  
  print(f"{'Total Uncertainty':<20} | {total_uncert:.5f}      | {'':<12} | {'100.0%':<12} | {total_uncert:.5f}")
  print("-" * 105)
  
  # 2. Iterate through defined systematic groups
  # Logic: Group_Uncert = sqrt(Width_Before^2 - Width_After^2)
  
  # Start from index 0 (Total)
  prev_width = widths[0]
  
  sum_sq_contributions = 0.0
  
  total_syst = 0.
  for i in range(len(group_labels)):
      # widths[0] is Total.
      # widths[1] is after freezing Group 1.
      # So Group 1 = sqrt(widths[0]^2 - widths[1]^2)
      
      width_after = widths[i+1]
      
      # Calculate difference in quadrature
      diff_sq = prev_width**2 - width_after**2
      
      if diff_sq < 0:
          comp_uncert = 0.0
      else:
          comp_uncert = math.sqrt(diff_sq)
      
      # Linear Percentage (sigma_i / sigma_tot) - These WON'T sum to 100%
      rel_percent = (comp_uncert / total_uncert) * 100.0
      
      # Contribution to Variance (sigma_i^2 / sigma_tot^2) - These SHOULD sum to 100%
      var_percent = (comp_uncert**2 / total_uncert**2) * 100.0
      sum_sq_contributions += var_percent

      print(f"{group_labels[i]:<20} | {comp_uncert:.5f}      | {rel_percent:5.1f}%       | {var_percent:5.1f}%       | {width_after:.5f}")
      
      prev_width = width_after
      total_syst += comp_uncert**2
  
  # 3. Calculate "Rest" (Unidentified Systematics)
  # Logic: After removing all defined groups (Jet...Xsec), we are at `widths[-2]` (Freeze Xsec).
  # The final state is `widths[-1]` (Stat Only).
  # If `widths[-2] > widths[-1]`, the difference implies systematics NOT in the list.
  
  width_after_last_syst = widths[-2] # The width before freezing 'all' but after freezing 'xsec'
  diff_sq_rest = width_after_last_syst**2 - stat_uncert**2
  
  if diff_sq_rest > 1e-6: # Use a small epsilon for float comparison
      rest_uncert = math.sqrt(diff_sq_rest)
      rel_percent = (rest_uncert / total_uncert) * 100.0
      var_percent = (rest_uncert**2 / total_uncert**2) * 100.0
      sum_sq_contributions += var_percent
      
      print(f"{'Rest / Unidentified':<20} | {rest_uncert:.5f}      | {rel_percent:5.1f}%       | {var_percent:5.1f}%       | {'(derived)':<10}")
  else:
      rest_uncert = 0.0
  
  total_syst += rest_uncert**2
  total_syst = math.sqrt(total_syst)
  rel_percent_syst = (total_syst / total_uncert) * 100.0
  var_percent_syst = (total_syst**2 / total_uncert**2) * 100.0
  print(f"{'Total systematic':<20} | {total_syst:.5f}      | {rel_percent_syst:5.1f}%       | {var_percent_syst:5.1f}%       | {'':<10}")

  # 4. Statistical Uncertainty
  rel_percent_stat = (stat_uncert / total_uncert) * 100.0
  var_percent_stat = (stat_uncert**2 / total_uncert**2) * 100.0
  sum_sq_contributions += var_percent_stat
  
  print(f"{'Data statistical':<20} | {stat_uncert:.5f}      | {rel_percent_stat:5.1f}%       | {var_percent_stat:5.1f}%       | {stat_uncert:.5f}")
  
  print("-" * 105)
  print(f"Sum of % Variance (should be ~100%): {sum_sq_contributions:.1f}%")
  #print("Note: 'Relative %' sums > 100% because uncertainties add in quadrature.")
  print("=" * 105)
  print()
