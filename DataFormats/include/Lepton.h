#ifndef Lepton_h
#define Lepton_h

#include "Particle.h"
#include <map>

class Lepton: public Particle {
public:

  Lepton();
  ~Lepton();

  void  PrintObject(TString label);


  inline bool HasCloseJet() const{
    /// Some low Pt muons have no close jet since Jet Pt cut is 10 GeV in MINIAOD
    /// So here Jet based var are set to a default val
    
    if(j_lep_jetbscore == 0.1 && j_lep_jetcvsbscore == 0.4 && j_lep_jetcvslscore == 0.1) return false;
    return true;
  }
  inline double MVCFCut(TString Year) {

    if(Year=="2016" && IsBB())  return el_mva_cut_cf_2016_B;
    if(Year=="2016" && !IsBB()) return el_mva_cut_cf_2016_EC;
    if(Year=="2017" && IsBB())  return el_mva_cut_cf_2017_B;
    if(Year=="2017" && !IsBB()) return el_mva_cut_cf_2017_EC;
    if(Year=="2018" && IsBB())  return el_mva_cut_cf_2018_B;
    if(Year=="2018" && !IsBB()) return el_mva_cut_cf_2018_EC;

    return -999;
  }

  inline double MVAFakeCut(TString ID, TString Year) {

    if(ID == "HNL_HN3L") return TopMVA_cut;

    if(j_LeptonFlavour==MUON){
      if(Year=="2016")  return mu_mva_cut_fake_2016;
      if(Year=="2017")  return mu_mva_cut_fake_2017;
      if(Year=="2018")  return mu_mva_cut_fake_2018;
    }
    else{
      if(Year=="2016" && IsBB())  return el_mva_cut_fake_2016_B;
      if(Year=="2016" && !IsBB()) return el_mva_cut_fake_2016_EC;
      if(Year=="2017" && IsBB())  return el_mva_cut_fake_2017_B;
      if(Year=="2017" && !IsBB()) return el_mva_cut_fake_2017_EC;
      if(Year=="2018" && IsBB())  return el_mva_cut_fake_2018_B;
      if(Year=="2018" && !IsBB()) return el_mva_cut_fake_2018_EC;
    }
    return -999;
  }

  inline TString LepGenTypeNumberString() const {
    TString Sign = (j_LeptonType <0) ? "minus" : "plus";
    if(fabs(j_LeptonType) == 1) return Sign + "1";
    if(fabs(j_LeptonType) == 2) return Sign + "2";
    if(fabs(j_LeptonType) == 3) return Sign + "3";
    if(fabs(j_LeptonType) == 4) return Sign + "4";
    if(fabs(j_LeptonType) == 5) return Sign + "5";
    if(fabs(j_LeptonType) == 6) return Sign + "6";
    return "Zero";
  }
      


  //// Function to Check Nature of lepton
  inline TString LepGenTypeString() const {
    //// return TString based on Gen type
    if(j_LeptonIsCF) {
      if(j_LeptonIsPromptConv)  return "IsPromptConvCF";
      if((j_LeptonType==1 || j_LeptonType==2))         return "IsPromptCF";
      else if ((j_LeptonType>=4 || j_LeptonType<-4 ))  return "IsConvCF";
      else return "IsFakeCF";
    }
    if(j_LeptonIsPromptConv) return "IsPromptConv";
    if((j_LeptonType==1 || j_LeptonType==2))  return "IsPrompt";
    if( (j_LeptonType<0 && j_LeptonType>=-4)) return "IsFake";
    if(j_LeptonType==3) return "IsEWtau";
    if((j_LeptonType>=4 || j_LeptonType<-4 )) return "IsConv";
    else return "NULL";
  }

  inline TString sLepGenType() const {
    if( j_LeptonType<-4 ) return  "ExtConv";
    if((j_LeptonType>=4 )) return  "IntConv";
    if( (j_LeptonType<0 && j_LeptonType>=-4)) {
      if(j_lep_jetflavour == 5) return "FakeHF_B";
      if(j_lep_jetflavour == 4) return "FakeHF_C";
      if(j_lep_jetflavour == 0) return "FakeLF";
    }
    if(j_LeptonIsPromptConv) return "IntConv";
    if(j_LeptonIsCF) return "CF";
    if(j_LeptonType==1) return "Prompt";
    if( j_LeptonType==2) return "Signal";
    if( j_LeptonType==3) return "Tau";
    return "";
    
  }



  inline TString GetLeptonTypeTString(){
    
    if(j_LeptonType < 0) return "Minus"+TString::Itoa(fabs(j_LeptonType), 10);
    return "Plus"+TString::Itoa(fabs(j_LeptonType), 10);
  }

  inline bool IsPrompt() const {
    if((j_LeptonType==1 || j_LeptonType==2 || j_LeptonType==3)) return true;
    else return false;
  }

  inline bool IsFake() const {
    if( (j_LeptonType<0 && j_LeptonType>=-4)) return true;
    else return false;
  }
  inline bool IsEWtau() const {
    if(j_LeptonType==3) return true;
    else return false;
  }

  inline bool IsConv() const {
    if((j_LeptonType>=4 || j_LeptonType<-4 )) return true;
    if(j_LeptonIsPromptConv) return true;
    else return false;
  }

  inline bool LeptonIsCF() const {return j_LeptonIsCF;}
  inline bool LeptonIsPromptConv() const {return j_LeptonIsPromptConv;}



  enum Flavour{
    NONE, ELECTRON, MUON, TAU
  };

  enum EtaRegion{
    IB, OB, GAP, EC
  };

  inline double defEta() const{
    double lepeta = this->Eta();
    if(j_LeptonFlavour==ELECTRON) lepeta = j_elSCeta;
    return lepeta;
  }


  inline EtaRegion etaRegion() const {
    double sceta = fabs(defEta());
    if( sceta < 0.8 ) return IB;
    else if( sceta < 1.444 ) return OB;
    else if( sceta < 1.566 ) return GAP;
    else return EC;
  }
 
  inline int Region() const {
    double eta = fabs(defEta());
    if( eta < 0.8 ) return 1;
    else if( eta < 1.479 ) return 2;
    else return 3;
  }
  
  inline TString sEtaRegion() const {
    if(j_LeptonFlavour==MUON){

      double eta = fabs(defEta());
      if( fabs(eta) < 0.9 )      return "EtaBin1";
      else if( fabs(eta) < 1.2)  return "EtaBin2";
      else if( fabs(eta) < 2.1 ) return "EtaBin3";
      else  if( fabs(eta) < 2.4 )return "EtaBin4";
      return "EtaX";
    }
    else{

      double eta = defEta();
      if( eta< -2 )         return "scEtaBin1";
      else if( eta< -1.56 ) return "scEtaBin2";
      else if( eta > -1.44 && eta< -0.8 ) return "scEtaBin3";
      else if( eta< 0 )     return "scEtaBin4";
      else if( eta< 0.8 )   return "scEtaBin5";
      else if( eta< 1.444 ) return "scEtaBin6";
      else if( eta< 2 && eta> 1.56) return "scEtaBin7";
      else return "scEtaBin8";
    }    
  }

  inline TString sPtRegion(TString Year, TString Analyzer="CF") const {
    if(Analyzer=="CF"){
      if(j_LeptonFlavour==MUON){
	if( this->Pt() > 10 && this->Pt() < 15 ) return "PtBin1";
	else  if( this->Pt()  < 20 ) return "PtBin2";
	else  if( this->Pt()  < 25 ) return "PtBin3";
	else  if( this->Pt()  < 30 ) return "PtBin4";
	else  if( this->Pt()  < 40 ) return "PtBin5";
	else  if( this->Pt()  < 50 ) return "PtBin6";
	else  if( this->Pt()  < 60 ) return "PtBin7";
	else   return "PtBin8";
	
      }
      else{
	if(Year=="2016"){
	  if( this->Pt() > 10 && this->Pt() < 20 ) return "PtBin1";
	  else  if( this->Pt()  < 35 ) return "PtBin2";
	  else  if( this->Pt()  < 50 ) return "PtBin3";
	  else  if( this->Pt()  < 100 ) return "PtBin4";
	  else   return "PtBin5";
	}
	else{
	  if( this->Pt() > 10 && this->Pt() < 20 ) return "PtBin1";
	  else  if( this->Pt()  < 35 ) return "PtBin2";
	  else  if( this->Pt()  < 50 ) return "PtBin3";
	  else  if( this->Pt()  < 100 ) return "PtBin4";
	  else  if( this->Pt()  < 200 ) return "PtBin5";
	  else   return "Pt6";
	}
      }
    }
    return "PtNULL";
  }
  
  inline TString GetPtLabel(){
    if (this->Pt() < 10.) return "pt_5_10";
    if (this->Pt() < 15.) return "pt_10_15";
    if (this->Pt() < 20.) return "pt_15_20";
    if (this->Pt() < 35.) return "pt_20_35";
    if (this->Pt() < 50.) return "pt_35_50";
    if (this->Pt() < 100.) return "pt_50_100";
    if (this->Pt() < 2000.) return "pt_100_150";
    return "pt_150_2000";
 }
  
  inline TString GetMotherPtLabel(){
    double MotherPt = this->Pt()/ j_lep_jetptratio;
    if (MotherPt < 15.) return "Mpt_10_15";
    if (MotherPt < 20.) return "Mpt_15_20";
    if (MotherPt < 30.) return "Mpt_20_30";
    if (MotherPt < 40.) return "Mpt_30_40";
    if (MotherPt < 50.) return "Mpt_40_50";
    if (MotherPt < 100.) return "Mpt_50_100";
    if (MotherPt < 2000.) return "Mpt_100_2000";
    return "";
  }

  inline TString GetEtaLabel(){
    double eta = fEta();
    if(fabs(eta) < 0.8 ) return "IB";
    if(fabs(eta) < 1.5 ) return "OB";
    if(fabs(eta) < 2.5 ) return "EC";
    return "";

  }

  inline bool IsIB() const { return (Region() == 1); }
  inline bool IsOB() const { return (Region() == 2); }
  inline bool IsEC() const { return (Region() == 3); }
  inline bool IsBB() const { return (Region() < 3); }

  //// HNL UL Funtions
  inline double PtMaxed(double ptmax=2000, double shift=1.) const { 
    double ptshift = this->Pt()*shift;
    if(ptshift > ptmax) return ptmax - 0.0001;
    return ptshift ;
  }


  
  inline double InvPt(double shift=1.) const {
    double ptshift = this->Pt()*shift;
    if(ptshift > 1000) return (1/1000.) - 0.00001;
    return (1/ptshift);
  }

  inline double PtParton(double Corr, double MVACut, double UpperValue=2.){
    double mva_val = j_lep_mva;
    if(j_LeptonFlavour!=MUON) mva_val=j_lep_mva_hnl_fake_ed_v5; 
      
    if (mva_val > MVACut)  return this->Pt();
    double ptpart = ( this->Pt() /j_lep_jetptratio ) * Corr;
    //    if(ptpart > UpperValue*this->Pt() ) return UpperValue*this->Pt() ;
    return ptpart;
  }


  
  /// TEMP Variables to test MVA Top 
  inline double JetNTracksMVA() const { return j_jetntracks_mva;}
  inline double JetNTracks() const { return j_jetntracks;}
  void SetJetNTracks(double d);
  void SetJetNTracksMVA(double d);
    
  /// UncorrectedPt used for Lepton class fuctions
  inline double UncorrectedPt() const { return j_unCorrPt;}
  void SetUncorrectedPt(double d);

  //// MiniAOD level pt
  void SetMiniAODPt(double d);
  inline double MiniAODPt() const {return j_MiniAODPt;}
  
  inline double ElScEta() const {return j_elSCeta;};
  void SetElSCEta(double sceta);
  
  /// Standard Functions

  inline TString Run_Era() const {return j_run_era;}
  inline int Run_Year() const {return j_run_year;}

  inline TString sRun_Year() const {
    if(j_run_year==2016) return "2016";
    if(j_run_year==2017) return "2017";
    if(j_run_year==2018) return "2018";
    return "999";
  }

  void SetdXY(double dXY, double dXYerr);
  inline double fdXY() const {return fabs(j_dXY);}
  inline double dXY() const {return j_dXY;}
  inline double dXYerr() const {return j_dXYerr;}

  inline double LogdXY() const {return std::log(fabs(j_dXY));}
  inline double LogdZ() const {return std::log(fabs(j_dZ));}
  inline double LogdXYSig() const {return std::log(fabs(j_dXY/j_dXYerr));}
  inline double LogdZSig() const {return std::log(fabs(j_dZ/j_dZerr));}

  TString LepDoubleToString(double d) const{

    std::string str = std::to_string (d);
    str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
    str.erase ( str.find_last_not_of('.') + 1, std::string::npos );

    TString ts_str = TString(str);
    //    ts_str = ts_str.ReplaceAll(".","p");
    //ts_str = ts_str.ReplaceAll("-","neg");

    return ts_str;

  }

  inline void SetRunYear(int run_year){
    j_run_year=run_year;
  }
  inline void SetRunEra(TString run_era){
    j_run_era=run_era;
  }

  void SetLepMVA(double mva);
  inline double LepMVA() const {return j_lep_mva;}

  //// BDT ID Functions
  void SetHNL_FakeLepMVAV4(double mvafake,double mvafake_hf,double mvafake_hfb,double mvafake_hfc, double mvafake_lf, double mvafake_top);
  void SetHNL_FakeLepMVA_EtaDependantV4(double mvafake,double mvafake_hf,double mvafake_hfb,double mvafake_hfc, double mvafake_lf, double mvafake_top);

  void SetHNL_FakeLepMVAMuonV4(double mvafake);
  void SetHNL_FakeLepMVAMuon_EtaDependantV4(double mvafake);


  void SetHNL_FakeLepMVA_EtaDependantV5(double mvafake, double mvafake_hfb,double mvafake_hfc, double mvafake_lf); 
  void SetHNL_FakeFlavourLepMVA_V5(double mvafake1,double mvafake2, double mvafake3,double mvafake4);

  void SetHNL_ConvLepMVAV4(double mvaconv);
  void SetHNL_ConvLepMVA_EtaDependantV4(double mvaconv);
  void SetHNL_ConvLepMVA_EtaDependantV5(double mvaconv);

  void SetHNL_CFLepMVAV4(double mvacfv4);
  void SetHNL_CFLepMVA_EtaDependantV4( double mvacfv4);
  void SetHNL_CFLepMVA_EtaDependantV5( double mvacf,double mvacfpt);

  
  map<TString, double> MAPBDTFake() const {
    map<TString, double> _map;
    if(j_LeptonFlavour==ELECTRON){
      _map["El_ED_Fake_v5"]        = j_lep_mva_hnl_fake_ed_v5;
      _map["El_Fake_QCD_LFvsHF_v5"]= j_lep_mva_hnl_fake_QCD_LFvsHF_v5;
      _map["El_Fake_QCD_BvsC_v5"]  = j_lep_mva_hnl_fake_QCD_BvsC_v5;
    }
    else{
      _map["Mu_Fake_QCD_LFvsHF_v5"] = j_lep_mva_hnl_fake_QCD_LFvsHF_v5;
      _map["Mu_Fake_QCD_BvsC_v5"]   = j_lep_mva_hnl_fake_QCD_BvsC_v5;
      _map["Mu_HF_Fake_POG"]        = j_lep_mva;
    }
    return _map;
  }




  map<TString, double> MAPBDT() const {

    map<TString, double> _map;
    if(j_LeptonFlavour==ELECTRON){
      _map["El_Fake_v4"]        = j_lep_mva_hnl_fake_v4;
      _map["El_Fake_v4_HF"]     = j_lep_mva_hnl_fake_v4_hf;
      _map["El_Fake_v4_HFB"]    = j_lep_mva_hnl_fake_v4_hfb;
      _map["El_Fake_v4_HFC"]    = j_lep_mva_hnl_fake_v4_hfc;
      _map["El_Fake_v4_LF"]     = j_lep_mva_hnl_fake_v4_lf;
      _map["El_Fake_v4_Top"]    = j_lep_mva_hnl_fake_v4_top;
      _map["El_ED_Fake_v4"]     = j_lep_mva_hnl_fake_ed_v4;
      _map["El_ED_Fake_v4_HF"]  = j_lep_mva_hnl_fake_ed_v4_hf;
      _map["El_ED_Fake_v4_HFB"] = j_lep_mva_hnl_fake_ed_v4_hfb;
      _map["El_ED_Fake_v4_HFC"] = j_lep_mva_hnl_fake_ed_v4_hfc;
      _map["El_ED_Fake_v4_LF"]  = j_lep_mva_hnl_fake_ed_v4_lf;
      _map["El_ED_Fake_v4_Top"] = j_lep_mva_hnl_fake_ed_v4_top;
      _map["El_Conv_v4"]        = j_lep_mva_hnl_conv_v4;
      _map["El_ED_Conv_v4"]     = j_lep_mva_hnl_conv_ed_v4;
      _map["El_CF_v4"]          = j_lep_mva_hnl_cf_v4;
      _map["El_ED_CF_v4"]       = j_lep_mva_hnl_ed_cf_v4;

      _map["El_ED_CF_v5"]          = j_lep_mva_hnl_ed_cf_v5;
      _map["El_ED_CF_v5Pt"]        = j_lep_mva_hnl_ed_cf_v5Pt;
      _map["El_ED_Conv_v5"]        = j_lep_mva_hnl_conv_ed_v5;
      _map["El_ED_Fake_v5"]        = j_lep_mva_hnl_fake_ed_v5;
      _map["El_Fake_v5_HFB"]    = j_lep_mva_hnl_fake_v5_hfb;
      _map["El_Fake_v5_HFC"]    = j_lep_mva_hnl_fake_v5_hfc;
      _map["El_Fake_v5_LF"]     = j_lep_mva_hnl_fake_v5_lf;
      _map["El_Fake_QCD_LFvsHF_v5"]= j_lep_mva_hnl_fake_QCD_LFvsHF_v5;
      _map["El_Fake_QCD_BvsC_v5"]  = j_lep_mva_hnl_fake_QCD_BvsC_v5;
      _map["El_Fake_QCD_LF1_v5"]    = j_lep_mva_hnl_fake_LF1_v5;
      _map["El_Fake_QCD_LF2_v5"]    = j_lep_mva_hnl_fake_LF2_v5;

    }
    else{
      _map["Mu_LF_Fake_v4"]         = j_lep_mva_hnl_fake_v4;
      _map["Mu_ED_LF_Fake_v4"]      = j_lep_mva_hnl_fake_ed_v4;
      _map["Mu_Fake_QCD_LFvsHF_v5"] = j_lep_mva_hnl_fake_QCD_LFvsHF_v5;
      _map["Mu_Fake_QCD_BvsC_v5"]   = j_lep_mva_hnl_fake_QCD_BvsC_v5;
      _map["Mu_Fake_QCD_LF1_v5"]    = j_lep_mva_hnl_fake_LF1_v5;
      _map["Mu_Fake_QCD_LF2_v5"]    = j_lep_mva_hnl_fake_LF2_v5;
      _map["Mu_HF_Fake_POG"]        = j_lep_mva;
    }
    return _map;
  }


  inline double HNL_MVA_Fake(TString vers="HNL") const {

    ///// v5 is July 23 MVA Trained                                                                                                                                                                                                       
    ///// v4 is Mar - May trained                                                                                                                                                                                                         
    ///// v4 has v4 {BB+EC} and v4ED which trains sep                                                                                                                                                                                       
    ///// v5 has only ED var but the BB lep are actually inclusive                                                                                                                                                                        

    if(j_LeptonFlavour==ELECTRON){
      if(vers=="v4")          return j_lep_mva_hnl_fake_v4;
      else if(vers=="v4HF")   return j_lep_mva_hnl_fake_v4_hf;
      else if(vers=="v4HFB")  return j_lep_mva_hnl_fake_v4_hfb;
      else if(vers=="v4HFC")  return j_lep_mva_hnl_fake_v4_hfc;
      else if(vers=="v4LF")   return j_lep_mva_hnl_fake_v4_lf;
      else if(vers=="v4Top")  return j_lep_mva_hnl_fake_v4_top;
      else if(vers=="EDv4")   return j_lep_mva_hnl_fake_ed_v4;
      else if(vers=="EDv4HF") return j_lep_mva_hnl_fake_ed_v4_hf;
      else if(vers=="EDv4HFB")return j_lep_mva_hnl_fake_ed_v4_hfb;
      else if(vers=="EDv4HFC")return j_lep_mva_hnl_fake_ed_v4_hfc;
      else if(vers=="EDv4LF") return j_lep_mva_hnl_fake_ed_v4_lf;
      else if(vers=="EDv4Top")return j_lep_mva_hnl_fake_ed_v4_top;

      else if(vers=="EDv5")   return j_lep_mva_hnl_fake_ed_v5;
      else if(vers=="v5HFB")  return j_lep_mva_hnl_fake_v5_hfb;
      else if(vers=="v5HFC")  return j_lep_mva_hnl_fake_v5_hfc;
      else if(vers=="v5LF")   return j_lep_mva_hnl_fake_v5_lf;
      else if(vers=="QCD_LFvsHF_v5") return j_lep_mva_hnl_fake_QCD_LFvsHF_v5;
      else if(vers=="QCD_BvsC_v5")   return j_lep_mva_hnl_fake_QCD_BvsC_v5;
      else if(vers=="QCD_LF1_v5")     return j_lep_mva_hnl_fake_LF1_v5;
      else if(vers=="QCD_LF2_v5")     return j_lep_mva_hnl_fake_LF2_v5;      
      else if(vers=="HF")   return j_lep_mva_hnl_fake_v5_hfb;
      else if(vers=="HFB")  return j_lep_mva_hnl_fake_v5_hfb;
      else if(vers=="HFC")  return j_lep_mva_hnl_fake_v5_hfc;
      else if(vers=="LF")   return j_lep_mva_hnl_fake_v5_lf;
      else if(vers=="HNL")  return j_lep_mva_hnl_fake_ed_v5;
      
    }
    else{
      if(vers=="v4")                  return j_lep_mva_hnl_fake_v4;
      else if(vers=="EDv4")           return j_lep_mva_hnl_fake_ed_v4;
      else if(vers=="QCD_LFvsHF_v5")  return  j_lep_mva_hnl_fake_QCD_LFvsHF_v5;
      else if(vers=="QCD_BvsC_v5")    return  j_lep_mva_hnl_fake_QCD_BvsC_v5;
      else if(vers=="QCD_LF1_v5")     return j_lep_mva_hnl_fake_LF1_v5;
      else if(vers=="QCD_LF2_v5")     return j_lep_mva_hnl_fake_LF2_v5;
      else if(vers=="HFTop")          return  j_lep_mva;
      else if(vers=="HF")  return  j_lep_mva;
      else if(vers=="LF")  return  j_lep_mva_hnl_fake_ed_v4;
      else if(vers=="HNL") return  j_lep_mva;

    }
    cout<<"[Lepton::HNL_MVA_Fake] no version set "<< vers<< endl;
    exit(ENODATA);
    
  }

  inline int TagHF() const {
    if(j_LeptonFlavour==ELECTRON){
      if(HNL_MVA_Fake("EDv4HF")<HNL_MVA_Fake("EDv4LF")) return 1;
      else return 0;
    }
    else{
      if(LepMVA()<HNL_MVA_Fake("EDv4")) return 1;
      else return 0;
    }
  }

  inline double HNL_MVA_Conv(TString vers="EDv5") const {
 
    ///// v5 is July 23 MVA Trained
    ///// v2 is Mar - May trained
    ///// v2 has v2 {BB+EC} and ED which trains sep
    ///// v5 has only ED var but the BB lep are actually inclusive
    ///// v2/5 was trainned with Top leptons
    ///// v5p1 was trainned with Top + HNL signals
    ///// v5IP has IP3D variable in training 

    if(j_LeptonFlavour==ELECTRON){
      if(vers=="v4")       return j_lep_mva_hnl_conv_v4;
      else if(vers=="EDv4")return j_lep_mva_hnl_conv_ed_v4;
      else if(vers=="EDv5")return j_lep_mva_hnl_conv_ed_v5;
    }
    else return 1;
    cout<<"[Lepton::HNL_MVA_CONV]"<< endl;
    exit(ENODATA);

  }
  inline double HNL_MVA_CF(TString vers="EDv5") const {


    if(j_LeptonFlavour==ELECTRON){

      if(vers=="v4")return j_lep_mva_hnl_cf_v4;
      if(vers=="EDv4")return j_lep_mva_hnl_ed_cf_v4;
      if(vers=="EDv5")return j_lep_mva_hnl_ed_cf_v5;
      else if(vers=="EDv5Pt")return j_lep_mva_hnl_ed_cf_v5Pt;

    }
    else return 1;
    cout<<"[Lepton::HNL_MVA_CF] " << vers << endl;
    exit(ENODATA);

  }
  
  inline void SetBDT(TString mvaname, double mvavalue){
    j_lep_map_mva_hnl[mvaname] = mvavalue;
  }
  
  map<TString, double> MAPBDTTmp()  {
    return j_lep_map_mva_hnl;
  }
  

  inline double MVAValueTMP (TString idkey)  {

    map<TString, double>::iterator mit = j_lep_map_mva_hnl.find(idkey);
    if(mit != j_lep_map_mva_hnl.end()) return mit->second;
    
    cout<<"[Lepton::MVAValueTMP ERROR] " << idkey << endl;
    exit(ENODATA);
  }

  inline double MVAKey (TString idkey) const {

    if(j_LeptonFlavour==ELECTRON){
      if(idkey == "Conv_EDv5")  return j_lep_mva_hnl_conv_ed_v5;
      if(idkey == "CF_EDv5")    return j_lep_mva_hnl_ed_cf_v5;
      if(idkey == "CFPt_EDv5")  return j_lep_mva_hnl_ed_cf_v5Pt;
      if(idkey == "Fake_EDv5")  return j_lep_mva_hnl_fake_ed_v5;
      if(idkey == "FakeLF_EDv5")  return j_lep_mva_hnl_fake_v5_lf;
    }
    if(j_LeptonFlavour==MUON){
      if(idkey=="HFTop")          return  j_lep_mva;
    }
    
    cout<<"[Lepton::MVAKey ERROR] " << idkey << endl;
    exit(ENODATA);
  }


  //// Close jet functionality  NOT IN SKFLAT ADDED in ANALYZERCORE OR IN BDT SKIM
  void SetJetPtRel(double ptrel);
  void SetJetPtRatio(double ptr);
  void SetCloseJetBScore(double bjscore);
  void SetCloseJetCvsBScore(double bjscore);
  void SetCloseJetCvsLScore(double bjscore);
  void SetCloseJetFlavour(int flav);

  inline double CloseJet_Ptrel()   const {return j_lep_jetptrel;}
  inline double CloseJet_Ptratio() const {return j_lep_jetptratio;}
  inline double CloseJet_BScore()  const {return j_lep_jetbscore;}
  inline double CloseJet_CvsBScore()  const {return j_lep_jetcvsbscore;}
  inline double CloseJet_CvsLScore()  const {return j_lep_jetcvslscore;}
  inline int CloseJet_FlavourInt()  const {return j_lep_jetflavour;}
  inline TString CloseJet_Flavour()  const {
    if(j_lep_jetflavour == 5) return "HF_B";
    if(j_lep_jetflavour == 4) return "HF_C";
    if(j_lep_jetflavour == 0) return "LF";
    return "Pileup";
  }

  inline double MotherJetPt(double UpperValue=1.5)const{
    double MotherJPt =  this->Pt()/ j_lep_jetptratio;
    if(MotherJPt < this->Pt()) return this->Pt();
    if(MotherJPt > UpperValue *this->Pt()) return UpperValue *this->Pt();
    return this->Pt()/ j_lep_jetptratio;
  }
  

  inline TString MotherJetFlavour()  const {
    if(j_lep_jetflavour >= 4) return "HF";
    if(j_lep_jetflavour == 0) return "LF";
    return "Pileup";
  }


  void SetdZ(double dZ, double dZerr);
  inline double fdZ() const {return fabs(j_dZ);}
  inline double dZ() const {return j_dZ;}
  inline double dZerr() const {return j_dZerr;}

  void SetIP3D(double IP3D, double IP3Derr);
  inline double SIP3D() const {return (j_IP3Derr >0) ? fabs(j_IP3D/j_IP3Derr):0.0;};
  inline double IP3D() const {return j_IP3D;}
  inline double IP3Derr() const {return j_IP3Derr;}


  inline double HTOverPt() const{ return  (j_HTOverPt > 10) ? 10 : j_HTOverPt; }

  //==== AbsIso will be set in Muon/Electron,
  //==== and use SetRelIso to save calculated RelIso
  void SetRelIso(double r);
  inline double RelIso() const {return j_RelIso;}

  inline double MiniIsoChHad() const {return j_MiniIso_ChHad;}
  inline double MiniIsoNHad() const {return j_MiniIso_NHad;}
  inline double MiniIsoPhHad() const {return j_MiniIso_PhHad;}
  inline double IsoChHad() const {return j_Iso_ChHad;}
  inline double IsoNHad() const {return j_Iso_NHad;}
  inline double IsoPhHad() const {return j_Iso_PhHad;}

  

  //==== SUSY mini Iso has same formula for Muon and Electron
  void SetMiniIso(double ch, double nh, double ph, double pu, double rho, double EA);
  inline double MiniRelIso() const {return j_MiniRelIso;}

  inline double MiniRelIsoCharged() const {return j_MiniIso_ChHad/this->Pt();}
  inline double MiniRelIsoNeutral() const {return (j_MiniRelIso*this->Pt() - j_MiniIso_ChHad)/this->Pt();}
  
  void SetLepIso(double ch, double nh, double ph);

  inline double fEta() const {return fabs(defEta());}

  inline TString FakeTaggerString() const {
    TString TaggerSt="";
    if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > 0.8) TaggerSt+="LF1_";
    else if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > 0.5) TaggerSt+="LF2_";
    else if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > 0.) TaggerSt+="LF3_";
    else if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > -0.5) TaggerSt+="LF4_";
    else TaggerSt+="LF5_";
    if(j_lep_mva_hnl_fake_QCD_BvsC_v5 > 0.8)  TaggerSt+="HF1";
    else if(j_lep_mva_hnl_fake_QCD_BvsC_v5 > 0.2)  TaggerSt+="HF2";
    else if(j_lep_mva_hnl_fake_QCD_BvsC_v5 > -0.4)  TaggerSt+="HF3";
    else TaggerSt+="HF4";
    
    return TaggerSt;
  }

  inline TString FakeTaggerStringLF() const {

    TString TaggerSt="";
    if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > 0.8) TaggerSt+="LF1";
    else if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > 0.5) TaggerSt+="LF2";
    else if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > 0.) TaggerSt+="LF3";
    else if(j_lep_mva_hnl_fake_QCD_LFvsHF_v5 > -0.5) TaggerSt+="LF4";
    else TaggerSt+="LF5_";
    return TaggerSt;
  }
  inline TString FakeTaggerStringHF() const {
    TString TaggerSt="";
    if(j_lep_mva_hnl_fake_QCD_BvsC_v5 > 0.8)       TaggerSt+="HF1";
    else if(j_lep_mva_hnl_fake_QCD_BvsC_v5 > 0.2)  TaggerSt+="HF2";
    else if(j_lep_mva_hnl_fake_QCD_BvsC_v5 > -0.4)  TaggerSt+="HF3";
    else TaggerSt+="HF4";
    return TaggerSt;
  }
   

  inline TString LeptonFakeTagger() const {
    if (j_lep_jetbscore > 0.2) return "HF1";
    if (j_lep_jetbscore > 0.05) return "HF2";
    if (j_lep_jetbscore > 0.025) return "HF3";
    return "HF4";
  }


  bool Pass_MVA(double mva1, double cut, TString s_mva) const;
  bool Pass_MVA_BBEC(TString MVALabel , double cut_b, double cut_ec, TString s_helper) const ;

  
  inline double GetPtSlopeCut(double MinPt, double MaxPt, double MinCut, double MaxCut) const{
    
    double cutSlope(0);
    double cutFinal(0);
    if(MinCut < MaxCut){
      
      cutSlope  = (MaxCut-MinCut) / (MaxPt-MinPt);
      cutFinal  = std::max( MinCut, std::min(MaxCut , MinCut + cutSlope*(this->Pt()-MinPt) ) );
    }
    else   if(MinCut >MaxCut){
      cutSlope  = (MaxCut-MinCut) / (MaxPt-MinPt);
      cutFinal  = std::min(MinCut,std::max( MaxCut,MinCut + cutSlope*(this->Pt()-MinPt)));
    }
    else cutFinal = MinCut;

    return cutFinal;
  }

  inline bool PassLepID()  const {return j_passID;}
  inline bool LepIDSet()  const {return j_IDSet;}
  inline TString LepTightIDName() const {return j_TIDName;}

  TString CutString(double cut);

  inline int LeptonGenType() const {return j_LeptonType;}
  void SetLeptonType(int t);

  void SetLeptonIsCF(bool t);
  void SetLeptonIsPromptConv(bool t);

  inline Flavour LeptonFlavour() const {return j_LeptonFlavour;}
  void SetLeptonFlavour(Flavour f);

  void SetPassID(bool p, TString IDName);
  void SetID();

  inline bool IsElectron() const {return j_LeptonFlavour==ELECTRON;}
  inline bool IsMuon() const {return j_LeptonFlavour==MUON;}

  inline TString GetFlavour() const {
    if (j_LeptonFlavour==ELECTRON) return "Electron";
    else return "Muon";
  }

  inline TString  etaRegionString() const {    return GetEtaRegion("4bin"); }
  inline TString  sRegion() const {return GetEtaRegion("2bin");}
  
  inline TString GetEtaRegion(TString Version="2bin") const {
    double sceta = fabs(defEta());

    if(Version == "2bin"){
      if(IsBB()) return "BB";
      return "EC";
    }
    if(Version == "3bingap"){
      if(sceta < 0.8) return "BB";
      if(sceta  < 1.444) return "OB";
      if(sceta  < 1.566) return "GAP";
      return "EC";
    }
    if(Version == "3bin"){
      if(sceta < 0.8) return "BB";
      if(sceta  < 1.5) return "OB";
      return "EE";
    }
    if(Version == "4bin"){
      if( sceta < 0.8 ) return "EB1";
      else if( sceta < 1.479 ) return "EB2";
      else if( sceta < 2. ) return "EE1";
      else return "EE2";
    }
    return "";
  }

  inline float miniIsoDr() const {
    float mindr = 0.05;
    float maxdr = 0.2;
    float kt_scale = 10.0;
    return std::max(mindr, std::min(maxdr, float(kt_scale/this->Pt())));
  }

  void SetHTOverPt(double f);
    
  void SetPtCone(double f){ j_ptcone = f; }
  inline bool IsPtConeAvailable() const {
    if(j_ptcone<0) return false;
    else return true;
  }
  inline double PtCone() const {
    if(j_ptcone<0){
      cout << "[Lepton::PtCone] ptcone not set" << endl;
      exit(EXIT_FAILURE);
    }
    return j_ptcone;
  }
  inline double CalcPtCone(double this_reliso, double Tight_reliso){
    return ( this->Pt() ) * ( 1. + max(0., (this_reliso-Tight_reliso)) );
  }
  
  inline double CalcTrkPtCone(double this_reliso, double Tight_reliso){
    return ( this->Pt() + max(0., (this_reliso-Tight_reliso)));
  }

  inline double CalcMVACone(double Tight_mva, double UpperValue=1.5){
    double this_mva (0.);

    if(j_LeptonFlavour==MUON) this_mva = j_lep_mva;
    else this_mva = j_lep_mva_hnl_fake_ed_v5;
    
  
    if(this_mva > Tight_mva) return this->Pt();
    double MSlope  = 1  - (1 + this_mva) / (1+Tight_mva);  
    double PMDiff  = (j_lep_jetptratio < 1.) ? (this->Pt()/j_lep_jetptratio) - this->Pt() : 0; /// Mother Jet - Lepton (Similar to Iso)
    double MJProxy = (MSlope * PMDiff) + this->Pt() ;
    if((MJProxy/this->Pt()) > UpperValue) return UpperValue*this->Pt();
    return MJProxy;
  }

  inline TString PrintInfo() const{
    return this->GetFlavour() +" Charge = " + this->LepDoubleToString(Charge()) + " Pt " + this->LepDoubleToString(this->Pt()) + " Lep Eta " + this->LepDoubleToString(this->Eta()) + " Lep Phi = " + this->LepDoubleToString(this->Phi()) + " type = " + this->LepGenTypeNumberString();
  }

  virtual void Print();

  
  void SetTrkIso(double iso);
  inline double TrkIso() const {return j_trkiso; }
  

  std::map<TString, double> j_lep_map_mva_hnl;

  /////// FAKE MVA
  /// Version 4
  double j_lep_mva_hnl_fake_v4_hf,   j_lep_mva_hnl_fake_v4_hfb,   j_lep_mva_hnl_fake_v4_hfc,   j_lep_mva_hnl_fake_v4_lf, j_lep_mva_hnl_fake_v4_top, j_lep_mva_hnl_fake_v4;
  double j_lep_mva_hnl_fake_ed_v4_hf,j_lep_mva_hnl_fake_ed_v4_hfb,j_lep_mva_hnl_fake_ed_v4_hfc,j_lep_mva_hnl_fake_ed_v4_lf, j_lep_mva_hnl_fake_ed_v4_top, j_lep_mva_hnl_fake_ed_v4;

  int j_run_year;
  TString j_run_era;
  
  /// Version 5
  double j_lep_mva_hnl_fake_ed_v5,j_lep_mva_hnl_fake_v5_hfb,j_lep_mva_hnl_fake_v5_hfc,j_lep_mva_hnl_fake_v5_lf;
  
  double j_lep_mva_hnl_fake_QCD_LFvsHF_v5, j_lep_mva_hnl_fake_QCD_BvsC_v5,j_lep_mva_hnl_fake_LF1_v5,j_lep_mva_hnl_fake_LF2_v5;

  ///// Conv
  
  double j_lep_mva_hnl_conv_v4,j_lep_mva_hnl_conv_ed_v4,j_lep_mva_hnl_conv_ed_v5;

  ////// CF
 
  double j_lep_mva_hnl_cf_v4,j_lep_mva_hnl_ed_cf_v4;
  double j_lep_mva_hnl_ed_cf_v5,j_lep_mva_hnl_ed_cf_v5Pt;
  
  double el_mva_cut_fake_2016_B;
  double el_mva_cut_fake_2017_B;
  double el_mva_cut_fake_2018_B;
  double el_mva_cut_fake_2016_EC;
  double el_mva_cut_fake_2017_EC;
  double el_mva_cut_fake_2018_EC;

  double mu_mva_cut_fake_2016;
  double mu_mva_cut_fake_2017;
  double mu_mva_cut_fake_2018;

  double el_mva_cut_cf_2016_B;
  double el_mva_cut_cf_2017_B;
  double el_mva_cut_cf_2018_B;
  double el_mva_cut_cf_2016_EC;
  double el_mva_cut_cf_2017_EC;
  double el_mva_cut_cf_2018_EC;

  double TopMVA_cut;
  

private:
  double j_dXY, j_dXYerr;
  double j_trkiso;
  double j_lep_mva;
  double j_dZ, j_dZerr;
  double j_IP3D, j_IP3Derr;
  double j_MiniAODPt;
  double j_unCorrPt,j_elSCeta;
  double j_jetntracks,j_jetntracks_mva;
  double j_RelIso, j_MiniRelIso;
  double j_MiniIso_ChHad,j_MiniIso_NHad,j_MiniIso_PhHad;
  double j_Iso_ChHad,j_Iso_NHad,j_Iso_PhHad;
  double j_ptcone;
  double j_lep_jetptrel,j_lep_jetptratio,j_lep_jetbscore,j_lep_jetcvsbscore,j_lep_jetcvslscore;
  int j_lep_jetflavour;

  double j_HTOverPt;

  Flavour j_LeptonFlavour;

  int j_LeptonType;
  bool j_LeptonIsCF;
  bool j_LeptonIsPromptConv;

  bool j_passID;  
  bool j_IDSet;
  TString j_TIDName;


  ClassDef(Lepton,1)

};

#endif
