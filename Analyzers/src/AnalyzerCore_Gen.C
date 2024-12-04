#include "AnalyzerCore.h"


//=========================================================                                  //==== Gen Matching Tools                                                                                             

bool AnalyzerCore::HasMEPhoton(Lepton lep){
  ////https://github.com/GhentAnalysis/heavyNeutrino/blob/cd101501001e9776b324002da585e38fe0a378d4/multilep/src/GenTools.cc
  ///https://github.com/GhentAnalysis/heavyNeutrino/blob/cd101501001e9776b324002da585e38fe0a378d4/multilep/src/LeptonAnalyzer.cc#L530C45-L530C55

  int Idx_Closest    = GenMatchedIdx(lep,All_Gens);
  
  //// If Idx_Closest > 0 then status 1 lepton is matched and not photon
  if(Idx_Closest > 0) return false;

  float minDr=0.2;
  unsigned int closest_index = 0;

  for(int i=2; i<int(All_Gens.size()); i++){
    
    Gen gen = All_Gens.at(i);
    if(lep.DeltaR(gen) < minDr)  {
      if(gen.Status()==1){
	minDr = lep.DeltaR(gen) ;
	closest_index = i;
      }
    }
  }
  
  if(closest_index == 0) return false;
  if(All_Gens[closest_index].PID() ==22 && GenIsPrompt(All_Gens[closest_index])) return true;
  return false;
  
}
bool AnalyzerCore::GenIsPrompt(Gen gen){
  
  if(gen.IsEmpty()) return false;
  if(gen.MotherIndex() < 0)  return false;
  if(gen.MotherIndex() > int(All_Gens.size()))  return false;
  
  Gen mom = GenGetMother(gen);
  if(mom.IsEmpty()) return false;

  if(abs(mom.PID()) == 15 && mom.isPromptDecayed()) return true;
  return (gen.isPromptFinalState() || gen.isPromptDecayed());
}


Gen  AnalyzerCore::GenGetMother(Gen gen){
  
  Gen pmoth;

  if(gen.IsEmpty()) return pmoth;
  //// In case mother index is not found return empty gen
  if(gen.MotherIndex()  < 2) return pmoth;
  if(gen.MotherIndex()  > int(All_Gens.size())  ) return pmoth;
  
  pmoth = All_Gens[gen.MotherIndex()];

  if(gen.PID() == pmoth.PID()) return GenGetMother(pmoth); 
  else return pmoth;
}
                                                                    

int AnalyzerCore::HotFixLeptonType(Lepton lep){
  ///// Fix some truth matching for samples that dont work with Jiwhans function
  if(MCSample.Contains("MiNNLO")){

    /// Internal conversions in these samples set as type1
    //// A hot fix  is to check number of status 1/3 leptons from matched mother.
    //// .... All 3/4 lepton are int conversion in check 

    if(lep.LeptonGenType() != 1) return -999;

    vector<int> matched_lep_check;
    int Idx_Closest    = GenMatchedIdx(lep,All_Gens);
    int MatchPID = (lep.IsMuon()) ? 13 : 11;

    if(Idx_Closest < 0) return -999;
    if(!(fabs(All_Gens[Idx_Closest].PID()) ==MatchPID)) return -999;

    int motherindex = All_Gens[Idx_Closest].MotherIndex();
    for(int i=2; i< int(All_Gens.size()); i++){
      if(motherindex == All_Gens[i].MotherIndex()){
	if(All_Gens[i].Status() == 1 && ((fabs(All_Gens[i].PID()) == 11) || (fabs(All_Gens[i].PID()) == 13 ) || (fabs(All_Gens[i].PID()) == 15 ))){
	  matched_lep_check.push_back(i);
	  if(i == Idx_Closest){
	    if(matched_lep_check.size() > 2) return 5;
	  }
	}
      }
    }

    return -999;
  }
  if(MCSample.Contains("Sherpa")){
    
    bool LepPrompt=false;
    for(unsigned int i=2; i<All_Gens.size(); i++){
      Gen gen = All_Gens.at(i);
      
      if(gen.Status() != 1) continue;
      int mindex = All_Gens.at(i).MotherIndex();
      int MotherPID = fabs(All_Gens.at(mindex).PID());
      bool PromptLepMu = (MotherPID == 13)  || (MotherPID == 15);
      bool PromptLepEl = (MotherPID == 11) || (MotherPID == 15);
      
      while (MotherPID > 10 && MotherPID < 16){
	mindex = All_Gens.at(mindex).MotherIndex();
	MotherPID= fabs(All_Gens.at(mindex).PID());
      }
      
      bool PromptLepMuFull = PromptLepMu &&  ((MotherPID == 21)  || (MotherPID < 6));
      bool PromptLepElFull = PromptLepEl &&  ((MotherPID == 21)  || (MotherPID < 6));
      
      if(fabs(gen.PID()) == 13){
	if(PromptLepMuFull && lep.LeptonFlavour() == Lepton::MUON && lep.DeltaR(gen) < 0.1) LepPrompt=true;
      }
      if(fabs(gen.PID()) == 11){
	if(PromptLepElFull &&lep.LeptonFlavour() != Lepton::MUON && lep.DeltaR(gen) < 0.1) LepPrompt=true;
      }
    }
    if (LepPrompt) return 1;
    else return -1;
    
  }
  
  return -999;
}                         

void AnalyzerCore::PrintGen(const std::vector<Gen>& gens){

  cout << "===========================================================" << endl;
  cout << "RunNumber:EventNumber = " << run << ":" << event << endl;
  cout << "index\tPID\tStatus\tMIdx\tMPID\tStart\tPt\tEta\tPhi\tM" << endl;
  for(unsigned int i=2; i<gens.size(); i++){
    Gen gen = gens.at(i);
    if(!MCSample.Contains("Sherpa")){
      vector<int> history = TrackGenSelfHistory(gen, gens);
      cout << i << "\t" << gen.PID() << "\t" << gen.Status() << "\t" << gen.MotherIndex() << "\t" << gens.at(gen.MotherIndex()).PID()<< "\t" << history[0] << "\t";
    }
    else     cout << i << "\t" << gen.PID() << "\t" << gen.Status() << "\t" << gen.MotherIndex() << "\t"  ;

    printf("%.2f\t%.2f\t%.2f\t%.2f\n",gen.Pt(), gen.Eta(), gen.Phi(), gen.M());
  }

}

bool AnalyzerCore::GenTypeMatched(TString gen_string){

  vector<TString> lables =GetGenListPlot();
  if (std::find(lables.begin(), lables.end(), gen_string) == lables.end()) return false;

  return true;
}


vector<TString> AnalyzerCore::GetGenListPlot(){

  vector<TString> lables = {"electron","muon","pi0","pi+", "ph","tau","q",
			    "c","b","t","g","K","K*","K+","D","D0","D+",
			    "B","B0","B+","D0_K+","B0_K+","LightMeson",
			    "ccMeson","bbMeson","CMeson", "BMeson","CBaryon",
			    "BBaryon", "proton", "conv", "__"};
                            

  return lables;

}


vector<TString> AnalyzerCore::GetGenList(){

  vector<TString> lables = {"electron", "muon", "pi0ph", "pi0", "pi+", "ph",
			    "tau", "q", "c", "b", "t", "g", "JPsi", "K", "K*",
			    "K+", "D", "D0", "D+", "B", "B0", "B+", "D0_K+", "B0_K+",
			    "LightMeson", "CMeson", "ccMeson", "BMeson", "bbMeson",
			    "CBaryon", "BBaryon", "proton", "conv", "__"};
  
  return lables;

}

TString AnalyzerCore::MatchGenPID(int PID, vector<Gen> gens,  Gen gen){

  if(PID == 13 && gen.Status()==1 && (fabs(gens.at(gen.MotherIndex()).PID()) == 24)) return "muon";
  if(PID == 11 && gen.Status()==1 && (fabs(gens.at(gen.MotherIndex()).PID()) == 24)) return "electron";
  if(PID == 22 && gen.Status()==1 && (fabs(gens.at(gen.MotherIndex()).PID()) == 111)) return "pi0ph";
  if(PID == 321 && gen.Status()==1 && (fabs(gens.at(gen.MotherIndex()).PID()) == 421)) {
    if(fabs( gens.at(gens.at(gen.MotherIndex()).MotherIndex()).PID()) == 413)     return "D";
    if(fabs( gens.at(gens.at(gen.MotherIndex()).MotherIndex()).PID()) == 423)     return "D";
    return "D0";
  }
  if(PID == 321 && gen.Status()==1 && (fabs(gens.at(gen.MotherIndex()).PID()) == 511)) {
    return "B0";
  }
  int MID = fabs(gens.at(gen.MotherIndex()).PID());

  if(PID == 511 )  return "B0";
  if(PID  == 531 ) return "B";
  if(PID  == 533 ) return "B";
  if(PID  == 521 ) return "B+";
  if(PID > 5000  && PID < 6000 ) return "BBaryon";
  if(PID > 4000  && PID < 5000 ) return "CBaryon";

  if(MID == 511 ) return "B0";
  if(MID  == 531 ) return "B";
  if(MID  == 533 ) return "B";
  if(MID  == 521 ) return "B+";
  if(MID  == 431 ) return "D";
  if(MID  == 433 ) return "D";
  if(MID > 5000  && MID < 6000 ) return "BBaryon";
  if(MID > 4000  && MID < 5000 ) return "CBaryon";

  return "";
}


TString AnalyzerCore::MatchGenDef(std::vector<Gen>& gens,const Lepton& Lep, bool DEBUG){

  if(DEBUG) {cout << "Lepton type = " << Lep.GetFlavour() << endl;}
  int Idx_Closest    = GenMatchedIdx(Lep,gens);
  if(DEBUG) {cout << "Idx_Closest = " << Idx_Closest << endl;}
  if (Idx_Closest < 0){
    /// Check All particles since no matched status 1 lepton is found                                                                                                                                                
    double mindR=0.4;
    int closest_pid=9999;

    vector<TString> MatchedGens;
    vector<int> MatchedGensID;
    vector<int> CloseGensID;
    int closest_id=-1;
    for(unsigned int i=2; i<gens.size(); i++){
      Gen gen = gens.at(i);
      if(gen.Pt() < 5) continue;
      if(fabs(gen.PID()) == 23) continue;
      if(fabs(gen.PID()) == 24) continue;
      if(fabs(gen.PID()) == 12) continue;
      if(fabs(gen.PID()) == 14) continue;
      if(fabs(gen.PID()) == 16) continue;
      if(fabs(gen.PID()) == 6) continue;

      if(Lep.DeltaR(gen) < 0.4)  {
        if(fabs(gen.Pt() - Lep.Pt() ) / Lep.Pt() < 0.25) {

          if(MatchGenPID(fabs(gen.PID()), gens, gen) != "") {
            return MatchGenPID(fabs(gen.PID()),gens,  gen);
          }
          MatchedGensID.push_back(i);
        }
	CloseGensID.push_back(i);
      }
      if(Lep.DeltaR(gen) < mindR ){
        mindR=Lep.DeltaR(gen) ;
        closest_pid= fabs(gen.PID());
        closest_id = i;
      }
    }
    mindR=0.4;
    TString pid_matched="NULL";
    for(auto j : MatchedGensID) {
      if(Lep.DeltaR(gens[j]) < mindR){
        mindR=Lep.DeltaR(gens[j]);
        if(fabs(gens[j].Pt() - Lep.Pt() ) / Lep.Pt() < 0.25) {
          pid_matched=Lep.PIDToString(fabs(gens[j].PID()));
	  int mind = gens.at(j).MotherIndex();
          int pdid = j;
          while(std::find(CloseGensID.begin(), CloseGensID.end(), mind) != CloseGensID.end()) {
            if(fabs(gens.at(mind).PID()) <= 6)  break;
            if(fabs(gens.at(mind).PID()) == 2212)  break;
	    pdid = mind;
            mind = gens.at(mind).MotherIndex();
          }
          if(fabs(gens[pdid].PID()) == 423) return "D";
          if(fabs(gens[pdid].PID()) == 421) return "D0";
          if(fabs(gens[pdid].PID()) == 431) return "D";
          if(fabs(gens[pdid].PID()) == 433) return "D";
          if(fabs(gens[pdid].PID()) == 413) return "D";
          if(fabs(gens[pdid].PID()) == 411) return "D+";
          if(fabs(gens[pdid].PID()) == 511) return "B0";
          if(fabs(gens[pdid].PID()) == 531) return "B";
          if(fabs(gens[pdid].PID()) == 533) return "B";
          if(fabs(gens[pdid].PID()) == 513) return "B";
          if(fabs(gens[pdid].PID()) == 521) return "B+";
          if(fabs(gens[pdid].PID()) == 523) return "B";
          if(fabs(gens[mind].PID()) == 423) return "D";
          if(fabs(gens[mind].PID()) == 421) return "D0";
          if(fabs(gens[mind].PID()) == 431) return "D";
          if(fabs(gens[mind].PID()) == 433) return "D";
          if(fabs(gens[mind].PID()) == 413) return "D";
          if(fabs(gens[mind].PID()) == 411) return "D+";
          if(fabs(gens[mind].PID()) == 511) return "B0";
          if(fabs(gens[mind].PID()) == 531) return "B";
          if(fabs(gens[mind].PID()) == 533) return "B";
          if(fabs(gens[mind].PID()) == 513) return "B";
          if(fabs(gens[mind].PID()) == 521) return "B+";
          if(fabs(gens[mind].PID()) == 523) return "B";
        }
      }
    }

    if(closest_pid == 11){
      vector<int> history = TrackGenSelfHistory(gens[closest_id], gens);
      if(fabs(gens.at(history[1]).PID()) == 23) return "electron";
      if(fabs(gens.at(history[1]).PID()) == 15) return "tau";
    }
    if(closest_pid == 13){
      vector<int> history = TrackGenSelfHistory(gens[closest_id], gens);
      if(fabs(gens.at(history[1]).PID()) == 23) return "muon";
      if(fabs(gens.at(history[1]).PID()) == 15) return "tau";
    }

    if(pid_matched!= "NULL")return pid_matched;
    if(closest_pid < 4) return "q";
    if(closest_pid == 4) return "c";
    if(closest_pid == 5) return "b";
    if(closest_pid == 11) return "electron";
    if(closest_pid == 13) return "muon";
    if(closest_pid == 22) return "ph";
    if(closest_pid == 21) return "g";
    if(closest_pid == 211)return "pi+";
    if(closest_pid == 111) return "pi0";
    if(closest_pid == 443) return "JPsi";
    if(closest_pid == 411) return "D+";
    if(closest_pid == 321) return "K+";
    if(closest_pid == 313) return "K*";
    if(closest_pid == 323) return "K*";
    if(closest_pid == 130) return "K";
    if(closest_pid == 310) return "K";
    if(closest_pid == 311) return "K";
    if(closest_pid == 10313) return "K";
    if(closest_pid == 223) return "LightMeson";
    if(closest_pid == 411) return "D+";
    if(closest_pid == 421) return "D0";
    if(closest_pid == 511) return "B0";
    if(closest_pid == 521) return "B+";
    if(closest_pid == 513) return "B";
    if(closest_pid == 523) return "B";
    if(closest_pid == 531) return "B";
    if(closest_pid == 533) return "B";

    int pdid2=-1;
    while(std::find(CloseGensID.begin(), CloseGensID.end(), closest_id) != CloseGensID.end()) {
      if(fabs(gens.at(closest_id).PID()) <= 6)  break;
      pdid2 = closest_id;
      closest_id = gens.at(closest_id).MotherIndex();
    }

    if(pdid2 > 0 )return Lep.PIDToString(gens.at(pdid2).PID());
    if(closest_pid != 9999 )return Lep.PIDToString(closest_pid) ;
  }
  else{
    if(DEBUG) {cout << "MATCHED CLOSTEST  = fabs(gens.at(gens[Idx_Closest].MotherIndex()).PID()) " << fabs(gens.at(gens[Idx_Closest].MotherIndex()).PID())  <<endl;}

    vector<int> history; //= TrackGenSelfHistory(gens[Idx_Closest], gens);                                                                                                                                       
    int motherindex = gens[Idx_Closest].MotherIndex();
    history.push_back(Idx_Closest);
    while(motherindex > 1){
      history.push_back(motherindex);
      motherindex = gens[motherindex].MotherIndex();
    }

    for(auto ig : history){
      if(fabs(gens.at(ig).PID()) == 521)return "B+";
      if(fabs(gens.at(ig).PID()) == 511)return "B0";
      if(fabs(gens.at(ig).PID()) == 421)return "D0";
      if(fabs(gens.at(ig).PID()) == 411)return "D+";
    }
    for(auto ig : history){
      if(Lep.LeptonFlavour()==Lepton::MUON && Lep.PIDToString(gens.at(ig).PID()) == "muon") continue;
      if(Lep.LeptonFlavour()==Lepton::ELECTRON && Lep.PIDToString(gens.at(ig).PID()) == "electron") continue;

      if(Lep.PIDToString(gens.at(ig).PID()) != "NULL") {
        if(Lep.PIDToString(gens.at(ig).PID()) == "D")  return Lep.PIDToString(gens.at(ig).PID());
        if(Lep.PIDToString(gens.at(ig).PID()) == "B")  return Lep.PIDToString(gens.at(ig).PID());
      }
    }

    for(auto ig : history){
      if(Lep.LeptonFlavour()==Lepton::MUON && Lep.PIDToString(gens.at(ig).PID()) == "muon") continue;
      if(Lep.LeptonFlavour()==Lepton::ELECTRON && Lep.PIDToString(gens.at(ig).PID()) == "electron") continue;
      if(Lep.PIDToString(gens.at(ig).PID()) != "NULL") return Lep.PIDToString(gens.at(ig).PID());
    }
    int mother_id = gens[Idx_Closest].MotherIndex();
    while(fabs(gens.at(mother_id).PID()) == 13)  mother_id = gens[mother_id].MotherIndex();
    while(fabs(gens.at(mother_id).PID()) == 11)  mother_id = gens[mother_id].MotherIndex();

    if(fabs(gens.at(mother_id).PID()) == 2212 ) {

      for(unsigned int i=2; i<gens.size(); i++){
        Gen gen = gens.at(i);
        if(Lep.DeltaR(gen) < 0.4)  {
          if(fabs(gen.PID()) == 22 && gen.Status()==1 && (fabs(gens.at(gen.MotherIndex()).PID()) == 111)) return "pi0ph";
          if((fabs(gens.at(gen.MotherIndex()).PID()) == 111)) return "pi0";
          if(fabs(gen.PID()) == 22 ) return "conv";

        }
      }
      return "proton";
    }

    if(fabs(gens.at(mother_id).PID()) == 2212)return "proton";
    if(fabs(gens.at(mother_id).PID()) == 22)  return "ph";
    if(fabs(gens.at(mother_id).PID()) == 130) return "K0L";
    if(fabs(gens.at(mother_id).PID()) == 443) return "JPsi";
    if(fabs(gens.at(mother_id).PID()) == 15)  return "tau";
    if(fabs(gens[Idx_Closest].PID()) != 11){
      if(fabs(gens.at(mother_id).PID()) == 11 )return "electron";
    }
    if(fabs(gens[Idx_Closest].PID()) !=13){
      if(fabs(gens.at(mother_id).PID()) == 13 )return "muon";
    }
  }

  return "__";
}




void AnalyzerCore::PrintMatchedGen(std::vector<Gen>& gens,const Lepton& Lep){

  cout << "===========================================================" << endl;
  cout << "RunNumber:EventNumber = " << run << ":" << event << endl;
  cout << "index\tPID\tStatus\tMIdx\tMPID\tStart\tPt\tEta\tPhi\tM" << endl;

  unsigned int Idx_Closest    = GenMatchedIdx(Lep,gens);
  cout << "Matched gen = " << Idx_Closest << endl;
  cout << Lep.PrintInfo() << endl;

  float minDr=1.;
  unsigned int closest_index = 0;
  for(unsigned int i=2; i<gens.size(); i++){

    Gen gen = gens.at(i);
    if(Lep.DeltaR(gen) < minDr)  {
      minDr = Lep.DeltaR(gen) ;
      closest_index = i;
    }
  }

  for(unsigned int i=2; i<gens.size(); i++){

    Gen gen = gens.at(i);

    TString addon="";
    if(closest_index == i) addon = " ----> ";
    else addon="";
    if (Idx_Closest == i)  addon += " ----> ";

    if(MCSample.Contains("Sherpa")){
      if(Lep.DeltaR(gen) < 0.4)  {
        cout <<  i << "\t" << gen.SPID() << "\t" << gen.Status() << "\t" << gen.MotherIndex() << "\t";
        printf("%.2f\t%.2f\t%.2f\t%.2f =======> DrMatched %.2f\t "  +addon+ "\n",gen.Pt(), gen.Eta(), gen.Phi(), gen.M(),Lep.DeltaR(gen));
	
      }
    }
    else{
      if(Lep.DeltaR(gen) < 0.4)  {
	vector<int> history = TrackGenSelfHistory(gen, gens);
	cout <<  i << "\t" << gen.SPID() << "\t" << gen.Status() << "\t" << gen.MotherIndex() << "\t" << gens.at(gen.MotherIndex()).SPID()<< "\t" << history[0] << "\t";
	printf("%.2f\t%.2f\t%.2f\t%.2f =======> DrMatched %.2f\t "  +addon+ "\n",gen.Pt(), gen.Eta(), gen.Phi(), gen.M(),Lep.DeltaR(gen));
	
      }
    }
  }
  return;
}


bool AnalyzerCore::HasPromptConv(Electron el){
  
  if(All_Gens.size() == 0) return false;

  ///// Version 1
  
  int TruthIdx  = GenMatchedIdx(el, All_Gens);;
  if(TruthIdx < 0) return false;


  int nSt1el(0);
  for(unsigned int i=2; i < All_Gens.size(); i++){
    if(TruthIdx == int(i)) continue;
    if(TruthIdx < 0) continue;

    if(fabs(All_Gens.at(TruthIdx).Eta()-All_Gens.at(i).Eta())>0.1) continue;
    if(fabs(All_Gens.at(TruthIdx).DeltaPhi(All_Gens.at(i)))>0.3) continue;

    if(All_Gens[TruthIdx].MotherIndex() == All_Gens[i].MotherIndex()){
      if(All_Gens[i].Status() == 1) {
        if(fabs(All_Gens[i].PID()) == 11) {
	  nSt1el++;
	}
      }
    }
  }
  ///// Rare cases of Z->e -> ph->e ;
  //// So history of matched el is Z  and is type 1 
  /*
    ===========================================================
    RunNumber:EventNumber = 1:56568815
    indexPIDStatusMIdxMPIDStartPtEtaPhiM
    Matched gen = 26
    23 e- 23 19 Z 23 31.86 0.341.140.00 =======> DrMatched 0.10 
    25 ph 23 19 Z 25 43.01 0.221.210.00 =======> DrMatched 0.03 
    26 e+ 1 25 ph 26 48.50 0.251.190.00 =======> DrMatched 0.00  ---->  ----> 
    27 e- 1 25 ph 27 2.62 0.071.330.00 =======> DrMatched 0.23 
    28 e- 1 23 e- 23 23.76 0.341.140.00 =======> DrMatched 0.10 
    ===========================================================
    RunNumber:EventNumber = 1:56568815
    indexPIDStatusMIdxMPIDStartPtEtaPhiM
    Matched gen = 24
    24e+119Z2440.07-0.74-0.230.00 =======> DrMatched 0.00  ---->  ----> 
    Electron Electron Charge = 1 Pt 48.674604 Lep Eta 0.248848 Lep Phi = 1.193147 type = plus1
    Electron Electron Charge = 1 Pt 39.865459 Lep Eta -0.739164 Lep Phi = -0.22767 type = plus1
    Is CF = 0
    Is CF = 0
    Is Conv = 0
    Is Conv = 0
    Electron Electron Charge = 1 Pt 48.674604 Lep Eta 0.248848 Lep Phi = 1.193147 type = plus1 LeptonIsPromptConv=0
    Electron Electron Charge = 1 Pt 39.865459 Lep Eta -0.739164 Lep Phi = -0.22767 type = plus1 LeptonIsPromptConv=0
  */
  if( !GenGetMother(All_Gens.at(TruthIdx)).IsEmpty()  && !GenGetMother(GenGetMother(All_Gens.at(TruthIdx))).IsEmpty()){

    if( GenGetMother(All_Gens.at(TruthIdx)).PID() == 22 && 
	fabs(GenGetMother(GenGetMother(All_Gens.at(TruthIdx))).PID()) == 11) return true;
    
    if( GenGetMother(All_Gens.at(TruthIdx)).PID() == 22 &&
	fabs(GenGetMother(GenGetMother(All_Gens.at(TruthIdx))).PID()) == 13) return true;
    
    if( GenGetMother(All_Gens.at(TruthIdx)).PID() == 22 &&
	fabs(GenGetMother(GenGetMother(All_Gens.at(TruthIdx))).PID()) == 15) return true;
    
    if( GenGetMother(All_Gens.at(TruthIdx)).PID() == 22 &&
	fabs(GenGetMother(GenGetMother(All_Gens.at(TruthIdx))).PID()) == 23) return true;
    
    if( GenGetMother(All_Gens.at(TruthIdx)).PID() == 22 &&
	fabs(GenGetMother(GenGetMother(All_Gens.at(TruthIdx))).PID()) == 24) return true;
  }

  /// if nSt1el (number of status 1 electrons in dR < 0.4 from same mother , if > 1 e->eee  that passes JH Gen code                                                                                                                                                                                                                                  
  bool bIsConv  =  (nSt1el > 0);

  return bIsConv;

}

bool AnalyzerCore::IsCF(Electron el, std::vector<Gen> truthColl, bool CheckCloseEl){

  int charge_el_reco = el.Charge();
  Lepton l = Lepton(el);

  int LepType = GetLeptonType_JH(el, truthColl);

  if(LepType<= 0)    return false;

  //// Get Index of matched Status 1 el                                                                                                                                                                                      
  unsigned int Idx_Closest    = GenMatchedIdx(el,truthColl);

  if(CheckCloseEl){

    // check if multiple close status 1 el overlap                                                                                                                                                                 
    int LastSelfIdx    = LastAbsSelfMotherIdx(Idx_Closest,All_Gens);

    int nSt1el(0);
    for(unsigned int i=2; i < All_Gens.size(); i++){
      if(Idx_Closest == i) continue;
      if(All_Gens[i].DeltaR(All_Gens[Idx_Closest])>0.1) continue;
      if(All_Gens[Idx_Closest].MotherIndex() == All_Gens[i].MotherIndex()){
        if(All_Gens[i].Status() == 1) {
          if(fabs(All_Gens[i].PID()) == 11) nSt1el++;
        }
      }
    }
    if(nSt1el > 0){
      int MGenQ = (All_Gens[LastSelfIdx].PID()  < 0 ) ? 1 : -1;
      if( fabs(All_Gens[LastSelfIdx].PID()) == 11 && (MGenQ != el.Charge()))  return  true;
      else return  false;
    }
  }

  int IdxType_NearEl = LepType>3? GetPrElType_InSameSCRange(Idx_Closest, truthColl, "IdxType"):Idx_Closest;
  int Idx_NearEl     = LepType>3? IdxType_NearEl/10:Idx_Closest;

  if(charge_el_reco*truthColl.at(Idx_NearEl).PID()>0) return true;

  return false;
}


bool AnalyzerCore::IsCF(Muon mu, std::vector<Gen> truthColl){

  int charge_mu_reco = mu.Charge();
  Lepton l = Lepton(mu);

  int LepType = GetLeptonType_JH(mu, truthColl);

  if(LepType<= 0)    return false;
  int Idx_Closest    = GenMatchedIdx(mu,truthColl);
  int IdxType_NearEl = LepType>3? GetPrElType_InSameSCRange(Idx_Closest, truthColl, "IdxType"):Idx_Closest;
  int Idx_NearEl     = LepType>3? IdxType_NearEl/10:Idx_Closest;

  if(charge_mu_reco*truthColl.at(Idx_NearEl).PID()>0) return true;

  return false;
}

bool AnalyzerCore::ConversionSplitting(std::vector<Lepton *> leps, bool RunConvMode,  int nlep, AnalyzerParameter param){

  if(!RunConvMode) return true;
  if(IsData) return true;


  bool IsSampleConvSplit = false;
  vector<TString> ConvSamples  = {"ZGTo","DYJet","WGToLNuG"};
  for(auto i : ConvSamples) if (MCSample.Contains(i)) IsSampleConvSplit=true;

  if(!IsSampleConvSplit) return true;

  bool SplitExtConv=true;
  if(param.GetChannelFlavour() == "Muon") SplitExtConv=false;
  
  if(HasFlag("GENTConv")) SplitExtConv=true;

  //  int nlep_pt15(0);
  bool LowPtConv = false;
  /// Only remove events if..
  if(nlep != int(leps.size())) return true;
  
  bool hasExtConv=false;
  for(auto ilep : leps){
    if(HasMEPhoton(*ilep)) hasExtConv=true;
    if(ilep->Pt() < 15. && ilep->IsConv()) LowPtConv=true;
  }

  if(MCSample.Contains("ZGTo")){
    if(LowPtConv) return false;

    if(SplitExtConv){
      if(hasExtConv) return true;
      else return false;
    }
    else return true;
  }
  else if(MCSample.Contains("DYJets")) {

    if(LowPtConv) return true;

    if(SplitExtConv){
      if(!hasExtConv) return true;
      else return false;
    }
    else return false;
       
  }
  
  if(MCSample.Contains("WGToLNuG_MG")){

    if(LowPtConv) return true;
    else return false;

  }
  else if(MCSample.Contains("WGToLNuG")) {
    if(!LowPtConv) return true;
    else return false;
  }

  return true;

}

bool AnalyzerCore::ConversionVeto(std::vector<Lepton *> leps,const std::vector<Gen>& gens){

  if (IsData) return true;

  double ph_pt=-1;
  bool photon_found=false;
  bool GENTMatched=false;
  for(auto ilep : leps){
    for(unsigned int i=2; i<gens.size(); i++){
      Gen gen = gens.at(i);
      if(ilep->DeltaR(gen) < 0.2) {
        if(gen.PID() == 22 && gen.isPromptFinalState()) ph_pt = gen.Pt();
        if(gen.PID() == 22 && gen.isPromptFinalState() && gen.Pt()> 15.) {
          photon_found=true;
          GENTMatched=true;
          for(unsigned int j=2; j<gens.size(); j++){
            if(!(fabs(gens.at(j).PID()) <7 || fabs(gens.at(j).PID()) == 21)) continue;
            if(gens.at(j).Status() != 23) continue;
            if(gens.at(j).DeltaR(gen) <0.05)GENTMatched=false;
          }
        }
      }
    }
    if(GENTMatched) break;
  }

  if(!photon_found) {
    cout << "No Matching photon found " << endl;
    PrintGen(gens);
  }

  if(MCSample.Contains("WGTo") ||MCSample.Contains("ZGTo") )   {
    if(!photon_found) return true;
    if(!GENTMatched)cout << "GHENT Conv event removed in " << MCSample << endl;
    cout << "Matched photon pt = " << ph_pt << endl;
    return GENTMatched;
  }
  else if(MCSample.Contains("DYJet") || MCSample.Contains("WJet") ){
    if(!photon_found) return true;
    cout << "Matched photon pt = " << ph_pt << endl;
    return !GENTMatched;
  }

  return true;
}




void AnalyzerCore::GetAFBGenParticles(const vector<Gen>& gens,Gen& parton0,Gen& parton1,Gen& l0,Gen& l1,int mode){
  //mode 0:bare 1:dressed01 2:dressed04 3:beforeFSR
  if(!IsDYSample&&!MCSample.Contains("GamGamToLL")&&!MCSample.Contains("TTLL")){
    cout <<"[SMPAnalyzerCore::GetAFBGenParticles] this is only for dilepton event"<<endl;
    exit(EXIT_FAILURE);
  }
  parton0=Gen();
  parton1=Gen();
  l0=Gen();
  l1=Gen();
  vector<const Gen*> leptons;
  vector<const Gen*> photons;
  int ngen=gens.size();
  for(int i=0;i<ngen;i++){
    if(!gens.at(i).isPrompt()) continue;
    int genpid=gens.at(i).PID();
    if(gens.at(i).isHardProcess()){
      if(abs(genpid)<7||genpid==21||genpid==22){
	if(parton0.IsEmpty()) parton0=gens[i];
	else if(parton1.IsEmpty()) parton1=gens[i];
      }
    }
    if(gens.at(i).Status()==1){
      if(abs(genpid)==11||abs(genpid)==13) leptons.push_back(&gens[i]);
      else if(gens.at(i).PID()==22) photons.push_back(&gens[i]);
    }
  }
  int nlepton=leptons.size();
  const double maxdr=0.4;
  for(int i=0;i<nlepton;i++){
    if(leptons[i]->PID()!=lhe_l0.ID()) continue;
    if(leptons[i]->DeltaR(lhe_l0)>maxdr) continue;
    if( fabs(leptons[i]->E()-lhe_l0.E()) < fabs(l0.E()-lhe_l0.E()) ){
      l0=*leptons[i];
    }
  }
  if(l0.PID()==0){
    for(int i=0;i<nlepton;i++){
      if(leptons[i]->PID()!=lhe_l0.ID()) continue;
      if(l0.PID()==0 || leptons[i]->DeltaR(lhe_l0)<l0.DeltaR(lhe_l0)){
	l0=*leptons[i];
      }
    }
  }
  for(int i=0;i<nlepton;i++){
    if(leptons[i]->PID()!=lhe_l1.ID()) continue;
    if(leptons[i]->DeltaR(lhe_l1)>maxdr) continue;
    if( fabs(leptons[i]->E()-lhe_l1.E()) < fabs(l1.E()-lhe_l1.E()) ){
      l1=*leptons[i];
    }
  }
  if(l1.PID()==0){
    for(int i=0;i<nlepton;i++){
      if(leptons[i]->PID()!=lhe_l1.ID()) continue;
      if(l1.PID()==0 || leptons[i]->DeltaR(lhe_l1)<l1.DeltaR(lhe_l1)){
	l1=*leptons[i];
      }
    }
  }
  if(l0.Pt()<l1.Pt()){
    Gen tmp=l0;
    l0=l1;
    l1=tmp;
  }
  if(mode>=3){
    if(nlepton>=4){
      for(int i=0;i<nlepton;i++){
	if(leptons[i]->Index()==l0.Index()||leptons[i]->Index()==l1.Index()) continue;
	for(int j=i+1;j<nlepton;j++){
	  if(leptons[j]->Index()==l0.Index()||leptons[j]->Index()==l1.Index()) continue;
	  if(!(leptons[i]->PID()+leptons[j]->PID()==0)) continue;
	  vector<int> history_i=TrackGenSelfHistory(*leptons[i],gens);
	  vector<int> history_j=TrackGenSelfHistory(*leptons[j],gens);
	  if(history_i.at(1)==history_j.at(1)){
	    photons.push_back(leptons[i]);
	    photons.push_back(leptons[j]);
	  }
	}
      }
    }
    for(const auto& photon:photons){
      vector<int> history=TrackGenSelfHistory(*photon,gens);
      if(gens[history.at(1)].PID()==l0.PID()) l0+=*photon;
      else if(gens[history.at(1)].PID()==l1.PID()) l1+=*photon;
      else if(gens[history.at(1)].PID()==23){ // for minnlo+photos
	if(photon->DeltaR(l0)<photon->DeltaR(l1)) l0+=*photon;
	else l1+=*photon;
      }
    }    
  }else if(mode>=1){
    double delr=mode==1?0.1:0.4;
    for(const auto& photon:photons){
      if(l0.DeltaR(*photon)>delr&&l1.DeltaR(*photon)>delr) continue;
      if(l0.DeltaR(*photon)<l1.DeltaR(*photon)) l0+=*photon;
      else l1+=*photon;
    }
  }
}

void  AnalyzerCore::GetAFBLHEParticles(const vector<LHE>& lhes,LHE& p0,LHE& p1,LHE& l0,LHE& l1,LHE& j0){
  if(!IsDYSample&&!MCSample.Contains("GamGamToLL")&&!MCSample.Contains("TTLL")){
    cout <<"[AFBAnalyzer::GetAFBLHEParticles] this is only for dilepton event"<<endl;
    exit(EXIT_FAILURE);
  }
  p0=LHE();
  p1=LHE();
  l0=LHE();
  l1=LHE();
  j0=LHE();
  if(!lhes.size()) return;
  for(int i=0;i<(int)lhes.size();i++){
    if(p0.ID()==0&&lhes[i].Status()==-1&&lhes[i].Eta()>0) p0=lhes[i];
    if(p1.ID()==0&&lhes[i].Status()==-1&&lhes[i].Eta()<0) p1=lhes[i];
    if(l0.ID()==0&&(abs(lhes[i].ID())==11||abs(lhes[i].ID())==13||abs(lhes[i].ID())==15)) l0=lhes[i];
    if(l0.ID()&&(abs(lhes[i].ID())==11||abs(lhes[i].ID())==13||abs(lhes[i].ID())==15)) l1=lhes[i];
    if(lhes[i].Status()==1)
      if(abs(lhes[i].ID())<=6||lhes[i].ID()==21)
	if(lhes[i].Pt()>j0.Pt()) j0=lhes[i];
  }
  if(p0.ID()==0||p1.ID()==0||l0.ID()==0||l1.ID()==0){
    cout <<"[AFBAnalyzer::GetLHEParticles] something is wrong"<<endl;
    exit(EXIT_FAILURE);
  }
  if(l0.Pt()<l1.Pt()){
    LHE temp=l0;
    l0=l1;
    l1=temp;
  }
}

