// Example usage:
//   root -l -b -q 'corr_dual.C("fitDiagnostics_Run2_EE_M95_syst.root",0.8,true,true)'
// Arguments:
//   1) FitDiagnostics file path // JH : correlation tends to have 1.0s when it's b-only. Make FitDiagnostics with expectSignal=1 and it will be more informative.
//   2) correlation threshold (e.g. 0.8)
//   3) excludeBBB: true = remove autoMCStats nuisances (prop_*), false = include them
//   4) compareDelta: true = compute |rho_s+b| - |rho_b-only| and save top 30 differences

#include <vector>
#include <tuple>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>

bool EXCLUDE_BBB = false;
bool COMPARE_DELTA = false;

// Identify autoMCStats nuisances (Bin-By-Bin, "prop_*")
bool isBBB(const char* name) {
  if (!name) return false;
  std::string s(name);
  return s.rfind("prop_", 0) == 0;
}

// Identify global observables (e.g. *_In, ONE)
bool isGlobalObservable(const char* name) {
  std::string s(name ? name : "");
  if (s == "ONE") return true;
  if (s.size() >= 3 && s.rfind("_In") == s.size() - 3) return true;
  return false;
}

// Determine if a label should be excluded from correlation matrix
bool isExcludedLabel(const char* lab) {
  if (isGlobalObservable(lab)) return true;
  if (EXCLUDE_BBB && isBBB(lab)) return true;
  return false;
}

// Clone an empty histogram with only selected bin labels
TH2D* cloneEmptyLike(const TH2* src, const char* newname, int nkeep, const std::vector<int>& keep) {
  if (!src) return nullptr;
  auto out = new TH2D(newname, newname, nkeep, 0.5, nkeep + 0.5, nkeep, 0.5, nkeep + 0.5);
  for (int i = 1; i <= nkeep; ++i) {
    const char* lab = src->GetXaxis()->GetBinLabel(keep[i - 1]);
    out->GetXaxis()->SetBinLabel(i, lab);
    out->GetYaxis()->SetBinLabel(i, lab);
  }
  return out;
}

// Filter out unwanted labels (globals, BBB if requested)
TH2D* filterMatrixByLabels(const TH2* H, const char* name) {
  if (!H) return nullptr;
  const int n = H->GetNbinsX();
  std::vector<int> keep;
  keep.reserve(n);
  for (int i = 1; i <= n; ++i) {
    const char* lab = H->GetXaxis()->GetBinLabel(i);
    if (!isExcludedLabel(lab)) keep.push_back(i);
  }
  const int m = (int)keep.size();
  if (m == 0) return nullptr;

  TH2D* F = cloneEmptyLike(H, name, m, keep);
  for (int ii = 1; ii <= m; ++ii) {
    for (int jj = 1; jj <= m; ++jj) {
      F->SetBinContent(ii, jj, H->GetBinContent(keep[ii - 1], keep[jj - 1]));
    }
  }
  return F;
}

// Convert covariance matrix into correlation matrix (after filtering)
TH2D* cov2corr_filtered(const TH2* C, const char* name) {
  if (!C) return nullptr;
  std::unique_ptr<TH2D> Cf(filterMatrixByLabels(C, (std::string(name) + "_tmpcov").c_str()));
  if (!Cf) return nullptr;

  const int n = Cf->GetNbinsX();
  auto R = (TH2D*)Cf->Clone(name);
  R->Reset();

  for (int i = 1; i <= n; ++i) {
    const double vii = Cf->GetBinContent(i, i);
    for (int j = 1; j <= n; ++j) {
      const double vjj = Cf->GetBinContent(j, j);
      const double vij = Cf->GetBinContent(i, j);
      double rho = 0.0;
      if (vii > 0.0 && vjj > 0.0) {
        const double denom = std::sqrt(vii * vjj);
        if (denom > 0.0) rho = vij / denom;
      }
      if (!std::isfinite(rho)) rho = 0.0;
      rho = std::max(-1.0, std::min(1.0, rho));
      R->SetBinContent(i, j, rho);
    }
  }
  R->SetTitle(name);
  return R;
}

// Draw correlation matrix and print high-correlation pairs
void dumpCorr(TH2* H, const char* tag, double thr = 0.8) {
  if (!H) { printf("[%-4s] matrix missing\n", tag); return; }

  TCanvas* c = new TCanvas(Form("c_%s", tag), Form("Correlation %s", tag), 1200, 1000);
  H->SetTitle(Form("Correlation matrix (%s)", tag));
  H->GetZaxis()->SetRangeUser(-1, 1);
  H->Draw("COLZ");
  c->SaveAs(Form("corr_matrix_%s.pdf", tag));
  printf("[%-4s] saved heatmap -> corr_matrix_%s.pdf\n", tag, tag);

  FILE* fp = fopen(Form("corr_pairs_%s.txt", tag), "w");
  fprintf(fp, "Highly correlated nuisance pairs (|rho| > %.2f) — %s\n", thr, tag);
  const int n = H->GetNbinsX();
  for (int i = 1; i <= n; ++i) {
    for (int j = i + 1; j <= n; ++j) {
      const double r = std::fabs(H->GetBinContent(i, j));
      if (r > thr) {
        const char* a = H->GetXaxis()->GetBinLabel(i);
        const char* b = H->GetXaxis()->GetBinLabel(j);
        if (strcmp(a, b) != 0) {
          printf("|rho|=%.3f : %s ~ %s  (%s)\n", r, a, b, tag);
          fprintf(fp, "|rho|=%.3f : %s ~ %s\n", r, a, b);
        }
      }
    }
  }
  fclose(fp);
  printf("[%-4s] dumped pairs -> corr_pairs_%s.txt\n", tag, tag);
}

// Main function: load matrices, filter, dump, and optionally compare b-only vs s+b
void corr_dual(const char* fin = "fitDiagnostics.root", double thr = 0.8, bool excludeBBB = false, bool compareDelta = false) {
  EXCLUDE_BBB = excludeBBB;
  COMPARE_DELTA = compareDelta;

  gSystem->RedirectOutput("corr_dual_log.txt", "w");

  TFile* f = TFile::Open(fin);
  if (!f || f->IsZombie()) {
    printf("Cannot open %s\n", fin);
    gSystem->RedirectOutput(0);
    return;
  }

  printf("Input: %s | threshold: %.2f | EXCLUDE_BBB = %s | COMPARE_DELTA = %s\n",
         fin, thr, EXCLUDE_BBB ? "true" : "false", COMPARE_DELTA ? "true" : "false");

  // --- b-only fit
  TH2* Rb_raw = (TH2*)f->Get("correlation_fit_b");
  TH2* Rb = Rb_raw ? filterMatrixByLabels(Rb_raw, "correlation_fit_b_filtered")
                   : cov2corr_filtered((TH2*)f->Get("covariance_fit_b"), "correlation_from_cov_b_filtered");
  dumpCorr(Rb, "b-only", thr);

  // --- s+b fit
  TH2* Rs_raw = (TH2*)f->Get("correlation_fit_s");
  TH2* Rs = Rs_raw ? filterMatrixByLabels(Rs_raw, "correlation_fit_s_filtered")
                   : cov2corr_filtered((TH2*)f->Get("covariance_fit_s"), "correlation_from_cov_s_filtered");
  dumpCorr(Rs, "splusb", thr);

  // --- Compare |rho_s+b| - |rho_b-only|
  if (COMPARE_DELTA && Rb && Rs) {
    std::map<std::string,int> idx_b, idx_s;
    const int nb = Rb->GetNbinsX(), ns = Rs->GetNbinsX();
    for (int i = 1; i <= nb; ++i) idx_b[Rb->GetXaxis()->GetBinLabel(i)] = i;
    for (int j = 1; j <= ns; ++j) idx_s[Rs->GetXaxis()->GetBinLabel(j)] = j;

    std::vector<std::tuple<double,std::string,std::string>> diffs;
    for (auto& kv_i : idx_b) {
      auto it_i = idx_s.find(kv_i.first);
      if (it_i == idx_s.end()) continue;
      for (auto& kv_j : idx_b) {
        if (kv_j.first <= kv_i.first) continue;
        auto it_j = idx_s.find(kv_j.first);
        if (it_j == idx_s.end()) continue;
        const double rb = std::fabs(Rb->GetBinContent(kv_i.second, kv_j.second));
        const double rs = std::fabs(Rs->GetBinContent(it_i->second, it_j->second));
        diffs.emplace_back(rs - rb, kv_i.first, kv_j.first);
      }
    }
    std::partial_sort(diffs.begin(), diffs.begin() + std::min<size_t>(30, diffs.size()), diffs.end(),
                      [](auto& L, auto& R) { return std::get<0>(L) > std::get<0>(R); });

    FILE* fp = fopen("corr_delta_top.txt", "w");
    const int lim = std::min<int>(30, diffs.size());
    for (int k = 0; k < lim; ++k) {
      auto& t = diffs[k];
      fprintf(fp, "Delta=%.3f : %s ~ %s (|rho|_s+b - |rho|_b)\n",
              std::get<0>(t), std::get<1>(t).c_str(), std::get<2>(t).c_str());
    }
    fclose(fp);
    printf("Saved top correlation deltas -> corr_delta_top.txt\n");
  }

  gSystem->RedirectOutput(0);
  printf("All logs saved to corr_dual_log.txt\n");
}

