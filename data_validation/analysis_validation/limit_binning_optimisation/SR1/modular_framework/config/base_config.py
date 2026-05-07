MASSES = [
    "400","450","500",
    "600","700","800","900",
    "1000","1500","2000"
]
NBINS_TO_SCAN=[1,2,3,4,5,6,7]

#### Fake correction
USE_FAKE_FIX = True   

RUN_Z_NO_UNC=True

Bin_NBKG_REQ = 1.0
Bin_NBKG_REQ_Tight = 1.
Bin_BKG_RelUnc = 0.5

TAG = "Default"

def build_log_tag():
    parts = []

    # Z treatment
    if RUN_Z_NO_UNC:
        parts.append("runZnoUnc")
    else:
        parts.append("runZwithUnc")

    # Fake treatment
    if USE_FAKE_FIX:
        parts.append("FakeFix")
    else:
        parts.append("NoFakeFix")

    # Nbins
    if len(NBINS_TO_SCAN) > 1:
        parts.append(f"NBin{min(NBINS_TO_SCAN)}to{max(NBINS_TO_SCAN)}")
    else:
        parts.append(f"NBin{NBINS_TO_SCAN[0]}")

    # ----------------------------------
    # Stat config (NEW)
    # ----------------------------------
    parts.append(
        f"StatNBkg{Bin_NBKG_REQ}_Tight{Bin_NBKG_REQ_Tight}_RelErr{Bin_BKG_RelUnc}"
    )

    return "_".join(parts)

