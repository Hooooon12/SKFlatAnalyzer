#include "GetEffLumi.h"

void GetEffLumi::initializeAnalyzer(){
  fChain->SetBranchStatus("*",0);
  fChain->SetBranchStatus("IsData",1);
  if(!IsDATA){
    fChain->SetBranchStatus("gen_weight",1);
    if(fChain->GetBranch("weight_Scale")) fChain->SetBranchStatus("weight_Scale",1);
    if(fChain->GetBranch("weight_AlphaS"))   fChain->SetBranchStatus("weight_AlphaS",1);
    if(fChain->GetBranch("weight_PDF"))      fChain->SetBranchStatus("weight_PDF",1);
//    if(fChain->GetBranch("weight_alpsfact")) fChain->SetBranchStatus("weight_alpsfact",1);
    if(fChain->GetBranch("weight_PSSyst"))   fChain->SetBranchStatus("weight_PSSyst",1);
  }
}

void GetEffLumi::executeEvent(){
  double weight = 1.;
  double weight_sign = 1.;
  if(!IsDATA){
    weight = MCweight(false,false);
    weight_sign = MCweight(true,false);
  }

  FillHist("sumEvents", 0, 1., 1, 0., 1.);
  FillHist("sumW", 0, weight, 1, 0., 1.);
  FillHist("sumSign", 0, weight_sign, 1, 0., 1.);
  if(!IsDATA && weight_Scale->size()>0){
    for(int i=0,n=weight_Scale->size();i<n;i++){
      FillHist("sumW_Reweight", i, weight*weight_Scale->at(i), n, 0, n);
    }
    for(int i=0,n=weight_Scale   ->size(); i<n; i++){ FillHist("sumW_Scale",    i, weight*weight_Scale   ->at(i), n, 0, n); }
    for(int i=0,n=weight_AlphaS  ->size(); i<n; i++){ FillHist("sumW_AlphaS",   i, weight*weight_AlphaS  ->at(i), n, 0, n); }
    for(int i=0,n=weight_PDF     ->size(); i<n; i++){ FillHist("sumW_PDF",      i, weight*weight_PDF     ->at(i), n, 0, n); }
//    for(int i=0,n=weight_alpsfact->size(); i<n; i++){ FillHist("sumW_alpsfact", i, weight*weight_alpsfact->at(i), n, 0, n); }
    for(int i=0,n=weight_PSSyst  ->size(); i<n; i++){ FillHist("sumW_PSSyst",   i, weight*weight_PSSyst  ->at(i), n, 0, n); }
  }

}
