MASSES = [
    "600","700","800","900",
    "1000","1500","2000"
]


#### Fake correction
USE_FAKE_FIX = True   

RUN_Z_NO_UNC=True

Bin_NBKG_REQ = 1.0
Bin_NBKG_REQ_Tight = 1.0
Bin_BKG_RelUnc = 0.5

TAG = "Default"
NBinScan=False
RunGlobalSig=True



def build_log_tag(
    runglobalsig=None,
    nbinscan=None,
    use_fake_fix=None,
    run_z_no_unc=None,
    bin_nbkg_req=None,
    bin_nbkg_req_tight=None,
    bin_bkg_relunc=None
):
    # fallback to globals if None
    if runglobalsig is None:
        runglobalsig = RunGlobalSig
    if nbinscan is None:
        nbinscan = NBinScan
    if use_fake_fix is None:
        use_fake_fix = USE_FAKE_FIX
    if run_z_no_unc is None:
        run_z_no_unc = RUN_Z_NO_UNC
    if bin_nbkg_req is None:
        bin_nbkg_req = Bin_NBKG_REQ
    if bin_nbkg_req_tight is None:
        bin_nbkg_req_tight = Bin_NBKG_REQ_Tight
    if bin_bkg_relunc is None:
        bin_bkg_relunc = Bin_BKG_RelUnc

    parts = []

    if run_z_no_unc:
        parts.append("runZnoUnc")
    else:
        parts.append("runZwithUnc")

    if use_fake_fix:
        parts.append("FakeFix")
    else:
        parts.append("NoFakeFix")

    if nbinscan:
        parts.append("VaryNBin")
    else:
        parts.append("NoVaryNBin")

    if runglobalsig:
        parts.append("GlobalSigScan")
    else:
        parts.append("NoGlobalSigScan")

    parts.append(
        f"StatNBkg{bin_nbkg_req}_Tight{bin_nbkg_req_tight}_RelErr{bin_bkg_relunc}"
    )

    return "_".join(parts)



# Rebuild tag AFTER override                                                                                                                                                                       
LOG_TAG = build_log_tag()
