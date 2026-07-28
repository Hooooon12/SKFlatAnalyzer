#include "HNL_RegionDefinitions.h"

/*

  Analysis REGION Functions 
  -----------------------------------                                                                                                                   a) RunPreslection
  b) PassVBFInitial : Use this IF using shape for SR2 
  c) PassVBF

  Presel
  ----------------------------
  - PassPreselection

  SR Functions
  -----------------------------------
  * RunAllSignalRegions Runs All SR files
  1- RunSignalRegionAK8 (SR1) OR RunSignalRegionAK8String To run RunSignalRegionAK8 and return Limit input string
  2- RunSignalRegionWW  (SR2) OR RunSignalRegionWWString To run RunSignalRegionWW and return Limit input string  
  3- RunSignalRegionAK4 (SR3) OR RunSignalRegionAK4String To run RunSignalRegionAK4  and return Limit input string  
  4- RunSignalRegionAK4StringBDT Run SR3 BDT
  

 */



///// CheckBin Used to scan events in a particular Limit bin, to debug

void HNL_RegionDefinitions::CheckBin(TString signal,TString binvalue, TString checkbin,HNL_LeptonCore::Channel channel, AnalyzerParameter& param,std::vector<Lepton *>& leps,std::vector<Jet>& jets,vector<FatJet>&  fatjets, std::vector<Jet>& bjets, Particle& METv, double w){

  if(binvalue!= checkbin) return;
  if(!MCSample.Contains(signal)) return;

  cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " channel " << GetChannelString(channel) << " weight = " << w<< endl;
  cout << "MET = " << METv.Pt() << endl;
  for(auto ilep : leps) cout << "Lepton pt " << ilep->Pt() << endl;

}

/// Main code for running SR
void HNL_RegionDefinitions::RunAllSignalRegions(HNL_LeptonCore::ChargeType qq,
            std::vector<Electron> electronsInitial, std::vector<Electron> electrons_veto, std::vector<Muon> muons, std::vector<Muon> muons_veto, std::vector<Tau> TauColl, 
            std::vector<Jet> AK4_JetCollLoose,std::vector<Jet> JetColl, std::vector<Jet> VBF_JetColl,std::vector<FatJet>  AK8_JetColl, std::vector<Jet> B_JetColl, 
            Event ev,   Particle METv, AnalyzerParameter param, int nElForRunCF,   float weight_ll){
  
  if (run_Debug) {
    std::cout << "[DEBUG] RunAllSignalRegions called" << std::endl;
    std::cout << "  Event ev         = " << event << std::endl;
    std::cout << "  MET pt           = " << METv.Pt() << std::endl;
    std::cout << "  syst             = " << param.syst_ << std::endl;
    std::cout << "  weight_ll        = " << weight_ll << std::endl;
  }
  
  //// Need to correct MET/Energy of electrons in the case of CF bkg estimate
  std::vector<Electron> electrons;
  if(RunCF) {
    /// Add code to smear individual electron for CF Bkg                                                                                                                                                                                                                                   
    for(unsigned int i=0; i<electronsInitial.size(); i++){
      Electron this_electron = electronsInitial.at(i);
      double ElEnergyShift = 1;

      if(int(i)==nElForRunCF) ElEnergyShift = GetShiftCFEl(this_electron, param.Electron_Tight_ID);

      this_electron.SetPtEtaPhiM( electronsInitial.at(i).Pt()*ElEnergyShift, electronsInitial.at(i).Eta(), electronsInitial.at(i).Phi(), electronsInitial.at(i).M() );
      electrons.push_back( this_electron);
    }
    //// Update MET For CF
    METv = UpdateMETCF(METv,electronsInitial,electrons);
  }
  else  electrons = electronsInitial;

  
  vector<HNL_LeptonCore::Channel> channels = {GetChannelENum(param.Channel)};

  if(GetChannelENum(param.Channel) == HNL_LeptonCore::NONE){
    cout << "CHANNEL NOT SET" << endl;
    exit(EXIT_FAILURE);
  }

  HNL_LeptonCore::SearchRegion CutFlow_Region = SignalRegion;

  //// Loop over channels is remnany of old code, now we only have one channel, but keep loop in case 
  for(auto dilep_channel : channels){
    
    double  weight_channel = weight_ll;

    //// Select CHannel used for Signals to check if signal is EE/MM/Emu using gen info
    if(IsSignal()&& !SelectChannel(dilep_channel)) continue;
  
    if(param.IsCentral()) FillHist( "AllChannel/RunAllSignalRegions_NOCUT_ALLCHANNEL_"+param.Name,  1,  weight_ll, 2,0,2);

    TString channel_string = GetChannelString(dilep_channel);
    param.CutFlowDir = "CutFlow";
    /// Used for Lepton Inclusive plots
    param.NameInclusive_Channel = param.Name  + "/"+param.InclusiveChannelName();
    param.Name = param.Name  + "/"+channel_string;


    //// Set HT over Pt for use in SR/CR VBF limit inputs
    for(long unsigned int imu =0 ; imu <  muons.size(); imu++) muons[imu].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));
    for(long unsigned int iel =0 ; iel < electrons.size() ; iel++) electrons[iel].SetHTOverPt(GetHT(AK4_JetCollLoose,AK8_JetColl));

    std::vector<Lepton *> LepsT       = MakeLeptonPointerVector(muons,     electrons,     param);
    std::vector<Lepton *> LepsV       = MakeLeptonPointerVector(muons_veto,electrons_veto,param);
    
    //// Dont plot if running all systematics     
    //if(!runSyst&&param.runPlotter){
    //  if(param.IsCentral()){
    //    if(IsSignal()){
    //      Fill_RegionPlots(param,"Signal_NoCut_RAW" , TauColl, All_Jets,  All_FatJets, LepsV, METv, nPV, weight_ll);
    //      Fill_RegionPlots(param,"Signal_NoCut_ID" , TauColl, JetColl, AK8_JetColl, LepsT, METv, nPV, weight_ll);

    //      double sumw = 0.;
    //      double sumw2 = 0.;
    //      double sumsq = 0.;
    //      int nrep = 0;
    //      for(unsigned int iw=0; iw<weight_PDF->size(); iw++){
    //        double PDF_W = 1.;
    //        TString PNAME_PDF = GetPDFUncertainty(iw,PDF_W);
    //        cout << "PDF_W: " << PDF_W << endl;

    //        sumw  += PDF_W;
    //        sumw2 += PDF_W * PDF_W;
    //        sumsq += (PDF_W - 1.) * (PDF_W - 1.);
    //        nrep++;
    //      }

    //      double meanw = 1.;
    //      double sigmaw = 0.;
    //      if(nrep > 0){
    //        meanw = sumw / nrep;
    //        //cout << "meanw: " << meanw << endl;
    //        double var;
    //        if(MCSample.Contains("VBFTypeI")){
    //          var = sumw2 / nrep - meanw * meanw;
    //        }
    //        else if(MCSample.Contains("DYTypeI")||MCSample.Contains("SSWWTypeI")||MCSample.Contains("WZTo3LNu_amcatnlo")){
    //          var = sumsq;
    //        }
    //        else{
    //          cout << "Unknown MC sample:" << MCSample << endl;
    //          cout << "return;" << MCSample << endl;
    //          return;
    //        }
    //        if(var < 0.) var = 0.;
    //        sigmaw = sqrt(var);
    //      }

    //      double rel_pdf_unc = 0.;
    //      if(meanw != 0.) rel_pdf_unc = sigmaw / meanw;
    //      if(rel_pdf_unc > 1.){
    //        cout << "[NOTE] rel_pdf_unc > 1. --> " << rel_pdf_unc << endl;
    //        cout << "Check below logs!!" << endl;
    //        PrintGen(All_Gens);
    //      }
    //      FillHist("NoCut_Q"   , genWeight_Q  , 1, 250, 0, 5000, "Q");
    //      FillHist("NoCut_X1"  , genWeight_X1 , 1, 50,  0, 1,    "Bjorken x_{1}");
    //      FillHist("NoCut_X2"  , genWeight_X2 , 1, 50,  0, 1,    "Bjorken x_{2}");
    //      FillHist("NoCut_PID1", genWeight_id1, 1, 40,  -10, 30,  "PID_{1}");
    //      FillHist("NoCut_PID2", genWeight_id2, 1, 40,  -10, 30,  "PID_{2}");
    //      FillHist("NoCut_X1_Q", genWeight_X1, genWeight_Q, 1, 50, 0, 1, 250, 0, 5000);
    //      FillHist("NoCut_X2_Q", genWeight_X2, genWeight_Q, 1, 50, 0, 1, 250, 0, 5000);
    //      FillHist("NoCut_X1_X2", genWeight_X1, genWeight_X2, 1, 50, 0, 1, 50, 0, 1);

    //      FillProf("PDFUnc_vs_Q"   , genWeight_Q  , rel_pdf_unc, 1, 250, 0, 5000);
    //      FillProf("PDFUnc_vs_X1"  , genWeight_X1 , rel_pdf_unc, 1, 50,  0, 1);
    //      FillProf("PDFUnc_vs_X2"  , genWeight_X2 , rel_pdf_unc, 1, 50,  0, 1);
    //      FillProf("PDFUnc_vs_PID1", genWeight_id1, rel_pdf_unc, 1, 40,  -10, 30);
    //      FillProf("PDFUnc_vs_PID2", genWeight_id2, rel_pdf_unc, 1, 40,  -10, 30);
    //      FillProf("PDFUnc_X1_Q", genWeight_X1, genWeight_Q, rel_pdf_unc, 1, 50, 0, 1, 250, 0, 5000);
    //      FillProf("PDFUnc_X2_Q", genWeight_X2, genWeight_Q, rel_pdf_unc, 1, 50, 0, 1, 250, 0, 5000);
    //      FillProf("PDFUnc_X1_X2", genWeight_X1, genWeight_X2, rel_pdf_unc, 1, 50, 0, 1, 50, 0, 1);

    //      if(genWeight_id1==22){ // beam 1 is photon
    //        FillHist("NoCut_Xphoton"  , genWeight_X1 , 1, 50,  0, 1,    "Bjorken x_{#gamma}");
    //        FillHist("NoCut_Xparton"  , genWeight_X2 , 1, 50,  0, 1,    "Bjorken x_{q/g}");
    //        FillProf("PDFUnc_vs_Xphoton", genWeight_X1, rel_pdf_unc, 1, 50, 0, 1);
    //        FillProf("PDFUnc_vs_Xparton", genWeight_X2, rel_pdf_unc, 1, 50, 0, 1);
    //        FillProf("PDFUnc_Xphoton_Q", genWeight_X1, genWeight_Q, rel_pdf_unc, 1, 50, 0, 1, 250, 0, 5000);
    //        FillProf("PDFUnc_Xparton_Q", genWeight_X2, genWeight_Q, rel_pdf_unc, 1, 50, 0, 1, 250, 0, 5000);
    //      }
    //      else if(genWeight_id2==22){ // beam 2 is photon
    //        FillHist("NoCut_Xphoton"  , genWeight_X2 , 1, 50,  0, 1,    "Bjorken x_{#gamma}");
    //        FillHist("NoCut_Xparton"  , genWeight_X1 , 1, 50,  0, 1,    "Bjorken x_{q/g}");
    //        FillProf("PDFUnc_vs_Xphoton", genWeight_X2, rel_pdf_unc, 1, 50, 0, 1);
    //        FillProf("PDFUnc_vs_Xparton", genWeight_X1, rel_pdf_unc, 1, 50, 0, 1);
    //        FillProf("PDFUnc_Xphoton_Q", genWeight_X2, genWeight_Q, rel_pdf_unc, 1, 50, 0, 1, 250, 0, 5000);
    //        FillProf("PDFUnc_Xparton_Q", genWeight_X1, genWeight_Q, rel_pdf_unc, 1, 50, 0, 1, 250, 0, 5000);
    //      }
    //    }
    //  }
    //}

    //return; //JH

    //// Set METST value after shifting Electrons                                                                                                                                                                                             
    ev.SetMET2ST(GetMET2ST(LepsT, JetColl, AK8_JetColl, METv));
    
    if(param.IsCentral()){
      double weight_Cutflow    = weight_channel;
      
      bool CFRun = (RunCF && dilep_channel != MuMu && IsData && !SameCharge(LepsT));
      bool OSRun = (HasFlag("RunOS") && IsData && !SameCharge(LepsT));
      
      if(RunCF) {
        if(CFRun)  weight_Cutflow = GetCFWeightElectron(LepsT, param,nElForRunCF,true);
      }

      if(OSRun)  weight_Cutflow = 1;
      
      bool FakeRun = (RunFake&& IsData);
      if(FakeRun) weight_Cutflow = GetFakeWeight(LepsT, param);
      
      bool RunCutFlow=true;
      if(RunCF && !CFRun) RunCutFlow=false;
      if(RunFake && !FakeRun) RunCutFlow=false;
      if(HasFlag("RunOS") && !OSRun) RunCutFlow=false;

      if(RunCutFlow){
  
        FillCutflow(CutFlow_Region, weight_Cutflow, "NoCut", param);
        if (PassTriggerSelection(dilep_channel, ev, LepsT,param.TriggerSelection)){
          FillCutflow(CutFlow_Region, weight_Cutflow, "Trigger",param);
          
          if(PassGenMatchFilter(LepsT,param)){
            if(ConversionSplitting(LepsT,RunConv,2,param)){
              FillCutflow(CutFlow_Region, weight_Cutflow, "GENMatched",param);
              if(PassHEMVeto(LepsV,weight_Cutflow)) {
                FillCutflow(CutFlow_Region, weight_Cutflow, "HEMVeto", param);
                if(PassMETFilter()) {
                  FillCutflow(CutFlow_Region, weight_Cutflow, "METFilter",param);
                  if(CheckLeptonFlavourForChannel(dilep_channel, LepsT)) {
                    if(LepsT.size()==2){
                      FillCutflow(CutFlow_Region, weight_Cutflow, "LeptonFlavour",param);
                      if(SameCharge(LepsT) || (CFRun)){
                  
                        FillCutflow(CutFlow_Region, weight_Cutflow, "SSLepton",param);
                        if(LepsV.size()==2) {
                          
                          FillCutflow(CutFlow_Region, weight_Cutflow, "LepVeto",param);
                          Particle ll =  (*LepsT[0]) + (*LepsT[1]);
                          if(ll.M() > 20){
                            if(dilep_channel!=EE ||  ( (fabs(ll.M()-M_Z) > M_ZWINDOW_VETO))){
                              FillCutflow(CutFlow_Region, weight_Cutflow, "DiLepMass",param);
                              if(B_JetColl.size()==0)     FillCutflow(CutFlow_Region, weight_Cutflow, "BJet",param);
                              if(B_JetColl.size()==0 && ev.MET2ST() < 15)   FillCutflow(CutFlow_Region, weight_Cutflow, "MET",param);
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }        
            }
          }
        }
      }
    }

    if(HasFlag("RunOS")) continue;

    if(!PassGenMatchFilter(LepsT,param)) continue;
    
    //if(param.syst_ ==AnalyzerParameter::Central){
    //  for(auto iel : electrons){
    //FillHist( param.Name+"/Inclusive_El_HEM_EtaPhi", iel.Eta(), iel.Phi(),  fabs(weight_channel),  100, -2.5, 2.5, 70, -3.5,3.5);
    //  if(!IsData || IsHEMIssueRun()) {
    //    FillHist( param.Name+"/HEM_Region_El_HEM_EtaPhi", iel.Eta(), iel.Phi(),  fabs(weight_channel),  100, -2.5, 2.5, 70, -3.5,3.5);
    //    
    //    if(FindHEMElectron(iel)) FillHist( param.Name+"/HEM_Region_Fail_El_HEM_EtaPhi", iel.Eta(), iel.Phi(),  fabs(weight_channel),  100, -2.5, 2.5, 70, -3.5,3.5);
    //    
    //  }
    //  }
    //}
    
    if(!PassHEMVeto(LepsV,weight_channel)) continue;
    if(!PassMETFilter()) return;
    
    FillCutflow(HNL_LeptonCore::ChannelDepInc, weight_channel, GetChannelString(dilep_channel) +"_NoCut",param);

    if(HasFlag("OldConv")){
      if(!ConversionSplitting(LepsT,RunConv,2,param)) continue;
    }

    if(! CheckLeptonFlavourForChannel(dilep_channel, LepsT))  continue;
    
    if(param.IsCentral()) PassJetHEMVeto(JetColl,param.Name+"_Jet",weight_channel);

    if(LepsT.size() ==2)  FillCutflow(HNL_LeptonCore::ChannelDepDilep, weight_channel, GetChannelString(dilep_channel) +"_Dilep",param);


    std::vector<TString> WGSamples = {"WGJJToLNu","WGToLNuG_MG","WGToLNuG"};
       
    if(RunCF){
      if(dilep_channel == MuMu) continue;
      
      if(IsData && SameCharge(LepsT)) continue;
      if(!IsData && !SameCharge(LepsT)) continue;
      
      if(IsData)weight_channel = GetCFWeightElectron(LepsT, param,nElForRunCF,true);
      if(IsData)FillWeightHist(param.Name+"/CFWeight", weight_channel);

    }
    else if(HasFlag("SSConvSplit")){
      /// used to test conv with SS cut
      if(!SameCharge(LepsT)) continue;
    }
    else if (std::find(WGSamples.begin(), WGSamples.end(), MCSample) != WGSamples.end()) {
      /// USE SS+OS / 2 using dilepton skim
      weight_channel*= 0.5;
      if(LepsT.size() != 2) continue;
    }
    else{
      if(!SameCharge(LepsT)) continue;
    }
    
    if(RunFake&& IsData){
      
      weight_channel = GetFakeWeight(LepsT, param);
      FillFakeWeightHist(param.Name+"/FakeWeight", LepsT,param, weight_channel);

    }
   

    if (!PassTriggerSelection(dilep_channel, ev, LepsT,param.TriggerSelection)) {
      if(dilep_channel==MuMu){
  if(PassTriggerSelection(dilep_channel, ev, LepsT,"HighPt"))   FillCutflow(HNL_LeptonCore::ChannelDepTrigger, weight_channel, GetChannelString(dilep_channel) +"_MultiTrigger",param);
      }
      if(dilep_channel==EE){
        if(PassTriggerSelection(dilep_channel, ev, LepsT,"POGSglLep"))   FillCutflow(HNL_LeptonCore::ChannelDepTrigger, weight_channel, GetChannelString(dilep_channel) +"_MultiTrigger",param);
      } 
      if(dilep_channel==EMu){
        if(PassTriggerSelection(dilep_channel, ev, LepsT,"Full"))   FillCutflow(HNL_LeptonCore::ChannelDepTrigger, weight_channel, GetChannelString(dilep_channel) +"_MultiTrigger",param);
      } 
      continue;
    }

    //// Fill Trigger Cutflow
    FillCutflow(HNL_LeptonCore::ChannelDepTrigger, weight_channel, GetChannelString(dilep_channel) +"_MultiTrigger",param); /// test adding SL trigger
    FillCutflow(HNL_LeptonCore::ChannelDepTrigger, weight_channel, GetChannelString(dilep_channel) +"_Trigger",param);
    
    EvalTrigWeight(dilep_channel, muons,electrons,param, ev,weight_channel);


    /// RunMainRegionCode runs SR1/SR2/SR3
    
    if(isnan(weight_channel)) {
      cout << "Weight for event is set to Nan..." << endl;
      exit(EXIT_FAILURE);
    }
    
    if(HasFlag("RunCR")) RunMainRegionCode(false,dilep_channel,Inclusive, LepsT, LepsV, TauColl, JetColl,  VBF_JetColl, AK8_JetColl, B_JetColl,ev, METv ,param, weight_channel);
    else     RunMainRegionCode(true,dilep_channel,Inclusive, LepsT, LepsV, TauColl, JetColl, VBF_JetColl, AK8_JetColl, B_JetColl,ev, METv ,param, weight_channel);
  }
}

void   HNL_RegionDefinitions::RunMainRegionCode(bool IsSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType qq, std::vector<Lepton *>& LepsT,std::vector<Lepton *>& LepsV, vector<Tau>& TauColl,  std::vector<Jet>& JetColl,       std::vector<Jet>& VBF_JetColl,std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, Event& ev, Particle& METv, AnalyzerParameter& param,  float weight_reg){
  
  TString channel_string = GetChannelString(channel);

  HNL_LeptonCore::SearchRegion LimitRegionR1,LimitRegionR2,LimitRegionR3,LimitRegionsBDTR3;

  HNL_LeptonCore::SearchRegion LimitRegionsInvMETR1 = HNL_LeptonCore::MuonInvMETCR1;
  HNL_LeptonCore::SearchRegion LimitRegionsInvBJetR1 = HNL_LeptonCore::MuonInvBJetCR1;
  HNL_LeptonCore::SearchRegion LimitRegionsInvMETR2 = HNL_LeptonCore::MuonInvMETCR2;
  HNL_LeptonCore::SearchRegion LimitRegionsInvBJetR2 = HNL_LeptonCore::MuonInvBJetCR2;
  HNL_LeptonCore::SearchRegion LimitRegionsInvMETR3 = HNL_LeptonCore::MuonInvMETCR3;
  HNL_LeptonCore::SearchRegion LimitRegionsInvBJetR3 = HNL_LeptonCore::MuonInvBJetCR3;
  HNL_LeptonCore::SearchRegion LimitRegionsInvMETBDTR3 = HNL_LeptonCore::MuonInvMETCR3BDT;
  HNL_LeptonCore::SearchRegion LimitRegionsInvBJetBDTR3 = HNL_LeptonCore::MuonInvBJetCR3BDT;

  /// For CR use 2 BJet definitions

  ///      JetTagging::Parameters param_jetsT = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Tight, JetTagging::incl, JetTagging::mujets);
  /// std::vector<Jet> BJetCollLoose    = GetHNLJets(param.BJetColl,     param);
  ///  return SelectBJets(param, BJetCollLoose, param_jetsT);
  vector<Jet> B_JetColl_CR; 
  if(!IsSR){
    /// Select Tight WP for IB CRs
    JetTagging::Parameters param_jetsT = JetTagging::Parameters(JetTagging::DeepJet, JetTagging::Tight, JetTagging::incl, JetTagging::mujets);
    B_JetColl_CR = SelectBJets(param,B_JetColl,  param_jetsT);            
  }
  else B_JetColl_CR = B_JetColl;
  
  if(IsSR){
  
    /// Remove now LimitRegions and LimitRegionsBDT which merged SR1/2/3 into one plot as SRs can have different bin numbers per mass/era 
    LimitRegionR1  = HNL_LeptonCore::MuonSR1;
    LimitRegionR2  = HNL_LeptonCore::MuonSR2;    
    LimitRegionR3  = HNL_LeptonCore::MuonSR3;    
    LimitRegionsBDTR3 = HNL_LeptonCore::MuonSR3BDT;
    
    if (channel == EE) {
      LimitRegionR1  = HNL_LeptonCore::ElectronSR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronSR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronSR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronSR3BDT;
    }
    if (channel == EMu) {
      LimitRegionR1  = HNL_LeptonCore::ElectronMuonSR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronMuonSR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronMuonSR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronMuonSR3BDT;
    }
  }
  else{    
    

    LimitRegionR1  = HNL_LeptonCore::MuonCR1;
    LimitRegionR2  = HNL_LeptonCore::MuonCR2;
    LimitRegionR3  = HNL_LeptonCore::MuonCR3;
    LimitRegionsBDTR3 = HNL_LeptonCore::MuonCR3BDT;

    LimitRegionsInvMETR1 = HNL_LeptonCore::MuonInvMETCR1;
    LimitRegionsInvBJetR1 = HNL_LeptonCore::MuonInvBJetCR1;
    LimitRegionsInvMETR2 = HNL_LeptonCore::MuonInvMETCR2;
    LimitRegionsInvBJetR2 = HNL_LeptonCore::MuonInvBJetCR2;
    LimitRegionsInvMETR3 = HNL_LeptonCore::MuonInvMETCR3;
    LimitRegionsInvBJetR3 = HNL_LeptonCore::MuonInvBJetCR3;
    LimitRegionsInvMETBDTR3 = HNL_LeptonCore::MuonInvMETCR3BDT;
    LimitRegionsInvBJetBDTR3 = HNL_LeptonCore::MuonInvBJetCR3BDT;

    if (channel == EE) {

      LimitRegionR1  = HNL_LeptonCore::ElectronCR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronCR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronCR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronCR3BDT;


      LimitRegionsInvMETR1 = HNL_LeptonCore::ElectronInvMETCR1;
      LimitRegionsInvBJetR1 = HNL_LeptonCore::ElectronInvBJetCR1;
      LimitRegionsInvMETR2 = HNL_LeptonCore::ElectronInvMETCR2;
      LimitRegionsInvBJetR2 = HNL_LeptonCore::ElectronInvBJetCR2;
      LimitRegionsInvMETR3 = HNL_LeptonCore::ElectronInvMETCR3;
      LimitRegionsInvBJetR3 = HNL_LeptonCore::ElectronInvBJetCR3;

      LimitRegionsInvMETBDTR3 = HNL_LeptonCore::ElectronInvMETCR3BDT;
      LimitRegionsInvBJetBDTR3 = HNL_LeptonCore::ElectronInvBJetCR3BDT;

    }
    if (channel == EMu) {

      LimitRegionR1  = HNL_LeptonCore::ElectronMuonCR1;
      LimitRegionR2  = HNL_LeptonCore::ElectronMuonCR2;
      LimitRegionR3  = HNL_LeptonCore::ElectronMuonCR3;
      LimitRegionsBDTR3 = HNL_LeptonCore::ElectronMuonCR3BDT;


      LimitRegionsInvMETR1 = HNL_LeptonCore::ElectronMuonInvMETCR1;
      LimitRegionsInvBJetR1 = HNL_LeptonCore::ElectronMuonInvBJetCR1;
      LimitRegionsInvMETR2 = HNL_LeptonCore::ElectronMuonInvMETCR2;
      LimitRegionsInvBJetR2 = HNL_LeptonCore::ElectronMuonInvBJetCR2;
      LimitRegionsInvMETR3 = HNL_LeptonCore::ElectronMuonInvMETCR3;
      LimitRegionsInvBJetR3 = HNL_LeptonCore::ElectronMuonInvBJetCR3;

      LimitRegionsInvMETBDTR3 = HNL_LeptonCore::ElectronMuonInvMETCR3BDT;
      LimitRegionsInvBJetBDTR3 = HNL_LeptonCore::ElectronMuonInvBJetCR3BDT;

    }
  }
  
  if(HasFlag("ScanSystematicMET")){
    cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " METv = " << METv.Pt() << " ev.MET2ST() = " << ev.MET2ST() << endl;
    return;
  }

  if(HasFlag("ScanSystematic")){
    if(param.GetSystType() == "Central" ) cout << "######################################" << endl;
    cout << "Scanning effects on Obj with Syst ["<< param.GetSystType()<<"] "<< endl;
    cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " channel " << GetChannelString(channel) << " weight = " << weight_reg<< endl;
    cout << "-----------------------" << endl;
    cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " METv = " << METv.Pt() << " ev.MET2ST() = " << ev.MET2ST() << endl;
    cout << "-----------------------" << endl;
    cout << "-----------------------" << endl;
    cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " N(bjet) = "<<B_JetColl.size() << endl;
    cout << "Scan jets " << endl;
    int count = 0;  // Initialize counter
    for (std::size_t ij = 0; ij < JetColl.size(); ij++) {
      cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " Jet " << count++ << " pt = " << JetColl[ij].Pt() << endl;
    }
    cout << "-----------------------" << endl;
    count = 0;  
    for (std::size_t ij=0; ij < AK8_JetColl.size(); ij++) {
      Particle N=  AK8_JetColl[ij] + *LepsT[0]+*LepsT[1];
      cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " FatJet " << count++ << " pt = " << AK8_JetColl[ij].Pt() << " N mass = " << N.M() << endl;

    }    
    cout << "-----------------------" << endl;
    cout << "-----------------------" << endl;

    cout << "Scan leptons " << endl;
    count = 0;  // Initialize counter                                                                                                                                                                           
    for (auto& il : LepsT) {
      cout << " Event  " << event << " " << il->GetFlavour() << " " << count++ << " pt = " << il->Pt() << " eta = " << il->Eta() << endl;
      if(il->Pt() > 150 && fabs(il->Eta()) > 1.5) cout << "HPT EC" << endl;
    }        
    
    cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " SR2: HTOverPt = " << LepsT[0]->HTOverPt() << endl;
    cout << "######################################" << endl;

  }

  if(AK8_JetColl.size() > 0)  {
    if(!ConversionSplittingReg(LepsT,RunConv,1,2,param)) return;
  }
  
  else {
    TString SR2check=RunSignalRegionWWString(IsSR, channel, qq, LepsT, LepsV, TauColl, VBF_JetColl, AK8_JetColl, B_JetColl, B_JetColl, ev, METv, param, -999.0) ;
    if(SR2check != "false" && SR2check != "NULL") {
      if(!ConversionSplittingReg(LepsT,RunConv,2,2,param)) return;
    }
    else{
      if(!ConversionSplittingReg(LepsT,RunConv,3,2,param)) return;
    }
  }
  
  if(!PassPreselection(IsSR,channel,qq, 
           LepsT, LepsV, TauColl, 
           JetColl, VBF_JetColl, AK8_JetColl, B_JetColl,
           ev, METv ,param, weight_reg)) return;


  
  if(AK8_JetColl.size() > 0) {

    // 1) Define your mass groups clearly (ints are simpler/cheaper than strings)
    struct MassGroup {
      int ref_mass;
      std::vector<int> masses;
    };
    
    const std::vector<MassGroup> mass_groups = {
      {400, {85, 90, 95, 100, 125, 150, 200, 250, 300,350, 400}},
      {450, {450}},
      {500, {500}},
      {600, {600}},
      {700, {700}},
      {800, {800}},
      {900, {900}},
      {1000, {1000}},
      {1100, {1100}},
      {1200, {1200, 1300}},
      {1500, {1500, 1700}},
      {2000, {2000, 2500, 3000}},
    };
    
    // Cache these once
    const bool isSR      = IsSR;
    const bool isSignal  = IsSignal();
    
    // If you keep TString elsewhere, ok; otherwise prefer std::string
    const TString mcSampleName = MCSample; // or however you get it

    // Top level debug
    if(run_Debug)std::cout << "[DEBUG] New event: sample=" << mcSampleName
        << " isSignal=" << isSignal
        << " isSR=" << isSR
        << " nAK8=" << AK8_JetColl.size()
        << std::endl;
    
    
    for (const auto& grp : mass_groups) {
      const int ref_mass = grp.ref_mass;
      const auto& masses = grp.masses;
      
      // --- CR (control region) runs only for the first ref mass (400) ---
      // This matches your comment "CR only run in first loop as CR is not mass dependant"


      bool sr1_fill_plot = (ref_mass == 400 || isSignal);

      if(run_Debug)std::cout << "[DEBUG]  Mass group loop: ref_mass=" << ref_mass << " sr1_fill_plot=" << sr1_fill_plot << std::endl;

      if (!isSR && !sr1_fill_plot && !isSignal) {
        if(run_Debug)std::cout << "[DEBUG]   -> Skipping group ref_mass=" << ref_mass
           << " due to CR condition "
           << "(isSR=" << isSR
           << ", sr1_fill_plot=" << sr1_fill_plot
           << ", isSignal=" << isSignal << ")"
           << std::endl;
       continue;
      }
            
      // --- For signal samples, restrict to allowed mass range(s) ---
      // We assume sample names contain exact tokens like "M500_private".
      bool inSignalRange = true;
      if (isSignal) {
        inSignalRange = std::any_of(
                  masses.begin(), masses.end(),
                  [&](int m) {
                    // Build exact token once; TString::Format to avoid temp std::string
                    const TString token = TString::Format("M%d_private", m);
                    return mcSampleName.Contains(token);
                  }
                  );
        if (!inSignalRange) continue;
      }

      
      TString RegionBin= RunSignalRegionAK8String (IsSR, TString::Format("%d", ref_mass),0,sr1_fill_plot, channel,qq, LepsT, LepsV, TauColl, 
               JetColl, AK8_JetColl,B_JetColl,B_JetColl_CR,
               ev, METv ,param,weight_reg) ;
      
      if( RegionBin != "false") {
  
        /// Region 1+2+3                                                                                                                                                                  
        //FillLimitInput(LimitRegionsR1, weight_reg,   RegionBin,  "LimitExtraction/"+param.Name,"SR1_"+channel_string,channel_string);
        

        //// Dont plot if running all systematics                                                                                                                                 
        if(!runSyst&&param.runPlotter){
          if(IsSR&&param.IsCentral() && sr1_fill_plot) Fill_RegionPlots(param,"AllSR" , TauColl, 
                        JetColl, AK8_JetColl, LepsT, 
                        METv, nPV, weight_reg);
        }
        
        if(param.runPlotter&&sr1_fill_plot){
          if(IsSR)FillCutflow(HNL_LeptonCore::ChannelDepSR1, weight_reg, channel_string +"_SR1",param);
          else FillCutflow(HNL_LeptonCore::ChannelDepCR1, weight_reg, channel_string +"_CR1",param);
        }
        
        //// Region1 only limit
        if(IsSR){
          //// Used for scan not needed for analysis 
          if(HasFlag("SR1Scan")){
            FillLimitInput(LimitRegionR1, weight_reg,   RegionBin,  "LimitExtraction/"+param.Name,"SR1_"+channel_string,channel_string);
          }

          FillLimitInput(LimitRegionR1, weight_reg,   RegionBin,  "LimitExtraction/"+param.Name+"/M"+ref_mass,"SR1_"+channel_string,channel_string);
          
        }
        else{
          
          FillLimitInput(LimitRegionR1, weight_reg,   RegionBin,  "LimitExtraction/"+param.Name,"CR1_"+channel_string,channel_string);
          if(B_JetColl_CR.size() == 1)       FillLimitInput(LimitRegionsInvBJetR1, weight_reg*param.w.btagSF_tight/param.w.btagSF,   RegionBin+"_InvBJet",  "LimitExtraction/"+param.Name,"CR1_"+channel_string,channel_string);
          else if(B_JetColl.size()==0) FillLimitInput(LimitRegionsInvMETR1, weight_reg,   RegionBin+"_InvMET",  "LimitExtraction/"+param.Name,"CR1_"+channel_string,channel_string);
        }
      }
    }

    return;
  }//// END OF AK8 Loop
  else{

    //// Fail AK8 Req

    TString RegionBin = RunSignalRegionWWString(IsSR, channel, qq, LepsT, LepsV, TauColl, VBF_JetColl, AK8_JetColl, B_JetColl, B_JetColl, ev, METv, param, weight_reg); // In IB CR2, we use Medium WP for b tag

    //// SSWW SR events with  MJJ < 700 removed frmo analysis 

    double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*LepsT[0]).Phi() - (*LepsT[1]).Phi() )) );
    bool SSWWVeto= (RegionBin == "NULL" && ll_dphi > 2 && LepsT[0]->HTOverPt() < 1);

    /// NULL means MJJ is low 
    if(RegionBin != "false" && RegionBin != "NULL") {

      if(param.syst_ == AnalyzerParameter::PDFUp)   weight_reg*=GetPDFUncertainty("SR2",1);
      if(param.syst_ == AnalyzerParameter::PDFDown) weight_reg*=GetPDFUncertainty("SR2",-1);


      //// Dont plot if running all systematics
      if(!runSyst&&param.runPlotter){
        if(IsSR&&param.IsCentral()) Fill_RegionPlots(param,"AllSR" , TauColl, JetColl, AK8_JetColl, LepsT,  METv, nPV, weight_reg);
      }
      
      if(IsSR)FillCutflow(HNL_LeptonCore::ChannelDepSR2, weight_reg, channel_string +"_SR2",param);
      else FillCutflow(HNL_LeptonCore::ChannelDepCR2, weight_reg, channel_string +"_CR2",param);
      
      /// Region 1+2+3
      //      FillLimitInput(LimitRegions, weight_reg, RegionBin,"LimitExtraction/"+param.Name);

      /// Region 2 only Limit
      if(IsSR) {
        FillLimitInput(LimitRegionR2, weight_reg, RegionBin,  "LimitExtraction/"+param.Name,"SR2",channel_string);

        //std::cout << "[DEBUG] RunAllSignalRegions called" << std::endl;
        //std::cout << "  Event ev         = " << event << std::endl;
        //std::cout << "  MET pt           = " << METv.Pt() << std::endl;
        //std::cout << "  weight_ll        = " << weight_reg << std::endl;
        //std::cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " channel " << GetChannelString(channel) << " weight = " << weight_reg<< endl;
        //for(auto ilep : LepsT) cout << "Lepton pt " << ilep->Pt() << endl;
        //Particle ll =  (*LepsT[0]) + (*LepsT[1]);
        //std::cout << "  mll = " << ll.M() << endl;
        
        /// Use an alternative binning
        //TString RegionBinAlt = GetSingleBinnedWWString(LepsT[0]->HTOverPt(),ll_dphi);
        //FillLimitInput(LimitRegionR2, weight_reg, RegionBinAlt,  "LimitExtractionAlt/"+param.Name,"SR2",channel_string);
      }
      else {
	FillLimitInput(LimitRegionR2, weight_reg, "CR2",  "LimitExtraction/"+param.Name,"CR2",channel_string);

        if(B_JetColl.size() == 1)      FillLimitInput(LimitRegionsInvBJetR2, weight_reg, RegionBin, "LimitExtraction/"+param.Name, "CR2", channel_string); // In IB CR2, we use Medium WP for b tag
        else if(B_JetColl.size() == 0) FillLimitInput(LimitRegionsInvMETR2, weight_reg, RegionBin, "LimitExtraction/"+param.Name, "CR2", channel_string);

      }
    }
    else{

      
      //// Fail VBF Req ---> SR3
      // This block handles events that fail the VBF (Vector Boson Fusion) requirements and are assigned to signal region SR3
      
      /// If using up/down : note Jihun should use PDF weights in limit step so this should be depreciated
      // Apply PDF systematic variations if specified
      // The usage here is legacy and should be replaced by proper handling in the limit-setting step
      if(param.syst_ == AnalyzerParameter::PDFUp) weight_reg *= GetPDFUncertainty("SR3", 1);   // Apply PDF uncertainty upward variation
      if(param.syst_ == AnalyzerParameter::PDFDown) weight_reg *= GetPDFUncertainty("SR3", -1);  // Apply PDF uncertainty downward variation
      
      if(IsSR && B_JetColl.size() == 0 && ev.MET2ST() < 15) FillCutflow(HNL_LeptonCore::SRLowMass, weight_reg, "SR3_LowMass", param);
      if(!IsSR  && B_JetColl_CR.size() == 1 && ev.MET2ST() < 15) FillCutflow(HNL_LeptonCore::CRLowMass, weight_reg*param.w.btagSF_tight/param.w.btagSF, "CR3_IB",param);
      if(!IsSR  && B_JetColl.size() == 0 && ev.MET2ST() > 15) FillCutflow(HNL_LeptonCore::CRLowMass, weight_reg, "CR3_IM",param);
      
      /// RunBDT checks if:
      // a) signal mc and mass <= 500 --> Pass
      // b) non signal --> Pass
      if(RunBDT()){

        /// FinalBDTHyperParamMap: This has NCut/NTree info for each Mass/Version/channel scenario
        for(auto imapHP :FinalBDTHyperParamMap){

          // Only run BDT for this channel                                                                                                                          
          if(!imapHP.first.Contains(channel_string)) continue;
          
          /// Loop over BDT versions
          for(auto iversion : BDTVersions_to_run){
            vector<TString> bin_criteria;
            if (IsSR) {
              if(HasFlag("AllBDTVersions")){
                if(iversion == "V2") bin_criteria = {"StrictBin"};
                if(iversion == "V3") bin_criteria = {"StrictBin","Strict_10_Bin", "Strict_15_Bin", "Strict_20_Bin"};
                if(iversion == "V4") bin_criteria = {"Strict_10_Bin", "Strict_15_Bin", "Strict_20_Bin"};
              }
              else bin_criteria = {"Strict_15_Bin"};

            } else {
              bin_criteria = {""};
            }
            for(auto ibinning : bin_criteria){

	      // Only run BDT for version in BDTVersions_to_run 	  
	      if(!imapHP.first.Contains(iversion)) continue;
	      
	      /// GetBDTSignalMass gets for :
	      // a) bkg: TString of mass of sample i.e., M200, based on imapHP key
	      // b) signal : gets mass string but if mass string is not in sample name it returns NULL
	      /// -->  Only plot limits for BDT if signal is same mass as imapHP.first
	      TString SampleMass = GetBDTSignalMass(imapHP.first);
	      
	      //// Fill SR Cutflow for just one mass and channel BUT loop over versions
	      
	      if(imapHP.first.Contains(SampleMass+"_"+channel_string+"_"+TString(iversion))){
		
		TString RegBDT = RunSignalRegionAK4StringBDT(IsSR,SampleMass , iversion, ibinning,  imapHP.second.first, imapHP.second.second, channel,qq, LepsT, JetColl,  B_JetColl,B_JetColl_CR, ev, METv ,param,weight_reg);
		
		if(RegBDT != "false"){
		  
		  //// iversion now added to direrctory structure
		  FillLimitInput(LimitRegionsBDTR3,
				 weight_reg,
				 RegBDT,
				 "LimitExtractionBDT/"+param.Name+"_"+iversion+"_"+ibinning+"/M"+SampleMass,
				 "SR3BDT_"+channel_string+"_"+SampleMass+"_"+iversion+"_"+ibinning,
				 channel_string);
		  
		  
		  if(!IsSR){
		    if(B_JetColl_CR.size()==1) FillLimitInput(LimitRegionsInvBJetBDTR3,
							   weight_reg*param.w.btagSF_tight/param.w.btagSF,
							   RegBDT,
							   "LimitExtractionBDT/"+param.Name+"_"+iversion+"/M"+SampleMass,
							   "SR3BDT_"+channel_string+"_"+SampleMass+"_"+iversion,
							   channel_string);
		    
		    else  if(B_JetColl.size()==0) FillLimitInput(LimitRegionsInvMETBDTR3,
					weight_reg,
					RegBDT,
					"LimitExtractionBDT/"+param.Name+"_"+iversion+"/M"+SampleMass,
					"SR3BDT_"+channel_string+"_"+SampleMass+"_"+iversion,
					channel_string);
		  }
		}
	      }
	    }
	  }
	}
      }

      if(IsSR&&SSWWVeto) return;
      
      RegionBin  = RunSignalRegionAK4String (IsSR,channel,qq, LepsT, LepsV, TauColl, JetColl, AK8_JetColl, B_JetColl,B_JetColl_CR, ev, METv ,param,weight_reg);
      if(RegionBin != "false") {

        //CheckBin("DYType",RegionBin,"SR3_bin4",channel,param,LepsT,JetColl, AK8_JetColl,B_JetColl, METv,weight_reg);
        
        //// Dont plot if running all systematics
        
        if(!runSyst&&param.runPlotter){
          if(IsSR&&param.IsCentral()) Fill_RegionPlots(param,"AllSR" , TauColl, JetColl, AK8_JetColl, LepsT,  METv, nPV, weight_reg);
        }
        
        if(IsSR) FillCutflow(HNL_LeptonCore::ChannelDepSR3HM, weight_reg, channel_string +"_SR3",param);
        else  FillCutflow(HNL_LeptonCore::ChannelDepCR3HM, weight_reg, channel_string +"_CR3",param)  ;
        
        if(IsSR) FillCutflow(HNL_LeptonCore::SRHighMass, weight_reg, "SR3_HighMass",param);

        //// Binned R1+2+3 only limit input
        //  FillLimitInput(LimitRegions, weight_reg,   RegionBin,"LimitExtraction/"+param.Name);
        ///  R3 HighMass only limit input

        if(run_Debug)cout << "Event  " << event << " param.GetSystType() = " << param.GetSystType()  << " channel " << GetChannelString(channel) << " weight = " << weight_reg<< endl;

        if(IsSR) FillLimitInput(LimitRegionR3, weight_reg, RegionBin,  "LimitExtraction/"+param.Name,"SR3",channel_string);
        else{
          FillLimitInput(LimitRegionR3, weight_reg, RegionBin,  "LimitExtraction/"+param.Name,"CR3",channel_string);
          if(B_JetColl_CR.size() == 1)       FillLimitInput(LimitRegionsInvBJetR3, weight_reg*param.w.btagSF_tight/param.w.btagSF, RegionBin,  "LimitExtraction/"+param.Name,"CR3",channel_string);
          else    if(B_JetColl.size() == 0)   FillLimitInput(LimitRegionsInvMETR3, weight_reg, RegionBin,  "LimitExtraction/"+param.Name,"CR3",channel_string);
        }
      }
    }
  }

  return;
}




bool  HNL_RegionDefinitions::PassPreselection(bool ApplyForSR,HNL_LeptonCore::Channel channel,HNL_LeptonCore::ChargeType qq, std::vector<Lepton *>& leps,std::vector<Lepton *>& leps_veto, vector<Tau>& TauColl,  std::vector<Jet>& JetColl, std::vector<Jet>& VBF_JetColl,std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, Event& ev, Particle& METv, AnalyzerParameter& param,  float w){

  // ONLY CODE SS PRESLECTION 
  
  // APPLY PT CUTS
  /*
    ee channel 25/10
    emu   25/5
    mm    20/5
    
    
   */
  
  if(run_Debug) cout << "HNL_RegionDefinitions::PassPreselection " << GetChannelString(channel) <<  endl;

  // Make sure events contain 2 leps
  if (leps_veto.size() != 2) return false;
  
  // CUT ON MASS OF LL PAIR

  Particle ll =  (*leps[0]) + (*leps[1]);
  
  // VETO Z PEAK IN EE CHANNEL
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  if (param.SRConfig == "PreselectionV2") {
    if ((fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return false;
  }

  
  if(ll.M() < M_CUT_LL) return false; // TO_CHECK: IS 20 BEST OPTION
  
  if(TauColl.size() > 0) return false;


  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRLowMass, w, "Preselection",param);
  if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRHighMass, w, "Preselection",param);

  if(!ApplyForSR) FillCutflow(HNL_LeptonCore::CRLowMass, w, "Preselection",param);
  
  //// Dont plot if running all systematics
  if(!runSyst&&param.runPlotter){
    if(param.IsCentral())Fill_RegionPlots(param,"Preselection" , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    if(param.IsCentral())Fill_RegionPlots(param,"Preselection_vbf" , TauColl, VBF_JetColl, AK8_JetColl, leps,  METv, nPV, w);
  }
  
  FillCutflow(HNL_LeptonCore::ChannelDepPresel, w, GetChannelString(channel) +"_Presel",param);
  
  return true;
}




bool  HNL_RegionDefinitions::RunSignalRegionAK8(bool ApplyForSR,TString mass_range, int binning_method,bool fill_plots, HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq , std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& TauColl, std::vector<Jet>& JetColl, std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, std::vector<Jet>& B_JetColl_CR, Event& ev, Particle& METv, AnalyzerParameter& param, float w){


  TString SR1String = RunSignalRegionAK8String(ApplyForSR,mass_range,binning_method,fill_plots, channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,B_JetColl_CR, ev, METv, param, w);
  
  if(SR1String == "false") return false;
  else return true;
  
}


/// Return TString for Limit bin

TString HNL_RegionDefinitions::RunSignalRegionAK8String(bool ApplyForSR,
              TString ref_mass,
              int binning_method,
              bool fill_plots,
              HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq , 
              std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& TauColl,
              std::vector<Jet>& JetColl, std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, std::vector<Jet>& B_JetColl_CR, 
              Event& ev, Particle& METv, AnalyzerParameter& param,  float w){
   
  double met_cut     = 10; 
  if(channel == MuMu) met_cut     = 15;

  double met2_st     = ev.MET2ST(); 
  bool PassHMMet     = ApplyForSR ? (met2_st <= met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl_CR.size()== 1);

  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  ((PassHMMet &&B_JetColl.size()==0)|| (PassBJetMVeto)) ;


  TString RegionTag                = ApplyForSR ? "SR1" : "CR1";
  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR1 : HNL_LeptonCore::CR1;

  
  if(fill_plots)FillCutflow(Reg, w, RegionTag+"_Init",param);

  if(qq==Plus && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";
   if(fill_plots) FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";  
  if (leps_veto.size() != 2) return "false";

  if(fill_plots){
    //// Dont plot if running all systematics
    if(!runSyst&&param.runPlotter){
      if(param.IsCentral()) Fill_RegionPlots(param,"Inclusive"+RegionTag , TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    }
  }
  
  if(leps[1]->Pt() < 20) return "false";
    
  if(fill_plots)FillCutflow(Reg, w, RegionTag+"_lep_pt",param);
  
  Particle ll =  (*leps[0]) + (*leps[1]);
  if(ll.M() < M_CUT_LL) return "false";

  if(fill_plots)FillCutflow(Reg, w, RegionTag+"_dilep_mass",param);
  
  if(AK8_JetColl.size() != 1)  return "false";

  if(fill_plots){
    FillCutflow(Reg, w, RegionTag+"_1AK8",param);
    
    if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRLowMass,  w, "SR1",param);
    if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRHighMass, w, "SR1",param);
    if(!ApplyForSR && ((met2_st <= met_cut) &&B_JetColl_CR.size()==1)) FillCutflow(HNL_LeptonCore::CRLowMass, w*param.w.btagSF_tight/param.w.btagSF, "CR1_IB",param);
    if(!ApplyForSR && ((met2_st > met_cut) &&B_JetColl.size()==0) ) FillCutflow(HNL_LeptonCore::CRLowMass, w, "CR1_IM",param);

      
    if(PassHMMet)FillCutflow(Reg, w, RegionTag+"_MET",param);
    if(!ApplyForSR&&PassBJetMVeto) FillCutflow(Reg, w, RegionTag+"_bveto",param);
    
    if(PassRegionReq&&ApplyForSR) FillCutflow(Reg, w, RegionTag+"_bveto",param);    
  }
  
  if(!PassRegionReq) return "false";
  
  //Fill Limit plot

  /// Define W* mass
  Particle Wcand = AK8_JetColl[0] + *leps[0] + *leps[1];
  
  //// Fill Plots before All SR cuts for better stats 
  if(fill_plots&&param.runPlotter){
    if(ApplyForSR) Fill_RegionPlots(param,"Pass"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    else{
      
      if(B_JetColl_CR.size() ==1) Fill_RegionPlots(param,"Pass"+RegionTag+"_InvBJet" ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w*param.w.btagSF_tight/param.w.btagSF);
      else    Fill_RegionPlots(param,"Pass"+RegionTag+"_InvMET" ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    }
  }
  
  //// Apply CR Binning
  if(!ApplyForSR){
    if(B_JetColl_CR.size() == 1)   runCutCounts[event].Cut((TString("SR1_IB_") + GetChannelString(channel)).Data());      
    
    if(fill_plots){
      FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/Inv_SR1",0,  w, 1,0,1 ,"CR Binned");
      if(B_JetColl_CR.size() == 1)      FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvBJet_SR1",0,  w*param.w.btagSF_tight/param.w.btagSF, 1,0,1 ,"CR Binned");
      else  FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR1",0,  w, 1,0,1 ,"CR Binned");
    }
    return "CR1";
  }

  Particle N1cand = AK8_JetColl[0] + *leps[0];
  double MN1 = (N1cand.M() > 2000.) ? 1999. : N1cand.M();

  //// Check new suggestion for W mass constraint 
  if(HasFlag("UseWMassConstraint")) MN1 = N1cand.M() - AK8_JetColl[0].SDMass() + M_W;
  if(MN1 > 2000.)  MN1 = 1999.;
  
  vector<double> ml1jbins = GetLimitBinBoundary("SR1",ref_mass,GetChannelString(channel));
  
  for(unsigned int ibin=1; ibin < ml1jbins.size(); ibin++){
    if(MN1 < ml1jbins[ibin]) return RegionTag+"_MNbin"+to_string(ibin);
  }
  
  return "true";

}




bool  HNL_RegionDefinitions::RunSignalRegionWW(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& TauColl,  std::vector<Jet>& JetColl, std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, std::vector<Jet>& B_JetColl_CR, Event& ev, Particle& METv, AnalyzerParameter& param,   float w){

  TString SR2String = RunSignalRegionWWString(ApplyForSR,channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,B_JetColl_CR,ev, METv, param, w);
  if(SR2String == "false") return false;
  else return true;
  
}



TString HNL_RegionDefinitions::RunSignalRegionWWString(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto , std::vector<Tau>& TauColl,   std::vector<Jet>& JetColl, std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, std::vector<Jet>& B_JetColl_CR, Event& ev, Particle& METv, AnalyzerParameter& param,  float w){

  double met_cut     = 15;
  double met2_st     = ev.MET2ST();

  bool FillPlots = (w == -999.0) ?  false : true;
  
  TString RegionTag                = ApplyForSR ? "SR2" : "CR2";
  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR2 : HNL_LeptonCore::CR2;


  bool PassHMMet     = ApplyForSR ? (met2_st < met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl_CR.size() ==1);

  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  ((PassHMMet &&B_JetColl.size()==0)|| (PassBJetMVeto)) ;

 
  if (leps_veto.size() != 2) return "false";

  if(qq==Plus  && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";

  if(FillPlots)FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";

  /// Keep 15 GeV for SR binning stats
  if(leps[1]->Pt() < 15.) return "false";

  if(FillPlots)FillCutflow(Reg, w, RegionTag+"_lep_pt",param);

  if(ApplyForSR) Fill_RegionPlots(param,"Initial"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

  bool use_leadjets=true;

  if(FillPlots)FillCutflow(Reg, w, RegionTag+"_DPhi",param);
  
  if( ( (*leps[0]) + (*leps[1]) ).M() < 20.) return "false";
  if(FillPlots)FillCutflow(Reg, w, RegionTag+"_LLMass",param);

  if(JetColl.size() < 2) return "false";
  if(FillPlots)FillCutflow(Reg, w, RegionTag+"_DiJet",param);

  int ijet1(0), ijet2(1);

  if(use_leadjets){ijet1=0;ijet2=1;}

  if(HasFlag("RemoveCentralVBFJets")){

    if(fabs(JetColl[ijet1].Eta()) < 1.5)  return "false";
    if(fabs(JetColl[ijet2].Eta()) < 1.5)  return "false";
  }
  
  double maxDiJetDeta =fabs(JetColl[ijet1].Eta() - JetColl[ijet2].Eta());

  if(maxDiJetDeta < 2.5) return "false";
  if(FillPlots)FillCutflow(Reg, w, RegionTag+"_DiJetEta",param);

  //// Dont plot if running all systematics
  if(!runSyst&&param.runPlotter){
    if(param.IsCentral()&&FillPlots) Fill_RegionPlots(param,"Inclusive"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
  }
  
  Particle JJ = JetColl[ijet1] + JetColl[ijet2];
  if(ApplyForSR) {
    if(JJ.M() < 700) {
      return "NULL";
    }
  }
  else     if(JJ.M() < 500) return "NULL";
  
  if(FillPlots)FillCutflow(Reg, w, RegionTag+"_DiJetMass",param);
  
  double Av_JetEta= 0.5*(JetColl[ijet1].Eta()+ JetColl[ijet2].Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);
  
  if(zeppenfeld > 0.75) return "false";
  FillCutflow(Reg, w, RegionTag+"_Zep",param);
  
  /// remove mjj cut set to 0
  //  if(!PassVBF(JetColl,leps,0., true)) return "false";
  if(FillPlots){
    if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRLowMass, w, "SR2",param);
    if(ApplyForSR) FillCutflow(HNL_LeptonCore::SRHighMass, w, "SR2",param);
    if(!ApplyForSR && ((met2_st <= met_cut) && B_JetColl_CR.size()==1)) FillCutflow(HNL_LeptonCore::CRLowMass, w, "CR2_IB",param);
    if(!ApplyForSR && ((met2_st > met_cut) && B_JetColl.size()==0)) FillCutflow(HNL_LeptonCore::CRLowMass, w, "CR2_IM",param);
    
    //// Dont plot if running all systematics
    if(!runSyst&&param.runPlotter){
      if(param.IsCentral()) Fill_RegionPlots(param,"PassVBF"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    }
    
    FillCutflow(Reg, w, RegionTag+"_VBF",param);

  }
  
  if(PassRegionReq){
    if(FillPlots){  
      if(PassHMMet) FillCutflow(Reg, w, RegionTag+"_met",param);
      if(PassBJetMVeto) FillCutflow(Reg, w, RegionTag+"_bveto",param);
      
      if(param.runPlotter){
	if(ApplyForSR)        Fill_RegionPlots(param,"Pass"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);      
	else{
	  if(B_JetColl_CR.size() == 1) Fill_RegionPlots(param,"Pass"+RegionTag+"_InvBJet", TauColl, JetColl, AK8_JetColl, leps, METv, nPV, w);
	  else Fill_RegionPlots(param,"Pass"+RegionTag+"_InvMET", TauColl, JetColl, AK8_JetColl, leps, METv, nPV, w);
	}
      }
    }
    
    
    double HTOverPT = leps[0]->HTOverPt();
    
    if(FillPlots)FillCutflow(Reg, w, RegionTag+"_ht_lt1",param);
    
    double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
    
    
    if(!ApplyForSR){

      //double Wmass = GetRecoObjMass("HNL_SR3_NLL",JetColl, AK8_JetColl, leps);

      //if(HasFlag("WMassWindow")  && Wmass > 200 && Wmass < 250.) {
      //if(B_JetColl.size() ==1)   Fill_RegionPlots(param,"Pass"+RegionTag+"_Window_InvBJet" ,  TauColl, JetColl, AK8_JetColl, leps,  METv,    nPV, w);
      // }

      //if(HasFlag("ApplyWMassCut") && (Wmass > 0) &&  (Wmass < 300)) return "false";
      //////// CR LIMIT BINS
      if(FillPlots){
        FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/Inv_SR2", 0,  w, 1,0,1 ,"Reco H_{T}/P_{T}^{lep1}");
        if(B_JetColl_CR.size() == 1)  FillHist("LimitExtraction/"+param.Name+"/"+RegionTag+"/InvBJet_SR2", 0, w, 1, 0, 1, "Reco H_{T}/P_{T}^{lep1}");
        else{
          if(HTOverPT < 3.)  FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR2", 0,  w, 3,0,3 ,"Reco H_{T}/P_{T}^{lep1}");
          else if(HTOverPT < 5.) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR2", 1,  w, 3,0,3 ,"Reco H_{T}/P_{T}^{lep1}");
          else                   FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR2", 2,  w, 3,0,3 ,"Reco H_{T}/P_{T}^{lep1}");
        }
      }
      if(B_JetColl_CR.size() == 1)    return "CR2_InvBJet";
      if(HTOverPT < 3.) return  "CR2_InvMET_HTLT_Bin1";
      else if(HTOverPT < 5.) return  "CR2_InvMET_HTLT_Bin2";
      else  return  "CR2_InvMET_HTLT_Bin3";
    }
    else{
      
      // Define cuts by era and channel
      struct CutValues {
        double cut1, cut2, cut3, cut4, cut5, cut6;};
        
      std::map<std::string, std::map<int, CutValues>> sr2_cuts = {
          {"Run2_PerFlavour_EraStatReq", {
            {HNL_LeptonCore::Channel::MuMu, {2.2, 3.6, 4.5, 0.9, 1.5, 2.3}},
            {HNL_LeptonCore::Channel::EE,   {0.9, 2.4, 3.2, 0.7, 1.3, 2.5}},
            {HNL_LeptonCore::Channel::EMu,  {0.9, 2.1, 3.2, 0.7, 1.3, 2.5}}
          }},
          {"Run2_Global_EraStatReq", {
            {HNL_LeptonCore::Channel::MuMu, {2.2, 3.2, 4.4, 0.9, 1.5, 2.4}},
            {HNL_LeptonCore::Channel::EE,   {2.2, 3.2, 4.4, 0.9, 1.5, 2.4}},
            {HNL_LeptonCore::Channel::EMu,  {2.2, 3.2, 4.4, 0.9, 1.5, 2.4}}
          }},

      };

      
      // Default cuts
      CutValues cuts{0.0, 0.0, 0.0, 0.0, 0.0,0.0};

      // Convert TString -> std::string for the map key
      //const std::string era = DataEra.Data();
      // Use your real channel variable name here (assumed 'channel')
      const HNL_LeptonCore::Channel chan = channel;
      
      // Safe lookup without creating entries
      std::string key = "Run2_PerFlavour_EraStatReq";

      if (auto eraIt = sr2_cuts.find(key); eraIt != sr2_cuts.end()) {
        if (auto chIt = eraIt->second.find(chan); chIt != eraIt->second.end()) {
          cuts = chIt->second;
        } else {
          throw std::runtime_error("Channel not found in sr2_cuts");
        }
      } else {
        throw std::runtime_error("Key not found in sr2_cuts");
      }
      
      
      // Bin selection (preserves your original logic)
      if (ll_dphi > 2.0) {
        if (HTOverPT < cuts.cut4) return RegionTag + "_HTLT_Bin1";
        if (HTOverPT < cuts.cut5) return RegionTag + "_HTLT_Bin2";
        if (HTOverPT < cuts.cut6) return RegionTag + "_HTLT_Bin3";
        return RegionTag + "_HTLT_Bin4";
      } else {
        if (HTOverPT < cuts.cut1) return RegionTag + "_HTLT_Bin5";
        if (HTOverPT < cuts.cut2) return RegionTag + "_HTLT_Bin6";
        if (HTOverPT < cuts.cut3) return RegionTag + "_HTLT_Bin7";
        return RegionTag + "_HTLT_Bin8";
      }
      
      
    }
  }
  return "false";
}



bool  HNL_RegionDefinitions::RunSignalRegionAK4(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto ,  std::vector<Tau>& TauColl,std::vector<Jet>& JetColl, std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, std::vector<Jet>& B_JetColl_CR, Event& ev, Particle& METv, AnalyzerParameter& param,   float w){

  TString SR3String = RunSignalRegionAK4String(ApplyForSR,channel, qq, leps, leps_veto, TauColl,JetColl,AK8_JetColl,B_JetColl,B_JetColl_CR,ev, METv, param,w);
  if(SR3String == "false") return false;
  else return true;

}




TString HNL_RegionDefinitions::RunSignalRegionAK4StringBDT(bool ApplyForSR, TString mN, TString version, TString Binning,  TString NCut, TString NTree, HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *>& LepTColl, std::vector<Jet>& JetColl, std::vector<Jet>& B_JetColl, std::vector<Jet>& B_JetColl_CR,Event& ev, Particle& METv, AnalyzerParameter& param,  float w){
  
  //// Only fill Cutflow/plots for 1 mass point and version
  bool FillCutFlow = (mN=="100" && version == "V3" && Binning == "Strict_15_Bin") ;

  TString BDTLabel = "M"+mN+"_NTrees"+NTree+"_NCuts"+NCut+"_MaxDepth3_"+version+"_"+Binning;
  TString BDTLabel_simple = "M"+mN+"_NTrees"+NTree+"_NCuts"+NCut+"_MaxDepth3_"+version;

  double met_cut     = 15; /// Apply buffer between SR and CR MET cuts
  vector<FatJet> AK8_JetColl;

  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR3BDT : HNL_LeptonCore::CR3BDT;


  double met2_st     = ev.MET2ST();

  bool PassHMMet     = ApplyForSR ? (met2_st < met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl_CR.size() ==1);

  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  ((PassHMMet &&B_JetColl.size()==0)|| (PassBJetMVeto)) ;

  TString RegionTag                = ApplyForSR ? "SR3BDT" : "CR3BDT";



  if(qq==Plus && LepTColl[0]->Charge() < 0) return "false";
  if(qq==Minus && LepTColl[0]->Charge() > 0) return "false";
  if(FillCutFlow)FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, LepTColl)) return "false";
  if(FillCutFlow)FillCutflow(Reg, w, RegionTag+"_lep_pt",param);

  if (LepTColl.size() != 2) return "false";

  Particle ll =  (*LepTColl[0]) + (*LepTColl[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) return "false";

  if(FillCutFlow)  FillCutflow(Reg, w, RegionTag+"_dilep_mass",param);

  //float MVAvalue = EvaluateEventMVA(mN, NCut,NTree, channel, LepTColl, ev, METv, param, w);
  //cout << "Summary of BDT " << mN << endl;
  //cout << "Predetermined value = " << MVAvalue << " " <<  ev.HNL_MVA_Event(GetChannelString(channel)+"_"+mN) << endl;


  if(!PassRegionReq)  return "false";
  if(FillCutFlow)  FillCutflow(Reg, w, RegionTag+"_MET",param);
  if(FillCutFlow) FillCutflow(Reg, w, RegionTag+"_bveto",param);

  bool isBDTVar = false;
  if(HasFlag("SSDilepBDT")) isBDTVar = true;

  float MVAvalueIncl    = EvaluateEventMVA(mN, "Incl", version, NCut, NTree, channel, LepTColl, ev, METv, param, w, isBDTVar); // true : fill MVA variables

  vector<Tau> TauColl;

  if(param.runPlotter){

    if(!ApplyForSR|| HasFlag("PlotBDT")){
      FillHist("LimitExtraction/"+param.Name+"/"+RegionTag+"BDT/"+BDTLabel, MVAvalueIncl, w, 400, -1., 1.);
      if(B_JetColl_CR.size() == 1) FillHist("LimitExtraction/"+param.Name+"/"+RegionTag+"BDT/InvBJet_"+BDTLabel, MVAvalueIncl, w*param.w.btagSF_tight/param.w.btagSF, 400, -1., 1.);
      else FillHist("LimitExtraction/"+param.Name+"/"+RegionTag+"BDT/InvMET_"+BDTLabel, MVAvalueIncl, w, 400, -1., 1.);
    }
    
    if((ApplyForSR) && Binning == "Strict_15_Bin")   FillHist("LimitExtraction/"+param.Name+"/"+RegionTag+"BDT/"+BDTLabel_simple, MVAvalueIncl, w, 2000, -1., 1.);
    
    if(ApplyForSR ) Fill_RegionPlots(param,"Pass"+RegionTag+"BDT" ,TauColl,  JetColl, AK8_JetColl, LepTColl,  METv, nPV, w);
    else{
      if(B_JetColl_CR.size() ==1)   Fill_RegionPlots(param,"Pass"+RegionTag+"BDT_InvBJet" ,  TauColl, JetColl, AK8_JetColl, LepTColl,  METv,    nPV, w*param.w.btagSF_tight/param.w.btagSF);
      else     Fill_RegionPlots(param,"Pass"+RegionTag+"BDT_InvMET" ,  TauColl, JetColl, AK8_JetColl, LepTColl,  METv, nPV, w);
    } 
  }
  
  if(FillCutFlow){
    if(ApplyForSR)FillCutflow(HNL_LeptonCore::ChannelDepSR3, w, GetChannelString(channel) +"_"+RegionTag,param);
    else FillCutflow(HNL_LeptonCore::ChannelDepCR3, w, GetChannelString(channel) +"_"+RegionTag,param);
  }

  ///////// BDT BINS NEED OPTIMISING

  vector<pair<TString, double > > BDTLimitBinsMu;
  vector<pair<TString, double > > BDTLimitBinsEl;    
  vector<pair<TString, double > > BDTLimitBinsEMu;                                                     
  
  /// Change binning so no end bins have < 0.5 bkg
    
  //// Check Binnings in HNL_LeptonCore_BDT.C 
  if(ApplyForSR) {
    if(channel == MuMu) SetBinningBDT(GetChannelString(channel), mN,RegionTag,  RegionTag+"_"+GetChannelString(channel)+"_"+mN +"_"+version+"_"+Binning ,  BDTLimitBinsMu);
    if(channel == EE)   SetBinningBDT(GetChannelString(channel), mN,RegionTag,  RegionTag+ "_"+GetChannelString(channel)+"_"+mN +"_"+version+"_"+Binning ,  BDTLimitBinsEl);
    if(channel == EMu)  SetBinningBDT(GetChannelString(channel), mN,RegionTag,  RegionTag+ "_"+GetChannelString(channel)+"_"+mN +"_"+version+"_"+Binning,  BDTLimitBinsEMu);
  }
  else {

    if(channel == MuMu) SetBinningBDT(GetChannelString(channel), mN,RegionTag, RegionTag+"_"+GetChannelString(channel) ,  BDTLimitBinsMu);
    if(channel == EE)   SetBinningBDT(GetChannelString(channel), mN,RegionTag, RegionTag+"_"+GetChannelString(channel) ,  BDTLimitBinsEl);
    if(channel == EMu)  SetBinningBDT(GetChannelString(channel), mN,RegionTag, RegionTag+"_"+GetChannelString(channel) ,  BDTLimitBinsEMu);
  }
  
  /// Need to Add EMu Binning as current EMu uses EE
  
  double  bdtbin = 0.5;
  if(channel == MuMu){ 
    for(auto imap : BDTLimitBinsMu){
      if(MVAvalueIncl< imap.second) {
        return imap.first;
      }
      bdtbin=bdtbin+1.;
    }  
  }
  if(channel == EE){
        
    for(auto imap : BDTLimitBinsEl){
      if(MVAvalueIncl< imap.second) {
        return imap.first;
      }
      bdtbin=bdtbin+1.;
    }
  }
  if(channel == EMu){

    for(auto imap : BDTLimitBinsEMu){
      if(MVAvalueIncl< imap.second) {
        return imap.first;
      }
      bdtbin=bdtbin+1.;
    }
  }


  return "true";
}


TString HNL_RegionDefinitions::RunSignalRegionAK4String(bool ApplyForSR,HNL_LeptonCore::Channel channel, HNL_LeptonCore::ChargeType qq ,std::vector<Lepton *>& leps, std::vector<Lepton *>& leps_veto ,  std::vector<Tau>& TauColl,std::vector<Jet>& JetColl, std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, std::vector<Jet>& B_JetColl_CR, Event& ev, Particle& METv, AnalyzerParameter& param,  float w){

  double met_cut     = 15. ;
  double met2_st     = ev.MET2ST();
  bool PassHMMet     = ApplyForSR ? (met2_st < met_cut) :  (met2_st > met_cut);
  bool PassBJetMVeto = ApplyForSR ? (B_JetColl.size()==0) : (B_JetColl_CR.size() == 1);

  bool PassRegionReq = ApplyForSR ? (PassHMMet && PassBJetMVeto) :  ((PassHMMet &&B_JetColl.size()==0)|| (PassBJetMVeto)) ;

  TString RegionTag                = ApplyForSR ? "SR3" : "CR3";
  HNL_LeptonCore::SearchRegion Reg = ApplyForSR ? HNL_LeptonCore::SR3 : HNL_LeptonCore::CR3;

  if(qq==Plus  && leps[0]->Charge() < 0) return "false";
  if(qq==Minus && leps[0]->Charge() > 0) return "false";
   FillCutflow(Reg, w, RegionTag+"_lep_charge",param);

  if(!CheckLeptonFlavourForChannel(channel, leps)) return "false";

  FillCutflow(Reg, w, RegionTag+"_lep_pt",param);
  if(!runSyst&&param.runPlotter){
    if(param.IsCentral()) Fill_RegionPlots(param,"Inclusive"+RegionTag ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);
  }
  
  if (leps_veto.size() != 2) return "false";

  //  if(TauColl.size() > 0) return "false";
  FillCutflow(Reg, w, RegionTag+"_tauveto",param);

  Particle ll =  (*leps[0]) + (*leps[1]);
  if (channel==EE  && (fabs(ll.M()-M_Z) < M_ZWINDOW_VETO)) {
    return "false";
  }
  

  FillCutflow(Reg, w, RegionTag+"_dilep_mass",param);

  if(!PassRegionReq) return "false";

  FillCutflow(Reg, w, RegionTag+"_MET",param);
  FillCutflow(Reg, w, RegionTag+"_bveto",param);
  
  if(JetColl.size() > 0){
    if(JetColl[0].Pt() < 25.)  return "false";
  }
  FillCutflow(Reg, w, RegionTag+"_J1Pt",param);

  
  /// Remove Low Pt in High Mass SR
  //  if(leps[1]->Pt() < 50 ||  ll.M() < 50) {
  //  return "false";
  //}

  FillCutflow(Reg, w, RegionTag+"_L1Pt",param);
  
  double LT = leps[0]->Pt() + leps[1]->Pt();
  
  if(LT < 100){
    return "false";
  }
  
  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );                                                        

  //Fill_RegionPlots(param,"Pass"+RegionTag ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);

  if(param.runPlotter){
    if(ApplyForSR)        Fill_RegionPlots(param,"Pass"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    else{
      if(B_JetColl_CR.size() ==1)   Fill_RegionPlots(param,"Pass"+RegionTag+"_InvBJet" ,  TauColl, JetColl, AK8_JetColl, leps,  METv,    nPV, w*param.w.btagSF_tight/param.w.btagSF);
      else     Fill_RegionPlots(param,"Pass"+RegionTag+"_InvMET" ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    }
  }
    
  ////// Set Limit Binned 
  if(ApplyForSR && JetColl.size() < 2){
    FillCutflow(Reg, w, RegionTag+"_lowjet",param);
    if(!runSyst&&param.runPlotter){
      if(param.IsCentral()){
        Fill_RegionPlots(param,"Pass"+RegionTag+"_LowJet" ,TauColl,  JetColl, AK8_JetColl, leps,  METv, nPV, w);
      }
    }

    if(ApplyForSR)        Fill_RegionPlots(param,"FullPass"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);

    //// These cuts are temp HL will check
    return GetSR3StringBin(RegionTag,GetChannelString(channel), true, met2_st,LT,ll_dphi,leps[0]->HTOverPt() );
  }  

  if(!ApplyForSR){

    if(LT < 150) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/Inv_SR3", 0,  w, 4,0,4,"LT");
    else  if(LT < 200) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/Inv_SR3", 1,  w, 4,0,4 ,"LT");
    else  if(LT < 300) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/Inv_SR3", 2,  w, 4,0,4 ,"LT");
    else FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/Inv_SR3", 3,  w, 4,0,4 ,"LT");
    
    if(B_JetColl_CR.size() == 1)  {
      if(LT < 150) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvBJet_SR3", 0,  w*param.w.btagSF_tight/param.w.btagSF, 4,0,4,"LT");
      else  if(LT < 200) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvBJet_SR3", 1,  w*param.w.btagSF_tight/param.w.btagSF, 4,0,4 ,"LT");
      else  if(LT < 300) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvBJet_SR3", 2,  w*param.w.btagSF_tight/param.w.btagSF, 4,0,4 ,"LT");
      else FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvBJet_SR3", 3,  w*param.w.btagSF_tight/param.w.btagSF, 4,0,4 ,"LT");
    }
    else{
      if(LT < 150) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR3", 0,  w, 4,0,4,"LT");
      else  if(LT < 200) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR3", 1,  w, 4,0,4 ,"LT");
      else  if(LT < 300) FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR3", 2,  w, 4,0,4 ,"LT");
      else FillHist(  "LimitExtraction/"+ param.Name+"/"+RegionTag+"/InvMET_SR3", 3,  w, 4,0,4 ,"LT");
    }
  }
  
  FillCutflow(Reg, w, RegionTag+"_dijet",param);
                                                          
  double dijetmass_tmp=999.;
  double dijetmass=9990000.;
  int m=-999;
  int n=-999;

  for(UInt_t emme=0; emme<JetColl.size(); emme++){
    for(UInt_t enne=1; enne<JetColl.size(); enne++) {
      if(emme == enne) continue;
      dijetmass_tmp = (JetColl[emme]+JetColl[enne]).M();

      if ( fabs(dijetmass_tmp-M_W) < fabs(dijetmass-M_W) ) {
        dijetmass = dijetmass_tmp;
        m = emme;
        n = enne;
      }
    }
  }

  if(ApplyForSR){
    Particle Wcand  = JetColl[m]+JetColl[n]+ *leps[0] + *leps[1];
    if(Wcand.M() < 400) return "false";
  }

  if(!runSyst&&param.runPlotter){
    if(ApplyForSR&&param.IsCentral()){
      Fill_RegionPlots(param,"Pass"+RegionTag +"_DiJet",  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
    }
  }
  
  if(!runSyst&&param.runPlotter){
    if(ApplyForSR)        Fill_RegionPlots(param,"FullPass"+RegionTag ,  TauColl, JetColl, AK8_JetColl, leps,  METv, nPV, w);
  }
  
  TString LimitBin = GetSR3StringBin(RegionTag,GetChannelString(channel), false, met2_st,LT,ll_dphi,leps[0]->HTOverPt() );

  return LimitBin;
}


HNL_RegionDefinitions::HNL_RegionDefinitions(){
      
}
 
HNL_RegionDefinitions::~HNL_RegionDefinitions(){

  
}

TString HNL_RegionDefinitions::GetSingleBinnedWWString(double HTOverPT, double ll_dphi){
  
  //// Try same cuts for all eras                                                                                                                                        
  if(ll_dphi > 2.) {
    if(HTOverPT < 1.5)  return "SR2_HTLT_Bin1";
    if(HTOverPT < 2.5)  return "SR2_HTLT_Bin2";
    return "SR2_HTLT_Bin3";
  }
  else{
    if(HTOverPT < 2)  return "SR2_HTLT_Bin4";
    if(HTOverPT < 3)  return "SR2_HTLT_Bin5";
    return "SR2_HTLT_Bin6";
  }
}

void HNL_RegionDefinitions::RunSR3BDT(HNL_LeptonCore::ChargeType qq, std::vector<Electron>& electrons, std::vector<Electron>& electrons_veto, std::vector<Muon>& muons, std::vector<Muon>& muons_veto,  std::vector<Tau>& TauColl, std::vector<Jet>& JetColl, std::vector<Jet>& VBF_JetColl,std::vector<FatJet>&  AK8_JetColl, std::vector<Jet>& B_JetColl, Event& ev,   Particle& METv, AnalyzerParameter& param,   float weight_ll){




  return;

}



bool HNL_RegionDefinitions::PassVBFInitial(vector<Jet>&  JetColl){

  if(JetColl.size() < 2) return false;

  int ijet1(0), ijet2(1);
  double maxDiJetDeta = fabs(JetColl[ijet1].Eta() - JetColl[ijet2].Eta());

  if(maxDiJetDeta < 2.5) return false;
  Particle JJ = JetColl[ijet1] + JetColl[ijet2];

  return true;


}

bool HNL_RegionDefinitions::PassVBF(vector<Jet>&  JetColl,std::vector<Lepton *>& leps, double mjj_cut ,bool use_leadjets){

  if(leps.size() != 2) return false;
  //  double ll_dphi = fabs(TVector2::Phi_mpi_pi( ( (*leps[0]).Phi() - (*leps[1]).Phi() )) );
  //  if(ll_dphi < 2.) return false;
  if( ( (*leps[0]) + (*leps[1]) ).M() < 20.) return false;
  if(JetColl.size() < 2) return false;

  int ijet1(0), ijet2(1);
  double maxDiJetDeta =fabs(JetColl[ijet1].Eta() - JetColl[ijet2].Eta());
  
  if(maxDiJetDeta < 2.5) return false;
  Particle JJ = JetColl[ijet1] + JetColl[ijet2];
  if(JJ.M() < mjj_cut) return false;
  double Av_JetEta= 0.5*(JetColl[ijet1].Eta()+ JetColl[ijet2].Eta());
  double zeppenfeld = CalulateMaxZeppenfeld(leps, Av_JetEta,maxDiJetDeta);

  if(zeppenfeld > 0.75) return false;

  return true;
}



