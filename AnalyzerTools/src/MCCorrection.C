#include "MCCorrection.h"

#include <fstream>
#include <nlohmann/json.hpp>


MCCorrection::MCCorrection() : 
IgnoreNoHist(false)
{

  DEBUG=false;
  
  MissingHists.clear();

  histDir = TDirectoryHelper::GetTempDirectory("MCCorrection");

  genFinderDY = new GenFinderForDY();

}

vector<TString> MCCorrection::Split(TString s,TString del){
  TObjArray* array=s.Tokenize(del);
  vector<TString> out;
  for(const auto& obj:*array) out.push_back(((TObjString*)obj)->String());
  array->Delete();
  return out;

}

using json = nlohmann::json;



void MCCorrection::ReadMuonRecoSFs_LowPt(const std::string& json_file)
{
  MuonSFTable.clear();

  std::ifstream fin(json_file);
  if (!fin.is_open()) {
    std::cerr << "ERROR: Could not open low-pt muon SF JSON: "
              << json_file << std::endl;
    return;
  }

  json j;
  fin >> j;

  // Expected structure:
  // j["NUM_TrackerMuons_DEN_genTracks"]["abseta_pt"]
  if (!j.contains("NUM_TrackerMuons_DEN_genTracks")) {
    std::cerr << "ERROR: Low-pt muon SF JSON missing NUM_TrackerMuons_DEN_genTracks: "
              << json_file << std::endl;
    return;
  }

  const json& node =
    j["NUM_TrackerMuons_DEN_genTracks"]["abseta_pt"];

  if (!node.contains("binning")) {
    std::cerr << "ERROR: Low-pt muon SF JSON missing binning array: "
              << json_file << std::endl;
    return;
  }

  const json& binning = node["binning"];
  const json& eta_bins = binning[0]["binning"];
  const json& pt_bins  = binning[1]["binning"];

  // Loop over eta bins
  for (size_t i = 0; i + 1 < eta_bins.size(); ++i) {

    double eta_min = eta_bins[i];
    double eta_max = eta_bins[i+1];

    std::ostringstream eta_key;
    eta_key << "abseta:[" << eta_min << "," << eta_max << "]";

    if (!node.contains(eta_key.str())) continue;

    const json& eta_node = node[eta_key.str()];

    // Loop over pT bins
    for (size_t jbin = 0; jbin + 1 < pt_bins.size(); ++jbin) {

      double pt_min = pt_bins[jbin];
      double pt_max = pt_bins[jbin+1];

      std::ostringstream pt_key;
      pt_key << "pt:[" << pt_min << "," << pt_max << "]";

      if (!eta_node.contains(pt_key.str())) continue;

      const json& leaf = eta_node[pt_key.str()];

      MuonSFEntry e;
      e.eta_min = eta_min;
      e.eta_max = eta_max;
      e.p_min   = pt_min;
      e.p_max   = pt_max;

      e.nominal = leaf.value("value", 1.0);
      e.stat    = leaf.value("stat", 0.0);
      e.syst    = leaf.value("syst", 0.0);

      MuonSFTable.push_back(e);
    }
  }

  std::cout << "Loaded " << MuonSFTable.size()
            << " muon low-pt RECO SF entries from "
            << json_file << std::endl;
}


#include "nlohmann/json.hpp"
using nlohmann::json;

void MCCorrection::ReadMuonRecoSFs_HighPt(const std::string& json_file)
{
  MuonSFTable.clear();

  if (DEBUG) {
    std::cout << "[ReadMuonRecoSFs_HighPt] Opening JSON file: "
              << json_file << std::endl;
  }

  std::ifstream fin(json_file);
  if (!fin.is_open()) {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: Could not open SF JSON file: "
              << json_file << std::endl;
    return;
  }

  json j;
  try {
    fin >> j;
  } catch (const json::parse_error& e) {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: JSON parse error in file "
              << json_file << std::endl
              << "  " << e.what() << std::endl;
    return;
  }

  if (!j.is_object() || !j.contains("corrections")) {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: Missing corrections array in "
              << json_file << std::endl;
    return;
  }

  const json& corrs = j["corrections"];
  if (!corrs.is_array() || corrs.empty()) {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: corrections is not a non-empty array in "
              << json_file << std::endl;
    return;
  }

  const json& corr = corrs[0];
  if (!corr.contains("data")) {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: data node missing in "
              << json_file << std::endl;
    return;
  }

  const json& data = corr["data"];
  std::string nodetype = data.value("nodetype", "");
  if (nodetype != "binning") {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: Expected data.nodetype=binning, got "
              << nodetype << std::endl;
    return;
  }

  if (!data.contains("edges") || !data.contains("content")) {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: Missing edges or content in data node"
              << std::endl;
    return;
  }

  const json& eta_bins    = data["edges"];
  const json& eta_content = data["content"];

  if (!eta_bins.is_array() || !eta_content.is_array()) {
    std::cerr << "[ReadMuonRecoSFs_HighPt] ERROR: eta edges or content not arrays"
              << std::endl;
    return;
  }

  if (DEBUG) {
    std::cout << "[ReadMuonRecoSFs_HighPt] Found "
              << eta_content.size() << " eta bins" << std::endl;
  }

  for (size_t i = 0; i < eta_content.size(); ++i) {

    if (i + 1 >= eta_bins.size()) {
      std::cerr << "[ReadMuonRecoSFs_HighPt] WARNING: eta edge index out of range at i="
                << i << std::endl;
      break;
    }

    double eta_min = eta_bins[i].get<double>();
    double eta_max = eta_bins[i + 1].get<double>();

    const json& pnode = eta_content[i];
    std::string p_nodetype = pnode.value("nodetype", "");
    if (p_nodetype != "binning") {
      std::cerr << "[ReadMuonRecoSFs_HighPt] WARNING: expected p-node binning at eta bin "
                << i << std::endl;
      continue;
    }

    if (!pnode.contains("edges") || !pnode.contains("content")) {
      std::cerr << "[ReadMuonRecoSFs_HighPt] WARNING: p-node missing edges or content at eta bin "
                << i << std::endl;
      continue;
    }

    const json& p_bins    = pnode["edges"];
    const json& p_content = pnode["content"];

    if (DEBUG) {
      std::cout << "[ReadMuonRecoSFs_HighPt]   Eta bin " << i
                << " [" << eta_min << ", " << eta_max << "]"
                << " with " << p_content.size() << " p bins" << std::endl;
    }

    for (size_t jbin = 0; jbin < p_content.size(); ++jbin) {

      if (jbin + 1 >= p_bins.size()) {
        std::cerr << "[ReadMuonRecoSFs_HighPt] WARNING: p edge index out of range at jbin="
                  << jbin << std::endl;
        break;
      }

      double p_min = p_bins[jbin].get<double>();
      double p_max = p_bins[jbin + 1].get<double>();

      const json& catnode = p_content[jbin];

      const json* scale_cat = nullptr;

      std::string cat_nodetype = catnode.value("nodetype", "");
      std::string cat_input    = catnode.value("input", "");

      if (cat_nodetype == "category" && cat_input == "scale_factors") {
        scale_cat = &catnode;
      } else if (catnode.contains("content") && catnode["content"].is_array()) {
        const json& categories = catnode["content"];
        for (const auto& c : categories) {
          if (!c.is_object()) continue;
          std::string c_nodetype = c.value("nodetype", "");
          std::string c_input    = c.value("input", "");
          if (c_nodetype == "category" && c_input == "scale_factors") {
            scale_cat = &c;
            break;
          }
        }
      }

      if (!scale_cat) {
        std::cerr << "[ReadMuonRecoSFs_HighPt] WARNING: no scale_factors category at "
                  << "eta bin " << i << ", p bin " << jbin << std::endl;
        continue;
      }

      if (!scale_cat->contains("content") || !(*scale_cat)["content"].is_array()) {
        std::cerr << "[ReadMuonRecoSFs_HighPt] WARNING: scale_factors category "
                  << "missing content array at eta bin " << i
                  << ", p bin " << jbin << std::endl;
        continue;
      }

      double nominal = 1.0;
      double stat    = 0.0;
      double syst    = 0.0;

      for (const auto& kv : (*scale_cat)["content"]) {
        if (!kv.is_object()) continue;
        std::string key = kv.value("key", "");
        double val = kv.value("value", 0.0);
        if (key == "nominal") nominal = val;
        else if (key == "stat") stat = val;
        else if (key == "syst") syst = val;
      }

      if (DEBUG) {
        std::cout << "[ReadMuonRecoSFs_HighPt]     Entry eta=[" << eta_min
                  << "," << eta_max << "], p=[" << p_min << "," << p_max
                  << "] nominal=" << nominal
                  << " stat=" << stat
                  << " syst=" << syst << std::endl;
      }

      MuonSFEntry entry;
      entry.eta_min = eta_min;
      entry.eta_max = eta_max;
      entry.p_min   = p_min;
      entry.p_max   = p_max;
      entry.nominal = nominal;
      entry.stat    = stat;
      entry.syst    = syst;

      MuonSFTable.push_back(entry);
    }
  }

  std::cout << "[ReadMuonRecoSFs_HighPt] Loaded "
            << MuonSFTable.size()
            << " muon high-p RECO SF entries from "
            << json_file << std::endl;
}



const MCCorrection::MuonSFEntry* MCCorrection::GetMuonRecoSF(double abseta, double p)
{
  for (const auto& e : MuonSFTable) {
    if (abseta >= e.eta_min && abseta < e.eta_max &&
        p      >= e.p_min   && p      < e.p_max) {
      return &e;
    }
  }
  return nullptr;
}

void MCCorrection::ReadHistograms(){

  TString datapath = getenv("DATA_DIR");

  TDirectory* origDir = gDirectory;

  //==== ID/Trigger
  TString IDpath = datapath+"/"+GetEra()+"/ID/";

  vector<TString> elhistmaps=Split(gSystem->GetFromPipe("find "+IDpath+"/Electron/ -name 'histmap*.txt' -type f"),"\n");
  for(const auto& elhistmap:elhistmaps){
    string elline;
    ifstream in(elhistmap);
    while(getline(in,elline)){
      std::istringstream is( elline );

      TString tstring_elline = elline;
      if(tstring_elline.Contains("#")) continue;

      TString a,b,c,d,e,f;
      is >> a; // ID,RECO
      is >> b; // Eff,SF
      is >> c; // <WPnames>
      is >> d; // <rootfilename>
      is >> e; // <histname>
      is >> f; // Class
      
      TString elFileName = IDpath + "/Electron/" + d;
      TFile *file = TFile::Open(elFileName, "READ");
      
      if (!file || file->IsZombie()) {
	cout << "[MCCorrection::ReadHistograms] ERROR: cannot open electron file: "
	     << elFileName << endl;
	if (file) { file->Close(); delete file; }
	origDir->cd();
	continue;
      }
      
      TObject *obj = file->Get(e);
      if (!obj) {
	cout << "[MCCorrection::ReadHistograms] ERROR: cannot find object '"
	     << e << "' in file " << elFileName << endl;
	file->Close();
	delete file;
	origDir->cd();
	continue;
      }

      
      
      if (f == "TH2F" || f == "TH2D") {
	TH2 *h2 = dynamic_cast<TH2 *>(obj);
	if (!h2) {
	  cout << "[MCCorrection::ReadHistograms] ERROR: object '" << e
	       << "' in file " << elFileName << " is not TH2 as expected" << endl;
	} else {
	  TString key = a + "_" + b + "_" + c;
	  if (DEBUG) {
	    cout << "[MCCorrection::ReadHistograms] Electron: storing TH2 with key "
		 << key << " from file " << elFileName
		 << " hist " << e << endl;
	  }
	  histDir->cd();
	  map_hist_Electron[key] = (TH2 *)h2->Clone();
	}
      }
      else if (f == "TGraphAsymmErrors") {
	TGraphAsymmErrors *gr = dynamic_cast<TGraphAsymmErrors *>(obj);
	if (!gr) {
	  cout << "[MCCorrection::ReadHistograms] ERROR: object '" << e
	       << "' in file " << elFileName
	       << " is not TGraphAsymmErrors as expected" << endl;
	} else {
	  TString key = a + "_" + b + "_" + c;
	  if (DEBUG) {
	    cout << "[MCCorrection::ReadHistograms] Electron: storing TGraphAsymmErrors with key "
		 << key << " from file " << elFileName
		 << " hist " << e << endl;
	  }
	  histDir->cd();
	  map_graph_Electron[key] = (TGraphAsymmErrors *)gr->Clone();
	}
      }
      else {
	cout << "[MCCorrection::ReadHistograms] Wrong class type in line: "
	     << elline << endl;
      }
      
      file->Close();
      delete file;
      origDir->cd();
    }
  }
  
  if(DEBUG) cout << "[MCCorrection::MCCorrection] map_hist_Electron :" << endl;
  for(std::map< TString, TH2* >::iterator it=map_hist_Electron.begin(); it!=map_hist_Electron.end(); it++){
    if(DEBUG) cout << "[MCCorrection::MCCorrection] key = " << it->first << endl;
  }
  if(DEBUG)   cout << "[MCCorrection::MCCorrection] map_graph_Electron :" << endl;
  for(std::map< TString, TGraphAsymmErrors* >::iterator it=map_graph_Electron.begin(); it!=map_graph_Electron.end(); it++){
    if(DEBUG)  cout << "[MCCorrection::MCCorrection] key = " << it->first << endl;
  }

  
  vector<TString> muhistmaps=Split(gSystem->GetFromPipe("find "+IDpath+"/Muon/ -name 'histmap*.txt' -type f"),"\n");
  for(const auto& muhistmap:muhistmaps){
    string elline2;
    ifstream in2(muhistmap);
    while(getline(in2,elline2)){
      std::istringstream is( elline2 );

      TString tstring_elline = elline2;
      if(tstring_elline.Contains("#")) continue;
      
      TString a,b,c,d,e;
      is >> a; // ID,RERCO
      is >> b; // Eff,SF
      is >> c; // <WPnames>
      is >> d; // <rootfilename>
      is >> e; // <histname>
      TFile *file = new TFile(IDpath+"/Muon/"+d);
      histDir->cd();
      map_hist_Muon[a+"_"+b+"_"+c] = (TH2 *)file->Get(e)->Clone();
      file->Close();
      delete file;
      origDir->cd();
    }
  }

  if(DEBUG) cout << "[MCCorrection::MCCorrection] map_hist_Muon :" << endl;
  for(std::map< TString, TH2* >::iterator it=map_hist_Muon.begin(); it!=map_hist_Muon.end(); it++){
    if(DEBUG) cout << "[MCCorrection::MCCorrection] key = " << it->first << endl;
  }


  // == Get Prefiring maps
  TString PrefirePath  = datapath+"/"+GetEra()+"/Prefire/";

  string elline3;
  ifstream in3(PrefirePath+"/histmap.txt");
  while(getline(in3,elline3)){
    std::istringstream is( elline3 );

    TString tstring_elline = elline3;
    if(tstring_elline.Contains("#")) continue;

    TString a,b,c;
    is >> a; // Jet, Photon
    is >> b; // <rootfilename>
    is >> c; // <histname>
    
    TFile *file = new TFile(PrefirePath+b);
    histDir->cd();
    map_hist_prefire[a + "_prefire"] = (TH2 *)file->Get(c)->Clone();
    file->Close();
    delete file;
    origDir->cd();
  }


  // == Get Pileup Reweight maps
  TString PUReweightPath = datapath+"/"+GetEra()+"/PileUp/";

  string elline4;
  ifstream  in4(PUReweightPath+"/histmap.txt");
  while(getline(in4,elline4)){
    std::istringstream is( elline4 );

    TString tstring_elline = elline4;
    if(tstring_elline.Contains("#")) continue;

    TString a,b,c;
    is >> a; // sample name
    is >> b; // syst
    is >> c; // rootfile name

    //if(DataYear == 2017 && a!=MCSample) continue;
   
    TFile *file = new TFile(PUReweightPath+c);
    if( (TH1D *)file->Get(a+"_"+b) ){
      histDir->cd();
      map_hist_pileup[a+"_"+b+"_pileup"] = (TH1D *)file->Get(a+"_"+b)->Clone();
    }
    else{
      cout << "[MCCorrection::ReadHistograms] No : " << a + "_" + b << endl;
    }
    file->Close();
    delete file;
    origDir->cd();
  }
/*
  cout << "[MCCorrection::MCCorrection] map_hist_pileup :" << endl;
  for(std::map< TString, TH1D* >::iterator it=map_hist_pileup.begin(); it!=map_hist_pileup.end(); it++){
    cout << it->first << endl;
  }
*/
  
  // == Get Official DY Pt reweight maps
  TString DYPtReweightPath = datapath+"/"+GetEra()+"/DYPtReweight/Zpt_weights_"+TString::Itoa(DataYear,10)+".root";
  TFile *file_DYPtReweightPath = new TFile(DYPtReweightPath);
  histDir->cd();
  hist_DYPtReweight_2D = (TH2D *)file_DYPtReweightPath->Get("zptmass_weights")->Clone();
  file_DYPtReweightPath->Close();
  delete file_DYPtReweightPath;
  origDir->cd();
}

MCCorrection::~MCCorrection(){

  if(MissingHists.size() > 0){
    cout << "MCCorrection Missing Hists " << endl;
    for(auto iMissing : MissingHists) cout << "Missing : " << iMissing << endl;
  }

  delete genFinderDY;

}

void MCCorrection::SetMCSample(TString s){
  MCSample = s;
}
void MCCorrection::SetIsDATA(bool b){
  IsDATA = b;
}
void MCCorrection::SetEventInfo(int r, int l, int e){
  run = r;
  lumi = l;
  event = e;
}
void MCCorrection::SetIsFastSim(bool b){
  IsFastSim = b;
}


double MCCorrection::JetPileUpSF(Jet j, TString WP, int sys){
  
  // corrections taken frmo https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJetIDUL

  if(!j.IsGenMatched()) return 1.;

  double pt = j.UnsmearedPt();
  double eta = j.Eta();

  if (pt > 50) return 1.;
  if (pt < 20) return 1.;
  if(fabs(eta) > 5.) return 1.;

  double value(1.);

  if(DataEra =="2016preVFP"){
    if(WP=="Loose"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=1.0108729600906372;
	  if(sys==1) value=1.020517371594906;
	  if(sys==-1) value=1.0012285485863686;
	}
	else if( pt < 30.0  ){
	  value=1.003844976425171;
	  if(sys==1) value=1.0132539877668023;
	  if(sys==-1) value=0.9944359650835396;
	}
	else if( pt < 40.0  ){
	  value=1.0172959566116333;
	  if(sys==1) value=1.0263737989589572;
	  if(sys==-1) value=1.0082181142643094;
	}
	else if( pt < 50.0  ){
	  value=1.000735878944397;
	  if(sys==1) value=1.008548471145332;
	  if(sys==-1) value=0.992923286743462;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=1.0222991704940796;
	  if(sys==1) value=1.037572399713099;
	  if(sys==-1) value=1.0070259412750602;
	}
	else if( pt < 30.0  ){
	  value=0.9944618344306946;
	  if(sys==1) value=1.0273245833814144;
	  if(sys==-1) value=0.9615990854799747;
	}
	else if( pt < 40.0  ){
	  value=0.9930357336997986;
	  if(sys==1) value=1.0069646667689085;
	  if(sys==-1) value=0.9791068006306888;
	}
	else if( pt < 50.0  ){
	  value=1.0031803846359253;
	  if(sys==1) value=1.018784949555993;
	  if(sys==-1) value=0.9875758197158576;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=1.0011762380599976;
	  if(sys==1) value=1.0149571308866143;
	  if(sys==-1) value=0.9873953452333808;
	}
	else if( pt < 30.0  ){
	  value=1.0192692279815674;
	  if(sys==1) value=1.030311468988657;
	  if(sys==-1) value=1.0082269869744778;
	}
	else if( pt < 40.0  ){
	  value=1.0100284814834597;
	  if(sys==1) value=1.026971710845828;
	  if(sys==-1) value=0.9930852521210908;
	}
	else if( pt < 50.0  ){
	  value=1.003816843032837;
	  if(sys==1) value=1.0134221082553267;
	  if(sys==-1) value=0.9942115778103472;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=1.0035617351531982;
	  if(sys==1) value=1.013137542642653;
	  if(sys==-1) value=0.9939859276637436;
	}
	else if( pt < 30.0  ){
	  value=0.9967981576919556;
	  if(sys==1) value=1.0018321871757507;
	  if(sys==-1) value=0.9917641282081604;
	}
	else if( pt < 40.0  ){
	  value=0.9923815727233888;
	  if(sys==1) value=0.9964602584950626;
	  if(sys==-1) value=0.9883028869517148;
	}
	else if( pt < 50.0  ){
	  value=0.9953727722167968;
	  if(sys==1) value=1.0018926663324237;
	  if(sys==-1) value=0.98885287810117;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=1.00294291973114;
	  if(sys==1) value=1.0056876274757087;
	  if(sys==-1) value=1.0001982119865716;
	}
	else if( pt < 30.0  ){
	  value=1.0057798624038696;
	  if(sys==1) value=1.0098982974886894;
	  if(sys==-1) value=1.0016614273190498;
	}
	else if( pt < 40.0  ){
	  value=1.0006095170974731;
	  if(sys==1) value=1.0016989780124277;
	  if(sys==-1) value=0.9995200561825186;
	}
	else if( pt < 50.0  ){
	  value=1.0013240575790403;
	  if(sys==1) value=1.0094247180968523;
	  if(sys==-1) value=0.9932233970612288;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=1.002514362335205;
	  if(sys==1) value=1.0036641425685957;
	  if(sys==-1) value=1.0013645821018144;
	}
	else if( pt < 30.0  ){
	  value=1.000902771949768;
	  if(sys==1) value=1.0019749021157622;
	  if(sys==-1) value=0.999830641783774;
	}
	else if( pt < 40.0  ){
	  value=1.0003013610839844;
	  if(sys==1) value=1.0013067219406366;
	  if(sys==-1) value=0.999296000227332;
	}
	else if( pt < 50.0  ){
	  value=0.9978928565979004;
	  if(sys==1) value=0.9987609187373891;
	  if(sys==-1) value=0.9970247944584116;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=1.0015063285827637;
	  if(sys==1) value=1.0045477214735;
	  if(sys==-1) value=0.9984649356920272;
	}
	else if( pt < 30.0  ){
	  value=1.001303791999817;
	  if(sys==1) value=1.0025298283435404;
	  if(sys==-1) value=1.0000777556560934;
	}
	else if( pt < 40.0  ){
	  value=1.0001004934310913;
	  if(sys==1) value=1.0012290882878006;
	  if(sys==-1) value=0.998971898574382;
	}
	else if( pt < 50.0  ){
	  value=0.9997990727424622;
	  if(sys==1) value=1.000393255322706;
	  if(sys==-1) value=0.9992048901622184;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9989891052246094;
	  if(sys==1) value=1.0035466756671667;
	  if(sys==-1) value=0.994431534782052;
	}
	else if( pt < 30.0  ){
	  value=1.0003026723861694;
	  if(sys==1) value=1.0030760501977056;
	  if(sys==-1) value=0.9975292945746332;
	}
	else if( pt < 40.0  ){
	  value=0.9995943307876588;
	  if(sys==1) value=1.0010244525037706;
	  if(sys==-1) value=0.9981642090715468;
	}
	else if( pt < 50.0  ){
	  value=0.9987806677818298;
	  if(sys==1) value=1.0012747545260936;
	  if(sys==-1) value=0.996286581037566;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=1.006646990776062;
	  if(sys==1) value=1.0146907223388553;
	  if(sys==-1) value=0.9986032592132688;
	}
	else if( pt < 30.0  ){
	  value=0.9925604462623596;
	  if(sys==1) value=0.9996731565333904;
	  if(sys==-1) value=0.985447735991329;
	}
	else if( pt < 40.0  ){
	  value=0.990737795829773;
	  if(sys==1) value=0.994511561235413;
	  if(sys==-1) value=0.9869640304241328;
	}
	else if( pt < 50.0  ){
	  value=1.0024679899215698;
	  if(sys==1) value=1.0134408688172698;
	  if(sys==-1) value=0.9914951110258698;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=1.021924614906311;
	  if(sys==1) value=1.0461265090852976;
	  if(sys==-1) value=0.9977227207273244;
	}
	else if( pt < 30.0  ){
	  value=1.001871109008789;
	  if(sys==1) value=1.0222946498543024;
	  if(sys==-1) value=0.9814475681632756;
	}
	else if( pt < 40.0  ){
	  value=1.0112050771713257;
	  if(sys==1) value=1.0201185159385204;
	  if(sys==-1) value=1.002291638404131;
	}
	else if( pt < 50.0  ){
	  value=1.0068092346191406;
	  if(sys==1) value=1.019912043586373;
	  if(sys==-1) value=0.993706425651908;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=1.006964087486267;
	  if(sys==1) value=1.0217602932825685;
	  if(sys==-1) value=0.9921678816899656;
	}
	else if( pt < 30.0  ){
	  value=1.0149080753326416;
	  if(sys==1) value=1.0315323378890753;
	  if(sys==-1) value=0.998283812776208;
	}
	else if( pt < 40.0  ){
	  value=0.9976702332496644;
	  if(sys==1) value=1.0122332302853465;
	  if(sys==-1) value=0.983107236213982;
	}
	else if( pt < 50.0  ){
	  value=0.9923028349876404;
	  if(sys==1) value=1.0027680424973369;
	  if(sys==-1) value=0.981837627477944;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=1.0068870782852173;
	  if(sys==1) value=1.0158022344112396;
	  if(sys==-1) value=0.997971922159195;
	}
	else if( pt < 30.0  ){
	  value=1.0063172578811646;
	  if(sys==1) value=1.016610617749393;
	  if(sys==-1) value=0.996023898012936;
	}
	else if( pt < 40.0  ){
	  value=1.0116499662399292;
	  if(sys==1) value=1.0254328828305006;
	  if(sys==-1) value=0.9978670496493578;
	}
	else if( pt < 50.0  ){
	  value=1.016661286354065;
	  if(sys==1) value=1.0220462176948786;
	  if(sys==-1) value=1.0112763550132513;
	}
      }
    }
    if(WP=="Medium"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=1.009238600730896;
	  if(sys==1) value=1.0269603189080954;
	  if(sys==-1) value=0.9915168825536966;
	}
	else if( pt < 30.0  ){
	  value=1.0122334957122805;
	  if(sys==1) value=1.0301006473600864;
	  if(sys==-1) value=0.994366344064474;
	}
	else if( pt < 40.0  ){
	  value=1.0216593742370603;
	  if(sys==1) value=1.0355164716020226;
	  if(sys==-1) value=1.0078022768720984;
	}
	else if( pt < 50.0  ){
	  value=1.0073423385620115;
	  if(sys==1) value=1.025580734014511;
	  if(sys==-1) value=0.9891039431095124;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=1.0109485387802124;
	  if(sys==1) value=1.0278748702257872;
	  if(sys==-1) value=0.9940222073346376;
	}
	else if( pt < 30.0  ){
	  value=0.9906418919563292;
	  if(sys==1) value=1.0070923697203398;
	  if(sys==-1) value=0.9741914141923188;
	}
	else if( pt < 40.0  ){
	  value=0.9812734127044678;
	  if(sys==1) value=0.9991926774382592;
	  if(sys==-1) value=0.9633541479706764;
	}
	else if( pt < 50.0  ){
	  value=0.9828067421913148;
	  if(sys==1) value=0.9968843841925265;
	  if(sys==-1) value=0.968729100190103;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=1.00056791305542;
	  if(sys==1) value=1.0216193478554487;
	  if(sys==-1) value=0.9795164782553912;
	}
	else if( pt < 30.0  ){
	  value=1.0163049697875977;
	  if(sys==1) value=1.0359486266970634;
	  if(sys==-1) value=0.996661312878132;
	}
	else if( pt < 40.0  ){
	  value=1.0045398473739624;
	  if(sys==1) value=1.0149505650624633;
	  if(sys==-1) value=0.9941291296854616;
	}
	else if( pt < 50.0  ){
	  value=1.0044893026351929;
	  if(sys==1) value=1.015910167247057;
	  if(sys==-1) value=0.9930684380233288;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.967920184135437;
	  if(sys==1) value=0.9820404453203082;
	  if(sys==-1) value=0.9537999229505658;
	}
	else if( pt < 30.0  ){
	  value=0.991851270198822;
	  if(sys==1) value=1.0030262311920524;
	  if(sys==-1) value=0.9806763092055916;
	}
	else if( pt < 40.0  ){
	  value=0.9774518609046936;
	  if(sys==1) value=0.9941350482404232;
	  if(sys==-1) value=0.960768673568964;
	}
	else if( pt < 50.0  ){
	  value=0.972534477710724;
	  if(sys==1) value=1.0159219652414322;
	  if(sys==-1) value=0.9291469901800156;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9853202700614928;
	  if(sys==1) value=0.9958924176171422;
	  if(sys==-1) value=0.9747481225058436;
	}
	else if( pt < 30.0  ){
	  value=0.99795800447464;
	  if(sys==1) value=1.007184524089098;
	  if(sys==-1) value=0.9887314848601818;
	}
	else if( pt < 40.0  ){
	  value=0.9859752655029296;
	  if(sys==1) value=0.993187534622848;
	  if(sys==-1) value=0.9787629963830112;
	}
	else if( pt < 50.0  ){
	  value=0.9897959232330322;
	  if(sys==1) value=0.9993089782074094;
	  if(sys==-1) value=0.9802828682586552;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.9760807156562804;
	  if(sys==1) value=0.982783215586096;
	  if(sys==-1) value=0.969378215726465;
	}
	else if( pt < 30.0  ){
	  value=0.9860830903053284;
	  if(sys==1) value=0.990587454289198;
	  if(sys==-1) value=0.9815787263214588;
	}
	else if( pt < 40.0  ){
	  value=0.9853748083114624;
	  if(sys==1) value=0.989158485084772;
	  if(sys==-1) value=0.9815911315381528;
	}
	else if( pt < 50.0  ){
	  value=0.98869389295578;
	  if(sys==1) value=0.9918402663897724;
	  if(sys==-1) value=0.9855475195217878;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9878827333450316;
	  if(sys==1) value=0.99694534111768;
	  if(sys==-1) value=0.9788201255723834;
	}
	else if( pt < 30.0  ){
	  value=0.9890553951263428;
	  if(sys==1) value=0.9936554441228508;
	  if(sys==-1) value=0.9844553461298348;
	}
	else if( pt < 40.0  ){
	  value=0.9892473220825196;
	  if(sys==1) value=0.9926466669421644;
	  if(sys==-1) value=0.9858479772228748;
	}
	else if( pt < 50.0  ){
	  value=0.99429190158844;
	  if(sys==1) value=1.0006286352872849;
	  if(sys==-1) value=0.987955167889595;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.974679172039032;
	  if(sys==1) value=0.9794722837395966;
	  if(sys==-1) value=0.9698860603384674;
	}
	else if( pt < 30.0  ){
	  value=0.9982985258102416;
	  if(sys==1) value=1.0064179431647062;
	  if(sys==-1) value=0.9901791084557772;
	}
	else if( pt < 40.0  ){
	  value=0.9784783124923706;
	  if(sys==1) value=0.9926815293729304;
	  if(sys==-1) value=0.9642750956118108;
	}
	else if( pt < 50.0  ){
	  value=0.9856771230697632;
	  if(sys==1) value=0.9939856007695198;
	  if(sys==-1) value=0.9773686453700066;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9824782609939576;
	  if(sys==1) value=0.999768601730466;
	  if(sys==-1) value=0.9651879202574492;
	}
	else if( pt < 30.0  ){
	  value=0.944740891456604;
	  if(sys==1) value=0.9755425173789264;
	  if(sys==-1) value=0.9139392655342816;
	}
	else if( pt < 40.0  ){
	  value=0.9645242094993592;
	  if(sys==1) value=0.9854970350861548;
	  if(sys==-1) value=0.9435513839125632;
	}
	else if( pt < 50.0  ){
	  value=0.9331141710281372;
	  if(sys==1) value=0.986414946615696;
	  if(sys==-1) value=0.8798133954405785;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=1.0269770622253418;
	  if(sys==1) value=1.0521058049052954;
	  if(sys==-1) value=1.0018483195453882;
	}
	else if( pt < 30.0  ){
	  value=1.0185061693191528;
	  if(sys==1) value=1.033481483347714;
	  if(sys==-1) value=1.0035308552905915;
	}
	else if( pt < 40.0  ){
	  value=1.0063261985778809;
	  if(sys==1) value=1.020033210515976;
	  if(sys==-1) value=0.9926191866397858;
	}
	else if( pt < 50.0  ){
	  value=0.996817111968994;
	  if(sys==1) value=1.0129444804042578;
	  if(sys==-1) value=0.9806897435337304;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.9994221925735474;
	  if(sys==1) value=1.0284410919994116;
	  if(sys==-1) value=0.9704032931476833;
	}
	else if( pt < 30.0  ){
	  value=1.0042245388031006;
	  if(sys==1) value=1.0324964560568333;
	  if(sys==-1) value=0.975952621549368;
	}
	else if( pt < 40.0  ){
	  value=0.9849497675895692;
	  if(sys==1) value=1.008041437715292;
	  if(sys==-1) value=0.9618580974638462;
	}
	else if( pt < 50.0  ){
	  value=0.9988752603530884;
	  if(sys==1) value=1.0207100249826908;
	  if(sys==-1) value=0.977040495723486;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=1.015510082244873;
	  if(sys==1) value=1.029062731191516;
	  if(sys==-1) value=1.0019574332982302;
	}
	else if( pt < 30.0  ){
	  value=1.0157957077026367;
	  if(sys==1) value=1.0301059409976006;
	  if(sys==-1) value=1.0014854744076729;
	}
	else if( pt < 40.0  ){
	  value=1.0148974657058716;
	  if(sys==1) value=1.0353409126400948;
	  if(sys==-1) value=0.9944540187716484;
	}
	else if( pt < 50.0  ){
	  value=1.0151649713516235;
	  if(sys==1) value=1.0250524627044797;
	  if(sys==-1) value=1.0052774799987674;
	}
      }
    }
    if(WP=="Tight"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=1.015224575996399;
	  if(sys==1) value=1.0380124505609274;
	  if(sys==-1) value=0.9924367014318703;
	}
	else if( pt < 30.0  ){
	  value=1.0208653211593628;
	  if(sys==1) value=1.0536956042051315;
	  if(sys==-1) value=0.988035038113594;
	}
	else if( pt < 40.0  ){
	  value=1.0272109508514404;
	  if(sys==1) value=1.059500552713871;
	  if(sys==-1) value=0.99492134898901;
	}
	else if( pt < 50.0  ){
	  value=1.0291582345962524;
	  if(sys==1) value=1.0550516042858362;
	  if(sys==-1) value=1.0032648649066689;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=1.0513708591461182;
	  if(sys==1) value=1.137310028076172;
	  if(sys==-1) value=0.9654316902160645;
	}
	else if( pt < 30.0  ){
	  value=0.9936762452125548;
	  if(sys==1) value=1.025804117321968;
	  if(sys==-1) value=0.9615483731031418;
	}
	else if( pt < 40.0  ){
	  value=0.9632971882820128;
	  if(sys==1) value=0.9877825323492287;
	  if(sys==-1) value=0.938811844214797;
	}
	else if( pt < 50.0  ){
	  value=0.9630456566810608;
	  if(sys==1) value=0.9863274618983268;
	  if(sys==-1) value=0.9397638514637948;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9554686546325684;
	  if(sys==1) value=0.9816301800310612;
	  if(sys==-1) value=0.9293071292340755;
	}
	else if( pt < 30.0  ){
	  value=1.0119526386260986;
	  if(sys==1) value=1.0285060927271843;
	  if(sys==-1) value=0.995399184525013;
	}
	else if( pt < 40.0  ){
	  value=0.992242395877838;
	  if(sys==1) value=1.006057788617909;
	  if(sys==-1) value=0.9784270031377672;
	}
	else if( pt < 50.0  ){
	  value=0.9952195286750792;
	  if(sys==1) value=1.012992363423109;
	  if(sys==-1) value=0.9774466939270496;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.9589192271232604;
	  if(sys==1) value=0.9859559051692486;
	  if(sys==-1) value=0.9318825490772724;
	}
	else if( pt < 30.0  ){
	  value=0.9845906496047974;
	  if(sys==1) value=1.019047062844038;
	  if(sys==-1) value=0.9501342363655568;
	}
	else if( pt < 40.0  ){
	  value=0.9592021107673644;
	  if(sys==1) value=0.9883802272379398;
	  if(sys==-1) value=0.9300239942967892;
	}
	else if( pt < 50.0  ){
	  value=0.9655543565750122;
	  if(sys==1) value=0.9904244095087053;
	  if(sys==-1) value=0.9406843036413192;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.992780327796936;
	  if(sys==1) value=1.008645921945572;
	  if(sys==-1) value=0.9769147336483002;
	}
	else if( pt < 30.0  ){
	  value=1.0010524988174438;
	  if(sys==1) value=1.0093569103628397;
	  if(sys==-1) value=0.992748087272048;
	}
	else if( pt < 40.0  ){
	  value=0.993213951587677;
	  if(sys==1) value=1.0040430752560496;
	  if(sys==-1) value=0.9823848279193044;
	}
	else if( pt < 50.0  ){
	  value=0.9838834404945374;
	  if(sys==1) value=1.033528808504343;
	  if(sys==-1) value=0.9342380724847316;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.9662302136421204;
	  if(sys==1) value=0.9747632909566164;
	  if(sys==-1) value=0.9576971363276244;
	}
	else if( pt < 30.0  ){
	  value=0.9790337085723876;
	  if(sys==1) value=0.986887276172638;
	  if(sys==-1) value=0.9711801409721376;
	}
	else if( pt < 40.0  ){
	  value=0.9799229502677916;
	  if(sys==1) value=0.986653132364154;
	  if(sys==-1) value=0.9731927681714296;
	}
	else if( pt < 50.0  ){
	  value=0.9875119924545288;
	  if(sys==1) value=0.9957363633438944;
	  if(sys==-1) value=0.9792876215651632;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9825466871261596;
	  if(sys==1) value=0.9950406523421408;
	  if(sys==-1) value=0.9700527219101788;
	}
	else if( pt < 30.0  ){
	  value=0.981842875480652;
	  if(sys==1) value=0.9904463216662408;
	  if(sys==-1) value=0.973239429295063;
	}
	else if( pt < 40.0  ){
	  value=0.981294333934784;
	  if(sys==1) value=0.9878163374960424;
	  if(sys==-1) value=0.9747723303735256;
	}
	else if( pt < 50.0  ){
	  value=0.9908015727996826;
	  if(sys==1) value=0.997292194981128;
	  if(sys==-1) value=0.9843109506182371;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9768283367156982;
	  if(sys==1) value=0.9895382467657328;
	  if(sys==-1) value=0.9641184266656636;
	}
	else if( pt < 30.0  ){
	  value=0.9914827346801758;
	  if(sys==1) value=1.0049045318737626;
	  if(sys==-1) value=0.978060937486589;
	}
	else if( pt < 40.0  ){
	  value=0.9590254426002502;
	  if(sys==1) value=0.9786741007119416;
	  if(sys==-1) value=0.9393767844885588;
	}
	else if( pt < 50.0  ){
	  value=0.9777031540870668;
	  if(sys==1) value=0.9906611945480108;
	  if(sys==-1) value=0.9647451136261224;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9666730761528016;
	  if(sys==1) value=0.9956286661326884;
	  if(sys==-1) value=0.9377174861729144;
	}
	else if( pt < 30.0  ){
	  value=0.9360567927360536;
	  if(sys==1) value=0.9830327555537224;
	  if(sys==-1) value=0.8890808299183846;
	}
	else if( pt < 40.0  ){
	  value=0.9742059111595154;
	  if(sys==1) value=0.9937519393861294;
	  if(sys==-1) value=0.9546598829329014;
	}
	else if( pt < 50.0  ){
	  value=0.9701592922210692;
	  if(sys==1) value=1.003482323139906;
	  if(sys==-1) value=0.9368362613022329;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=1.0184986591339111;
	  if(sys==1) value=1.0403483491390944;
	  if(sys==-1) value=0.996648969128728;
	}
	else if( pt < 30.0  ){
	  value=1.005620002746582;
	  if(sys==1) value=1.036993458867073;
	  if(sys==-1) value=0.974246546626091;
	}
	else if( pt < 40.0  ){
	  value=1.0134236812591553;
	  if(sys==1) value=1.0347101371735334;
	  if(sys==-1) value=0.9921372253447772;
	}
	else if( pt < 50.0  ){
	  value=0.987140417098999;
	  if(sys==1) value=1.0047380700707436;
	  if(sys==-1) value=0.9695427641272544;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.9927007555961608;
	  if(sys==1) value=1.0471101142466068;
	  if(sys==-1) value=0.938291396945715;
	}
	else if( pt < 30.0  ){
	  value=0.990735113620758;
	  if(sys==1) value=1.0279352404177189;
	  if(sys==-1) value=0.9535349868237972;
	}
	else if( pt < 40.0  ){
	  value=0.9749282598495485;
	  if(sys==1) value=1.0080080181360245;
	  if(sys==-1) value=0.9418485015630722;
	}
	else if( pt < 50.0  ){
	  value=0.9762906432151794;
	  if(sys==1) value=1.0080364644527435;
	  if(sys==-1) value=0.9445448219776154;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=1.0231517553329468;
	  if(sys==1) value=1.0412511490285397;
	  if(sys==-1) value=1.005052361637354;
	}
	else if( pt < 30.0  ){
	  value=1.0140665769577026;
	  if(sys==1) value=1.045599825680256;
	  if(sys==-1) value=0.9825333282351494;
	}
	else if( pt < 40.0  ){
	  value=1.0226882696151731;
	  if(sys==1) value=1.057081259787083;
	  if(sys==-1) value=0.988295279443264;
	}
	else if( pt < 50.0  ){
	  value=1.021700739860535;
	  if(sys==1) value=1.043820222839713;
	  if(sys==-1) value=0.9995812568813562;
	}
      }
    }
  }

  if(DataEra =="2016postVFP"){
    if(WP=="Loose"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9764943718910216;
	  if(sys==1) value=0.988523037172854;
	  if(sys==-1) value=0.9644657066091896;
	}
	else if( pt < 30.0  ){
	  value=0.9805583953857422;
	  if(sys==1) value=0.995529611594975;
	  if(sys==-1) value=0.9655871791765094;
	}
	else if( pt < 40.0  ){
	  value=0.9717985987663268;
	  if(sys==1) value=0.9827600540593266;
	  if(sys==-1) value=0.9608371434733272;
	}
	else if( pt < 50.0  ){
	  value=0.9793575406074524;
	  if(sys==1) value=0.9858466852456332;
	  if(sys==-1) value=0.9728683959692715;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.9882404208183287;
	  if(sys==1) value=1.0186067707836628;
	  if(sys==-1) value=0.9578740708529948;
	}
	else if( pt < 30.0  ){
	  value=0.975849986076355;
	  if(sys==1) value=0.9971050322055816;
	  if(sys==-1) value=0.9545949399471284;
	}
	else if( pt < 40.0  ){
	  value=0.9438166618347168;
	  if(sys==1) value=0.9571260409429668;
	  if(sys==-1) value=0.9305072827264668;
	}
	else if( pt < 50.0  ){
	  value=0.960119128227234;
	  if(sys==1) value=0.9693156778812408;
	  if(sys==-1) value=0.9509225785732268;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9788523316383362;
	  if(sys==1) value=1.0064546633511782;
	  if(sys==-1) value=0.9512499999254942;
	}
	else if( pt < 30.0  ){
	  value=0.9931873083114624;
	  if(sys==1) value=1.0047235917299986;
	  if(sys==-1) value=0.9816510248929262;
	}
	else if( pt < 40.0  ){
	  value=0.988155722618103;
	  if(sys==1) value=0.9980393508449196;
	  if(sys==-1) value=0.9782720943912864;
	}
	else if( pt < 50.0  ){
	  value=0.968733549118042;
	  if(sys==1) value=0.9773229165002704;
	  if(sys==-1) value=0.9601441817358136;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.9902721643447876;
	  if(sys==1) value=0.9984002029523252;
	  if(sys==-1) value=0.98214412573725;
	}
	else if( pt < 30.0  ){
	  value=0.9924913048744202;
	  if(sys==1) value=0.9991197916679084;
	  if(sys==-1) value=0.985862818080932;
	}
	else if( pt < 40.0  ){
	  value=0.9845371842384338;
	  if(sys==1) value=0.9899434451945126;
	  if(sys==-1) value=0.9791309232823552;
	}
	else if( pt < 50.0  ){
	  value=0.987579584121704;
	  if(sys==1) value=0.9939970029518008;
	  if(sys==-1) value=0.9811621652916074;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=1.00111722946167;
	  if(sys==1) value=1.0064955125562849;
	  if(sys==-1) value=0.9957389463670552;
	}
	else if( pt < 30.0  ){
	  value=1.0006059408187866;
	  if(sys==1) value=1.0036433341447264;
	  if(sys==-1) value=0.9975685474928468;
	}
	else if( pt < 40.0  ){
	  value=0.9984756112098694;
	  if(sys==1) value=1.001216868404299;
	  if(sys==-1) value=0.9957343540154396;
	}
	else if( pt < 50.0  ){
	  value=0.9974622130393982;
	  if(sys==1) value=1.0009497834835202;
	  if(sys==-1) value=0.9939746425952762;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=1.0014088153839111;
	  if(sys==1) value=1.0028147029224783;
	  if(sys==-1) value=1.000002927845344;
	}
	else if( pt < 30.0  ){
	  value=1.0007022619247437;
	  if(sys==1) value=1.0014398613129742;
	  if(sys==-1) value=0.9999646625365132;
	}
	else if( pt < 40.0  ){
	  value=0.9976889491081238;
	  if(sys==1) value=0.9985038510640152;
	  if(sys==-1) value=0.9968740471522324;
	}
	else if( pt < 50.0  ){
	  value=0.9977906942367554;
	  if(sys==1) value=0.99826142861275;
	  if(sys==-1) value=0.9973199598607608;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9987947344779968;
	  if(sys==1) value=1.0019866505172104;
	  if(sys==-1) value=0.9956028184387832;
	}
	else if( pt < 30.0  ){
	  value=1.0006016492843628;
	  if(sys==1) value=1.002056380151771;
	  if(sys==-1) value=0.9991469184169546;
	}
	else if( pt < 40.0  ){
	  value=0.9974884390830994;
	  if(sys==1) value=0.9982049971004016;
	  if(sys==-1) value=0.9967718810657972;
	}
	else if( pt < 50.0  ){
	  value=0.9986939430236816;
	  if(sys==1) value=1.0006140393670648;
	  if(sys==-1) value=0.9967738466802984;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9954342842102052;
	  if(sys==1) value=1.0003525419160724;
	  if(sys==-1) value=0.9905160265043378;
	}
	else if( pt < 30.0  ){
	  value=0.9974737167358398;
	  if(sys==1) value=1.000575816258788;
	  if(sys==-1) value=0.9943716172128916;
	}
	else if( pt < 40.0  ){
	  value=0.996349275112152;
	  if(sys==1) value=0.9982887734659016;
	  if(sys==-1) value=0.9944097767584026;
	}
	else if( pt < 50.0  ){
	  value=0.9952284097671508;
	  if(sys==1) value=0.9980414011515676;
	  if(sys==-1) value=0.992415418382734;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9944986701011658;
	  if(sys==1) value=1.002526368945837;
	  if(sys==-1) value=0.9864709712564944;
	}
	else if( pt < 30.0  ){
	  value=1.0006219148635864;
	  if(sys==1) value=1.0079931202344596;
	  if(sys==-1) value=0.9932507094927132;
	}
	else if( pt < 40.0  ){
	  value=0.9899159073829652;
	  if(sys==1) value=0.9939262070693076;
	  if(sys==-1) value=0.9859056076966226;
	}
	else if( pt < 50.0  ){
	  value=0.9883880019187928;
	  if(sys==1) value=1.0136222895234823;
	  if(sys==-1) value=0.9631537143141032;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.9643930196762084;
	  if(sys==1) value=0.9794936971738936;
	  if(sys==-1) value=0.9492923421785237;
	}
	else if( pt < 30.0  ){
	  value=1.0115385055541992;
	  if(sys==1) value=1.034691151231527;
	  if(sys==-1) value=0.9883858598768712;
	}
	else if( pt < 40.0  ){
	  value=0.9673327207565308;
	  if(sys==1) value=0.9753428427502512;
	  if(sys==-1) value=0.9593225987628102;
	}
	else if( pt < 50.0  ){
	  value=0.966128706932068;
	  if(sys==1) value=0.9781898353248836;
	  if(sys==-1) value=0.9540675785392524;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.9448215365409852;
	  if(sys==1) value=0.9817226901650428;
	  if(sys==-1) value=0.9079203829169272;
	}
	else if( pt < 30.0  ){
	  value=0.9513925313949584;
	  if(sys==1) value=0.9743122793734074;
	  if(sys==-1) value=0.9284727834165096;
	}
	else if( pt < 40.0  ){
	  value=0.9432397484779358;
	  if(sys==1) value=0.9540326362475752;
	  if(sys==-1) value=0.9324468607082964;
	}
	else if( pt < 50.0  ){
	  value=0.9651272892951964;
	  if(sys==1) value=0.982077905908227;
	  if(sys==-1) value=0.948176672682166;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.967510998249054;
	  if(sys==1) value=0.9793760553002356;
	  if(sys==-1) value=0.9556459411978722;
	}
	else if( pt < 30.0  ){
	  value=0.9826385378837584;
	  if(sys==1) value=0.9905560715124012;
	  if(sys==-1) value=0.974721004255116;
	}
	else if( pt < 40.0  ){
	  value=0.9711507558822632;
	  if(sys==1) value=0.9811293249949812;
	  if(sys==-1) value=0.9611721867695452;
	}
	else if( pt < 50.0  ){
	  value=0.987113118171692;
	  if(sys==1) value=0.9937031958252192;
	  if(sys==-1) value=0.9805230405181646;
	}
      }
    }
    if(WP=="Medium"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9368915557861328;
	  if(sys==1) value=0.9515459276735784;
	  if(sys==-1) value=0.9222371838986874;
	}
	else if( pt < 30.0  ){
	  value=0.97916442155838;
	  if(sys==1) value=0.9931109100580215;
	  if(sys==-1) value=0.9652179330587388;
	}
	else if( pt < 40.0  ){
	  value=0.943783402442932;
	  if(sys==1) value=0.9612052775919436;
	  if(sys==-1) value=0.9263615272939204;
	}
	else if( pt < 50.0  ){
	  value=0.968328297138214;
	  if(sys==1) value=0.9803996095433832;
	  if(sys==-1) value=0.9562569847330452;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.9497989416122437;
	  if(sys==1) value=0.9825106374919416;
	  if(sys==-1) value=0.917087245732546;
	}
	else if( pt < 30.0  ){
	  value=0.948367714881897;
	  if(sys==1) value=0.9676130414009094;
	  if(sys==-1) value=0.9291223883628844;
	}
	else if( pt < 40.0  ){
	  value=0.8985410928726196;
	  if(sys==1) value=0.9185530375689268;
	  if(sys==-1) value=0.8785291481763124;
	}
	else if( pt < 50.0  ){
	  value=0.9218207597732544;
	  if(sys==1) value=0.9406112898141146;
	  if(sys==-1) value=0.9030302297323942;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9404815435409546;
	  if(sys==1) value=0.967584377154708;
	  if(sys==-1) value=0.9133787099272012;
	}
	else if( pt < 30.0  ){
	  value=0.9571629762649536;
	  if(sys==1) value=0.9822810925543308;
	  if(sys==-1) value=0.9320448599755764;
	}
	else if( pt < 40.0  ){
	  value=0.9710679054260254;
	  if(sys==1) value=0.9824374951422216;
	  if(sys==-1) value=0.9596983157098292;
	}
	else if( pt < 50.0  ){
	  value=0.94086492061615;
	  if(sys==1) value=0.9556632349267602;
	  if(sys==-1) value=0.9260666063055396;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.9418466687202454;
	  if(sys==1) value=0.9617417175322772;
	  if(sys==-1) value=0.9219516199082136;
	}
	else if( pt < 30.0  ){
	  value=0.9554069638252258;
	  if(sys==1) value=0.9720539320260286;
	  if(sys==-1) value=0.938759995624423;
	}
	else if( pt < 40.0  ){
	  value=0.926270306110382;
	  if(sys==1) value=0.93985331710428;
	  if(sys==-1) value=0.9126872951164842;
	}
	else if( pt < 50.0  ){
	  value=0.9416481852531432;
	  if(sys==1) value=0.9620326533913612;
	  if(sys==-1) value=0.9212637171149254;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9805225729942322;
	  if(sys==1) value=1.0045271925628183;
	  if(sys==-1) value=0.9565179534256458;
	}
	else if( pt < 30.0  ){
	  value=0.9774410724639891;
	  if(sys==1) value=0.9908810639753938;
	  if(sys==-1) value=0.9640010809525849;
	}
	else if( pt < 40.0  ){
	  value=0.9709873795509338;
	  if(sys==1) value=0.9780570077709854;
	  if(sys==-1) value=0.9639177513308824;
	}
	else if( pt < 50.0  ){
	  value=0.9780806303024292;
	  if(sys==1) value=1.000572977587581;
	  if(sys==-1) value=0.9555882830172776;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.9703319668769836;
	  if(sys==1) value=0.9793203352019192;
	  if(sys==-1) value=0.9613435985520482;
	}
	else if( pt < 30.0  ){
	  value=0.9821048378944396;
	  if(sys==1) value=0.986853983718902;
	  if(sys==-1) value=0.9773556920699776;
	}
	else if( pt < 40.0  ){
	  value=0.9732218980789183;
	  if(sys==1) value=0.9762107657734304;
	  if(sys==-1) value=0.9702330303844064;
	}
	else if( pt < 50.0  ){
	  value=0.9835677742958068;
	  if(sys==1) value=0.9868070939555764;
	  if(sys==-1) value=0.9803284546360372;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9610775709152222;
	  if(sys==1) value=0.9708911338821054;
	  if(sys==-1) value=0.951264007948339;
	}
	else if( pt < 30.0  ){
	  value=0.9785289168357848;
	  if(sys==1) value=0.9820270852651448;
	  if(sys==-1) value=0.9750307484064252;
	}
	else if( pt < 40.0  ){
	  value=0.9774290323257446;
	  if(sys==1) value=0.9794968536589296;
	  if(sys==-1) value=0.9753612109925598;
	}
	else if( pt < 50.0  ){
	  value=0.983317255973816;
	  if(sys==1) value=0.9860855543520302;
	  if(sys==-1) value=0.9805489575956016;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9519857168197632;
	  if(sys==1) value=0.9720248617231846;
	  if(sys==-1) value=0.9319465719163418;
	}
	else if( pt < 30.0  ){
	  value=0.9697411060333252;
	  if(sys==1) value=0.9779509976506232;
	  if(sys==-1) value=0.9615312144160272;
	}
	else if( pt < 40.0  ){
	  value=0.9692504405975342;
	  if(sys==1) value=0.9748808559961616;
	  if(sys==-1) value=0.9636200251989068;
	}
	else if( pt < 50.0  ){
	  value=0.959742546081543;
	  if(sys==1) value=0.9728891626000404;
	  if(sys==-1) value=0.9465959295630456;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9542087912559508;
	  if(sys==1) value=0.9708408005535604;
	  if(sys==-1) value=0.9375767819583416;
	}
	else if( pt < 30.0  ){
	  value=0.9620720148086548;
	  if(sys==1) value=0.9850128274410964;
	  if(sys==-1) value=0.9391312021762132;
	}
	else if( pt < 40.0  ){
	  value=0.9326270818710328;
	  if(sys==1) value=0.9468763107433916;
	  if(sys==-1) value=0.918377852998674;
	}
	else if( pt < 50.0  ){
	  value=0.9339836239814758;
	  if(sys==1) value=0.9635264668613672;
	  if(sys==-1) value=0.9044407811015844;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.9225303530693054;
	  if(sys==1) value=0.9402669612318276;
	  if(sys==-1) value=0.9047937449067832;
	}
	else if( pt < 30.0  ){
	  value=0.987621247768402;
	  if(sys==1) value=1.0103125553578138;
	  if(sys==-1) value=0.9649299401789904;
	}
	else if( pt < 40.0  ){
	  value=0.9447421431541444;
	  if(sys==1) value=0.9588945684954524;
	  if(sys==-1) value=0.9305897178128362;
	}
	else if( pt < 50.0  ){
	  value=0.9401068687438964;
	  if(sys==1) value=0.9649587888270617;
	  if(sys==-1) value=0.9152549486607312;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.8687840104103088;
	  if(sys==1) value=0.892739187926054;
	  if(sys==-1) value=0.8448288328945637;
	}
	else if( pt < 30.0  ){
	  value=0.9171204566955566;
	  if(sys==1) value=0.952907420694828;
	  if(sys==-1) value=0.8813334926962852;
	}
	else if( pt < 40.0  ){
	  value=0.90696120262146;
	  if(sys==1) value=0.923633700236678;
	  if(sys==-1) value=0.8902887050062418;
	}
	else if( pt < 50.0  ){
	  value=0.915726602077484;
	  if(sys==1) value=0.939509654417634;
	  if(sys==-1) value=0.8919435497373343;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.9358916878700256;
	  if(sys==1) value=0.9465085500851274;
	  if(sys==-1) value=0.925274825654924;
	}
	else if( pt < 30.0  ){
	  value=0.979978322982788;
	  if(sys==1) value=0.988728603348136;
	  if(sys==-1) value=0.9712280426174402;
	}
	else if( pt < 40.0  ){
	  value=0.9519377946853638;
	  if(sys==1) value=0.9659423679113388;
	  if(sys==-1) value=0.9379332214593888;
	}
	else if( pt < 50.0  ){
	  value=0.9703235030174256;
	  if(sys==1) value=0.9837033431977034;
	  if(sys==-1) value=0.9569436628371476;
	}
      }
    }
    if(WP=="Tight"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.8881083726882935;
	  if(sys==1) value=0.9158145394176244;
	  if(sys==-1) value=0.8604022059589624;
	}
	else if( pt < 30.0  ){
	  value=0.940796434879303;
	  if(sys==1) value=0.9749593324959278;
	  if(sys==-1) value=0.906633537262678;
	}
	else if( pt < 40.0  ){
	  value=0.913599967956543;
	  if(sys==1) value=0.9356414675712584;
	  if(sys==-1) value=0.8915584683418274;
	}
	else if( pt < 50.0  ){
	  value=0.934347927570343;
	  if(sys==1) value=0.9533111769706012;
	  if(sys==-1) value=0.915384678170085;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.8890071511268616;
	  if(sys==1) value=0.9266448989510536;
	  if(sys==-1) value=0.8513694033026695;
	}
	else if( pt < 30.0  ){
	  value=0.8717129826545715;
	  if(sys==1) value=0.90470939129591;
	  if(sys==-1) value=0.8387165740132332;
	}
	else if( pt < 40.0  ){
	  value=0.8417760729789734;
	  if(sys==1) value=0.8694277852773666;
	  if(sys==-1) value=0.8141243606805801;
	}
	else if( pt < 50.0  ){
	  value=0.870175838470459;
	  if(sys==1) value=0.8949620369821787;
	  if(sys==-1) value=0.8453896399587393;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.916359007358551;
	  if(sys==1) value=0.9418538250029088;
	  if(sys==-1) value=0.8908641897141933;
	}
	else if( pt < 30.0  ){
	  value=0.9450721144676208;
	  if(sys==1) value=0.9667845591902732;
	  if(sys==-1) value=0.9233596697449684;
	}
	else if( pt < 40.0  ){
	  value=0.939761996269226;
	  if(sys==1) value=0.9534022286534308;
	  if(sys==-1) value=0.9261217638850212;
	}
	else if( pt < 50.0  ){
	  value=0.9239671230316162;
	  if(sys==1) value=0.9497155360877514;
	  if(sys==-1) value=0.898218709975481;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.8824059367179871;
	  if(sys==1) value=0.918253805488348;
	  if(sys==-1) value=0.8465580679476261;
	}
	else if( pt < 30.0  ){
	  value=0.9087703227996826;
	  if(sys==1) value=0.9441696368157864;
	  if(sys==-1) value=0.8733710087835789;
	}
	else if( pt < 40.0  ){
	  value=0.8677110075950623;
	  if(sys==1) value=0.8841983005404472;
	  if(sys==-1) value=0.8512237146496773;
	}
	else if( pt < 50.0  ){
	  value=0.8827672004699707;
	  if(sys==1) value=0.922607034444809;
	  if(sys==-1) value=0.8429273664951324;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9393235445022584;
	  if(sys==1) value=0.9636361375451088;
	  if(sys==-1) value=0.9150109514594078;
	}
	else if( pt < 30.0  ){
	  value=0.9484681487083436;
	  if(sys==1) value=0.967680336907506;
	  if(sys==-1) value=0.929255960509181;
	}
	else if( pt < 40.0  ){
	  value=0.9115300178527832;
	  if(sys==1) value=0.9234077129513024;
	  if(sys==-1) value=0.8996523227542639;
	}
	else if( pt < 50.0  ){
	  value=0.9107062220573424;
	  if(sys==1) value=0.9382380209863186;
	  if(sys==-1) value=0.8831744231283665;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.9401719570159912;
	  if(sys==1) value=0.9523009406402708;
	  if(sys==-1) value=0.9280429733917116;
	}
	else if( pt < 30.0  ){
	  value=0.9649282693862916;
	  if(sys==1) value=0.972862678579986;
	  if(sys==-1) value=0.9569938601925968;
	}
	else if( pt < 40.0  ){
	  value=0.953787326812744;
	  if(sys==1) value=0.9588240026496352;
	  if(sys==-1) value=0.9487506509758532;
	}
	else if( pt < 50.0  ){
	  value=0.9528379440307616;
	  if(sys==1) value=0.9593537291511892;
	  if(sys==-1) value=0.946322158910334;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9316625595092772;
	  if(sys==1) value=0.944084924645722;
	  if(sys==-1) value=0.9192401943728328;
	}
	else if( pt < 30.0  ){
	  value=0.962064266204834;
	  if(sys==1) value=0.9698028927668928;
	  if(sys==-1) value=0.9543256396427752;
	}
	else if( pt < 40.0  ){
	  value=0.9577284455299376;
	  if(sys==1) value=0.962214351631701;
	  if(sys==-1) value=0.9532425394281744;
	}
	else if( pt < 50.0  ){
	  value=0.9628252983093262;
	  if(sys==1) value=0.9754579970613122;
	  if(sys==-1) value=0.95019259955734;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9120782613754272;
	  if(sys==1) value=0.9351946227252484;
	  if(sys==-1) value=0.8889619000256062;
	}
	else if( pt < 30.0  ){
	  value=0.9351145029067992;
	  if(sys==1) value=0.950827917084098;
	  if(sys==-1) value=0.9194010887295008;
	}
	else if( pt < 40.0  ){
	  value=0.9200109839439392;
	  if(sys==1) value=0.9268980473279952;
	  if(sys==-1) value=0.9131239205598832;
	}
	else if( pt < 50.0  ){
	  value=0.9245796799659728;
	  if(sys==1) value=0.9539031106978656;
	  if(sys==-1) value=0.8952562492340803;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.8994465470314026;
	  if(sys==1) value=0.9277993254363536;
	  if(sys==-1) value=0.8710937686264515;
	}
	else if( pt < 30.0  ){
	  value=0.9313709139823914;
	  if(sys==1) value=0.9763144478201866;
	  if(sys==-1) value=0.8864273801445961;
	}
	else if( pt < 40.0  ){
	  value=0.8677879571914673;
	  if(sys==1) value=0.8919323123991489;
	  if(sys==-1) value=0.8436436019837856;
	}
	else if( pt < 50.0  ){
	  value=0.8828809261322021;
	  if(sys==1) value=0.9215170703828336;
	  if(sys==-1) value=0.8442447818815708;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.9021578431129456;
	  if(sys==1) value=0.9234448913484812;
	  if(sys==-1) value=0.8808707948774099;
	}
	else if( pt < 30.0  ){
	  value=0.9719262719154358;
	  if(sys==1) value=1.000553885474801;
	  if(sys==-1) value=0.9432986583560704;
	}
	else if( pt < 40.0  ){
	  value=0.9164524674415588;
	  if(sys==1) value=0.9318654239177704;
	  if(sys==-1) value=0.9010395109653472;
	}
	else if( pt < 50.0  ){
	  value=0.8871735334396362;
	  if(sys==1) value=0.9114967342466116;
	  if(sys==-1) value=0.8628503326326609;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.7892871499061584;
	  if(sys==1) value=0.8198089506477118;
	  if(sys==-1) value=0.7587653491646051;
	}
	else if( pt < 30.0  ){
	  value=0.8377863168716431;
	  if(sys==1) value=0.8919403031468391;
	  if(sys==-1) value=0.783632330596447;
	}
	else if( pt < 40.0  ){
	  value=0.8475003242492676;
	  if(sys==1) value=0.8805415332317352;
	  if(sys==-1) value=0.8144591152667999;
	}
	else if( pt < 50.0  ){
	  value=0.869925856590271;
	  if(sys==1) value=0.9093753919005394;
	  if(sys==-1) value=0.8304763212800026;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.886569082736969;
	  if(sys==1) value=0.903524961322546;
	  if(sys==-1) value=0.869613204151392;
	}
	else if( pt < 30.0  ){
	  value=0.9545508623123168;
	  if(sys==1) value=0.974157875403762;
	  if(sys==-1) value=0.934943849220872;
	}
	else if( pt < 40.0  ){
	  value=0.9322054982185364;
	  if(sys==1) value=0.946978872641921;
	  if(sys==-1) value=0.9174321237951516;
	}
	else if( pt < 50.0  ){
	  value=0.9388443827629088;
	  if(sys==1) value=0.960156623274088;
	  if(sys==-1) value=0.91753214225173;
	}
      }
    }
  }
  if(DataEra =="2017"){
    if(WP=="Loose"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9704928398132324;
	  if(sys==1) value=0.9774027420207858;
	  if(sys==-1) value=0.963582937605679;
	}
	else if( pt < 30.0  ){
	  value=0.9877998232841492;
	  if(sys==1) value=0.9957791874185205;
	  if(sys==-1) value=0.979820459149778;
	}
	else if( pt < 40.0  ){
	  value=0.987576961517334;
	  if(sys==1) value=0.9973523309454322;
	  if(sys==-1) value=0.9778015920892358;
	}
	else if( pt < 50.0  ){
	  value=0.9824506640434264;
	  if(sys==1) value=0.9914622437208892;
	  if(sys==-1) value=0.973439084365964;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=1.0038564205169678;
	  if(sys==1) value=1.0371177159249785;
	  if(sys==-1) value=0.9705951251089572;
	}
	else if( pt < 30.0  ){
	  value=1.0089690685272217;
	  if(sys==1) value=1.0282266773283482;
	  if(sys==-1) value=0.9897114597260952;
	}
	else if( pt < 40.0  ){
	  value=0.9778291583061218;
	  if(sys==1) value=0.9909565597772598;
	  if(sys==-1) value=0.9647017568349838;
	}
	else if( pt < 50.0  ){
	  value=0.9826528429985046;
	  if(sys==1) value=1.00451279617846;
	  if(sys==-1) value=0.9607928898185492;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9783495664596558;
	  if(sys==1) value=1.008137697353959;
	  if(sys==-1) value=0.9485614355653524;
	}
	else if( pt < 30.0  ){
	  value=0.9957076907157898;
	  if(sys==1) value=1.019806755706668;
	  if(sys==-1) value=0.9716086257249116;
	}
	else if( pt < 40.0  ){
	  value=0.9818277359008788;
	  if(sys==1) value=0.9956630039960146;
	  if(sys==-1) value=0.9679924678057432;
	}
	else if( pt < 50.0  ){
	  value=0.989267647266388;
	  if(sys==1) value=1.003722507506609;
	  if(sys==-1) value=0.9748127870261668;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.9942102432250975;
	  if(sys==1) value=1.0034406743943691;
	  if(sys==-1) value=0.9849798120558262;
	}
	else if( pt < 30.0  ){
	  value=0.99796724319458;
	  if(sys==1) value=1.001285555306822;
	  if(sys==-1) value=0.994648931082338;
	}
	else if( pt < 40.0  ){
	  value=0.992525041103363;
	  if(sys==1) value=0.995440344326198;
	  if(sys==-1) value=0.989609737880528;
	}
	else if( pt < 50.0  ){
	  value=0.9900828003883362;
	  if(sys==1) value=0.9975515361875296;
	  if(sys==-1) value=0.9826140645891428;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=1.0012199878692627;
	  if(sys==1) value=1.006566529162228;
	  if(sys==-1) value=0.9958734465762972;
	}
	else if( pt < 30.0  ){
	  value=1.0005053281784058;
	  if(sys==1) value=1.0035254468675705;
	  if(sys==-1) value=0.9974852094892412;
	}
	else if( pt < 40.0  ){
	  value=0.9948417544364928;
	  if(sys==1) value=0.996098724193871;
	  if(sys==-1) value=0.9935847846791148;
	}
	else if( pt < 50.0  ){
	  value=1.0002033710479736;
	  if(sys==1) value=1.004329009912908;
	  if(sys==-1) value=0.9960777321830392;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.9989957213401794;
	  if(sys==1) value=1.002802147064358;
	  if(sys==-1) value=0.9951892956160008;
	}
	else if( pt < 30.0  ){
	  value=1.0010031461715698;
	  if(sys==1) value=1.0026208370691163;
	  if(sys==-1) value=0.9993854552740232;
	}
	else if( pt < 40.0  ){
	  value=0.9979901313781738;
	  if(sys==1) value=0.9985836961423048;
	  if(sys==-1) value=0.997396566614043;
	}
	else if( pt < 50.0  ){
	  value=0.9976873397827148;
	  if(sys==1) value=0.9983609852497466;
	  if(sys==-1) value=0.9970136943156832;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.997891128063202;
	  if(sys==1) value=1.0017616800032556;
	  if(sys==-1) value=0.9940205761231482;
	}
	else if( pt < 30.0  ){
	  value=1.001104474067688;
	  if(sys==1) value=1.0024873954243958;
	  if(sys==-1) value=0.9997215527109802;
	}
	else if( pt < 40.0  ){
	  value=0.9991947412490844;
	  if(sys==1) value=0.9997365478775464;
	  if(sys==-1) value=0.9986529346206224;
	}
	else if( pt < 50.0  ){
	  value=0.9987912774086;
	  if(sys==1) value=0.9993866269360296;
	  if(sys==-1) value=0.99819592788117;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9974673390388488;
	  if(sys==1) value=1.002103884704411;
	  if(sys==-1) value=0.9928307933732868;
	}
	else if( pt < 30.0  ){
	  value=1.0012121200561523;
	  if(sys==1) value=1.0041447808034718;
	  if(sys==-1) value=0.9982794593088328;
	}
	else if( pt < 40.0  ){
	  value=0.9949336647987366;
	  if(sys==1) value=0.995736597222276;
	  if(sys==-1) value=0.9941307323751972;
	}
	else if( pt < 50.0  ){
	  value=0.9966449737548828;
	  if(sys==1) value=0.9977524315472692;
	  if(sys==-1) value=0.9955375159624964;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9894250631332396;
	  if(sys==1) value=0.9971075020730495;
	  if(sys==-1) value=0.98174262419343;
	}
	else if( pt < 30.0  ){
	  value=0.9962260127067566;
	  if(sys==1) value=1.0010350216180086;
	  if(sys==-1) value=0.9914170037955046;
	}
	else if( pt < 40.0  ){
	  value=0.98963463306427;
	  if(sys==1) value=0.9927234202623368;
	  if(sys==-1) value=0.9865458458662032;
	}
	else if( pt < 50.0  ){
	  value=0.9891897439956664;
	  if(sys==1) value=0.9925775928422808;
	  if(sys==-1) value=0.985801895149052;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.9727675914764404;
	  if(sys==1) value=1.026849914342165;
	  if(sys==-1) value=0.918685268610716;
	}
	else if( pt < 30.0  ){
	  value=0.991970419883728;
	  if(sys==1) value=1.0038654301315546;
	  if(sys==-1) value=0.9800754096359016;
	}
	else if( pt < 40.0  ){
	  value=0.9768085479736328;
	  if(sys==1) value=1.0129484459757805;
	  if(sys==-1) value=0.9406686499714852;
	}
	else if( pt < 50.0  ){
	  value=0.977146565914154;
	  if(sys==1) value=0.9910143138840796;
	  if(sys==-1) value=0.9632788179442286;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=1.011976718902588;
	  if(sys==1) value=1.064836736768484;
	  if(sys==-1) value=0.9591167010366916;
	}
	else if( pt < 30.0  ){
	  value=0.9985321760177612;
	  if(sys==1) value=1.0232694279402494;
	  if(sys==-1) value=0.973794924095273;
	}
	else if( pt < 40.0  ){
	  value=0.9920911192893982;
	  if(sys==1) value=1.0053841825574636;
	  if(sys==-1) value=0.9787980560213329;
	}
	else if( pt < 50.0  ){
	  value=0.9709333181381226;
	  if(sys==1) value=0.9864700874313712;
	  if(sys==-1) value=0.955396548844874;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.9686812162399292;
	  if(sys==1) value=0.9791983449831604;
	  if(sys==-1) value=0.958164087496698;
	}
	else if( pt < 30.0  ){
	  value=0.9877821803092957;
	  if(sys==1) value=0.998684199526906;
	  if(sys==-1) value=0.9768801610916852;
	}
	else if( pt < 40.0  ){
	  value=0.9849979281425476;
	  if(sys==1) value=0.996074004098773;
	  if(sys==-1) value=0.9739218521863222;
	}
	else if( pt < 50.0  ){
	  value=0.9889028072357178;
	  if(sys==1) value=1.000779878348112;
	  if(sys==-1) value=0.9770257361233234;
	}
      }
    }
    if(WP=="Medium"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9479166865348816;
	  if(sys==1) value=0.9571364261209964;
	  if(sys==-1) value=0.9386969469487668;
	}
	else if( pt < 30.0  ){
	  value=0.9735035300254822;
	  if(sys==1) value=0.9903011880815028;
	  if(sys==-1) value=0.9567058719694614;
	}
	else if( pt < 40.0  ){
	  value=0.9718155264854432;
	  if(sys==1) value=0.9903217256069184;
	  if(sys==-1) value=0.953309327363968;
	}
	else if( pt < 50.0  ){
	  value=0.973204493522644;
	  if(sys==1) value=0.9847613144665956;
	  if(sys==-1) value=0.9616476725786924;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.981261432170868;
	  if(sys==1) value=1.041458036750555;
	  if(sys==-1) value=0.9210648275911808;
	}
	else if( pt < 30.0  ){
	  value=0.9982516169548036;
	  if(sys==1) value=1.0379988849163055;
	  if(sys==-1) value=0.9585043489933014;
	}
	else if( pt < 40.0  ){
	  value=0.965974986553192;
	  if(sys==1) value=0.9948978051543236;
	  if(sys==-1) value=0.9370521679520608;
	}
	else if( pt < 50.0  ){
	  value=0.9601989984512328;
	  if(sys==1) value=0.9861661791801452;
	  if(sys==-1) value=0.9342318177223206;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9485541582107544;
	  if(sys==1) value=0.97174527682364;
	  if(sys==-1) value=0.9253630395978688;
	}
	else if( pt < 30.0  ){
	  value=0.9650957584381104;
	  if(sys==1) value=0.9947757814079524;
	  if(sys==-1) value=0.9354157354682684;
	}
	else if( pt < 40.0  ){
	  value=0.9703133702278136;
	  if(sys==1) value=0.9849568950012326;
	  if(sys==-1) value=0.9556698454543948;
	}
	else if( pt < 50.0  ){
	  value=0.9728686213493348;
	  if(sys==1) value=0.9920425266027452;
	  if(sys==-1) value=0.9536947160959244;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.9407263398170472;
	  if(sys==1) value=0.9563353834673762;
	  if(sys==-1) value=0.925117296166718;
	}
	else if( pt < 30.0  ){
	  value=0.9627998471260072;
	  if(sys==1) value=0.9798490218818188;
	  if(sys==-1) value=0.9457506723701954;
	}
	else if( pt < 40.0  ){
	  value=0.9561551213264464;
	  if(sys==1) value=0.9669150728732347;
	  if(sys==-1) value=0.9453951697796584;
	}
	else if( pt < 50.0  ){
	  value=0.9363899230957032;
	  if(sys==1) value=0.9572511874139308;
	  if(sys==-1) value=0.9155286587774754;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9776323437690736;
	  if(sys==1) value=1.0046027470380068;
	  if(sys==-1) value=0.9506619405001402;
	}
	else if( pt < 30.0  ){
	  value=0.9796167612075806;
	  if(sys==1) value=0.9837882639840244;
	  if(sys==-1) value=0.9754452584311366;
	}
	else if( pt < 40.0  ){
	  value=0.9693097472190856;
	  if(sys==1) value=0.9753864724189044;
	  if(sys==-1) value=0.9632330220192672;
	}
	else if( pt < 50.0  ){
	  value=0.9674500226974488;
	  if(sys==1) value=0.97253954783082;
	  if(sys==-1) value=0.9623604975640774;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.9722836017608644;
	  if(sys==1) value=0.9805829050019385;
	  if(sys==-1) value=0.9639842985197902;
	}
	else if( pt < 30.0  ){
	  value=0.9859439134597778;
	  if(sys==1) value=0.989763204474002;
	  if(sys==-1) value=0.9821246224455537;
	}
	else if( pt < 40.0  ){
	  value=0.984768271446228;
	  if(sys==1) value=0.9874068358913064;
	  if(sys==-1) value=0.9821297070011497;
	}
	else if( pt < 50.0  ){
	  value=0.9876219034194946;
	  if(sys==1) value=0.9919511922635138;
	  if(sys==-1) value=0.9832926145754756;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9714887738227844;
	  if(sys==1) value=0.9785751006565988;
	  if(sys==-1) value=0.96440244698897;
	}
	else if( pt < 30.0  ){
	  value=0.9888686537742616;
	  if(sys==1) value=0.9934129696339368;
	  if(sys==-1) value=0.984324337914586;
	}
	else if( pt < 40.0  ){
	  value=0.988268494606018;
	  if(sys==1) value=0.99177270475775;
	  if(sys==-1) value=0.984764284454286;
	}
	else if( pt < 50.0  ){
	  value=0.9920573830604552;
	  if(sys==1) value=0.9949814747087656;
	  if(sys==-1) value=0.9891332914121448;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9673900604248048;
	  if(sys==1) value=0.9902042336761951;
	  if(sys==-1) value=0.9445758871734142;
	}
	else if( pt < 30.0  ){
	  value=0.9708315134048462;
	  if(sys==1) value=0.9826444946229458;
	  if(sys==-1) value=0.9590185321867466;
	}
	else if( pt < 40.0  ){
	  value=0.9676527976989746;
	  if(sys==1) value=0.9749464252963662;
	  if(sys==-1) value=0.960359170101583;
	}
	else if( pt < 50.0  ){
	  value=0.9662854671478271;
	  if(sys==1) value=0.974022792186588;
	  if(sys==-1) value=0.9585481421090662;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9305241703987122;
	  if(sys==1) value=0.9550872780382632;
	  if(sys==-1) value=0.905961062759161;
	}
	else if( pt < 30.0  ){
	  value=0.9466087818145752;
	  if(sys==1) value=0.9681361243128777;
	  if(sys==-1) value=0.9250814393162728;
	}
	else if( pt < 40.0  ){
	  value=0.9424522519111632;
	  if(sys==1) value=0.955069019459188;
	  if(sys==-1) value=0.9298354843631388;
	}
	else if( pt < 50.0  ){
	  value=0.9294148683547974;
	  if(sys==1) value=0.9568702019751072;
	  if(sys==-1) value=0.9019595347344876;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.947242259979248;
	  if(sys==1) value=0.9698379132896662;
	  if(sys==-1) value=0.92464660666883;
	}
	else if( pt < 30.0  ){
	  value=0.9764279127120972;
	  if(sys==1) value=0.9994936678558588;
	  if(sys==-1) value=0.9533621575683356;
	}
	else if( pt < 40.0  ){
	  value=0.95717054605484;
	  if(sys==1) value=0.9748294930905104;
	  if(sys==-1) value=0.9395115990191698;
	}
	else if( pt < 50.0  ){
	  value=0.9599449038505554;
	  if(sys==1) value=0.9863823745399714;
	  if(sys==-1) value=0.9335074331611396;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=1.0318809747695925;
	  if(sys==1) value=1.100379854440689;
	  if(sys==-1) value=0.9633820950984956;
	}
	else if( pt < 30.0  ){
	  value=0.9641295075416564;
	  if(sys==1) value=1.0210994146764278;
	  if(sys==-1) value=0.9071596004068851;
	}
	else if( pt < 40.0  ){
	  value=0.9606981873512268;
	  if(sys==1) value=0.9830762539058924;
	  if(sys==-1) value=0.9383201207965612;
	}
	else if( pt < 50.0  ){
	  value=0.9541211128234864;
	  if(sys==1) value=0.9801766462624072;
	  if(sys==-1) value=0.9280655793845654;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.9652435779571532;
	  if(sys==1) value=0.9800928719341756;
	  if(sys==-1) value=0.9503942839801311;
	}
	else if( pt < 30.0  ){
	  value=0.9855998158454896;
	  if(sys==1) value=1.003025010228157;
	  if(sys==-1) value=0.968174621462822;
	}
	else if( pt < 40.0  ){
	  value=0.9766442775726318;
	  if(sys==1) value=0.9973241630941628;
	  if(sys==-1) value=0.9559643920511008;
	}
	else if( pt < 50.0  ){
	  value=0.9744853377342224;
	  if(sys==1) value=0.9916402325034142;
	  if(sys==-1) value=0.9573304429650308;
	}
      }
    }
    if(WP=="Tight"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9225448966026306;
	  if(sys==1) value=0.9449704922735692;
	  if(sys==-1) value=0.9001193009316921;
	}
	else if( pt < 30.0  ){
	  value=0.95746910572052;
	  if(sys==1) value=0.9915497228503228;
	  if(sys==-1) value=0.9233884885907172;
	}
	else if( pt < 40.0  ){
	  value=0.9437658190727234;
	  if(sys==1) value=0.9733462538570166;
	  if(sys==-1) value=0.9141853842884302;
	}
	else if( pt < 50.0  ){
	  value=0.948938250541687;
	  if(sys==1) value=0.9709912613034248;
	  if(sys==-1) value=0.9268852397799492;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.9290843605995178;
	  if(sys==1) value=1.0025271400809288;
	  if(sys==-1) value=0.8556415811181068;
	}
	else if( pt < 30.0  ){
	  value=0.970641791820526;
	  if(sys==1) value=1.0240733847022057;
	  if(sys==-1) value=0.9172101989388466;
	}
	else if( pt < 40.0  ){
	  value=0.9364191293716432;
	  if(sys==1) value=0.9711876660585405;
	  if(sys==-1) value=0.9016505926847458;
	}
	else if( pt < 50.0  ){
	  value=0.944108545780182;
	  if(sys==1) value=0.9969371184706688;
	  if(sys==-1) value=0.891279973089695;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9328270554542542;
	  if(sys==1) value=0.986523911356926;
	  if(sys==-1) value=0.8791301995515823;
	}
	else if( pt < 30.0  ){
	  value=0.9371998310089112;
	  if(sys==1) value=0.977646578103304;
	  if(sys==-1) value=0.8967530839145184;
	}
	else if( pt < 40.0  ){
	  value=0.9584156274795532;
	  if(sys==1) value=0.9767194911837578;
	  if(sys==-1) value=0.9401117637753488;
	}
	else if( pt < 50.0  ){
	  value=0.9608407020568848;
	  if(sys==1) value=0.9982155375182628;
	  if(sys==-1) value=0.9234658665955068;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.8981014490127563;
	  if(sys==1) value=0.9212721772491932;
	  if(sys==-1) value=0.8749307207763195;
	}
	else if( pt < 30.0  ){
	  value=0.9301182627677916;
	  if(sys==1) value=0.9618608988821508;
	  if(sys==-1) value=0.8983756266534328;
	}
	else if( pt < 40.0  ){
	  value=0.903658926486969;
	  if(sys==1) value=0.9257435239851476;
	  if(sys==-1) value=0.8815743289887905;
	}
	else if( pt < 50.0  ){
	  value=0.877452552318573;
	  if(sys==1) value=0.9161321595311164;
	  if(sys==-1) value=0.8387729451060295;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9423590302467346;
	  if(sys==1) value=0.9671793729066848;
	  if(sys==-1) value=0.9175386875867844;
	}
	else if( pt < 30.0  ){
	  value=0.9621372222900392;
	  if(sys==1) value=0.9716144362464548;
	  if(sys==-1) value=0.9526600083336234;
	}
	else if( pt < 40.0  ){
	  value=0.9374666810035706;
	  if(sys==1) value=0.947746112011373;
	  if(sys==-1) value=0.927187249995768;
	}
	else if( pt < 50.0  ){
	  value=0.9392529726028442;
	  if(sys==1) value=0.9584447890520096;
	  if(sys==-1) value=0.9200611561536788;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.952170729637146;
	  if(sys==1) value=0.9622245691716672;
	  if(sys==-1) value=0.9421168901026248;
	}
	else if( pt < 30.0  ){
	  value=0.9773167371749878;
	  if(sys==1) value=0.9856116585433484;
	  if(sys==-1) value=0.9690218158066272;
	}
	else if( pt < 40.0  ){
	  value=0.9747474789619446;
	  if(sys==1) value=0.9808540800586344;
	  if(sys==-1) value=0.9686408778652548;
	}
	else if( pt < 50.0  ){
	  value=0.9762046933174132;
	  if(sys==1) value=0.9816812695935369;
	  if(sys==-1) value=0.9707281170412898;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9576103091239928;
	  if(sys==1) value=0.9688726756721736;
	  if(sys==-1) value=0.9463479425758125;
	}
	else if( pt < 30.0  ){
	  value=0.9824218153953552;
	  if(sys==1) value=0.9923951867967844;
	  if(sys==-1) value=0.972448443993926;
	}
	else if( pt < 40.0  ){
	  value=0.9824478030204772;
	  if(sys==1) value=0.988515071105212;
	  if(sys==-1) value=0.9763805349357426;
	}
	else if( pt < 50.0  ){
	  value=0.983664095401764;
	  if(sys==1) value=0.99112440738827;
	  if(sys==-1) value=0.976203783415258;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.943404257297516;
	  if(sys==1) value=0.9688571617007256;
	  if(sys==-1) value=0.9179513528943062;
	}
	else if( pt < 30.0  ){
	  value=0.9439650177955629;
	  if(sys==1) value=0.9669431485235692;
	  if(sys==-1) value=0.9209868870675564;
	}
	else if( pt < 40.0  ){
	  value=0.9291677474975586;
	  if(sys==1) value=0.9447317561134696;
	  if(sys==-1) value=0.9136037388816476;
	}
	else if( pt < 50.0  ){
	  value=0.9285306334495544;
	  if(sys==1) value=0.9468708727508784;
	  if(sys==-1) value=0.9101903941482306;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.891372799873352;
	  if(sys==1) value=0.9272712990641594;
	  if(sys==-1) value=0.8554743006825447;
	}
	else if( pt < 30.0  ){
	  value=0.9108262658119202;
	  if(sys==1) value=0.9601360596716404;
	  if(sys==-1) value=0.8615164719521999;
	}
	else if( pt < 40.0  ){
	  value=0.8866842985153198;
	  if(sys==1) value=0.9109666980803012;
	  if(sys==-1) value=0.8624018989503384;
	}
	else if( pt < 50.0  ){
	  value=0.8617858290672302;
	  if(sys==1) value=0.8990232534706593;
	  if(sys==-1) value=0.8245484046638012;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.9325675964355468;
	  if(sys==1) value=0.9598763287067412;
	  if(sys==-1) value=0.9052588641643524;
	}
	else if( pt < 30.0  ){
	  value=0.964238941669464;
	  if(sys==1) value=0.999591052532196;
	  if(sys==-1) value=0.9288868308067322;
	}
	else if( pt < 40.0  ){
	  value=0.9402828812599182;
	  if(sys==1) value=0.9595694411545992;
	  if(sys==-1) value=0.9209963213652372;
	}
	else if( pt < 50.0  ){
	  value=0.9414673447608948;
	  if(sys==1) value=0.9810311533510684;
	  if(sys==-1) value=0.901903536170721;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.939677894115448;
	  if(sys==1) value=0.9979004263877868;
	  if(sys==-1) value=0.8814553618431091;
	}
	else if( pt < 30.0  ){
	  value=0.9140788316726683;
	  if(sys==1) value=0.981552854180336;
	  if(sys==-1) value=0.8466048091650009;
	}
	else if( pt < 40.0  ){
	  value=0.9178310632705688;
	  if(sys==1) value=0.9542306251823902;
	  if(sys==-1) value=0.8814315013587475;
	}
	else if( pt < 50.0  ){
	  value=0.8977847099304199;
	  if(sys==1) value=0.9234836045652628;
	  if(sys==-1) value=0.872085815295577;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.9414833188056946;
	  if(sys==1) value=0.9670762866735458;
	  if(sys==-1) value=0.9158903509378432;
	}
	else if( pt < 30.0  ){
	  value=0.9723371267318726;
	  if(sys==1) value=1.0046567767858503;
	  if(sys==-1) value=0.9400174766778946;
	}
	else if( pt < 40.0  ){
	  value=0.9663638472557068;
	  if(sys==1) value=1.0041960142552853;
	  if(sys==-1) value=0.9285316802561284;
	}
	else if( pt < 50.0  ){
	  value=0.9483115673065186;
	  if(sys==1) value=0.9784704297780992;
	  if(sys==-1) value=0.918152704834938;
	}
      }
    }
  }

  if(DataEra =="2018"){
    if(WP=="Loose"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9730643033981324;
	  if(sys==1) value=0.983271673321724;
	  if(sys==-1) value=0.9628569334745408;
	}
	else if( pt < 30.0  ){
	  value=0.983113706111908;
	  if(sys==1) value=0.9938151286914944;
	  if(sys==-1) value=0.9724122835323216;
	}
	else if( pt < 40.0  ){
	  value=0.9807552099227904;
	  if(sys==1) value=0.9900195393711329;
	  if(sys==-1) value=0.9714908804744482;
	}
	else if( pt < 50.0  ){
	  value=0.9823193550109864;
	  if(sys==1) value=0.9919309290125966;
	  if(sys==-1) value=0.972707781009376;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.9893547892570496;
	  if(sys==1) value=1.0094086099416018;
	  if(sys==-1) value=0.9693009685724974;
	}
	else if( pt < 30.0  ){
	  value=0.9852572679519652;
	  if(sys==1) value=1.005913743749261;
	  if(sys==-1) value=0.9646007921546698;
	}
	else if( pt < 40.0  ){
	  value=0.9701254963874816;
	  if(sys==1) value=0.9829556718468666;
	  if(sys==-1) value=0.9572953209280968;
	}
	else if( pt < 50.0  ){
	  value=0.9712991118431092;
	  if(sys==1) value=0.9801072888076304;
	  if(sys==-1) value=0.9624909348785876;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9935064911842346;
	  if(sys==1) value=1.0087972730398178;
	  if(sys==-1) value=0.9782157093286514;
	}
	else if( pt < 30.0  ){
	  value=0.9844945073127748;
	  if(sys==1) value=1.0143085811287165;
	  if(sys==-1) value=0.9546804334968328;
	}
	else if( pt < 40.0  ){
	  value=0.9937040209770204;
	  if(sys==1) value=0.9987259171903132;
	  if(sys==-1) value=0.9886821247637272;
	}
	else if( pt < 50.0  ){
	  value=1.0006418228149414;
	  if(sys==1) value=1.007793817203492;
	  if(sys==-1) value=0.9934898284263908;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.9901512861251832;
	  if(sys==1) value=0.9969759648665786;
	  if(sys==-1) value=0.9833266073837876;
	}
	else if( pt < 30.0  ){
	  value=0.9886822700500488;
	  if(sys==1) value=0.9961008769460022;
	  if(sys==-1) value=0.9812636631540954;
	}
	else if( pt < 40.0  ){
	  value=0.990407168865204;
	  if(sys==1) value=0.9953695726580918;
	  if(sys==-1) value=0.985444765072316;
	}
	else if( pt < 50.0  ){
	  value=0.9932383894920348;
	  if(sys==1) value=0.9950301381759346;
	  if(sys==-1) value=0.9914466408081352;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9970674514770508;
	  if(sys==1) value=1.0006464391481131;
	  if(sys==-1) value=0.9934884638059884;
	}
	else if( pt < 30.0  ){
	  value=0.9985875487327576;
	  if(sys==1) value=0.9997805861057714;
	  if(sys==-1) value=0.9973945113597438;
	}
	else if( pt < 40.0  ){
	  value=0.9965534210205078;
	  if(sys==1) value=0.9974705799249932;
	  if(sys==-1) value=0.9956362621160224;
	}
	else if( pt < 50.0  ){
	  value=0.9956389665603638;
	  if(sys==1) value=0.9974404386011884;
	  if(sys==-1) value=0.9938374945195392;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.998893678188324;
	  if(sys==1) value=1.002201255178079;
	  if(sys==-1) value=0.9955861011985688;
	}
	else if( pt < 30.0  ){
	  value=0.9985945224761964;
	  if(sys==1) value=0.9999336055479944;
	  if(sys==-1) value=0.9972554394043982;
	}
	else if( pt < 40.0  ){
	  value=0.9975852966308594;
	  if(sys==1) value=0.9981797148357144;
	  if(sys==-1) value=0.9969908784260042;
	}
	else if( pt < 50.0  ){
	  value=0.9974826574325562;
	  if(sys==1) value=0.9985357348341496;
	  if(sys==-1) value=0.9964295800309628;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=1.0001007318496704;
	  if(sys==1) value=1.0021169607061893;
	  if(sys==-1) value=0.9980845029931515;
	}
	else if( pt < 30.0  ){
	  value=0.9989957213401794;
	  if(sys==1) value=1.0005022494588047;
	  if(sys==-1) value=0.9974891932215542;
	}
	else if( pt < 40.0  ){
	  value=0.9969788789749146;
	  if(sys==1) value=0.997341444453923;
	  if(sys==-1) value=0.996616313495906;
	}
	else if( pt < 50.0  ){
	  value=0.9965764284133912;
	  if(sys==1) value=0.9969393314677292;
	  if(sys==-1) value=0.9962135253590532;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9941337704658508;
	  if(sys==1) value=0.9983492409810424;
	  if(sys==-1) value=0.9899182999506592;
	}
	else if( pt < 30.0  ){
	  value=0.9967732429504396;
	  if(sys==1) value=0.9989909585565327;
	  if(sys==-1) value=0.994555527344346;
	}
	else if( pt < 40.0  ){
	  value=0.9939271211624146;
	  if(sys==1) value=0.9962150664068758;
	  if(sys==-1) value=0.9916391759179533;
	}
	else if( pt < 50.0  ){
	  value=0.9948238730430604;
	  if(sys==1) value=0.9968288438394666;
	  if(sys==-1) value=0.992818902246654;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9961037039756776;
	  if(sys==1) value=1.0017537814565003;
	  if(sys==-1) value=0.9904536264948548;
	}
	else if( pt < 30.0  ){
	  value=0.9978662729263306;
	  if(sys==1) value=1.005708470009267;
	  if(sys==-1) value=0.9900240758433938;
	}
	else if( pt < 40.0  ){
	  value=0.9943519830703736;
	  if(sys==1) value=0.9975536861456932;
	  if(sys==-1) value=0.991150279995054;
	}
	else if( pt < 50.0  ){
	  value=0.9894294142723083;
	  if(sys==1) value=0.99177955230698;
	  if(sys==-1) value=0.9870792762376368;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.9651383757591248;
	  if(sys==1) value=0.9918622877448796;
	  if(sys==-1) value=0.9384144637733698;
	}
	else if( pt < 30.0  ){
	  value=0.9897080063819884;
	  if(sys==1) value=1.0128183010965586;
	  if(sys==-1) value=0.9665977116674184;
	}
	else if( pt < 40.0  ){
	  value=0.9803033471107484;
	  if(sys==1) value=0.9934488460421562;
	  if(sys==-1) value=0.9671578481793404;
	}
	else if( pt < 50.0  ){
	  value=0.97774475812912;
	  if(sys==1) value=0.9832028415985404;
	  if(sys==-1) value=0.9722866746596992;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=1.0134971141815186;
	  if(sys==1) value=1.0586192645132542;
	  if(sys==-1) value=0.9683749638497828;
	}
	else if( pt < 30.0  ){
	  value=0.9733235836029052;
	  if(sys==1) value=1.0016967244446278;
	  if(sys==-1) value=0.9449504427611828;
	}
	else if( pt < 40.0  ){
	  value=0.9820564985275269;
	  if(sys==1) value=0.9931835550814868;
	  if(sys==-1) value=0.970929441973567;
	}
	else if( pt < 50.0  ){
	  value=0.969573140144348;
	  if(sys==1) value=0.985459104180336;
	  if(sys==-1) value=0.9536871761083604;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.9794412851333618;
	  if(sys==1) value=0.9864100404083728;
	  if(sys==-1) value=0.9724725298583508;
	}
	else if( pt < 30.0  ){
	  value=0.978374481201172;
	  if(sys==1) value=0.994885127991438;
	  if(sys==-1) value=0.9618638344109058;
	}
	else if( pt < 40.0  ){
	  value=0.9792837500572203;
	  if(sys==1) value=0.9899646863341331;
	  if(sys==-1) value=0.9686028137803078;
	}
	else if( pt < 50.0  ){
	  value=0.9870522618293762;
	  if(sys==1) value=0.9969467138871552;
	  if(sys==-1) value=0.9771578097715974;
	}
      }
    }
    if(WP=="Medium"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9443326592445374;
	  if(sys==1) value=0.9597311401739717;
	  if(sys==-1) value=0.928934178315103;
	}
	else if( pt < 30.0  ){
	  value=0.9672638177871704;
	  if(sys==1) value=0.986627833917737;
	  if(sys==-1) value=0.9478998016566038;
	}
	else if( pt < 40.0  ){
	  value=0.9671739339828492;
	  if(sys==1) value=0.981185589917004;
	  if(sys==-1) value=0.953162278048694;
	}
	else if( pt < 50.0  ){
	  value=0.9671528935432434;
	  if(sys==1) value=0.9809407237917184;
	  if(sys==-1) value=0.9533650632947684;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.9980430006980896;
	  if(sys==1) value=1.0330252051353457;
	  if(sys==-1) value=0.9630607962608336;
	}
	else if( pt < 30.0  ){
	  value=0.9794815182685852;
	  if(sys==1) value=1.002917006611824;
	  if(sys==-1) value=0.9560460299253464;
	}
	else if( pt < 40.0  ){
	  value=0.9706997871398926;
	  if(sys==1) value=0.986885318532586;
	  if(sys==-1) value=0.9545142557471992;
	}
	else if( pt < 50.0  ){
	  value=0.966023325920105;
	  if(sys==1) value=0.9765391675755382;
	  if(sys==-1) value=0.9555074842646718;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=0.9799513220787048;
	  if(sys==1) value=1.003381410613656;
	  if(sys==-1) value=0.9565212335437536;
	}
	else if( pt < 30.0  ){
	  value=0.9862907528877258;
	  if(sys==1) value=1.009206559509039;
	  if(sys==-1) value=0.9633749462664128;
	}
	else if( pt < 40.0  ){
	  value=0.9910166263580322;
	  if(sys==1) value=0.9991491390392184;
	  if(sys==-1) value=0.982884113676846;
	}
	else if( pt < 50.0  ){
	  value=1.0111744403839111;
	  if(sys==1) value=1.0210414789617062;
	  if(sys==-1) value=1.001307401806116;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.9366703629493712;
	  if(sys==1) value=0.9535877499729396;
	  if(sys==-1) value=0.9197529759258032;
	}
	else if( pt < 30.0  ){
	  value=0.9636510610580444;
	  if(sys==1) value=0.9862790983170272;
	  if(sys==-1) value=0.9410230237990618;
	}
	else if( pt < 40.0  ){
	  value=0.9598051309585572;
	  if(sys==1) value=0.9832114819437264;
	  if(sys==-1) value=0.9363987799733876;
	}
	else if( pt < 50.0  ){
	  value=0.9407570958137512;
	  if(sys==1) value=0.9619723726063968;
	  if(sys==-1) value=0.9195418190211058;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9678887724876404;
	  if(sys==1) value=0.9786772578954696;
	  if(sys==-1) value=0.9571002870798112;
	}
	else if( pt < 30.0  ){
	  value=0.9773459434509276;
	  if(sys==1) value=0.9935336522758008;
	  if(sys==-1) value=0.9611582346260548;
	}
	else if( pt < 40.0  ){
	  value=0.9768170118331908;
	  if(sys==1) value=0.985347406938672;
	  if(sys==-1) value=0.9682866167277098;
	}
	else if( pt < 50.0  ){
	  value=0.9703410267829896;
	  if(sys==1) value=0.9840672938153148;
	  if(sys==-1) value=0.9566147597506642;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.9674434065818788;
	  if(sys==1) value=0.977762933820486;
	  if(sys==-1) value=0.9571238793432713;
	}
	else if( pt < 30.0  ){
	  value=0.9796333909034728;
	  if(sys==1) value=0.984389207791537;
	  if(sys==-1) value=0.9748775740154088;
	}
	else if( pt < 40.0  ){
	  value=0.9814410209655762;
	  if(sys==1) value=0.9862394649535418;
	  if(sys==-1) value=0.9766425769776106;
	}
	else if( pt < 50.0  ){
	  value=0.9790186882019044;
	  if(sys==1) value=0.9839864131063224;
	  if(sys==-1) value=0.9740509632974864;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9670897722244264;
	  if(sys==1) value=0.9781615948304534;
	  if(sys==-1) value=0.9560179496183993;
	}
	else if( pt < 30.0  ){
	  value=0.9820140600204468;
	  if(sys==1) value=0.9866345305927098;
	  if(sys==-1) value=0.9773935894481838;
	}
	else if( pt < 40.0  ){
	  value=0.9811092615127563;
	  if(sys==1) value=0.9839900378137828;
	  if(sys==-1) value=0.97822848521173;
	}
	else if( pt < 50.0  ){
	  value=0.9822949171066284;
	  if(sys==1) value=0.9875711146742104;
	  if(sys==-1) value=0.9770187195390464;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.9571226835250854;
	  if(sys==1) value=0.9716032696887852;
	  if(sys==-1) value=0.9426420973613858;
	}
	else if( pt < 30.0  ){
	  value=0.9708889126777648;
	  if(sys==1) value=0.9853320131078362;
	  if(sys==-1) value=0.9564458122476936;
	}
	else if( pt < 40.0  ){
	  value=0.9753014445304872;
	  if(sys==1) value=0.9847442554309964;
	  if(sys==-1) value=0.9658586336299776;
	}
	else if( pt < 50.0  ){
	  value=0.96933251619339;
	  if(sys==1) value=0.9825607845559716;
	  if(sys==-1) value=0.9561042478308082;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.9445341229438782;
	  if(sys==1) value=0.9692758154124022;
	  if(sys==-1) value=0.9197924304753542;
	}
	else if( pt < 30.0  ){
	  value=0.9540934562683104;
	  if(sys==1) value=0.9836427941918372;
	  if(sys==-1) value=0.9245441183447838;
	}
	else if( pt < 40.0  ){
	  value=0.9581589698791504;
	  if(sys==1) value=0.976039109751582;
	  if(sys==-1) value=0.9402788300067186;
	}
	else if( pt < 50.0  ){
	  value=0.9355971813201904;
	  if(sys==1) value=0.9621349982917308;
	  if(sys==-1) value=0.90905936434865;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.9228169322013856;
	  if(sys==1) value=0.9514927547425032;
	  if(sys==-1) value=0.8941411096602678;
	}
	else if( pt < 30.0  ){
	  value=0.9755691289901732;
	  if(sys==1) value=0.998990062624216;
	  if(sys==-1) value=0.9521481953561306;
	}
	else if( pt < 40.0  ){
	  value=0.9705421924591064;
	  if(sys==1) value=0.9853245066478848;
	  if(sys==-1) value=0.955759878270328;
	}
	else if( pt < 50.0  ){
	  value=0.9672954082489014;
	  if(sys==1) value=0.9734411938115954;
	  if(sys==-1) value=0.9611496226862072;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.9813975691795348;
	  if(sys==1) value=1.0107917804270985;
	  if(sys==-1) value=0.9520033579319715;
	}
	else if( pt < 30.0  ){
	  value=0.9552091360092164;
	  if(sys==1) value=0.9896924048662186;
	  if(sys==-1) value=0.920725867152214;
	}
	else if( pt < 40.0  ){
	  value=0.9535202980041504;
	  if(sys==1) value=0.9708480909466743;
	  if(sys==-1) value=0.9361925050616264;
	}
	else if( pt < 50.0  ){
	  value=0.9527621865272522;
	  if(sys==1) value=0.967692155390978;
	  if(sys==-1) value=0.9378322176635264;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.9634611010551452;
	  if(sys==1) value=0.9828852135688066;
	  if(sys==-1) value=0.944036988541484;
	}
	else if( pt < 30.0  ){
	  value=0.9638413190841676;
	  if(sys==1) value=0.988035149872303;
	  if(sys==-1) value=0.939647488296032;
	}
	else if( pt < 40.0  ){
	  value=0.9635021090507508;
	  if(sys==1) value=0.9790805811062456;
	  if(sys==-1) value=0.947923636995256;
	}
	else if( pt < 50.0  ){
	  value=0.9704571962356568;
	  if(sys==1) value=0.983060646802187;
	  if(sys==-1) value=0.9578537456691264;
	}
      }
    }
    if(WP=="Tight"){
      if( eta < -3.0  ){
	if( pt < 25.0  ){
	  value=0.9123855233192444;
	  if(sys==1) value=0.9447780214250088;
	  if(sys==-1) value=0.87999302521348;
	}
	else if( pt < 30.0  ){
	  value=0.9325392246246338;
	  if(sys==1) value=0.966895341873169;
	  if(sys==-1) value=0.8981831073760986;
	}
	else if( pt < 40.0  ){
	  value=0.9323574304580688;
	  if(sys==1) value=0.9566156696528196;
	  if(sys==-1) value=0.908099191263318;
	}
	else if( pt < 50.0  ){
	  value=0.947959840297699;
	  if(sys==1) value=0.9720894731581212;
	  if(sys==-1) value=0.9238302074372768;
	}
      }
      else if( eta < -2.75  ){
	if( pt < 25.0  ){
	  value=0.9777221083641052;
	  if(sys==1) value=1.0181144140660765;
	  if(sys==-1) value=0.9373298026621342;
	}
	else if( pt < 30.0  ){
	  value=0.9769383668899536;
	  if(sys==1) value=1.0043769795447588;
	  if(sys==-1) value=0.9494997542351484;
	}
	else if( pt < 40.0  ){
	  value=0.9594559669494628;
	  if(sys==1) value=0.9821218084543945;
	  if(sys==-1) value=0.9367901254445314;
	}
	else if( pt < 50.0  ){
	  value=0.9679809212684632;
	  if(sys==1) value=0.987114930525422;
	  if(sys==-1) value=0.9488469120115042;
	}
      }
      else if( eta < -2.5  ){
	if( pt < 25.0  ){
	  value=1.005553960800171;
	  if(sys==1) value=1.1022052317857742;
	  if(sys==-1) value=0.9089026898145676;
	}
	else if( pt < 30.0  ){
	  value=0.98653244972229;
	  if(sys==1) value=1.0054620038717983;
	  if(sys==-1) value=0.9676028955727816;
	}
	else if( pt < 40.0  ){
	  value=0.9891988635063172;
	  if(sys==1) value=0.9982815692201256;
	  if(sys==-1) value=0.9801161577925086;
	}
	else if( pt < 50.0  ){
	  value=1.0060012340545654;
	  if(sys==1) value=1.025231333449483;
	  if(sys==-1) value=0.986771134659648;
	}
      }
      else if( eta < -2.0  ){
	if( pt < 25.0  ){
	  value=0.8920367360115051;
	  if(sys==1) value=0.9188260082155466;
	  if(sys==-1) value=0.8652474638074636;
	}
	else if( pt < 30.0  ){
	  value=0.9376601576805116;
	  if(sys==1) value=0.9889065325260162;
	  if(sys==-1) value=0.8864137828350067;
	}
	else if( pt < 40.0  ){
	  value=0.9146437048912048;
	  if(sys==1) value=0.9498930163681508;
	  if(sys==-1) value=0.879394393414259;
	}
	else if( pt < 50.0  ){
	  value=0.887484610080719;
	  if(sys==1) value=0.9202889204025269;
	  if(sys==-1) value=0.8546802997589111;
	}
      }
      else if( eta < -1.479  ){
	if( pt < 25.0  ){
	  value=0.9376685619354248;
	  if(sys==1) value=0.9632300194352864;
	  if(sys==-1) value=0.9121071044355632;
	}
	else if( pt < 30.0  ){
	  value=0.9598485231399536;
	  if(sys==1) value=0.993195068091154;
	  if(sys==-1) value=0.9265019781887532;
	}
	else if( pt < 40.0  ){
	  value=0.9445458650588988;
	  if(sys==1) value=0.9618737380951644;
	  if(sys==-1) value=0.9272179920226336;
	}
	else if( pt < 50.0  ){
	  value=0.932072103023529;
	  if(sys==1) value=0.9686540365219116;
	  if(sys==-1) value=0.8954901695251465;
	}
      }
      else if( eta < 0.0  ){
	if( pt < 25.0  ){
	  value=0.943747580051422;
	  if(sys==1) value=0.9593330305069684;
	  if(sys==-1) value=0.9281621295958756;
	}
	else if( pt < 30.0  ){
	  value=0.9667944312095642;
	  if(sys==1) value=0.9781299317255616;
	  if(sys==-1) value=0.9554589306935668;
	}
	else if( pt < 40.0  ){
	  value=0.9677889347076416;
	  if(sys==1) value=0.9752939864993097;
	  if(sys==-1) value=0.9602838829159736;
	}
	else if( pt < 50.0  ){
	  value=0.96150004863739;
	  if(sys==1) value=0.9716003024950624;
	  if(sys==-1) value=0.951399794779718;
	}
      }
      else if( eta < 1.479  ){
	if( pt < 25.0  ){
	  value=0.9492308497428894;
	  if(sys==1) value=0.963837245479226;
	  if(sys==-1) value=0.9346244540065528;
	}
	else if( pt < 30.0  ){
	  value=0.9700198769569396;
	  if(sys==1) value=0.9810011368244886;
	  if(sys==-1) value=0.9590386170893908;
	}
	else if( pt < 40.0  ){
	  value=0.970570147037506;
	  if(sys==1) value=0.9787962175905704;
	  if(sys==-1) value=0.9623440764844418;
	}
	else if( pt < 50.0  ){
	  value=0.9644343256950378;
	  if(sys==1) value=0.9708873918280004;
	  if(sys==-1) value=0.9579812595620752;
	}
      }
      else if( eta < 2.0  ){
	if( pt < 25.0  ){
	  value=0.927402138710022;
	  if(sys==1) value=0.9549942184239626;
	  if(sys==-1) value=0.8998100589960814;
	}
	else if( pt < 30.0  ){
	  value=0.9488179087638856;
	  if(sys==1) value=0.98749141395092;
	  if(sys==-1) value=0.9101444035768508;
	}
	else if( pt < 40.0  ){
	  value=0.9348430633544922;
	  if(sys==1) value=0.9540467467159032;
	  if(sys==-1) value=0.9156393799930812;
	}
	else if( pt < 50.0  ){
	  value=0.938547432422638;
	  if(sys==1) value=0.976015493273735;
	  if(sys==-1) value=0.9010793715715408;
	}
      }
      else if( eta < 2.5  ){
	if( pt < 25.0  ){
	  value=0.904109537601471;
	  if(sys==1) value=0.9509893022477628;
	  if(sys==-1) value=0.8572297729551792;
	}
	else if( pt < 30.0  ){
	  value=0.9214363098144532;
	  if(sys==1) value=0.985083632171154;
	  if(sys==-1) value=0.8577889874577522;
	}
	else if( pt < 40.0  ){
	  value=0.9172147512435912;
	  if(sys==1) value=0.9501766227185726;
	  if(sys==-1) value=0.88425287976861;
	}
	else if( pt < 50.0  ){
	  value=0.8936662673950195;
	  if(sys==1) value=0.9322713054716588;
	  if(sys==-1) value=0.8550612293183804;
	}
      }
      else if( eta < 2.75  ){
	if( pt < 25.0  ){
	  value=0.8969632387161255;
	  if(sys==1) value=0.940669409930706;
	  if(sys==-1) value=0.853257067501545;
	}
	else if( pt < 30.0  ){
	  value=0.9583280682563782;
	  if(sys==1) value=0.9843799620866776;
	  if(sys==-1) value=0.9322761744260788;
	}
	else if( pt < 40.0  ){
	  value=0.9523929953575134;
	  if(sys==1) value=0.9740919806063176;
	  if(sys==-1) value=0.9306940101087092;
	}
	else if( pt < 50.0  ){
	  value=0.9607360363006592;
	  if(sys==1) value=0.9943101778626442;
	  if(sys==-1) value=0.9271618947386742;
	}
      }
      else if( eta < 3.0  ){
	if( pt < 25.0  ){
	  value=0.948461890220642;
	  if(sys==1) value=0.978716617450118;
	  if(sys==-1) value=0.918207162991166;
	}
	else if( pt < 30.0  ){
	  value=0.9412412047386168;
	  if(sys==1) value=0.9904454052448272;
	  if(sys==-1) value=0.8920370042324066;
	}
	else if( pt < 40.0  ){
	  value=0.9336880445480348;
	  if(sys==1) value=0.9517748281359673;
	  if(sys==-1) value=0.915601260960102;
	}
	else if( pt < 50.0  ){
	  value=0.9314092993736268;
	  if(sys==1) value=0.9575607255101204;
	  if(sys==-1) value=0.905257873237133;
	}
      }
      else if( eta < 5.0  ){
	if( pt < 25.0  ){
	  value=0.9306880831718444;
	  if(sys==1) value=0.9647228494286536;
	  if(sys==-1) value=0.8966533169150352;
	}
	else if( pt < 30.0  ){
	  value=0.9270596504211426;
	  if(sys==1) value=0.9691345021128654;
	  if(sys==-1) value=0.8849847987294197;
	}
	else if( pt < 40.0  ){
	  value=0.9346536993980408;
	  if(sys==1) value=0.9594519138336182;
	  if(sys==-1) value=0.9098554849624634;
	}
	else if( pt < 50.0  ){
	  value=0.950029730796814;
	  if(sys==1) value=0.9757633972913028;
	  if(sys==-1) value=0.9242960643023252;
	}
      }
    }
  }
  

  return value; 


}

double MCCorrection::MuonReco_SF(TString key, double eta, double p, int sys){

  //cout << "[MCCorrection::MuonReco_SF] key = " << key << endl;
  //cout << "[MCCorrection::MuonReco_SF] eta = " << eta << ", p = " << p << endl;

  if(key=="Default") return 1.;

  double value = -1.;
  double error_up = 0.;
  double error_down = 0.;
  double error = 0.;

  eta = fabs(eta);

  
  if(key=="HighPtMuonRecoSF" || (key=="MuonRecoSF" && p > 200) ){
        
    //==== XXX this histogram uses P not Pt    

    //==== boundaries
    if(p<50.) p = 50.;
    if(p>=3500.) p = 3499.;
    if(eta>=2.4) eta = 2.39;

    
    const MCCorrection::MuonSFEntry* e = GetMuonRecoSF(eta, p);
    if (!e) return 1.0;
    
    if (sys == 0) return e->nominal;
    if (sys == +1) return e->nominal + e->syst;
    if (sys == -1) return e->nominal - e->syst;
    if (sys == +2) return e->nominal + e->stat;
    if (sys == -2) return e->nominal - e->stat;
    
    
    //https://twiki.cern.ch/twiki/bin/view/CMS/MuonUL2016#RECO_efficiencies

    if(DataEra  == "2016preVFP"){
      if( eta < 1.6  ){
        if( p < 100){
          error_up=0.995227801323898;
          error_down=0.994125661987993;
          value=0.9946767316559455;
        }
        else if( p < 150){
	  error_up=0.9992971079470516;
          error_down=0.9983044687053764;
          value=0.998800788326214;
        }
        else if( p < 200){
	  error_up=1.0006919833780739;
          error_down=0.9993354675244253;
          value=1.0000137254512496;
        }
        else if( p < 300){
	  error_up=1.0003450607514124;
          error_down=0.9988239782076257;
          value=0.9995845194795191;
        }
        else if( p < 400){
	  error_up=1.0009512177465236;
          error_down=0.9974986685593457;
          value=0.9992249431529346;
        }
        else if( p < 600){
	  error_up=1.0020476967834313;
          error_down=0.9962302127919886;
          value=0.99913895478771;
        }
        else if( p < 1500){
	  error_up=1.0031575636920835;
          error_down=0.9832126701900217;
          value=0.9931851169410526;
	}
        else if( p < 3500){
	  error_up=0.7135634730037197;
          error_down=0.09926689577708875;
          value=0.40641518439040425;
        }
      }
      else if( eta < 2.4  ){
	if( p < 100){
          error_up=1.;
          error_down=1.;
          value=1.;
        }
        else if( p < 150){
	  error_up=0.9960360524184441;
          error_down=0.9916217541150506;
          value=0.9938289032667473;
        }
        else  if( p < 200){
	  error_up=0.9955722693414139;
          error_down=0.994016476410681;
          value=0.9947943728760474;
        }
        else  if( p < 300){
	  error_up=0.9904363039717452;
          error_down=0.9890618258283379;
          value=0.9897490649000416;
        }
        else  if( p < 400){
	  error_up=0.9889311423295442;
          error_down=0.9866255818663261;
          value=0.9877783620979351;
        }
        else  if( p < 600){
	  error_up=0.9845169634353598;
          error_down=0.9813576797186575;
          value=0.9829373215770086;
        }
        else  if( p < 1500){
	  error_up=0.9819584607662794;
          error_down=0.9751102314105503;
          value=0.9785343460884148;
        }
        else  if( p < 3500){
	  error_up=0.9289459343427907;
          error_down=0.8062601657488716;
          value=0.8676030500458312;
        }
      }
    }

    if(DataEra =="2016postVFP"){
      if( eta < 1.6  ){
        if( p < 100){      
	  error_up=0.9952458090969507;
          error_down=0.9942774651685373;
          value=0.994761637132744;
	}
        else if( p < 150){
	  error_up=0.9984969334216286;
          error_down=0.997527833503865;
          value=0.9980123834627468;
        }
        else if( p < 200){ 
	  error_up=1.0003081580613284;
          error_down=0.9990817187103893;
          value=0.9996949383858589;
        }
        else if( p < 300){ 
	  error_up=1.0014241464145324;
          error_down=1.0001284047735537;
          value=1.000776275594043;
        }
        else if( p < 400){
	  error_up=1.000115661673285;
          error_down=0.9974618254931225;
          value=0.9987887435832037;
	}
        else if( p < 600){ 
	  error_up=1.0066094560103684;
          error_down=1.0026120834858474;
          value=1.004610769748108;
	}
        else if( p < 1500){
	  error_up=0.9946914547731844;
          error_down=0.9781850693240632;
          value=0.9864382620486238;
      }
        else if( p < 3500){ 
	  error_up=0.9172845394883645;
          error_down=0.30264389466787656;
          value=0.6099642170781205;
	}
      }
      else if( eta < 2.4  ){
        if( p < 100){ 
	  error_up=1.3675000000000002;
          error_down=0.6325;
          value=1.0;
	}
        else  if( p < 150){ 
	  error_up=0.9961574992167305;
          error_down=0.9914378558504254;
          value=0.993797677533578;
	}
        else  if( p < 200){
	  error_up=0.9910038694607181;
          error_down=0.989474909446801;
          value=0.9902393894537596;
	}
        else  if( p < 300){  
	  error_up=0.9872890297959525;
          error_down=0.9858709806954036;
          value=0.986580005245678;
	}
        else  if( p < 400){    
	  error_up=0.9859852399421813;
          error_down=0.9837357831810145;
          value=0.9848605115615979;
	}
        else  if( p < 600){
	  error_up=0.9787932077956544;
          error_down=0.9757496435099909;
          value=0.9772714256528227;
	}
        else  if( p < 1500){
	  error_up=0.9731578694891617;
          error_down=0.967194930594714;
          value=0.9701764000419378;
	}
	else  if( p < 3500){  
	  error_up=0.9428604681642978;
          error_down=0.8537608822977072;
          value=0.8983106752310025;
	}
      }
    }

    else if(DataEra=="2017"){
      if( eta < 1.6  ){
        if( p < 100){      
	  error_up=0.9948795406366364;
          error_down=0.9936900729345366;
          value=0.9942848067855865;
	}
        else if( p < 150){ 
	  error_up=0.9977473776473309;
          error_down=0.9966262399225394;
          value=0.9971868087849352;
        }
        else if( p < 200){ 
	  error_up=0.9999019988337379;
          error_down=0.9983973791806195;
          value=0.9991496890071787;
        }
        else if( p < 300){ 
	  error_up=1.0011180886507811;
          error_down=0.9997957595397501;
          value=1.0004569240952657;
        }
        else if( p < 400){
	  error_up=1.0003360542823345;
          error_down=0.9980258419730407;
          value=0.9991809481276875;
	}
        else if( p < 600){
	  error_up=1.0010455523435753;
          error_down=0.9978320157692674;
          value=0.9994387840564214;
	}
        else if( p < 1500){
	  error_up=1.0005913340427095;
          error_down=0.9921529862255839;
          value=0.9963721601341468;
	}
        else if( p < 3500){ 
	  error_up=0.8195883888830711;
          error_down=0.40398213663456756;
          value=0.6117852627588193;
	}
      }
      else if( eta < 2.4  ){
        if( p < 100){ 
	  error_up=1.6;
          error_down=0.4;
          value=1;
	}
        else  if( p < 150){
	  error_up=0.9939986228182041;
          error_down=0.9877430617098582;
          value=0.9908708422640311;
	}
        else  if( p < 200){
	  error_up=0.9883758813162412;
          error_down=0.9858267294813762;
          value=0.9871013053988087;
	}
        else  if( p < 300){ 
	  error_up=0.9873557088250168;
          error_down=0.9853593609294634;
          value=0.9863575348772401;
	}
        else  if( p < 400){ 
	  error_up=0.9821041844954014;
          error_down=0.9795758826153608;
          value=0.9808400335553811;
	}
        else  if( p < 600){ 
	  error_up=0.9815896653733617;
          error_down=0.9786751767048915;
          value=0.9801324210391266;
	}
        else  if( p < 1500){   
	  error_up=0.9780596921978489;
          error_down=0.9736188827735329;
          value=0.9758392874856909;
	}
	else  if( p < 3500){ 
	  error_up=0.9693729392576922;
          error_down=0.9186720565856947;
          value=0.9440224979216935;
	}
      }
    }
    else if(DataEra=="2018"){
      if( eta < 1.6  ){
        if( p < 100){
          error_up=0.9960185764743364;
          error_down=0.9949202552228559;
          value=0.9954694158485962;
        }
        else if( p < 150){
          error_up=0.9980450760959432;
          error_down=0.9969556392097965;
          value=0.9975003576528698;
        }
        else if( p < 200){
          error_up=0.9993621637394963;
          error_down=0.9979759356484939;
          value=0.9986690496939951;
        }
        else if( p < 300){
          error_up=0.9986443075093399;
          error_down=0.9974570852428604;
          value=0.9980506963761001;
        }
        else if( p < 400){
          error_up=0.9990211860548934;
          error_down=0.9969010814976738;
          value=0.9979611337762836;
        }
        else if( p < 600){
          error_up=1.0010239170101964;
          error_down=0.99822553959427;
          value=0.9996247283022333;
        }
        else if( p < 1500){
          error_up=0.9965751142706425;
          error_down=0.9889760274242743;
          value=0.9927755708474584;
        }
        else if( p < 3500){
          error_up=1.0357738580360492;
          error_down=0.7314880389782692;
          value=0.8836309485071592;
        }
      }
      else if( eta < 2.4  ){
        if( p < 100){
          error_up=1.8399999999999999;
          error_down=0.16000000000000003;
          value=1.0;
        }
        else  if( p < 150){
          error_up=0.9989776561237231;
          error_down=0.9906878406602764;
          value=0.9948327483919998;
        }
        else  if( p < 200){
          error_up=0.9904796219870464;
          error_down=0.9882286893690275;
          value=0.989354155678037;
        }
        else  if( p < 300){
          error_up=0.9847926661903523;
          error_down=0.982960800497906;
          value=0.9838767333441292;
        }
        else  if( p < 400){
          error_up=0.9818963616444432;
          error_down=0.9794762086336957;
          value=0.9806862851390694;
        }
        else  if( p < 600){
          error_up=0.9785028851501875;
          error_down=0.9757961539897776;
          value=0.9771495195699825;
        }
        else  if( p < 1500){
          error_up=0.9746258589475912;
          error_down=0.9704134714564226;
          value=0.9725196652020069;
        }
        else  if( p < 3500){
          error_up=0.8613748500937793;
          error_down=0.8065308683508711;
          value=0.8339528592223252;
        }
      }

    }



  }
  else   if(key=="LowPMuonRecoSF"){
    
    // https://gitlab.cern.ch/cms-muonPOG/muonefficiencies/-/blob/master/Run2/UL/2018/Efficiency_muon_generalTracks_Run2018_UL_trackerMuon.json

    if(p > 40) p = 39;
    if(eta>=2.4) eta = 2.39;
    
    

    if(DataEra=="2016preVFP"){
      if( eta < 0.90  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=0.972383324835;
	  error=0.094795110538;
	}
	else if( p < 3.75){
	  value=0.962771373003;
	  error=0.0409698988938;
	}
	else if( p < 4.00){
	  value=0.993506549937;
	  error=0.0292235049138;
	}
	else if( p < 4.50){
	  value=1.01454503854;
	  error=0.0136417092986;
	}
	else if( p < 5.00){
	  value=0.994616531568;
	  error=0.0147002648006;
	}
	else if( p < 6.00){
	  value=1.00003946132;
	  error=0.00934252306154;
	}
	else if( p < 8.00){
	  value=0.998933312831;
	  error=0.00713380459472;
	}
	else if( p < 10.00){
	  value=1.00154814627;
	  error=0.00803338918012;
	}
	else if( p < 15.00){
	  value=1.00434363885;
	  error=0.00562471609649;
	}
	else if( p < 20.00){
	  value=0.996417829921;
	  error=0.0136810368233;
	}
	else if( p < 30.00){
	  value=1.00566472981;
	  error=0.00621942047362;
	}
	else if( p < 40.00){
	  value=0.971616452362;
	  error=0.0478982037641;
	}
      }
      if( eta < 1.20  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=0.943209506974;
	  error=0.181523749079;
	}
	else if( p < 3.75){
	  value=0.930592527249;
	  error=0.0737530506503;
	}
	else if( p < 4.00){
	  value=0.990808598684;
	  error=0.0533955809804;
	}
	else if( p < 4.50){
	  value=0.942421973125;
	  error=0.0321387812657;
	}
	else if( p < 5.00){
	  value=1.01547360976;
	  error=0.0176410663197;
	}
	else if( p < 6.00){
	  value=1.01066507031;
	  error=0.0148298526733;
	}
	else if( p < 8.00){
	  value=1.00116318855;
	  error=0.0133177093841;
	}
	else if( p < 10.00){
	  value=0.991937105081;
	  error=0.0184986952901;
	}
	else if( p < 15.00){
	  value=1.00805201331;
	  error=0.010524902975;
	}
	else if( p < 20.00){
	  value=0.984825408075;
	  error=0.0275128575606;
	}
	else if( p < 30.00){
	  value=0.994735780783;
	  error=0.0271395981484;
	}
	else if( p < 40.00){
	  value=1.0;
	  error=0.168933618503;
	}
      }
      else if( eta < 2.10  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=1.00473791675;
	  error=0.0138260631292;
	}
	else if( p < 3.75){
	  value=1.0047603375;
	  error=0.0164149415652;
	}
	else if( p < 4.00){
	  value=1.00408524678;
	  error=0.0296613272427;
	}
	else if( p < 4.50){
	  value=0.991496844728;
	  error=0.026057788837;
	}
	else if( p < 5.00){
	  value=0.993739054514;
	  error=0.0243870649209;
	}
	else if( p < 6.00){
	  value=1.00293120745;
	  error=0.00782199447939;
	}
	else if( p < 8.00){
	  value=1.00229868512;
	  error=0.0102438679625;
	}
	else if( p < 10.00){
	  value=0.96471451631;
	  error=0.0223204245154;
	}
	else if( p < 15.00){
	  value=0.97712907813;
	  error=0.0229375566921;
	}
	else if( p < 20.00){
	  value=0.956938843743;
	  error=0.0523969199402;
	}
	else if( p < 30.00){
	  value=1.00300605362;
	  error=0.0260880460932;
	}
	else if( p < 40.00){
	  value=1;
	  error=0;
	}
      }
      else if( eta < 2.40  ){
	if( p < 2.50){
	  value=1.00206680284;
	  error=0.0645121894922;
	}
	else if( p < 2.75){
	  value=0.876477398048;
	  error=0.173014936378;
	}
	else if( p < 3.00){
	  value=0.860629507608;
	  error=0.161040832919;
	}
	else if( p < 3.25){
	  value=0.993738276906;
	  error=1.02296060477;
	}
	else if( p < 3.50){
	  value=0.920340465259;
	  error=0.158957563128;
	}
	else if( p < 3.75){
	  value=0.99999979339;
	  error=0.0871480317957;
	}
	else if( p < 4.00){
	  value=0.921297526911;
	  error=0.12822517491;
	}
	else if( p < 4.50){
	  value=0.981752384259;
	  error=0.0821554486749;
	}
	else if( p < 5.00){
	  value=0.955118121205;
	  error=0.137864260806;
	}
	else if( p < 6.00){
	  value=0.999999999902;
	  error=0.0222657617355;
	}
	else if( p < 8.00){
	  value=0.94578229235;
	  error=0.0769697086539;
	}
	else if( p < 10.00){
	  value=0.791148809953;
	  error=0.242148481368;
	}
	else if( p < 15.00){
	  value=1.00035415988;
	  error=0.065870366007;
	}
	else if( p < 20.00){
	  value=1;
	  error=0;
	}
	else if( p < 30.00){
	  value=1.0;
	  error=0.843022531493;
	}
	else if( p < 40.00){
	  value=1;
	  error=0;
	}
      }
    }
    if(DataEra=="2016postVFP"){
      if( eta < 0.90  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=1.27359283079;
	  error=0.112631126243;
	}
	else if( p < 3.75){
	  value=1.03853666589;
	  error=0.0729113038096;
	}
	else if( p < 4.00){
	  value=1.01792726636;
	  error=0.0450536667232;
	}
	else if( p < 4.50){
	  value=1.00643963371;
	  error=0.0276127827344;
	}
	else if( p < 5.00){
	  value=0.995261038031;
	  error=0.0253876672132;
	}
	else if( p < 6.00){
	  value=0.988661731113;
	  error=0.0195365865044;
	}
	else if( p < 8.00){
	  value=1.00715892901;
	  error=0.00528112352706;
	}
	else if( p < 10.00){
	  value=1.00664789176;
	  error=0.00511832147548;
	}
	else if( p < 15.00){
	  value=1.00666316862;
	  error=0.00456586162349;
	}
	else if( p < 20.00){
	  value=1.00588087274;
	  error=0.0132720664806;
	}
	else if( p < 30.00){
	  value=1.00583995415;
	  error=0.0954859119535;
	}
	else if( p < 40.00){
	  value=1.00474170214;
	  error=1.00474320372;
	}
      }
      if( eta < 1.20  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=1;
	  error=0;
	}
	else if( p < 3.75){
	  value=0.618525396988;
	  error=0.39036338589;
	}
	else if( p < 4.00){
	  value=0.946916641501;
	  error=0.11251015088;
	}
	else if( p < 4.50){
	  value=1.01618476821;
	  error=0.0281056554737;
	}
	else if( p < 5.00){
	  value=0.931048476722;
	  error=0.0839023120055;
	}
	else if( p < 6.00){
	  value=1.00989733933;
	  error=0.0505548032736;
	}
	else if( p < 8.00){
	  value=1.0070275084;
	  error=0.0225683236529;
	}
	else if( p < 10.00){
	  value=1.0062041003;
	  error=0.043542102611;
	}
	else if( p < 15.00){
	  value=1.00990977943;
	  error=0.0827463681032;
	}
	else if( p < 20.00){
	  value=1.00652900073;
	  error=0.116758844301;
	}
	else if( p < 30.00){
	  value=1.00471199305;
	  error=0.310671867853;
	}
	else if( p < 40.00){
	  value=1;
	  error=0;
	}
      }
      else if( eta < 2.10  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=0.945572524069;
	  error=0.0905112351118;
	}
	else if( p < 3.75){
	  value=1.00466553289;
	  error=0.0286419287425;
	}
	else if( p < 4.00){
	  value=0.991255659043;
	  error=0.0363592687939;
	}
	else if( p < 4.50){
	  value=0.96120014501;
	  error=0.0551508708951;
	}
	else if( p < 5.00){
	  value=1.00352085435;
	  error=0.0190312088681;
	}
	else if( p < 6.00){
	  value=1.00282366674;
	  error=0.0252218243278;
	}
	else if( p < 8.00){
	  value=1.0022434489;
	  error=0.0146358097292;
	}
	else if( p < 10.00){
	  value=0.984268691396;
	  error=0.0296320654136;
	}
	else if( p < 15.00){
	  value=1.0045166286;
	  error=0.0354427719803;
	}
	else if( p < 20.00){
	  value=1.0031376418;
	  error=0.155256533333;
	}
	else if( p < 30.00){
	  value=1.00236853141;
	  error=0.221728002133;
	}
	else if( p < 40.00){
	  value=1;
	  error=0;
	}
      }
      else if( eta < 2.40  ){
	if( p < 2.50){
	  value=1.00310164197;
	  error=0.155266232581;
	}
	else if( p < 2.75){
	  value=1.00150805936;
	  error=0.171325247118;
	}
	else if( p < 3.00){
	  value=1.00209532018;
	  error=0.295934854793;
	}
	else if( p < 3.25){
	  value=1.00108441938;
	  error=0.206677746607;
	}
	else if( p < 3.50){
	  value=1.00149760867;
	  error=0.265679446617;
	}
	else if( p < 3.75){
	  value=1.00139976595;
	  error=0.265793533922;
	}
	else if( p < 4.00){
	  value=1.00157390762;
	  error=0.525138116826;
	}
	else if( p < 4.50){
	  value=1.00049408391;
	  error=0.18919255742;
	}
	else if( p < 5.00){
	  value=1.00115906208;
	  error=0.460722385339;
	}
	else if( p < 6.00){
	  value=1.00026210847;
	  error=0.232263758752;
	}
	else if( p < 8.00){
	  value=1.00048483953;
	  error=0.168945122039;
	}
	else if( p < 10.00){
	  value=1.00022453496;
	  error=0.663378813999;
	}
	else if( p < 15.00){
	  value=1;
	  error=0;
	}
	else if( p < 20.00){
	  value=1;
	  error=0;
	}
	else if( p < 30.00){
	  value=1;
	  error=0;
	}
	else if( p < 40.00){
	  value=1;
	  error=0;
	}
      }
    }
    if(DataEra=="2017"){
      if( eta < 0.90  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=0.961940117477;
	  error=0.0337106966894;
	}
	else if( p < 3.75){
	  value=0.998356792982;
	  error=0.0102826049619;
	}
	else if( p < 4.00){
	  value=0.995013489654;
	  error=0.00690953181017;
	}
	else if( p < 4.50){
	  value=0.992836351654;
	  error=0.00374909135108;
	}
	else if( p < 5.00){
	  value=0.998488580421;
	  error=0.00306968294404;
	}
	else if( p < 6.00){
	  value=0.997315278304;
	  error=0.00205957246766;
	}
	else if( p < 8.00){
	  value=0.998259855261;
	  error=0.00163079442348;
	}
	else if( p < 10.00){
	  value=0.999830740405;
	  error=0.00175120297043;
	}
	else if( p < 15.00){
	  value=1.00115488195;
	  error=0.00160224642041;
	}
	else if( p < 20.00){
	  value=1.00007283521;
	  error=0.00271120169642;
	}
	else if( p < 30.00){
	  value=0.996738551594;
	  error=0.00441464105224;
	}
	else if( p < 40.00){
	  value=1.00380411588;
	  error=0.00857571895658;
	}
      }
      if( eta < 1.20  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=1.03823375786;
	  error=0.0636970163346;
	}
	else if( p < 3.75){
	  value=1.02955318854;
	  error=0.0563921118297;
	}
	else if( p < 4.00){
	  value=0.982478319852;
	  error=0.0150823448802;
	}
	else if( p < 4.50){
	  value=0.993390596097;
	  error=0.00740513018417;
	}
	else if( p < 5.00){
	  value=0.998284128449;
	  error=0.00634737429884;
	}
	else if( p < 6.00){
	  value=0.998688670698;
	  error=0.00437430271673;
	}
	else if( p < 8.00){
	  value=0.999107511874;
	  error=0.00322220810445;
	}
	else if( p < 10.00){
	  value=0.995666939592;
	  error=0.00432898951003;
	}
	else if( p < 15.00){
	  value=1.00334635593;
	  error=0.00460833946614;
	}
	else if( p < 20.00){
	  value=1.00468121543;
	  error=0.00737208937539;
	}
	else if( p < 30.00){
	  value=1.00153074394;
	  error=0.0100323912114;
	}
	else if( p < 40.00){
	  value=1.00394359458;
	  error=0.155456591783;
	}
      }
      else if( eta < 2.10  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=0.993304933123;
	  error=0.00652620422029;
	}
	else if( p < 3.75){
	  value=0.990857056404;
	  error=0.00704652129738;
	}
	else if( p < 4.00){
	  value=0.992049635375;
	  error=0.00639087307322;
	}
	else if( p < 4.50){
	  value=0.995596833051;
	  error=0.00406035505111;
	}
	else if( p < 5.00){
	  value=0.998872078081;
	  error=0.00425769219759;
	}
	else if( p < 6.00){
	  value=0.9983142294;
	  error=0.00303257067061;
	}
	else if( p < 8.00){
	  value=1.00101546789;
	  error=0.00193091661228;
	}
	else if( p < 10.00){
	  value=0.998225417108;
	  error=0.00305507493419;
	}
	else if( p < 15.00){
	  value=0.99135346022;
	  error=0.00383391873523;
	}
	else if( p < 20.00){
	  value=0.996862354953;
	  error=0.00736605280079;
	}
	else if( p < 30.00){
	  value=0.999444309416;
	  error=0.00830189983495;
	}
	else if( p < 40.00){
	  value=1.01123860237;
	  error=0.312729566197;
	}
      }
      else if( eta < 2.40  ){
	if( p < 2.50){
	  value=1.00393726596;
	  error=0.0120741390017;
	}
	else if( p < 2.75){
	  value=0.976390459301;
	  error=0.0333295542621;
	}
	else if( p < 3.00){
	  value=1.00163926168;
	  error=0.00462764037864;
	}
	else if( p < 3.25){
	  value=1.0013976167;
	  error=0.0101862745879;
	}
	else if( p < 3.50){
	  value=0.991336829112;
	  error=0.0225348949676;
	}
	else if( p < 3.75){
	  value=0.981760882338;
	  error=0.0249064064798;
	}
	else if( p < 4.00){
	  value=0.966670531783;
	  error=0.0304792484786;
	}
	else if( p < 4.50){
	  value=1.00112322034;
	  error=0.00298322352412;
	}
	else if( p < 5.00){
	  value=0.995032495847;
	  error=0.0150514252857;
	}
	else if( p < 6.00){
	  value=0.997992430803;
	  error=0.00826198541347;
	}
	else if( p < 8.00){
	  value=1.00035082588;
	  error=0.00471208796501;
	}
	else if( p < 10.00){
	  value=0.99372076853;
	  error=0.0112046165142;
	}
	else if( p < 15.00){
	  value=1.00161272379;
	  error=0.00974718192971;
	}
	else if( p < 20.00){
	  value=1.00080456837;
	  error=0.866615767913;
	}
	else if( p < 30.00){
	  value=1.00000000093;
	  error=0.842931396664;
	}
	else if( p < 40.00){
	  value=1;
	  error=0;
	}
      }
    }
    if(DataEra=="2018"){
      if( eta < 0.90  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=1.2310665992;
	  error=0.137984963045;
	}
	else if( p < 3.75){
	  value=1.04962554301;
	  error=0.0446410396235;
	}
	else if( p < 4.00){
	  value=1.03795215667;
	  error=0.0212854997878;
	}
	else if( p < 4.50){
	  value=1.02457867275;
	  error=0.00910099116431;
	}
	else if( p < 5.00){
	  value=1.00359235777;
	  error=0.00959257153166;
	}
	else if( p < 6.00){
	  value=1.00209673746;
	  error=0.00606391471304;
	}
	else if( p < 8.00){
	  value=0.989454279859;
	  error=0.00633700559473;
	}
	else if( p < 10.00){
	  value=1.00026832782;
	  error=0.00586300474842;
	}
	else if( p < 15.00){
	  value=1.00376550296;
	  error=0.00499739865085;
	}
	else if( p < 20.00){
	  value=1.00159431525;
	  error=0.00659954170383;
	}
	else if( p < 30.00){
	  value=1.00046496919;
	  error=0.00815979942403;
	}
	else if( p < 40.00){
	  value=1.00450328406;
	  error=0.105899825487;
	}
      }
      if( eta < 1.20  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=1.09912096282;
	  error=0.377686298035;
	}
	else if( p < 3.75){
	  value=0.969940092837;
	  error=0.0772491685647;
	}
	else if( p < 4.00){
	  value=0.971352048958;
	  error=0.0555540057875;
	}
	else if( p < 4.50){
	  value=1.01281527952;
	  error=0.0200590510385;
	}
	else if( p < 5.00){
	  value=1.01427307848;
	  error=0.00893393471524;
	}
	else if( p < 6.00){
	  value=1.01055599635;
	  error=0.00630763162163;
	}
	else if( p < 8.00){
	  value=1.00153901707;
	  error=0.0104782695042;
	}
	else if( p < 10.00){
	  value=1.00575160156;
	  error=0.00686279492734;
	}
	else if( p < 15.00){
	  value=1.0014223328;
	  error=0.0148490357889;
	}
	else if( p < 20.00){
	  value=1.00948508671;
	  error=0.0173955906654;
	}
	else if( p < 30.00){
	  value=1.00592195903;
	  error=0.0370834915232;
	}
	else if( p < 40.00){
	  value=1.00399152695;
	  error=0.310666001321;
	}
      }
      else if( eta < 2.10  ){
	if( p < 2.50){
	  value=1;
	  error=0;
	}
	else if( p < 2.75){
	  value=1;
	  error=0;
	}
	else if( p < 3.00){
	  value=1;
	  error=0;
	}
	else if( p < 3.25){
	  value=1;
	  error=0;
	}
	else if( p < 3.50){
	  value=0.967148565412;
	  error=0.0339607152553;
	}
	else if( p < 3.75){
	  value=1.00423295572;
	  error=0.0106346377961;
	}
	else if( p < 4.00){
	  value=1.00366923547;
	  error=0.0159661321833;
	}
	else if( p < 4.50){
	  value=1.00423151363;
	  error=0.00515869673557;
	}
	else if( p < 5.00){
	  value=0.991708609291;
	  error=0.0169604064769;
	}
	else if( p < 6.00){
	  value=1.00288064981;
	  error=0.00441960481445;
	}
	else if( p < 8.00){
	  value=1.00208665548;
	  error=0.00619953953918;
	}
	else if( p < 10.00){
	  value=1.00169067966;
	  error=0.00174596779379;
	}
	else if( p < 15.00){
	  value=0.9986517754;
	  error=0.00996366084949;
	}
	else if( p < 20.00){
	  value=0.981362308636;
	  error=0.0244163658474;
	}
	else if( p < 30.00){
	  value=1.00423756178;
	  error=0.0629848725121;
	}
	else if( p < 40.00){
	  value=1.00207384822;
	  error=0.389923444104;
	}
      }
      else if( eta < 2.40  ){
	if( p < 2.50){
	  value=1.00449512342;
	  error=0.0247106395089;
	}
	else if( p < 2.75){
	  value=1.00401146604;
	  error=0.0981516782583;
	}
	else if( p < 3.00){
	  value=0.924288418358;
	  error=0.0830067550683;
	}
	else if( p < 3.25){
	  value=1.00216828309;
	  error=0.0166275616383;
	}
	else if( p < 3.50){
	  value=0.979243963244;
	  error=0.0630539795812;
	}
	else if( p < 3.75){
	  value=1.00085928647;
	  error=0.173791204108;
	}
	else if( p < 4.00){
	  value=1.00085737266;
	  error=0.0290962169464;
	}
	else if( p < 4.50){
	  value=1.00134104701;
	  error=0.0134999319026;
	}
	else if( p < 5.00){
	  value=1.00086686796;
	  error=0.0214402988827;
	}
	else if( p < 6.00){
	  value=1.00128273463;
	  error=0.0139318151378;
	}
	else if( p < 8.00){
	  value=1.00098369172;
	  error=0.00738503131934;
	}
	else if( p < 10.00){
	  value=1.00082382289;
	  error=0.0637165711514;
	}
	else if( p < 15.00){
	  value=1.00151429069;
	  error=0.26568472759;
	}
	else if( p < 20.00){
	  value=1;
	  error=0;
	}
	else if( p < 30.00){
	  value=1.0;
	  error=0.842718108194;
	}
	else if( p < 40.00){
	  value=1;
	  error=0;
	}
      }
    }
  }


  else if(key == "MuonRecoSF"){
  
      //==== boundaries                                                                                                                                                
    if(p<10.) p = 10.;
    if(p>=60.) p = 59.;
    if(eta>=2.4) eta = 2.39;

    if(DataEra=="2016preVFP"){
      if( eta < 0.9  ){
	if( p < 60){
	  error=0.000354089739933;
	  value=0.99982295513;
	}
      }
      else if( eta < 1.2  ){
	if( p < 60){
	  error=0.000310245921391;
	  value=1.00015934169;
	}
      }
      else if( eta < 2.1  ){
	if( p < 60){
	  error=0.000212771198785;
	  value=0.999893614401;
	}
      }
      else if( eta < 2.4  ){
	if( p < 60){
	  error=0.00194623599145;
	  value=0.999026882004;
	}
      }
    }
    if(DataEra=="2016postVFP"){
      if( eta < 0.9  ){
	if( p < 60){
	  error=0.00143669276524;
	  value=1.00004064198;
	}
      }
      else if( eta < 1.2  ){
	if( p < 60){
	  error=0.00109178573431;
	  value=0.999795931115;
	}
      }
      else if( eta < 2.1  ){
	if( p < 60){
	  error=0.00148146540329;
	  value=0.999492840057;
	}
      }
      else if( eta < 2.4  ){
	if( p < 60){
	  error=0.00173647787446;
	  value=0.999072861951;
	}
      }
    }
    if(DataEra=="2017"){
      if( eta < 0.9  ){
	if( p < 60){
	  error=0.000651487438728;
	  value=0.999674256281;
	}
      }
      else if( eta < 1.2  ){
	if( p < 60){
	  error=0.000437279592853;
	  value=0.999781360204;
	}
      }
      else if( eta < 2.1  ){
	if( p < 60){
	  error=0.00106505150809;
	  value=0.999467474246;
	}
      }
      else if( eta < 2.4  ){
	if( p < 60){
	  error=0.00118109622227;
	  value=0.999356641263;
	}
      }
    }
    if(DataEra=="2018"){
      if( eta < 0.9  ){
	if( p < 60){
	  error=0.000382398736862;
	  value=0.999808800632;
	}
      }
      else if( eta < 1.2  ){
	if( p < 60){
	  error=0.000522112423093;
	  value=0.99975470198;
	}
      }
      else if( eta < 2.1  ){
	if( p < 60){
	  error=0.000831441627577;
	  value=0.999584279186;
	}
      }
      else if( eta < 2.4  ){
	if( p < 60){
	  error=0.00172374082924;
	  value=0.999034174161;
	}
      }
    }


  }
  //https://gitlab.cern.ch/cms-muonPOG/muonefficiencies/-/blob/master/Run2/UL/2018/Efficiency_muon_generalTracks_Run2018_UL_trackerMuon.json  

  if(value > 0) {
    if(key=="HighPtMuonRecoSF" || (key=="MuonRecoSF" && p > 200))   {
      if(sys == 0) return value;
      if(sys > 0) return error_up;
      if(sys < 0) return error_down;
    }
    else return value+double(sys)*error;
  }

  TH2 *this_hist = map_hist_Muon["RECO_SF_"+key];
  if(!this_hist){
    if(IgnoreNoHist) {

      TString MapK = "RECO_SF_"+key;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;

    }
    else{
      cerr << "[MCCorrection::MuonReco_SF] No "<<"RECO_SF_"+key<<endl;
      exit(ENODATA);
    }
  }

  int this_bin = this_hist->FindBin(p, eta);
  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);

  //cout << "[MCCorrection::MuonReco_SF] --> value = " << value << "\t" << ", error = " << error << endl;

  return value+double(sys)*error;

}

double MCCorrection::MuonTracker_SF(TString ID, double eta, double pt, int sys){

  if(ID=="Default") return 1.;
  double value = 1.;  double error = 0.;

  if (pt < 25) pt = 25.1;
  if (pt > 65) pt = 64.1;
  
  TH2 *this_hist = map_hist_Muon["Tracker_SF_"+ID];
  if(!this_hist){
    if(IgnoreNoHist) {

      TString MapK = "Tracker_SF_"+ID;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;

    }
    else{
      exit(ENODATA);
    }
  }
  
  int this_bin = this_hist->FindBin(eta,pt);
  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);
  
  return value+double(sys)*error;
  
}

double MCCorrection::MuonID_SF(TString ID, double eta, double pt, int sys){

  if(ID=="Default") return 1.;
  if(ID=="-") return 1.;

  //cout << "[MCCorrection::MuonID_SF] ID = " << ID << endl;
  //cout << "[MCCorrection::MuonID_SF] eta = " << eta << ", pt = " << pt << endl;

  //// Set Eta as |eta|
  eta = fabs(eta);

  double ptmin=10.1, ptmax=119., etamin=0.01, etamax=2.39;

  if(ID.Contains("HNL_ULID"))  {
    ptmin=10.1, ptmax=999., etamin=0.01, etamax=2.39;
  }
  else  if(ID.Contains("HNTight")) {
    ptmin=10.1, ptmax=119., etamin=0.01, etamax=2.39;
  }
  else   if(ID=="NUM_TightID_DEN_TrackerMuons" || ID=="NUM_MediumID_DEN_TrackerMuons" || ID=="NUM_HighPtID_DEN_TrackerMuons"){
    //==== boundaries

    ptmin=15.1, ptmax=119., etamin=0.01, etamax=2.39;
  }
  else if(ID.Contains("HNTop")){ 
    ptmin=10.1, ptmax=199., etamin=0.01, etamax=2.39; 
  }

  
  pt  = std::min(std::max(pt,  ptmin),  ptmax);
  eta = std::min(std::max(eta, etamin), etamax);
  
  // Central SF histogram
  TH2 *this_hist = map_hist_Muon["ID_SF_" + ID];
  
  if (!this_hist) {
    if (IgnoreNoHist) {
      TString MapK = "ID_SF_" + ID;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK) == MissingHists.end())
	MissingHists.push_back(MapK);
      return 1.0;
    } else {
      std::cerr << "[MCCorrection::MuonID_SF] No ID_SF_" << ID << std::endl;
      std::exit(ENODATA);
    }
  }
  
  
  // Find bin (assumed same binning)
  int this_bin = this_hist->FindBin(eta, pt);
  
  double value     = this_hist->GetBinContent(this_bin);
  double error_tot = this_hist->GetBinError(this_bin); // total (stat + syst)
  
  // sys == 0: return central value
  if (sys == 0) return value;
  
  // Stat-only SF histogram (sf+s0m0)                                                                                                                                                                                                                                                                                       
  TH2 *this_hist_stat = map_hist_Muon["ID_SF_Stat_" + ID];

  
  // If stat histogram missing
  if (!this_hist_stat) {
    if (IgnoreNoHist) {
      TString MapK_stat = "ID_SF_Stat_" + ID;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK_stat) == MissingHists.end())
	MissingHists.push_back(MapK_stat);
      
      // If stat map missing: cannot split stat/syst
      int sign = (sys > 0 ? 1 : -1);
      
      if (abs(sys) == 2) {
	// stat requested but unknown -> no shift
	return value ;
      }
      if (abs(sys) == 1) {
	// syst requested -> use total error
	return value + sign * error_tot;
      }
      
      return value;
    } else {
      std::cerr << "[MCCorrection::MuonID_SF] No ID_SF_stat_" << ID << std::endl;
      std::exit(ENODATA);
    }
  }
  
  // We have a stat-only histogram
  int this_bin_stat = this_hist_stat->FindBin(eta, pt);
  
  // According to you: stat-only error is in GetBinError of this_hist_stat
  double error_stat = this_hist_stat->GetBinError(this_bin_stat);
  
  // Compute syst error: error_tot^2 = error_stat^2 + error_syst^2
  double error_syst = 0.0;
  
  double diff2 = error_tot*error_tot - error_stat*error_stat;
  if (diff2 > 0.0)
    error_syst = std::sqrt(diff2);
  else
    error_syst = 0.0;
  
  // sys codes:
  //   abs(sys) == 2 -> stat up/down
  //   abs(sys) == 1 -> syst up/down
  int sign = (sys > 0 ? 1 : -1);
  
  if (abs(sys) == 2) {
    return value + sign * error_stat;
  }
  if (abs(sys) == 1) {
    return value + sign * error_syst;
  }
  
  // Fallback
  return value;
    
  

}

double MCCorrection::MuonISO_SF(TString ID, double eta, double pt, int sys){

  if(ID=="Default") return 1.;

  //cout << "[MCCorrection::MuonISO_SF] eta = " << eta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  eta = fabs(eta);


  if(ID=="NUM_TightRelIso_DEN_TightIDandIPCut" || ID=="NUM_TightRelIso_DEN_MediumID" || ID=="NUM_LooseRelTkIso_DEN_HighPtIDandIPCut"){
    //==== boundaries
    if(pt<15.) pt = 15.1;
    if(pt>=120.) pt = 119.9;
    if(eta>=2.4) eta = 2.39;
  }
  else{
    if(pt<15.) pt = 15.1;
    if(pt>=2000.) pt = 1999.;
    if(eta>=2.4) eta = 2.39;
  }
  TH2 *this_hist = map_hist_Muon["ISO_SF_"+ID];
  if(!this_hist){
    if(IgnoreNoHist) {
      TString MapK = "ISO_SF_"+ID;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;
    }
    else{
      cerr << "[MCCorrection::MuonISO_SF] No "<<"ISO_SF_"+ID<<endl;
      exit(ENODATA);
    }
  }

  int this_bin(-999);

  this_bin = this_hist->FindBin(eta,pt);
  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);

  //cout << "[MCCorrection::MuonISO_SF] value = " << value << endl;

  return value+double(sys)*error;

}

double MCCorrection::MuonTrigger_Eff(TString ID, TString trig, int DataOrMC, double eta, double pt, int sys){

  //cout << "[MCCorrection::MuonTrigger_Eff] Called" << endl;

  if(ID=="Default") return 1.;
  if(trig=="Default") return 1.;

  //cout << "[MCCorrection::MuonTrigger_Eff] DataYear = " << DataYear << endl;
  //cout << "[MCCorrection::MuonTrigger_Eff] ID = " << ID << "\t" << "trig = " << trig << endl;
  //cout << "[MCCorrection::MuonTrigger_Eff] DataOrMC = " << DataOrMC << endl;
  //cout << "[MCCorrection::MuonTrigger_Eff] eta = " << eta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  eta = fabs(eta);

  //==== 2016
  if(DataYear==2016){
    if(trig=="IsoMu24"){
      if(pt<26.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>200.) pt = 199.;
    }
    else if(trig=="Mu50"){
      if(pt<52.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;
      if(pt>200.) pt = 199.;
    }
  }
  else if(DataYear==2017){
    if(trig=="IsoMu27"){
      //==== FIXME MiniAODPt Pt
      //==== FIXME 28.9918  29.0363
      //==== FIXME This event pass pt>29GeV cut, but MiniAOD pt < 29 GeV
      //==== FIXME So when I return 0., SF goes nan.. let's return 1 for now..
      if(pt<29.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>200.) pt = 199.;
    }
    else if(trig=="Mu50"){
      if(pt<52.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>200.) pt = 199.;

    }
  }
  else if(DataYear==2018){
    if(trig=="IsoMu24"){
      if(pt<26.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>200.) pt = 199.;
    }
    else if(trig=="Mu50"){
      if(pt<52.) return 1.; //FIXME
      if(eta>=2.4) eta = 2.39;

      if(pt>200.) pt = 199.;

    }
  }
  else{
    cerr << "[MCCorrection::MuonTrigger_Eff] Wrong year : " << DataYear << endl;
    exit(ENODATA);
  }

  TString histkey = "Trigger_Eff_DATA_"+trig+"_"+ID;
  if(DataOrMC==1) histkey = "Trigger_Eff_MC_"+trig+"_"+ID;
  //cout << "[MCCorrection::MuonTrigger_Eff] histkey = " << histkey << endl;
  TH2 *this_hist = map_hist_Muon[histkey];
  if(!this_hist){
    if(IgnoreNoHist) {
      TString MapK = histkey;
      if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
      return 1.;
    }
    else{
      cerr << "[MCCorrection::MuonTrigger_Eff] No "<<histkey<<endl;
      exit(ENODATA);
    }
  }

  int this_bin = this_hist->FindBin(eta,pt);

  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);

  //cout << "[MCCorrection::MuonTrigger_Eff] value = " << value << endl;

  return value+double(sys)*error;


}


double MCCorrection::MuonTrigger_SF(TString ID, TString trig, std::vector<Lepton *> leps, int sys){

  vector<double> mu_pt;
  vector<double> mu_eta;
  for(auto ilep : leps){
    if(ilep->LeptonFlavour() != Lepton::MUON) continue;
    mu_pt.push_back(ilep->Pt());
    mu_eta.push_back(ilep->Eta());
  }

  return MuonTrigger_SF(ID, trig, mu_pt, mu_eta, sys);

}

double MCCorrection::MuonTrigger_SF(TString ID, TString trig, const std::vector<Muon>& muons, int sys){

  vector<double> mu_pt;
  vector<double> mu_eta;
  for(auto ilep : muons){
    mu_pt.push_back(ilep.MiniAODPt());
    mu_eta.push_back(ilep.Eta());
  }

  return MuonTrigger_SF(ID, trig, mu_pt, mu_eta, sys);

}


double MCCorrection::MuonTrigger_SF(TString ID, TString trig, vector<double> mu_pt, vector<double> mu_eta, int sys){

  if(mu_pt.size() == 0) return 1.;

  if(ID=="Default") return 1.;
  if(trig=="Default") return 1.;

  double value = 1.;

  if(trig=="IsoMu24" || trig=="IsoMu27" || trig=="Mu50"){

    double eff_DATA = 1.;
    double eff_MC = 1.;

    for(unsigned int i=0; i< mu_pt.size(); i++){
      eff_DATA *= ( 1.-MuonTrigger_Eff(ID, trig, 0, mu_eta[i], mu_pt[i], sys) );
      eff_MC   *= ( 1.-MuonTrigger_Eff(ID, trig, 1, mu_eta[i], mu_pt[i], -sys) );
    }

    eff_DATA = 1.-eff_DATA;
    eff_MC = 1.-eff_MC;

    value = eff_DATA/eff_MC;

/*
    if(eff_DATA==0||eff_MC==0){
      cout << "==== Zero Trigger Eff ====" << endl;
      for(unsigned int i=0;i<muons.size();i++){
        cout << muons.at(i).MiniAODPt() << "\t" << muons.at(i).Pt() << endl;
      }
    }
*/

  }

  return value;

}

double MCCorrection::MuonTrigger_SF(TString ID, TString trig, const std::vector<Muon *>& muons, int sys){

  if(muons.size() == 0) return 1.;

  std::vector<Muon> muvec;
  for(unsigned int i=0; i<muons.size(); i++){
    Muon this_muon = *(muons.at(i));
    muvec.push_back( this_muon );
  }

  return MuonTrigger_SF(ID, trig, muvec, sys);

}

double MCCorrection::ElectronID_SF(TString ID, double sceta, double pt, int sys){

  if(ID=="Default") return 1.;
  if(ID=="-") return 1.;

  double value = 1.;

  if(pt<15.) pt = 15.1; //JH : HOTFIX for case electron ID SF doesn't have entry below 15 GeV
  if(pt>=500.) pt = 499.9;
  if(sceta>=2.5) sceta = 2.49;
  if(sceta<-2.5) sceta = -2.49;

  if( ID.Contains("TopHNSST") ){ pt = max(pt,(double) 15.1); pt = min(pt,(double) 199.); }
  if( ID.Contains("HNL_ULID") ){ pt = max(pt,(double) 15.1); pt = min(pt,(double) 499.); }
  if( ID.Contains("HNTightV2") ){ pt = max(pt,(double) 15.1); pt = min(pt,(double) 499.); }

  if( ID.Contains("HEEP") ){

    //==== https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaRunIIRecommendations#HEEPV7_0
    //==== summary: a simple robust ID designed to be safe for high electrons.
    //==== The Et evolution of this ID must be well described in the MC therefore this ID is designed so its scale factor is flat vs Et.
    //==== As a result the HEEP differs in that it provides just a single number for the barrel and a single number for the endcap.
    //==== * note there almost certainly will have to be a retune for 2018 due to HCAL data/MC disagreements
    //==== * 2018 prompt: expected Dec 2018

    bool IsBarrel = fabs(sceta) < 1.479;
    double this_SF(1.);
    double this_SF_staterr(0.); // absolute value
    double this_SF_systerr(0.); // absolute value
    double this_SF_err(0.);

    if(DataYear==2016){
      this_SF         = (IsBarrel ? 0.971 : 0.983);
      this_SF_staterr = (IsBarrel ? 0.001 : 0.001);

      if(IsBarrel) this_SF_systerr = (pt<90. ? 0.01 : min(1.+(pt-90.)*0.0022,3.)*0.01) * this_SF;
      else         this_SF_systerr = (pt<90. ? 0.01 : min(1.+(pt-90.)*0.0143,4.)*0.01) * this_SF;

      this_SF_err = sqrt(this_SF_staterr*this_SF_staterr+this_SF_systerr*this_SF_systerr);
    }
    else if(DataYear==2017){ 
      this_SF         = (IsBarrel ? 0.967 : 0.973);
      this_SF_staterr = (IsBarrel ? 0.001 : 0.002);
      
      if(IsBarrel) this_SF_systerr = (pt<90. ? 0.01 : min(1.+(pt-90.)*0.0022,3.)*0.01) * this_SF;
      else         this_SF_systerr = (pt<90. ? 0.02 : min(1.+(pt-90.)*0.0143,5.)*0.01) * this_SF;
      
      this_SF_err = sqrt(this_SF_staterr*this_SF_staterr+this_SF_systerr*this_SF_systerr);
    }
    else if(DataYear==2018){

      //==== https://twiki.cern.ch/twiki/bin/viewauth/CMS/EgammaRunIIRecommendations#HEEPv7_0_2018Prompt

      this_SF         = (IsBarrel ? 0.969 : 0.984);
      this_SF_staterr = (IsBarrel ? 0.000 : 0.001);

      if(IsBarrel) this_SF_systerr = (pt<90. ? 0.01 : min(1.+(pt-90.)*0.0022,3.)*0.01) * this_SF;
      else         this_SF_systerr = (pt<90. ? 0.02 : min(1.+(pt-90.)*0.0143,5.)*0.01) * this_SF;

      this_SF_err = sqrt(this_SF_staterr*this_SF_staterr+this_SF_systerr*this_SF_systerr);
    }
    else{
      cerr << "[MCCorrection::ElectronID_SF] (Hist) Wrong year "<< DataYear << endl;
      exit(ENODATA);
    }

    return this_SF+double(sys)*this_SF_err;

  }
  else{

    if (ID.Contains("HN") && pt < 15.) pt = 15.1;
    
    TH2 *this_hist = map_hist_Electron["ID_SF_" + ID];
    if (!this_hist) {
      if (IgnoreNoHist) {
	TString MapK = "ID_SF_" + ID;
	if (std::find(MissingHists.begin(), MissingHists.end(), MapK) == MissingHists.end())
	  MissingHists.push_back(MapK);
	return 1.;
      } else {
	cerr << "[MCCorrection::ElectronID_SF] (Hist) No ID_SF_" << ID << endl;
	exit(ENODATA);
      }
    }
    
    // TMP ID variables
    sceta = fabs(sceta);
    
    // Find bin in central SF hist
    int this_bin = this_hist->FindBin(sceta, pt);
    
    value = this_hist->GetBinContent(this_bin);
    double error_tot = this_hist->GetBinError(this_bin); // total (stat + syst)

    
    // No variation requested: return central SF
    if (sys == 0) return value;
    
    // Try to get stat-only SF hist (sf+s0m0)
    TH2 *this_hist_stat = map_hist_Electron["ID_SF_Stat_" + ID];
    
    if (!this_hist_stat) {
      if (IgnoreNoHist) {
	TString MapKStat = "ID_SF_Stat_" + ID;
	if (std::find(MissingHists.begin(), MissingHists.end(), MapKStat) == MissingHists.end())
	  MissingHists.push_back(MapKStat);
	
	int sign = (sys > 0 ? 1 : -1);
	
	// If stat map is missing:
	//  - for |sys| == 2 (stat) we cannot do anything -> no shift
	//  - for |sys| == 1 (syst) use total error as syst
	if (abs(sys) == 2) {
	  return value;
	}
	if (abs(sys) == 1) {
	  return value + sign * error_tot;
	}
	
	return value;
      } else {
	cerr << "[MCCorrection::ElectronID_SF] (Hist) No ID_SF_Stat_" << ID << endl;
	exit(ENODATA);
      }
    }
    
    // We have the stat-only histogram
    int this_bin_stat = this_hist_stat->FindBin(sceta, pt);
    
    // According to your setup: stat-only error is in GetBinError of ID_SF_Stat
    double error_stat = this_hist_stat->GetBinError(this_bin_stat);
    
    // Compute syst-only error from total and stat: error_tot^2 = error_stat^2 + error_syst^2
    double error_syst = 0.0;
    double diff2 = error_tot * error_tot - error_stat * error_stat;
    if (diff2 > 0.0)
      error_syst = sqrt(diff2);
    else
      error_syst = 0.0;
    
    int sign = (sys > 0 ? 1 : -1);
    
    // |sys| == 2 -> stat variation
    if (abs(sys) == 2) {
      return value + sign * error_stat;
    }
    
    // |sys| == 1 -> syst variation
    if (abs(sys) == 1) {
      return value + sign * error_syst;
    }
    
    // Fallback for unexpected sys codes
    return value;
    
  }

}

double MCCorrection::ElectronReco_SF(TString key, double sceta, double pt, int sys){

  double value = 1.;
  double error = 0.;
  
  if(key == "Default") return 1.;
  
  if(key == "RECO_SF") {
    TString ptrange = "ptgt20";
    if(pt<20.) ptrange = "ptlt20";
    
    if(pt<10.) pt = 10.;
    if(pt>=500.) pt = 499.;
    if(sceta>=2.5) sceta = 2.49;
    if(sceta<-2.5) sceta = -2.5;
    
    
    // Adjust ptrange, sfKey, eff keys and stat keys to your actual names
    TString sfKey       = "RECO_SF_"       + ptrange;      // EGamma_SF_2D
    TString effMcKey    = "RECO_EFF_MC_"   + ptrange;
    TString effDataKey  = "RECO_EFF_DATA_" + ptrange;
    TString statMcKey   = "RECO_STAT_MC_"  + ptrange;      // bin content = abs stat error in MC eff
    TString statDataKey = "RECO_STAT_DATA_"+ ptrange;      // bin content = abs stat error in Data eff
    
    // Central SF histogram (EGamma_SF_2D)
    TH2 *hSF = map_hist_Electron[sfKey];
    if (!hSF) {
      if (IgnoreNoHist) {
	if (std::find(MissingHists.begin(), MissingHists.end(), sfKey) == MissingHists.end()) {
	  MissingHists.push_back(sfKey);
	}
	return 1.0; // default SF if no map
      } else {
	std::cerr << "[MCCorrection::ElectronReco_SF] No " << sfKey << std::endl;
      std::exit(ENODATA);
      }
    }
    
    // Find bin (assumes same binning for all involved histograms)
    int bin = hSF->FindBin(sceta, pt);
    
    double sfValue    = hSF->GetBinContent(bin);
    double sfTotError = hSF->GetBinError(bin); // total (stat + syst)
    
    // If no variation requested, just return central SF
    if (sys == 0) {
      return sfValue;
    }
    
    // MC, Data eff and their stat hists
    TH2 *hEffMc    = map_hist_Electron[effMcKey];
    TH2 *hEffData  = map_hist_Electron[effDataKey];
    TH2 *hStatMc   = map_hist_Electron[statMcKey];
    TH2 *hStatData = map_hist_Electron[statDataKey];
    
    // Basic safety checks on eff and stat hists
    auto checkAndMaybeRecordMissing = [&](TH2 *h, const TString &key) {
      if (h) return true;
      if (IgnoreNoHist) {
	if (std::find(MissingHists.begin(), MissingHists.end(), key) == MissingHists.end()) {
	  MissingHists.push_back(key);
	}
	return false;
      } else {
	std::cerr << "[MCCorrection::ElectronReco_SF] No " << key << std::endl;
	std::exit(ENODATA);
      }
    };
    
    bool haveEffMc    = checkAndMaybeRecordMissing(hEffMc,    effMcKey);
    bool haveEffData  = checkAndMaybeRecordMissing(hEffData,  effDataKey);
    bool haveStatMc   = checkAndMaybeRecordMissing(hStatMc,   statMcKey);
    bool haveStatData = checkAndMaybeRecordMissing(hStatData, statDataKey);
    
    double sfStatError  = 0.0;
    double sfSystError  = 0.0;
    
    if (haveEffMc && haveEffData && haveStatMc && haveStatData) {
      double eff_mc          = hEffMc->GetBinContent(bin);
      double eff_data        = hEffData->GetBinContent(bin);
      double stat_error_mc   = hStatMc->GetBinContent(bin);   // abs stat error in MC eff
      double stat_error_data = hStatData->GetBinContent(bin); // abs stat error in Data eff
      
      // Protect against zero or negative efficiencies
      if (eff_mc > 0.0 && eff_data > 0.0) {
	double rel_mc   = stat_error_mc   / eff_mc;
	double rel_data = stat_error_data / eff_data;
	
	// SF stat error from MC and data stat errors
	sfStatError = sfValue * std::sqrt(rel_mc * rel_mc + rel_data * rel_data);
	
	// Now get syst from total and stat: tot^2 = stat^2 + syst^2
	double diff2 = sfTotError * sfTotError - sfStatError * sfStatError;
	if (diff2 > 0.0) {
	  sfSystError = std::sqrt(diff2);
	} else {
	  // Numerical issues or total < stat: fall back to 0 syst
	  sfSystError = 0.0;
	}
      } else {
	// Eff is zero or negative: cannot define relative stat. Just treat total as syst.
	sfStatError = 0.0;
	sfSystError = sfTotError;
      }
    } else {
      // Missing some of the needed maps: fall back to using total error as "syst" only
      sfStatError = 0.0;
      sfSystError = sfTotError;
    }
    
    // Apply requested variation:
    // sys = +/-2 -> stat up/down
    // sys = +/-1 -> syst up/down
    if (sys == 2) {
      return sfValue + sfStatError;
    } else if (sys == -2) {
      return sfValue - sfStatError;
    } else if (sys == 1) {
      return sfValue + sfSystError;
    } else if (sys == -1) {
      return sfValue - sfSystError;
    }
    
    // Unknown sys code: just return central value
    return sfValue;
  }
    

  if(key == "RECO_SF_AFB") {

    TString ptrange = "ptgt20";

    if(pt<20.) pt = 20.;
    if(pt>=500.) pt = 499.;
    if(sceta>=2.5) sceta = 2.49;
    if(sceta<-2.5) sceta = -2.5;

    TH2 *this_hist = map_hist_Electron["RECO_AFB_SF_"+ptrange];
    if(!this_hist){
      if(IgnoreNoHist) {
	TString MapK = "RECO_AFB_SF_"+ptrange;
        if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
        return 1.;	
      }
      else{
        cerr << "[MCCorrection::ElectronReco_SF] No "<<"RECO_AFB_SF_"+ptrange<<endl;
        exit(ENODATA);
      }
    }


    int this_bin = this_hist->FindBin(sceta,pt);
    value = this_hist->GetBinContent(this_bin);
    error = this_hist->GetBinError(this_bin);

    return value+double(sys)*error;
  }


  return value+double(sys)*error;
  
}

double MCCorrection::ElectronTrigger_Eff(TString ID, TString trig, int DataOrMC, double sceta, double pt, int sys){

  if(ID=="Default") return 1.;
  if(trig=="Default") return 1.;

  //cout << "[MCCorrection::ElectronTrigger_Eff] ID = " << ID << "\t" << "trig = " << trig << endl;
  //cout << "[MCCorrection::ElectronTrigger_Eff] DataOrMC = " << DataOrMC << endl;
  //cout << "[MCCorrection::ElectronTrigger_Eff] sceta = " << sceta << ", pt = " << pt << endl;

  double value = 1.;
  double error = 0.;

  if(trig=="WREGammaTrigger"){

    if(pt<50.) pt = 50.;
    if(pt>=500.) pt = 499.;

    if(sceta<-2.5) sceta = -2.5;
    if(sceta>=2.5) sceta = 2.49;

    TString etaregion = "Barrel";
    if(fabs(sceta) > 1.566) etaregion = "EndCap";

    TString histkey = "Trigger_Eff_DATA_"+trig+"_"+ID+"_"+etaregion;
    if(DataOrMC==1) histkey = "Trigger_Eff_MC_"+trig+"_"+ID+"_"+etaregion;
    //cout << "[MCCorrection::ElectronTrigger_Eff] histkey = " << histkey << endl;
    TH2 *this_hist = map_hist_Electron[histkey];
    if(!this_hist){
      if(IgnoreNoHist) {
	TString MapK = histkey;
        if (std::find(MissingHists.begin(), MissingHists.end(), MapK ) == MissingHists.end())   MissingHists.push_back(MapK);
        return 1.;
      }
      else{
        cerr << "[MCCorrection::ElectronTrigger_Eff] No "<<histkey<<endl;
        exit(ENODATA);
      }
    }

    int this_bin = this_hist->FindBin(sceta, pt);

    value = this_hist->GetBinContent(this_bin);
    error = this_hist->GetBinError(this_bin);

  }

  //cout << "[MCCorrection::ElectronTrigger_Eff] value = " << value << endl;

  return value+double(sys)*error;

}

double MCCorrection::ElectronTrigger_SF(TString ID, TString trig, const std::vector<Electron>& electrons, int sys){

  vector<double> el_pt;
  vector<double> el_eta;
  for(auto ilep : electrons){
    el_pt.push_back(ilep.Pt());
    el_eta.push_back(ilep.scEta());
  }

  return ElectronTrigger_SF(ID, trig, el_pt, el_eta, sys);


}

double MCCorrection::ElectronTrigger_SF(TString ID, TString trig, std::vector<Lepton *> leps, int sys){

  vector<double> el_pt;
  vector<double> el_eta;
  for(auto ilep : leps){
    if(ilep->LeptonFlavour() != Lepton::ELECTRON) continue;

    el_pt.push_back(ilep->Pt());
    el_eta.push_back(ilep->Eta());
  }

  return ElectronTrigger_SF(ID, trig, el_pt, el_eta, sys);
  
}



double MCCorrection::ElectronTrigger_SF(TString ID, TString trig, vector<double> el_pt, vector<double> el_eta, int sys){

  if(ID=="Default") return 1.;
  if(trig=="Default") return 1.;

  double value = 1.;

  if(trig=="WREGammaTrigger"){

    double eff_DATA = 1.;
    double eff_MC = 1.;

    for(unsigned int i=0; i<el_pt.size(); i++){
      eff_DATA *= ( 1.-ElectronTrigger_Eff(ID, trig, 0, el_eta[i], el_pt[i], sys) );
      eff_MC   *= ( 1.-ElectronTrigger_Eff(ID, trig, 1, el_eta[i], el_pt[i], -sys) );
    }

    eff_DATA = 1.-eff_DATA;
    eff_MC = 1.-eff_MC;

    value = eff_DATA/eff_MC;
    if(IsFastSim) value = eff_DATA;


  }
  
  if(value == 0) return 1.;
  return value;

}

double MCCorrection::ElectronTrigger_SF(TString ID, TString trig, const std::vector<Electron *>& electrons, int sys){

  std::vector<Electron> muvec;
  for(unsigned int i=0; i<electrons.size(); i++){
    Electron this_electron = *(electrons.at(i));
    muvec.push_back( this_electron );
  }

  return ElectronTrigger_SF(ID, trig, muvec, sys);

}

double MCCorrection::GetPrefireWeight(const std::vector<Photon>& photons, const std::vector<Jet>& jets, int sys){

  double photon_weight = 1.;
  double jet_weight = 1.;
  
  TH2 *photon_hist = map_hist_prefire["Photon_prefire"];
  TH2 *jet_hist = map_hist_prefire["Jet_prefire"];

  
  for(unsigned int i_pho = 0; i_pho < photons.size(); i_pho++){
    Photon current_photon = photons.at(i_pho);
    double eta = current_photon.scEta();
    double pt = current_photon.Pt();
    
    int this_bin = photon_hist->FindBin(eta, pt);

    double this_eff = photon_hist->GetBinContent(this_bin);
    double this_efferr = photon_hist->GetBinError(this_bin);

    double current_weight = 1. - (this_eff + (double)sys * this_efferr );;
    photon_weight = photon_weight * current_weight;
  }
  
  for(unsigned int i_jet = 0; i_jet < jets.size(); i_jet++){
    Jet current_jet = jets.at(i_jet);
    double eta = current_jet.Eta();
    double pt = current_jet.Pt();
    
    int this_bin = jet_hist->FindBin(eta, pt);

    double this_eff = jet_hist->GetBinContent(this_bin);
    double this_efferr = jet_hist->GetBinError(this_bin);

    double current_weight = 1.- (this_eff + (double)sys * this_efferr);
    jet_weight = jet_weight * current_weight;
  }

  return photon_weight * jet_weight;;

}


double MCCorrection::GetPileUpWeightBySampleName(int N_pileup, int syst){
  
  int this_bin = N_pileup+1;
  if(N_pileup >= 100) this_bin=100;

  TString this_histname = MCSample;
  if(syst == 0){
    this_histname += "_central_pileup";
  }
  else if(syst == -1){
    this_histname += "_sig_down_pileup";
  }
  else if(syst == 1){
    this_histname += "_sig_up_pileup";
  }
  else{
    cerr << "[MCCorrection::GetPileUpWeightBySampleName] syst should be 0, -1, or +1" << endl;
    exit(ENODATA);
  }

  TH1D *this_hist = map_hist_pileup[this_histname];
  if(!this_hist){
    cerr << "[MCCorrection::GetPileUpWeightBySampleName] No " << this_histname << endl;
    exit(ENODATA);
  }

  return this_hist->GetBinContent(this_bin);

}

double MCCorrection::GetPileUpWeight(int N_pileup, int syst){

  TString this_histname = "MC_" + GetEra();
  if(syst == 0){
    this_histname += "_central_pileup";
  }
  else if(syst == -1){
    this_histname += "_sig_down_pileup";
  }
  else if(syst == 1){
    this_histname += "_sig_up_pileup";
  }
  else{
    cerr << "[MCCorrection::GetPileUpWeight] syst should be 0, -1, or +1" << endl;
    exit(ENODATA);
  }

  TH1D *this_hist = map_hist_pileup[this_histname];
  if(!this_hist){
    cerr << "[MCCorrection::GetPileUpWeight] No " << this_histname << endl;
    exit(ENODATA);
  }
  
  int this_bin = N_pileup+1;
  if(this_bin>this_hist->GetNbinsX()) this_bin=this_hist->GetNbinsX();

  double pu_weight=this_hist->GetBinContent(this_bin);
  return pu_weight;

}

double MCCorrection::GetTopPtReweight(const std::vector<Gen>& gens){
  //==== ref: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopPtReweighting2017
  //==== Only top quarks in SM ttbar events must be reweighted, 
  //==== not single tops or tops from BSM production mechanisms.
  if(!MCSample.Contains("TT") || !MCSample.Contains("powheg")){
    return 1.;
  }
  //==== initialize with large number
  double toppt1=10000, toppt2=10000;
  bool found_top = false, found_atop = false;

  for(vector<Gen>::const_iterator genit=gens.begin(); genit!=gens.end(); genit++){
    
    if(genit->Status() == 22){
      if(genit->PID() == 6){
        toppt1= genit->Pt();
        found_top = true;
      }
      else if(genit->PID() == -6){
        toppt2= genit->Pt();
        found_atop = true;
      }
    }
    //==== after we found top pair, break the loop
    if(found_top && found_atop) break;
  }
  double pt_reweight = 1.;
  //==== if top pair is not found, return 1.
  //==== the measurement covers only the range pt(top)<=800GeV, otherwise, return 1.
  if(toppt1<=800 && toppt2 <=800){
    pt_reweight*=exp(0.0615-0.0005*toppt1);
    pt_reweight*=exp(0.0615-0.0005*toppt2);
    pt_reweight = sqrt(pt_reweight);
  }
  return pt_reweight;
}

double MCCorrection::GetOfficialDYReweight(const vector<Gen>& gens, int sys){

  genFinderDY->Find(gens);
  Particle genZ = genFinderDY->GenZ;

  double mZ = genZ.M();
  double ptZ = genZ.Pt();

  int bin_mass = hist_DYPtReweight_2D->GetXaxis()->FindBin(mZ);
  int bin_pt   = hist_DYPtReweight_2D->GetYaxis()->FindBin(ptZ);

  double value = hist_DYPtReweight_2D->GetBinContent( bin_mass, bin_pt );
  double error = hist_DYPtReweight_2D->GetBinError( bin_mass, bin_pt );

  return value+double(sys)*error;

}

void MCCorrection::SetJetTaggingParameters(std::vector<JetTagging::Parameters> v){
  jetTaggingPars = v;
}

void MCCorrection::SetupJetTagging(TString Eff_File){

  if(IsDATA) return;

  TString datapath = getenv("DATA_DIR");
  TString btagpath = datapath+"/"+GetEra()+"/BTag/";

  std::map< string, BTagCalibration > tmp_map_BTagCalibration; //==== key = tagger+"_"+method
  for(unsigned int i=0; i<jetTaggingPars.size(); i++){
    //==== (DeepCSV,Medium,incl,comb

    cout << "[MCCorrection::SetJetTaggingParameters] Contructing BTagCalibrationReader with ";jetTaggingPars.at(i).Print();
    string this_tagger = JetTagging::TaggerToString(jetTaggingPars.at(i).j_Tagger);

    string this_wp = JetTagging::WPToString(jetTaggingPars.at(i).j_WP);
    BTagEntry::OperatingPoint op = BTagEntry::OP_LOOSE;
    if(this_wp=="Loose"){
      op = BTagEntry::OP_LOOSE;
    }
    else if(this_wp=="Medium"){
      op = BTagEntry::OP_MEDIUM;
    }
    else if(this_wp=="Tight"){
      op = BTagEntry::OP_TIGHT;
    }
    else{
      cerr << "[MCCorrection::ReadJetTaggingCVSs()] Wrong WP : " << this_wp << endl;
      exit(ENODATA);
    }
    //==== When using iterativefit method, use BTagEntry::OP_RESHAPING
    if(jetTaggingPars.at(i).j_MeasurmentType_Light==JetTagging::iterativefit ||
       jetTaggingPars.at(i).j_MeasurmentType_Heavy==JetTagging::iterativefit ){
      op = BTagEntry::OP_RESHAPING;
    }

    string this_mt_L = JetTagging::MeasurmentTypeToString(jetTaggingPars.at(i).j_MeasurmentType_Light);
    string this_mt_H = JetTagging::MeasurmentTypeToString(jetTaggingPars.at(i).j_MeasurmentType_Heavy);

    ifstream in(btagpath+"/cvsmap.txt");
    string btagline; // dummy
    while(getline(in,btagline)){
      std::istringstream is( btagline );

      TString tstring_btagline = btagline;
      if(tstring_btagline.Contains("#")) continue;

      int tmp_Year, tmp_Run_Start, tmp_Run_End;
      string tmp_tagger, tmp_pd, tmp_filename;

      is >> tmp_Year; // YEAR
      is >> tmp_tagger; // TAGGER
      is >> tmp_pd; // Period_dep
      is >> tmp_Run_Start; // Run_start
      is >> tmp_Run_End; // Run_end
      is >> tmp_filename; // csv file
      if( tmp_tagger != this_tagger ) continue;
      if( tmp_pd != "All") continue; // don't no period dep now

      //==== Get BTagCalibration objects for both heavy-method and light-method first
      //==== BTagCalibration() is where we read and addEntry from csv file, so it takes time
      //==== So let's not contruct it again if it already exists
      string this_key_L = tmp_tagger+"_"+this_mt_L;
      std::map< string, BTagCalibration >::const_iterator tmp_it_L = tmp_map_BTagCalibration.find( tmp_tagger+"_"+this_key_L );
      if(tmp_it_L==tmp_map_BTagCalibration.end()){
        tmp_map_BTagCalibration[tmp_tagger+"_"+this_key_L] = BTagCalibration(tmp_tagger, this_mt_L, btagpath.Data()+tmp_filename);
      }
      string this_key_H = tmp_tagger+"_"+this_mt_H;
      std::map< string, BTagCalibration >::const_iterator tmp_it_H = tmp_map_BTagCalibration.find( tmp_tagger+"_"+this_key_H );
      if(tmp_it_H==tmp_map_BTagCalibration.end()){
        tmp_map_BTagCalibration[tmp_tagger+"_"+this_key_H] = BTagCalibration(tmp_tagger, this_mt_H, btagpath.Data()+tmp_filename);
      }

      //==== Now, contructing BTagCalibrationReader obect

      std::vector<std::string> systvec_L = {"up", "up_correlated", "up_uncorrelated", "down", "down_correlated", "down_uncorrelated"};
      std::vector<std::string> systvec_C = {"up", "up_correlated", "up_uncorrelated", "down", "down_correlated", "down_uncorrelated"};
      std::vector<std::string> systvec_B = {"up", "up_correlated", "up_uncorrelated", "down", "down_correlated", "down_uncorrelated"};
      if(this_mt_L=="iterativefit"){
        systvec_L = {"up_hf","down_hf","up_jes","down_jes","up_lfstats1","down_lfstats1","up_lfstats2","down_lfstats2"};
        systvec_C = {"up_cferr1","down_cferr1","up_cferr2","down_cferr2"};
        systvec_B = {"up_hfstats1","down_hfstats1","up_hfstats2","down_hfstats2","up_lf","down_lf","up_jes","down_jes"};
      }

      //==== Load L
      map_BTagCalibrationReader[tmp_tagger+"_"+this_wp+"_L_"+this_mt_L] = new BTagCalibrationReader(op, "central", systvec_L);
      map_BTagCalibrationReader[tmp_tagger+"_"+this_wp+"_L_"+this_mt_L]->load( tmp_map_BTagCalibration[tmp_tagger+"_"+this_key_L], BTagEntry::FLAV_UDSG, this_mt_L);
      //==== Load C
      map_BTagCalibrationReader[tmp_tagger+"_"+this_wp+"_C_"+this_mt_H] = new BTagCalibrationReader(op, "central", systvec_C);
      map_BTagCalibrationReader[tmp_tagger+"_"+this_wp+"_C_"+this_mt_H]->load( tmp_map_BTagCalibration[tmp_tagger+"_"+this_key_H], BTagEntry::FLAV_C, this_mt_H);
      //==== Load B
      map_BTagCalibrationReader[tmp_tagger+"_"+this_wp+"_B_"+this_mt_H] = new BTagCalibrationReader(op, "central", systvec_B);
      map_BTagCalibrationReader[tmp_tagger+"_"+this_wp+"_B_"+this_mt_H]->load( tmp_map_BTagCalibration[tmp_tagger+"_"+this_key_H], BTagEntry::FLAV_B, this_mt_H);

    }


  } // END loop jetTaggingPars

  cout << "[MCCorrection::SetJetTaggingParameters] Printing all BTagCalibrationReader :" << endl;
  for(std::map< std::string, BTagCalibrationReader* >::iterator it=map_BTagCalibrationReader.begin(); it!=map_BTagCalibrationReader.end(); it++){
    cout << "[MCCorrection::SetJetTaggingParameters] key = " << it->first << endl;
  }
  SetupMCJetTagEff(Eff_File);

}

double MCCorrection::GetJetTaggingSF(JetTagging::Parameters jtp, int JetFlavor, double JetPt, double JetEta, double Jetdiscr, string Syst){

  if(IsDATA) return 1.;

  string this_tagger = JetTagging::TaggerToString( jtp.j_Tagger );
  string this_wp = JetTagging::WPToString( jtp.j_WP );
  string this_mt_L = JetTagging::MeasurmentTypeToString(jtp.j_MeasurmentType_Light);
  string this_mt_H = JetTagging::MeasurmentTypeToString(jtp.j_MeasurmentType_Heavy);

  string key = JetTagging::TaggerToString( jtp.j_Tagger )+"_"+this_wp;
  BTagEntry::JetFlavor jf = BTagEntry::FLAV_B;
  if(abs(JetFlavor)==5){
    key += "_B_"+this_mt_H;
    jf = BTagEntry::FLAV_B;
  }
  else if(abs(JetFlavor)==4){
    key += "_C_"+this_mt_H;
    jf = BTagEntry::FLAV_C;
  }
  else{
    key += "_L_"+this_mt_L;
    jf = BTagEntry::FLAV_UDSG;
  }

  std::map< std::string, BTagCalibrationReader* >::const_iterator it = map_BTagCalibrationReader.find(key);
  if(it== map_BTagCalibrationReader.end()){
    cerr << "[MCCorrection::GetJetTaggingSF] b tag SF map not found for key = " << key << endl;
    return 1.;
    exit(ENODATA);
  }

  double this_SF = it->second->eval_auto_bounds(Syst, jf, fabs(JetEta), JetPt, Jetdiscr);
  //cout << "[MCCorrection::GetJetTaggingSF] key = " << it->first << endl;
  //cout << "[MCCorrection::GetJetTaggingSF] Jet tagging parameter : ";jtp.Print();
  //printf("[MCCorrection::GetJetTaggingSF] (JetFlavor, JetPt, JetEta, Jetdiscr, Syst) = (%d, %f, %f, %f, %s)\n",JetFlavor,JetPt,JetEta,Jetdiscr,Syst.c_str());
  //cout << "--> SF = " << this_SF << endl;
  return this_SF;

}

double MCCorrection::GetJetTaggingCutValue(JetTagging::Tagger tagger, JetTagging::WP wp){

  if(DataEra=="2016preVFP"){
    if(tagger==JetTagging::DeepCSV){
      if(wp==JetTagging::Loose)  return 0.2027;
      if(wp==JetTagging::Medium) return 0.6001;
      if(wp==JetTagging::Tight)  return 0.8819;
    }
    if(tagger==JetTagging::DeepCSV_CvsL){
      if(wp==JetTagging::Loose) return 0.088;
      if(wp==JetTagging::Medium) return 0.181;
      if(wp==JetTagging::Tight) return 0.417;
    }
    if(tagger==JetTagging::DeepCSV_CvsB){
      if(wp==JetTagging::Loose) return 0.214;
      if(wp==JetTagging::Medium) return 0.228;
      if(wp==JetTagging::Tight) return 0.138;
    }
    if(tagger==JetTagging::DeepJet){
      if(wp==JetTagging::Loose)  return 0.0508;
      if(wp==JetTagging::Medium) return 0.2598;
      if(wp==JetTagging::Tight)  return 0.6502;
    }
    if(tagger==JetTagging::DeepJet_CvsL){
      if(wp==JetTagging::Loose) return 0.039;
      if(wp==JetTagging::Medium) return 0.098;
      if(wp==JetTagging::Tight) return 0.270;
    }
    if(tagger==JetTagging::DeepJet_CvsB){
      if(wp==JetTagging::Loose) return 0.327;
      if(wp==JetTagging::Medium) return 0.370;
      if(wp==JetTagging::Tight) return 0.256;
    }
  }
  if(DataEra=="2016postVFP"){
    if(tagger==JetTagging::DeepCSV){
      if(wp==JetTagging::Loose)  return 0.1918;
      if(wp==JetTagging::Medium) return 0.5847;
      if(wp==JetTagging::Tight)  return 0.8767;
    }
    if(tagger==JetTagging::DeepCSV_CvsL){
      if(wp==JetTagging::Loose) return 0.088;
      if(wp==JetTagging::Medium) return 0.180;
      if(wp==JetTagging::Tight) return 0.407;
    }
    if(tagger==JetTagging::DeepCSV_CvsB){
      if(wp==JetTagging::Loose) return 0.204;
      if(wp==JetTagging::Medium) return 0.221;
      if(wp==JetTagging::Tight) return 0.136;
    }
    if(tagger==JetTagging::DeepJet){
      if(wp==JetTagging::Loose)  return 0.0480;
      if(wp==JetTagging::Medium) return 0.2489;
      if(wp==JetTagging::Tight)  return 0.6377;
    }
    if(tagger==JetTagging::DeepJet_CvsL){
      if(wp==JetTagging::Loose) return 0.039;
      if(wp==JetTagging::Medium) return 0.099;
      if(wp==JetTagging::Tight) return 0.269;
    }
    if(tagger==JetTagging::DeepJet_CvsB){
      if(wp==JetTagging::Loose) return 0.305;
      if(wp==JetTagging::Medium) return 0.353;
      if(wp==JetTagging::Tight) return 0.247;
    }
  }
  if(DataEra=="2017"){
    if(tagger==JetTagging::DeepCSV){
      if(wp==JetTagging::Loose)  return 0.1355;
      if(wp==JetTagging::Medium) return 0.4506;
      if(wp==JetTagging::Tight)  return 0.7738;
    }
    if(tagger==JetTagging::DeepCSV_CvsL){
      if(wp==JetTagging::Loose) return 0.04;
      if(wp==JetTagging::Medium) return 0.144;
      if(wp==JetTagging::Tight) return 0.73;
    }
    if(tagger==JetTagging::DeepCSV_CvsB){
      if(wp==JetTagging::Loose) return 0.345;
      if(wp==JetTagging::Medium) return 0.29;
      if(wp==JetTagging::Tight) return 0.10;
    }
    if(tagger==JetTagging::DeepJet){
      if(wp==JetTagging::Loose)  return 0.0532;
      if(wp==JetTagging::Medium) return 0.3040;
      if(wp==JetTagging::Tight)  return 0.7476;
    }
    if(tagger==JetTagging::DeepJet_CvsL){
      if(wp==JetTagging::Loose) return 0.03;
      if(wp==JetTagging::Medium) return 0.085;
      if(wp==JetTagging::Tight) return 0.52;
    }
    if(tagger==JetTagging::DeepJet_CvsB){
      if(wp==JetTagging::Loose) return 0.4;
      if(wp==JetTagging::Medium) return 0.34;
      if(wp==JetTagging::Tight) return 0.05;
    }
  }
  if(DataEra=="2018"){
    if(tagger==JetTagging::DeepCSV){
      if(wp==JetTagging::Loose)  return 0.1208;
      if(wp==JetTagging::Medium) return 0.4168;
      if(wp==JetTagging::Tight)  return 0.7665;
    }
    if(tagger==JetTagging::DeepCSV_CvsL){
      if(wp==JetTagging::Loose) return 0.064;
      if(wp==JetTagging::Medium) return 0.153;
      if(wp==JetTagging::Tight) return 0.405;
    }
    if(tagger==JetTagging::DeepCSV_CvsB){
      if(wp==JetTagging::Loose) return 0.313;
      if(wp==JetTagging::Medium) return 0.363;
      if(wp==JetTagging::Tight) return 0.288;
    }
    if(tagger==JetTagging::DeepJet){
      if(wp==JetTagging::Loose)  return 0.0490;
      if(wp==JetTagging::Medium) return 0.2783;
      if(wp==JetTagging::Tight)  return 0.7100;
    }
    if(tagger==JetTagging::DeepJet_CvsL){
      if(wp==JetTagging::Loose) return 0.038;
      if(wp==JetTagging::Medium) return 0.099;
      if(wp==JetTagging::Tight) return 0.282;
    }
    if(tagger==JetTagging::DeepJet_CvsB){
      if(wp==JetTagging::Loose) return 0.246;
      if(wp==JetTagging::Medium) return 0.325;
      if(wp==JetTagging::Tight) return 0.267;
    }
  }

  cout << "[MCCorrection::GetJetTaggingCutValue] Wrong " << endl;
  cout << "[MCCorrection::GetJetTaggingCutValue] DataEra = " << DataEra << endl;
  cout << "[MCCorrection::GetJetTaggingCutValue] tagger = " << tagger << endl;
  cout << "[MCCorrection::GetJetTaggingCutValue] wp = " << wp << endl;
  exit(ENODATA);

  return 1;

}

void MCCorrection::SetupMCJetTagEff(TString EffFile){
  cout<<"[MCCorrection::SetupMCJetTagEff] setting MCJetTagEff"<<endl;

  TString datapath=getenv("DATA_DIR");
  TString mcjetpath=datapath+"/"+DataEra+"/BTag/"+EffFile;
  //MeasureJetTaggingEfficiency_TTLL_TTLJ_hadded.root";
  ifstream fcheck(mcjetpath);
  if(!fcheck.good()){
    cout<<"[MCCorrection::SetupMCJetTagEff] no "+mcjetpath<<endl;
    return;
  }
  TFile fmcjet(mcjetpath);
  // Denominator histogram setup first
  vector<TString> jfs = {"B", "C", "Light"};
  for(unsigned int i=0; i<jfs.size(); i++){
    TString hden="Jet_"+DataEra+"_eff_"+jfs.at(i)+"_denom";
    TH2* this_hist=(TH2*)fmcjet.Get(hden);
    map_hist_mcjet[hden]=this_hist;
    this_hist->SetDirectory(0);
    cout<<"[MCCorrection::SetupMCJetTagEff] setting "<<hden<<endl;
  }

  // Numerator histogram setup and divide using "binomial option"
  for (const auto& obj : *(fmcjet.GetListOfKeys())) {
    TH2* this_hist = (TH2*)((TKey*)obj)->ReadObj();
    TString hnum = this_hist->GetName();
    if (!hnum.Contains("num")) continue;

    TString hden = "";
    if (hnum.Contains("_B_")) hden = "Jet_" + DataEra + "_eff_B_denom";
    else if (hnum.Contains("_C_")) hden = "Jet_" + DataEra + "_eff_C_denom";
    else hden = "Jet_" + DataEra + "_eff_Light_denom";

    // Set any 0 or negative bins to 1
    for (int x = 1; x <= this_hist->GetNbinsX(); ++x) {
        for (int y = 1; y <= this_hist->GetNbinsY(); ++y) {
            double bin_content = this_hist->GetBinContent(x, y);
            if (bin_content <= 0) {
                this_hist->SetBinContent(x, y, 1.0);
                this_hist->SetBinError(x, y, 0.0); // optional: remove error for fake bin
                cout << "[BinFix] " << hnum 
                     << " bin (" << x << "," << y 
                     << ") had content " << bin_content 
                     << " -> set to 1.0" << endl;
		//exit(EXIT_FAILURE);

	    }
        }
    }

    // === Check denominator histogram bins before division ===
    // Set any 0 or negative bins to 1                                                                                                                                                       
    for (int x = 1; x <= map_hist_mcjet[hden]->GetNbinsX(); ++x) {
        for (int y = 1; y <= map_hist_mcjet[hden]->GetNbinsY(); ++y) {
            double bin_content = map_hist_mcjet[hden]->GetBinContent(x, y);
            if (bin_content <= 0) {
                map_hist_mcjet[hden]->SetBinContent(x, y, 1.0);
                map_hist_mcjet[hden]->SetBinError(x, y, 0.0); // optional: remove error for fake bin                                                                                                    
                cout << "[BinFix] " << hnum
                     << " bin (" << x << "," << y
                     << ") had content " << bin_content
                     << " -> set to 1.0" << endl;

            }
        }
    }

    // === Divide numerator by denominator ===
    this_hist->Divide(this_hist, map_hist_mcjet[hden], 1., 1., "b");

    // === Optional: Print efficiencies and warnings ===

    /*
      for (int x = 1; x <= this_hist->GetNbinsX(); ++x) {
      for (int y = 1; y <= this_hist->GetNbinsY(); ++y) {
      double eff = this_hist->GetBinContent(x, y);
      cout << "[Efficiency] " << hnum << " bin (" << x << "," << y << ") = " << eff << endl;
      
      if (eff <= 0 || eff >= 1) {
      cout << "[Warning] Efficiency out of range: " << hnum
      << " bin (" << x << "," << y << ") = " << eff << endl;
      }
      }
      }
    */

    
    map_hist_mcjet[hnum] = this_hist;
    this_hist->SetDirectory(0);
    cout << "[MCCorrection::SetupMCJetTagEff] setting " << hnum << endl;
  }
 
}

double MCCorrection::GetMCJetTagEff(JetTagging::Tagger tagger, JetTagging::WP wp, int JetFlavor, double JetPt, double JetEta, int sys){

  if(IsDATA) return 1.;

  if(JetPt<20) JetPt = 20.;
  if(JetPt>=1000.) JetPt = 999.;
  if(JetEta>=2.5) JetEta = 2.49;
  if(JetEta<-2.5) JetEta = -2.5;

  TString jf = "";
  if(JetFlavor == 5) jf = "B";
  else if(JetFlavor == 4) jf = "C";
  else if(JetFlavor == 0) jf = "Light";
  else{
    cout<<"[MCCorrection::GetMCJetTagEff] no JetFlavor"<<endl;
    exit(EXIT_FAILURE);
  }

  double value = 1., error = 0., out = 1.;
  TString hnum="Jet_"+DataEra+"_"+JetTagging::TaggerToString(tagger)+"_"+JetTagging::WPToString(wp)+"_eff_"+jf+"_num";
  TH2 *this_hist = map_hist_mcjet[hnum];
  int this_bin = this_hist->FindBin(fabs(JetEta),JetPt);
  value = this_hist->GetBinContent(this_bin);
  error = this_hist->GetBinError(this_bin);
  
  out = value+double(sys)*error;
  if(out<=0.0){
    cout<<"GetMCJetTagEff value " << out << "  is too low.... "<<endl; exit(ENODATA);
  }  
  else if(out>=0.98) {
    if(JetPt > 600.0)   return GetMCJetTagEff(tagger,  wp, JetFlavor, 499.0,JetEta, sys);
    else {
      cout<<"GetMCJetTagEff value " << out << "  is too high.... "<<endl;
      exit(ENODATA);
    }
  }
  return out;
}

double MCCorrection::GetBTaggingReweight_1a(const vector<Jet>& jets, JetTagging::Parameters jtp, string Syst){
//Syst. usage ex.: "SystUpHTag"(all component variation for heavy flav(b,c).), 
//                 "SystUpHTagCorr"(variation of heavy flav(b,c) sf only for yearly correlated components)
//change H->L for light flav., Up->Down for downward variation, Corr->UnCorr for yearly independent components
 
  if(IsDATA) return 1.;

  TString SystStr(Syst);
  double Prob_MC(1.), Prob_DATA(1.), SF(1.);
  bool Syst_HTag=false, Syst_LTag=false; int SystDir=0, CorrType=0;
  string SystKey;
  if(SystStr.Contains("Syst")){
    if     (SystStr.Contains("HTag")) Syst_HTag=true;    
    else if(SystStr.Contains("LTag")) Syst_LTag=true;
    if     (SystStr.Contains("Up")  ) SystDir= 1;
    else if(SystStr.Contains("Down")) SystDir=-1;
    if     (SystStr.Contains("UnCorr")) CorrType=-1;
    else if(SystStr.Contains("Corr"))   CorrType= 1;
    if(SystDir==0){ cout<<"SystStr in not correct form"<<endl; exit(ENODATA); }
    if(!(Syst_HTag or Syst_LTag)){ cout<<"SystMode but no H/L mode assigned"<<endl; exit(ENODATA); }
  }

  for(unsigned int i=0; i<jets.size(); i++){
    int JetHadFlav = jets.at(i).hadronFlavour();
    bool ApplySyst=false;
    if     (Syst_HTag && (JetHadFlav==4 or JetHadFlav==5)){ ApplySyst=true; }
    else if(Syst_LTag && (JetHadFlav==0                 )){ ApplySyst=true; }

    if     (ApplySyst && CorrType==0) SystKey=SystDir>0? "up":"down";
    else if(ApplySyst && CorrType >0) SystKey=SystDir>0? "up_correlated":"down_correlated";
    else if(ApplySyst && CorrType <0) SystKey=SystDir>0? "up_uncorrelated":"down_uncorrelated";
    else                              SystKey="central";


    double this_MC_Eff = GetMCJetTagEff(jtp.j_Tagger, jtp.j_WP, jets.at(i).hadronFlavour(), jets.at(i).Pt(), jets.at(i).Eta());
    double this_SF = GetJetTaggingSF(jtp,
                                     jets.at(i).hadronFlavour(),
                                     jets.at(i).Pt(),
                                     jets.at(i).Eta(),
                                     jets.at(i).GetTaggerResult(jtp.j_Tagger),
                                     SystKey );
    double this_DATA_Eff = this_MC_Eff*this_SF;

    bool isTagged = jets.at(i).GetTaggerResult(jtp.j_Tagger) > GetJetTaggingCutValue(jtp.j_Tagger, jtp.j_WP);
    if(isTagged){
      Prob_MC *= this_MC_Eff;
      Prob_DATA *= this_DATA_Eff;
    }
    else{
      Prob_MC *= 1.-this_MC_Eff;
      Prob_DATA *= 1.-this_DATA_Eff;
    }
    if(DEBUG) cout << "Jet " << i << " this_SF = " << this_SF  << " this_MC_Eff = " << this_MC_Eff << " this_DATA_Eff = " << this_DATA_Eff << " isTagged = " << isTagged << endl;
  }

  if(Prob_MC>0. && Prob_DATA>0.) SF=Prob_DATA/Prob_MC;
  else SF=0.;

  return SF;
}

double MCCorrection::GetBTaggingReweight_1d(const vector<Jet>& jets, JetTagging::Parameters jtp, string Syst){

  if(IsDATA) return 1.;

  if(jtp.j_MeasurmentType_Light!=JetTagging::iterativefit || 
     jtp.j_MeasurmentType_Heavy!=JetTagging::iterativefit){
    cout << "[MCCorrection::GetBTaggingReweight_1d] This method only works for iterativefit method" << endl;
    cout << "[MCCorrection::GetBTaggingReweight_1d] jtp.j_MeasurmentType_Light = " << jtp.j_MeasurmentType_Light << endl;
    cout << "[MCCorrection::GetBTaggingReweight_1d] jtp.j_MeasurmentType_Heavy = " << jtp.j_MeasurmentType_Heavy << endl;
    exit(ENODATA);
    return 1.;
  }

  double rew(1.);

  for(unsigned int i=0; i<jets.size(); i++){

    int abs_hadFlavour = abs(jets.at(i).hadronFlavour());
    TString tmp_Syst(Syst);

/*
    systvec_L = {"up_hf","down_hf","up_jes","down_jes","up_lfstats1","down_lfstats1","up_lfstats2","down_lfstats2"};
    systvec_C = {"up_cferr1","down_cferr1","up_cferr2","down_cferr2"};
    systvec_B = {"up_hfstats1","down_hfstats1","up_hfstats2","down_hfstats2","up_lf","down_lf","up_jes","down_jes"};
*/

    bool GoodSyst = false;
    if(abs_hadFlavour==5){
      if(      tmp_Syst.Contains(TRegexp("hfstats[1-2]$")) ) GoodSyst = true;
      else if( tmp_Syst.Contains(TRegexp("lf$"))           ) GoodSyst = true;
      else if( tmp_Syst.Contains(TRegexp("jes$"))          ) GoodSyst = true;
    }
    else if(abs_hadFlavour==4){
      if(      tmp_Syst.Contains(TRegexp("cferr[1-2]$"))   ) GoodSyst = true;
    }
    else{
      if(      tmp_Syst.Contains(TRegexp("hf$")) )           GoodSyst = true;
      else if( tmp_Syst.Contains(TRegexp("jes$")) )          GoodSyst = true;
      else if( tmp_Syst.Contains(TRegexp("lfstats[1-2]$")) ) GoodSyst = true;
    }

    if(!GoodSyst) tmp_Syst = "central";

    double this_SF = GetJetTaggingSF(jtp,
                                     jets.at(i).hadronFlavour(),
                                     jets.at(i).Pt(),
                                     jets.at(i).Eta(),
                                     jets.at(i).GetTaggerResult(jtp.j_Tagger),
                                     string(tmp_Syst) );
    rew *= this_SF;
  }

  return rew;

}

bool MCCorrection::IsBTagged_2a(JetTagging::Parameters jtp, const Jet& jet, string Syst){

  double this_discr = jet.GetTaggerResult(jtp.j_Tagger);
  double cutValue = GetJetTaggingCutValue(jtp.j_Tagger, jtp.j_WP);

  bool isBTagged = this_discr > cutValue;

  if(IsDATA) return isBTagged;

  //==== Set seed
  unsigned int runNum_uint  = static_cast <unsigned int> (run);
  unsigned int lumiNum_uint = static_cast <unsigned int> (lumi);
  unsigned int evNum_uint   = static_cast <unsigned int> (event);
  unsigned int jet0eta = uint32_t(fabs(jet.Eta())/0.01);
  int m_nomVar=1;
  std::uint32_t seed = jet0eta + m_nomVar + (lumiNum_uint<<10) + (runNum_uint<<20) + evNum_uint;

  TRandom3 rand_(seed);

  bool newBTag = isBTagged;

  //=== Get SF
  double Btag_SF =  GetJetTaggingSF(jtp,
                                    jet.hadronFlavour(),
                                    jet.Pt(),
                                    jet.Eta(),
                                    jet.GetTaggerResult(jtp.j_Tagger),
                                    Syst );


  if(Btag_SF == 1) return newBTag; //no correction needed

  //=== throw random number to apply correction
  float coin = rand_.Uniform(1.);
  if(Btag_SF > 1){
    //=== use this if SF>1

    if( !isBTagged ) {

      double Btag_eff = GetMCJetTagEff(jtp.j_Tagger, jtp.j_WP, jet.hadronFlavour(), jet.Pt(), jet.Eta());
      //=== fraction of jets that need to be upgraded
      float mistagPercent = (1.0 - Btag_SF) / (1.0 - (1./Btag_eff) );

      //=== upgrade to tagged
      if( coin < mistagPercent ) {newBTag = true;}
    }

  }else{
    //=== use this if SF<1
 
    //=== downgrade tagged to untagged
    if( isBTagged && coin > Btag_SF ) {newBTag = false;}

  }

  return newBTag;

}

