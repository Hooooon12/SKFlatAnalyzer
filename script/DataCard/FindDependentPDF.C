// FindConstraint.C
// Usage: root -l -q 'FindDependentPDF.C("/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP/Run2_EE_M95_syst/Run2_EE_M95_syst.root", "CMS_btag_lf_corr")'

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooArgSet.h"
#include "TIterator.h"
#include "TString.h"

void FindDependentPDF(const char* filename, const char* npName) {
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    RooWorkspace *w = (RooWorkspace*)f->Get("w");
    if (!w) {
        w = (RooWorkspace*)f->Get("combWS");
        if(!w) w = (RooWorkspace*)f->Get("ws_combined");
    }

    if (!w) {
        printf("Error: Cannot find RooWorkspace (tried 'w', 'combWS', etc).\n");
        return;
    }

    RooRealVar *np = w->var(npName);
    if (!np) {
        printf("Error: NP '%s' not found in workspace.\n", npName);
        return;
    }

    printf("\n>>> Scanning ALL PDFs depending on NP: %s <<<\n", npName);

    RooArgSet allPdfs = w->allPdfs();
    TIterator *iter = allPdfs.createIterator();
    RooAbsPdf *pdf;
    int count = 0;

    while ((pdf = (RooAbsPdf*)iter->Next())) {
        if (pdf->dependsOn(*np)) {
            count++;
            printf("--------------------------------------------------\n");
            printf("Found Dependent PDF #%d\n", count);
            printf("  Name : %s\n", pdf->GetName());
            printf("  Class: %s\n", pdf->ClassName());
            
            printf("  > Parameters:\n");
            RooArgSet* params = pdf->getParameters((RooAbsData*)0);
            TIterator* pIter = params->createIterator();
            RooAbsArg* param;
            while((param = (RooAbsArg*)pIter->Next())) {
                 if (TString(param->GetName()) == TString(npName)) continue; // 본인 제외
                 
                 double val = 0.0;
                 if (param->InheritsFrom("RooAbsReal")) val = ((RooAbsReal*)param)->getVal();
                 
                 printf("    - %-25s [%s] Val=%.4f\n", 
                        param->GetName(), param->ClassName(), val);
            }
        }
    }

    if (count == 0) {
        printf("\n[Mystery] No PDFs depend on this variable. Is it a 'dead' variable?\n");
    }
    
    f->Close();
}
