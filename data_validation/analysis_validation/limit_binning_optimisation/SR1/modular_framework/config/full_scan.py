from config.base_config import *

MASSES = [
    "400","450","500",
    "600","700","800","900",
    "1000","1500","2000"
]
NBINS_TO_SCAN=[5,6,7]


TAG="Full_Scan"
# Rebuild tag AFTER override
LOG_TAG = build_log_tag()
