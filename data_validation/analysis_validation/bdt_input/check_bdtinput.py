import ROOT
import os

# Configuration
versions = {
    "V3": "HNL_SR3_BDT_KinVar_BDTV3",
    "V4": "HNL_SR3_BDT_KinVar_BDTV4"
}

eras = ["2016postVFP", "2016preVFP", "2017", "2018"]

c_list = [
    "RunConv__/HNL_SR3_BDT_KinVar_Conv.root",
    "RunCF__/HNL_SR3_BDT_KinVar_CF.root",
    "RunFake__/HNL_SR3_BDT_KinVar_MCFake_SSOS.root",
    "RunPrompt__/HNL_SR3_BDT_KinVar_Prompt.root"
]

hist_names = ["sumw_EE", "sumw_EMu", "sumw_MuMu"]

# Pretty header
header = (
    "{:<12} | {:<35} | {:<8} | {:>10} {:>20} | {:>10} {:>20} || {:>9} {:>9}"
    .format("Era", "File", "Hist", "Entries V3", "Int V3 [%]", "Entries V4", "Int V4 [%]", "%Ent", "%Int")
)
print(header)
print("=" * len(header))

def percent_change(v3, v4):
    if v3 and v3 != 0 and v4 is not None:
        return "{:+.2f}%".format((v4 - v3) / v3 * 100)
    else:
        return "   -   "

for era in eras:
    print("=" * len(header))
    print("{:<12} | {:<35} | {:<8}".format(era, "", ""))  # Era line once

    for hist_name in hist_names:
        hist_short = hist_name.replace("sumw_", "")

        # Collect all integrals/entries first
        total_integrals = {"V3": 0.0, "V4": 0.0}
        integrals_by_file = {"V3": {}, "V4": {}}
        entries_by_file = {"V3": {}, "V4": {}}

        for c in c_list:
            file_display_name = os.path.basename(c)

            for version_key, version_path in versions.items():
                if c.startswith("/"):
                    filepath = c.replace(versions["V3"], version_path).replace(versions["V4"], version_path)
                else:
                    filepath = f"/data9/Users/HNL_public/SUS-24-014/BDTInput/{version_path}/{era}/{c}"

                entries = None
                integral = 0.0
                if os.path.isfile(filepath):
                    f = ROOT.TFile.Open(filepath)
                    if f and not f.IsZombie():
                        hist = f.Get(hist_name)
                        if hist:
                            entries = int(hist.GetEntries())
                            integral = hist.Integral()
                        f.Close()

                integrals_by_file[version_key][file_display_name] = integral
                entries_by_file[version_key][file_display_name] = entries
                total_integrals[version_key] += integral

        # Print histogram block header line
        print(" " * 13 + "| {:<35} | {:<8}".format("", hist_short))

        for c in c_list:
            file_display_name = os.path.basename(c)

            ent_v3 = entries_by_file["V3"][file_display_name]
            ent_v4 = entries_by_file["V4"][file_display_name]

            int_v3_val = integrals_by_file["V3"][file_display_name]
            int_v4_val = integrals_by_file["V4"][file_display_name]

            int_v3_pct = (int_v3_val / total_integrals["V3"] * 100) if total_integrals["V3"] > 0 else 0
            int_v4_pct = (int_v4_val / total_integrals["V4"] * 100) if total_integrals["V4"] > 0 else 0

            int_v3_str = f"{int_v3_val:6.2f} [{int_v3_pct:5.2f}%]" if int_v3_val is not None else "-"
            int_v4_str = f"{int_v4_val:6.2f} [{int_v4_pct:5.2f}%]" if int_v4_val is not None else "-"

            pct_entries = percent_change(ent_v3, ent_v4)
            pct_int = percent_change(int_v3_val, int_v4_val)

            print("{:<12} | {:<35} | {:<8} | {:>10} {:>20} | {:>10} {:>20} || {:>9} {:>9}".format(
                "", file_display_name, "",
                ent_v3 if ent_v3 is not None else "-",
                int_v3_str,
                ent_v4 if ent_v4 is not None else "-",
                int_v4_str,
                pct_entries,
                pct_int
            ))

        # Separator after each histogram block
        print("-" * (len(header) - 12))  # exclude Era col indent
