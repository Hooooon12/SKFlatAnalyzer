#include "HNL_LeptonCore.h"


Particle HNL_LeptonCore::GetvCorrMET(const TString& METType, const std::vector<Muon>& tight_muons, AnalyzerParameter param, Particle METUncorr){

  //// THIS FUNCTION UPDATES MET BASED ON JET Smearing / muon rocc  
  //// Follows https://twiki.cern.ch/twiki/bin/view/CMS/MissingETRun2Corrections                                                                                                                                                                                               
  //// Jets in simulation can be smeared (as shown in JetResolution twiki) to achieve better agreement with data. This is done by default in GetAllJets().....  This correction is a propagation of the smeared such jets to MET. The Smeared MET correction replaces the vector sum of transverse momenta of particles which can be clustered as jets with the vector sum of the transverse momenta of the jets to which smearing is applied.                                                                                                                                                               
  //// Jets pt > 15 GeV not near PF muon OR electrons                                                                                                                                                                                                                                                                         
  
  bool SmearJets  = METType.Contains("SmearJet");
  bool SmearMuons = METType.Contains("SmearMuon");
  bool SmearBoth  = !SmearJets &&  !SmearMuons;
  
  if(SmearBoth || SmearJets) {
    
    std::vector<Jet>  Jets        = GetHNLJets("SmearCorr",param); //// GetvCorrMEToly called for Syst==Cental so get jets == cebtral jets always                                                                                                                                                                             
    std::vector<Muon> loose_muons = GetMuons("POGLoose",     10.,  2.4);
    
    std::vector<Jet> jets_corr;
    for(auto ij : Jets){
     if(ij.Pt() < 15.) continue;
     if(fabs(ij.Eta()) > 2.5) continue;
     bool overlap_mu(false);
     for(auto imu : loose_muons) {
       if(imu.DeltaR(ij) < 0.4) {
   overlap_mu=true;
   break;
       }
     }
     if(overlap_mu) continue;
     double jetEMFrac = ij.ChargedEmEnergyFraction() + ij.NeutralEmEnergyFraction();
     if (jetEMFrac > 0.9) continue;
     jets_corr.push_back(ij);
   }
   

   Particle UpdatedMET = UpdateMETSmearedJet(METUncorr,jets_corr);
   if(SmearJets) return UpdatedMET;

   //   std::vector<Muon> tight_muons = SelectMuons(param, param.Muon_Tight_ID, 10.,  2.4);

   Particle UpdatedMET2 = UpdateMET(UpdatedMET,tight_muons);
   return UpdatedMET2;

 }
 else   if(SmearMuons ) {
   //   std::vector<Muon> tight_muons = SelectMuons(param, param.Muon_Tight_ID, 10.,  2.4);
   Particle UpdatedMET = UpdateMET(METUncorr,tight_muons);
   return UpdatedMET;
 }

 return METUncorr;
}


Particle HNL_LeptonCore::GetvMET(const TString& METType, AnalyzerParameter& param, bool propsmear ){

  if(param.syst_ != AnalyzerParameter::Central){
    cout << "[HNL_LeptonCore::GetvMET] Current syst index : Wrong GetvMET function " <<  endl;
    exit(EXIT_FAILURE);    
  }

  bool ApplySyst      = (!IsDATA) && (param.syst_ != AnalyzerParameter::Central);

  Particle vStandMET = GetMiniAODvMET(METType);
  
  /// in case no smearing propogation
  if(!ApplySyst && !propsmear) return vStandMET; 
  
  //// Simply return smeared MET for non syst function
  return GetvCorrMET(METType,SelectMuons(param.Muon_Tight_ID, 10.,  2.4),param,vStandMET);

}



Particle HNL_LeptonCore::GetvMET(const TString& METType, AnalyzerParameter param, const std::vector<Jet>& jets, const std::vector<FatJet>& fatjets,
				 const std::vector<Muon>& muons, const std::vector<Electron>& electrons, bool propsmear ){


  ////// This function is used to get MET both central and systematic                                                                                                                                                                                                                                                         

  bool ApplySyst      = (!IsDATA) && (param.syst_ != AnalyzerParameter::Central);
  

  Particle vStandMET = GetMiniAODvMET(METType);

  if(HasFlag("ScanSystematic"))  cout << "GetvMET Nom = " << vStandMET.Pt() << endl;
  
  if(!ApplySyst && !propsmear) return vStandMET;  //// This function calls central values stored in MINMIAOD OR POG COrrected                                        
  if(HasFlag("ScanSystematic")) {
    Particle a = GetvCorrMET(METType,muons,param,vStandMET) ;
    cout << "GetvMET smear = " << a.Pt() << " " << vStandMET.Pt()<<endl;

    Particle b = GetvCorrMET(METType,muons,param,vStandMET) ;
    cout << "GetvMET smear2 = " << b.Pt() << " " << vStandMET.Pt()<<endl;

  }
  
  if(!ApplySyst && propsmear)  return GetvCorrMET(METType,muons,param,vStandMET);

  bool UsePuppi     = METType.Contains("Puppi");
  bool IsxyCorr     = METType.Contains("xyCorr");

  // Map the systematic type to the corresponding index
  int IdxSyst = -1;

  // Create a mapping of systematic types to indices
  switch(param.syst_) {
  case AnalyzerParameter::METUnclUp:           IdxSyst = 10; break;
  case AnalyzerParameter::METUnclDown:         IdxSyst = 11; break;
  case AnalyzerParameter::JetResUp:            IdxSyst = 100; break;
  case AnalyzerParameter::JetResDown:          IdxSyst = 101; break;
  case AnalyzerParameter::JetEnUp:             IdxSyst = 102; break;
  case AnalyzerParameter::JetEnDown:           IdxSyst = 103; break;
  case AnalyzerParameter::MuonEnUp:            IdxSyst = 104; break;
  case AnalyzerParameter::MuonEnDown:          IdxSyst = 105; break;
  case AnalyzerParameter::MuonResUp:           IdxSyst = 106; break;
  case AnalyzerParameter::MuonResDown:         IdxSyst = 107; break;
  case AnalyzerParameter::ElectronEnUp:        IdxSyst = 56; break;
  case AnalyzerParameter::ElectronEnDown:      IdxSyst = 57; break;
  case AnalyzerParameter::ElectronResUp:       IdxSyst = 58; break;
  case AnalyzerParameter::ElectronResDown:     IdxSyst = 59; break;
  case AnalyzerParameter::BTagSFHTagCorrUp:        IdxSyst = 60; break;
  case AnalyzerParameter::BTagSFHTagCorrDown:      IdxSyst = 61; break;
  case AnalyzerParameter::BTagSFLTagCorrUp:        IdxSyst = 62; break;
  case AnalyzerParameter::BTagSFLTagCorrDown:      IdxSyst = 63; break;
  case AnalyzerParameter::BTagSFHTagUnCorrUp:        IdxSyst = 64; break;
  case AnalyzerParameter::BTagSFHTagUnCorrDown:      IdxSyst = 65; break;
  case AnalyzerParameter::BTagSFLTagUnCorrUp:        IdxSyst = 66; break;
  case AnalyzerParameter::BTagSFLTagUnCorrDown:      IdxSyst = 67; break;

  default:                                      IdxSyst = -1; break;  // Default case in case no match is found
  }


  Particle vMETCorr = propsmear ? GetvCorrMET(METType,muons,param,vStandMET) : vStandMET;
  if(IdxSyst == -1) return vMETCorr;

  Particle vMETFinal;
  
  if(IdxSyst >= 100 ) vMETFinal = vMETCorr; /// Jet smearing  already propagated
  else if(IdxSyst >= 20 )  vMETFinal = UpdateMETSyst(param, vMETCorr, jets, fatjets, muons, electrons);
  else if(IdxSyst>=0){

    if(UsePuppi) {
      if(isfinite(PuppiMET_Type1_pt_shifts->at(IdxSyst))){
	if(HasFlag("ScanSystematicMET")){
	  if(IdxSyst == 10) cout <<" Uncluster Up PuppiMET_Type1_pt = " << PuppiMET_Type1_pt << " PuppiMET_Type1_pt_shifts->at(IdxSyst) = " << PuppiMET_Type1_pt_shifts->at(IdxSyst) << endl;
	  else cout <<" Uncluster Down PuppiMET_Type1_pt = " << PuppiMET_Type1_pt << " PuppiMET_Type1_pt_shifts->at(IdxSyst) = " << PuppiMET_Type1_pt_shifts->at(IdxSyst) << endl;

	  if(IdxSyst == 10) cout <<" Uncluster Up pfMET_Type1_pt = " << pfMET_Type1_pt << " pfMET_Type1_pt_shifts->at(IdxSyst) = " << pfMET_Type1_pt_shifts->at(IdxSyst) << endl;
          else cout <<" Uncluster Down pfMET_Type1_pt = " << pfMET_Type1_pt << " pfMET_Type1_pt_shifts->at(IdxSyst) = " << pfMET_Type1_pt_shifts->at(IdxSyst) << endl;
	  

	}
        vMETFinal = UpdateMETSyst(PuppiMET_Type1_pt, PuppiMET_Type1_phi, PuppiMET_Type1_pt_shifts->at(IdxSyst),PuppiMET_Type1_phi_shifts->at(IdxSyst), vMETCorr);
      }
    }
    else {
      if(isfinite(pfMET_Type1_PhiCor_pt_shifts->at(IdxSyst))){
        if(IsxyCorr) vMETFinal = UpdateMETSyst(pfMET_Type1_PhiCor_pt, pfMET_Type1_PhiCor_phi, pfMET_Type1_PhiCor_pt_shifts->at(IdxSyst), pfMET_Type1_PhiCor_phi_shifts->at(IdxSyst), vMETCorr);
        else         vMETFinal = UpdateMETSyst(pfMET_Type1_pt, pfMET_Type1_phi, pfMET_Type1_pt_shifts->at(IdxSyst), pfMET_Type1_phi_shifts->at(IdxSyst), vMETCorr);
      }
    }
  }

  if(HasFlag("ScanSystematic")){
    cout << "GetvMET [IdxSyst = "<<IdxSyst<< "]final = " << vMETFinal.Pt() << endl;
  }

  return vMETFinal;

}



// https://lathomas.web.cern.ch/lathomas/METStuff/XYCorrections/XYMETCorrection_withUL17andUL18andUL16.h                                                                                                                                      

std::pair<double,double> HNL_LeptonCore::METXYCorr_Met_MetPhi(double uncormet, double uncormet_phi, int runnb, const TString& year, bool isMC, int npv, bool isUL , bool ispuppi){

  std::pair<double,double>  TheXYCorr_Met_MetPhi(uncormet,uncormet_phi);

  if(npv>100) npv=100;
  int runera =-1;
  bool usemetv2 =false;
  if(isMC && year == "2016" && !isUL) runera = y2016MC;
  else if(isMC && year == "2017" && !isUL) {runera = y2017MC; usemetv2 =true;}
  else if(isMC && year == "2018" && !isUL) runera = y2018MC;
  else if(isMC && year == "2016APV" && isUL) runera = yUL2016MCAPV;
  else if(isMC && year == "2016nonAPV" && isUL) runera = yUL2016MCnonAPV;
  else if(isMC && year == "2017" && isUL) runera = yUL2017MC;
  else if(isMC && year == "2018" && isUL) runera = yUL2018MC;


  else if(!isMC && runnb >=272007 && runnb <=275376 && !isUL) runera = y2016B;
  else if(!isMC && runnb >=275657 && runnb <=276283 && !isUL) runera = y2016C;
  else if(!isMC && runnb >=276315 && runnb <=276811 && !isUL) runera = y2016D;
  else if(!isMC && runnb >=276831 && runnb <=277420 && !isUL) runera = y2016E;
  else if(!isMC && runnb >=277772 && runnb <=278808 && !isUL) runera = y2016F;
  else if(!isMC && runnb >=278820 && runnb <=280385 && !isUL) runera = y2016G;
  else if(!isMC && runnb >=280919 && runnb <=284044 && !isUL) runera = y2016H;

  else if(!isMC && runnb >=297020 && runnb <=299329 && !isUL){ runera = y2017B; usemetv2 =true;}
  else if(!isMC && runnb >=299337 && runnb <=302029 && !isUL){ runera = y2017C; usemetv2 =true;}
  else if(!isMC && runnb >=302030 && runnb <=303434 && !isUL){ runera = y2017D; usemetv2 =true;}
  else if(!isMC && runnb >=303435 && runnb <=304826 && !isUL){ runera = y2017E; usemetv2 =true;}
  else if(!isMC && runnb >=304911 && runnb <=306462 && !isUL){ runera = y2017F; usemetv2 =true;}

  else if(!isMC && runnb >=315252 && runnb <=316995 && !isUL) runera = y2018A;
  else if(!isMC && runnb >=316998 && runnb <=319312 && !isUL) runera = y2018B;
  else if(!isMC && runnb >=319313 && runnb <=320393 && !isUL) runera = y2018C;
  else if(!isMC && runnb >=320394 && runnb <=325273 && !isUL) runera = y2018D;

  else if(!isMC && runnb >=315252 && runnb <=316995 && isUL) runera = yUL2018A;
  else if(!isMC && runnb >=316998 && runnb <=319312 && isUL) runera = yUL2018B;
  else if(!isMC && runnb >=319313 && runnb <=320393 && isUL) runera = yUL2018C;
  else if(!isMC && runnb >=320394 && runnb <=325273 && isUL) runera = yUL2018D;

  else if(!isMC && runnb >=297020 && runnb <=299329 && isUL){ runera = yUL2017B; usemetv2 =false;}
  else if(!isMC && runnb >=299337 && runnb <=302029 && isUL){ runera = yUL2017C; usemetv2 =false;}
  else if(!isMC && runnb >=302030 && runnb <=303434 && isUL){ runera = yUL2017D; usemetv2 =false;}
  else if(!isMC && runnb >=303435 && runnb <=304826 && isUL){ runera = yUL2017E; usemetv2 =false;}
  else if(!isMC && runnb >=304911 && runnb <=306462 && isUL){ runera = yUL2017F; usemetv2 =false;}

  else if(!isMC && runnb >=272007 && runnb <=275376 && isUL) runera = yUL2016B;
  else if(!isMC && runnb >=275657 && runnb <=276283 && isUL) runera = yUL2016C;
  else if(!isMC && runnb >=276315 && runnb <=276811 && isUL) runera = yUL2016D;
  else if(!isMC && runnb >=276831 && runnb <=277420 && isUL) runera = yUL2016E;
  else if(!isMC && ((runnb >=277772 && runnb <=278768) || runnb==278770) && isUL) runera = yUL2016F;
  else if(!isMC && ((runnb >=278801 && runnb <=278808) || runnb==278769) && isUL) runera = yUL2016Flate;
  else if(!isMC && runnb >=278820 && runnb <=280385 && isUL) runera = yUL2016G;
  else if(!isMC && runnb >=280919 && runnb <=284044 && isUL) runera = yUL2016H;


  else {
    //Couldn't find data/MC era => no correction applied                                                                                                                                                                                      
    return TheXYCorr_Met_MetPhi;
  }


  double METxcorr(0.),METycorr(0.);

  if(!usemetv2){//Current recommendation for 2016 and 2018                                                                                                                                                                                    
    if(!ispuppi){
      if(runera==y2016B) METxcorr = -(-0.0478335*npv -0.108032);
      if(runera==y2016B) METycorr = -(0.125148*npv +0.355672);
      if(runera==y2016C) METxcorr = -(-0.0916985*npv +0.393247);
      if(runera==y2016C) METycorr = -(0.151445*npv +0.114491);
      if(runera==y2016D) METxcorr = -(-0.0581169*npv +0.567316);
      if(runera==y2016D) METycorr = -(0.147549*npv +0.403088);
      if(runera==y2016E) METxcorr = -(-0.065622*npv +0.536856);
      if(runera==y2016E) METycorr = -(0.188532*npv +0.495346);
      if(runera==y2016F) METxcorr = -(-0.0313322*npv +0.39866);
      if(runera==y2016F) METycorr = -(0.16081*npv +0.960177);
      if(runera==y2016G) METxcorr = -(0.040803*npv -0.290384);
      if(runera==y2016G) METycorr = -(0.0961935*npv +0.666096);
      if(runera==y2016H) METxcorr = -(0.0330868*npv -0.209534);
      if(runera==y2016H) METycorr = -(0.141513*npv +0.816732);
      if(runera==y2017B) METxcorr = -(-0.259456*npv +1.95372);
      if(runera==y2017B) METycorr = -(0.353928*npv -2.46685);
      if(runera==y2017C) METxcorr = -(-0.232763*npv +1.08318);
      if(runera==y2017C) METycorr = -(0.257719*npv -1.1745);
      if(runera==y2017D) METxcorr = -(-0.238067*npv +1.80541);
      if(runera==y2017D) METycorr = -(0.235989*npv -1.44354);
      if(runera==y2017E) METxcorr = -(-0.212352*npv +1.851);
      if(runera==y2017E) METycorr = -(0.157759*npv -0.478139);
      if(runera==y2017F) METxcorr = -(-0.232733*npv +2.24134);
      if(runera==y2017F) METycorr = -(0.213341*npv +0.684588);
      if(runera==y2018A) METxcorr = -(0.362865*npv -1.94505);
      if(runera==y2018A) METycorr = -(0.0709085*npv -0.307365);
      if(runera==y2018B) METxcorr = -(0.492083*npv -2.93552);
      if(runera==y2018B) METycorr = -(0.17874*npv -0.786844);
      if(runera==y2018C) METxcorr = -(0.521349*npv -1.44544);
      if(runera==y2018C) METycorr = -(0.118956*npv -1.96434);
      if(runera==y2018D) METxcorr = -(0.531151*npv -1.37568);
      if(runera==y2018D) METycorr = -(0.0884639*npv -1.57089);
      if(runera==y2016MC) METxcorr = -(-0.195191*npv -0.170948);
      if(runera==y2016MC) METycorr = -(-0.0311891*npv +0.787627);
      if(runera==y2017MC) METxcorr = -(-0.217714*npv +0.493361);
      if(runera==y2017MC) METycorr = -(0.177058*npv -0.336648);
      if(runera==y2018MC) METxcorr = -(0.296713*npv -0.141506);
      if(runera==y2018MC) METycorr = -(0.115685*npv +0.0128193);


      //UL2017                                                                                                                                                                                                                                
      if(runera==yUL2017B) METxcorr = -(-0.211161*npv +0.419333);
      if(runera==yUL2017B) METycorr = -(0.251789*npv +-1.28089);
      if(runera==yUL2017C) METxcorr = -(-0.185184*npv +-0.164009);
      if(runera==yUL2017C) METycorr = -(0.200941*npv +-0.56853);
      if(runera==yUL2017D) METxcorr = -(-0.201606*npv +0.426502);
      if(runera==yUL2017D) METycorr = -(0.188208*npv +-0.58313);
      if(runera==yUL2017E) METxcorr = -(-0.162472*npv +0.176329);
      if(runera==yUL2017E) METycorr = -(0.138076*npv +-0.250239);
      if(runera==yUL2017F) METxcorr = -(-0.210639*npv +0.72934);
      if(runera==yUL2017F) METycorr = -(0.198626*npv +1.028);
      if(runera==yUL2017MC) METxcorr = -(-0.300155*npv +1.90608);
      if(runera==yUL2017MC) METycorr = -(0.300213*npv +-2.02232);

      //UL2018                                                                                                                                                                                                                                
      if(runera==yUL2018A) METxcorr = -(0.263733*npv +-1.91115);
      if(runera==yUL2018A) METycorr = -(0.0431304*npv +-0.112043);
      if(runera==yUL2018B) METxcorr = -(0.400466*npv +-3.05914);
      if(runera==yUL2018B) METycorr = -(0.146125*npv +-0.533233);
      if(runera==yUL2018C) METxcorr = -(0.430911*npv +-1.42865);
      if(runera==yUL2018C) METycorr = -(0.0620083*npv +-1.46021);
      if(runera==yUL2018D) METxcorr = -(0.457327*npv +-1.56856);
      if(runera==yUL2018D) METycorr = -(0.0684071*npv +-0.928372);
      if(runera==yUL2018MC) METxcorr = -(0.183518*npv +0.546754);
      if(runera==yUL2018MC) METycorr = -(0.192263*npv +-0.42121);

      //UL2016                                                                                                                                                                                                                                
      if(runera==yUL2016B) METxcorr = -(-0.0214894*npv +-0.188255);
      if(runera==yUL2016B) METycorr = -(0.0876624*npv +0.812885);
      if(runera==yUL2016C) METxcorr = -(-0.032209*npv +0.067288);
      if(runera==yUL2016C) METycorr = -(0.113917*npv +0.743906);
      if(runera==yUL2016D) METxcorr = -(-0.0293663*npv +0.21106);
      if(runera==yUL2016D) METycorr = -(0.11331*npv +0.815787);
      if(runera==yUL2016E) METxcorr = -(-0.0132046*npv +0.20073);
      if(runera==yUL2016E) METycorr = -(0.134809*npv +0.679068);
      if(runera==yUL2016F) METxcorr = -(-0.0543566*npv +0.816597);
      if(runera==yUL2016F) METycorr = -(0.114225*npv +1.17266);
      if(runera==yUL2016Flate) METxcorr = -(0.134616*npv +-0.89965);
      if(runera==yUL2016Flate) METycorr = -(0.0397736*npv +1.0385);
      if(runera==yUL2016G) METxcorr = -(0.121809*npv +-0.584893);
      if(runera==yUL2016G) METycorr = -(0.0558974*npv +0.891234);
      if(runera==yUL2016H) METxcorr = -(0.0868828*npv +-0.703489);
      if(runera==yUL2016H) METycorr = -(0.0888774*npv +0.902632);
      if(runera==yUL2016MCnonAPV) METxcorr = -(-0.153497*npv +-0.231751);
      if(runera==yUL2016MCnonAPV) METycorr = -(0.00731978*npv +0.243323);
      if(runera==yUL2016MCAPV) METxcorr = -(-0.188743*npv +0.136539);
      if(runera==yUL2016MCAPV) METycorr = -(0.0127927*npv +0.117747);



    }
    //UL2017Puppi                                                                                                                                                                                                                             
    if(ispuppi){
      if(runera==yUL2017B) METxcorr = -(-0.00382117*npv +-0.666228);
      if(runera==yUL2017B) METycorr = -(0.0109034*npv +0.172188);
      if(runera==yUL2017C) METxcorr = -(-0.00110699*npv +-0.747643);
      if(runera==yUL2017C) METycorr = -(-0.0012184*npv +0.303817);
      if(runera==yUL2017D) METxcorr = -(-0.00141442*npv +-0.721382);
      if(runera==yUL2017D) METycorr = -(-0.0011873*npv +0.21646);
      if(runera==yUL2017E) METxcorr = -(0.00593859*npv +-0.851999);
      if(runera==yUL2017E) METycorr = -(-0.00754254*npv +0.245956);
      if(runera==yUL2017F) METxcorr = -(0.00765682*npv +-0.945001);
      if(runera==yUL2017F) METycorr = -(-0.0154974*npv +0.804176);
      if(runera==yUL2017MC) METxcorr = -(-0.0102265*npv +-0.446416);
      if(runera==yUL2017MC) METycorr = -(0.0198663*npv +0.243182);

      //UL2018Puppi                                                                                                                                                                                                                           
      if(runera==yUL2018A) METxcorr = -(-0.0073377*npv +0.0250294);
      if(runera==yUL2018A) METycorr = -(-0.000406059*npv +0.0417346);
      if(runera==yUL2018B) METxcorr = -(0.00434261*npv +0.00892927);
      if(runera==yUL2018B) METycorr = -(0.00234695*npv +0.20381);
      if(runera==yUL2018C) METxcorr = -(0.00198311*npv +0.37026);
      if(runera==yUL2018C) METycorr = -(-0.016127*npv +0.402029);
      if(runera==yUL2018D) METxcorr = -(0.00220647*npv +0.378141);
      if(runera==yUL2018D) METycorr = -(-0.0160244*npv +0.471053);
      if(runera==yUL2018MC) METxcorr = -(-0.0214557*npv +0.969428);
      if(runera==yUL2018MC) METycorr = -(0.0167134*npv +0.199296);

      //UL2016Puppi                                                                                                                                                                                                                           
      if(runera==yUL2016B) METxcorr = -(-0.00109025*npv +-0.338093);
      if(runera==yUL2016B) METycorr = -(-0.00356058*npv +0.128407);
      if(runera==yUL2016C) METxcorr = -(-0.00271913*npv +-0.342268);
      if(runera==yUL2016C) METycorr = -(0.00187386*npv +0.104);
      if(runera==yUL2016D) METxcorr = -(-0.00254194*npv +-0.305264);
      if(runera==yUL2016D) METycorr = -(-0.00177408*npv +0.164639);
      if(runera==yUL2016E) METxcorr = -(-0.00358835*npv +-0.225435);
      if(runera==yUL2016E) METycorr = -(-0.000444268*npv +0.180479);
      if(runera==yUL2016F) METxcorr = -(0.0056759*npv +-0.454101);
      if(runera==yUL2016F) METycorr = -(-0.00962707*npv +0.35731);
      if(runera==yUL2016Flate) METxcorr = -(0.0234421*npv +-0.371298);
      if(runera==yUL2016Flate) METycorr = -(-0.00997438*npv +0.0809178);
      if(runera==yUL2016G) METxcorr = -(0.0182134*npv +-0.335786);
      if(runera==yUL2016G) METycorr = -(-0.0063338*npv +0.093349);
      if(runera==yUL2016H) METxcorr = -(0.015702*npv +-0.340832);
      if(runera==yUL2016H) METycorr = -(-0.00544957*npv +0.199093);
      if(runera==yUL2016MCnonAPV) METxcorr = -(-0.0058341*npv +-0.395049);
      if(runera==yUL2016MCnonAPV) METycorr = -(0.00971595*npv +-0.101288);
      if(runera==yUL2016MCAPV) METxcorr = -(-0.0060447*npv +-0.4183);
      if(runera==yUL2016MCAPV) METycorr = -(0.008331*npv +-0.0990046);


    }


  }
  else {//these are the corrections for v2 MET recipe (currently recommended for 2017)                                                                                                                                                        
    if(runera==y2016B) METxcorr = -(-0.0374977*npv +0.00488262);
    if(runera==y2016B) METycorr = -(0.107373*npv +-0.00732239);
    if(runera==y2016C) METxcorr = -(-0.0832562*npv +0.550742);
    if(runera==y2016C) METycorr = -(0.142469*npv +-0.153718);
    if(runera==y2016D) METxcorr = -(-0.0400931*npv +0.753734);
    if(runera==y2016D) METycorr = -(0.127154*npv +0.0175228);
    if(runera==y2016E) METxcorr = -(-0.0409231*npv +0.755128);
    if(runera==y2016E) METycorr = -(0.168407*npv +0.126755);
    if(runera==y2016F) METxcorr = -(-0.0161259*npv +0.516919);
    if(runera==y2016F) METycorr = -(0.141176*npv +0.544062);
    if(runera==y2016G) METxcorr = -(0.0583851*npv +-0.0987447);
    if(runera==y2016G) METycorr = -(0.0641427*npv +0.319112);
    if(runera==y2016H) METxcorr = -(0.0706267*npv +-0.13118);
    if(runera==y2016H) METycorr = -(0.127481*npv +0.370786);
    if(runera==y2017B) METxcorr = -(-0.19563*npv +1.51859);
    if(runera==y2017B) METycorr = -(0.306987*npv +-1.84713);
    if(runera==y2017C) METxcorr = -(-0.161661*npv +0.589933);
    if(runera==y2017C) METycorr = -(0.233569*npv +-0.995546);
    if(runera==y2017D) METxcorr = -(-0.180911*npv +1.23553);
    if(runera==y2017D) METycorr = -(0.240155*npv +-1.27449);
    if(runera==y2017E) METxcorr = -(-0.149494*npv +0.901305);
    if(runera==y2017E) METycorr = -(0.178212*npv +-0.535537);
    if(runera==y2017F) METxcorr = -(-0.165154*npv +1.02018);
    if(runera==y2017F) METycorr = -(0.253794*npv +0.75776);
    if(runera==y2018A) METxcorr = -(0.362642*npv +-1.55094);
    if(runera==y2018A) METycorr = -(0.0737842*npv +-0.677209);
    if(runera==y2018B) METxcorr = -(0.485614*npv +-2.45706);
    if(runera==y2018B) METycorr = -(0.181619*npv +-1.00636);
    if(runera==y2018C) METxcorr = -(0.503638*npv +-1.01281);
    if(runera==y2018C) METycorr = -(0.147811*npv +-1.48941);
    if(runera==y2018D) METxcorr = -(0.520265*npv +-1.20322);
    if(runera==y2018D) METycorr = -(0.143919*npv +-0.979328);
    if(runera==y2016MC) METxcorr = -(-0.159469*npv +-0.407022);
    if(runera==y2016MC) METycorr = -(-0.0405812*npv +0.570415);
    if(runera==y2017MC) METxcorr = -(-0.182569*npv +0.276542);
    if(runera==y2017MC) METycorr = -(0.155652*npv +-0.417633);
    if(runera==y2018MC) METxcorr = -(0.299448*npv +-0.13866);
    if(runera==y2018MC) METycorr = -(0.118785*npv +0.0889588);





  }
  double CorrectedMET_x = uncormet *cos( uncormet_phi)+METxcorr;
  double CorrectedMET_y = uncormet *sin( uncormet_phi)+METycorr;

  double CorrectedMET = sqrt(CorrectedMET_x*CorrectedMET_x+CorrectedMET_y*CorrectedMET_y);
  double CorrectedMETPhi;
  if(CorrectedMET_x==0 && CorrectedMET_y>0) CorrectedMETPhi = TMath::Pi();
  else if(CorrectedMET_x==0 && CorrectedMET_y<0 )CorrectedMETPhi = -TMath::Pi();
  else if(CorrectedMET_x >0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x);
  else if(CorrectedMET_x <0&& CorrectedMET_y>0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x) + TMath::Pi();
  else if(CorrectedMET_x <0&& CorrectedMET_y<0) CorrectedMETPhi = TMath::ATan(CorrectedMET_y/CorrectedMET_x) - TMath::Pi();
  else CorrectedMETPhi =0;

  TheXYCorr_Met_MetPhi.first= CorrectedMET;
  TheXYCorr_Met_MetPhi.second= CorrectedMETPhi;
  return TheXYCorr_Met_MetPhi;

}


map<TString, Particle> HNL_LeptonCore::METMap( AnalyzerParameter& param){

  vector<TString> vmets = {"T1xyCorr",
                           "PuppiT1xyCorr",
                           "T1",
                           "PuppiT1",
                           "PuppiT1xyULCorr",
                           "T1xyULCorr"};


  Particle METv      = GetMiniAODvMET("T1xyCorr");
  Particle PuppiMETv = GetMiniAODvMET("PuppiT1xyCorr");
  Particle METvNoPhi = GetMiniAODvMET("T1");
  Particle PuppiMETvNoPhi = GetMiniAODvMET("PuppiT1");
  Particle PuppiMETvULPhiCorr = GetMiniAODvMET("PuppiT1xyULCorr");
  Particle METvULPhiCorr = GetMiniAODvMET("T1xyULCorr");
  map<TString, Particle> mapmet;
  for(auto i : vmets) {
    mapmet[i] = GetvMET(i,param);
    mapmet[i+"_propsmear"] = GetvMET(i,param,true);
    mapmet[i+"_SmearJet_propsmear"] = GetvMET(i+"SmearJet",param,true);
    mapmet[i+"_SmearMuon_propsmear"] = GetvMET(i+"SmearMuon",param,true);
  }

  return mapmet;
}

Particle HNL_LeptonCore::GetMiniAODvMET(const TString&  METType){
  //PuppiT1xyULCorr
  bool IsType1      = METType.Contains("T1");
  bool IsxyCorr     = METType.Contains("xyCorr");
  bool UsePuppi     = METType.Contains("Puppi");
  bool IsFixxyCorr  = METType.Contains("xyULCorr");

  //METXYCorr_Met_MetPhi(double uncormet, double uncormet_phi, int runnb, TString year, bool isMC, int npv, bool isUL =false,bool ispuppi=false)r                                                          

  
  double Met_pt(0.), Met_phi(0.);

  Particle vMET;

  if(UsePuppi && IsType1  && IsxyCorr)  { Met_pt = PuppiMET_Type1_PhiCor_pt ; Met_phi = PuppiMET_Type1_PhiCor_phi;}
  if(UsePuppi && IsType1  && !IsxyCorr) { Met_pt = PuppiMET_Type1_pt;         Met_phi = PuppiMET_Type1_phi;}
  if(UsePuppi && !IsType1 && !IsxyCorr) { Met_pt = PuppiMET_pt;               Met_phi = PuppiMET_phi;}

  if(!UsePuppi&& IsType1  && IsxyCorr)  { Met_pt = pfMET_Type1_PhiCor_pt;     Met_phi = pfMET_Type1_PhiCor_phi;}
  if(!UsePuppi&& IsType1  && !IsxyCorr) { Met_pt = pfMET_Type1_pt;            Met_phi = pfMET_Type1_phi;}
  if(!UsePuppi&& !IsType1 && !IsxyCorr) { Met_pt = pfMET_pt;                  Met_phi = pfMET_phi;}

  if(IsFixxyCorr) {
    TString Year= "2016";
    if(DataYear==2017) Year= "2017";
    if(DataYear==2018) Year= "2018";
    std::pair<double,double> METPair = METXYCorr_Met_MetPhi(Met_pt, Met_phi, run, Year, !IsData, nPV, true,UsePuppi);
    Met_pt=METPair.first;
    Met_phi=METPair.second;
  }


  vMET.SetPtEtaPhiM(Met_pt, 0., Met_phi, 0.);
  return vMET;

}




