#include "HNL_LeptonCore.h"


bool HNL_LeptonCore::PassTandP_PairCriteria(vector<Muon> muons, int itag, int iprobe){

  
  if(muons[itag].Charge() == muons[iprobe].Charge()) return false;

  Particle MLL = muons[itag]+muons[iprobe];

  if(MLL.M() < 120) return false;

  if(muons[itag].DeltaR(muons[iprobe]) < 0.4) return false;

  if((muons[itag].Pt() / muons[iprobe].Pt()) > 3.) return false;

  if((PVchi2/PVndof) > 20) return false;

  return true;

}

