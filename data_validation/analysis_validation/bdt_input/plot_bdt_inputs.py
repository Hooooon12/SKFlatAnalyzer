import ROOT
import os
import math

ROOT.gROOT.SetBatch(True)

eras = ["2016postVFP", "2016preVFP", "2017", "2018"]
trees = ["Tree_mm", "Tree_ee", "Tree_em"]
input_dir_v3 = "/data9/Users/HNL_public/SUS-24-014/BDTInput/HNL_SR3_BDT_KinVar_BDTV3"
input_dir_v4 = "/data9/Users/HNL_public/SUS-24-014/BDTInput/HNL_SR3_BDT_KinVar_BDTV4"
output_dir = "plots_v3_vs_v4"
range_file = "variable_ranges.txt"

file_dirs = {
    "RunConv__": "HNL_SR3_BDT_KinVar_Conv.root",
    "RunCF__": "HNL_SR3_BDT_KinVar_CF.root",
    "RunFake__": "HNL_SR3_BDT_KinVar_MCFake_SSOS.root",
    "RunPrompt__": "HNL_SR3_BDT_KinVar_Prompt.root",
}

os.makedirs(output_dir, exist_ok=True)

# Extract all branches from the Prompt file
prompt_file_path = f"{input_dir_v3}/2016postVFP/RunPrompt__/HNL_SR3_BDT_KinVar_Prompt.root"
f_prompt = ROOT.TFile.Open(prompt_file_path)
t_prompt = f_prompt.Get("Tree_mm")

if not os.path.exists(range_file):
    print(f"Creating default {range_file}...")
    with open(range_file, "w") as f:
        for branch in t_prompt.GetListOfBranches():
            name = branch.GetName()
            f.write(f"{name} 0 500\n")
    print(f"Please edit {range_file} to set custom ranges.")

# Load variable ranges
variable_ranges = {}
with open(range_file, "r") as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) != 3:
            continue
        var, xmin, xmax = parts
        variable_ranges[var] = (float(xmin), float(xmax))

def get_combined_hist(tree_name, era, version_dir, branch_name):
    hist = None
    for subdir, filename in file_dirs.items():
        full_path = f"{version_dir}/{era}/{subdir}/{filename}"
        if not os.path.exists(full_path):
            print(f"WARNING: File not found: {full_path}")
            continue

        f = ROOT.TFile.Open(full_path)
        t = getattr(f, tree_name, None)
        if not t or not t.GetListOfBranches().FindObject(branch_name):
            print(f"WARNING: Branch '{branch_name}' not found in {tree_name} of {full_path}")
            f.Close()
            continue

        if branch_name not in variable_ranges:
            print(f"WARNING: '{branch_name}' missing in {range_file}, skipping.")
            f.Close()
            continue

        xmin, xmax = variable_ranges[branch_name]
        nbins = 100
        histname = f"temp_hist_{branch_name}_{tree_name}_{subdir}"
        ROOT.gDirectory.Delete(histname + ";*")
        draw_expr = f"{branch_name} >> {histname}({nbins}, {xmin}, {xmax})"
        n_drawn = t.Draw(draw_expr, "w_tot", "goff")

        temp_hist = ROOT.gDirectory.Get(histname)
        if n_drawn <= 0 or not temp_hist or temp_hist.GetEntries() == 0:
            print(f"WARNING: Empty hist for '{branch_name}' in {tree_name} of {full_path}")
            f.Close()
            continue

        if not hist:
            hist = temp_hist.Clone(f"sum_{branch_name}_{tree_name}")
            hist.SetDirectory(0)
        else:
            hist.Add(temp_hist)

        # Rebin if wide range
        if (xmax - xmin) > 1000:
            hist.Rebin(2)
        elif (xmax - xmin) > 100:
            hist.Rebin(2)
        f.Close()
    return hist

def normalize(hist):
    if hist and hist.Integral() > 0:
        hist.Scale(1.0 / hist.Integral())

def plot_comparison(hist1, hist2, branch_name, tree_name, era):
    c = ROOT.TCanvas("c", "", 800, 600)
    hist1.SetLineColor(ROOT.kRed)
    hist2.SetLineColor(ROOT.kBlue)

    normalize(hist1)
    normalize(hist2)

    hist1.SetTitle(f"{branch_name} | {tree_name} | {era}")
    hist1.GetXaxis().SetTitle(branch_name)
    hist1.GetYaxis().SetTitle("Normalized")
    ROOT.gStyle.SetOptStat(0)
    ymax = max(hist1.GetMaximum(), hist2.GetMaximum())
    hist1.SetMaximum(ymax * 1.4)

    hist1.Draw("hist")
    hist2.Draw("hist same")

    leg = ROOT.TLegend(0.65, 0.75, 0.88, 0.88)
    leg.AddEntry(hist1, "BDTV4", "l")
    leg.AddEntry(hist2, "BDTV3", "l")
    leg.Draw()

    # Create nested output directory
    tree_dir = os.path.join(output_dir, era, tree_name)
    os.makedirs(tree_dir, exist_ok=True)

    # Save plot as PDF
    outname = os.path.join(tree_dir, f"{branch_name}.pdf")
    c.SaveAs(outname)
    c.Close()

# Main loop
for era in eras:
    for tree_name in trees:
        print(f"\n--- {tree_name} | {era} ---")
        for i, branch in enumerate(variable_ranges.keys(), 1):
            print(f"  - {i}/{len(variable_ranges)}: {branch}")

            hist_v4 = get_combined_hist(tree_name, era, input_dir_v4, branch)
            hist_v3 = get_combined_hist(tree_name, era, input_dir_v3, branch)

            if not hist_v4:
                print(f"    [SKIP] No valid BDTV4 hist for {branch} in {tree_name} | {era}")
                continue
            if not hist_v3:
                print(f"    [SKIP] No valid BDTV3 hist for {branch} in {tree_name} | {era}")
                continue

            plot_comparison(hist_v4, hist_v3, branch, tree_name, era)

print(f"\n[OK] All comparison plots saved under: {output_dir}")
