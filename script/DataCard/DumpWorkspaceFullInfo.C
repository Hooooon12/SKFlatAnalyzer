// DumpWorkspaceFullInfo.C
// Usage: root -l -q 'DumpWorkspaceFullInfo.C("Run2_EE_M95_syst.root")' > full_dump.txt
//
// Features:
// 1. Data Unrolling: Map Bin Index to physical Channel/Year and Observable value.
// 2. Channel Inspection: Check PDF structure and Expected Yield for each channel.
// 3. Systematics Detail: List all Nuisance Parameters and their Constraint settings.

#include "TFile.h"
#include "RooWorkspace.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooCategory.h"
#include "RooAbsPdf.h"
#include "RooStats/ModelConfig.h"
#include "TIterator.h"
#include "TString.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <map>
#include <cmath>
#include <string>

using namespace std;

void DumpWorkspaceFullInfo(const char* filename) {
    // 1. Open File
    TFile *f = TFile::Open(filename);
    if (!f || f->IsZombie()) {
        cerr << "Error: Cannot open file " << filename << endl;
        return;
    }

    // 2. Get Workspace
    RooWorkspace *w = (RooWorkspace*)f->Get("w");
    if(!w) w = (RooWorkspace*)f->Get("combWS");

    if (!w) {
        cerr << "Error: Workspace not found." << endl;
        return;
    }

    // Header
    cout << "================================================================================" << endl;
    cout << "   FULL WORKSPACE DUMP : " << filename << endl;
    cout << "================================================================================" << endl << endl;

    // -------------------------------------------------------------------------
    // PART 1. DATA UNROLLING (Bin Map)
    // -------------------------------------------------------------------------
    cout << ">>> [PART 1] DATA MAPPING (Bin Index -> Physical Region) <<<" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << setw(6) << "BinIdx" << " | " 
         << setw(30) << "Channel (Region_Year)" << " | " 
         << setw(15) << "Observable (X)" << " | " 
         << setw(10) << "Events" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;

    RooDataSet* data = (RooDataSet*)w->data("data_obs");
    if (data) {
        // Iterate through all bins (entries) in the dataset
        for (int i = 0; i < data->numEntries(); i++) {
            const RooArgSet* args = data->get(i);
            double weight = data->weight(); // Actual observed event count in this bin

            // A. Get Channel Name (from 'CMS_channel' category)
            RooCategory* cat = (RooCategory*)args->find("CMS_channel");
            string channelName = (cat) ? cat->getLabel() : "Unknown";

            // B. Get Observable Value (Usually 'CMS_th1x' or similar)
            // We search for a RooRealVar that is NOT the weight
            double xVal = -999;
            TIterator* iter = args->createIterator();
            RooAbsArg* arg;
            while((arg = (RooAbsArg*)iter->Next())) {
                if (arg->IsA() == RooRealVar::Class() && TString(arg->GetName()) != "weight") {
                     xVal = ((RooRealVar*)arg)->getVal();
                     break; 
                }
            }

            cout << setw(6) << i << " | " 
                 << setw(30) << channelName << " | " 
                 << setw(15) << fixed << setprecision(2) << xVal << " | " 
                 << setw(10) << weight << endl;
        }
    } else {
        cout << "Error: 'data_obs' dataset not found in workspace!" << endl;
    }
    cout << "--------------------------------------------------------------------------------" << endl << endl;


    // -------------------------------------------------------------------------
    // PART 2. CHANNEL (CATEGORY) INSPECTION
    // -------------------------------------------------------------------------
    cout << ">>> [PART 2] CHANNEL STRUCTURE & EXPECTATIONS <<<" << endl;
    
    RooCategory* cat = w->cat("CMS_channel");
    if (cat) {
        TIterator* typeIter = cat->typeIterator();
        RooCatType* type;
        
        while ((type = (RooCatType*)typeIter->Next())) {
            string chName = type->GetName();
            int chID = type->getVal();

            cout << "--------------------------------------------------------" << endl;
            cout << " Channel [" << chID << "] : " << chName << endl;
            
            // Expected PDF Name Convention: pdf_bin[ChannelName]
            TString pdfName = TString::Format("pdf_bin%s", chName.c_str());
            RooAbsPdf* pdf = w->pdf(pdfName);

            if (pdf) {
                cout << "  - Main PDF: " << pdf->GetName() << " (" << pdf->ClassName() << ")" << endl;
                
                // Try to find the expected yield (Normalization term)
                // Convention: n_exp_bin[ChannelName]
                TString normName = TString::Format("n_exp_bin%s", chName.c_str());
                RooAbsReal* norm = (RooAbsReal*)w->obj(normName); 
                
                if (norm) {
                     cout << "  - Total Expected Yield: " << norm->getVal() << endl;
                } else {
                     cout << "  - Total Expected Yield: (Not found or not pre-calculated)" << endl;
                }
            } else {
                cout << "  - Main PDF not found (tried name: '" << pdfName << "')" << endl;
            }
        }
    } else {
        cout << "Warning: 'CMS_channel' category not found." << endl;
    }
    cout << endl;

    // -------------------------------------------------------------------------
    // PART 3. NUISANCE PARAMETERS & CONSTRAINTS (Detailed)
    // -------------------------------------------------------------------------
    cout << ">>> [PART 3] NUISANCE PARAMETERS & CONSTRAINTS <<<" << endl;
    cout << "----------------------------------------------------------------------------------------------------" << endl;
    cout << setw(40) << left << "Parameter Name" << " | " 
         << setw(25) << "Constraint Type" << " | " 
         << "Settings (Prior)" << endl;
    cout << "----------------------------------------------------------------------------------------------------" << endl;

    // 1. Collect all Nuisance Parameters
    RooArgSet npSet;
    RooStats::ModelConfig* mc = (RooStats::ModelConfig*)w->genobj("ModelConfig");
    
    if (mc && mc->GetNuisanceParameters()) {
        npSet.add(*mc->GetNuisanceParameters());
    } else {
        // Fallback: Scan all non-constant variables if ModelConfig is missing
        RooArgSet vars = w->allVars();
        TIterator* iter = vars.createIterator();
        RooRealVar* v;
        while((v=(RooRealVar*)iter->Next())) {
            if(!v->isConstant()) npSet.add(*v);
        }
    }

    // 2. Sort parameters alphabetically for better readability
    vector<string> sortedNames;
    TIterator* nIter = npSet.createIterator();
    RooRealVar* np;
    while((np=(RooRealVar*)nIter->Next())) {
        sortedNames.push_back(np->GetName());
    }
    std::sort(sortedNames.begin(), sortedNames.end());

    // 3. Loop and Analyze
    for (const auto& name : sortedNames) {
        RooRealVar* v = w->var(name.c_str());
        if(!v) continue;

        string cType = "Flat (Unconstrained)";
        string cParams = "-";

        // Search for the PDF that constrains this variable
        TIterator* clientIter = v->clientIterator();
        RooAbsArg* client;
        while((client=(RooAbsArg*)clientIter->Next())) {
             if(!client->InheritsFrom("RooAbsPdf")) continue;
             TString cname = client->ClassName();
             
             // SKIP: Physics models (Templates, Morphing, etc.)
             if(cname.Contains("SumPdf") || cname.Contains("Propagator") || 
                cname.Contains("Product") || cname.Contains("Simultaneous") || 
                cname.Contains("AddPdf")) continue;

             // FOUND: Likely a constraint PDF
             cType = cname.Data();
             
             // Extract Parameters of the Constraint
             RooAbsPdf* pdf = (RooAbsPdf*)client;
             RooArgSet* params = pdf->getParameters((RooAbsData*)0);
             TIterator* pIter = params->createIterator();
             RooAbsArg* p;
             cParams = "";
             
             while((p=(RooAbsArg*)pIter->Next())) {
                 // Skip the NP itself
                 if(TString(p->GetName()) == name) continue;
                 
                 double val = 0;
                 if(p->InheritsFrom("RooAbsReal")) val = ((RooAbsReal*)p)->getVal();
                 
                 // Special formatting for Poisson: Error = sqrt(N)
                 if(cname == "RooPoisson") {
                     cParams += "[N=" + to_string((int)val) + ", Err=" + to_string(sqrt(val)).substr(0,4) + "] ";
                 } else {
                     cParams += string(p->GetName()) + "=" + to_string(val).substr(0,5) + " ";
                 }
             }
             
             // Special formatting for SimpleGaussianConstraint (Hidden Sigma)
             if(cname == "SimpleGaussianConstraint") cParams += "(Implicit Sigma=1.0) ";
             
             break; // Stop after finding the first constraint
        }
        
        cout << setw(40) << left << name << " | " 
             << setw(25) << cType << " | " 
             << cParams << endl;
    }

    f->Close();
}
