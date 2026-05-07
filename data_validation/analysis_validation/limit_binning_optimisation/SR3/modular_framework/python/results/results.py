
# results.py

# =========================================================
# EMPTY RESULT TEMPLATE
# =========================================================

def empty_result():
    return {
        "flav": None,
        "mass": None,

        "best_met": None,

        # physics outputs
        "Z_run2": 0.0,
        "Z_quad": 0.0,

        # main content
        "best_regions": {},

        # scan summary
        "per_met": []
    }


# =========================================================
# EMPTY REGION TEMPLATE (optional helper)
# =========================================================

def empty_region():
    return {
        "bins": [],

        # per-bin physics
        "bin_info": [],

        # region summary
        "Z": 0.0,
        "B": 0.0
    }


# =========================================================
# VALIDATION
# =========================================================

def validate_result_structure(res):

    required_keys = [
        "flav",
        "mass",
        "best_met",
        "Z_run2",
        "Z_quad",
        "best_regions",
        "per_met"
    ]

    if not isinstance(res, dict):
        return False, "Result is not a dict"

    for key in required_keys:
        if key not in res:
            return False, f"Missing key: {key}"

    if not isinstance(res["best_regions"], dict):
        return False, "best_regions must be dict"

    if not isinstance(res["per_met"], list):
        return False, "per_met must be list"

    return True, "OK"


# =========================================================
# OPTIONAL: DEEP STRUCTURE CHECK
# =========================================================

def recursive_keys(d, prefix=""):
    keys = []
    for k, v in d.items():
        full = f"{prefix}.{k}" if prefix else k
        keys.append(full)
        if isinstance(v, dict):
            keys.extend(recursive_keys(v, full))
    return keys


def compare_result_structure(a, b):
    return set(recursive_keys(a)) == set(recursive_keys(b))


# =========================================================
# OPTIONAL: CLEAN NONE VALUES (useful for safety)
# =========================================================

def remove_none(d):
    if isinstance(d, dict):
        return {
            k: remove_none(v)
            for k, v in d.items()
            if v is not None
        }
    elif isinstance(d, list):
        return [remove_none(x) for x in d]
    else:
        return d

