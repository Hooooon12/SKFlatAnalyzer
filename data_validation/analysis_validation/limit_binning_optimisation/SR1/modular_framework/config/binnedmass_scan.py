from config.base_config import *

MASSES = [
    "400","450","500",
    "600","700","800","900",
    "1000","1100","1200","1300","1500","1700","2000","2500","3000"
]
NBINS_TO_SCAN=[7]


TAG="BinnedMass_Scan"
# Rebuild tag AFTER override
LOG_TAG = build_log_tag()
