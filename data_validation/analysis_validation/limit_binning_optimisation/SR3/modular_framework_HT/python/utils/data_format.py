# =========================================================
# BUILD ARRAYS
# =========================================================
import math
import numpy as np

from python.config.default_config import ERAS, FLAVOURS
from python.utils.loader import load_background, load_signal, load_fake


def hist_to_array(h):
    return np.array([h.GetBinContent(i) for i in range(1, h.GetNbinsX() + 1)])


def bins_to_array_with_err(bins):

    n = len(bins)
    B = np.zeros(n)
    E = np.zeros(n)

    for i in range(n):
        B[i] = bins[i][2]
        E[i] = bins[i][3]

    return B, E


# ========================================================
# BUILD MAIN DATA STRUCTURE
# =========================================================
def build_data_sr3(base, masses, sig_name = "HNL" ):

    data = {}

    MET_CUTS = ["2", "3", "4", "5"]

    CATEGORIES = [
        "LowJet_HT_LT_MET{X}_LTcut",
        "HighJet_HT_LT_MET{X}_LTcut",
        "LowJet_HT_LT_MET{X}_GTcut",
        "HighJet_HT_LT_MET{X}_GTcut",
    ]

    for met in MET_CUTS:
        print(100*"_")
        data[met] = {}
        
        for cat_template in CATEGORIES:

            cat = cat_template.format(X=met)

            print(f"[INFO] Loading region: MET={met}, CAT={cat}")

            subdata = {}

            # ----------------------------------
            # Load everything
            # ----------------------------------
            bkg_all = {}
            fake_all = {}
            sig_all = {}

            for flav in FLAVOURS:

                bkg_all[flav] = load_background(base, flav, hist_name=cat)
                fake_all[flav] = load_fake(base, flav, hist_name=cat)

                sig_all[flav] = {}
                for m in masses:
                    sig_all[flav][m] = load_signal(base, flav, m, hist_name=cat,    sig_name = f"{sig_name}_{m}")

            # ----------------------------------
            # Find valid reference histogram for edges
            # ----------------------------------
            ref = None
            for flav in FLAVOURS:
                for era in ERAS:
                    if len(bkg_all[flav][era]) > 0:
                        ref = bkg_all[flav][era]
                        break
                if ref is not None:
                    break

            if ref is None:
                print(f"[WARNING] No valid histogram found for {cat}, skipping region")
                continue

            edges = np.array([b[0] for b in ref] + [ref[-1][1]])
            subdata["edges"] = edges

            nbins = len(edges) - 1

            # ----------------------------------
            # Background + Fake arrays
            # ----------------------------------
            subdata["background"] = {}
            subdata["fake"] = {}
            subdata["bkg_err2"] = {}

            for flav in FLAVOURS:

                subdata["background"][flav] = {}
                subdata["fake"][flav] = {}
                subdata["bkg_err2"][flav] = {}

                for era in ERAS:

                    bins = bkg_all[flav][era]
                    fake = fake_all[flav][era]

                    # --- Background
                    if len(bins) == 0:
                        B_arr = np.zeros(nbins)
                        E_arr = np.zeros(nbins)
                    else:
                        B_arr, E_arr = bins_to_array_with_err(bins)

                    subdata["background"][flav][era] = B_arr
                    subdata["bkg_err2"][flav][era] = E_arr

                    # --- Fake
                    if len(fake) == 0:
                        subdata["fake"][flav][era] = np.zeros(nbins)
                    else:
                        subdata["fake"][flav][era] = np.array([f[2] for f in fake])

            # ----------------------------------
            # Signal arrays
            # ----------------------------------
            subdata["signal"] = {}

            for flav in FLAVOURS:
                subdata["signal"][flav] = {}

                for m in masses:
                    subdata["signal"][flav][m] = {}

                    for era in ERAS:

                        h = sig_all[flav][m][era]

                        if h is None:
                            arr = np.zeros(nbins)
                        else:
                            arr = hist_to_array(h)

                        subdata["signal"][flav][m][era] = arr

            # ----------------------------------
            # Combine signal per mass (sum eras)
            # ----------------------------------
            subdata["signal_combined_mass"] = {}

            for flav in FLAVOURS:

                subdata["signal_combined_mass"][flav] = {}

                for m in masses:

                    arr = sum(subdata["signal"][flav][m][era] for era in ERAS)

                    if arr.sum() > 0:
                        arr = arr / arr.sum()

                    subdata["signal_combined_mass"][flav][m] = arr

            # ----------------------------------
            # Sum over masses
            # ----------------------------------
            subdata["signal_sum"] = {}

            for flav in FLAVOURS:
                total = sum(subdata["signal_combined_mass"][flav][m] for m in masses)
                subdata["signal_sum"][flav] = total

            # ----------------------------------
            # Global signal (all flavours)
            # ----------------------------------
            subdata["signal_global"] = {}

            for m in masses:

                total = 0

                for flav in FLAVOURS:
                    for era in ERAS:
                        total += subdata["signal"][flav][m][era]

                if total.sum() > 0:
                    total = total / total.sum()

                subdata["signal_global"][m] = total

            # ----------------------------------
            # Compute norm
            # ----------------------------------
            subdata["norm"] = {}

            for m in masses:

                S = subdata["signal_global"][m]

                B = sum(
                    subdata["background"][flav][era]
                    for flav in FLAVOURS
                    for era in ERAS
                )

                Z2 = 0.0

                for i in range(len(S)):
                    s = S[i]
                    b = B[i]

                    if s > 0 and b > 0:
                        Z2 += 2 * ((s + b) * math.log(1 + s / b) - s)

                subdata["norm"][m] = Z2

            # ----------------------------------
            # Store
            # ----------------------------------
            data[met][cat] = subdata

    return data
