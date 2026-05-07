# =========================================================                                                                                                                        # LOADERS                                                                                                                                                                          
# =========================================================
import os,ROOT
from default_config import ERAS, FLAVOURS

def load_histogram(file_path, hist_path):
    f = ROOT.TFile(file_path)
    if not f or f.IsZombie():
        raise RuntimeError(f"Cannot open {file_path}")

    h = f.Get(hist_path)
    if not h:
        raise RuntimeError(f"Missing {hist_path} in {file_path}")

    h = h.Clone()
    h.SetDirectory(0)
    f.Close()

    return h



def load_background(base, flav):

    hist = {}

    path = f"PassSR1/HNL_ULIDv2/{flav}/AK8/AK8J_Unbinned_Mass/l1J"

    for era in ERAS:
        fname = os.path.join(base, era, "HNL_SignalRegion_Plotter_Bkg.root")
        h = load_histogram(fname, path)
        hist[era] = get_bins(h)

    return hist


def load_fake(base, flav):

    hist = {}

    path = f"PassSR1/HNL_ULIDv2/{flav}/AK8/AK8J_Unbinned_Mass/l1J"

    for era in ERAS:
        fname = os.path.join(base, era, "HNL_SignalRegion_Plotter_Fake.root")
        h = load_histogram(fname, path)
        hist[era] = get_bins(h)

    return hist


def load_signal(base, flav, mass):

    hist = {}

    path = f"PassSR1/HNL_ULIDv2/{flav}/AK8/AK8J_Unbinned_Mass/l1J"
    sig_name = f"HNL_DYVBF_{mass}"
    
    total_integral = 0.0

    # ----------------------------------
    # Load histograms WITHOUT scaling
    # ----------------------------------
    for era in ERAS:
        fname = os.path.join(base, era, f"HNL_SignalRegion_Plotter_{sig_name}.root")
        h = load_histogram(fname, path)

        hist[era] = h
        total_integral += h.Integral()

    # ----------------------------------
    # Normalise at Run2 level
    # ----------------------------------
    if total_integral > 0:
        for era in ERAS:
            h = hist[era]
            h.Scale(1.0 / total_integral)

    return hist

def get_bins(h):
    out = []
    for i in range(1, h.GetNbinsX()+1):
        lo = h.GetBinLowEdge(i)
        hi = h.GetBinLowEdge(i+1)
        val = h.GetBinContent(i)
        err = h.GetBinError(i)
        #rel = err/val if val > 0 else 0
        err2 = err * err
        
        out.append((lo, hi, val, err2))
    return out

