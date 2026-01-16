// InspectWorkspace.C
// Usage: root -l -q 'InspectWorkspace.C("/data6/Users/jihkim/LatestCombine/CMSSW_14_1_0_pre4/src/DilepHN/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_AltBin_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr_NewRP/Run2_EE_M95_syst/Run2_EE_M95_syst.root")'

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooAbsData.h"
#include "RooCategory.h"
#include "RooAbsCategory.h"
#include "TIterator.h"
#include "TString.h"
#include <list> 

void InspectWorkspace(const char* filename) {
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        printf("Error: Cannot open file %s\n", filename);
        return;
    }

    RooWorkspace *w = (RooWorkspace*)f->Get("w");
    if(!w) w = (RooWorkspace*)f->Get("combWS");

    if (!w) {
        printf("Error: Workspace not found.\n");
        return;
    }

    printf("\n==============================================================\n");
    printf(" 🕵️  Workspace Inspector : %s\n", filename);
    printf("==============================================================\n");

    // 1. Check Datasets
    // Note: In modern ROOT, allData() returns std::list<RooAbsData*>, so we use a C++ range-based loop.
    printf("\n[1] Datasets (Observed Data & Toys)\n");
    auto dataList = w->allData(); 
    for (auto data : dataList) {
        if (!data) continue;
        printf("  - Name: %-25s | Type: %s | Entries: %d\n", 
               data->GetName(), data->ClassName(), (int)data->numEntries());
    }

    // 2. Check Snapshots via ModelConfig
    printf("\n[2] Snapshots (Saved States)\n");
    RooStats::ModelConfig* mc = (RooStats::ModelConfig*)w->genobj("ModelConfig");
    if(mc) {
        printf("  Found 'ModelConfig'. Defined sets:\n");
        const RooArgSet* poi = mc->GetParametersOfInterest();
        const RooArgSet* nps = mc->GetNuisanceParameters();
        const RooArgSet* glo = mc->GetGlobalObservables();

        if(poi) printf("  - POI: %d vars\n", poi->getSize());
        if(nps) printf("  - NPs: %d vars\n", nps->getSize());
        if(glo) printf("  - Global Obs: %d vars\n", glo->getSize());
    } else {
        printf("  (ModelConfig object not found)\n");
    }

    // 3. Check Categories (Channels)
    printf("\n[3] Categories (Discrete Variables / Channels)\n");
    RooArgSet cats = w->allCats();
    TIterator* cIter = cats.createIterator();
    RooAbsCategory* cat;
    while((cat = (RooAbsCategory*)cIter->Next())) {
        printf("  - %s (Index categories defining channels)\n", cat->GetName());
    }

    // 4. Check Functions (Sample)
    printf("\n[4] Special Functions (Logic)\n");
    RooArgSet funcs = w->allFunctions();
    TIterator* fIter = funcs.createIterator();
    RooAbsArg* func;
    int fCount = 0;
    while((func = (RooAbsArg*)fIter->Next())) {
        TString cname = func->ClassName();
        // Only print RooFormulaVar as an example, since there are too many functions
        if(cname.Contains("RooFormulaVar") && fCount < 5) {
             printf("  - %-30s [Formula] : %s\n", func->GetName(), func->GetTitle());
             fCount++;
        }
    }
    if(fCount >= 5) printf("  ... (and many more formulas)\n");

    printf("\n==============================================================\n");
    f->Close();
}
