import ROOT

#from python.scan.ref_fom_utils import evaluate_sr2_run2_with_boundary

def build_sr2_plot_results(data):

    # run main evaluator                                                                                                                                                                                    
    res = []#evaluate_sr2_run2_with_boundary(data)

    results_for_plots = []

    # Run2                                                                                                                                                                                                  
    results_for_plots.append({
        "results": convert_results_for_plot(res, mode="run2"),
        "raw": res,
        "label": "SR2 Run2",
    })

    # Quad                                                                                                                                                                                                  
    results_for_plots.append({
        "results": convert_results_for_plot(res, mode="quad"),
        "raw": res,
        "label": "SR2 Quad",
    })

    return results_for_plots



def convert_results_for_plot(results, mode="run2"):

    out = {}

    if isinstance(results, dict):
        iterable = []
        for flav in results:
            iterable.extend(results[flav])
    else:
        iterable = results

    seen = set()

    for r in iterable:
        flav = r["flav"]
        mass = r["mass"]
        met = r.get("met", "NA")

        key = (flav, mass, met)   # safer                                                                                                                                                                                                                             

        if key in seen:
            continue
        seen.add(key)

        if flav not in out:
            out[flav] = []

        m = float(mass)
        val = r["run2"] if mode == "run2" else r["quad"]

        out[flav].append((m, val, met))

    for flav in out:
        out[flav].sort(key=lambda x: x[0])

    return out



def convert_scan_results_for_plot(results, mode="run2"):

    out = {}

    if isinstance(results, dict):
        iterable = []
        for flav in results:
            iterable.extend(results[flav])
    else:
        iterable = results

    seen = set()

    for r in iterable:
        flav = r["flavs"]
        mass = r["masses"]
        met = r.get("met", "NA")

        key = (flav, mass, met)   # safer

        if key in seen:
            continue
        seen.add(key)

        if flav not in out:
            out[flav] = []

        m = float(mass)
        val = r["run2"] if mode == "run2" else r["quad"]

        out[flav].append((m, val, met))

    for flav in out:
        out[flav].sort(key=lambda x: x[0])

    return out



def make_mass_plot_multi(results_list, flav, LOG_TAG, out_tag="default"):

    import os
    c = ROOT.TCanvas(f"c_mass_{flav}", "", 800, 700)
    c.SetLeftMargin(0.12)
    c.SetBottomMargin(0.12)

    graphs = []
    all_y = []

    marker_styles = [20, 21, 22, 23, 24, 25, 26, 27, 28, 30]
    line_styles   = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    colors        = [1, 2, 4, 6, 8, 9, 28, 46, 38, 41, 30, 12, 14, 15]

    for idx, entry in enumerate(results_list):

        results = entry["results"]
        label   = entry["label"]

        if not isinstance(results, dict):
            print(f"[ERROR] Bad results format for label {label}: {type(results)}")
            continue
        
        if flav not in results:
            print(f"[DEBUG] {label} missing {flav}")
            continue

        data = results[flav]

        # expect list of (mass, value, met)
        points = data

        if not points:
            print(f"[DEBUG] {label} empty")
            continue

        g = ROOT.TGraph()

        for i, (m, f, met) in enumerate(points):
            g.SetPoint(i, m, f)
            all_y.append(f)

        color = colors[idx % len(colors)]

        g.SetLineStyle(line_styles[idx % len(line_styles)])
        g.SetMarkerStyle(marker_styles[idx % len(marker_styles)])
        g.SetLineColor(color)
        g.SetMarkerColor(color)
        g.SetLineWidth(2)

        graphs.append((g, label, points))

    if not graphs:
        print("[WARNING] No graphs to draw")
        return

    # ------------------
    # Draw
    # ------------------
    first = True
    for g, _, _ in graphs:
        if first:
            g.Draw("APL")
            g.GetXaxis().SetTitle("Mass [GeV]")
            g.GetYaxis().SetTitle("Significance Z (asymptotic)")
            g.SetMinimum(0.0)
            first = False
        else:
            g.Draw("PL SAME")

    if all_y:
        ymax = max(all_y)
        graphs[0][0].SetMaximum(max(1.0, ymax * 1.5))

    # ------------------
    # Legend
    # ------------------
    leg = ROOT.TLegend(0.15, 0.65, 0.45, 0.88)
    leg.SetTextFont(42)
    leg.SetBorderSize(0)
    leg.SetFillStyle(0)
    leg.SetTextSize(0.022)
    leg.SetEntrySeparation(0.005)
    leg.SetMargin(0.12)

    for g, label, points in graphs:
        # include MET in legend (use first point's MET)
        #    met = points[0][2] if len(points[0]) > 2 else "NA"
        label_with_met = f"{label}"
        leg.AddEntry(g, label_with_met, "lp")

    leg.Draw()

    # ------------------
    # Text
    # ------------------
    txt = ROOT.TLatex()
    txt.SetNDC()
    txt.SetTextSize(0.04)
    txt.DrawLatex(0.18, 0.92, flav)
    txt.DrawLatex(0.6, 0.92, "13 TeV, Run2")

    # ------------------
    # Save
    # ------------------

    BASE_DIR = "/data6/Users/jalmond/HNL/SKFlatAnalyzer/data_validation/analysis_validation/limit_binning_optimisation/SR2/modular_framework/"
    #os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    outdir = os.path.join(BASE_DIR, "output", "plots", LOG_TAG, out_tag)
    os.makedirs(outdir, exist_ok=True)

    c.SaveAs(f"{outdir}/fom_vs_mass_{flav}.pdf")


