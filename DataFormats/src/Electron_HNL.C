#include "Electron.h"

int  Electron::PassHNLTight(TString ID) const{

  /// These are Finalised Tight IDs used in POG/HNL or other analysis                                                                                     

  /// Loose MVA used to train                                                                                                                                         
  if(ID=="MVALoose")  return BtoI(Pass_LepMVAID());

  // breakdown of ID for checking Eff.                                                                                                                                
  if(ID=="HNL_ULID_Baseline")        return BtoI(PassMVABaseLine());
  if(ID=="HNL_ULID_TrkIso_Baseline") return BtoI(PassMVABaseLineTrkIso());

  ////////////////////////////////////////////////////////////////////////////////////                                                            
  ////// HNL UltraLegacy ID (HNL_ULID)                                                                                                                                                                                
  ////////////////////////////////////////////////////////////////////////////////////                                                        
  ////////////////////////////////////////////////////////////////////////////////////                                                                                                                                

  ////////////////////////////////////////////////////////////////////////////////////                                                                                                                                
  ///  Era Scanned ID                                                                                                                                                                                                 
  ////////////////////////////////////////////////////////////////////////////////////                                                                                                                                
  TString Year = this->sRun_Year();
  TString Era  = this->Run_Era();

  //// TC = Tight Charge                                                                                                                                                                                              
  //// *TC* ID are IDs before updating Charge cut to a MEDIUM charge cut to reduce CF at high pt                                                                                                                      
  if(ID == "HNL_TC0_ULID")     return BtoI(PassID("HNL_ULID_Defv1_FO") && PassID("HNL_ULID_Fake"));
  if(ID == "HNL_TC1_ULID")     return BtoI(PassID("HNL_ULID_Defv1_FO") && PassID("HNL_ULID_Fake") && IsGsfCtfChargeConsistent());
  if(ID == "HNL_TC2_ULID")     return BtoI(PassID("HNL_ULID_Defv1_FO") && PassID("HNL_ULID_Fake") && IsGsfScPixChargeConsistent());
  if(ID == "HNL_TC3_ULID")     return BtoI(PassID("HNL_ULID_Defv1_FO") && PassID("HNL_ULID_Fake") && IsGsfCtfChargeConsistent() && IsGsfScPixChargeConsistent());

  //// HNL_ULID_Defv2_FO_ has Medium Charge cut on top of HNL_ULID_Defv1_FO_                                                                                                                                            
  if(ID == "HNL_ULID")         return BtoI(PassID("HNL_ULID_Defv2_FO") && PassID("HNL_ULID_Fake"));

  ////// Check variation on conversion cut
  if(ID == "HNL_ULID_ConvHighPt") return BtoI(PassID("HNL_ULID_Defv3_FO") && PassID("HNL_ULID_Fake_HighPt"));
  if(ID == "HNL_ULID_NoConv")    return BtoI(PassID("HNL_ULID_Defv4_FO") && PassID("HNL_ULID_Fake_HighPt"));


  if(ID == "HNL_ULID_BaseLineCC")   return BtoI(PassMVABaseLine()&&IsGsfCtfChargeConsistent());

  //// version 1 with no Charge Cut
  if(ID == "HNL_ULID_NoMedCC")   return BtoI(PassID("HNL_ULID_Defv1_FO") && PassID("HNL_ULID_Fake"));

  /// version 2 with charge cut
  if(ID == "HNL_ULID_"+Year)    return BtoI(PassID("HNL_ULID_Defv2_FO") && PassID("HNL_ULID_Fake"));

  if(ID == "HNL_ULID_TrkIso")    return BtoI(PassID("HNL_ULID_TrkIso_Conv")   &&
                                             PassID("HNL_ULID_TrkIso_CF")     &&
					     PassID("HNL_ULID_TrkIso_Fake"));



  /// Version 3 with High Pt turn off
  if(ID == "HNL_HighPt_ULID_"+Year)      return BtoI(PassID("HNL_ULID_Defv3_FO") && PassID("HNL_ULID_Fake_HighPt"));
  if(ID == "HNL_HighPt_ULID")            return BtoI(PassID("HNL_ULID_Defv3_FO") && PassID("HNL_ULID_Fake_HighPt"));
  
  // Loose Fake IDs [FO]                            
               
  // HNL_ULID_Defv1_FO* are OLD FO ie before changing Charge cut                                                                                                                                                     
  if(ID == "HNL_ULID_Defv1_FO")   return BtoI(PassID("HNL_ULID_Conv")   && PassID("HNL_ULID_CF"));
  // HNL_ULID_Defv2_FO_ is new default FO ID WIth medium charge cut                                                                                                                                                     
  if(ID == "HNL_ULID_Defv2_FO")    return BtoI(PassID("HNL_ULID_Conv")         && PassID("HNL_ULID_CF") &&  IsGsfCtfChargeConsistent());

  /// v3 has removed CONV MVA at high pt
  if(ID == "HNL_ULID_Defv3_FO")    return BtoI(PassID("HNL_ULID_Conv_HighPt")  && PassID("HNL_ULID_CF") &&  IsGsfCtfChargeConsistent());

  //// Version 4+ are used for checking Efficiency
  /// v4 has no CONV MVA
  if(ID == "HNL_ULID_Defv4_FO")    return BtoI(PassID("HNL_ULID_CF") &&  IsGsfCtfChargeConsistent());
  /// v5 has no CF &&CONV
  if(ID == "HNL_ULID_Defv5_FO")   return BtoI(PassMVABaseLine()&&IsGsfCtfChargeConsistent());
  if(ID == "HNL_ULID_Defv3")   return BtoI(PassID("HNL_ULID_Conv_HighPt")   && PassID("HNL_ULID_CF") &&  IsGsfCtfChargeConsistent()&& PassID("HNL_ULID_Fake_HighPt") );
  if(ID == "HNL_ULID_Defv4")   return BtoI(PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_HighPt")   &&  IsGsfCtfChargeConsistent());
  if(ID == "HNL_ULID_Defv5")   return BtoI(PassMVABaseLine()&&IsGsfCtfChargeConsistent()&& PassID("HNL_ULID_Fake_HighPt") );
  if(ID == "HNL_ULID_Defv6")   return BtoI(PassID("HNL_ULID_Conv_HighPt") && PassID("HNL_ULID_Fake_HighPt")   &&  IsGsfCtfChargeConsistent());

  ///// SPLIT ID for N-1 Eff
  if(ID == "HNL_ULID_Split_1")  return BtoI(Pass_LepMVAID_split1());
  if(ID == "HNL_ULID_Split_2")  return BtoI((Pass_LepMVAID_split1()  &&  Pass_LepMVAID_split2()));
  if(ID == "HNL_ULID_Split_3")  return BtoI((Pass_LepMVAID_split1()  &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()));
  if(ID == "HNL_ULID_Split_4")  return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4()));
  if(ID == "HNL_ULID_Split_4b") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4b()));
  if(ID == "HNL_ULID_Split_5")  return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()));
  if(ID == "HNL_ULID_Split_5b") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfScPixChargeConsistent()));
  if(ID == "HNL_ULID_Split_6")  return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF")));
  if(ID == "HNL_ULID_Split_7")  return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake")));
  if(ID == "HNL_ULID_Split_7b") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_HighPt")));
  if(ID == "HNL_ULID_Split_7c") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_m0p6")));
  if(ID == "HNL_ULID_Split_7d") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_m0p4")));
  if(ID == "HNL_ULID_Split_7e") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_m0p2")));
  if(ID == "HNL_ULID_Split_7f") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_0")));
  if(ID == "HNL_ULID_Split_7g") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_0p1")));
  if(ID == "HNL_ULID_Split_7h") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake_0p2")));

  if(ID == "HNL_ULID_Split_8")  return BtoI(PassID("HNL_ULID_Split_7") && PassID("HNL_ULID_Conv"));
  if(ID == "HNL_ULID_Split_8b") return BtoI(PassID("HNL_ULID_Split_7") && PassID("HNL_ULID_Conv_HighPt"));

  if(ID == "HNL_ULID_Probe_Split_2") return BtoI(Pass_LepMVAID_split1());
  if(ID == "HNL_ULID_Probe_Split_3") return BtoI(Pass_LepMVAID_split1()  &&  Pass_LepMVAID_split2());
  if(ID == "HNL_ULID_Probe_Split_4") return BtoI(Pass_LepMVAID_split1()  &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3());
  if(ID == "HNL_ULID_Probe_Split_5") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4()));
  if(ID == "HNL_ULID_Probe_Split_6") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()));
  if(ID == "HNL_ULID_Probe_Split_7") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF")));
  if(ID == "HNL_ULID_Probe_Split_8") return BtoI((Pass_LepMVAID_split1() &&  Pass_LepMVAID_split2() && Pass_LepMVAID_split3()  && Pass_LepMVAID_split4() && IsGsfCtfChargeConsistent()  && PassID("HNL_ULID_CF") && PassID("HNL_ULID_Fake")));
  




  //// FO WIth High Pt Fix

  /// FO = Fake Obj, is main obj in NP bkg estimate                                                                                                                                                                                         
  if(ID == "HNL_HighPt_ULID_FO")  return BtoI(PassID("HNL_HighPt_ULID_FO_v9_a"));
  if(ID == "HNL_HighPt_ULID_FO_Down") return BtoI(PassID("HNL_HighPt_ULID_FO_v0"));

  if(Era=="2016a"){
    if(ID == "HNL_HighPt_ULID_FO_Up")   return BtoI(PassID("HNL_HighPt_ULID_FO_v8_a"));
  }
  if(Era=="2016b"){
    if(ID == "HNL_HighPt_ULID_FO_Up")   return BtoI(PassID("HNL_HighPt_ULID_FO_v8_a"));
  }
  if(Era=="2017"){
    if(ID == "HNL_HighPt_ULID_FO_Up")      return BtoI(PassID("HNL_HighPt_ULID_FO_v8_a"));
  }
  if(Era=="2018"){
    if(ID == "HNL_HighPt_ULID_FO_Up")      return BtoI(PassID("HNL_HighPt_ULID_FO_v8_a"));
  }

  //// Now List Fake DeepJet Scan IDs v1-9 a,b,c....                                                                                                                                                                                        
  double CloseJet_PtratioCut = 0.4;
  if(Year=="2016") CloseJet_PtratioCut = 0.5;

  if(ID.Contains("HNL_HighPt_ULID_FO_")){

    ///// V0 ID is BASIC loose ID with NO DEEPJET cuts                                                                                                                                                                                     
                                                                                                                                                                                                                                            
    double BJetDeepJetCut = GetDeepJetFromID(ID,"HNL_HighPt_ULID_FO_") ;

    bool ApplyCloseJet_CvsBScore=ApplyCvsB(ID);
    bool ApplyCloseJet_CvsLScore=ApplyCvsB(ID);
    double CBJetDeepJetCut = GetCvsBCut(ID);
    double CLJetDeepJetCut = GetCvsLCut(ID);

    if(!PassID("HNL_ULID_Defv3_FO")) return 0;
    if(!PassID("HNL_ULID_Fake_HighPt")){
      if(this->HasCloseJet()){
        if(CloseJet_Ptratio() < CloseJet_PtratioCut)     return 0;
        if(this->CloseJet_BScore()    >  BJetDeepJetCut) return 0;
        if(ApplyCloseJet_CvsBScore && this->CloseJet_CvsBScore() <  CBJetDeepJetCut ) return 0;
        if(ApplyCloseJet_CvsLScore && this->CloseJet_CvsLScore() >  CLJetDeepJetCut ) return 0;
      }
    }
    return 1;
  }


  /// FO = Fake Obj, is main obj in NP bkg estimate                                                                                                                                                                
  if(ID == "HNL_ULID_FO")  return BtoI(PassID("HNL_ULID_FO_v9_a"));
  if(ID == "HNL_ULID_FO_Down") return BtoI(PassID("HNL_ULID_FO_v0"));

  if(Era=="2016a"){
    if(ID == "HNL_ULID_FO_Up")   return BtoI(PassID("HNL_ULID_FO_v8_a"));
  }
  if(Era=="2016b"){
    if(ID == "HNL_ULID_FO_Up")   return BtoI(PassID("HNL_ULID_FO_v8_a"));
  }
  if(Era=="2017"){
    if(ID == "HNL_ULID_FO_Up")      return BtoI(PassID("HNL_ULID_FO_v8_a"));
  }
  if(Era=="2018"){
    if(ID == "HNL_ULID_FO_Up")      return BtoI(PassID("HNL_ULID_FO_v8_a"));
  }

  //// Now List Fake DeepJet Scan IDs v1-9 a,b,c....                                                                                                                                                                  

  if(ID.Contains("HNL_ULID_FO_")){

    ///// V0 ID is BASIC loose ID with NO DEEPJET cuts                                                                                                                                                                                                                                                                
    double BJetDeepJetCut = GetDeepJetFromID(ID,"HNL_ULID_FO_") ;

    bool ApplyCloseJet_CvsBScore=ApplyCvsB(ID);
    bool ApplyCloseJet_CvsLScore=ApplyCvsB(ID);
    double CBJetDeepJetCut = GetCvsBCut(ID);
    double CLJetDeepJetCut = GetCvsLCut(ID);

    if(!PassID("HNL_ULID_Defv2_FO")) return 0;
    if(!PassID("HNL_ULID_Fake")){
      if(this->HasCloseJet()){
        if(CloseJet_Ptratio() < CloseJet_PtratioCut)     return 0;
        if(this->CloseJet_BScore()    >  BJetDeepJetCut) return 0;
        if(ApplyCloseJet_CvsBScore && this->CloseJet_CvsBScore() <  CBJetDeepJetCut ) return 0;
        if(ApplyCloseJet_CvsLScore && this->CloseJet_CvsLScore() >  CLJetDeepJetCut ) return 0;
      }
    }
    return 1;
  }

  /// OLD FUNCTION OLNY USed now for comparing perfomance                                                                                                         

  if(ID.Contains("HNL_ULID_v1_FO_")){

    double BJetDeepJetCut = GetDeepJetFromID(ID,"HNL_ULID_v1_FO_") ;

    bool ApplyCloseJet_CvsBScore=ApplyCvsB(ID);
    bool ApplyCloseJet_CvsLScore=ApplyCvsB(ID);
    double CBJetDeepJetCut = GetCvsBCut(ID);
    double CLJetDeepJetCut = GetCvsLCut(ID);

    if(!PassID("HNL_ULID_Defv1_FO")) return 0;
    if(!PassID("HNL_ULID_Fake")){
      if(this->HasCloseJet()){
        if(CloseJet_Ptratio() < CloseJet_PtratioCut)         return 0;
        if(this->CloseJet_BScore()    >  BJetDeepJetCut) return 0;
        if(ApplyCloseJet_CvsBScore && this->CloseJet_CvsBScore() <  CBJetDeepJetCut ) return 0;
        if(ApplyCloseJet_CvsLScore && this->CloseJet_CvsLScore() >  CLJetDeepJetCut ) return 0;
      }
    }
    return 1;
  }

  ////////////// SINGLE MVA CUTS [Split for ID SF]                                                                                                                                                                    

  ////  ID cuts is Cut1*CUt2*Cut3, for each era... Here we define and apply each cut                                                                                                                                  
  /// 2016                                                                                                                                                  
  if(ID == "HNL_ULID_Fake_m0p8" ) return BtoI(Pass_MVA_BBEC("Fake_EDv5", -0.8,-0.8,   "Fake_v5"));
  if(ID == "HNL_ULID_Fake_m0p6" ) return BtoI(Pass_MVA_BBEC("Fake_EDv5", -0.6,-0.6,   "Fake_v5"));
  if(ID == "HNL_ULID_Fake_m0p4" ) return BtoI(Pass_MVA_BBEC("Fake_EDv5", -0.4,-0.4,   "Fake_v5"));
  if(ID == "HNL_ULID_Fake_m0p2" ) return BtoI(Pass_MVA_BBEC("Fake_EDv5", -0.2,-0.2,   "Fake_v5"));
  if(ID == "HNL_ULID_Fake_0" )   return  BtoI(Pass_MVA_BBEC("Fake_EDv5", 0 , 0,   "Fake_v5"));
  if(ID == "HNL_ULID_Fake_0p1" ) return  BtoI(Pass_MVA_BBEC("Fake_EDv5", 0.1,0.1,   "Fake_v5"));
  if(ID == "HNL_ULID_Fake_0p2" ) return  BtoI(Pass_MVA_BBEC("Fake_EDv5", 0.2,0.2,   "Fake_v5"));

                                                     
  if(ID == "HNL_ULID_Conv_Lone" ) {
    if(this->Pt() > 150) return 1;
    return BtoI( Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,60, -0.7,0.) , GetPtSlopeCut(20,60,-0.7,0.) ,   "Conv_v5"));
  }     

  
  if(Run_Year()==2016){
    if(ID == "HNL_ULID_Conv" ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,60, -0.7,0.) , GetPtSlopeCut(20,60,-0.7,0.) ,   "Conv_v5"));
    if(ID == "HNL_ULID_CF"   ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("CF_EDv5",   el_mva_cut_cf_2016_B,   el_mva_cut_cf_2016_EC,     "CF_v5"));
    if(ID == "HNL_ULID_Fake" ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2016_B, el_mva_cut_fake_2016_EC,   "Fake_v5"));

    if(ID == "HNL_ULID_TrkIso_Conv" ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,60, -0.7,0.) , GetPtSlopeCut(20,60,-0.7,0.) ,   "Conv_v5"));
    if(ID == "HNL_ULID_TrkIso_CF"   ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("CF_EDv5",   el_mva_cut_cf_2016_B,   el_mva_cut_cf_2016_EC,     "CF_v5"));
    if(ID == "HNL_ULID_TrkIso_Fake" ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2016_B, el_mva_cut_fake_2016_EC,   "Fake_v5"));

    if(ID == "HNL_ULID_Fake_HighPt" ) {
      double barrelcut=0.;
      double endcapcut=0.;
      double ptlow = 150;
      double pthigh=200;
      if(this->Pt() < ptlow) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2016_B, el_mva_cut_fake_2016_EC,   "Fake_v5"));
      if(this->Pt() > pthigh) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", barrelcut, endcapcut,   "Fake_v5"));
      else return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", GetPtSlopeCut(ptlow,pthigh, el_mva_cut_fake_2016_B,barrelcut), GetPtSlopeCut(ptlow,pthigh, el_mva_cut_fake_2016_EC,endcapcut),   "Fake_v5"));
    }


    if(ID == "HNL_ULID_Conv_HighPt" ) {
      double ptlow = 150;
      double pthigh= 200;
      if(this->Pt() > pthigh) return 1;
      if(this->Pt()  < ptlow ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,50, -0.7,0.) , GetPtSlopeCut(20,50,-0.7,0.) ,   "Conv_v5"));
      else return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(ptlow,pthigh, 0.,-1) , GetPtSlopeCut(ptlow,pthigh,0.,-1) ,   "Conv_v5"));
    }


  }
  /// 2017                                                                                                                                                                                     
  else   if(Run_Year()==2017){

    if(ID == "HNL_ULID_Conv" ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,50, -0.4,0.) , GetPtSlopeCut(20,50,-0.4,0.2) ,   "Conv_v5"));
    if(ID == "HNL_ULID_CF"   ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("CF_EDv5",   el_mva_cut_cf_2017_B,   el_mva_cut_cf_2017_EC,     "CF_v5"));
    if(ID == "HNL_ULID_Fake" ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2017_B, el_mva_cut_fake_2017_EC,   "Fake_v5"));

    if(ID == "HNL_ULID_TrkIso_Conv" ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,50, -0.4,0.) , GetPtSlopeCut(20,50,-0.4,0.2) ,   "Conv_v5"));
    if(ID == "HNL_ULID_TrkIso_CF"   ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("CF_EDv5",   el_mva_cut_cf_2017_B,   el_mva_cut_cf_2017_EC,     "CF_v5"));
    if(ID == "HNL_ULID_TrkIso_Fake" ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2017_B, el_mva_cut_fake_2017_EC,   "Fake_v5"));

    if(ID == "HNL_ULID_Fake_HighPt" ) {
      double barrelcut=0.;
      double endcapcut=0.;
      double ptlow = 150;
      double pthigh=200;
      if(this->Pt() < ptlow) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2017_B, el_mva_cut_fake_2017_EC,   "Fake_v5"));
      if(this->Pt() > pthigh) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", barrelcut, endcapcut,   "Fake_v5"));
      else return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", GetPtSlopeCut(ptlow,pthigh, el_mva_cut_fake_2017_B,barrelcut), GetPtSlopeCut(ptlow,pthigh, el_mva_cut_fake_2017_EC,endcapcut),   "Fake_v5"));
    }


    if(ID == "HNL_ULID_Conv_HighPt" ) {
      double ptlow = 150;
      double pthigh= 200;
      if(this->Pt() > pthigh) return 1;
      if(this->Pt()  < ptlow ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,50, -0.4,0.) , GetPtSlopeCut(20,50,-0.4,0.2) ,   "Conv_v5"));
      else return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(ptlow,pthigh, 0.,-1) , GetPtSlopeCut(ptlow,pthigh,0.2,-1) ,   "Conv_v5"));
    }


  }
  else{

    if(ID == "HNL_ULID_Conv" ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,50, -0.4,0.2) , GetPtSlopeCut(20,50,-0.4,0.2) ,   "Conv_v5"));
    if(ID == "HNL_ULID_CF"   ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("CF_EDv5",   el_mva_cut_cf_2018_B ,  el_mva_cut_cf_2018_EC ,     "CF_v5")) ;
    if(ID == "HNL_ULID_Fake" ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2018_B, el_mva_cut_fake_2018_EC ,   "Fake_v5"));

    if(ID == "HNL_ULID_TrkIso_Conv" ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,50, -0.4,0.2) , GetPtSlopeCut(20,50,-0.4,0.2) ,   "Conv_v5"));
    if(ID == "HNL_ULID_TrkIso_CF"   ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("CF_EDv5",   el_mva_cut_cf_2018_B ,  el_mva_cut_cf_2018_EC ,     "CF_v5")) ;
    if(ID == "HNL_ULID_TrkIso_Fake" ) return BtoI(PassMVABaseLineTrkIso() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2018_B, el_mva_cut_fake_2018_EC ,   "Fake_v5"));


    if(ID == "HNL_ULID_Fake_HighPt" ) {
      double barrelcut=0.;
      double endcapcut=0.;
      double ptlow = 150;
      double pthigh=200;
      if(this->Pt() < ptlow) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", el_mva_cut_fake_2018_B, el_mva_cut_fake_2018_EC,   "Fake_v5"));
      if(this->Pt() > pthigh) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", barrelcut, endcapcut,   "Fake_v5"));
      else return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Fake_EDv5", GetPtSlopeCut(ptlow,pthigh, el_mva_cut_fake_2018_B,barrelcut), GetPtSlopeCut(ptlow,pthigh, el_mva_cut_fake_2018_EC,endcapcut),   "Fake_v5"));
    }

    if(ID == "HNL_ULID_Conv_HighPt" ) {
      
      double ptlow = 150;
      double pthigh=200;

      if(this->Pt() > pthigh) return 1;
      if(this->Pt()  < ptlow ) return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(20,50, -0.4,0.2) , GetPtSlopeCut(20,50,-0.4,0.2) ,   "Conv_v5"));
      else return BtoI(PassMVABaseLine() && Pass_MVA_BBEC("Conv_EDv5", GetPtSlopeCut(ptlow,pthigh, 0.2,-1) , GetPtSlopeCut(ptlow,pthigh,0.2,-1) ,   "Conv_v5"));      
    }

  }
  return -1;
}



bool Electron::Pass_LepMVATopID() const {

  /// ID USED IN BDT by TOP group             
                                                                                                                                                                                                                   
  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;
  if(MiniRelIso() > 0.4) return false;
  if(NMissingHits() > 1) return false;
  if(SIP3D() > 8) return false;
  if(this->fdXY() > 0.05) return false;
  if(this->fdZ() > 0.1) return false;
  return true;

}



bool Electron::Pass_LepMVAID_TrkIso() const {

  /// ID USED IN BDT == TopMVA LooseID + trigger emulation                                                                                                                                  
                                                                                                                                                                                                    

  if(!passMVAID_noiso_WPLoose()) return false;
  if(!Pass_TriggerEmulationLoose())  return false;
  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;
  //  if(MiniRelIso() > 0.4) return false;
  if(! (TrkIso()<20.) ) return false;
  if(NMissingHits() > 1) return false;
  if(SIP3D() > 8) return false;
  if(this->fdXY() > 0.05) return false;
  if(this->fdZ() > 0.1) return false;
  return true;

}



bool Electron::Pass_LepMVAID() const {

  /// ID USED IN BDT == TopMVA LooseID + trigger emulation                                                                                                                                                            

  if(!passMVAID_noiso_WPLoose()) return false;
  if(!Pass_TriggerEmulationLoose())  return false;
  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;
  if(MiniRelIso() > 0.4) return false;
  if(NMissingHits() > 1) return false;
  if(SIP3D() > 8) return false;
  if(this->fdXY() > 0.05) return false;
  if(this->fdZ() > 0.1) return false;
  return true;

}


bool Electron::Pass_LepMVAID_split1() const {

  //// Trigger emulation
  if(!Pass_TriggerEmulationLoose())  return false;
  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;
  return true;

}

bool Electron::Pass_LepMVAID_split2() const {

  if(!passMVAID_noiso_WPLoose()) return false;
  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;
  return true;
}


bool Electron::Pass_LepMVAID_split3() const {

  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;

  if(SIP3D() > 8) return false;
  if(this->fdXY() > 0.05) return false;
  if(this->fdZ() > 0.1) return false;
  return true;

}

bool Electron::Pass_LepMVAID_split4() const {

  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;
  if(MiniRelIso() > 0.4) return false;
  if(NMissingHits() > 1) return false;
  return true;

}

bool Electron::Pass_LepMVAID_split4b() const {

  if(this->Pt() < 10) return false;
  if(this->fEta() > 2.5) return false;
  if(! (TrkIso()<20.) ) return false;
  if(NMissingHits() > 1) return false;
  return true;

}






bool Electron::Pass_HNTight(double relisoCut, double dxyCut1, double dxyCut2, double dzCut, double ipcut1, double ipcut2) const{
  if(! (passTightID()) ) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY())<dxyCut1 && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<ipcut1 ))   return false;
  }
  else{
    if(! (fabs(dXY())<dxyCut2 && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<ipcut2 ))   return false;

  }
  if(! IsGsfCtfScPixChargeConsistent()) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  return true;
}

bool Electron::Pass_HNMVALoose(double relisoCut, double dxyCut, double dzCut) const{
  if(!( passMVAID_noiso_WPLoose() )) return false;
  if(! (RelIso()<relisoCut) ) return false;
  return true;
}

bool Electron::Pass_HNMVATight(double relisoCut, double dxyCut, double dzCut) const{
  if(!( passMVAID_noIso_WP90() )) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;
  return true;
}

bool Electron::Pass_HNVetoMVA() const{

  if(!passMVAID_noiso_WPLoose()) return false;

  if(! (fabs(dXY())<0.2 && fabs(dZ())<0.5) ) return false;

  if(! (MiniRelIso() < 0.4) ) return false; /// change to mini iso to be consistent with LooseMVA cut                                                                                                                 

  return true;
}

bool Electron::Pass_HNLoose2016MVAISO(double dxyCut, double dzCut, double sipCut) const{

  if( fabs(scEta()) <= 0.8 ){
    if(! (MVAIso()>-0.99) ) return false;
  }
  else if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVAIso()>-0.99) ) return false;
  }
  else{
    if(! (MVAIso()>-0.99) ) return false;
  }
  if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<sipCut) ) return false;

  if(! (PassConversionVeto()) ) return false;
  if(! IsGsfCtfScPixChargeConsistent())  return false;

  if(! (Pass_TriggerEmulation()) ) return false;

  return true;
}

bool Electron::PassHNID() const{

  if(! (PassConversionVeto()) ) return false;
  if(! IsGsfCtfScPixChargeConsistent())  return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  return true;
}


bool Electron::Pass_HNLoose2016MVANonIso(double relisoCut, double dxyCut, double dzCut, double sipCut) const{
  if( fabs(scEta()) <= 0.8 ){
    if(! (MVANoIso()>-0.99) ) return false;
  }
  else if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVANoIso()>-0.99) ) return false;
  }
  else{
    if(! (MVANoIso()>-0.99) ) return false;
  }
  if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<sipCut) ) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! IsGsfCtfScPixChargeConsistent())  return false;

  if(! (Pass_TriggerEmulation()) ) return false;

  return true;
}




bool Electron::Pass_HNLoose2016(double relisoCut, double dxyCut, double dzCut, double sipCut) const{
  if( fabs(scEta()) <= 0.8 ){
    if(! (MVANoIso()>-0.99) ) return false;
  }
  else if( fabs(scEta()) > 0.8 && fabs(scEta()) <= 1.479 ){
    if(! (MVANoIso()>-0.99) ) return false;
  }
  else{
    if(! (MVANoIso()>-0.99) ) return false;
  }
  if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<sipCut) ) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;

  if(! (Pass_TriggerEmulation()) ) return false;

  return true;
}



bool Electron::passMediumIDHN() const{
  if(! (passMediumID()) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (Pass_TriggerEmulation()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) )return false;

  return true;
}



bool Electron::Pass_HNTight2016() const{


  if(!PassMVA_2016(0.9, 0.825, 0.5)) return false;

  if(! (fabs(dXY())<0.01 && fabs(dZ())<0.04 && fabs(IP3D()/IP3Derr())<4.) ) return false;
  if(! (RelIso()<0.08) ) return false;
  if(! (PassConversionVeto()) ) return false;
  if(! (IsGsfCtfScPixChargeConsistent()) ) return false;

  if(! (Pass_TriggerEmulation()) ) return false;

  return true;
}


//===============================================                                                                                                                                                                     
//==== Run2 ID                                                                                                                                                                                                        
//===============================================                                                                                                                                                                     

bool Electron::Pass_HNVetoCutBased(double relisoCut) const{

  if(! (Pass_CutBasedVetoNoIso()) ) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if(! (fabs(dXY())<0.2 && fabs(dZ())<0.5) ) return false;
  return true;
}

bool Electron::Pass_HNLoosest() const{

  if(! (RelIso()< 0.6 || MiniRelIso() < 0.4) ) return false;
  if(!( Pass_CutBasedVetoNoIso() || passMVAID_noiso_WPLoose()))  return false;

  return true;
}
bool Electron::Pass_HNLoose(double relisoCut, double dxyCut, double dzCut) const{

  return true;
}

bool Electron::Pass_HNLooseID(double relisoCut, double dxyCut, double dzCut, double ipcut) const{

  if(! (Pass_CutBasedVetoNoIso()) ) return false;

  if(! (RelIso()<relisoCut) ) return false;
  if(! (fabs(dXY())<dxyCut && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<ipcut    )) return false;
  if(! IsGsfCtfScPixChargeConsistent()) return false;

  if(! (Pass_TriggerEmulation()) ) return false;
  return true;

}

bool Electron::Pass_HNMedium(double relisoCut, double dxyCut1, double dxyCut2, double dzCut, double ipcut1, double ipcut2) const{

  if(! (passMediumID()) ) return false;
  if(! (RelIso()<relisoCut) ) return false;
  if( fabs(scEta()) <= 1.479 ){
    if(! (fabs(dXY())<dxyCut1 && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<ipcut1 ))   return false;
  }
  else{
    if(! (fabs(dXY())<dxyCut2 && fabs(dZ())<dzCut && fabs(IP3D()/IP3Derr())<ipcut2 ))   return false;

  }

  if(! IsGsfCtfScPixChargeConsistent()) return false;

  if(! (Pass_TriggerEmulation()) ) return false;
  return true;
}



