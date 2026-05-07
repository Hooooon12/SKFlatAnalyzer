from python.config.base_config import *

TAG="DefaultScan"
# Rebuild tag AFTER override

MASSES = [
    "0",
    "1000"
]



LOG_TAG = build_log_tag(
    RUN_Z_NO_UNC,
    Bin_NBKG_REQ,
    Bin_NBKG_REQ_Tight,
    Bin_BKG_RelUnc)

    
LOG_TAG = build_log_tag()
