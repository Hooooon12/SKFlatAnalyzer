def GetScanName(config):

    scan_type = config.get("scan_type", "Unknown")
    nbin_mode = config.get("nbin_mode", "Unknown")
    opt_mode  = config.get("opt_mode", "Unknown")

    # format nbin nicely
    if isinstance(nbin_mode, int):
        nbin_str = f"{nbin_mode}bin"
    else:
        nbin_str = str(nbin_mode)

    name = f"{scan_type}_{nbin_str}_{opt_mode}"

    return name

def validate_config(cfg):
    """
    Validate required config fields and print a clear summary.
    Raises an error if anything critical is missing.
    """

    required_fields = [
        "MASSES",
        "USE_FAKE_FIX",
        "RUN_Z_NO_UNC",
        "LOG_TAG",
        "TAG",
    ]

    optional_fields = [
        "Bin_NBKG_REQ",
        "Bin_NBKG_REQ_Tight",
        "Bin_BKG_RelUnc",
    ]

    print("\n==============================")
    print(" CONFIG VALIDATION")
    print("==============================")

    missing = []
    for field in required_fields:
        if not hasattr(cfg, field):
            missing.append(field)
        else:
            print(f"[OK] {field:20s} = {getattr(cfg, field)}")

    if missing:
        print("\n[ERROR] Missing required config fields:")
        for m in missing:
            print(f"  - {m}")
        raise AttributeError(
            f"[CONFIG ERROR] Missing required fields: {missing} "
            f"in config '{cfg.__name__}'"
        )

    print("\n[OPTIONAL CHECK]")
    for field in optional_fields:
        if hasattr(cfg, field):
            print(f"[OK] {field:20s} = {getattr(cfg, field)}")
        else:
            print(f"[WARN] {field:20s} not set (using defaults)")

    # -------------------------
    # Sanity checks
    # -------------------------
    print("\n[SANITY CHECKS]")

    if len(cfg.MASSES) == 0:
        raise ValueError("[CONFIG ERROR] MASSES is empty")

    if not isinstance(cfg.USE_FAKE_FIX, bool):
        raise TypeError("[CONFIG ERROR] USE_FAKE_FIX must be True/False")

    if not isinstance(cfg.RUN_Z_NO_UNC, bool):
        raise TypeError("[CONFIG ERROR] RUN_Z_NO_UNC must be True/False")

    print("[OK] Basic sanity checks passed")

    print("==============================\n")
