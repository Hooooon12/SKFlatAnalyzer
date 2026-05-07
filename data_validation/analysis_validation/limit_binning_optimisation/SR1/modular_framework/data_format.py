# =========================================================                                             
# BUILD ARRAYS                                                                                          
# =========================================================                                             
import math

from default_config import ERAS, FLAVOURS


from loader import load_histogram,load_background, load_signal,load_fake

def hist_to_array(h):
    return np.array([h.GetBinContent(i) for i in range(1, h.GetNbinsX()+1)])

import numpy as np

def bins_to_array_with_err(bins):

    n = len(bins)
    B = np.zeros(n)
    E = np.zeros(n)

    for i in range(n):
        bkg = bins[i][2]
        err2 = bins[i][3]

        B[i] = bkg
        E[i] = err2

    return B, E

# =========================================================                                                                                                                                                                                                                                                                                      
# BUILD MAIN DATA STRUCTURE                                                                                                                                                                                                                                                                                                                      
# =========================================================                                                                                                                                                                                                                                                                                      

def build_data(base, masses):

    data = {}

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Load everything                                                                                                                                                                                                                                                                                                                            
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    bkg_all = {}
    fake_all = {}
    sig_all = {}

    for flav in FLAVOURS:

        bkg_all[flav] = load_background(base, flav)
        fake_all[flav] = load_fake(base, flav)

        sig_all[flav] = {}
        for m in masses:
            sig_all[flav][m] = load_signal(base, flav, m)

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Edges                                                                                                                                                                                                                                                                                                                                      
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    ref = bkg_all[FLAVOURS[0]][ERAS[0]]
    edges = np.array([b[0] for b in ref] + [ref[-1][1]])

    data["edges"] = edges

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Background + Fake arrays                                                                                                                                                                                                                                                                                                                   
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    data["background"] = {}
    data["fake"] = {}
    data["bkg_err2"] = {}

    for flav in FLAVOURS:

        data["background"][flav] = {}
        data["fake"][flav] = {}
        data["bkg_err2"][flav] = {}

        for era in ERAS:

            bins = bkg_all[flav][era]
            fake = fake_all[flav][era]
            B_arr, E_arr = bins_to_array_with_err(bins)

            #data["background"][flav][era] = bins_to_array(bins, fake)                                                                                                                                                                                                                                                                           
            data["background"][flav][era] = B_arr

            data["bkg_err2"][flav][era] = E_arr
            data["fake"][flav][era] = np.array([f[2] for f in fake])

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Signal arrays                                                                                                                                                                                                                                                                                                                              
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    data["signal"] = {}

    for flav in FLAVOURS:
        data["signal"][flav] = {}

        for m in masses:
            data["signal"][flav][m] = {}

            for era in ERAS:
                data["signal"][flav][m][era] = hist_to_array(sig_all[flav][m][era])

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Combine signal per mass (sum eras)                                                                                                                                                                                                                                                                                                         
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    data["signal_combined_mass"] = {}

    for flav in FLAVOURS:

        data["signal_combined_mass"][flav] = {}

        for m in masses:

            arr = sum(data["signal"][flav][m][era] for era in ERAS)
            for era in ERAS:
                
                arr = data["signal"][flav][m][era]
            
                print(f"{flav} {m} {era} shape = {arr.shape}")
                
            if arr.sum() > 0:
                arr = arr / arr.sum()

            data["signal_combined_mass"][flav][m] = arr

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Sum over masses                                                                                                                                                                                                                                                                                                                            
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    data["signal_sum"] = {}

    for flav in FLAVOURS:
        total = sum(data["signal_combined_mass"][flav][m] for m in masses)
        data["signal_sum"][flav] = total

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Global signal (all flavours)                                                                                                                                                                                                                                                                                                               
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    data["signal_global"] = {}

    for m in masses:

        total = 0

        for flav in FLAVOURS:
            for era in ERAS:
                total += data["signal"][flav][m][era]

        if total.sum() > 0:
            total = total / total.sum()

        data["signal_global"][m] = total

    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    # Compute norm                                                                                                                                                                                                                                                                                                                               
    # ----------------------------------                                                                                                                                                                                                                                                                                                         
    data["norm"] = {}

    for m in masses:

        S = data["signal_global"][m]
        B = sum(
            data["background"][flav][era]
            for flav in FLAVOURS
            for era in ERAS
        )

        Z2 = 0.0

        for i in range(len(S)):
            s = S[i]
            b = B[i]

            if s > 0 and b > 0:
                Z2 += 2*((s+b)*math.log(1+s/b) - s)

        data["norm"][m] = Z2

    return data

