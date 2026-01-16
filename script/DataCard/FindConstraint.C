// FindConstraint.C
// Usage: root -l -q 'FindConstraint.C("/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP/Run2_EE_M95_syst/Run2_EE_M95_syst.root", "CMS_btag_lf_corr")'

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooArgSet.h"
#include "TIterator.h"
#include "TString.h"

void FindConstraint(const char* filename, const char* npName) {
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    RooWorkspace *w = (RooWorkspace*)f->Get("w");
    if (!w) w = (RooWorkspace*)f->Get("combWS");

    if (!w) {
        printf("Error: Workspace not found.\n");
        return;
    }

    RooRealVar *np = w->var(npName);
    if (!np) {
        printf("Error: NP '%s' not found.\n", npName);
        return;
    }

    printf("\n>>> Hunting for Constraint Term: %s <<<\n", npName);

    RooArgSet allPdfs = w->allPdfs();
    TIterator *iter = allPdfs.createIterator();
    RooAbsPdf *pdf;
    bool foundAny = false;

    while ((pdf = (RooAbsPdf*)iter->Next())) {
        if (!pdf->dependsOn(*np)) continue;

        TString cname = pdf->ClassName();
        
        if (cname.Contains("RooRealSumPdf")) continue;
        if (cname.Contains("CMSHistErrorPropagator")) continue;
        if (cname.Contains("RooAddPdf")) continue;

        bool isLikelyConstraint = (cname.Contains("Gauss") || cname.Contains("LogNormal") || cname.Contains("Poisson"));

        if (isLikelyConstraint) {
            foundAny = true;
            printf("\n[SUSPECT FOUND] --------------------------\n");
            printf("  Name : %s\n", pdf->GetName());
            printf("  Class: %s\n", cname.Data());
            
            RooArgSet* params = pdf->getParameters((RooAbsData*)0);
            TIterator* pIter = params->createIterator();
            RooAbsArg* param;
            while((param = (RooAbsArg*)pIter->Next())) {
                 if (TString(param->GetName()) == TString(npName)) continue;
                 
                 double val = -999;
                 bool isConst = param->isConstant();
                 if (param->InheritsFrom("RooAbsReal")) val = ((RooAbsReal*)param)->getVal();
                 
                 printf("    -> Param: %-25s (Val=%.4f, Const=%d)\n", param->GetName(), val, isConst);
            }
        }
    }

    if (!foundAny) {
        printf("\n[Result] No Standard Constraint PDF found.\n");
        printf("Possibilities:\n");
        printf(" 1. This parameter is 'flatParam' (Unconstrained).\n");
        printf(" 2. The constraint is packed inside a 'RooProdPdf' that wasn't unpacked here.\n");
    }
    
    f->Close();
}
