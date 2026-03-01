python - <<'PY'
import ROOT
import math

#fname = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_beforeFixData/2018/sr3/M400_EMu_card_input.root"
fname = "/data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv7_EMuCF_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr/2018/sr3/M400_EMu_card_input.root"

f = ROOT.TFile.Open(fname)

print("\n=== ERR^2 SANITY CHECK ===\n")

for key in f.GetListOfKeys():
    name = key.GetName()
    h = f.Get(name)
    if not h or not h.InheritsFrom("TH1"):
        continue

    nb = h.GetNbinsX()
    suspicious = 0

    for b in range(1, nb+1):
        c = h.GetBinContent(b)
        e = h.GetBinError(b)

        # 1) negative error?
        if e < 0:
            print(c,e)
            suspicious += 1

        # 2) content=0 but error>0
        if c == 0.0 and e > 0.0:
            print(c,e)
            suspicious += 1

        # 3) extremely inconsistent err^2 vs content
        # (very loose check)
        if c != 0:
            ratio = (e*e)/abs(c)
            if ratio > 1000:   # extremely large weight fluctuation
                print(c,e)
                suspicious += 1

    # Print short summary
    print(f"{name:35s}  integral={h.Integral():10.4g}  suspicious_bins={suspicious}")

print("\nDone.\n")
PY

