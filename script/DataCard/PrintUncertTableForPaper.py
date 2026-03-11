import ROOT
import math
import sys
import argparse

# ------------------------------------------------------------------------------
# Argument Parsing
# ------------------------------------------------------------------------------
parser = argparse.ArgumentParser(description='Script for generating LaTeX uncertainty table.', formatter_class=argparse.RawTextHelpFormatter)
parser.add_argument('-wp', dest='InputWP', required=True, help='LimitInput working point string')
parser.add_argument('-e', dest='era', default="Run2", help='Era (e.g., Run2)')
parser.add_argument('--InjectSignal', default='0', help='inject signals to asimov (default: 0 -> b-only)')
args = parser.parse_args()

# Configuration based on user request
AsimovName = "s" + args.InjectSignal
BasePath = "/data9/Users/HNL_public/SUS-24-014/Combine/CMSSW_14_1_0_pre4/src/DilepHN"
SignalType = "HNL" # Fixed as per context

# The exact columns requested for the paper
TargetMasses = ["100", "1000", "10000"] # GeV
TargetChannels = ["MuMu", "EE", "EMu"]

# Set ROOT to batch mode
ROOT.gROOT.SetBatch(True)

# ------------------------------------------------------------------------------
# Helper Functions
# ------------------------------------------------------------------------------
def get_precision_width(filename, tree_name="limit"):
    """
    Opens a ROOT file and calculates the precise width at 2*deltaNLL = 1.0 
    using TSpline3 interpolation. Returns 0.0 if file/tree is missing or invalid.
    """
    try:
        f = ROOT.TFile.Open(filename)
        if not f or f.IsZombie():
            # implicitly ignore missing files by returning 0, assuming job failed or not needed
            return 0.0

        t = f.Get(tree_name)
        if not t:
            f.Close()
            return 0.0

        # Load NLL scan data (2*deltaNLL vs r)
        n = t.Draw("2*deltaNLL:r", "2*deltaNLL < 10", "goff")
        if n < 3:
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
        
        step = (r_max_val - r_min_val) / 20000.0
        r_vals = [r_min_val + i*step for i in range(20001)]
        
        crossings = []
        for i in range(len(r_vals)-1):
            r1 = r_vals[i]
            r2 = r_vals[i+1]
            y1 = spline.Eval(r1)
            y2 = spline.Eval(r2)
            
            if (y1 - 1.0) * (y2 - 1.0) < 0:
                r_cross = r1 + (1.0 - y1) * (r2 - r1) / (y2 - y1)
                crossings.append(r_cross)
                
        f.Close()

        if len(crossings) >= 2:
            return (crossings[-1] - crossings[0]) / 2.0
        elif len(crossings) == 1:
            return crossings[0]
        else:
            return 0.0
    except Exception:
        return 0.0

# ------------------------------------------------------------------------------
# Data Collection Logic
# ------------------------------------------------------------------------------

# Dictionary to store results: data[mass][channel][PaperLabel] = value_string
table_data = {m: {c: {} for c in TargetChannels} for m in TargetMasses}

# Mapping: Script Internal Label -> Paper Label
# Keys must match the 'group_labels' definition order in the loop below
label_map = {
    "Luminosity":     "Integrated luminosity",
    "Lepton uncert.": "Lepton selection \\& energy",
    "Jet uncert.":    "Jet selection \\& energy",
    "MET scale":      "Unclustered energy",
    "b-tagging":      "b tagging efficiency",
    "Pileup":         "Pileup modeling",
    "Prefiring":      "L1 prefiring",
    "HEM":            "HEM",
    "Cross section":  "Cross-section uncertainty",
    "MC stats":       "Template statistical",
    "PDF":            "PDF variation",
    "Scale":          "QCD scale variation",
    "Fake rate":      "Nonprompt leptons",
    "Charge Flip":    "Charge misidentification", 
    # Special Keys
    "Total systematic": "Total systematic",
    "Statistical":      "Data statistical"
}

print(f"Collecting data for {args.InputWP} ({args.era})...")

for mass in TargetMasses:
    for channel in TargetChannels:
        
        # Construct WorkPath
        WorkPath = f"{BasePath}/{args.InputWP}/{args.era}_{channel}_M{mass}_{SignalType}_syst/Breakdown/{AsimovName}"
        
        # Define file structure (Cumulative Freezing Order)
        # Note: Must match the original script's logic exactly
        files = [
            ("Total",            f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_total.MultiDimFit.mH120.root"),
            ("Freeze Jet",       f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet.MultiDimFit.mH120.root"),
            ("Freeze PDF",       f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf.MultiDimFit.mH120.root"),
            ("Freeze Scale",     f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale.MultiDimFit.mH120.root"),
            ("Freeze Fake",      f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake.MultiDimFit.mH120.root"),
            ("Freeze Lep",       f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep.MultiDimFit.mH120.root"),
            ("Freeze Pileup",    f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup.MultiDimFit.mH120.root"),
            ("Freeze Lumi",      f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi.MultiDimFit.mH120.root"),
            ("Freeze Btag",      f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag.MultiDimFit.mH120.root"),
            ("Freeze Prefire",   f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire.MultiDimFit.mH120.root"),
            ("Freeze MET",       f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met.MultiDimFit.mH120.root"),
            ("Freeze Xsec",      f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec.MultiDimFit.mH120.root"),
            ("Freeze HEM",       f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM.MultiDimFit.mH120.root"),
            ("Freeze MCstat",    f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat.MultiDimFit.mH120.root"),
        ]
        
        # Labels for the step-by-step subtraction
        # Order MUST correspond to the 'files' list indices [1] to [N]
        step_labels = [
            "Jet uncert.",
            "PDF",
            "Scale",
            "Fake rate",
            "Lepton uncert.",
            "Pileup",
            "Luminosity",
            "b-tagging",
            "Prefiring",
            "MET scale",
            "Cross section",
            "HEM",
            "MC stats",
        ]
        
        # Handle Charge Flip for EE, EMu
        if "E" in channel:
            files.append(("Freeze CF", f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_jet_pdf_scale_fake_lep_pileup_lumi_btag_prefire_met_xsec_HEM_mcstat_cf.MultiDimFit.mH120.root"))
            step_labels.append("Charge Flip")
            
        files.append(("Stat Only", f"{WorkPath}/higgsCombine.{args.era}_{channel}_M{mass}_{SignalType}_syst_{AsimovName}_freeze_all.MultiDimFit.mH120.root"))

        # 1. Retrieve widths
        widths = []
        for _, fname in files:
            widths.append(get_precision_width(fname))
            
        total_uncert = widths[0]
        stat_uncert = widths[-1]
        
        # If total is 0 (e.g., job failed), skip calculation
        if total_uncert == 0:
            for sl in step_labels:
                table_data[mass][channel][label_map[sl]] = "-"
            table_data[mass][channel][label_map["Statistical"]] = "-"
            table_data[mass][channel][label_map["Total systematic"]] = "-"
            continue

        # 2. Calculate Components
        prev_width = widths[0]
        
        for i, label in enumerate(step_labels):
            width_after = widths[i+1]
            diff_sq = prev_width**2 - width_after**2
            val = math.sqrt(diff_sq) if diff_sq > 0 else 0.0
            
            # Calculate Linear Relative Percentage
            pct = (val / total_uncert) * 100.0
            
            # Map to Paper Label and store formatted string
            paper_label = label_map[label]
            
            # Format: "< 0.1" if small, else "12.3%"
            if pct < 0.05:
                table_data[mass][channel][paper_label] = "$<0.1\\%$"
            else:
                table_data[mass][channel][paper_label] = f"{pct:.1f}\\%"
            
            prev_width = width_after

        # 3. Handle Charge Flip for MuMu channel
        if channel == "MuMu":
             table_data[mass][channel][label_map["Charge Flip"]] = "\\NA" # dash

        # 4. Statistical
        stat_pct = (stat_uncert / total_uncert) * 100.0
        table_data[mass][channel][label_map["Statistical"]] = f"{stat_pct:.1f}\\%"

        # 5. Total Systematic (Derived from sqrt(Total^2 - Stat^2))
        syst_sq = total_uncert**2 - stat_uncert**2
        syst_val = math.sqrt(syst_sq) if syst_sq > 0 else 0.0
        syst_pct = (syst_val / total_uncert) * 100.0
        table_data[mass][channel][label_map["Total systematic"]] = f"{syst_pct:.1f}\\%"


# ------------------------------------------------------------------------------
# LaTeX Table Generation
# ------------------------------------------------------------------------------

# Define the row order strictly as requested
row_structure = [
    #(r"\multicolumn{1}{c}{\textbf{Simulation:}}", None),
    (r"\multicolumn{1}{c}{\textbf{Simulation:}} &", None),
    (r"\multicolumn{3}{c|}{} &", None),
    (r"\multicolumn{3}{c|}{} &", None),
    (r"\multicolumn{3}{c}{} \\", None),
    ("Integrated luminosity",       "Integrated luminosity"),
    ("Lepton selection \\& energy", "Lepton selection \\& energy"),
    ("Jet selection \\& energy",    "Jet selection \\& energy"),
    ("Unclustered energy",          "Unclustered energy"),
    ("b tagging efficiency",        "b tagging efficiency"),
    ("Pileup modeling",             "Pileup modeling"),
    ("L1 prefiring",                "L1 prefiring"),
    ("HEM",                         "HEM"),
    ("Cross-section uncertainty",   "Cross-section uncertainty"),
    ("Template statistical",        "Template statistical"),
    (r"\hline", None),
    #(r"\multicolumn{1}{c}{\textbf{Theory:}}", None),
    (r"\multicolumn{1}{c}{\textbf{Theory:}} &", None),
    (r"\multicolumn{3}{c|}{} &", None),
    (r"\multicolumn{3}{c|}{} &", None),
    (r"\multicolumn{3}{c}{} \\", None),
    ("PDF variation",               "PDF variation"),
    ("QCD scale variation",         "QCD scale variation"),
    (r"\hline", None),
    #(r"\multicolumn{1}{c}{\textbf{Data-driven:}}", None),
    (r"\multicolumn{1}{c}{\textbf{Data-driven:}} &", None),
    (r"\multicolumn{3}{c|}{} &", None),
    (r"\multicolumn{3}{c|}{} &", None),
    (r"\multicolumn{3}{c}{} \\", None),
    ("Nonprompt leptons",           "Nonprompt leptons"),
    ("Charge misidentification",    "Charge misidentification"),
    (r"\hline", None),
    (r"\textbf{Total systematic}",  "Total systematic"),
    (r"\textbf{Data statistical}",       "Data statistical"),
    (r"\hline", None),
]

print("\n" + "="*50)
print(" LaTeX Code Output")
print("="*50 + "\n")

print(r"\begin{tabular}{lccc|ccc|ccc}")
print(r"\hline \hline")
print(r"\multirow{2}{*}{Source / Channel}")
print(r"    & \multicolumn{3}{c|}{100~\GeV}")
print(r"    & \multicolumn{3}{c|}{1~\TeV}")
print(r"    & \multicolumn{3}{c}{10~\TeV} \\")
print(r"    & $\mu\mu$ & $ee$ & $e\mu$")
print(r"    & $\mu\mu$ & $ee$ & $e\mu$")
print(r"    & $\mu\mu$ & $ee$ & $e\mu$ \\")
print(r"\hline")

for display_text, lookup_key in row_structure:
    if lookup_key is None:
        # It's a header or line break
        if display_text == r"\hline":
            print(display_text)
        else:
            #print(f"{display_text} \\\\")
            print(f"{display_text}")
    else:
        # It's a data row
        row_str = f"{display_text}"
        for mass in TargetMasses:
            for channel in TargetChannels:
                val = table_data[mass][channel].get(lookup_key, "-")
                row_str += f" & {val}"
        row_str += r" \\"
        print(row_str)

print(r"\end{tabular}")
