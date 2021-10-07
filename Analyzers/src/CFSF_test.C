#include "CFSF_test.h" 

CFSF_test::CFSF_test(){

}

void CFSF_test::initializeAnalyzer(){

  if(DataYear==2016){
    EleIDs = { 
      //"HEEP_dZ_CF",
      "HNTightV1",
      //"HNMVATight",
    }; // PassID() in Electron.C
    EleIDSFKeys = {
      "",
    }; // histmap.txt
    EleTriggerName = "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v"; // script/PDandTriggers/2016/DoubleEG.txt
    lep0ptcut = 25.;
    lep1ptcut = 15.;
  }
  else if(DataYear==2017){
    EleIDs = {
      //"HEEP_dZ_CF",
      "HNTightV1",
      //"HNMVATight",
    };
    EleIDSFKeys = {
      "",
    };
    EleTriggerName = "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_v";
    lep0ptcut = 25.;
    lep1ptcut = 15.;
  }
  else if(DataYear==2018){
    EleIDs = {
      //"HEEP2018_dZ_CF",
      "HNTightV1",
      //"HNMVATight",
    };
    EleIDSFKeys = {
      "",
    };
    EleTriggerName = "HLT_Ele23_Ele12_CaloIdL_TrackIdL_IsoVL_DZ_v";
    lep0ptcut = 25.;
    lep1ptcut = 15.;
  }

}

CFSF_test::~CFSF_test(){

}

void CFSF_test::executeEvent(Long64_t Nentry){

  AllEles = GetAllElectrons();
  AllMuons = GetAllMuons();
  AllJets = GetAllJets();

  AnalyzerParameter param;

  for(unsigned int i=0; i<EleIDs.size(); i++){

    TString EleID = EleIDs.at(i);
    //TString EleIDSFKey = EleIDSFKeys.at(i);
  
    param.Clear();
  
    param.CFsyst_ = AnalyzerParameter::CF_Central;
  
    param.Name = EleID;

    param.Electron_User_ID = EleID;
 
    if(HasFlag("250")){
      param.Electron_User_ID = EleID+"_250";
    }
    else if(HasFlag("200")){
      param.Electron_User_ID = EleID+"_200";
    } // To check 3charge cut
  
    executeEventFromParameter(param, Nentry);
  
    if(HasFlag("RunSyst")){
  
      for(int i=1; i<AnalyzerParameter::N_CFSyst; i++){
  
      param.CFsyst_ = AnalyzerParameter::CFSyst(i);
      param.Name = EleID+"_"+"Syst_"+param.GetCFSystType();
      executeEventFromParameter(param, Nentry);
  
      }

    }

  }

}

void CFSF_test::executeEventFromParameter(AnalyzerParameter param, Long64_t Nentry){

  Event ev = GetEvent();

  MllLeft = 30;
  MllRight = 150;
  MinPt = 25;
  NBin = 60; //initialize syst. parameters //JH : To test wider fitting range

  if(param.CFsyst_ == AnalyzerParameter::CF_Central){
  
  }
  else if(param.CFsyst_ == AnalyzerParameter::MllRangeUp){
    MllLeft = 55;
    MllRight = 125;
    //NBin = 40;
  }
  else if(param.CFsyst_ == AnalyzerParameter::MllRangeDown){
    MllLeft = 65;
    MllRight = 115;
    //NBin = 20;
  }
  else if(param.CFsyst_ == AnalyzerParameter::MinPtUp){
    MinPt = 28;
  }
  else if(param.CFsyst_ == AnalyzerParameter::MinPtDown){
    MinPt = 22;
  }
  else if(param.CFsyst_ == AnalyzerParameter::NBinUp){
    NBin = 35;
  }
  else if(param.CFsyst_ == AnalyzerParameter::NBinDown){
    NBin = 25;
  }
  else{
    cout << "[ExampleRun::executeEventFromParameter] Wrong syst" << endl;
    exit(EXIT_FAILURE);
  }

  vector<Electron> eles;
  vector<Muon> muons;
  vector<Jet> jets;

  if(!IsDATA){

    /* CF ID selection */

    if(param.Electron_User_ID.Contains("HEEP")) eles = SelectElectrons(AllEles, param.Electron_User_ID, 50., 2.5);
    else eles = SelectElectrons(AllEles, param.Electron_User_ID, 25., 2.5);
    std::sort(eles.begin(), eles.end(), PtComparing);

    vector<Gen> gens = GetGens();
    vector<Electron> eles_prmpt = ElectronPromptOnlyChargeFlip(eles, gens); // Get prompt electrons except conversions
    
    if(HasFlag("CFrate")){

      /* Measure CF rate using MC */

      for(unsigned int i=0; i<eles.size(); i++){
        Gen truth_lep = GetGenMatchedLepton(eles.at(i), gens);
        if(GetLeptonType(eles.at(i), gens)<=0 || GetLeptonType(eles.at(i), gens)>=4) continue; // conversion veto
        if(truth_lep.PID() == 0) continue; 
    
        int truth_lep_Charge;
        if(truth_lep.PID() == 11) truth_lep_Charge = -1;
        else if(truth_lep.PID() == -11) truth_lep_Charge = 1;
      
        //Measure with original method (many 1/pt bins, 3 eta bins)
        if(abs(eles.at(i).scEta())<0.8){
          FillHist(param.Name+"/CFrate/EtaRegion1_Denom", 1./eles.at(i).Pt(), 1., 40, 0., 0.04);
          if(truth_lep_Charge*eles.at(i).Charge()<0){
            //cout << "!!EtaRegion1!! truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
            FillHist(param.Name+"/CFrate/EtaRegion1_Num", 1./eles.at(i).Pt(), 1., 40, 0., 0.04);
            //// SH's ask //
            //if(eles.size() == 2&&i == 0){
            //FillHist(param.Name+"/CFrate/EtaRegion1_IsLeading", 1, 1, 2, 0, 2);
            //}
            //else if(eles.size() == 2&&i == 1){
            //FillHist(param.Name+"/CFrate/EtaRegion1_IsLeading", 0, 1, 2, 0, 2);
            //}
          }
        }
        else if(0.8<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<1.4442){
          FillHist(param.Name+"/CFrate/EtaRegion2_Denom", 1./eles.at(i).Pt(), 1., 40, 0., 0.04);
          if(truth_lep_Charge*eles.at(i).Charge()<0){
            //cout << "!!EtaRegion2!! truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
            FillHist(param.Name+"/CFrate/EtaRegion2_Num", 1./eles.at(i).Pt(), 1., 40, 0., 0.04);
            //// SH's ask //
            //if(eles.size() == 2&&i == 0){
            //FillHist(param.Name+"/CFrate/EtaRegion2_IsLeading", 1, 1, 2, 0, 2);
            //}
            //else if(eles.size() == 2&&i == 1){
            //FillHist(param.Name+"/CFrate/EtaRegion2_IsLeading", 0, 1, 2, 0, 2);
            //}
          }
        }
        else if(1.556<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<2.5){
          FillHist(param.Name+"/CFrate/EtaRegion3_Denom", 1./eles.at(i).Pt(), 1., 40, 0., 0.04);
          if(truth_lep_Charge*eles.at(i).Charge()<0){
            //cout << "!!EtaRegion3!! truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
            FillHist(param.Name+"/CFrate/EtaRegion3_Num", 1./eles.at(i).Pt(), 1., 40, 0., 0.04);
            //// SH's ask //
            //if(eles.size() == 2&&i == 0){
            //FillHist(param.Name+"/CFrate/EtaRegion3_IsLeading", 1, 1, 2, 0, 2);
            //}
            //else if(eles.size() == 2&&i == 1){
            //FillHist(param.Name+"/CFrate/EtaRegion3_IsLeading", 0, 1, 2, 0, 2);
            //}
          }
        }

        //Measure with eta bins
        
        FillHist(param.Name+"/CFrate_Eta/Denom", eles.at(i).scEta(), 1., 50, -2.5, 2.5);
        FillHist(param.Name+"/CFrate_IP/Denom", eles.at(i).dXY(), 1., 200, -0.1, 0.1);
        FillHist(param.Name+"/CFrate_2D/Denom", 1./eles.at(i).Pt(), eles.at(i).scEta(), 1., 40, 0., 0.04, 50, -2.5, 2.5);
        if(truth_lep_Charge*eles.at(i).Charge()<0){
          //cout << "!!EtaRegion3!! truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
          FillHist(param.Name+"/CFrate_Eta/Num", eles.at(i).scEta(), 1., 50, -2.5, 2.5);
          FillHist(param.Name+"/CFrate_IP/Num", eles.at(i).dXY(), 1., 200, -0.1, 0.1);
          FillHist(param.Name+"/CFrate_2D/Num", 1./eles.at(i).Pt(), eles.at(i).scEta(), 1., 40, 0., 0.04, 50, -2.5, 2.5);
        }

      }
    }

    if(HasFlag("CFrateValidation")){

      /* Select events with only two tight same-sign electrons */
      if(eles.size()==2&&eles.at(0).Charge()*eles.at(1).Charge()>0){

        for(int i=0; i<4; i++){

          double dR = (i+1)*0.1;

          Gen truth_lep1 = GetGenMatchedLepton(eles.at(0), gens, dR);
          Gen truth_lep2 = GetGenMatchedLepton(eles.at(1), gens, dR);

          FillHist(param.Name+"/CFrateValidation/Total", dR, 1, 4, 0.1, 0.5);
          if(abs(truth_lep1.PID())==11&&abs(truth_lep2.PID())==11) FillHist(param.Name+"/CFrateValidation/Matched", dR, 1, 4, 0.1, 0.5);
          else if(truth_lep1.PID()==0||truth_lep2.PID()==0) FillHist(param.Name+"/CFrateValidation/Missed", dR, 1, 4, 0.1, 0.5);

        }

      }
    }

    if(HasFlag("ClosureTest")){

      /* MC Closure test start */
     
      if(!PassMETFilter()) return;
      Particle METv = ev.GetMETVector();

      //if(eles.size() == 2){
  
      //  double weight = GetCFweight(eles, param.Electron_User_ID);
  
      //  Particle ZCand = eles.at(0)+eles.at(1);
  
      //  if(75.<=ZCand.M()&&ZCand.M()<105.){
      //    if(eles.at(0).Charge()*eles.at(1).Charge()<0){
      //      FillHist(param.Name+"/ClosureTest/ZMass_OS_CFweighted", ZCand.M(), weight, 30, 75., 105.);
      //    }
      //    else{
      //      FillHist(param.Name+"/ClosureTest/ZMass_SS", ZCand.M(), 1., 30, 75., 105.);
      //      FillHist(param.Name+"/ClosureTest/pt1_SS", eles.at(0).Pt(), 1., 70, 20., 90.);
      //      FillHist(param.Name+"/ClosureTest/pt2_SS", eles.at(1).Pt(), 1., 70, 20., 90.);
      //      FillHist(param.Name+"/ClosureTest/MET_SS", METv.Pt(), 1., 100, 0., 100.);
      //    }
      //  }
      //}
  
      /* Now require only prompt electrons in SS events */

      if(eles_prmpt.size() == 2){

        double weight_prmpt = GetCFweight(eles_prmpt, param.Electron_User_ID, false, 0.);

        Particle ZCand_prmpt = eles_prmpt.at(0)+eles_prmpt.at(1);

        if(75.<=ZCand_prmpt.M()&&ZCand_prmpt.M()<105.){
          if(eles_prmpt.at(0).Charge()*eles_prmpt.at(1).Charge()<0){
            FillHist(param.Name+"/ClosureTest/ZMass_prmpt_OS_CFweighted", ZCand_prmpt.M(), weight_prmpt, 30, 75., 105.);
          }
          else{
            FillHist(param.Name+"/ClosureTest/ZMass_prmpt_SS", ZCand_prmpt.M(), 1., 30, 75., 105.);
            FillHist(param.Name+"/ClosureTest/pt1_prmpt_SS", eles_prmpt.at(0).Pt(), 1., 70, 20., 90.);
            FillHist(param.Name+"/ClosureTest/pt2_prmpt_SS", eles_prmpt.at(1).Pt(), 1., 70, 20., 90.);
            FillHist(param.Name+"/ClosureTest/MET_prmpt_SS", METv.Pt(), 1., 100, 0., 100.);
          }
        }
      }
  
      //
  
      /* There's disagreement between OS_CFweighted and SS(where NO requirement on its promptness?). */
      /* Now, Let's shift the electrons' energy */
      
      if(eles_prmpt.size() != 2) return;
  
      Particle ZCand = eles_prmpt.at(0)+eles_prmpt.at(1);
      Particle ZCand_tmp;
      Particle METv_tmp;
      double weight_tmp;
  
      for(int i=0;i<50;i++){
        vector<Electron> eles_tmp = eles_prmpt; // copy the vector
        for(int j=0;j<2;j++){
          eles_tmp.at(j).SetE(eles_tmp.at(j).E()*(1-0.001*(i+1)));
          eles_tmp.at(j).SetPtEtaPhiE(eles_tmp.at(j).E() * TMath::Sin(eles_tmp.at(j).Theta()), eles_tmp.at(j).Eta(), eles_tmp.at(j).Phi(), eles_tmp.at(j).E());
        }
  
        ZCand_tmp = eles_tmp.at(0) + eles_tmp.at(1);
        METv_tmp.SetPxPyPzE(METv.Px()+ZCand.Px()-ZCand_tmp.Px(),METv.Py()+ZCand.Py()-ZCand_tmp.Py(),0,METv.E()+ZCand.E()-ZCand_tmp.E());
        weight_tmp = GetCFweight(eles_tmp, param.Electron_User_ID, false, 0.);
  
        if(! (75.<=ZCand_tmp.M()&&ZCand_tmp.M()<105.) ) continue;
  
        if(eles_prmpt.at(0).Charge()*eles_prmpt.at(1).Charge()<0){
          FillHist(param.Name+"/ClosureTest/ZMass_OS_CFweighted_shifted_"+TString::Itoa(i+1,10), ZCand_tmp.M(), weight_tmp, 30, 75., 105.);
          FillHist(param.Name+"/ClosureTest/pt1_OS_CFweighted_shifted_"+TString::Itoa(i+1,10), eles_tmp.at(0).Pt(), weight_tmp, 70, 20., 90.);
          FillHist(param.Name+"/ClosureTest/pt2_OS_CFweighted_shifted_"+TString::Itoa(i+1,10), eles_tmp.at(1).Pt(), weight_tmp, 70, 20., 90.);
          FillHist(param.Name+"/ClosureTest/MET_OS_CFweighted_shifted_"+TString::Itoa(i+1,10), METv_tmp.Pt(), weight_tmp, 100, 0., 100.);
        }
      }

    }

    if(HasFlag("HalfSampleTest")){

      /* Start the Half Sample Test : Even -> observed, Odd -> predicted */

      /* Call the needed particles */

      if(!PassMETFilter()) return;
      Particle METv = ev.GetMETVector();
 
      muons = SelectMuons(AllMuons, "POGTight", 20., 2.4); //Just to calculate HT
      jets = SelectJets(AllJets, "tight", 30., 2.4); //Just to calculate HT
      vector<Jet> jets_LeptonVeto = JetsVetoLeptonInside(jets, eles, muons); //To calculate HT

      /* Calculate the needed variables */

      double MET = METv.Pt(); //MET

      double LT = 0;
      for(unsigned int i=0; i<eles.size(); i++){
        LT += eles.at(i).Pt();
      } 
      for(unsigned int i=0; i<muons.size(); i++){
        LT += muons.at(i).Pt();
      }  //LT

      if(muons.size()==0&&eles.size()==0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID", 0, 1, 7, 0, 7);
      else if(muons.size()==0&&eles.size()==1) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID", 1, 1, 7, 0, 7);
      else if(muons.size()==0&&eles.size()==2) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID", 2, 1, 7, 0, 7);
      else if(muons.size()==1&&eles.size()==0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID", 3, 1, 7, 0, 7);
      else if(muons.size()==2&&eles.size()==0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID", 4, 1, 7, 0, 7);
      else if(muons.size()==1&&eles.size()==1) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID", 5, 1, 7, 0, 7);
      else if(muons.size()>0&&eles.size()>0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID", 6, 1, 7, 0, 7); //To check number of leptons

      int fake_ele = 0;
      for(unsigned int i=0; i<eles.size(); i++){
        Gen truth_lep = GetGenMatchedLepton(eles.at(i), gens);
        if(truth_lep.PID() == 0) fake_ele++;
      }
      if(fake_ele==0){
        if(muons.size()==0&&eles.size()==0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID_NoFakeEles", 0, 1, 7, 0, 7);
        else if(muons.size()==0&&eles.size()==1) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID_NoFakeEles", 1, 1, 7, 0, 7);
        else if(muons.size()==0&&eles.size()==2) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID_NoFakeEles", 2, 1, 7, 0, 7);
        else if(muons.size()==1&&eles.size()==0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID_NoFakeEles", 3, 1, 7, 0, 7);
        else if(muons.size()==2&&eles.size()==0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID_NoFakeEles", 4, 1, 7, 0, 7);
        else if(muons.size()==1&&eles.size()==1) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID_NoFakeEles", 5, 1, 7, 0, 7);
        else if(muons.size()>0&&eles.size()>0) FillHist(param.Name+"/HalfSampleTest/lepton_size_PassID_NoFakeEles", 6, 1, 7, 0, 7); 
      } //To check number of leptons

      double HT = 0;
      for(unsigned int i=0; i<jets_LeptonVeto.size(); i++){
        HT += jets_LeptonVeto.at(i).Pt();
      } //HT

      double ST = MET + LT + HT; //
      double METsquaredOverST = pow(MET,2)/ST; //

      if(Nentry%2==0){ //Even : measure CFrate with pT, eta as before

        for(unsigned int i=0; i<eles.size(); i++){
          Gen truth_lep = GetGenMatchedLepton(eles.at(i), gens);
          if(truth_lep.PID() == 0) return; 
      
          int truth_lep_Charge;
          if(truth_lep.PID() == 11) truth_lep_Charge = -1;
          else if(truth_lep.PID() == -11) truth_lep_Charge = 1;
        
          if(abs(eles.at(i).scEta())<0.8){
            FillHist(param.Name+"/HalfSampleTest/Even/EtaRegion1_Denom", 1/eles.at(i).Pt(), 1., 40, 0., 0.04);
            if(truth_lep_Charge*eles.at(i).Charge()<0){
              cout << "!!EtaRegion1!! truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
              FillHist(param.Name+"/HalfSampleTest/Even/EtaRegion1_Num", 1/eles.at(i).Pt(), 1., 40, 0., 0.04);
            }
          }
          else if(0.8<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<1.4442){
            FillHist(param.Name+"/HalfSampleTest/Even/EtaRegion2_Denom", 1/eles.at(i).Pt(), 1., 40, 0., 0.04);
            if(truth_lep_Charge*eles.at(i).Charge()<0){
              cout << "!!EtaRegion2!! truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
              FillHist(param.Name+"/HalfSampleTest/Even/EtaRegion2_Num", 1/eles.at(i).Pt(), 1., 40, 0., 0.04);
            }
          }
          else if(1.556<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<2.5){
            FillHist(param.Name+"/HalfSampleTest/Even/EtaRegion3_Denom", 1/eles.at(i).Pt(), 1., 40, 0., 0.04);
            if(truth_lep_Charge*eles.at(i).Charge()<0){
              cout << "!!EtaRegion3!! truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
              FillHist(param.Name+"/HalfSampleTest/Even/EtaRegion3_Num", 1/eles.at(i).Pt(), 1., 40, 0., 0.04);
            }
          }
        }

      }

      if(Nentry%2==1){ //Odd : measure CFrate with MET, METsquaredOverST. Also apply the weight from the Even set

        for(unsigned int i=0; i<eles.size(); i++){
          Gen truth_lep = GetGenMatchedLepton(eles.at(i), gens);
          if(truth_lep.PID() == 0) return; 
      
          int truth_lep_Charge;
          if(truth_lep.PID() == 11) truth_lep_Charge = -1;
          else if(truth_lep.PID() == -11) truth_lep_Charge = 1;
        
          FillHist(param.Name+"/HalfSampleTest/Odd/MET_Denom", MET, 1., 100, 0., 100.);
          FillHist(param.Name+"/HalfSampleTest/Odd/METsquaredOverST_Denom", METsquaredOverST, 1., 50, 0., 50.);
          if(truth_lep_Charge*eles.at(i).Charge()<0){
            cout << "truth lepton charge : " << truth_lep_Charge << ", reco lepton charge : " << eles.at(i).Charge() << endl;
            FillHist(param.Name+"/HalfSampleTest/Odd/MET_Num", MET, 1., 100, 0., 100.);
            FillHist(param.Name+"/HalfSampleTest/Odd/METsquaredOverST_Num", METsquaredOverST, 1., 50, 0., 50.);
          }

          double weight = GetHalfSampleWeight(eles.at(i), param.Electron_User_ID);

          FillHist(param.Name+"/HalfSampleTest/Odd/MET_weight", MET, weight, 100, 0., 100.);
          FillHist(param.Name+"/HalfSampleTest/Odd/METsquaredOverST_weight", METsquaredOverST, weight, 50, 0., 50.);

        }

      }

    }

  }

  /* Scale Factor measurement */

  if(HasFlag("ScaleFactor")){

    if(!PassMETFilter()) return;
    if(! (ev.PassTrigger(EleTriggerName) )) return;
    
    Particle METv = ev.GetMETVector();
  
    /* CF SF ID selection */
  
    eles = SelectElectrons(AllEles, param.Electron_User_ID, MinPt, 2.5);

    std::sort(eles.begin(), eles.end(), PtComparing);
  
    if(eles.size() != 2) return;
    if(HasFlag("HEM")){
      if( eles.at(0).isHEM()||eles.at(1).isHEM() ) return;
    }
    //if(eles.at(0).Pt()<lep0ptcut||eles.at(1).Pt()<lep1ptcut) return; //No need already pt min = 25

    double MCweight = 1.; // JH : test how DY distribution change with MC weight
    if(!IsDATA && MCSample.Contains("DYJets")){
      MCweight *= ev.MCweight();
      vector<Gen> gens = GetGens();
      for(unsigned int i=0; i<eles.size(); i++){
        if(GetLeptonType(eles.at(i), gens)<=0 || GetLeptonType(eles.at(i), gens)>=4) return; // JH : To extract MC template
      }
    }
  
    Particle ZCand = eles.at(0)+eles.at(1);
    if(! (MllLeft<=ZCand.M()&&ZCand.M()<MllRight) ) return;
  
    // BB
    if(abs(eles.at(0).scEta())<1.4442&&abs(eles.at(1).scEta())<1.4442){
      if(eles.at(0).Charge()*eles.at(1).Charge()>0){
        FillHist(param.Name+"/ScaleFactor/BB_ZMass_SS", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
      }
      if(eles.at(0).Charge()*eles.at(1).Charge()<0){
        FillHist(param.Name+"/ScaleFactor/BB_ZMass_OS", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
      } //JH : if you want to check OS distribution before applying the CF weight
    }
    // BE
    if((abs(eles.at(0).scEta())<1.4442&&abs(eles.at(1).scEta())>=1.556)||(abs(eles.at(0).scEta())>=1.556&&abs(eles.at(1).scEta())<1.4442)){
      if(eles.at(0).Charge()*eles.at(1).Charge()>0){
        FillHist(param.Name+"/ScaleFactor/BE_ZMass_SS", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
      }
      if(eles.at(0).Charge()*eles.at(1).Charge()<0){
        FillHist(param.Name+"/ScaleFactor/BE_ZMass_OS", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
      }
    }
    // EE
    if(abs(eles.at(0).scEta())>=1.556&&abs(eles.at(1).scEta())>=1.556){
      if(eles.at(0).Charge()*eles.at(1).Charge()>0){
        FillHist(param.Name+"/ScaleFactor/EE_ZMass_SS", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
      }
      if(eles.at(0).Charge()*eles.at(1).Charge()<0){
        FillHist(param.Name+"/ScaleFactor/EE_ZMass_OS", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
      }
    }

    if(METv.Pt() < 20){
      // BB
      if(abs(eles.at(0).scEta())<1.4442&&abs(eles.at(1).scEta())<1.4442){
        if(eles.at(0).Charge()*eles.at(1).Charge()>0){
          FillHist(param.Name+"/ScaleFactor/BB_ZMass_SS_MET20", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
        }
        if(eles.at(0).Charge()*eles.at(1).Charge()<0){
          FillHist(param.Name+"/ScaleFactor/BB_ZMass_OS_MET20", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
        }
      }
      // BE
      if((abs(eles.at(0).scEta())<1.4442&&abs(eles.at(1).scEta())>=1.556)||(abs(eles.at(0).scEta())>=1.556&&abs(eles.at(1).scEta())<1.4442)){
        if(eles.at(0).Charge()*eles.at(1).Charge()>0){
          FillHist(param.Name+"/ScaleFactor/BE_ZMass_SS_MET20", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
        }
        if(eles.at(0).Charge()*eles.at(1).Charge()<0){
          FillHist(param.Name+"/ScaleFactor/BE_ZMass_OS_MET20", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
        }
      }
      // EE
      if(abs(eles.at(0).scEta())>=1.556&&abs(eles.at(1).scEta())>=1.556){
        if(eles.at(0).Charge()*eles.at(1).Charge()>0){
          FillHist(param.Name+"/ScaleFactor/EE_ZMass_SS_MET20", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
        }
        if(eles.at(0).Charge()*eles.at(1).Charge()<0){
          FillHist(param.Name+"/ScaleFactor/EE_ZMass_OS_MET20", ZCand.M(), MCweight, NBin, MllLeft, MllRight);
        }
      }
    }

    /* Now let's shift the electrons' energy X% */
    
    vector<Electron> eles_shifted = eles; // copy the vector
    double X;
    if(DataYear==2016){
      if(param.Electron_User_ID=="HNTightV1") X = 1.0;
    }
    else if(DataYear==2017){
      if(param.Electron_User_ID=="HNTightV1") X = 1.0;
    }
    else if(DataYear==2018){
      if(param.Electron_User_ID=="HNTightV1") X = 0.8;
    }
    TString X_string = Form("%f",X);
    X_string = X_string(0,3)+"%";
    
    for(int j=0;j<2;j++){
      eles_shifted.at(j).SetE(eles_shifted.at(j).E()*(1-X/100));
      eles_shifted.at(j).SetPtEtaPhiE(eles_shifted.at(j).E() * TMath::Sin(eles_shifted.at(j).Theta()), eles_shifted.at(j).Eta(), eles_shifted.at(j).Phi(), eles_shifted.at(j).E());
    }
  
    Particle ZCand_shifted = eles_shifted.at(0) + eles_shifted.at(1);
    double weight_shifted = GetCFweight(eles_shifted, param.Electron_User_ID, false, 0.);
    double weight_shifted_SF = GetCFweight(eles_shifted, param.Electron_User_ID, true, 0.);
    weight_shifted *= MCweight;
    weight_shifted_SF *= MCweight;
  
    Particle METv_shifted;
    METv_shifted.SetPxPyPzE(METv.Px()+ZCand.Px()-ZCand_shifted.Px(),METv.Py()+ZCand.Py()-ZCand_shifted.Py(),0,METv.E()+ZCand.E()-ZCand_shifted.E());
  
    if(MllLeft<=ZCand_shifted.M()&&ZCand_shifted.M()<MllRight){
      if(eles.at(0).Charge()*eles.at(1).Charge()<0){
  
        // BB
        if(abs(eles_shifted.at(0).scEta())<1.4442&&abs(eles_shifted.at(1).scEta())<1.4442){
          FillHist(param.Name+"/ScaleFactor/BB_ZMass_OS_CFweighted_shifted_"+X_string, ZCand_shifted.M(), weight_shifted, NBin, MllLeft, MllRight);
          //FillHist(param.Name+"/ScaleFactor/BB_ZMass_OS_CFSFweighted_shifted_"+X_string, ZCand_shifted.M(), weight_shifted_SF, NBin, MllLeft, MllRight);
        }
    
        // BE
        if((abs(eles_shifted.at(0).scEta())<1.4442&&abs(eles_shifted.at(1).scEta())>=1.556)||(abs(eles_shifted.at(0).scEta())>=1.556&&abs(eles_shifted.at(1).scEta())<1.4442)){
          FillHist(param.Name+"/ScaleFactor/BE_ZMass_OS_CFweighted_shifted_"+X_string, ZCand_shifted.M(), weight_shifted, NBin, MllLeft, MllRight);
          FillHist(param.Name+"/ScaleFactor/BE_ZMass_OS_CFSFweighted_shifted_"+X_string, ZCand_shifted.M(), weight_shifted_SF, NBin, MllLeft, MllRight);
        }
    
        // EE
        if(abs(eles_shifted.at(0).scEta())>=1.556&&abs(eles_shifted.at(1).scEta())>=1.556){
          FillHist(param.Name+"/ScaleFactor/EE_ZMass_OS_CFweighted_shifted_"+X_string, ZCand_shifted.M(), weight_shifted, NBin, MllLeft, MllRight);
          //FillHist(param.Name+"/ScaleFactor/EE_ZMass_OS_CFSFweighted_shifted_"+X_string, ZCand_shifted.M(), weight_shifted_SF, NBin, MllLeft, MllRight);
        }
        
      }
    }
  }

}

/*
double CFSF_test::GetCFweight(std::vector<Electron> eles, TString id){
  
  double prob[2];

  if(id == "HEID"){

    //DY+TTLL CF

    for(int i=0;i<2;i++){ 

      if(abs(eles.at(i).scEta())<0.8){
        if(1/eles.at(i).Pt()<0.0075) prob[i] = 4.59433e-04-4.41286e-02/eles.at(i).Pt();
        else if(0.0075<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 2.01545e-04-8.19670e-03/eles.at(i).Pt();
        else prob[i] = 8.74309e-05-5.72703e-04/eles.at(i).Pt();
      }
      else if(0.8<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<1.4442){
        if(1/eles.at(i).Pt()<0.0055) prob[i] = 3.81412e-03-4.74900e-01/eles.at(i).Pt();
        else if(0.0055<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 1.60397e-03-6.97824e-02/eles.at(i).Pt();
        else prob[i] = 6.59614e-04-7.13587e-03/eles.at(i).Pt();
      }
      else if(1.556<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<2.5){
        if(1/eles.at(i).Pt()<0.0105) prob[i] = 1.23549e-02-6.80049e-01/eles.at(i).Pt();
        else if(0.0105<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0205) prob[i] = 7.27935e-03-1.88277e-01/eles.at(i).Pt();
        else prob[i] = 4.06341e-03-3.22562e-02/eles.at(i).Pt();
      }

    }

    //
  
    return prob[0]/(1.-prob[0])+prob[1]/(1.-prob[1]);

  }

  else if(id == "HNTightV2"){

    //DY+TTLL CF

    for(int i=0;i<2;i++){ 

      if(abs(eles.at(i).scEta())<0.8){
        if(1/eles.at(i).Pt()<0.0075) prob[i] = 2.91353e-04-2.96481e-02/eles.at(i).Pt();
        else if(0.0075<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 1.25648e-04-6.16739e-03/eles.at(i).Pt();
        else prob[i] = 3.75795e-05-7.16920e-04/eles.at(i).Pt();
      }
      else if(0.8<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<1.4442){
        if(1/eles.at(i).Pt()<0.0055) prob[i] = 2.93283e-03-3.85095e-01/eles.at(i).Pt();
        else if(0.0055<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 1.01466e-03-4.96765e-02/eles.at(i).Pt();
        else prob[i] = 3.43620e-04-8.40559e-03/eles.at(i).Pt();
      }
      else if(1.556<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<2.5){
        if(1/eles.at(i).Pt()<0.0105) prob[i] = 9.20799e-03-6.60680e-01/eles.at(i).Pt();
        else if(0.0105<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0205) prob[i] = 4.09673e-03-1.64067e-01/eles.at(i).Pt();
        else prob[i] = 1.35968e-03-2.70099e-02/eles.at(i).Pt();
      }

    }

    //
  
    return prob[0]/(1.-prob[0])+prob[1]/(1.-prob[1]);

  }

  else if(id == "HNTight2016"){

    //DY+TTLL CF

    for(int i=0;i<2;i++){

      if(abs(eles.at(i).scEta())<0.8){
        if(1/eles.at(i).Pt()<0.0075) prob[i] = 3.92360e-04-4.03780e-02/eles.at(i).Pt();
        else if(0.0075<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 1.44985e-04-6.52879e-03/eles.at(i).Pt();
        else prob[i] = 6.59979e-05-1.32125e-03/eles.at(i).Pt();
      }
      else if(0.8<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<1.4442){
        if(1/eles.at(i).Pt()<0.0055) prob[i] = 2.61602e-03-3.16041e-01/eles.at(i).Pt();
        else if(0.0055<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 1.09783e-03-5.16251e-02/eles.at(i).Pt();
        else prob[i] = 3.85519e-04-8.10426e-03/eles.at(i).Pt();
      }
      else if(1.556<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<2.5){
        if(1/eles.at(i).Pt()<0.0105) prob[i] = 9.63989e-03-7.02277e-01/eles.at(i).Pt();
        else if(0.0105<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0205) prob[i] = 4.38753e-03-1.75528e-01/eles.at(i).Pt();
        else prob[i] = 1.60630e-03-3.15704e-02/eles.at(i).Pt();
      }

    }

    //
  
    return prob[0]/(1.-prob[0])+prob[1]/(1.-prob[1]);

  }

}

double CFSF_test::GetCFweight_SF(std::vector<Electron> eles, TString id){
  
  double prob[2];
  double SF_BB, SF_EE;

  if(id == "HEID"){

    //DY+TTLL CF
    
    SF_BB = 0.585841;
    SF_EE = 0.831539;

    for(int i=0;i<2;i++){

      if(abs(eles.at(i).scEta())<0.8){
        if(1/eles.at(i).Pt()<0.0075) prob[i] = (4.59433e-04-4.41286e-02/eles.at(i).Pt())*SF_BB;
        else if(0.0075<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = (2.01545e-04-8.19670e-03/eles.at(i).Pt())*SF_BB;
        else prob[i] = (8.74309e-05-5.72703e-04/eles.at(i).Pt())*SF_BB;
      }
      else if(0.8<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<1.4442){
        if(1/eles.at(i).Pt()<0.0055) prob[i] = (3.81412e-03-4.74900e-01/eles.at(i).Pt())*SF_BB;
        else if(0.0055<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = (1.60397e-03-6.97824e-02/eles.at(i).Pt())*SF_BB;
        else prob[i] = (6.59614e-04-7.13587e-03/eles.at(i).Pt())*SF_BB;
      }
      else if(1.556<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<2.5){
        if(1/eles.at(i).Pt()<0.0105) prob[i] = (1.23549e-02-6.80049e-01/eles.at(i).Pt())*SF_EE;
        else if(0.0105<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0205) prob[i] = (7.27935e-03-1.88277e-01/eles.at(i).Pt())*SF_EE;
        else prob[i] = (4.06341e-03-3.22562e-02/eles.at(i).Pt())*SF_EE;
      }

    }

    //

    return prob[0]/(1.-prob[0])+prob[1]/(1.-prob[1]);

  }

  else if(id == "HNTight2016"){

    //DY+TTLL CF

    SF_BB = 0.585841;
    SF_EE = 0.831539;

    for(int i=0;i<2;i++){

      if(abs(eles.at(i).scEta())<0.8){
        if(1/eles.at(i).Pt()<0.0075) prob[i] = 3.92360e-04-4.03780e-02/eles.at(i).Pt();
        else if(0.0075<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 1.44985e-04-6.52879e-03/eles.at(i).Pt();
        else prob[i] = 6.59979e-05-1.32125e-03/eles.at(i).Pt();
      }
      else if(0.8<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<1.4442){
        if(1/eles.at(i).Pt()<0.0055) prob[i] = 2.61602e-03-3.16041e-01/eles.at(i).Pt();
        else if(0.0055<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0155) prob[i] = 1.09783e-03-5.16251e-02/eles.at(i).Pt();
        else prob[i] = 3.85519e-04-8.10426e-03/eles.at(i).Pt();
      }
      else if(1.556<=abs(eles.at(i).scEta())&&abs(eles.at(i).scEta())<2.5){
        if(1/eles.at(i).Pt()<0.0105) prob[i] = 9.63989e-03-7.02277e-01/eles.at(i).Pt();
        else if(0.0105<=1/eles.at(i).Pt()&&1/eles.at(i).Pt()<0.0205) prob[i] = 4.38753e-03-1.75528e-01/eles.at(i).Pt();
        else prob[i] = 1.60630e-03-3.15704e-02/eles.at(i).Pt();
      }

    }

    //

    return prob[0]/(1.-prob[0])+prob[1]/(1.-prob[1]);

  }

}

double CFSF_test::GetHalfSampleWeight(const Electron& electron, TString id){

  if(id == "HEID"){

    //DY+TTLL

    if(abs(electron.scEta())<0.8){
      if(1/electron.Pt()<0.005) return 9.58505e-04-1.59759e-01/electron.Pt();
      else if(0.005<=1/electron.Pt()&&1/electron.Pt()<0.0155) return 2.07325e-04-8.63460e-03/electron.Pt();
      else return 9.37334e-05-9.81244e-04/electron.Pt();
    }
    else if(0.8<=abs(electron.scEta())&&abs(electron.scEta())<1.4442){
      if(1/electron.Pt()<0.0055) return 3.82245e-03-4.55401e-01/electron.Pt();
      else if(0.0055<=1/electron.Pt()&&1/electron.Pt()<0.0155) return 1.65421e-03-7.32609e-02/electron.Pt();
      else return 6.57784e-04-6.53361e-03/electron.Pt();
    }
    else if(1.556<=abs(electron.scEta())&&abs(electron.scEta())<2.5){
      if(1/electron.Pt()<0.01) return 1.27778e-02-7.44197e-01/electron.Pt();
      else if(0.01<=1/electron.Pt()&&1/electron.Pt()<0.0205) return 7.25863e-03-1.88640e-01/electron.Pt();
      else return 4.17112e-03-3.71866e-02/electron.Pt();
    }

    //

  }

  else if(id == "HNTight2016"){

    //DY+TTLL

    if(abs(electron.scEta())<0.8){
      if(1/electron.Pt()<0.0075) return 4.12045e-04-4.45416e-02/electron.Pt();
      else if(0.0075<=1/electron.Pt()&&1/electron.Pt()<0.0155) return 1.18690e-04-4.48807e-03/electron.Pt();
      else return 7.28590e-05-1.58132e-03/electron.Pt();
    }
    else if(0.8<=abs(electron.scEta())&&abs(electron.scEta())<1.4442){
      if(1/electron.Pt()<0.0055) return 3.20894e-03-4.20695e-01/electron.Pt();
      else if(0.0055<=1/electron.Pt()&&1/electron.Pt()<0.0155) return 1.10226e-03-5.22899e-02/electron.Pt();
      else return 3.83228e-04-8.13698e-03/electron.Pt();
    }
    else if(1.556<=abs(electron.scEta())&&abs(electron.scEta())<2.5){
      if(1/electron.Pt()<0.01) return 1.01037e-02-7.46975e-01/electron.Pt();
      else if(0.01<=1/electron.Pt()&&1/electron.Pt()<0.019) return 4.40033e-03-1.78011e-01/electron.Pt();
      else return 1.59123e-03-3.18552e-02/electron.Pt();
    }

    //

  }

}
*/
