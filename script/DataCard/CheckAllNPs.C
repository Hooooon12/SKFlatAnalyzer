// CheckAllNPs.C
// Usage: root -l -q 'CheckAllNPs.C("/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP/Run2_EE_M95_syst/Run2_EE_M95_syst.root")'
//

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooRealVar.h"
#include "RooAbsPdf.h"
#include "RooArgSet.h"
#include "RooStats/ModelConfig.h"
#include "TIterator.h"
#include "TString.h"
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace RooStats;

struct NPInfo {
    std::string name;
    std::string constraintPdf;
    std::string constraintType;
    std::string params;
};

void GetConstraintInfo(RooRealVar* np, NPInfo& info) {
    info.name = np->GetName();
    info.constraintPdf = "-";
    info.constraintType = "Flat Param (No Constraint)";
    info.params = "-";

    TIterator* clientIter = np->clientIterator();
    RooAbsArg* client;
    
    while ((client = (RooAbsArg*)clientIter->Next())) {
        if (!client->InheritsFrom("RooAbsPdf")) continue;
        
        TString cname = client->ClassName();
        
        if (cname.Contains("RooRealSumPdf") || 
            cname.Contains("CMSHistErrorPropagator") ||
            cname.Contains("RooAddPdf") ||
            cname.Contains("RooProdPdf") ||
            cname.Contains("RooSimultaneous")) {
            continue;
        }

        info.constraintPdf = client->GetName();
        info.constraintType = cname.Data();

        std::string pStr = "";
        RooAbsPdf* pdf = (RooAbsPdf*)client;
        RooArgSet* params = pdf->getParameters((RooAbsData*)0);
        TIterator* pIter = params->createIterator();
        RooAbsArg* param;
        
        double poissonVal = -1.0;

        while((param = (RooAbsArg*)pIter->Next())) {
            if (TString(param->GetName()) == TString(np->GetName())) continue;

            double val = 0.0;
            if (param->InheritsFrom("RooAbsReal")) val = ((RooAbsReal*)param)->getVal();

            if (cname == "RooPoisson") poissonVal = val;

            char buf[64];
            snprintf(buf, 64, "%s=%.2f", param->GetName(), val);
            if (pStr.length() > 0) pStr += ", ";
            pStr += buf;
        }
        
        if (cname == "SimpleGaussianConstraint") {
            if (pStr.length() > 0) pStr += ", ";
            pStr += "(Implicit Sigma=1.0)";
        }
        
        if (cname == "RooPoisson" && poissonVal >= 0) {
            if (pStr.length() > 0) pStr += " ";
            char buf[64];
            snprintf(buf, 64, "(Poisson Error=%.2f)", sqrt(poissonVal));
            pStr += buf;
        }
        
        info.params = pStr;
        break;
    }
}

void CheckAllNPs(const char* filename, const char* wsName = "w") {
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    RooWorkspace *w = (RooWorkspace*)f->Get(wsName);
    if (!w) w = (RooWorkspace*)f->Get("combWS");
    if (!w) {
        printf("Error: Workspace '%s' not found.\n", wsName);
        return;
    }

    RooArgSet npSet;
    ModelConfig* mc = (ModelConfig*)w->genobj("ModelConfig");
    
    if (mc && mc->GetNuisanceParameters()) {
        printf(">>> Loading Nuisance Parameters from ModelConfig...\n");
        npSet.add(*mc->GetNuisanceParameters());
    } else {
        printf(">>> [Warning] ModelConfig not found. Scanning ALL non-constant variables...\n");
        RooArgSet allVars = w->allVars();
        TIterator* vIter = allVars.createIterator();
        RooRealVar* v;
        while((v = (RooRealVar*)vIter->Next())) {
            if (!v->isConstant() && TString(v->GetName()) != "r") {
                npSet.add(*v);
            }
        }
    }

    printf("\n%-40s | %-25s | %s\n", "Parameter Name", "Constraint Class", "Constraint Params");
    printf("------------------------------------------------------------------------------------------------------------------\n");

    std::vector<NPInfo> results;
    TIterator* iter = npSet.createIterator();
    RooRealVar* np;

    while((np = (RooRealVar*)iter->Next())) {
        NPInfo info;
        GetConstraintInfo(np, info);
        results.push_back(info);
    }

    std::sort(results.begin(), results.end(), [](const NPInfo& a, const NPInfo& b) {
        return a.name < b.name;
    });

    for (const auto& r : results) {
        printf("%-40s | %-25s | %s\n", r.name.c_str(), r.constraintType.c_str(), r.params.c_str());
    }
    printf("------------------------------------------------------------------------------------------------------------------\n");

    f->Close();
}
