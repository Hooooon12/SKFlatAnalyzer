from python.config.base_config import *


TAG="Full_Scan_SingleJob"
# Rebuild tag AFTER override

MASSES = [
    "600",
    "700","800","900",
    "1000","2000"
]
MASSES = [
    "600"]

LOG_TAG = build_log_tag(
    RunGlobalSig,
    NBinScan,
    USE_FAKE_FIX,
    RUN_Z_NO_UNC,
    Bin_NBKG_REQ,
    Bin_NBKG_REQ_Tight,
    Bin_BKG_RelUnc)

    
LOG_TAG = build_log_tag()
