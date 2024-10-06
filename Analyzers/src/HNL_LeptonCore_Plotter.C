#include "HNL_LeptonCore.h"

/*------------------------------------------------------------------------------------------------------------                                           
------------------------------------------------------------------------------------------------------------  
//                                                                                                          
//                                                                                                          
// FUNCTIONS NOW ARE FOR PLOTTING OF SR/CR/GENERAL                                                         
//                                                                                                         
------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------*/

void HNL_LeptonCore::FillFakeHistograms(AnalyzerParameter param, TString plot_dir,std::vector<Lepton *> Leptons, std::vector<Jet> jets,   std::vector<FatJet> fatjets, vector<Jet> BJetColl, Particle met,  double w){

  if(!DrawSyst(param.syst_)) return;
  
  for(auto ilep : Leptons){

    if(!IsData && !ilep->IsFake()) continue;

    TString LorT = (ilep->PassLepID()) ? "Tight" : "LooseNotTight";
    TString MotherJetFlavour = (IsData) ? "Data" :  ilep->MotherJetFlavour();

    vector<TString> PassLabels;
    PassLabels.push_back(plot_dir+"/"+ilep->GetFlavour());
    if(ilep->Pt() < 15)           PassLabels.push_back(plot_dir+"/Pt0to15_"+ilep->GetFlavour());
    else if(ilep->Pt() < 25)      PassLabels.push_back(plot_dir+"/Pt15to25_"+ilep->GetFlavour());
    else     PassLabels.push_back(plot_dir+"/Pt25toInf_"+ilep->GetFlavour());

    if(ilep->CloseJet_BScore()  <  0.02)  PassLabels.push_back(plot_dir+"/BScore0to0p02_"+ilep->GetFlavour());
    else if(ilep->CloseJet_BScore()  <  0.05)  PassLabels.push_back(plot_dir+"/BScore0p02to0p05_"+ilep->GetFlavour());
    else if(ilep->CloseJet_BScore()  <  0.2)  PassLabels.push_back(plot_dir+"/BScore0p05to0p2_"+ilep->GetFlavour());
    else  PassLabels.push_back(plot_dir+"/BScore0p2toInf_"+ilep->GetFlavour());

    if(ilep->CloseJet_BScore()  <  0.9 && ilep->CloseJet_CvsBScore() > 0.1 && ilep->CloseJet_CvsLScore() < 0.6)  PassLabels.push_back(plot_dir+"/DeepJetCuts_"+ilep->GetFlavour());
    

    for(auto ilab : PassLabels){
      
      if(ilep->GetFlavour() == "Electron"){
  FillHist((ilab+"_HF_MVA_"+MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("HF") ,  w, 200, -1, 1 );
  FillHist((ilab+"_HNL_Fake_MVA_"+MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("HNL") ,  w, 200, -1, 1 );
  FillHist((ilab+"_HNL_CF_MVA_"  +MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_CF("EDv5") ,  w, 200, -1, 1 );
  FillHist((ilab+"_HNL_Conv_MVA_"+MotherJetFlavour+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Conv("EDv5") ,  w, 200, -1, 1 );
      }
      
      
      FillHist((ilab+"_LF_MVA_"+MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("LF") ,  w, 200, -1, 1 );
      FillHist((ilab+"_ISO_"+   MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->RelIso() ,  w, 60, 0, 0.6 );
      FillHist((ilab+"_QCD_LFvsHF_"+MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_LFvsHF_v5") ,  w, 200,-1, 1);
      FillHist((ilab+"_QCD_BvsC_"+  MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_BvsC_v5") ,  w, 200, -1, 1 );
      FillHist((ilab+"_BScore_"+    MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_BScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsB_"+      MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsBScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsL_"+      MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsLScore() ,  w, 200, -1, 1);
      FillHist((ilab+"_PtRatio_"+   MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptratio() ,   w, 200, 0, 5 );
      FillHist((ilab+"_PtRel_"+     MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptrel() ,     w, 50, 0, 200  );
      FillHist((ilab+"_Pt_"+   MotherJetFlavour+"_"+ilep->sRegion()+"_"+param.Name +"_"+LorT).Data(), ilep->PtMaxed(200.) ,  w, 100, 0, 200);
      
      FillHist((ilab+"_LF_MVA_"+param.Name +"_"+LorT).Data(),ilep->HNL_MVA_Fake("LF") ,  w, 200, -1, 1 );
      FillHist((ilab+"_ISO_"+   param.Name +"_"+LorT).Data(), ilep->RelIso() ,  w, 60, 0, 0.6 );
      FillHist((ilab+"_QCD_LFvsHF_"+param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_LFvsHF_v5") ,  w, 200,-1, 1);
      FillHist((ilab+"_QCD_BvsC_"+  param.Name +"_"+LorT).Data(), ilep->HNL_MVA_Fake("QCD_BvsC_v5") ,  w, 200, -1, 1 );
      FillHist((ilab+"_BScore_"+    param.Name +"_"+LorT).Data(), ilep->CloseJet_BScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsB_"+      param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsBScore() ,  w, 200, -1, 1 );
      FillHist((ilab+"_CvsL_"+      param.Name +"_"+LorT).Data(), ilep->CloseJet_CvsLScore() ,  w, 200, -1, 1);
      FillHist((ilab+"_PtRatio_"+   param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptratio() ,   w, 200, 0, 5 );
      FillHist((ilab+"_PtRel_"+     param.Name +"_"+LorT).Data(), ilep->CloseJet_Ptrel() ,     w, 50, 0, 200  );
      FillHist((ilab+"_Pt_"+   param.Name +"_"+LorT).Data(), ilep->PtMaxed(200.) ,  w, 100, 0, 200);

    }
  }
  
  return;
}

bool HNL_LeptonCore::DrawSyst(AnalyzerParameter::Syst SystType){
  
  vector<AnalyzerParameter::Syst> SystToPlot;

  if(IsData){
    if(RunFake) SystToPlot= {AnalyzerParameter::Syst::FRUp,AnalyzerParameter::Syst::FRDown, AnalyzerParameter::Syst::FRAJ30,AnalyzerParameter::Syst::FRAJ60,AnalyzerParameter::Syst::FRLooseIDDJUp,AnalyzerParameter::Syst::FRLooseIDDJDown,AnalyzerParameter::Syst::FRPartonSFUp, AnalyzerParameter::Syst::FRPartonSFDown};
    if(RunCF)   SystToPlot= {AnalyzerParameter::Syst::CFRateUp,AnalyzerParameter::Syst::CFRateDown, AnalyzerParameter::Syst::CFSFUp,AnalyzerParameter::Syst::CFSFDown};
    
  }
  //else {
  //  SystToPlot= {AnalyzerParameter::Syst::JetEnUp, AnalyzerParameter::Syst::JetEnDown};
  //}
  
  SystToPlot.push_back(AnalyzerParameter::Syst::Central);
  if(std::find(SystToPlot.begin(), SystToPlot.end(), SystType) != SystToPlot.end()) return true;
  else return false;
}

void HNL_LeptonCore::Fill_PlotsAK8(AnalyzerParameter param, TString region, TString plot_dir, 
           std::vector<Tau> TauColl, std::vector<Jet> jets,   std::vector<FatJet> fatjets, std::vector<Lepton *> leps , 
           Particle  met, double nvtx,  double w){

  //// Only Draw JEC/Fake Syst
  if(!DrawSyst(param.syst_)) return;


  if(fatjets.size() == 0) return;
  if((leps.size()  == 1) && !(param.ChannelType() == "Lepton"))     return;
  if((leps.size()  == 2) && !(param.ChannelType() == "Dilepton"))   return;
  if((leps.size()  == 3) && !(param.ChannelType() == "Trilepton"))  return;
  if((leps.size()  == 4) && !(param.ChannelType() == "Quadlepton")) return;

  
  
  bool DrawLevel1 = true; // Always gets drawn                                                                                                              
  bool DrawLevel2 = (param.PlottingVerbose >= 1);
  bool DrawLevel3 = (param.PlottingVerbose >= 2);


  double minDRTauAK8=9999.;
  double minDRLepAK8=9999.;

  for(unsigned int i=0; i < fatjets.size(); i++){

    //for(auto itau : TauColl){
    //  if(fatjets[i].DeltaR(itau) < minDRTauAK8) minDRTauAK8 = fatjets[i].DeltaR(itau) ;
    //  if(DrawLevel3)FillHist( plot_dir+region+ "/AK8J_dR_Tau",   fatjets[i].DeltaR(itau) ,  w, 50, 0, 5, "#DeltaR (WAK8,Tau)");
    //}
    for(auto ilep : leps){
      if(fatjets[i].DeltaR(*ilep) < minDRLepAK8) minDRLepAK8 = fatjets[i].DeltaR(*ilep) ;
      if(DrawLevel3)FillHist( plot_dir+region+ "/AK8Jet_dR_"+ilep->GetFlavour(),   fatjets[i].DeltaR(*ilep) ,  w, 50, 0, 5, "#DeltaR (WAK8,"+ilep->GetFlavour()+")");
    }

    if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_Eta",          fatjets[i].Eta()       , w, 100, -5., 5.   , "AK8 Jet #eta");
    if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_Pt",           fatjets[i].Pt()        , w, 100, 0., 2000. , "AK8 Jet p_{T} GeV");
    if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_SDMass",       fatjets[i].SDMass()    , w, 100, 0., 500.  , "Mass_{softdrop} GeV");
    if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_tau21",        fatjets[i].PuppiTau21(), w, 200, 0., 1.    , "#tau_{21}");
    if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_MET_dR",       fatjets[i].DeltaR(met) , w, 50,  0., 5     ,"#DeltaR(FJ,met)");
    
    vector<JetTagging::Tagger> Taggers = {    JetTagging::DeepCSV, JetTagging::DeepCSV_CvsL, JetTagging::DeepCSV_CvsB,
                                              JetTagging::particleNet_TvsQCD, JetTagging::particleNet_WvsQCD, JetTagging::particleNet_ZvsQCD,
                                              JetTagging::particleNet_HbbvsQCD, JetTagging::particleNet_HccvsQCD, JetTagging::particleNet_H4qvsQCD, JetTagging::particleNet_QCD,
                                              JetTagging::particleNetMD_Xbb, JetTagging::particleNetMD_Xcc, JetTagging::particleNetMD_Xqq, JetTagging::particleNetMD_QCD};

    for (auto jet_tagger  : Taggers){
      if(DrawLevel2)FillHist( plot_dir+region+"/AK8J_Taggers_"+TString(JetTagging::TaggerToString(jet_tagger)) , fatjets[i].GetTaggerResult(jet_tagger), w, 50, 0, 1., "JetTagging::"+TString(JetTagging::TaggerToString(jet_tagger)));
    }

    if(DrawLevel1)FillHist( plot_dir+region+"/AK8J_Tagger_particleNet_WvsQCD" , fatjets[i].GetTaggerResult(JetTagging::particleNet_WvsQCD), w, 50, 0, 1., "JetTagging::particleNet_WvsQCD");
    if(DrawLevel2)FillHist( plot_dir+region+"/AK8J_Tagger_particleNet_TvsQCD" , fatjets[i].GetTaggerResult(JetTagging::particleNet_TvsQCD), w, 50, 0, 1., "JetTagging::particleNet_TvsQCD");
    if(DrawLevel2)FillHist( plot_dir+region+"/AK8J_Tagger_particleNet_ZvsQCD" , fatjets[i].GetTaggerResult(JetTagging::particleNet_ZvsQCD), w, 50, 0, 1., "JetTagging::particleNet_ZvsQCD");
    if(DrawLevel2)FillHist( plot_dir+region+"/AK8J_Tagger_particleNet_QCD"    , fatjets[i].GetTaggerResult(JetTagging::particleNet_QCD)   , w, 50, 0, 1., "JetTagging::particleNet_WvsQCD");
    if(DrawLevel2)FillHist( plot_dir+region+"/AK8J_Tagger_particleNetMD_QCD"  , fatjets[i].GetTaggerResult(JetTagging::particleNetMD_QCD) , w, 50, 0, 1., "JetTagging::particleNetMD_WvsQCD");
    
  }

  if(DrawLevel1)  FillHist( plot_dir+region+ "/AK8J_NB", fatjets.size() ,     w, 5, 0., 5., "N_{AK8 jets}");
  if(DrawLevel2)  FillHist( plot_dir+region+ "/AK8J_dRmin_Tau",   minDRTauAK8,  w, 50, 0, 5, "#DeltaR (WAK8,Tau)");
  if(DrawLevel2)  FillHist( plot_dir+region+ "/AK8J_dRmin_Lep",   minDRLepAK8,  w, 50, 0, 5, "#DeltaR (WAK8,Lep)");

  if(leps.size() < 2) return;
  Particle llJCand =  *leps[0] + *leps[1]+ fatjets[0];
  Particle l1JCand = *leps[0] +  fatjets[0];
  Particle l2JCand = *leps[1] +  fatjets[0];


  Particle WCand   = fatjets[0];
  Particle N1Cand  = fatjets[0] + *leps[0] ;
  Particle N2Cand  = fatjets[0] + *leps[1] ;
  Particle NCand = (leps[1]->Pt() < leps[0]->Pt()) ? N2Cand : N1Cand;
  Lepton Nlep = (leps[1]->Pt() < 100.) ?  *leps[1] : *leps[0];
  Lepton Wlep = (leps[1]->Pt() > 100.) ? *leps[1] : *leps[0];

  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/NLep_N",      NCand.DeltaR(Nlep) , w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/NLep_W",      WCand.DeltaR(Nlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/NLep_N",      NCand.DeltaR(Nlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/WLep_N",      NCand.DeltaR(Wlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/WLep_W",      WCand.DeltaR(Wlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/WLep_NLep",   Nlep.DeltaR(Wlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/W_N",         NCand.DeltaR(Wlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/NLep_N",    fabs(TVector2::Phi_mpi_pi( ( (NCand.Phi() - Nlep.Phi() )))),  w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/NLep_W",    fabs(TVector2::Phi_mpi_pi( ( (WCand.Phi() - Nlep.Phi() )))),  w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/WLep_N",    fabs(TVector2::Phi_mpi_pi( ( (NCand.Phi() - Wlep.Phi() )))),  w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/WLep_W",    fabs(TVector2::Phi_mpi_pi( ( (WCand.Phi() - Wlep.Phi() )))),  w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/WLep_NLep", fabs(TVector2::Phi_mpi_pi( ( (Nlep.Phi() - Wlep.Phi() )))),  w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/W_N",       fabs(TVector2::Phi_mpi_pi( ( (WCand.Phi() - NCand.Phi() )))),  w, 200, 0., 5.,"");

  Particle WrongN = Wlep + WCand;

  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/Wr_NLep_N",     WrongN.DeltaR(Nlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/Wr_WLep_N",     WrongN.DeltaR(Wlep), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/Wr_NLep_N",   fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - Nlep.Phi() )))),  w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/Wr_WLep_N",   fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - Wlep.Phi() )))),  w, 200, 0., 5.,"");

  Particle W1 = (NCand+Wlep);

  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/W1_NLep",    W1.DeltaR(Nlep),w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/W1_WLep",    W1.DeltaR(Wlep),w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/W1_N",       W1.DeltaR(NCand),w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaR/W1_WCand",   W1.DeltaR(WCand),w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/W1_NLep",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - Nlep.Phi() )))), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/W1_WLep",  fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - Wlep.Phi() )))), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/W1_N",     fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - NCand.Phi() )))), w, 200, 0., 5.,"");
  if(DrawLevel2)FillHist(plot_dir+region+ "/AK8J_DeltaPhi/W1_WCand", fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - WCand.Phi() )))), w, 200, 0., 5.,"");


  for(unsigned int ij =0; ij < jets.size(); ij++){
    if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaR/AK8J_AK4J",   fatjets[0].DeltaR(jets[ij]),  w, 50, 0, 5, "#DeltaR (WAK8,j)");

    //    if(fatjets[0].DeltaR(jets[ij]) < 0.8)  cout << plot_dir << " FAT JET JET DR = " << fatjets[0].DeltaR(jets[ij]) <<  " SD mass = " << fatjets[0].SDMass() <<  endl;
       
    if(jets[ij].GetTaggerResult(JetTagging::DeepJet) > mcCorr->GetJetTaggingCutValue(JetTagging::DeepJet , JetTagging::Medium)) {
      if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_DeepJetM_SDMass",       fatjets[0].SDMass()    , w, 100, 0., 500.  , "Mass_{softdrop} GeV");
      if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaR/AK8J_AK4BJ",   fatjets[0].DeltaR(jets[ij]),  w, 50, 0, 5, "#DeltaR (WAK8,Bj)");
    }
    if(jets[ij].GetTaggerResult(JetTagging::DeepCSV) > mcCorr->GetJetTaggingCutValue(JetTagging::DeepCSV , JetTagging::Medium)) {
      if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_DeepCSVM_SDMass",       fatjets[0].SDMass()    , w, 100, 0., 500.  , "Mass_{softdrop} GeV");
      if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaR/AK8J_AK4BJ_CSV",   fatjets[0].DeltaR(jets[ij]),  w, 50, 0, 5, "#DeltaR (WAK8,Bj)");
    }
  }

  if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_Mass/l1J",  l1JCand.M(),  w, 50, 0, 2500, "Reco M_{l1J}");
  if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_Mass/l2J",  l2JCand.M(),  w, 50, 0, 2500, "Reco M_{l2J}");
  if(DrawLevel1)FillHist( plot_dir+region+ "/AK8J_Mass/llJ",  llJCand.M(),  w, 50, 0, 2500, "Reco M_{llJ}");
  if(DrawLevel2) FillHist( plot_dir+region+ "/AK8J_DeltaR/W_lep1",   fatjets[0].DeltaR(*leps[0]),  w, 50, 0, 5, "#DeltaR (Wj1,lep1)");
  if(DrawLevel2) FillHist( plot_dir+region+ "/AK8J_DeltaR/W_lep2",   fatjets[0].DeltaR(*leps[1]),  w, 50, 0, 5, "#DeltaR (Wj2,lep2)");

  if(fatjets[0].DeltaR(*leps[0] ) < fatjets[0].DeltaR(*leps[1] )){
    Particle lJcloseCand = *leps[0]  +  fatjets[0];
    if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_Mass/W_close_lep",  lJcloseCand.M(),  w, 250, 0, 2000, "Reco M_{llJ}");
  }


  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/l1_llJ",  fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] + *leps[0]+*leps[1]).Phi() - (leps[0]->Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/l2_llJ",  fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] + *leps[0]+*leps[1]).Phi() - (leps[1]->Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/N1_llJ",  fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] + *leps[0]+*leps[1]).Phi() - ((*leps[0]+fatjets[0]).Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/N2_llJ",  fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] + *leps[0]+*leps[1]).Phi() - ((*leps[1]+fatjets[0]).Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/W_llJ",   fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] + *leps[0]+*leps[1]).Phi() - ((fatjets[0]).Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/l1_lJ",   fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] + *leps[0]).Phi() - (leps[1]->Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/l2_lJ",   fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] + *leps[1]).Phi() - (leps[0]->Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/l1_J",    fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] ).Phi() - (leps[0]->Phi() )))),  w,  100, 0., 5., "");
  if(DrawLevel2)FillHist( plot_dir+region+ "/AK8J_DeltaPhi/l2_J",    fabs(TVector2::Phi_mpi_pi( ( (fatjets[0] ).Phi() - (leps[1]->Phi() )))),  w,  100, 0., 5., "");

  return;
}

void HNL_LeptonCore::Fill_RegionPlots(AnalyzerParameter param, TString plot_dir,   std::vector<Jet> jets,    std::vector<FatJet> fatjets, std::vector<Lepton *> leps , Particle  met, double nvtx,  double w){

  if(!DrawSyst(param.syst_)) return;

  vector<Tau> NullTaus;
  Fill_RegionPlots(param, plot_dir, NullTaus,jets,fatjets, leps, met, nvtx, w);
}


void HNL_LeptonCore::Fill_RegionPlots(AnalyzerParameter param, TString plot_dir, vector<Tau> Taus,  std::vector<Jet> jets,    std::vector<FatJet> fatjets, std::vector<Lepton *> leps , Particle  met, double nvtx,  double w){

  if(!DrawSyst(param.syst_)) return;

  TString region ="/"+param.Name + param.hprefix;
  
  Fill_Plots(param, region,plot_dir , Taus,jets,fatjets, leps, met, nvtx, w);

  TString regionL = "/"+param.NameInclusive_Channel + param.hprefix;
  Fill_Plots(param, regionL , plot_dir , Taus, jets ,fatjets, leps, met, nvtx, w);

  return;
}

void HNL_LeptonCore::Fill_Plots(AnalyzerParameter param, TString region,  TString plot_dir, 
        vector<Tau> TauColl,  std::vector<Jet> jets, std::vector<FatJet> fatjets, std::vector<Lepton *> leps , 
        Particle  met, double nvtx,  double w){

  if(!DrawSyst(param.syst_)) return;

  TString regionAK8 = region+"/AK8";
  TString lepregion= region + "/LeptonMVA";

  if((leps.size()  == 1) && !(param.ChannelType() == "Lepton"))     return;
  if((leps.size()  == 2) && !(param.ChannelType() == "Dilepton"))   return;
  if((leps.size()  == 3) && !(param.ChannelType() == "Trilepton"))  return;
  if((leps.size()  == 4) && !(param.ChannelType() == "Quadlepton")) return;
  bool threelep = (leps.size()  == 3);
  bool fourlep  = (leps.size()  == 4);
  
  int nel(0), nmu(0);
  for(auto ilep: leps) {
    if(ilep->LeptonFlavour() == Lepton::ELECTRON) nel++;
    if(ilep->LeptonFlavour() == Lepton::MUON) nmu++;
  }
    
  //  std::vector<Jet>    AK4_BJetColl                = GetHNLJets("BJet_NoAk8Cleaning", param);
  //std::vector<FatJet> AK8_JetColl_PN_NoMass          = GetHNLAK8Jets("HNL_PN_NoMass",param);
  //std::vector<FatJet> AK8_JetColl_NoMass          = GetHNLAK8Jets("HNL_NoMass",param);
 
  Fill_PlotsAK8(param, regionAK8, plot_dir,TauColl,jets , fatjets, leps, met, nvtx,w);
  
  bool DrawLevel1 = true; // Always gets drawn
  bool DrawLevel2 = (param.PlottingVerbose >= 1);
  bool DrawLevel3 = (param.PlottingVerbose >= 2);

  if(plot_dir.Contains("HNL_HighMassNP")) {
    DrawLevel1 = true; 
    DrawLevel2 = true;
    DrawLevel3 = true;
  }

  /// Draw N leptons                                                                                                                                                                                             
  if(DrawLevel1) FillHist( plot_dir+ region+ "/NObj/N_El", nel,  w, 5, 0, 5, "El size");
  if(DrawLevel1) FillHist( plot_dir+ region+ "/NObj/N_Mu", nmu,  w, 5, 0, 5, "Mu size");
  // Draw N jets                                                                                                        
  if(DrawLevel1) FillHist( plot_dir+ region+ "/NObj/N_AK4J", jets.size() , w, 10, 0., 10., "N_{AK4 jets}");

  ///// Only Draw Multi Lepton plots

  if(leps.size() < 2) return;
  
  int nlepMVA(0);
  for(auto ilep : leps){
    map<TString, double> lep_bdt_map = ilep->MAPBDTFake();
    TString LepTag = "_lep1";
    if(nlepMVA==0) LepTag = "_lep0";
    else if(nlepMVA==1) LepTag = "_lep1";
    else LepTag =  "_lep2plus";
    for(auto i : lep_bdt_map) {
      if(DrawLevel1)  FillHist( plot_dir+ lepregion+ "/Lepton_"+LepTag+"_mva_"+i.first , i.second, w, 100, -1., 1., "MVA");
      if(DrawLevel1)  FillHist( plot_dir+ lepregion+ "/Lepton_"+ilep->GetEtaRegion("2bin")+"_"+LepTag+"_mva_"+i.first , i.second, w, 100, -1., 1., "MVA");
    }
    nlepMVA++;
  }


  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/SumQ", leps[0]->Charge() + leps[1]->Charge(),  w, 10, -5, 5, "Q size");

  Particle llCand = *leps[0] + *leps[1];
  Particle lllCand = (threelep) ? *leps[0] + *leps[1] + *leps[2] :  Particle();
  Particle lljjCand;

  if(jets.size() ==1 ) {

    lljjCand += jets[0];

    Particle WCand  = jets[0];
    Particle N1Cand = jets[0]+ *leps[0] ;
    Particle N2Cand = jets[0]+ *leps[1] ;

    if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/Single_AK4J_dR_W_lep1",   WCand.DeltaR(*leps[0] ),  w, 50, 0, 5, "#DeltaR (W,lep1)");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/Single_AK4J_dR_W_lep2",   WCand.DeltaR(*leps[1] ),  w, 50, 0, 5, "#DeltaR (W,lep2)");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/Single_AK4J_mW",          WCand.M(),                w, 50, 0, 500, "Reco_Onejet M_{jj}");

    int nSRbins=10;
    double mljbins[nSRbins] = { 0., 200., 400.,500., 600.,700., 1000.,1250., 1500., 2000.};
    double MN1 = (N1Cand.M() > 2000.) ? 1999. : N1Cand.M();
    double MN2 = (N2Cand.M() > 2000.) ? 1999. : N2Cand.M();
    double MllW = (lljjCand.M() > 2000.) ? 1999. : lljjCand.M();

    if(DrawLevel3)FillHist( plot_dir+ region+ "/Mass/Single_AK4J_M_l1W",        MN1,   w, 7, mljbins, "Reco_Onejet M_{l1jj}");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/Mass/Single_AK4J_M_l2W",        MN2,   w, 7, mljbins, "Reco_Onejet M_{l2jj} ");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/Mass/Single_AK4J_M_llW",        MllW,  w, 7, mljbins, "Reco_Onejet M_{lljj}");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaPhi/Single_AK4J_dPhi_l1_l1J",  fabs(TVector2::Phi_mpi_pi( ( (*leps[0]+ jets[0]).Phi() - (leps[1]->Phi() )))),  w,  500, 0., 2000., "Reco_Onejet M_{lljj}");
  }
  if(jets.size() > 1){

    double dijetmass_tmp=999.;
    double dijetmass=9990000.;
    int m=-999;
    int n=-999;

    for(UInt_t emme=0; emme<jets.size(); emme++){
      for(UInt_t enne=1; enne<jets.size(); enne++) {
        if(emme == enne) continue;
        dijetmass_tmp = (jets[emme]+jets[enne]).M();

        if ( fabs(dijetmass_tmp-80.4) < fabs(dijetmass-80.4) ) {
          dijetmass = dijetmass_tmp;
          m = emme;
          n = enne;
        }
      }
    }
    Particle W1Cand  = *leps[0]  + *leps[1]  + jets[m]+jets[n];
    Particle WCand   = jets[m]+jets[n];
    Particle N1Cand  = jets[m]+jets[n]+ *leps[0] ;
    Particle N2Cand  = jets[m]+jets[n]+ *leps[1] ;
    Particle NCand   = (leps[1]->Pt() < leps[0]->Pt()) ? N2Cand : N1Cand;
    Lepton Nlep      = (leps[1]->Pt() < 100.) ?  *leps[1] : *leps[0];
    Lepton Wlep      = (leps[1]->Pt() > 100.) ? *leps[1] : *leps[0];

    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_NLep_N",     NCand.DeltaR(Nlep) ,  w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_NLep_W",     WCand.DeltaR(Nlep),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_NLep_N",     NCand.DeltaR(Nlep),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_WLep_N",     NCand.DeltaR(Wlep),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_WLep_W",     WCand.DeltaR(Wlep),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_NLep_J1",    jets[m].DeltaR(Nlep), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_NLep_J2",    jets[n].DeltaR(Nlep), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_WLep_NLep",  Nlep.DeltaR(Wlep),    w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_W_N",        NCand.DeltaR(Wlep),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_NLep_N",   fabs(TVector2::Phi_mpi_pi( ( (NCand.Phi() - Nlep.Phi() )))),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_NLep_W",   fabs(TVector2::Phi_mpi_pi( ( (WCand.Phi() - Nlep.Phi() )))),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_WLep_N",   fabs(TVector2::Phi_mpi_pi( ( (NCand.Phi() - Wlep.Phi() )))),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_WLep_W",   fabs(TVector2::Phi_mpi_pi( ( (WCand.Phi() - Wlep.Phi() )))),   w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_WLep_NLep",fabs(TVector2::Phi_mpi_pi( ( (Nlep.Phi() - Wlep.Phi() )))),    w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_W_N",      fabs(TVector2::Phi_mpi_pi( ( (WCand.Phi() - NCand.Phi() )))),  w, 200, 0., 5.,"");

    Particle WrongN = Wlep + WCand;
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaRWr_NLep_N",    WrongN.DeltaR(Nlep), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaRWr_WLep_N",    WrongN.DeltaR(Wlep), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhiWr_NLep_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - Nlep.Phi() )))),  w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhiWr_WLep_N",  fabs(TVector2::Phi_mpi_pi( ( (WrongN.Phi() - Wlep.Phi() )))),  w, 200, 0., 5.,"");

    Particle W1 = (NCand+Wlep);
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_W1_NLep",   W1.DeltaR(Nlep), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_W1_WLep",   W1.DeltaR(Wlep), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_W1_N",      W1.DeltaR(NCand),w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_DeltaR_W1_WCand",  W1.DeltaR(WCand),w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_W1_NLep", fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - Nlep.Phi() )))),  w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_W1_WLep", fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - Wlep.Phi() )))),  w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_W1_N",    fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - NCand.Phi() )))), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_DeltaPhi_W1_WCand",fabs(TVector2::Phi_mpi_pi( ( (W1.Phi() - WCand.Phi() )))), w, 200, 0., 5.,"");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_l1_llJ",      fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] + *leps[0]+*leps[1]).Phi() - (leps[0]->Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_l2_llJ",      fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] + *leps[0]+*leps[1]).Phi() - (leps[1]->Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_N1_llJ",      fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] + *leps[0]+*leps[1]).Phi() - ((*leps[0]+jets[m]+ jets[n]).Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_N2_llJ",      fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] + *leps[0]+*leps[1]).Phi() - ((*leps[1]+jets[m]+ jets[n]).Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_W_llJ",       fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] + *leps[0]+*leps[1]).Phi() - ((jets[m]+ jets[n]).Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_l1_lJ",       fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] + *leps[0]).Phi() - (leps[1]->Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_l2_lJ",       fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] + *leps[1]).Phi() - (leps[0]->Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_l1_J",        fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] ).Phi() - (leps[0]->Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaPhi/DiJet_dPhi_l2_J",        fabs(TVector2::Phi_mpi_pi( ( (jets[m]+ jets[n] ).Phi() - (leps[1]->Phi() )))),  w,  100, 0., 5., "");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_dR_W_lep1",   WCand.DeltaR(*leps[0] ),  w, 50, 0, 5, "#DeltaR (W,lep1)");
    if(DrawLevel3)FillHist(plot_dir+ region+ "/DeltaR/DiJet_dR_W_lep2",   WCand.DeltaR(*leps[1] ),  w, 50, 0, 5, "#DeltaR (W,lep2)");
    if(DrawLevel1)FillHist(plot_dir+ region+ "/Mass/DiJet_M_W",   WCand.M(),  w, 50, 0, 500, "Reco M_{jj}");


    int nSRbins=6;
    double mljbins[nSRbins] = { 0., 100.,200., 400.,  1000.,3000.};
    double MN1  = (N1Cand.M() > 2500.) ? 2499. : N1Cand.M();
    double MN2  = (N2Cand.M() > 2500.) ? 2499. : N2Cand.M();
    double MllW = (W1Cand.M() > 2500.) ? 2499. : W1Cand.M();

    if(DrawLevel3) FillHist( plot_dir+ region+ "/Mass/DiJet_M_l1W_M_W",         MN1, MllW,  w, 100, 0., 2000., 100, 0., 2000.);
    if(DrawLevel1) FillHist( plot_dir+ region+ "/Mass/DiJet_M_l1jj",   MN1,        w, 2500, 0, 2500, "Reco M_{l1jj}");
    if(DrawLevel1) FillHist( plot_dir+ region+ "/Mass/DiJet_M_l2jj",   MN2,        w, 2500, 0, 2500, "Reco M_{l2jj}");
    if(DrawLevel1) FillHist( plot_dir+ region+ "/Mass/DiJet_M_lljj",   MllW,        w, 2500, 0, 2500, "Reco M_{lljj}");
    if(DrawLevel1) FillHist( plot_dir+ region+ "/Mass/DiJet_M_l1W",             MN1,        w, 5, mljbins , "Reco M_{l1jj}");
    if(DrawLevel1) FillHist( plot_dir+ region+ "/Mass/DiJet_M_l2W",             MN2,        w, 5, mljbins , "Reco M_{l2jj} ");
    if(DrawLevel3) FillHist( plot_dir+ region+ "/Mass/DiJet_M_lAv12W", (N1Cand.M()+  N2Cand.M())/2.,  w,  500, 0., 2000., "Reco M_{l1_2jj} ");
    if(DrawLevel1) FillHist( plot_dir+ region+ "/Mass/DiJet_M_llW",  MllW ,  w, 5, mljbins , "Reco M_{lljj}");


    if(DrawLevel3) FillHist( plot_dir+ region+ "/DeltaR/DiJet_dR_Wj1_lep2",   jets[m].DeltaR(*leps[1] ),  w, 50, 0, 5, "#DeltaR (Wj1,lep2)");
    if(DrawLevel3) FillHist( plot_dir+ region+ "/DeltaR/DiJet_dR_Wj2_lep2",   jets[n].DeltaR(*leps[1] ),  w, 50, 0, 5, "#DeltaR (Wj2,lep2)");
  }// end dijet loop                                            

  double LT(0.);
  double minDRLep1Tau=9999.;
  double minDRLep2Tau=9999.;
  int nlep(0);
  for(auto i : leps){
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/CloseJet_BScore_"+i->GetEtaRegion("2bin"), i->CloseJet_BScore() ,  w, 200, -1, 1 );
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lepton_pt", i->PtMaxed(1000.)  , w, 1000, 0., 1000.,"1_{2} p_{T} GeV");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lepton_pt_unweighted", i->PtMaxed(1000.)  , 1, 1000, 0., 1000.,"1_{2} p_{T} GeV");
    if(DrawLevel2)FillHist( plot_dir+ region+ "/Leptons/Lepton_et", i->Et()  , w, 1000, 0., 1000.,"1_{2} p_{T} GeV");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lepton_eta",i->Eta()  , w, 50, -2.5, 2.5,"l_{1} #eta");
    if(DrawLevel2)FillHist( plot_dir+ region+ "/Leptons/Lepton_phi",i->Phi()  , w, 50, -2.5, 2.5,"l_{1} #phi");
    LT += i->Pt();
    //for(auto itau  : TauColl){
    //  if(nlep==0 && i->DeltaR(itau) < minDRLep1Tau) minDRLep1Tau = i->DeltaR(itau);
    //  if(nlep==1 && i->DeltaR(itau) < minDRLep2Tau) minDRLep2Tau = i->DeltaR(itau);
    //}
    nlep++;
  }
  if(DrawLevel3&&minDRLep1Tau < 999)FillHist( plot_dir+ region+ "/DeltaR/dRMin_Lep1_Tau", minDRLep1Tau  ,w, 50, 0, 5, "#DeltaR (Tau,lep1)");
  if(DrawLevel3&&minDRLep2Tau < 999)FillHist( plot_dir+ region+ "/DeltaR/dRMin_Lep2_Tau", minDRLep2Tau  ,w, 50, 0, 5, "#DeltaR (Tau,lep2)");

  int nPtbins=10;
  double Pt1bins[nPtbins+1] = { 20.,25.,30., 40.,50., 70., 100.,  150.,  200.,400.,600};
  double Pt2bins[nPtbins+1] = { 10.,15., 20.,30., 40.,50., 100.,120., 140., 160.,  200.};
  double PTLep1  = (leps[0]->Pt() > 500.) ? 499. : leps[0]->Pt();
  double PTLep2  = (leps[1]->Pt() > 300.) ? 299. : leps[1]->Pt();


  for(auto il : leps){
    double PTLep = (il->Pt() > 200.) ? 199. : il->Pt();
    TString LepType = (IsData) ? "Data" : il->sLepGenType();
    if (LepType == "") continue;
    
    if(DrawLevel3)FillHist( plot_dir+ region+ "/Leptons/"+LepType+"_Lep_pt", PTLep  ,  w, nPtbins, Pt1bins,"l_{1} p_{T} GeV");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/Leptons/"+LepType+"_Lep_eta", il->fEta()  , w, 60, 0.,  3.,"l_{2} #eta");
    
    map<TString, double> lep_bdt_map = il->MAPBDT();
    for(auto i : lep_bdt_map)  {
      if(!i.first.Contains("v5")) continue;
      if(DrawLevel2){
        if(il->IsBB())FillHist( plot_dir+lepregion+ "/"+LepType+"_Lepton_BB_mva_"+i.first , i.second, w, 100, -1., 1., "MVA");
        else FillHist( plot_dir+lepregion+ "/"+LepType+"_Lepton_EC_mva_"+i.first  , i.second, w, 100, -1., 1., "MVA");
      }
    }
  }

  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_1_Pt", PTLep1  ,  w, nPtbins, Pt1bins,"l_{1} p_{T} GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_2_Pt", PTLep2  ,  w, nPtbins, Pt2bins,"1_{2} p_{T} GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_1_pt", PTLep1  ,  w, 100, 0, 500,"l_{1} p_{T} GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_2_pt", PTLep2  ,  w, 150, 0, 300,"1_{2} p_{T} GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_1_eta", leps[0]->Eta()  , w, 60, -3., 3,"l_{1} #eta");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_2_eta", leps[1]->Eta()  , w, 60, -3., 3.,"l_{2} #eta");
  if(leps.size()  >= 3) {
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_3_pt", leps[2]->PtMaxed(1000.)  , w, 200, 0., 1000.,"l_{3} p_{T} GeV");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_3_eta", leps[2]->Eta()  , w, 60, -3., 3.,"l_{3} #eta");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_3_phi", leps[2]->Phi()  , w, 60, -3., 3.,"l_{3} #phi");
  }
  if(leps.size()  >= 4) {
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_4_pt", leps[3]->PtMaxed(1000.)  , w, 200, 0., 1000.,"l_{4} p_{T} GeV");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_4_eta", leps[3]->Eta()  , w, 60, -3., 3.,"l_{4} #eta");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Lep_4_phi", leps[3]->Phi()  , w, 60, -3., 3.,"l_{4} #phi");
  }
  if(DrawLevel3)FillHist( plot_dir+ region+ "/SKEvent/Vertex_X", vertex_X  , w, 200, 0., 1,"L_{T} GeV");
  if(DrawLevel3)FillHist( plot_dir+ region+ "/SKEvent/Vertex_Y", vertex_Y  , w, 200, 0., 1,"L_{T} GeV");
  if(DrawLevel3)FillHist( plot_dir+ region+ "/SKEvent/Vertex_Z", vertex_Z  , w, 200, 0., 1,"L_{T} GeV");

  double HT = GetHT(jets, fatjets);
  double XT = HT+LT;
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/Ev_LT", LT  , w, 200, 0., 2000.,"L_{T} GeV");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/Ev_HT", HT  , w, 200, 0., 2000.,"H_{T} GeV");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/Ev_XT", XT  , w, 200, 0., 2000.,"X_{T} GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/SKEvent/HToLepPt1", HT/ leps[0]->Pt()  , w, 100, 0., 20.,"HT/PT(1)");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/SKEvent/XToLepPt1", XT/ leps[0]->Pt()  , w, 100, 0., 20.,"XT/PT(1)");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/HToLepPt2", HT/ leps[1]->Pt()  , w, 100, 0., 20.,"HT/PT(2)");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/XToLepPt2", XT/ leps[1]->Pt()  , w, 100, 0., 20.,"XT/PT(2)");
  if(threelep&&DrawLevel3)FillHist( plot_dir+ region+ "/SKEvent/HToLepPt3", HT/ leps[2]->Pt()  , w, 100, 0., 20.,"HT/PT(3)");
  if(fourlep&&DrawLevel3)FillHist( plot_dir+ region+ "/SKEvent/HToLepPt4", HT/ leps[3]->Pt()  , w, 100, 0., 20.,"HT/PT(4)");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/nPV",  nvtx , w, 120, 0., 120.);
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/nPileUp",  nPileUp, w, 120, 0., 120.);

  double ST = GetST(leps, jets, fatjets, met);
  double met2_st = pow(met.Pt(),2.)/ ST;

  if(DrawLevel1)FillHist( plot_dir+ region+ "/SKEvent/Ev_ST", ST  , w, 250, 0., 5000.,"ST GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/SKEvent/Ev_MET2_ST", met2_st  , w, 100, 0., 100.,"MET2/ST GeV");

  Particle METv           = GetMiniAODvMET("T1xyCorr");
  Particle PuppiMETv      = GetMiniAODvMET("PuppiT1xyCorr");
  Particle METvNoPhi      = GetMiniAODvMET("T1");
  Particle PuppiMETvNoPhi = GetMiniAODvMET("PuppiT1");
  Particle PuppiMETvULPhiCorr = GetMiniAODvMET("PuppiT1xyULCorr");
  Particle METvULPhiCorr  = GetMiniAODvMET("T1xyULCorr");

  if(DrawLevel1)FillHist( plot_dir+ region+ "/SKEvent/Ev_MET", met.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_pfMET_T1", METvNoPhi.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_pfMET_T1xyCorr", METv.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/Ev_PuppiMET_T1", PuppiMETvNoPhi.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_PuppiMET_T1xyCorr", PuppiMETv.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/Ev_PuppiMET_T1ULxyCorr", PuppiMETvULPhiCorr.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_pfMET_T1ULxyCorr", METvULPhiCorr.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_pfMETphi_T1", METvNoPhi.Phi()  , w, 200, -5., 5.,"MET #phi");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_pfMETphi_T1xyCorr", METv.Phi()  , w, 200, -5., 5.,"MET #phi");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_PuppiMETphi_T1", PuppiMETvNoPhi.Phi()  , w, 200, -5., 5.,"MET #phi");
  if(param.PlottingVerbose == 3)FillHist( plot_dir+ region+ "/SKEvent/Ev_PuppiMETphi_T1xyCorr", PuppiMETv.Phi()  , w, 200, -5., 5.,"MET #phi");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/SKEvent/Mt_lep1", MT(*leps[0] ,met)  , w, 200, 0., 400.,"MT GeV");
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/Mt_lep2", MT(*leps[1] ,met)  , w, 200, 0., 400.,"MT GeV");
  if(threelep) {
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Mass/M_lll", lllCand.M() , w, 200, 0., 800.,"M(lll) GeV");
    if(GetIndexNonMinOSSF(leps) > 0 && DrawLevel1)FillHist( plot_dir+ region+ "/SKEvent/Mt_minOSSF", MT(*leps[GetIndexNonMinOSSF(leps)], met) , w, 200, 0., 800.,"M(T) non MinOS GeV");
    if(GetIndexNonBestZ(leps,M_ZWINDOW)> 0 && DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/Mt_nonZLep", MT(*leps[GetIndexNonBestZ(leps,M_ZWINDOW)], met) , w, 200, 0., 800.,"M(T) non Z GeV");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Mass/M_minOSSF", GetMassMinOSSF(leps), w, 200, 0., 800.,"M non MinOS GeV");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Mass/M_minSSSF", GetMassMinSSSF(leps), w, 200, 0., 800.,"M(T) non MinSS GeV");
    if(GetIndexNonMinSSSF(leps) > 0 && DrawLevel1)FillHist( plot_dir+ region+ "/Mass/Mt_minSSSF", MT(*leps[GetIndexNonMinSSSF(leps)], met) , w, 200, 0., 800.,"M non MinSS GeV");
  }

  if(fourlep) {
    Particle llllCand = *leps[0] + *leps[1] + *leps[2] + *leps[3] ;
    if(llllCand.M() > 70 && llllCand.M() < 170)     FillHist( plot_dir+ region+ "/Mass/M_ZZ", llllCand.M() , w, 100, 70., 170.,"M_{4l} (GeV)");
    FillHist( plot_dir+ region+ "/Mass/M_llll", llllCand.M() , w, 200, 0., 800.,"M(llll) GeV");
    FillHist( plot_dir+ region+ "/Mass/M_BestZ", LeptonMassBestZ(leps,LeptonPairBestZCand(leps)) , w, 200, 0., 800.,"M(Z1) GeV");
    FillHist( plot_dir+ region+ "/Mass/M_OtherZ", LeptonMassNonZ(leps,LeptonPairBestZCand(leps)) , w, 200, 0., 800.,"M(Z1) GeV");
    FillHist( plot_dir+ region+ "/Mass/M_BestZAlt", GetMassBestZ(leps,true) , w, 200, 0., 800.,"M(Z1) GeV");
  }

  Event ev = GetEvent();
  Particle METunsmearedv = ev.GetMETVector();
  
  if(DrawLevel2)FillHist( plot_dir+ region+ "/SKEvent/MET_uncorr", METunsmearedv.Pt()  , w, 200, 0., 400.,"MET GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/Mass/M_LL",  llCand.M(), w, 400, 0., 2000., "M_{ll} GeV");
  if(DrawLevel1)FillHist( plot_dir+ region+ "/Mass/M_ll",  llCand.M(), w, 2000, 0., 2000., "M_{ll} GeV");

  double mindRlepj1(99999.);
  double mindRlepj2(99999.);
  int nBJet =0;
  JetTagging::Parameters JPForPlots = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Medium, JetTagging::incl, JetTagging::mujets);

  for(unsigned int i=0; i < jets.size(); i++){
    if( jets[i].GetTaggerResult(JPForPlots.j_Tagger) > mcCorr->GetJetTaggingCutValue(JPForPlots.j_Tagger, JPForPlots.j_WP) ) nBJet++;
    
    
    if(i == 0){
      if(DrawLevel1)FillHist( plot_dir+ region+ "/AK4Jets/Jet_1_pt",  jets[i].Pt() , w, 400, 0., 2000., "AK4 Jet p_{T} GeV");
      if(DrawLevel1)FillHist( plot_dir+ region+ "/AK4Jets/Jet_1_eta", jets[i].Eta() , w, 100, -5., 5., "AK4 Jet #eta ");
    }
    if(i == 1){
      if(DrawLevel1)FillHist( plot_dir+ region+ "/AK4Jets/Jet_2_pt",  jets[i].Pt() , w, 400, 0., 2000., "AK4 Jet p_{T} GeV");
      if(DrawLevel1)FillHist( plot_dir+ region+ "/AK4Jets/Jet_2_eta", jets[i].Eta() , w, 100, -5., 5., "AK4 Jet #eta ");
    }
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/pileup_mva",  jets[i].PileupJetId() , w, 100, -1., 1., "PileupJetId");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/pileup_loose", jets[i].PassPileupMVA("Loose", GetEraShort()), w, 2, 0., 2., "PileupJetId");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/pileup_medium", jets[i].PassPileupMVA("Medium", GetEraShort()), w, 2, 0., 2., "PileupJetId");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/pileup_tight", jets[i].PassPileupMVA("Tight", GetEraShort()), w, 2, 0., 2., "PileupJetId");
    if(DrawLevel2)FillHist( plot_dir+ region+ "/AK4Jets/DeepCSV_score", jets[i].GetTaggerResult(JetTagging::DeepCSV), w, 100, 0, 1., "DeepCSV_score");
    if(DrawLevel2)FillHist( plot_dir+ region+ "/AK4Jets/DeepJet_score", jets[i].GetTaggerResult(JetTagging::DeepJet), w, 100, 0, 1., "DeepCSV_score");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/Jet_pt",  jets[i].Pt() , w, 400, 0., 2000., "AK4 Jet p_{T} GeV");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/Jet_eta",  jets[i].Eta() , w, 100, -5., 5., "AK4 Jet #eta ");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/Jet_CEEF", jets[i].ChargedEmEnergyFraction(), w, 50, 0., 1. ,"");
    if(DrawLevel3)FillHist( plot_dir+ region+ "/AK4Jets/Jet_CHEF", jets[i].ChargedHadEnergyFraction(), w, 50, 0., 1. ,"");
    if(jets[i].DeltaR(*leps[0] ) < mindRlepj1) mindRlepj1=jets[i].DeltaR(*leps[0] );
    if(jets[i].DeltaR(*leps[1] ) < mindRlepj2) mindRlepj2=jets[i].DeltaR(*leps[1] );
    if(i == 0){
      if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/dR_jet1_lep1",  jets[i].DeltaR(*leps[0] ) , w, 50, 0., 5., "#Delta R(l1,j)") ;
      if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/dR_jet1_lep2",  jets[i].DeltaR(*leps[1] ) , w, 50, 0., 5., "#Delta R(l2,j)");
    }
    if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/dR_jet_lep1",  jets[i].DeltaR(*leps[0] ) , w, 50, 0., 5., "#Delta R(l1,j)") ;
    if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/dR_jet_lep2",  jets[i].DeltaR(*leps[1] ) , w, 50, 0., 5., "#Delta R(l2,j)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaR/dR_jet_MET",  jets[i].DeltaR(met) ,w, 50,  0., 5,"#DeltaR(j,met)");
    for(unsigned int j=1; j < jets.size(); j++){
      if(i==j)continue;
      if(DrawLevel1)FillHist( plot_dir+ region+"/DeltaR/Jet_dR_jj",  jets[i].DeltaR(jets[j]) ,w, 50,  0., 5,"#DeltaR(j,j)");
      if(DrawLevel3)FillHist( plot_dir+ region+"/Mass/Jet_M_jj",  (jets[i]+jets[j]).M() ,w, 200,  0., 2000,"M(j,j)");
    }
  }
  if(DrawLevel1)FillHist( plot_dir+ region+ "/NObj/N_BJet",  nBJet , w, 5, 0., 5., "PileupJetId");


  int nVBFMJJbins=4;
  double MJJbins[nVBFMJJbins+1] = { 500,750,1200,1700,3000};


  if(jets.size() > 1){
    double maxDiJetDeta=0.;
    int ijet1(-1), ijet2(-1);
    for(unsigned int ij = 0; ij < jets.size()-1; ij++){
      for(unsigned int ij2 = ij+1; ij2 < jets.size(); ij2++){

        double deta = fabs(jets[ij].Eta() - jets[ij2].Eta());
        if(deta > maxDiJetDeta) {
          maxDiJetDeta=deta;
          ijet1=ij;
          ijet2=ij2;
        }
      }
    }
    if(DrawLevel3)FillHist( plot_dir+ region+"/VBF/Jet_dR_jet1_lep1",  jets[ijet1].DeltaR(*leps[0]) ,w, 100,  0., 5,"#DeltaR(j,l1)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/VBF/Jet_dR_jet1_lep2",  jets[ijet1].DeltaR(*leps[1]) ,w, 100,  0., 5,"#DeltaR(j,l1)");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/VBF/MaxDEta_jet1_jet2", maxDiJetDeta  , w, 200, 0., 10., "Max DEta");
    Particle JJMEta = jets[ijet1] + jets[ijet2];
    if(DrawLevel1)FillHist( plot_dir+ region+ "/VBF/MaxDEtaJets_MJJ",JJMEta.M()   , w, nVBFMJJbins, MJJbins, "MaxDEta MJJ");
    double Av_JetEta= 0.5*(jets[ijet1].Eta()+ jets[ijet2].Eta());

    double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);
    if(DrawLevel1)FillHist( plot_dir+ region+ "/VBF/MaxDEta_Jets_zeppenfeld", zeppenfeld  , w, 200, 0., 10., "zeppenfeld");
  }

  if(jets.size()>1){
    Particle JJLead = jets[0] + jets[1];
    if(DrawLevel1)FillHist( plot_dir+ region+ "/VBF/Lead_MJJ",JJLead.M()   , w, nVBFMJJbins, MJJbins, "Lead MJJ");
    double maxDiJetDeta=fabs(jets[0].Eta() - jets[1].Eta());
    double Av_JetEta= 0.5*(jets[0].Eta()+ jets[1].Eta());
    double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);
    if(DrawLevel1)FillHist( plot_dir+ region+ "/VBF/Lead_zeppenfeld", zeppenfeld  , w, 100, 0., 2., "zeppenfeld");
    if(DrawLevel1)FillHist( plot_dir+ region+ "/VBF/Lead_DEta_jet1_jet2", maxDiJetDeta  , w, 200, 0., 10., "Max DEta");
  }

  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
  double ll_deta = fabs((*leps[0]).Eta() - (*leps[1]).Eta());
  if(DrawLevel1)FillHist( plot_dir+ region+ "/DeltaPhi/dPhi_lep1_lep2", ll_dphi  , w, 200, -5., 5., "#Delta #Phi(l1,l2)") ;
  if(DrawLevel1)FillHist( plot_dir+ region+ "/DeltaEta/dEta_lep1_lep2", ll_deta  , w, 200, -5., 5., "#Delta #Phi(l1,l2)") ;
  if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/dRmin_jet_lep1", mindRlepj1  , w, 50, 0., 5., "#Delta R(l1,j)") ;
  if(DrawLevel3)FillHist( plot_dir+ region+ "/DeltaR/dRmin_jet_lep2", mindRlepj2  , w, 50, 0., 5., "#Delta R(l1,j)") ;
  if(DrawLevel3)FillHist( plot_dir+ region+ "/Leptons/Sum_lep_eta", (fabs(leps[0]->Eta())+  fabs(leps[1]->Eta())) , w, 50, 0., 5.);
  if(fabs(leps[0]->Eta()) > fabs(leps[1]->Eta())) {
    if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Max_lep_eta", fabs(leps[0]->Eta()) , w, 50, 0., 2.5);
  }
  else  if(DrawLevel1)FillHist( plot_dir+ region+ "/Leptons/Max_lep_eta", fabs(leps[1]->Eta()) , w, 50, 0., 2.5);

  if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaR/dR_l1_MET",  leps[0]->DeltaR(met) ,w, 100,  0., 5,"#DeltaR(l1,met)");
  if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaR/dR_l2_MET",  leps[1]->DeltaR(met) ,w, 100,  0., 5,"#DeltaR(l2,met)");
  if(DrawLevel1)FillHist( plot_dir+ region+"/DeltaR/dR_ll", leps[0]->DeltaR(*leps[1] ) ,w, 50,  0., 5,"#DeltaR(l,l)");
  if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaPhi/dPhi_l1_MET",  fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (met).Phi() )) ) ,w, 100,  0., 5,"#DeltaR(l1,met)");
  if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaPhi/dPhi_l2_MET",  fabs(TVector2::Phi_mpi_pi( ( (*leps[1]).Phi() - (met).Phi() )) ) ,w, 100,  0., 5,"#DeltaR(l1,met)");

  if(leps[1]->Pt() < 100){
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaR/dR_lN_MET",  leps[1]->DeltaR(met) ,w, 100,  0., 5,"#DeltaR(l1,met)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaPhi/dPhi_lN_MET",  fabs(TVector2::Phi_mpi_pi( ( (*leps[1]).Phi() - (met).Phi() )) ) ,w, 100,  0., 5,"#DeltaR(l1,met)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaR/dR_lW_MET",  leps[0]->DeltaR(met) ,w, 100,  0., 5,"#DeltaR(l1,met)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaPhi/dPhi_lW_MET",  fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (met).Phi() ))) ,w, 100,  0., 5,"#DeltaR(l1,met)");
  }
  else{
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaR/dR_lN_MET",  leps[0]->DeltaR(met) ,w, 100,  0., 5,"#DeltaR(l1,met)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaPhi/dPhi_lN_MET",  fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (met).Phi() ))) ,w, 100,  0., 5,"#DeltaR(l1,met)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaR/dR_lW_MET",  leps[1]->DeltaR(met) ,w, 100,  0., 5,"#DeltaR(l1,met)");
    if(DrawLevel3)FillHist( plot_dir+ region+"/DeltaPhi/dPhi_lW_MET",  fabs(TVector2::Phi_mpi_pi( ( (*leps[1]).Phi() - (met).Phi() ))) ,w, 100,  0., 5,"#DeltaR(l1,met)");

  }
  return;
}






/// FillMuonPlots Fills all kinamatics                                                                                                                                                                                                                                          
void HNL_LeptonCore::FillMuonCollPlots(bool passSel, TString SelSt, AnalyzerParameter param, TString cut,  std::vector<Muon> muons, double w){

  if(!DrawSyst(param.syst_)) return;

  for(auto imu: muons)   FillMuonPlots(passSel, SelSt, param, cut, imu, w);

  return;
}
void HNL_LeptonCore::FillMuonCollPlots(AnalyzerParameter param, TString cut,  std::vector<Muon> muons, double w){

  if(!DrawSyst(param.syst_)) return;

  for(auto imu: muons)   FillMuonPlots(true, "", param, cut, imu, w);
  return;
}

/// FillAllMuonPlots Fills all kinamatics in pt/eta bins                                                                              
void HNL_LeptonCore::FillMuonPlots(AnalyzerParameter param, TString cut,  Muon muon, double w){
  if(!DrawSyst(param.syst_)) return;
  FillMuonPlots(true, "", param, cut, muon, w);
}                                                                                                                                      
    

void HNL_LeptonCore::FillMuonPlots(bool passSel, TString SelSt, AnalyzerParameter param, TString cut,  Muon muon, double w){
  
  if(!DrawSyst(param.syst_)) return;

  if(!passSel) return;

  cut = "MuonPlots/"+cut;

  if(SelSt != "") {
    cut = cut + "_"+SelSt;
    TString Option = ""; 
    if(param.SplitPlot == "PtBin_Type_")    FillMuonKinematicPlots(param,cut +"/"+muon.GetPtLabel() +"_"+muon.sLepGenType(), muon, w);
    if(param.SplitPlot.Contains("PtBin_"))  FillMuonKinematicPlots(param,cut +"/"+muon.GetPtLabel() ,  muon, w);
    if(param.SplitPlot.Contains("Type_"))   FillMuonKinematicPlots(param,cut +"/"+muon.sLepGenType(),  muon, w);
  }
    
  else FillMuonKinematicPlots(param, cut, muon, w);
  
  return;
}



void HNL_LeptonCore::FillLeptonKinematicPlots(AnalyzerParameter param, TString cut,  Lepton lep, double w){

  if(!DrawSyst(param.syst_)) return;

  vector<Jet> JetAllColl = All_Jets;

  int IdxMatchJet=-1;
  int IdxMatchAwayJet=-1;

  double mindR(999.);
  double maxDphi=-999;
  double PtRatioAwayJet(-999.);
  double Jet_Disc(-1);
  
  TString label = param.hpostfix;

  for(unsigned int ij=0; ij<JetAllColl.size(); ij++){

    double dR=lep.DeltaR(JetAllColl.at(ij));
    if(dR<mindR){       mindR=dR; IdxMatchJet=ij;     }
    double dphi =fabs(TVector2::Phi_mpi_pi(lep.Phi()- JetAllColl.at(ij).Phi()));
    if(dphi > maxDphi) {maxDphi= dphi; IdxMatchAwayJet=ij;}
  }
  if(IdxMatchJet!=-1)     Jet_Disc       = JetAllColl.at(IdxMatchJet).GetTaggerResult(JetTagging::DeepJet);
  if(IdxMatchAwayJet!=-1) PtRatioAwayJet = JetAllColl.at(IdxMatchAwayJet).Pt()/ lep.Pt();

  TString label_lep = (lep.LeptonFlavour()==Lepton::ELECTRON) ? "Electron": "Muon";
  double Pt_Lep = (lep.Pt() > 2000) ? 1999 : lep.Pt();

  label += label_lep;

  /// Kinematics                                                                                                                                                                                                                                                                
  FillHist( cut+ "/Lepton_Pt_"+label   , lep.Pt() , w, 5000, 0., 10000., label_lep+"p_{T} GeV");
  FillHist( cut+ "/Lepton_Eta_"+label  , lep.Eta() , w, 60, -3., 3.,label_lep+"#eta");
  FillHist( cut+ "/Lepton_Phi_"+label  , lep.Phi() , w, 60, -3., 3.,label_lep+"#phi");
  double ptbins[14] = { 0., 10.,15., 20., 30., 40.,50., 100.,200.,300.,400.,500. ,1000.,2000.};
  FillHist( cut+ "/Lepton_PtBinned_"+label   , Pt_Lep, w, 11, ptbins);

  // IP                                                                                                                                                                                                                                                                         
  FillHist( cut+ "/Lepton_Dxy_"+label           , lep.fdXY() , w, 250,  0, 0.2, "dXY");
  FillHist( cut+ "/Lepton_Dz_"+label            , lep.fdZ() , w, 250, 0, 0.5, "dZ");
  FillHist( cut+ "/Lepton_LogDxy_"+label        , lep.LogdXY() , w, 300, -20,  10, "dXY");

  FillHist( cut+ "/Lepton_dXYSig_"+ label       , lep.LogdXYSig(), w, 100, -10,10);
  FillHist( cut+ "/Lepton_dZSig_"+ label        , lep.LogdZSig(),w, 100,-10,10);
  FillHist( cut+ "/Lepton_SIP3D_"+ label        , lep.SIP3D(),w, 200,0,20);
  FillHist( cut+ "/Lepton_IP3D_"+ label        , lep.IP3D(),w, 100,0,1);


  // Lep/Jet                                                                                                                                                                                                                                                                    
  FillHist( cut+ "/Lepton_PtRel_"+label         , JetLeptonPtRelLepAware( lep), w, 500, 0., 100., "");
  FillHist( cut+ "/Lepton_PtRatio_"+label       , JetLeptonPtRatioLepAware(lep), w, 500, 0., 2., "");

  double MotherPt = lep.Pt() /  lep.CloseJet_Ptratio();
  if(MotherPt > 200) MotherPt=200;
  FillHist( cut+ "/Lepton_MotherJetPt_"+label,  MotherPt, w, 100, 0., 200);
  FillHist( cut+ "/Lepton_MotherFlavour_"+label , lep.CloseJet_FlavourInt(), w, 10, 0., 10);

  if(IdxMatchJet != -1 ){
    FillHist( cut+ "/Lepton_CHFracCJ_"+label      , JetAllColl.at(IdxMatchJet).ChargedHadEnergyFraction(), w, 100, 0., 1., "");
    FillHist( cut+ "/Lepton_NEMFracCJ_"+label     , JetAllColl.at(IdxMatchJet).NeutralEmEnergyFraction(), w, 100, 0., 1., "");
    FillHist( cut+ "/Lepton_CEMFracCJ_"+label     , JetAllColl.at(IdxMatchJet).ChargedEmEnergyFraction(), w, 100, 0., 1., "");
    FillHist( cut+ "/Lepton_NFracCJ_"+label       , JetAllColl.at(IdxMatchJet).NeutralHadEnergyFraction(), w, 100, 0., 1., "");
    FillHist( cut+ "/Lepton_JetDiscCJ_"+label     , Jet_Disc , w, 400, -2., 2., "");
    FillHist( cut+ "/Lepton_PtRatioAwayJet_"+label, PtRatioAwayJet , w, 100, 0., 5., "");
  }

  map<TString, double> lep_bdt_map = lep.MAPBDT();
  for(auto i : lep_bdt_map){
    FillHist( cut+ "/Lepton_mva_"+i.first+label  , i.second, w, 100, -1., 1., "MVA");
    FillHist( cut+ "/Lepton_MotherJetPtVsMVA_"+label,  lep.CloseJet_Ptratio(), i.second, w, 100, 0., 2., 100, -1., 1.);
    FillHist( cut+ "/Lepton_RelIsoVsMVA_"+label,  lep.RelIso() ,  i.second, w, 200, 0., 0.4, 100, -1., 1);
  }

  // Isolation                                                                                                                                                                                                                                                              
  FillHist( cut+ "/Lepton_MotherJetPtVRelIso_"+label,  lep.CloseJet_Ptratio(), lep.RelIso(),  w, 100, 0., 2., 200, 0., 0.4);
  FillHist( cut+ "/Lepton_MiniReliso_"+label , lep.MiniRelIso() , w, 500, 0., 2., "R_{ISO} GeV");
  FillHist( cut+ "/Lepton_Reliso_el_"+label , lep.RelIso() , w, 200, 0., 0.4, "R_{ISO} GeV");

}


void HNL_LeptonCore::FillMuonKinematicPlots(AnalyzerParameter param, TString cut,  Muon lep, double w){

  if(!DrawSyst(param.syst_)) return;

  FillLeptonKinematicPlots(param, cut, Lepton(lep),w);

  TString label = param.hpostfix;
  
  FillHist( cut+ "/Muon_Mva_"+label    , lep.MVA(), w, 600, -1., 1., "MVA");
  FillHist( cut+ "/Muon_Chi2_"+label   , lep.Chi2(), w, 200,0., 20., "chi2");
  FillHist( cut+ "/Muon_Validhits_"+label  , lep.ValidMuonHits(), w, 100,0., 100., "");
  FillHist( cut+ "/Muon_Matched_stations_"+label  , lep.MatchedStations(), w, 10,0., 10., "");
  FillHist( cut+ "/Muon_Pixel_hits_"+label  , lep.PixelHits(), w, 10,0., 10., "");
  FillHist( cut+ "/Muon_Tracker_layers_"+label  , lep.TrackerLayers(), w, 50,0., 50., "");

  vector<TString> IDs ={"HNTightV2","POGLoose","POGMedium","POGTight"};
  for (auto ID : IDs){
    if(lep.PassID(ID)) FillHist( cut+ "/Muon_Pass_"+ID+label  , 1, w, 4, 0., 4., "Pass " + ID);
    else FillHist( cut+ "/Muon_Pass_"+ID+label  , 0, w, 4, 0., 4., "Pass " + ID);

    if(lep.PassID(ID)) FillHist( cut+ "/Muon_Pass_"+ID+label  , 1, w, 4, 0., 4., "Pass " + ID);
    else FillHist( cut+ "/Muon_Pass_"+ID+label  , 0, w, 4, 0., 4., "Pass " + ID);

  }

  return;
}



void HNL_LeptonCore::FillElectronPlots(AnalyzerParameter param , TString cut,  std::vector<Electron> ElectronColl, double w){

  if(!DrawSyst(param.syst_)) return;

  for(auto iel : ElectronColl)   FillElectronKinematicPlots(param, cut, iel, w);

  return;
}

void HNL_LeptonCore::FillAllElectronPlots(AnalyzerParameter param , TString cut,  std::vector<Electron> ElectronColl, double w){

  if(!DrawSyst(param.syst_)) return;

  TString label  = param.hpostfix;


  FillHist( cut+ "/NElectrons"+label , size(ElectronColl) , w, 5, 0., 5., "n_{el}");

  for(unsigned int i=0; i < ElectronColl.size(); i++){

    TString eta_label="";
    if(fabs(ElectronColl.at(i).Eta()) < 1.5) eta_label = "_BB";
    else eta_label = "_EC";

    TString pt_label=GetPtBin(false,ElectronColl.at(i).Pt());

    int LepType= ElectronColl.at(i).LeptonGenType();
    TString gen_label ="";
    if (LepType >= 0) gen_label = to_string(LepType);
    else gen_label = "Minus_"+to_string(fabs(LepType));

    if(Analyzer== "HNL_LeptonIDBDTStudies"){

      if(GenTypeMatched(MatchGenDef(All_Gens,ElectronColl.at(i)))){
  
        if(label.Contains("Fake")) FillElectronKinematicPlots( param,"Electron_"+label+"_"+ ElectronColl.at(i).CloseJet_Flavour()+"_"+MatchGenDef(All_Gens,ElectronColl[i])+"_"+ cut, ElectronColl.at(i), w);
      }
    }
    FillElectronKinematicPlots(param, "Electron_"+label+"_"+ ElectronColl.at(i).CloseJet_Flavour() + cut, ElectronColl.at(i), w);
  }
  return;
}

void HNL_LeptonCore::FillElectronKinematicPlots(AnalyzerParameter param , TString cut,  Electron lep, double w){

  if(!DrawSyst(param.syst_)) return;
  
  FillLeptonKinematicPlots(param, cut, Lepton(lep),w);

  TString label = param.hpostfix;

  FillHist( cut+ "/Electron_SCEta_"+label  , lep.scEta() , w, 60, -3.,  3., "electron #eta");
  FillHist( cut+ "/Electron_SCPhi_"+label  , lep.scPhi() , w, 70, -3.5, 3.5,"electron #phi");

  /// POG MVA                                                                                                                                                                                                                                                                   
  FillHist( cut+ "/Electron_NoIsoMva_"           +label  , lep.MVANoIso(),                  w, 600, -1., 1., "MVA");
  FillHist( cut+ "/Electron_RelNoIsoMvaResponse_"+label  , lep.MVANoIsoResponse()/lep.Pt(), w, 160, -1.,1., "MVA");
  FillHist( cut+ "/Electron_NoIsoMvaResponse_"   +label  , lep.MVANoIsoResponse(),          w, 160, -8.,8., "MVA");
  FillHist( cut+ "/Electron_MvaIso_"             +label  , lep.MVAIso(),                    w, 600, -1., 1., "MVA");
  FillHist( cut+ "/Electron_RelMvaIsoResponse_"  +label  , lep.MVAIsoResponse()/lep.Pt(),   w, 600, -1., 1., "MVA");
  FillHist( cut+ "/Electron_MvaIsoResponse_"     +label  , lep.MVAIsoResponse(),            w, 600, -8., 8., "MVA");

  /// CF MVA                                                                                                                                                                                                                                                                    
  FillHist( cut+ "/Electron_EoverP_"       +label  , lep.EOverP() , w, 400, -10, 100);
  FillHist( cut+ "/Electron_LogEoverP_"    +label  , log(lep.EOverP()) , w, 400, -1, 10);

  FillHist( cut+ "/Electron_FBrem_"        +label  , std::max(lep.FBrem(),-2.),  w, 200, -1., 1);
  FillHist( cut+ "/Electron_R9_"           +label  , lep.R9(), w, 100, 0., 1);
  FillHist( cut+ "/Electron_dr03TkSumPt_"  +label  , lep.dr03TkSumPt()/lep.UncorrPt(), w, 200, 0., 0.5, "");
  FillHist( cut+ "/Electron_E15_"          +label  , lep.e15(), w, 1000, 0., 1000);
  FillHist( cut+ "/Electron_E25_"          +label  , lep.e25(), w, 1000, 0., 1000);
  FillHist( cut+ "/Electron_E55_"          +label  , lep.e55(), w, 1000, 0., 1000);
  FillHist( cut+ "/Electron_e2x5OverE5x5_" +label  , lep.e2x5OverE5x5(), w, 100, 0., 1);
  FillHist( cut+ "/Electron_e1x5OverE5x5_" +label  , lep.e1x5OverE5x5(), w, 100, 0., 1);
  FillHist( cut+ "/Electron_EtaWidth_"+label  , lep.EtaWidth(), w, 100, 0., 0.2);
  FillHist( cut+ "/Electron_PhiWidth_"+label  , lep.PhiWidth(), w, 200, 0., 0.2);

  FillHist( cut+ "/Electron_MissingHits_"+label  , lep.NMissingHits(), w, 8, 0., 8., "Missing Hits");
  FillHist( cut+ "/Electron_Full5x5_sigmaIetaIeta_"+label  , lep.Full5x5_sigmaIetaIeta(), w, 200, 0., 0.05, "");
  FillHist( cut+ "/Electron_PassConversionVeto_"+label  , lep.PassConversionVeto(), w, 2, 0., 2., "");
  FillHist( cut+ "/Electron_IsGsfCtfScPixChargeConsistent_"+label  , lep.IsGsfCtfScPixChargeConsistent(), w, 2, 0., 2., "");
  FillHist( cut+ "/Electron_IsGsfScPixChargeConsistent_"+label  , lep.IsGsfScPixChargeConsistent(), w, 2, 0., 2., "");
  FillHist( cut+ "/Electron_IsGsfCtfChargeConsistent_"+label  , lep.IsGsfCtfChargeConsistent(), w, 2, 0., 2., "");
  FillHist( cut+ "/Electron_dEtaSeed_"+label  , lep.dEtaSeed(), w, 200, -0.1, 0.1, "");
  FillHist( cut+ "/Electron_dPhiIn_"+label  , lep.dPhiIn(), w, 400, -0.5, 0.5, "");
  FillHist( cut+ "/Electron_dEtaIn_"+label  , lep.dEtaIn(), w, 400, -0.2, 0.2, "");
  FillHist( cut+ "/Electron_HoverE_"+label  , lep.HoverE(), w, 500, 0., 1., "");
  FillHist( cut+ "/Electron_TrkIso_"+label  , lep.TrkIso(), w, 1000, 0., 100, "");
  FillHist( cut+ "/Electron_isEcalDriven_"+label  , lep.isEcalDriven(), w, 2, 0., 2., "");
  FillHist( cut+ "/Electron_InvEminusInvP_"+label  , fabs(lep.InvEminusInvP()), w, 100, 0., 0.2);


  // COnv MVA                                                                                                                                                                                                                                                                   
  FillHist( cut+ "/Electron_dr03HcalTowerSumEt_"+label  , lep.dr03HcalTowerSumEt(), w, 200, 0., 10., "");
  /// Extra                                                                                                                                                                                                                                                                     
  FillHist( cut+ "/Electron_dr03EcalRecHitSumEt_"+label  , lep.dr03EcalRecHitSumEt(), w, 2000, 0., 100., "");
  FillHist( cut+ "/Electron_dr03HcalDepth1TowerSumEt_"+label  , lep.dr03HcalDepth1TowerSumEt(), w, 200, 0., 10., "");
  FillHist( cut+ "/Electron_ecalPFClusterIso_"+label  , lep.ecalPFClusterIso()/lep.UncorrPt(), w, 100, 0., 1., "");
  FillHist( cut+ "/Electron_hcalPFClusterIso_"+label  , lep.hcalPFClusterIso()/lep.UncorrPt(), w, 100, 0., 1., "");

  vector<TString> IDs ={"HNTightV2","passHEEPID_v1","passHEEPID_v2","passHEEPID_v3","HNL_ULID","passPOGTight","HNL_ULID_HEEP"};
  for (auto ID : IDs){
    if(lep.PassID(ID)) FillHist( cut+ "/Electron_Pass_"+ID+label  , 1, w, 4, 0., 4., "Pass " + ID);
    else FillHist( cut+ "/Electron_Pass_"+ID+label  , 0, w, 4, 0., 4., "Pass " + ID);
  }

  return;


}

void HNL_LeptonCore::Fill_SigRegionPlots4(HNL_LeptonCore::Channel channel, TString label, std::vector<Jet> jets, std::vector<FatJet> fatjets,  std::vector<Lepton *> leps, Particle  met, double nvtx,  double w){


  if(!CheckLeptonFlavourForChannel(channel, leps)) return;

  double HT(0.);
  for(auto ij : jets) HT += ij.Pt();

  //double ST += lep1.Pt() + lep2.Pt() + met.Pt();                                                                                                                                                                                     

  int NJet = jets.size();
  int bin = 0;
  double mass_lll = (*leps[0]+ *leps[1]+ *leps[2]).M();
  double LToHT=HT/(leps[0]->Pt()+leps[1]->Pt()+leps[2]->Pt());

  if(LToHT < 10.){
    if(mass_lll < 50){
      if(NJet<=0) bin=1;
      else bin=2;
    }
    else{
      if(NJet<=0) bin=3;
      else bin=4;
    }
  }
  else {
    if(mass_lll < 50){
      if(NJet<=0) bin=5;
      else bin=6;
    }
    else{
      if(NJet<=0) bin=7;
      else bin=8;
    }

  }
  FillHist( label + "/LimitInput_HNL_SR4/signalbin_"+label+"_HNL_SR4",  bin,  w, 8, 0.,8., "SRs");


}

void HNL_LeptonCore::FillLeptonPlots(AnalyzerParameter param,std::vector<Lepton *> leps, TString this_region, double weight){
  
  if(!DrawSyst(param.syst_)) return;

  for(unsigned int i=0; i<leps.size(); i++){

    TString this_itoa = TString::Itoa(i,10);

    Lepton *lep = leps[i];

    FillHist(this_region+"/Lepton_"+this_itoa+"_Pt_"+this_region, lep->Pt(), weight, 1000, 0., 1000.);
    FillHist(this_region+"/Lepton_"+this_itoa+"_Eta_"+this_region, lep->Eta(), weight, 60, -3., 3.);
    FillHist(this_region+"/Lepton_"+this_itoa+"_RelIso_"+this_region, lep->RelIso(), weight, 100, 0., 1.);
    FillHist(this_region+"/Lepton_"+this_itoa+"_MiniRelIso_"+this_region, lep->MiniRelIso(), weight, 100, 0., 1.);

    FillHist(this_region+"/Lepton_"+this_itoa+"_dXY_"+this_region, fabs(lep->dXY()), weight, 500, 0., 0.05);
    FillHist(this_region+"/Lepton_"+this_itoa+"_dXYSig_"+this_region, fabs(lep->dXY()/lep->dXYerr()), weight, 100, 0., 10);
    FillHist(this_region+"/Lepton_"+this_itoa+"_dZ_"+this_region, fabs(lep->dZ()), weight, 500, 0., 0.5);
    FillHist(this_region+"/Lepton_"+this_itoa+"_dZSig_"+this_region, fabs(lep->dZ()/lep->dZerr()), weight, 100, 0., 10);
    FillHist(this_region+"/Lepton_"+this_itoa+"_IP3D_"+this_region, fabs(lep->IP3D()), weight, 500, 0., 0.5);
    FillHist(this_region+"/Lepton_"+this_itoa+"_IP3DSig_"+this_region, fabs(lep->IP3D()/lep->IP3Derr()), weight, 100, 0., 10);

    if(lep->LeptonFlavour()==Lepton::ELECTRON){
      Electron *el = (Electron *)lep;
      FillHist(this_region+"/Lepton_"+this_itoa+"_MVANoIso_"+this_region, el->MVANoIso(), weight, 200, -1., 1.);
    }
    else if(lep->LeptonFlavour()==Lepton::MUON){
      Muon *mu = (Muon *)lep;
      FillHist(this_region+"/Lepton_"+this_itoa+"_Chi2_"+this_region, mu->Chi2(), weight, 500, 0., 50.);
      FillHist(this_region+"/Lepton_"+this_itoa+"_TrkRelIso_"+this_region, mu->TrkIso()/mu->TuneP4().Pt(), weight, 100, 0., 1.);
    }
    else{
      cout << "[HNL_LeptonCore::FillLeptonPlots] lepton flavour wrong.." << endl;
      exit(EXIT_FAILURE);
    }


  }
  return;
}

void HNL_LeptonCore::FillJetPlots(AnalyzerParameter param,std::vector<Jet> jets, std::vector<FatJet> fatjets, TString this_region, double weight){

  if(!DrawSyst(param.syst_)) return;

  for(unsigned int i=0; i<jets.size(); i++){

    TString this_itoa = TString::Itoa(i,10);
    FillHist(this_region+"/Jet_"+this_itoa+"_Pt_"+this_region, jets.at(i).Pt(), weight, 1000, 0., 1000.);
    FillHist(this_region+"/Jet_"+this_itoa+"_Eta_"+this_region, jets.at(i).Eta(), weight, 60, -3., 3.);

  }

  for(unsigned int i=0; i<fatjets.size(); i++){

    TString this_itoa = TString::Itoa(i,10);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Pt_"+this_region, fatjets.at(i).Pt(), weight, 1000, 0., 1000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Eta_"+this_region, fatjets.at(i).Eta(), weight, 60, -3., 3.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_Mass_"+this_region, fatjets.at(i).M(), weight, 3000, 0., 3000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_SDMass_"+this_region, fatjets.at(i).SDMass(), weight, 3000, 0., 3000.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_LSF_"+this_region, fatjets.at(i).LSF(), weight, 100, 0., 1.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_PuppiTau21_"+this_region, fatjets.at(i).PuppiTau2()/fatjets.at(i).PuppiTau1(), weight, 100, 0., 1.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_PuppiTau31_"+this_region, fatjets.at(i).PuppiTau3()/fatjets.at(i).PuppiTau1(), weight, 100, 0., 1.);
    FillHist(this_region+"/FatJet_"+this_itoa+"_PuppiTau32_"+this_region, fatjets.at(i).PuppiTau3()/fatjets.at(i).PuppiTau2(), weight, 100, 0., 1.);
  }

}

double HNL_LeptonCore::FillWeightHist(TString label, double _weight){

  int szst = 50 - std::string(label).size();
  TString empty_st = "";
  for(int i = 0 ; i < szst; i++) empty_st+= " ";
  if(run_Debug) cout << "HNL_LeptonCore::FillWeightHist ["+label+"] " <<  empty_st<< "  correction =" <<   _weight << endl;

  double max_x_range = 5.;
  if(label.Contains("Lumi")) max_x_range = 100000; 

  if(!label.Contains("Syst_"))   FillHist( "weights/"+ label , _weight ,1., 200, -1.*max_x_range, max_x_range,"ev weight");

  return _weight;
}


double HNL_LeptonCore::FillFakeWeightHist(TString label, vector<Lepton *> Leps,AnalyzerParameter param,  double _weight){


  if(run_Debug) cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  if(run_Debug) {
    for(auto i: Leps) {
      if(i->LeptonFlavour()==Lepton::MUON) cout << "Muon " << endl;
      else cout<< "Electron " << endl;
    }
  }

  if(run_Debug) cout << "nLep = " << Leps.size() << endl;
  if(run_Debug) cout << "Weight = " << _weight << endl;

  TString TLType="";
  for(auto i: Leps) {
    if(i->PassLepID())TLType+="T";
    else TLType+="L";
  }
  if(run_Debug) cout << "TLType = " << TLType << endl;

  //  if(Leps.size() > 0) cout << "LepTightIDName = " << Leps[0]->LepTightIDName() << endl;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
  // cout << "TLType = " << TLType <<  " Weight = " << _weight << endl;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      

  FillHist( "FakeWeights/"+ label , _weight ,1., 200, -5., 5,"ev weight");

  if(Leps.size() == 2){

    bool T1 = Leps[0]->PassLepID();
    bool T2 = Leps[1]->PassLepID();
    if(T1&&T2)         FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 1, _weight , 5, 0., 5);
    if(T1&&!T2)        FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 2, _weight , 5, 0., 5);
    if(!T1&&T2)        FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 3, _weight , 5, 0., 5);
    if(!T1&&!T2)       FillHist( "FakeStudy/TLSplit/TightLoose"+ label , 4, _weight , 5, 0., 5);
    if(run_Debug) cout << "FillFakeWeightHist Lepton Types T1T2 = " << T1 << " "  << T2 << endl;
  }

  for(auto ilep : Leps) {
    FillHist( "FakeStudy/MVAPlots/QCD_LFvsHF_v5_"+label, ilep->HNL_MVA_Fake("QCD_LFvsHF_v5"), 1, 100, -1., 1.);

    if(ilep->HNL_MVA_Fake("QCD_LFvsHF_v5") > 0) FillHist( "FakeStudy/MVAPlots/QCD_BvsC_v5_LF_"+label, ilep->HNL_MVA_Fake("QCD_BvsC_v5"), 1, 100, -1., 1);
    else  FillHist( "FakeStudy/MVAPlots/QCD_BvsC_v5_HF_"+label, ilep->HNL_MVA_Fake("QCD_BvsC_v5"), 1, 100, -1., 1.);


    if(!ilep->PassLepID())  {

      
      //FillHist( "FakeStudy/Rates/"+ilep->LepTightIDName()+"/Loose_"+ label , FR, 1 , 1000, 0., 5);
      //FillHist( "FakeStudy/Rates/"+ilep->LepTightIDName()+"/Loose_VsPt"+ label , ilep->PtMaxed(60.), FR, 1 , 60, 0, 60, 1000, 0., 5);
      //FillHist( "FakeStudy/Rates/"+ilep->LepTightIDName()+"/Loose_"+ilep->sEtaRegion()+"_"+ label , FR, 1 , 1000, 0., 5);

    }

  }


  if(Leps.size() == 2){
    bool T1 = Leps[0]->PassLepID();
    bool T2 = Leps[1]->PassLepID();
    double FW = GetFakeWeight(Leps, param);
    if(T1&&T2)FillHist( "FakeStudy/EventWeight/TT_"+ label , FW, _weight , 100, 0., 5);
    if(T1&&!T2)FillHist( "FakeStudy/EventWeight/TL_"+ label , FW, _weight , 100, 0., 5);
    if(!T1&&T2)FillHist( "FakeStudy/EventWeight/LT_"+ label , FW, _weight , 100, 0., 5);
    if(!T1&&!T2)FillHist( "FakeStudy/EventWeight/LL_"+ label , FW, _weight , 100, 0., 5);
    if(run_Debug) cout << "EventWeight = " << FW << endl;
  }

  return _weight;
}


