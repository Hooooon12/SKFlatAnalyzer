#include "HNL_Lepton_WG_Overlap_Studies.h"

void HNL_Lepton_WG_Overlap_Studies::initializeAnalyzer(){

  HNL_LeptonCore::initializeAnalyzer();

}

void HNL_Lepton_WG_Overlap_Studies::ProcessLeptonCategory(const std::string& s_labelPrefix,
                std::vector<Lepton*>& leptons,
                std::vector<Tau>& TauColl_Uncleaned,
                std::vector<Jet>& AK4_JetColl,
                std::vector<FatJet>& AK8_JetColl,
                Particle& METv,
                int nPV,
                AnalyzerParameter& param,
                double PhotonPt,
                double weight) {

  TString labelPrefix = TString(s_labelPrefix);
  
  if (leptons.size() < 2) return;
  if (leptons[1]->Pt() < 15) return;
  if (leptons[0]->Pt() < 25) return;

  if (leptons.size() == 2)
    Fill_RegionPlots(param, labelPrefix + "/Conv2L", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);

  if (leptons.size() == 3)
    Fill_RegionPlots(param, labelPrefix + "/Conv3L", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);

  if (leptons.size() == 2) {
    if (SameCharge(leptons)) {
      Fill_RegionPlots(param, labelPrefix + "/ConvSS", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);
      FillHist(labelPrefix + "/SS_Photon_Status23", PhotonPt, weight, 500, 0, 1000);
      //      FillHist(labelPrefix + "/SS_Photon_Status23_Mass", PhotonPt, weight, 500, 0, 1000);
      
    } else {
      FillHist(labelPrefix + "/OS_Photon_Status23", PhotonPt, weight, 500, 0, 1000);
      Fill_RegionPlots(param, labelPrefix + "/ConvOS", TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, leptons, METv, nPV, weight);
    }
  }
}

void HNL_Lepton_WG_Overlap_Studies::executeEvent(){


 
  Event ev = GetEvent();

  AnalyzerParameter param = HNL_LeptonCore::InitialiseHNLParameter("HNL_ULIDv2");
  double weight =SetupWeight(ev,param);

  std::vector<Electron>   ElectronCollV = GetElectrons(param.Electron_Veto_ID, 15., 2.5);
  std::vector<Muon>       MuonCollV     = GetMuons    (param.Muon_Veto_ID, 10., 2.4);

  std::vector<Electron>   ElectronCollT = GetElectrons(param.Electron_Tight_ID, 15., 2.5);
  std::vector<Muon>       MuonCollT     = GetMuons    (param.Muon_Tight_ID, 10., 2.4);
  
  TString PlotDir="Inclusive";

  std::vector<Lepton *> Veto_Leptons  = MakeLeptonPointerVector(MuonCollV,ElectronCollV,param);
  std::vector<Lepton *> Tight_Leptons   = MakeLeptonPointerVector(MuonCollT,ElectronCollT,param);
  std::vector<FatJet> AK8_JetColl                 = GetHNLAK8Jets(param.AK8JetColl,param);
  std::vector<Jet>    AK4_JetColl                 = GetHNLJets(param.AK4JetColl,     param);
  std::vector<Jet>    AK4_VBF_JetColl             = GetHNLJets(param.AK4VBFJetColl,  param);
  std::vector<Jet>    AK4_JetAllColl              = GetHNLJets("NoCut_Eta3",param);
  std::vector<Jet>    AK4_JetCollLoose            = GetHNLJets("Loose",     param);
  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet", param);

  std::vector<Tau>   TauColl_Uncleaned  = SelectTaus   (Veto_Leptons,"JetT_MuT_ELT",20., 2.3);

  Particle METv = GetvMET("PuppiT1xyULCorr", param, MuonCollT, ElectronCollT); // returns MET with systematic correction; run this afte 
  //ProcessLeptonCategory("VetoID", Veto_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV,  param, PhotonPt, weight);
  //ProcessLeptonCategory("TightID", Tight_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV, param, PhotonPt, weight);
  //if(Veto_Leptons.size()==2) ProcessLeptonCategory("TightID_Cleaned", Tight_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV, param, PhotonPt, weight);
  //if(Veto_Leptons.size()==2) ProcessLeptonCategory("TightID_Cleaned_PhStatus1", Tight_Leptons, TauColl_Uncleaned, AK4_JetColl, AK8_JetColl, METv, nPV, param, PhotonPt_with_Status1, weight);
  
  vector<Particle>  conv_gen_el,conv_gen_mu, daught_mu, daught_el;
  for(int igen = 0; igen < All_Gens.size(); igen++) {
    const Gen& gen = All_Gens[igen];
  
    if(fabs(gen.PID()) != 13 || gen.Status() != 23) continue;
  
    int midx = gen.MotherIndex();
    if(midx < 0 || midx >= All_Gens.size()) continue;
  
    if(fabs(All_Gens[midx].PID()) == 22) {
      conv_gen_mu.push_back(gen);
    }

  }
  if(conv_gen_mu.size()!=0){
    cout << "=====status 23 muon has a photon mother?!=====" << endl;
    PrintGen(All_Gens);
    cout << "==============================================" << endl;
  }
  
  FillHist( ("EventCount"), 0, 1., 10, 0, 10); // total Nevent

  if(_jentry < 100) PrintGen(All_Gens);

  map<int, vector<Gen>> leptons_by_mother;
  
  for (int i = 0; i < All_Gens.size(); i++) {
    const Gen& gen = All_Gens[i];
  
    int pid = abs(gen.PID());
    if (pid != 11 && pid != 13) continue;
  
    int midx = gen.MotherIndex();
    if (midx < 0 || midx >= All_Gens.size()) continue;
  
    if (All_Gens[midx].Status() != 23) continue;

    leptons_by_mother[midx].push_back(gen);
  }
  
  bool has_overlap = false;

  for (auto& kv : leptons_by_mother) {
  
    const vector<Gen>& leptons = kv.second;
  
    if (leptons.size() < 2) continue;

    if(leptons.size()>2){
      cout << "=====more than 2 leptons from the same status 23 mother?!=====" << endl;
      PrintGen(All_Gens);
      cout << "==============================================================" << endl;
    }

    if(leptons.size()==2&&(abs(leptons[0].PID())!=abs(leptons[1].PID()))){
      cout << "=====2 leptons from the same status 23 mother have different flavor?!=====" << endl;
      PrintGen(All_Gens);
      cout << "==========================================================================" << endl;
    }

    if(leptons.size()==2&&(leptons[0].PID()*leptons[1].PID() >= 0)){
      cout << "=====2 leptons from the same status 23 mother have same sign?!=====" << endl;
      PrintGen(All_Gens);
      cout << "===================================================================" << endl;
    }

    const Gen& mother = All_Gens[kv.first];
    int mother_pid = abs(mother.PID());
 
    if(leptons.size()==2){
      const Gen& l1 = leptons[0];
      const Gen& l2 = leptons[1];
  
      if (l1.PID() * l2.PID() >= 0) continue; // opposite sign
      if (abs(l1.PID())!=abs(l2.PID())) continue; // same flavor

      TLorentzVector v1, v2;
      v1.SetPtEtaPhiM(l1.Pt(), l1.Eta(), l1.Phi(), l1.M());
      v2.SetPtEtaPhiM(l2.Pt(), l2.Eta(), l2.Phi(), l2.M());
  
      double mll = (v1 + v2).M();

      if((MCSample.Contains("WG")&&mll>=4.)||(MCSample.Contains("WZ")&&mll<4.)) has_overlap = true;

      FillHist( ("DiLepton_SameMother/MotherPID"), mother_pid,weight, 100, 0, 100);

      if (mother_pid == 23) {
        FillHist( ("DiLepton_SameMother/Z/mass_ll"), mll,weight, 200, 0, 200);
      }
      else if (mother_pid == 22) {
        FillHist( ("DiLepton_SameMother/photon/mass_ll"), mll,weight, 200, 0, 200);
      }
      else if (mother_pid == 11) {
        FillHist( ("DiLepton_SameMother/electron/mass_ll"), mll,weight, 200, 0, 200);
      }
      else if (mother_pid == 13) {
        FillHist( ("DiLepton_SameMother/muon/mass_ll"), mll,weight, 200, 0, 200);
      }
      else if (mother_pid == 15) {
        FillHist( ("DiLepton_SameMother/tau/mass_ll"), mll,weight, 200, 0, 200);
      }
      else {
        FillHist( ("DiLepton_SameMother/else/mass_ll"), mll,weight, 200, 0, 200);
      }
    }
    else{
      for (int i = 0; i < leptons.size(); i++) {
        for (int j = i + 1; j < leptons.size(); j++) {
  
          const Gen& l1 = leptons[i];
          const Gen& l2 = leptons[j];
  
          if (l1.PID() * l2.PID() >= 0) continue; // opposite sign
          if (abs(l1.PID())!=abs(l2.PID())) continue; // same flavor

          TLorentzVector v1, v2;
          v1.SetPtEtaPhiM(l1.Pt(), l1.Eta(), l1.Phi(), l1.M());
          v2.SetPtEtaPhiM(l2.Pt(), l2.Eta(), l2.Phi(), l2.M());
  
          double mll = (v1 + v2).M();

          if((MCSample.Contains("WG")&&mll>=4.)||(MCSample.Contains("WZ")&&mll<4.)) has_overlap = true;

          FillHist( ("MultiLepton_SameMother/MotherPID"), mother_pid,weight, 100, 0, 100);

          if (mother_pid == 23) {
            FillHist( ("MultiLepton_SameMother/Z/mass_ll"), mll,weight, 200, 0, 200);
          }
          else if (mother_pid == 22) {
            FillHist( ("MultiLepton_SameMother/photon/mass_ll"), mll,weight, 200, 0, 200);
          }
          else if (mother_pid == 11) {
            FillHist( ("MultiLepton_SameMother/electron/mass_ll"), mll,weight, 200, 0, 200);
          }
          else if (mother_pid == 13) {
            FillHist( ("MultiLepton_SameMother/muon/mass_ll"), mll,weight, 200, 0, 200);
          }
          else if (mother_pid == 15) {
            FillHist( ("MultiLepton_SameMother/tau/mass_ll"), mll,weight, 200, 0, 200);
          }
          else {
            FillHist( ("MultiLepton_SameMother/else/mass_ll"), mll,weight, 200, 0, 200);
          }
        }
      }
    }
  }
  
  if(has_overlap) FillHist( ("EventCount"), 1, 1., 10, 0, 10); // possibly overlap Nevent

  return;
  
}




HNL_Lepton_WG_Overlap_Studies::HNL_Lepton_WG_Overlap_Studies(){


}
 
HNL_Lepton_WG_Overlap_Studies::~HNL_Lepton_WG_Overlap_Studies(){

}
