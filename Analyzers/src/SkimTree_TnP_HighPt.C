#include "SkimTree_TnP_HighPt.h"

void SkimTree_TnP_HighPt::initializeAnalyzer(){

  AnalyzerCore::initializeAnalyzer();

  outfile->cd();
  cout << "[SkimTree_TnP_HighPt::initializeAnalyzer()] gDirectory = " << gDirectory->GetName() << endl;
  newtree = fChain->CloneTree(0);

  single_electron_triggers.clear();
  single_muon_triggers.clear();
  if(DataYear==2016){
    single_muon_triggers = {
      "HLT_IsoMu24_v",
      "HLT_IsoTkMu24_v",
      "HLT_Mu50_v"
    };
    single_electron_triggers = {
      "HLT_Ele27_eta2p1_WPTight_Gsf_v",
    };
  }else if(DataYear==2017){
    single_muon_triggers = {
      "HLT_IsoMu24_v",
      "HLT_IsoMu27_v",
      "HLT_Mu50_v"

    };
    single_electron_triggers = {
      "HLT_Ele32_WPTight_Gsf_v",
      "HLT_Ele32_WPTight_Gsf_L1DoubleEG_v",
    };
  }else if(DataYear==2018){
    single_muon_triggers = {
      "HLT_IsoMu24_v",
      "HLT_Mu50_v"
    };
    single_electron_triggers = {
      "HLT_Ele32_WPTight_Gsf_v",
    };
  }else{
    cout<<"[SkimTree_TnP_HighPt::initializeAnalyzer] DataYear is wrong : " << DataYear << endl;
  }

  cout << "[SkimTree_TnP_HighPt::initializeAnalyzer] triggers to skim = " << endl;

  for(unsigned int i=0; i<single_muon_triggers.size(); i++){
    cout << "[SkimTree_TnP_HighPt::initializeAnalyzer]   " << single_muon_triggers.at(i) << endl;
  }
  for(unsigned int i=0; i<single_electron_triggers.size(); i++){
    cout << "[SkimTree_TnP_HighPt::initializeAnalyzer]   " << single_electron_triggers.at(i) << endl;
  }

}

void SkimTree_TnP_HighPt::executeEvent(){


  Event ev;
  ev.SetTrigger(*HLT_TriggerName);

  vector<Muon> muons=GetAllMuons();
  std::sort(muons.begin(),muons.end(),PtComparing);

  vector<Electron> electrons=GetAllElectrons();
  std::sort(electrons.begin(),electrons.end(),PtComparing);

  int nmuon=muons.size();
  int nelectron=electrons.size();
  
  if( ev.PassTrigger(single_muon_triggers) ){
    
    for(int i=0;i<nmuon;i++){
      for(int j=i+1;j<nmuon;j++){
	
	if(!IsData){
	  if(!muons.at(i).IsPrompt()) continue;
	  if(!muons.at(j).IsPrompt()) continue;
	}
	if(muons.at(i).Pt()<50) continue;
	if(muons.at(j).Pt()<50) continue;
        if(!( muons.at(i).Pt()>120 || muons.at(j).Pt() >120)) continue;

	if((muons.at(i)+muons.at(j)).M()<120) continue;
	if(newtree->Fill()<0) exit(EIO);
	return;
      }
    }
  }
  if( ev.PassTrigger(single_electron_triggers) ){

    for(int i=0;i<nelectron;i++){
      for(int j=i+1;j<nelectron;j++){
	if(electrons.at(i).Pt()<35) continue;
	if(electrons.at(j).Pt()<35) continue;
	if(!IsData){
          if(!electrons.at(i).IsPrompt()) continue;
          if(!electrons.at(j).IsPrompt()) continue;
	}
	if((electrons.at(i)+electrons.at(j)).M()<70) continue;
	if((electrons.at(i)+electrons.at(j)).M()>110) continue;
	if( !(electrons.at(i).Pt() > 120 || electrons.at(j).Pt() > 120)) continue;
	if(newtree->Fill()<0) exit(EIO);
	return;
      }
    }
  }
  return;
}

void SkimTree_TnP_HighPt::executeEventFromParameter(AnalyzerParameter param){

}

SkimTree_TnP_HighPt::SkimTree_TnP_HighPt(){
  newtree=NULL;
}

SkimTree_TnP_HighPt::~SkimTree_TnP_HighPt(){

}

void SkimTree_TnP_HighPt::WriteHist(){

  outfile->mkdir("recoTree");
  outfile->cd("recoTree");
  newtree->Write();
  outfile->cd();

}
