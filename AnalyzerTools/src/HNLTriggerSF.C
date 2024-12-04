#include "MCCorrection.h"

float MCCorrection::GetTriggerSF(vector<Electron>& EleColl, vector<Muon>& MuColl, TString SFKey, TString Option){

  if(IsDATA) return 1.;
  if(SFKey=="Default") return 1;
 
  TString NominalOpt=Option; NominalOpt.ReplaceAll("Syst","");
  bool SystRun=Option.Contains("Syst");
  float SystDir=0., RelSystData=0., RelSystMC=0., TotRelSyst=0.;

  float TriggerEff_Data = TriggerEfficiency(EleColl, MuColl, SFKey, true,  NominalOpt);
  float TriggerEff_MC   = TriggerEfficiency(EleColl, MuColl, SFKey, false, NominalOpt);
  if(SystRun){
    float TriggerEff_Data_syst = TriggerEfficiency(EleColl, MuColl, SFKey, true,  Option);
    float TriggerEff_MC_syst   = TriggerEfficiency(EleColl, MuColl, SFKey, false, Option);
    RelSystData = TriggerEff_Data!=0.? (TriggerEff_Data_syst-TriggerEff_Data)/TriggerEff_Data:0.;
    RelSystMC   = TriggerEff_MC  !=0.? (TriggerEff_MC_syst  -TriggerEff_MC  )/TriggerEff_MC  :0.;
    TotRelSyst  = sqrt(pow(RelSystData,2.)+pow(RelSystMC,2.));
    if     (Option.Contains("Up"))   SystDir =  1.;
    else if(Option.Contains("Down")) SystDir = -1.;
  }
  
  float TriggerScaleFactor = TriggerEff_MC!=0.? TriggerEff_Data/TriggerEff_MC:0.;
  if(TriggerScaleFactor<0) TriggerScaleFactor=0.;
  if(SystRun) TriggerScaleFactor *= (1.+SystDir*TotRelSyst);

  return TriggerScaleFactor;

}


float MCCorrection::TriggerEfficiency(vector<Electron>& EleColl, vector<Muon>& MuColl, TString SFKey, bool ReturnDataEff, TString Option){
  //DataorMC : T: Return DataEff, F: Return MCEff

  if(IsDATA) return 1.;

  TString StrMCorData = ReturnDataEff? "DATA":"MC";
  int SystDir=0;
  if(Option.Contains("Syst")){
    if     (Option.Contains("Up"))   SystDir= 1.;
    else if(Option.Contains("Down")) SystDir=-1.;
  }
  

  bool SiglMuTrig=false, SiglElTrig=false, DiMuTrig=false, DiElTrig=false, EMuTrig=false, TrigSoup2L=false;
  float MinPt1=-1, MaxPt1=-1, MinPt2=-1, MaxPt2=-1, MinPt3=-1, MaxPt3=-1., MinPt4=-1, MaxPt4=-1, MaxfEta1=-1, MaxfEta2=-1;
  float MinPt5=-1, MaxPt5=-1, MinPt6=-1, MaxPt6=-1, MinPt7=-1, MaxPt7=-1, MinPt8=-1, MaxPt8=-1;
  TH2F* HistEff1=NULL; TH2F* HistEff2=NULL; TH2F* HistEff3=NULL; TH2F* HistEff4=NULL;
  TH2F* HistEff5=NULL; TH2F* HistEff6=NULL; TH2F* HistEff7=NULL; TH2F* HistEff8=NULL;
  if(DataYear==2016 && SFKey.Contains("IsoORTkIsoMu24_POGTight")){
    SiglMuTrig=true, MinPt1=26., MaxPt1=500., MaxfEta1=2.4; 
    HistEff1 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_IsoMu24_POGTight"];
  }
  else if(DataYear==2017 && SFKey.Contains("IsoMu27_POGTight")){
    SiglMuTrig=true, MinPt1=29., MaxPt1=1200., MaxfEta1=2.4; 
    HistEff1 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_IsoMu27_POGTight"];
  }
  else if(DataYear==2018 && SFKey.Contains("IsoMu24_POGTight")){
    SiglMuTrig=true, MinPt1=26., MaxPt1=1200., MaxfEta1=2.4; 
    HistEff1 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_IsoMu24_POGTight"];
  }
  else if(DataYear==2016 && SFKey.Contains("Ele27WPTight_POGMVAIsoWP90")){
    SiglElTrig=true, MinPt1=35., MaxPt1=500., MaxfEta1=2.5;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_Ele27WPTight_POGMVAIsoWP90"];
  }
  else if(DataYear==2017 && SFKey.Contains("Ele32WPTight1OR2_POGMVAIsoWP90")){
    SiglElTrig=true, MinPt1=35., MaxPt1=500., MaxfEta1=2.5;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_Ele32WPTight1OR2_POGMVAIsoWP90"];
  }
  else if(DataYear==2018 && SFKey.Contains("Ele32WPTight_POGMVAIsoWP90")){
    SiglElTrig=true, MinPt1=35., MaxPt1=500., MaxfEta1=2.5;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_Ele32WPTight_POGMVAIsoWP90"];
  }
  else if(SFKey.Contains("Ele23Leg_EETr")){
    SiglElTrig=true, MinPt1=25., MaxPt1=200., MaxfEta1=2.5;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_DiElIsoEl23_HNL_ULID"];
  }
  else if(SFKey.Contains("Ele12Leg_EETr")){
    SiglElTrig=true, MinPt1=15., MaxPt1=200., MaxfEta1=2.5;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_DiElIsoEl12_HNL_ULID"];
  }
  else if(SFKey.Contains("Mu17Leg_MMTr")){
    SiglElTrig=true, MinPt1=20., MaxPt1=200., MaxfEta1=2.4;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_DiMuIsoMu17_HNL_ULID"];
  }
  else if(SFKey.Contains("Mu8Leg_MMTr")){
    SiglElTrig=true, MinPt1=10., MaxPt1=200., MaxfEta1=2.4;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_DiMuIsoMu8_HNL_ULID"];
  }
  else if(SFKey.Contains("DiMuIso")){
    DiMuTrig=true; MinPt1=20., MinPt2=10., MaxPt1=999., MaxPt2=999., MaxfEta1=2.4;
    TString Key1(SFKey), Key2(SFKey); Key1.ReplaceAll("DiMuIso","DiMuIsoMu17"); Key2.ReplaceAll("DiMuIso","DiMuIsoMu8");
    HistEff1 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_"+Key1];
    HistEff2 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_"+Key2];
  }
  else if(SFKey.Contains("DiElIso")){
    DiElTrig=true; MinPt1=25., MinPt2=15., MaxPt1=200., MaxPt2=200., MaxfEta1=2.5;
    TString Key1(SFKey), Key2(SFKey); Key1.ReplaceAll("DiElIso","DiElIsoEl23"); Key2.ReplaceAll("DiElIso","DiElIsoEl12");
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_"+Key1];
    HistEff2 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_"+Key2];
  }
  else if(SFKey=="EMuIso_HNL_ULID" or SFKey=="EMuIso_HNL_ULID"){
    EMuTrig=true;
    //    TString SSLabel = SFKey.Contains("IDSS")? "SS":"";
    TString TrEl2Pt = DataEra=="2016preVFP"? "8":"12";
    MinPt1=25., MinPt2=15., MaxPt1=200., MaxPt2=200., MaxfEta1=2.5;
    MinPt3=25., MinPt4=10., MaxPt3=200., MaxPt4=200., MaxfEta2=2.4;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_EMuIsoEl23_HNL_ULID"];
    HistEff2 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_EMuIsoEl"+TrEl2Pt+"_HNL_ULID"];
    HistEff3 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_EMuIsoMu23_HNL_ULID"];
    HistEff4 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_EMuIsoMu8_HNL_ULID"];
  }
  else if(SFKey.Contains("TrigSoup2L")){
    TrigSoup2L=true; TString SSLabel = SFKey.Contains("IDSS")? "SS":"";
    TString TrEl2Pt = DataEra=="2016preVFP"? "8":"12";
    MinPt1=25., MinPt2=15., MinPt3=25., MinPt4=15., MaxPt1=200., MaxPt2=200., MaxPt3=200., MaxPt4=200.;
    MinPt5=25., MinPt6=10., MinPt7=20., MinPt8=10., MaxPt5=200., MaxPt6=200., MaxPt7=200., MaxPt8=200.;
    MaxfEta1=2.5, MaxfEta2=2.4;
    HistEff1 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_EMuIsoEl23_HNL_ULID"+SSLabel];
    HistEff2 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_EMuIsoEl"+TrEl2Pt+"_HNL_ULID"+SSLabel];
    HistEff3 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_DiElIsoEl23_HNL_ULID"+SSLabel];
    HistEff4 = map_hist_Electron["Trigger_Eff_"+StrMCorData+"_DiElIsoEl12_HNL_ULID"+SSLabel];
    HistEff5 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_EMuIsoMu23_HNL_ULID"];
    HistEff6 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_EMuIsoMu8_HNL_ULID"];
    HistEff7 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_DiMuIsoMu17_HNL_ULID"];
    HistEff8 = map_hist_Muon["Trigger_Eff_"+StrMCorData+"_DiMuIsoMu8_HNL_ULID"];
  }
  MinPt1+=1E-5, MinPt2+=1E-5, MaxPt1-=1E-5, MaxPt2-=1E-5, MaxfEta1-=1E-5;
  MinPt3+=1E-5, MinPt4+=1E-5, MaxPt3-=1E-5, MaxPt4-=1E-5, MaxfEta2-=1E-5;
  MinPt5+=1E-5, MinPt6+=1E-5, MinPt7+=1E-5, MinPt8+=1E-5, MaxPt5-=1E-5, MaxPt6-=1E-5, MaxPt7-=1E-5, MaxPt8-=1E-5;

  if(!HistEff1){cerr<<"[MCCorrection::Trigger_Eff] No eff file for "<<SFKey<<endl; exit(EXIT_FAILURE);}
  if( (DiMuTrig or DiElTrig) && (!HistEff2) ){cerr<<"[MCCorrection::Trigger_Eff] No eff file for leg2 "<<SFKey<<endl; exit(EXIT_FAILURE);}
  if( EMuTrig && !(HistEff2 && HistEff3 && HistEff4)){ cerr<<"[MCCorrection::Trigger_Eff] No eff file for "<<SFKey<<endl; exit(EXIT_FAILURE); }
  if( TrigSoup2L && !(HistEff2 && HistEff3 && HistEff4 && HistEff5 && HistEff6 && HistEff7 && HistEff8) ){
    cerr<<"[MCCorrection::Trigger_Eff] No eff file for "<<SFKey<<endl; exit(EXIT_FAILURE); }


  float TriggerEff=0.;  int NMu=MuColl.size(), NEl=EleColl.size();
  if(SiglMuTrig){

    TriggerEff = 1;
    
    for(unsigned int it_m=0; it_m<MuColl.size(); it_m++){
      float pt   = MuColl.at(it_m).Pt();
      float feta = fabs(MuColl.at(it_m).Eta());
      if     (pt<MinPt1)  continue;
      else if(pt>MaxPt1)  continue;
      if     (feta>MaxfEta1)  continue;
      
      int BinIdx = HistEff1->FindBin(pt, feta);
      double SLTriggerEff = HistEff1->GetBinContent(BinIdx);
      if(SystDir!=0){ SLTriggerEff += float(SystDir)*HistEff1->GetBinError(BinIdx); }
      
      TriggerEff *= (1 - SLTriggerEff);

    }

    if (TriggerEff == 1) return 1;
    else return ( 1. -TriggerEff);

  }
  else if(SiglElTrig){

    TriggerEff = 1;

    for(unsigned int it_e=0; it_e<EleColl.size(); it_e++){
      float pt   = EleColl.at(it_e).Pt();
      float feta = fabs(EleColl.at(it_e).scEta());

      if     (pt<MinPt1)  continue;
      else if(pt>MaxPt1)  continue;
      if     (feta>MaxfEta1)  continue;

      int BinIdx = HistEff1->FindBin(feta, pt);
      //int BinIdx = HistEff1->FindBin(pt, feta);

      double SLTriggerEff = HistEff1->GetBinContent(BinIdx);
      if(SystDir!=0){ SLTriggerEff += float(SystDir)*HistEff1->GetBinError(BinIdx); }

      TriggerEff *= (1 - SLTriggerEff);

    }
    if (TriggerEff == 1) return 1;
    else return ( 1. -TriggerEff);


  }
  else if(DiMuTrig){
    if(NMu==2){
      float pt1  = MuColl.at(0).Pt(), pt2 = MuColl.at(1).Pt();
      float feta1 = fabs(MuColl.at(0).Eta()), feta2 = fabs(MuColl.at(1).Eta());
      pt1  = min(max(pt1,MinPt1),MaxPt1), pt2 = min(max(pt2,MinPt2),MaxPt2);
      feta1 = min(max(feta1,((float)-1.)*MaxfEta1),MaxfEta1), feta2 = min(max(feta2,((float)-1.)*MaxfEta1),MaxfEta1);
      
      float EffLeg1_Mu1 = HistEff1->GetBinContent(HistEff1->FindBin(feta1, pt1));
      float EffLeg2_Mu2 = HistEff2->GetBinContent(HistEff2->FindBin(feta2, pt2));
      float ErrLeg1_Mu1 = HistEff1->GetBinError(HistEff1->FindBin(feta1, pt1));
      float ErrLeg2_Mu2 = HistEff2->GetBinError(HistEff2->FindBin(feta2, pt2));
      float EffDZ       = DZEfficiency(SFKey, ReturnDataEff, "");
      if(SystDir!=0){ EffLeg1_Mu1+=float(SystDir)*ErrLeg1_Mu1; EffLeg2_Mu2+=float(SystDir)*ErrLeg2_Mu2; }
      TriggerEff = EffLeg1_Mu1*EffLeg2_Mu2*EffDZ;
    }
    if(NMu==3){
      float pt1  = MuColl.at(0).Pt() , pt2  = MuColl.at(1).Pt() , pt3  = MuColl.at(2).Pt(), eps=1E-5;
      float feta1 = fabs(MuColl.at(0).Eta()), feta2 = fabs(MuColl.at(1).Eta()), feta3 = fabs(MuColl.at(2).Eta());
      pt1  = min(max(pt1,MinPt1),MaxPt1), pt2 = min(max(pt2,MinPt2),MaxPt2), pt3 = min(max(pt3,MinPt2),MaxPt2);
      feta1 = min(max(feta1,eps),MaxfEta1), feta2 = min(max(feta2,eps),MaxfEta1), feta3 = min(max(feta3,eps),MaxfEta1);
      float EffLeg1_Mu1 = HistEff1->GetBinContent(HistEff1->FindBin(feta1, pt1));
      float EffLeg1_Mu2 = HistEff1->GetBinContent(HistEff1->FindBin(feta2, pt2));
      float EffLeg2_Mu2 = HistEff2->GetBinContent(HistEff2->FindBin(feta2, pt2));
      float EffLeg2_Mu3 = HistEff2->GetBinContent(HistEff2->FindBin(feta3, pt3));
      float EffDZ       = DZEfficiency(SFKey, ReturnDataEff, "");
      TriggerEff = EffLeg1_Mu1*( EffLeg2_Mu2+(1.-EffLeg2_Mu2*EffDZ)*EffLeg2_Mu3 )*EffDZ + (1.-EffLeg1_Mu1*EffDZ)*EffLeg1_Mu2*EffLeg2_Mu3*EffDZ;
    }
    //// TMP Fix for 4Mu events  Assume only first 3 mu trigger
    if(NMu==4){
      float pt1  = MuColl.at(0).Pt() , pt2  = MuColl.at(1).Pt() , pt3  = MuColl.at(2).Pt(), eps=1E-5;
      float feta1 = fabs(MuColl.at(0).Eta()), feta2 = fabs(MuColl.at(1).Eta()), feta3 = fabs(MuColl.at(2).Eta());
      pt1  = min(max(pt1,MinPt1),MaxPt1), pt2 = min(max(pt2,MinPt2),MaxPt2), pt3 = min(max(pt3,MinPt2),MaxPt2);
      feta1 = min(max(feta1,eps),MaxfEta1), feta2 = min(max(feta2,eps),MaxfEta1), feta3 = min(max(feta3,eps),MaxfEta1);
      float EffLeg1_Mu1 = HistEff1->GetBinContent(HistEff1->FindBin(feta1, pt1));
      float EffLeg1_Mu2 = HistEff1->GetBinContent(HistEff1->FindBin(feta2, pt2));
      float EffLeg2_Mu2 = HistEff2->GetBinContent(HistEff2->FindBin(feta2, pt2));
      float EffLeg2_Mu3 = HistEff2->GetBinContent(HistEff2->FindBin(feta3, pt3));
      float EffDZ       = DZEfficiency(SFKey, ReturnDataEff, "");
      TriggerEff = EffLeg1_Mu1*( EffLeg2_Mu2+(1.-EffLeg2_Mu2*EffDZ)*EffLeg2_Mu3 )*EffDZ + (1.-EffLeg1_Mu1*EffDZ)*EffLeg1_Mu2*EffLeg2_Mu3*EffDZ;
    }

  }
  else if(DiElTrig){
    if(NEl==2){
      float pt1  = EleColl.at(0).Pt(), pt2 = EleColl.at(1).Pt();
      float eta1 = fabs(EleColl.at(0).scEta()), eta2 = fabs(EleColl.at(1).scEta());
      pt1 = min(max(pt1,MinPt1),MaxPt1), pt2 = min(max(pt2,MinPt2),MaxPt2);
      eta1 = min(max(eta1,((float)-1.)*MaxfEta1),MaxfEta1), eta2 = min(max(eta2,((float)-1.)*MaxfEta1),MaxfEta1);

      float EffLeg1_El1 = HistEff1->GetBinContent(HistEff1->FindBin(eta1, pt1));
      float EffLeg2_El2 = HistEff2->GetBinContent(HistEff2->FindBin(eta2, pt2));
      float ErrLeg1_El1 = HistEff1->GetBinError(HistEff1->FindBin(eta1, pt1));
      float ErrLeg2_El2 = HistEff2->GetBinError(HistEff2->FindBin(eta2, pt2));
      float EffDZ       = DZEfficiency(SFKey, ReturnDataEff, "");
      if(SystDir!=0){ EffLeg1_El1+=float(SystDir)*ErrLeg1_El1; EffLeg2_El2+=float(SystDir)*ErrLeg2_El2; }
      TriggerEff = EffLeg1_El1*EffLeg2_El2*EffDZ;
    }
    if(NEl==3 || NEl==4){
      float pt1  = EleColl.at(0).Pt(), pt2 = EleColl.at(1).Pt(), pt3 = EleColl.at(2).Pt();
      float eta1 = fabs(EleColl.at(0).scEta()), eta2 = fabs(EleColl.at(1).scEta()), eta3 = fabs(EleColl.at(2).scEta());
      
      pt1 = min(max(pt1,MinPt1),MaxPt1), pt2 = min(max(pt2,MinPt2),MaxPt2), pt3 = min(max(pt3,MinPt2),MaxPt2);
      eta1 = min(max(eta1,((float)-1.)*MaxfEta1),MaxfEta1), eta2 = min(max(eta2,((float)-1.)*MaxfEta1),MaxfEta1);
      eta3 = min(max(eta3,((float)-1.)*MaxfEta1),MaxfEta1);

      float EffLeg1_El1 = HistEff1->GetBinContent(HistEff1->FindBin(eta1, pt1));
      float EffLeg1_El2 = HistEff1->GetBinContent(HistEff1->FindBin(eta2, pt2));
      float EffLeg2_El2 = HistEff2->GetBinContent(HistEff2->FindBin(eta2, pt2));
      float EffLeg2_El3 = HistEff2->GetBinContent(HistEff2->FindBin(eta3, pt3));
      float EffDZ       = DZEfficiency(SFKey, ReturnDataEff, "");
      TriggerEff = EffLeg1_El1*( EffLeg2_El2+(1.-EffLeg2_El2*EffDZ)*EffLeg2_El3 )*EffDZ + (1.-EffLeg1_El1*EffDZ)*EffLeg1_El2*EffLeg2_El3*EffDZ ;
    }
  }
  else if(EMuTrig){
    if(NEl==1 && NMu==1){
      float pt_m = MuColl.at(0).Pt(), pt_e = EleColl.at(0).Pt();
      float feta_m = fabs(MuColl.at(0).Eta()), feta_e = fabs(EleColl.at(0).scEta());
      feta_m = min(feta_m,MaxfEta2), feta_e = min(feta_e,MaxfEta1);

      float Eff_Mu = 0., Eff_El=0., Eff_DZ=0., Err_Mu=0., Err_El=0.;
      if(pt_e>MinPt1){
        pt_m   = min(max(pt_m,MinPt4),MaxPt4);
        Eff_Mu = HistEff4->GetBinContent(HistEff4->FindBin(pt_m, feta_m));
        Err_Mu = HistEff4->GetBinError(HistEff4->FindBin(pt_m, feta_m));
      }
      else{
        pt_m   = min(max(pt_m,MinPt3),MaxPt3);
        Eff_Mu = HistEff3->GetBinContent(HistEff3->FindBin(pt_m, feta_m));
        Err_Mu = HistEff3->GetBinError(HistEff3->FindBin(pt_m, feta_m));
      }
      if(pt_m>MinPt3){  
        pt_e   = min(max(pt_e,MinPt2),MaxPt2);
        Eff_El = HistEff2->GetBinContent(HistEff2->FindBin(pt_e, feta_e));
        Err_El = HistEff2->GetBinError(HistEff2->FindBin(pt_e, feta_e));
      }
      else{
        pt_e   = min(max(pt_e,MinPt1),MaxPt1);
        Eff_El = HistEff1->GetBinContent(HistEff1->FindBin(pt_e, feta_e));
        Err_El = HistEff1->GetBinError(HistEff1->FindBin(pt_e, feta_e));
      }
      Eff_DZ = DZEfficiency(SFKey, ReturnDataEff, "");
      if(SystDir!=0){ Eff_Mu+=float(SystDir)*Err_Mu; Eff_El+=float(SystDir)*Err_El; }
      TriggerEff = Eff_Mu*Eff_El*Eff_DZ;
    }
    if(NEl==1 && NMu==2){
      float pt_e  = EleColl.at(0).Pt(), pt_m1 = MuColl.at(0).Pt(), pt_m2 = MuColl.at(1).Pt();
      float feta_e = fabs(EleColl.at(0).scEta()), feta_m1 = fabs(MuColl.at(0).Eta()), feta_m2 = fabs(MuColl.at(1).Eta());
      feta_e = min(feta_e,MaxfEta1), feta_m1 = min(feta_m1,MaxfEta2), feta_m2 = min(feta_m2,MaxfEta2);
      float Eff_e = 0., Eff_m = 0., Eff_DZ = 0., Eff_MuLeg_Mu1 = 0., Eff_MuLeg_Mu2 = 0.;

      Eff_DZ = DZEfficiency(SFKey, ReturnDataEff, "");
      if(pt_e>MinPt1){
        pt_m1 = min(pt_m1,MaxPt4), pt_m2 = min(pt_m2,MaxPt4);
        Eff_MuLeg_Mu1 = HistEff4->GetBinContent(HistEff4->FindBin(pt_m1, feta_m1));
        Eff_MuLeg_Mu2 = HistEff4->GetBinContent(HistEff4->FindBin(pt_m2, feta_m2));
        Eff_m = Eff_MuLeg_Mu1 + (1.-Eff_MuLeg_Mu1*Eff_DZ)*Eff_MuLeg_Mu2;
      }
      else{
        pt_m1 = min(pt_m1,MaxPt3), pt_m2 = min(pt_m2,MaxPt3);
        Eff_MuLeg_Mu1 = HistEff3->GetBinContent(HistEff3->FindBin(pt_m1, feta_m1));
        Eff_MuLeg_Mu2 = HistEff3->GetBinContent(HistEff3->FindBin(pt_m2, feta_m2));
        Eff_m = Eff_MuLeg_Mu1 + (1.-Eff_MuLeg_Mu1*Eff_DZ)*Eff_MuLeg_Mu2;
      }
      if(pt_m1>MinPt3){
        pt_e  = min(pt_e,MaxPt2);
        Eff_e = HistEff2->GetBinContent(HistEff2->FindBin(pt_e, feta_e));
      }
      else{
        pt_e  = min(pt_e,MaxPt1);
        Eff_e = HistEff1->GetBinContent(HistEff1->FindBin(pt_e, feta_e));
      }
      TriggerEff = Eff_e*Eff_m*Eff_DZ;
    }
    if(NEl==2 && NMu==1){
      float pt_m   = MuColl.at(0).Pt(), pt_e1 = EleColl.at(0).Pt(), pt_e2 = EleColl.at(1).Pt();
      float feta_m = fabs(MuColl.at(0).Eta()), feta_e1 = fabs(EleColl.at(0).scEta()), feta_e2 = fabs(EleColl.at(1).scEta());
      feta_m = min(feta_m,MaxfEta2), feta_e1 = min(feta_e1,MaxfEta1), feta_e2 = min(feta_e2,MaxfEta1);
      float Eff_e = 0., Eff_m = 0., Eff_DZ = 0., Eff_ElLeg_El1 = 0., Eff_ElLeg_El2 = 0.;

      Eff_DZ = DZEfficiency(SFKey, ReturnDataEff, "");
      if(pt_m>MinPt3){
        pt_e1 = min(pt_e1,MaxPt2), pt_e2 = min(pt_e2,MaxPt2);
        Eff_ElLeg_El1 = HistEff2->GetBinContent(HistEff2->FindBin(pt_e1, feta_e1));
        Eff_ElLeg_El2 = HistEff2->GetBinContent(HistEff2->FindBin(pt_e2, feta_e2));
        Eff_e = Eff_ElLeg_El1 + (1.-Eff_ElLeg_El1*Eff_DZ)*Eff_ElLeg_El2;
      }
      else{
        pt_e1 = min(pt_e1,MaxPt1), pt_e2 = min(pt_e2,MaxPt1);
        Eff_ElLeg_El1 = HistEff1->GetBinContent(HistEff1->FindBin(pt_e1, feta_e1));
        Eff_ElLeg_El2 = HistEff1->GetBinContent(HistEff1->FindBin(pt_e2, feta_e2));
        Eff_e = Eff_ElLeg_El1 + (1.-Eff_ElLeg_El1*Eff_DZ)*Eff_ElLeg_El2;
      }
      if(pt_e1>MinPt1){
        pt_m  = min(pt_m,MaxPt4);
        Eff_m = HistEff4->GetBinContent(HistEff4->FindBin(pt_m, feta_m));
      }
      else{
        pt_m  = min(pt_m,MaxPt3);
        Eff_m = HistEff3->GetBinContent(HistEff3->FindBin(pt_m, feta_m));
      }
      TriggerEff = Eff_e*Eff_m*Eff_DZ;
    }
    if(NEl==2 && NMu==2){
      float pt_m1 = MuColl.at(0).Pt(), pt_m2 = MuColl.at(1).Pt(), pt_e1 = EleColl.at(0).Pt(), pt_e2 = EleColl.at(1).Pt();
      float feta_m1 = fabs(MuColl.at(0).Eta()), feta_m2 = fabs(MuColl.at(1).Eta()), feta_e1 = fabs(EleColl.at(0).scEta()), feta_e2 = fabs(EleColl.at(1).scEta());
      feta_m1 = min(feta_m1,MaxfEta2), feta_m2 = min(feta_m2,MaxfEta2), feta_e1 = min(feta_e1,MaxfEta1), feta_e2 = min(feta_e2,MaxfEta1);
      float Eff_e = 0., Eff_m = 0., Eff_DZ = 0., Eff_ElLeg_El1 = 0., Eff_ElLeg_El2 = 0., Eff_MuLeg_Mu1 = 0., Eff_MuLeg_Mu2 = 0.;

      Eff_DZ = DZEfficiency(SFKey, ReturnDataEff, "");
      if(pt_m1<MinPt3&&pt_m2<MinPt3){
        pt_e1 = min(pt_e1,MaxPt1), pt_e2 = min(pt_e2,MaxPt1);
        Eff_ElLeg_El1 = HistEff1->GetBinContent(HistEff1->FindBin(pt_e1, feta_e1));
        Eff_ElLeg_El2 = HistEff1->GetBinContent(HistEff1->FindBin(pt_e2, feta_e2));
        Eff_e = Eff_ElLeg_El1 + (1.-Eff_ElLeg_El1)*Eff_ElLeg_El2;
      }
      else{
        pt_e1 = min(pt_e1,MaxPt2), pt_e2 = min(pt_e2,MaxPt2);
        Eff_ElLeg_El1 = HistEff2->GetBinContent(HistEff2->FindBin(pt_e1, feta_e1));
        Eff_ElLeg_El2 = HistEff2->GetBinContent(HistEff2->FindBin(pt_e2, feta_e2));
        Eff_e = Eff_ElLeg_El1 + (1.-Eff_ElLeg_El1)*Eff_ElLeg_El2;
      }
      if(pt_e1<MinPt1&&pt_e2<MinPt1){
        pt_m1 = min(pt_m1,MaxPt3), pt_m2 = min(pt_m2,MaxPt3);
        Eff_MuLeg_Mu1 = HistEff3->GetBinContent(HistEff3->FindBin(pt_m1, feta_m1));
        Eff_MuLeg_Mu2 = HistEff3->GetBinContent(HistEff3->FindBin(pt_m2, feta_m2));
        Eff_m = Eff_MuLeg_Mu1 + (1.-Eff_MuLeg_Mu1)*Eff_MuLeg_Mu2;
      }
      else{
        pt_m1 = min(pt_m1,MaxPt4), pt_m2 = min(pt_m2,MaxPt4);
        Eff_MuLeg_Mu1 = HistEff4->GetBinContent(HistEff4->FindBin(pt_m1, feta_m1));
        Eff_MuLeg_Mu2 = HistEff4->GetBinContent(HistEff4->FindBin(pt_m2, feta_m2));
        Eff_m = Eff_MuLeg_Mu1 + (1.-Eff_MuLeg_Mu1)*Eff_MuLeg_Mu2;
      }
      TriggerEff = Eff_e*Eff_m*Eff_DZ;
    }
  }
  else if(TrigSoup2L){
    if(NMu==3){
      float pt1  = MuColl.at(0).Pt() , pt2  = MuColl.at(1).Pt() , pt3  = MuColl.at(2).Pt();
      float feta1 = fabs(MuColl.at(0).Eta()), feta2 = fabs(MuColl.at(1).Eta()), feta3 = fabs(MuColl.at(2).Eta());
      feta1 = min(feta1,MaxfEta2), feta2 = min(feta2,MaxfEta2), feta3 = min(feta3,MaxfEta2);
      float EffLeg1_Mu1 = HistEff7->GetBinContent(HistEff7->FindBin(feta1, min(pt1,MaxPt7)));
      float EffLeg1_Mu2 = HistEff7->GetBinContent(HistEff7->FindBin(feta2, min(pt2,MaxPt7)));
      float EffLeg2_Mu2 = HistEff8->GetBinContent(HistEff8->FindBin(feta2, min(pt2,MaxPt8)));
      float EffLeg2_Mu3 = HistEff8->GetBinContent(HistEff8->FindBin(feta3, min(pt3,MaxPt8)));
      float Eff_DZ      = DZEfficiency("DiMuIso", ReturnDataEff, "");
      TriggerEff = EffLeg1_Mu1*( EffLeg2_Mu2+(1.-EffLeg2_Mu2*Eff_DZ)*EffLeg2_Mu3 ) + (1.-EffLeg1_Mu1*Eff_DZ)*EffLeg1_Mu2*EffLeg2_Mu3;
      TriggerEff *= Eff_DZ;
    }
    else if(NEl==3){
      float pt1  = EleColl.at(0).Pt(), pt2 = EleColl.at(1).Pt(), pt3 = EleColl.at(2).Pt();
      float eta1 = fabs(EleColl.at(0).scEta()), eta2 = fabs(EleColl.at(1).scEta()), eta3 = fabs(EleColl.at(2).scEta());
      eta1 = min(max(eta1,((float)-1.)*MaxfEta1),MaxfEta1), eta2 = min(max(eta2,((float)-1.)*MaxfEta1),MaxfEta1);
      eta3 = min(max(eta3,((float)-1.)*MaxfEta1),MaxfEta1);
      float EffLeg1_El1 = HistEff3->GetBinContent(HistEff3->FindBin(eta1, min(pt1,MaxPt3)));
      float EffLeg1_El2 = HistEff3->GetBinContent(HistEff3->FindBin(eta2, min(pt2,MaxPt3)));
      float EffLeg2_El2 = HistEff4->GetBinContent(HistEff4->FindBin(eta2, min(pt2,MaxPt4)));
      float EffLeg2_El3 = HistEff4->GetBinContent(HistEff4->FindBin(eta3, min(pt3,MaxPt4)));
      float Eff_DZ      = DZEfficiency("DiElIso", ReturnDataEff, "");
      TriggerEff = EffLeg1_El1*( EffLeg2_El2+(1.-EffLeg2_El2*Eff_DZ)*EffLeg2_El3 ) + (1.-EffLeg1_El1*Eff_DZ)*EffLeg1_El2*EffLeg2_El3;
      TriggerEff *= Eff_DZ;
    }
    else if(NEl==1 && NMu==2){
      float pt_e   = EleColl.at(0).Pt()  , pt_m1   = MuColl.at(0).Pt()  , pt_m2   = MuColl.at(1).Pt()  ;
      float eta_e  = EleColl.at(0).scEta() , eta_m1  = MuColl.at(0).Eta() , eta_m2  = MuColl.at(1).Eta() ;
      float feta_e = fabs(eta_e)         , feta_m1 = fabs(eta_m1)       , feta_m2 = fabs(eta_m2)       ;
      feta_e = min(feta_e,MaxfEta1), feta_m1 = min(feta_m1,MaxfEta2), feta_m2 = min(feta_m2,MaxfEta2);
      float Eff_EMe = 0., Eff_EMm = 0., Eff_EMTrMuLeg_Mu1 = 0., Eff_EMTrMuLeg_Mu2 = 0., Eff_MMTrLeg1_Mu1 = 0., Eff_MMTrLeg2_Mu2 = 0.;
      float Eff_DZem = DZEfficiency("EMuIso", ReturnDataEff, "");
      float Eff_DZmm = DZEfficiency("DiMuIso", ReturnDataEff, "");
      if(pt_e>MinPt1){
        Eff_EMTrMuLeg_Mu1 = HistEff6->GetBinContent(HistEff6->FindBin(min(pt_m1,MaxPt6), feta_m1));
        Eff_EMTrMuLeg_Mu2 = HistEff6->GetBinContent(HistEff6->FindBin(min(pt_m2,MaxPt6), feta_m2));
        Eff_EMm = Eff_EMTrMuLeg_Mu1 + (1.-Eff_EMTrMuLeg_Mu1*Eff_DZem)*Eff_EMTrMuLeg_Mu2;
      }
      else{
        Eff_EMTrMuLeg_Mu1 = HistEff5->GetBinContent(HistEff5->FindBin(min(pt_m1,MaxPt5), feta_m1));
        Eff_EMTrMuLeg_Mu2 = HistEff5->GetBinContent(HistEff5->FindBin(min(pt_m2,MaxPt5), feta_m2));
        Eff_EMm = Eff_EMTrMuLeg_Mu1 + (1.-Eff_EMTrMuLeg_Mu1*Eff_DZem)*Eff_EMTrMuLeg_Mu2;
      }
      if(pt_m1>MinPt7){
        Eff_MMTrLeg1_Mu1 = HistEff7->GetBinContent(HistEff7->FindBin(feta_m1, min(pt_m1,MaxPt7))); 
        Eff_MMTrLeg2_Mu2 = HistEff8->GetBinContent(HistEff8->FindBin(feta_m2, min(pt_m2,MaxPt8))); 
      }
      if(pt_m1>MinPt5){
        Eff_EMe = HistEff2->GetBinContent(HistEff2->FindBin(min(pt_e,MaxPt2), feta_e));
      }
      else{
        Eff_EMe = HistEff1->GetBinContent(HistEff1->FindBin(min(pt_e,MaxPt1), feta_e));
      }
      TriggerEff = Eff_EMe*Eff_EMm*Eff_DZem+(1-Eff_EMe*Eff_DZem)*Eff_MMTrLeg1_Mu1*Eff_MMTrLeg2_Mu2*Eff_DZmm;
      
    }
    else if(NEl==2 && NMu==1){
      float pt_m   = MuColl.at(0).Pt()  , pt_e1   = EleColl.at(0).Pt()  , pt_e2   = EleColl.at(1).Pt()  ;
      float eta_m  = MuColl.at(0).Eta() , eta_e1  = EleColl.at(0).scEta() , eta_e2  = EleColl.at(1).scEta() ;
      eta_e1 = min(max(eta_e1,((float)-1.)*MaxfEta1),MaxfEta1), eta_e2 = min(max(eta_e2,((float)-1.)*MaxfEta1),MaxfEta1);
      float feta_m = fabs(eta_m)        , feta_e1 = fabs(eta_e1)       , feta_e2 = fabs(eta_e2)       ;
      float Eff_EMe = 0., Eff_EMm = 0., Eff_EMTrElLeg_El1 = 0., Eff_EMTrElLeg_El2 = 0., Eff_EETrLeg1_El1 = 0., Eff_EETrLeg2_El2 = 0.;
      float Eff_DZem = DZEfficiency("EMuIso", ReturnDataEff, "");
      float Eff_DZee = DZEfficiency("DiElIso", ReturnDataEff, "");
      if(pt_m>MinPt5){
        Eff_EMTrElLeg_El1 = HistEff2->GetBinContent(HistEff2->FindBin(min(pt_e1,MaxPt2), feta_e1));
        Eff_EMTrElLeg_El2 = HistEff2->GetBinContent(HistEff2->FindBin(min(pt_e2,MaxPt2), feta_e2));
        Eff_EMe = Eff_EMTrElLeg_El1 + (1.-Eff_EMTrElLeg_El1*Eff_DZem)*Eff_EMTrElLeg_El2;
      }
      else{
        Eff_EMTrElLeg_El1 = HistEff1->GetBinContent(HistEff1->FindBin(min(pt_e1,MaxPt1), feta_e1));
        Eff_EMTrElLeg_El2 = HistEff1->GetBinContent(HistEff1->FindBin(min(pt_e2,MaxPt1), feta_e2));
        Eff_EMe = Eff_EMTrElLeg_El1 + (1.-Eff_EMTrElLeg_El1*Eff_DZem)*Eff_EMTrElLeg_El2;
      }
      if(pt_e1>MinPt3){
        Eff_EETrLeg1_El1 = HistEff3->GetBinContent(HistEff3->FindBin(eta_e1, min(pt_e1,MaxPt3))); 
        Eff_EETrLeg2_El2 = HistEff4->GetBinContent(HistEff4->FindBin(eta_e2, min(pt_e2,MaxPt4))); 
      }
      if(pt_e1>MinPt1){
        Eff_EMm = HistEff6->GetBinContent(HistEff6->FindBin(min(pt_m,MaxPt6), feta_m));
      }
      else{
        Eff_EMm = HistEff5->GetBinContent(HistEff5->FindBin(min(pt_m,MaxPt5), feta_m));
      }
      TriggerEff = Eff_EMe*Eff_EMm*Eff_DZem+(1-Eff_EMm*Eff_DZem)*Eff_EETrLeg1_El1*Eff_EETrLeg2_El2*Eff_DZee;
    }
  } 

  TriggerEff = min(TriggerEff,(float) 1.);
  return TriggerEff;
}


float MCCorrection::DZEfficiency(TString SFKey, bool ReturnDataEff, TString Option){

  float Eff=0.;
  if(SFKey.Contains("DiMuIso")){
    if(DataEra=="2016postVFP") Eff = ReturnDataEff? 0.9798:0.9969;
    else if(DataEra=="2017"  ) Eff = 0.9958;
    else                       Eff = 1.;
  }
  else if(SFKey.Contains("DiElIso")){
    if     (DataEra=="2016preVFP" ) Eff = 0.986;
    else if(DataEra=="2016postVFP") Eff = 0.980;
    else                            Eff = 1.;
  }
  else if(SFKey.Contains("EMuIso")){
    if(DataEra=="2016postVFP") Eff = ReturnDataEff? 0.9648:0.9882;
    //else if(DataEra=="2017"  ) Eff = 0.9951; //for now included in muleg
    else                       Eff = 1.;
  }


  return Eff;
}
