import os
import ROOT
import numpy as np

from python.config.default_config import ERAS, FLAVOURS

REGIONS = ["low", "high"]


# =========================================================
# HIST HELPERS
# =========================================================
def hist_to_array(h, nbins):
    if h is None:
        return np.zeros(nbins)
    return np.array([h.GetBinContent(i) for i in range(1, h.GetNbinsX() + 1)])


def get_bins(h):
    out = []
    xaxis = h.GetXaxis()

    for i in range(1, h.GetNbinsX() + 1):
        lo = xaxis.GetBinLowEdge(i)
        hi = xaxis.GetBinUpEdge(i)
        val = h.GetBinContent(i)
        err = h.GetBinError(i)
        err2 = err * err

        out.append((lo, hi, val, err2))

    return out

# =========================================================
# FILE LOADER
# =========================================================
def load_histogram(fname, hname):

    f = ROOT.TFile(fname)

    if not f or f.IsZombie():
        print(f"[WARNING] Cannot open {fname}")
        return None

    h = f.Get(hname)

    if not h:
        print(f"[WARNING] Missing histogram {hname} in {fname}")
        f.Close()
        return None

    h = h.Clone()
    h.SetDirectory(0)
    f.Close()

    return h




