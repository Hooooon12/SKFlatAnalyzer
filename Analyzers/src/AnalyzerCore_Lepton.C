#include "AnalyzerCore.h"


std::vector<Muon> AnalyzerCore::GetMuons(TString id, double ptmin, double fetamax){

  std::vector<Muon> muons =  All_Muons;
  std::vector<Muon> out;
  for(unsigned int i=0; i<muons.size(); i++){
    if(!( muons.at(i).Pt()>ptmin ))           continue;
    if(!( fabs(muons.at(i).Eta())<fetamax ))  continue;
    if(!( muons.at(i).PassID(id) ))           continue;
    out.push_back( muons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);
  return out;
}

std::vector<Electron> AnalyzerCore::GetElectrons(TString id, double ptmin, double fetamax, bool vetoHEM){

  std::vector<Electron> electrons = All_Electrons;

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    if(!( electrons.at(i).Pt()>ptmin ))             continue;
    if(!( fabs(electrons.at(i).scEta())<fetamax ))  continue;
    if(!( electrons.at(i).PassID(id) ))             continue;
    if(vetoHEM){
      if ( FindHEMElectron (electrons.at(i)) ){
        continue;
      }
    }
    out.push_back( electrons.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);
  return out;
}

std::vector<Tau> AnalyzerCore::GetTaus(TString id, double ptmin, double fetamax){

  std::vector<Tau> taus = GetAllTaus();
  std::vector<Tau> out;

  for(unsigned int i=0; i<taus.size(); i++){
    if(!( taus.at(i).Pt()>ptmin ))          continue;
    if(!( fabs(taus.at(i).Eta())<fetamax )) continue;
    if(!( taus.at(i).PassID(id) ))          continue;
    out.push_back( taus.at(i) );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}



std::vector<Electron> AnalyzerCore::ScaleElectrons(const std::vector<Electron>& electrons, int sys){

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    //==== electrons is a const vector. So in this function, we have to copy the elements like below                                                                                                                                                                                                                                                                                                                                     
    Electron this_electron = electrons.at(i);

    double this_sf = this_electron.EnShift(sys);
    this_electron.SetPtEtaPhiM( this_electron.Pt() * this_sf, this_electron.Eta(), this_electron.Phi(), this_electron.M() );

    out.push_back( this_electron );
  }
  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}
std::vector<Electron> AnalyzerCore::SmearElectrons(const std::vector<Electron>& electrons, int sys){

  std::vector<Electron> out;
  for(unsigned int i=0; i<electrons.size(); i++){
    //==== electrons is a const vector. So in this function, we have to copy the elements like below                                                                                                                                                                                                                                                                                                                                     
    Electron this_electron = electrons.at(i);

    double this_sf = this_electron.ResShift(sys);
    this_electron.SetPtEtaPhiM( this_electron.Pt() * this_sf, this_electron.Eta(), this_electron.Phi(), this_electron.M() );

    out.push_back( this_electron );
  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}



std::vector<Muon> AnalyzerCore::ScaleMuons(const std::vector<Muon>& muons, int sys){

  std::vector<Muon> out;
  for(unsigned int i=0; i<muons.size(); i++){
    //==== muons is a const vector. So in this function, we have to copy the elements like below                                                                                                                                                                                                                                                                                                                                         
    Muon this_muon = muons.at(i);

    //==== Even for TuneP muons, MomentumShift() are set correctly from AnalyzerCore::UseTunePMuon()                                                                                                                                                                                                                                                                                                                                     
    //==== So we can just use MomentumShift()                                                                                                                                                                                                                                                                                                                                                                                            

    this_muon.SetPtEtaPhiM( this_muon.MomentumShift(sys), this_muon.Eta(), this_muon.Phi(), this_muon.M() );

    out.push_back(this_muon);

  }

  std::sort(out.begin(),       out.end(),        PtComparing);

  return out;

}


double AnalyzerCore::GetSmearFactor(double p, double eta){


  if(DataEra == "2016preVFP"){
    if(fabs(eta) < 1.2) return 0.0112 + 6.87e-5*p   -  3.88e-8* pow(p,2)   +  9.03e-12 * pow(p,3);
    else                return 0.013  + 6.93e-5*p   -  3.46e-8* pow(p,2)   +  7.72e-12 * pow(p,3);
  }
  if(DataEra == "2016postVFP"){
    if(fabs(eta) < 1.2) return 0.0102 + 6.77e-5*p  -  3.72e-8* pow(p,2)   +  8.53e-12 * pow(p,3);
    else                return 0.0129 + 6.48e-5*p  -  3.04e-8* pow(p,2)   +  6.63e-12 * pow(p,3);
  }

  if(DataYear == 2017){
    if(fabs(eta) < 1.2) return 0.0104 + 6.11e-5*p  -  3.31e-8* pow(p,2)   +  6.73e-12 * pow(p,3);
    else                return 0.0121 + 5.92e-5*p  -  2.61e-8* pow(p,2)   +  5.11e-12 * pow(p,3);
  }
  if(DataYear == 2018){
    if(fabs(eta) < 1.2) return 0.0108 + 5.93e-5*p  -  3.08e-8* pow(p,2)   +  6.04e-12 * pow(p,3);
    else                return 0.0136 + 5.47e-5*p  -  2.30e-8* pow(p,2)   +  4.66e-12 * pow(p,3);
  }

  return -1;
}



void  AnalyzerCore::SmearHighPtMuon(Muon& this_muon){

  if(DataYear == 2016)  return ;
  if(this_muon.MiniAODPt() < 200) return ;
  
  if(fabs(this_muon.Eta()) < 1.2) return;
  else{
    
    double MiniAODP = sqrt( this_muon.MiniAODPt() * this_muon.MiniAODPt() + this_muon.Pz() * this_muon.Pz() );
    
    ///                                                                                                                                                                                           
    double coeff_smear = 0.3202;
    if(DataYear == 2018) coeff_smear=0.46;
    double sigmaSmear = GetSmearFactor(MiniAODP, fabs(this_muon.Eta())) * coeff_smear;
    
    /// smear pt				    
    double new_pt =  this_muon.Pt() * (1+gRandom->Gaus(0,sigmaSmear));
    this_muon.SetPtEtaPhiM( new_pt, this_muon.Eta(), this_muon.Phi(), this_muon.M() );

  }
  return ;
}



void  AnalyzerCore::SmearHighPtMuonSyst(std::vector<Muon>& muons){

  for(unsigned int i=0; i<muons.size(); i++){
    
    //// Smear high pt only
    if(muons[i].MiniAODPt() < 200) continue;

    ///// Correction based on p https://indico.cern.ch/event/1027400/contributions/4314162/attachments/2224086/3825588/MuonPOG_13Apr21.pdf                                                          
    double MiniAODP = sqrt( muons[i].MiniAODPt() * muons[i].MiniAODPt() + muons[i].Pz() * muons[i].Pz() );
    
    //// 10% shift corresponds to 0.46                                                                                                                                                              
    double sigmaSmear = GetSmearFactor(MiniAODP, fabs(muons[i].Eta())) * 0.46;
    
    /// scale pt                                                                                                                                                                                    
    double new_pt = muons[i].Pt() * (1+gRandom->Gaus(0,sigmaSmear));
    muons[i].SetPtEtaPhiM( new_pt, muons[i].Eta(), muons[i].Phi(), muons[i].M() );
  }
  return ;

}

void AnalyzerCore::HighPtMuonCorr(std::vector<Muon>& muons){


  //// This function needs to be applied to ALLMuon                                                                                                                                
  if(muons.size() != muon_pt->size()) {
    cout << "HighPtMuonCorr function being applied wrong" << endl;
    exit(EXIT_FAILURE);
  }

  /// This function :
  //// a) sets pt of muons > 200 GeV to PF pt
  //// b) smears pt based on pog recommendation
  //// c) sets scale syst to GE syst
  for(unsigned int i=0; i<muons.size(); i++){
    if(muons[i].UncorrectedPt() > 200) {
      /// Change pt back to uncorrecetd PF
      
      muons[i].SetPtEtaPhiM(muons[i].UncorrectedPt(), muons[i].Eta(), muons[i].Phi(), muons[i].M());

      /// change syst scale
      //      cout << "muons[i] = " << muons[i].Pt() << endl;
      SmearHighPtMuon(muons[i]); 
      //      cout << "SmearHighPtMuon muons[i] = " << muons[i].Pt() <<endl;
      
      Particle this_tunep4 = muons[i].TuneP4();
      ScaledPts ptvalues = muonGE->GeneralizedEndpointPt(GetEraShort(), this_tunep4.Pt(), this_tunep4.Charge(), this_tunep4.Eta(), this_tunep4.Phi()*180./M_PI, event);
      muons[i].SetMomentumScaleUpDown(ptvalues.ScaledPt_Up,ptvalues.ScaledPt_Down);
      
    }
  }
  
  return;
}

std::vector<Muon> AnalyzerCore::GetHighPtMuons(TString method, TString ID, double pt, double eta){

  std::vector<Muon> muons = GetMuons(ID,pt,eta);
  std::vector<Muon> out;

  for(unsigned int i=0; i<muons.size(); i++){

    Muon this_muon=muons.at(i);
    double new_pt, new_pt_up, new_pt_down;

    if(method.Contains("TuneP")){
      Particle this_tunep4 = this_muon.TuneP4();
      new_pt= this_tunep4.Pt();
      new_pt_up = this_tunep4.Pt();
      new_pt_down= this_tunep4.Pt();

      if(method.Contains("Roch")){
	double TunePOverPt = this_tunep4.Pt() / this_muon.MiniAODPt();
	new_pt      = TunePOverPt * this_muon.Pt(); // this_muon.Pt() = MiniAODPt * RochesterCorrection                                                                                               
	new_pt_up   = TunePOverPt * this_muon.MomentumShift(+1);
	new_pt_down = TunePOverPt * this_muon.MomentumShift(-1);
      }
      if(method.Contains("GE")){
	ScaledPts ptvalues = muonGE->GeneralizedEndpointPt(GetEraShort(), this_tunep4.Pt(), this_tunep4.Charge(), this_tunep4.Eta(), this_tunep4.Phi()*180./M_PI, event);
	new_pt = ptvalues.ScaledPt;
        new_pt_up = ptvalues.ScaledPt_Up;
        new_pt_down = ptvalues.ScaledPt_Down;
      }
      if(method.Contains("POG")){
	if(this_muon.Pt() < 200){
	  double TunePOverPt = this_tunep4.Pt() / this_muon.MiniAODPt();
	  new_pt      = TunePOverPt * this_muon.Pt(); // this_muon.Pt() = MiniAODPt * RochesterCorrection                                                                                                           
	  new_pt_up   = TunePOverPt * this_muon.MomentumShift(+1);
	  new_pt_down = TunePOverPt * this_muon.MomentumShift(-1);

	}
	else{
	  ScaledPts ptvalues = muonGE->GeneralizedEndpointPt(GetEraShort(), this_tunep4.Pt(), this_tunep4.Charge(), this_tunep4.Eta(), this_tunep4.Phi()*180./M_PI, event);
	  new_pt = ptvalues.ScaledPt;
	  new_pt_up = ptvalues.ScaledPt_Up;
	  new_pt_down = ptvalues.ScaledPt_Down;
	}	
      }
      
      this_muon.SetPtEtaPhiM( new_pt, this_tunep4.Eta(), this_tunep4.Phi(), this_tunep4.M() );
      this_muon.SetMomentumScaleUpDown(new_pt_up,new_pt_down);
      
    }
    else{
      
      new_pt =this_muon.Pt();
      new_pt_up =this_muon.MomentumShift(+1) ;
      new_pt_down = this_muon.MomentumShift(-1);

      if(method.Contains("Nom")){
	new_pt= this_muon.MiniAODPt();
      }
      if(method.Contains("GE")){
	ScaledPts ptvalues = muonGE->GeneralizedEndpointPt(GetEraShort(), this_muon.Pt(), this_muon.Charge(), this_muon.Eta(), this_muon.Phi()*180./M_PI, event);
        new_pt = ptvalues.ScaledPt;
        new_pt_up = ptvalues.ScaledPt_Up;
        new_pt_down = ptvalues.ScaledPt_Down;
      }
      if(method.Contains("POG")){
	if(this_muon.Pt() > 200){
	  ScaledPts ptvalues = muonGE->GeneralizedEndpointPt(GetEraShort(), this_muon.Pt(), this_muon.Charge(), this_muon.Eta(), this_muon.Phi()*180./M_PI, event);
	  new_pt = ptvalues.ScaledPt;
	  new_pt_up = ptvalues.ScaledPt_Up;
	  new_pt_down = ptvalues.ScaledPt_Down;
	}
      }

      this_muon.SetPtEtaPhiM( new_pt, this_muon.Eta(), this_muon.Phi(), this_muon.M() );
      this_muon.SetMomentumScaleUpDown(new_pt_up,new_pt_down);
    }
    out.push_back(this_muon);
  }

  return out;
}
