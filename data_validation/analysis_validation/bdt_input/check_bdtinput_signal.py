import ROOT
import os

# ROOT histogram names
hist_names = ["sumw_EE", "sumw_EMu", "sumw_MuMu"]

# Signal masses
signal_masses = [100, 150, 200, 300, 400, 500]
c_list = [f"HNL_SR3_BDT_KinVar_DYTypeI_M{mass}.root" for mass in signal_masses]

# Versions and eras
versions = {
    "V2": "HNL_SR3_BDT_KinVar_BDTV2",
    "V3": "HNL_SR3_BDT_KinVar_BDTV3"
}
eras = ["2016postVFP", "2016preVFP", "2017", "2018"]

# Header
header = (
    "{:<12} | {:<35} | {:<8} | {:>10} {:>10} | {:>10} {:>10} || {:>9} {:>9}"
    .format("Era", "File", "Hist", "Entries V2", "Int V2", "Entries V3", "Int V3", "%Ent", "%Int")
)
print(header)
print("=" * len(header))

# Helper
def percent_change(v2, v3):
    if v2 and v2 != 0 and v3 is not None:
        return "{:+.2f}%".format((v3 - v2) / v2 * 100)
    else:
        return "   -   "

for era in eras:
    print("=" * len(header))
    print("{:<12} | {:<35} | {:<8}".format(era, "", ""))  # Era title once

    for hist_name in hist_names:
        hist_short = hist_name.replace("sumw_", "")
        print(" " * 13 + "| {:<35} | {:<8}".format("", hist_short))

        for file_name in c_list:
            values = {}
            for version_key, version_path in versions.items():
                filepath = f"/data9/Users/HNL_public/SUS-24-014/BDTInput/{version_path}/{era}/{file_name}"

                entries = None
                integral = None
                if os.path.isfile(filepath):
                    f = ROOT.TFile.Open(filepath)
                    if f and not f.IsZombie():
                        hist = f.Get(hist_name)
                        if hist:
                            entries = int(hist.GetEntries())
                            integral = round(hist.Integral(), 2)
                        f.Close()

                values[version_key] = (entries, integral)

            ent_v2, int_v2 = values["V2"]
            ent_v3, int_v3 = values["V3"]

            int_v2_str = f"{int_v2:.2f}" if int_v2 is not None else "-"
            int_v3_str = f"{int_v3:.2f}" if int_v3 is not None else "-"

            pct_entries = percent_change(ent_v2, ent_v3)
            pct_int = percent_change(int_v2, int_v3)

            print("{:<12} | {:<35} | {:<8} | {:>10} {:>10} | {:>10} {:>10} || {:>9} {:>9}".format(
                "", file_name, "",
                ent_v2 if ent_v2 is not None else "-", int_v2_str,
                ent_v3 if ent_v3 is not None else "-", int_v3_str,
                pct_entries, pct_int
            ))

        print("-" * (len(header) - 12))  # Separator under each hist block
