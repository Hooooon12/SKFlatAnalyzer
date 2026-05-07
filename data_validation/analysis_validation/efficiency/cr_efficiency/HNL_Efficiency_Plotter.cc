#include "HNL_Efficiency_Plotter.h"
#include <stdlib.h>

HNL_Efficiency_Plotter::HNL_Efficiency_Plotter(TString macroname){
  
  TH1::SetDefaultSumw2(true);
  TH1::AddDirectory(kFALSE);

  setTDRStyle();

  SetLabels=false;
  gStyle->SetOptStat(0);
  DoDebug = false;
  gErrorIgnoreLevel = kError;
  SkimName="SkimTree_HNMultiLeptonBDT";    
  AnalyserName="HNL_Validation";                                                                                                    
  MacroName=macroname;
  Era="2017";
  filename_prefix=AnalyserName;
  filename_suffix=".root";
  infilepath="./";
  def_infilepath="./";
  def_histpath="";
  MakePaperPlot = false;  
  //// clear vecs
  RegionType.clear();
  LeptonChannels.clear();
  HistPath.clear();
  ApplyMCNormSF.clear();
  HistNames.clear();
  LegendNames.clear();
  LxplusCutFlowResults.clear();
  LxplusHistResults.clear();
  CutFlowResults.clear();
  HistResults.clear();
  map_sample_string_to_scale.clear();

  log_of_generation_mixing=0;
  
  VarBins=false;
  MergeZeroBins= false;
  CopyToWebsite=true;
  comp_default_set=false;
}



void HNL_Efficiency_Plotter::DrawStackPlots(){
  DrawDataAll(false);
  make_bkglist();
  draw_hist();
  return;
}

void HNL_Efficiency_Plotter::DrawStackPlotsWithData(){

  DrawDataAll(true);
  make_bkglist();
  draw_hist();
  return;
}

void HNL_Efficiency_Plotter::DrawComparisonPlots(){
  if(!comp_default_set) return;
  if(FullHistNames.size() ==0) return;
  draw_comphist();
}

void HNL_Efficiency_Plotter::SetupDefaultHist(TString sample, TString hn, TString histtag, TString legend, TString htype, TString hunit, vector<double> rb, double Xmin, double Xmax, double Ymax){
  
  if(comp_default_set) return;
  comp_default_set=true;
  def_infilepath = infilepath + "/"+filename_prefix + "_"+SkimName+"_"+sample+".root";
  def_histpath=hn;
  
  x_title.push_back(GetTitleByType(htype));
  units.push_back(hunit);
  Rebins.push_back(rb);
  Xmins.push_back(Xmin);
  Xmaxs.push_back(Xmax);
  Ymaxs.push_back(Ymax);

  HistNames.push_back(histtag);
  LegendNames.push_back(legend);

}

void HNL_Efficiency_Plotter::SetupComparisonHist(TString sample, TString hist, TString histtag, TString legend){

  FullHistNames.push_back(hist);
  SamplePaths.push_back(infilepath + "/"+filename_prefix + "_"+SkimName+"_"+sample+".root");
  HistNames.push_back(histtag);  LegendNames.push_back(legend);


}
void HNL_Efficiency_Plotter::AddHist(TString hn, TString htype, TString hunit, vector<double> rb, double Xmin, double Xmax, double Ymax){

  HistNames.push_back(hn);
  x_title.push_back(GetTitleByType(htype));
  units.push_back(hunit);
  Rebins.push_back(rb);
  Xmins.push_back(Xmin);
  Xmaxs.push_back(Xmax);
  Ymaxs.push_back(Ymax);

  return;
}
void HNL_Efficiency_Plotter::BasicSetup(SetupHelper logy, SetupHelper ratio,  TString channel){
  
  /// setups up default options
  
  if(logy==LOGY) UseLogyAll(true);
  else UseLogyAll(false);
  if(ratio==DrawRatio) DrawRatioAll(true) ;
  else DrawRatioAll(false);

  LeptonChannel(channel);
}

void HNL_Efficiency_Plotter::SetupPlotter(TString era, TString Skim, TString Analyzer){
  SkimName=Skim;
  Era=era;
  AnalyserName=Analyzer;
  filename_prefix=AnalyserName;
  SetupPlotter();
}

void HNL_Efficiency_Plotter::SetupPlotter(){
  
  TString s_hostname = GetHostname(); 
  TString ENV_MERGEDFILE_PATH = getenv("FILE_MERGED_PATH"); 
  TString ENV_PLOT_PATH = getenv("PLOT_PATH"); 
  TString FLATVERSION = getenv("FLATVERSION"); 
  TString input_path = ENV_MERGEDFILE_PATH +"/"+AnalyserName+"/"; 
  TString output = ENV_PLOT_PATH + FLATVERSION + "/"+AnalyserName+"/"; 
  MakeDir(ENV_PLOT_PATH + FLATVERSION); 
  MakeDir(output);  output+="/"+MacroName+"/"; 
  MakeDir(output);  output+=Era+"/";                                                                                                          
  MakeDir(output);                                                                                                                                 
  plotpath=output;
  cout << MacroName << " Output dir = " << output << endl;

  infilepath = ENV_MERGEDFILE_PATH + "/"+AnalyserName+"/"+  Era;

  SetupSampleInfo();
                                 
  return;

}

TString HNL_Efficiency_Plotter::GetTitleByType(TString htype ){
  
  if(htype == "MET")   return "#slash{E}_{T}^{miss} [GeV]";
  if(htype == "METPhi") return "#phi(#slash{E}_{T}^{miss})";
  if(htype == "LPT1")  return "p^{l1}_{T} [GeV]";
  if(htype == "LPT2")  return "p^{l2}_{T} [GeV]";
  if(htype == "MLL")   return "m_{ll} (GeV)";
  if(htype == "NJ4") return "N^{AK4}_{j}";
  if(htype == "NJ8") return "N^{AK8}_{j}";
  if(htype == "NBJ") return "N^{AK4}_{bj}";
  return htype; 
  
}



HNL_Efficiency_Plotter::~HNL_Efficiency_Plotter(){

  system("rm -rf "+path_rebins);
  system("rm -rf "+path_y_axis);
  system("rm -rf "+path_x_axis);
  
}

void HNL_Efficiency_Plotter::make_cutflow(TString Hist_For_CutFlow){
  
  make_bkglist();

  for(i_cut = 0; i_cut < HistPath.size(); i_cut++){

    thiscut_plotpath = plotpath+"/"+ HistPath[i_cut];
    mkdir(thiscut_plotpath);
    
    cout
      << endl
      << "################### Writing in Directory " << Hist_For_CutFlow << " ###################" << endl
      << endl;
    
    
  
    
    if(DoDebug) cout << "========================================================================================" << endl;
    if(DoDebug) cout << "Running Make_Cutflow : Reading numbers from hist " << Hist_For_CutFlow << endl;
    
    map< TString, TH1D * > map_hist_y;
    
    TString temp_suffix = HistPath[i_cut];
    TString DirName = temp_suffix;//.Remove(0,1);                                                                                                                                            
    if(DoDebug) cout << "Running Make_Cutflow : HistPath =  " << DirName << endl;
    
    TH1D* hist_data = NULL;
    vector<TH1D*> hist_signal;
    
    
    for(i_file = 0; i_file < bkglist.size()+1+signal_mass.size(); i_file++){ // +1 for data                                                                           
      TString filepath, current_sample, signal_name_for_tex;
      
      //==== root file path name                                                                                                                                                                                                         
      //==== bkg                                                                                                                                                                                                                         
      if( i_file < bkglist.size() ){
	TString tmp = bkglist[i_file];
	if(bkglist[i_file].Contains("fake") || bkglist[i_file].Contains("chargeflip")) tmp += "_"+PrimaryDataset[i_cut];
	filepath = infilepath + "/"+filename_prefix+"_"+SkimName+"_"+tmp+filename_suffix;
	cout << "filepath = " << filepath << endl;
	current_sample = bkglist[i_file];
      }
      //==== data for i_file = bkglist.size()                                                                                                                                                                                            
      else if( i_file == bkglist.size() ){
	filepath = infilepath + "/"+filename_prefix + "_"+SkimName+"_data.root";
	current_sample = "data";
	cout << "filepath =  " << filepath <<endl;
	
      }
      //==== signal starting from i_file = bkglist.size()+1                                                                                                                                                                              
      else{
	
	int signal_index = i_file-bkglist.size()-1;
	
	//==== if cut is optimized cut, only draw that signal                                                                                                                                                                            
	if(HistPath[i_cut].Contains("cutHN")){
	  TString tmpcut = "_cutHN"+TString::Itoa(signal_mass[signal_index],10);
	  if(HistPath[i_cut]!=tmpcut) continue;
	}
	//==== else, follow signal_draw                                                                                                                                                                                                  
	else{
	  if(!signal_draw[signal_index]) continue;
	}
	
	if(DoDebug) cout << "signal_index = " << signal_index << " => mass = " << signal_mass[signal_index] << endl;
	TString WhichChannel = LeptonChannels[i_cut];
	TString WhichChannel_for_tex = WhichChannel;
	
	//==== TChannel                                                                                                                                                                                                                  
	if( signal_mass[signal_index] < 0 ){
	  WhichChannel = "HeavyNeutrinoTo"+WhichChannel+"_Tchannel_M";
	}
	else{
	  WhichChannel = "HN"+WhichChannel+"_";
	}
	TString string_signal_mass = WhichChannel+TString::Itoa(abs(signal_mass[signal_index]),10);
	
	signal_name_for_tex = "SchHN"+WhichChannel_for_tex+TString::Itoa(abs(signal_mass[signal_index]),10);
	if(signal_mass[signal_index] < 0 ) signal_name_for_tex = "TchHN"+WhichChannel_for_tex+TString::Itoa(abs(signal_mass[signal_index]),10);
	
	filepath = "./rootfiles/"+data_class+"/Signal/"+filename_prefix+"_SK"+string_signal_mass+filename_suffix;
	//cout << filepath << endl;                                                                                                                                                                                                      
	current_sample = string_signal_mass;
      }
      
      if(DoDebug){
	cout
	  << "filepath = " << filepath << endl
	  << "hisname = " << HistPath[i_cut]+"/"+Hist_For_CutFlow << endl;
      }
      
      //==== get root file                                                                                                                                                                                                               
      if(gSystem->AccessPathName(filepath)){
	if(DoDebug){
	  cout << "No file : " << filepath << endl;
	}
	continue;
      }
      TFile* file = new TFile(filepath);
      if( !file ){
	if(DoDebug){
	cout << "No file : " << filepath << endl;
	}
	continue;
      }
      
      TDirectory *dir = (TDirectory *)file->Get(DirName);
      if(!dir){
	if(DoDebug){
	  cout << "No Directory : " << file->GetName() << "\t" << DirName << endl;
	  file->ls();
	}
	file->Close();
	delete file;
	continue;
      }
      file->cd(DirName);
      
      //==== full histogram name                                                                                                                                                                                                         
      TString fullhistname = Hist_For_CutFlow;
      
      //==== get histogram                                                                                                                                                                                                               
      TH1D* hist_temp = (TH1D*)dir->Get(fullhistname);
      if(!hist_temp || hist_temp->GetEntries() == 0){
	if(DoDebug){
	  cout << "No histogram : " << current_sample << endl;
	}
	file->Close();
	delete file;
	continue;
      }
      //==== set histogram name, including sample name                                                                                                                        
      if(DoDebug)  cout << "Setting Hist TMP name " << fullhistname+"_"+current_sample << endl;
      
      hist_temp->SetName(fullhistname+"_"+current_sample);
      
      if(DoDebug)  cout << "==== make overflows bins   " << endl;
      //==== make overflows bins                                                                                                                                                                                                         
      if(DoDebug)  cout << "MakeOverflowBin " << endl;
      TH1D *hist_final = MakeOverflowBin(hist_temp);
      double ScaleBkg = (i_file < bkglist.size()) ? GetScale(bkglist[i_file]) : 1 ;
      hist_final->Scale(ScaleBkg);

      if(DoDebug)  cout << "Integral of hist_final = " << hist_final->Integral() << endl;
      
      TString current_MCsector = "";
      //==== Set Attributes here                                                                                                                                                                                                         
      //==== bkg                                                                                                                                                                                                                         
      if( i_file < bkglist.size() ){
	//==== get which MC sector                                                                                                                                                                                                       
	current_MCsector = find_MCsector();
	int n_bins = hist_final->GetXaxis()->GetNbins();
	//if(!MC_stacked_allerr){
	
	//	const Double_t *xcopy=hist_final->GetXaxis()->GetXbins()->GetArray();
	//	MC_stacked_allerr = new TH1D("MC_stacked_allerr", "", n_bins, xcopy);
	//	MC_stacked_staterr = new TH1D("MC_stacked_staterr", "", n_bins, xcopy);

	//}
	//MC_stacked_staterr->Add(hist_final);
	
	double ThisSyst = 0.;
	if( current_sample.Contains("fake") ) ThisSyst = 0.3;
	else if( current_sample.Contains("chargeflip") ) ThisSyst = 0.1;
	else{
	  double mcnorm = 0.1;
	  double lumi =  0.025;///analysisInputs.CalculatedSysts["Luminosity"];                                                                                       
	  if(Era=="2017") lumi =  0.02;
	  if(Era=="2018") lumi =  0.015;
	  
	  ThisSyst = sqrt( mcnorm*mcnorm + lumi*lumi );
	}
	
	for(int i=1; i<=n_bins; i++){
	  
	  double error_syst = ThisSyst*(hist_final->GetBinContent(i));
	  double error_sumw2 = hist_final->GetBinError(i);
	  double error_combined = sqrt( error_syst*error_syst + error_sumw2*error_sumw2 );
	  
	  hist_final->SetBinError(i, error_combined);
	}
	
	//MC_stacked->Add(hist_final);
	//MC_stacked_allerr->Add(hist_final);
      }
      
      //==== data for i_file = bkglist.size()                                                                                                                                                                                            
      else if( i_file == bkglist.size() ){
	hist_data = (TH1D*)hist_final->Clone();
      }
      //==== signal starting from i_file = bkglist.size()+1                                                                                                                                                                              
      else if( i_file > bkglist.size() ){
	int signal_index = i_file-bkglist.size()-1;
	if(DoDebug) cout << "signal index = " << signal_index << ", mass = " << signal_mass[signal_index] << endl;
	
	TString temp_hist_name(hist_final->GetName());
	double this_coupling_constant = coupling_constant(signal_mass[signal_index]);
	
	hist_final->Scale( k_factor*this_coupling_constant/(1.*TMath::Power(10,log_of_generation_mixing)) );
	hist_signal.push_back( (TH1D*)hist_final->Clone() );
	signal_survive_mass.push_back(signal_mass[signal_index]);
      }
      else{
	cout << "[Warning] attirubte setting, i_file > total sample size? This should not happen!" << endl;
      }
      

      cout << "current_sample = " << current_sample << endl;
      TString alias = "";
      if(current_sample.Contains("data")) alias = "data";
      else if(current_sample.Contains("HN")) alias = signal_name_for_tex;
      else{
	alias = map_sample_string_to_legendinfo[current_MCsector].first;
      }
      cout << "==> alias = " << alias << endl;
      
      if( map_hist_y.find(alias) == map_hist_y.end() ){
	map_hist_y[alias] = new TH1D(alias, "", 1, 0., 1);
      }
      
      map_hist_y[alias]->Add(hist_final);
      if(DoDebug) cout << "Adding to map_hist_y " << alias << " with integral " << hist_final->Integral() << endl;
      
      file->Close();
      delete file;
    
      if(DoDebug) cout << "end of this sample" << endl;
      
    } // END loop over samples                                                                                                                                   
    
    MakeTexFile(map_hist_y,Hist_For_CutFlow);
  }
}



TH1D* HNL_Efficiency_Plotter::ConstructHist(TString filepath, TString fullhistname){

  TH1D* hist_temp;
  
  //==== get root file                                                                                                                     

  if(DoDebug) cout << "ConstructHist " << endl;

  if(gSystem->AccessPathName(filepath)){
    if(DoDebug)     cout << "No file : " << filepath << endl;
    return hist_temp;
  }

  TFile* file = new TFile(filepath);
  if( !file ){
    if(DoDebug)    cout << "No file : " << filepath << endl;
    return hist_temp;
  }
  
  //==== get histogram                                                                                                                                                                                                                                                                
  hist_temp = (TH1D*)file->Get(fullhistname);
  if(!hist_temp || hist_temp->GetEntries() == 0){
    if(DoDebug){
      cout << "No histogram : " << fullhistname << endl;
    }
    file->Close();
    delete file;
    return hist_temp;
  }

  //==== set histogram name, including sample name                                                                                                                                                                                                                                    
  hist_temp->SetName(fullhistname);

  //==== make overflows bins                                                                                                                                                                                                                                                          
  TH1D *hist_final = MakeOverflowBin(hist_temp);

  //==== Remove Negative bins                                                                                                                                                                                                                                                         
  TAxis *xaxis = hist_final->GetXaxis();
  for(int ccc=1; ccc<=xaxis->GetNbins(); ccc++){
    if(DoDebug) cout << fullhistname << "\t["<<xaxis->GetBinLowEdge(ccc) <<", "<<xaxis->GetBinUpEdge(ccc) << "] : " << hist_final->GetBinContent(ccc) << endl;
    if(hist_final->GetBinContent(ccc)<0){
      hist_final->SetBinContent(ccc, 0.);
      hist_final->SetBinError(ccc, 0.);
    }
  }

  
  file->Close();
  return hist_final;
  
}

TH2D* HNL_Efficiency_Plotter::Construct2DHist(TString filepath, TString fullhistname){

  TH2D* hist_temp;

  //==== get root file                                                                                                                                                                                                                                                                                                     

  if(DoDebug) cout << "ConstructHist " << endl;

  if(gSystem->AccessPathName(filepath)){
    if(DoDebug)     cout << "No file : " << filepath << endl;
    return hist_temp;
  }

  TFile* file = new TFile(filepath);
  if( !file ){
    if(DoDebug)    cout << "No file : " << filepath << endl;
    return hist_temp;
  }

  //==== get histogram                                                                                                                                                                                                                                                                                                     
  hist_temp = (TH2D*)file->Get(fullhistname);
  if(!hist_temp || hist_temp->GetEntries() == 0){
    if(DoDebug){
      cout << "No histogram : " << fullhistname << endl;
    }
    file->Close();
    delete file;
    return hist_temp;
  }

  //==== set histogram name, including sample name                                                                                                                                                                                                                                                                         
  hist_temp->SetName(fullhistname);

  //==== make overflows bins                                                                                                                                                                                                                                                                                               
  file->Close();
  return hist_temp;

}


TH1D* HNL_Efficiency_Plotter::MakeHist(TString filepath, TString fullhistname){

  TH1D* hist_temp;

  //==== get root file                                                                                                                                       
  if(gSystem->AccessPathName(filepath)){
    if(DoDebug)     cout << "No file : " << filepath << endl;
    return hist_temp;
  }

  TFile* file = new TFile(filepath);
  if( !file ){
    if(DoDebug)    cout << "No file : " << filepath << endl;
    return hist_temp;
  }

  //==== get histogram                                                                                                                                       
  hist_temp = (TH1D*)file->Get(fullhistname);
  if(!hist_temp || hist_temp->GetEntries() == 0){
    if(DoDebug){
      cout << "No histogram : " << fullhistname << endl;
    }
    file->Close();
    delete file;
    return hist_temp;
  }
  
  //==== set histogram name, including sample name                                                                                                           
  hist_temp->SetName(fullhistname);

  //==== rebin here                                                                                                                                         
  if(Rebins.size() != 1)  exit (EXIT_FAILURE);
;
  vector<double> vrebin= Rebins[0];
  if(vrebin.size() > 1){

    if(DoDebug){
      cout << "Using Variable binning" << endl;
      for(auto rb : vrebin) cout << "--- " << rb << endl;
    }

    double TMParray[vrebin.size()];
    std::copy(vrebin.begin(), vrebin.end(), TMParray);
    hist_temp = (TH1D *)hist_temp->Rebin(vrebin.size()-1, "hnew1", TMParray);
  }
  else hist_temp->Rebin( vrebin[0] );


  //==== set X-axis range                                                                                                                                    
  SetXaxisRange(hist_temp);

  //==== make overflows bins                                                                                                                                 
  TH1D *hist_final = MakeOverflowBin(hist_temp);

  //==== Remove Negative bins                                                                                                                                
  TAxis *xaxis = hist_final->GetXaxis();
  for(int ccc=1; ccc<=xaxis->GetNbins(); ccc++){
    if(DoDebug) cout << fullhistname << "\t["<<xaxis->GetBinLowEdge(ccc) <<", "<<xaxis->GetBinUpEdge(ccc) << "] : " << hist_final->GetBinContent(ccc) << endl;
    if(hist_final->GetBinContent(ccc)<0){
      hist_final->SetBinContent(ccc, 0.);
      hist_final->SetBinError(ccc, 0.);
    }
  }  

  file->Close();

  return hist_final;
}

void HNL_Efficiency_Plotter::draw_comphist(){

  if(DoDebug) {
    cout << "Comparison Plotter: " << endl ; 
    cout << "def_infilepath =  " << def_infilepath << endl ;
    cout << "def_histpath = " << def_histpath << endl;
  }

  TH1D *hist_default = MakeHist(def_infilepath, def_histpath );
  hist_default->SetLineColor(kRed);
  hist_default->SetLineWidth(3);
  
  if(DoDebug)  cout << hist_default->Integral() << endl;

  for(i_var = 0; i_var < FullHistNames.size(); i_var++){
    
    thiscut_plotpath = plotpath+"/";
    mkdir(thiscut_plotpath);

    TFile *outputfile = new TFile(thiscut_plotpath+"/hists.root", "RECREATE");

    TH1D *hist_comp = MakeHist(SamplePaths[i_var], FullHistNames[i_var] );
    hist_comp->SetLineColor(kSpring-2);
    hist_comp->SetLineWidth(3);

    if(DoDebug)  cout << hist_comp->Integral() << endl;
    
    TLegend *lg= new TLegend(0.55, 0.60, 0.93, 0.90);
    lg->SetTextSize(0.03);
    draw_comp_canvas(hist_default, hist_comp, lg, outputfile);
    outputfile->Close();
    system("rm "+thiscut_plotpath+"/hists.root");

  }
}

void HNL_Efficiency_Plotter::draw_hist(){
  
  /// Loop over Cut/Regions set in Plotter.HistPath
  for(i_cut = 0; i_cut < HistPath.size(); i_cut++){

    thiscut_plotpath = plotpath+"/"+HistPath[i_cut];
    mkdir(thiscut_plotpath);

    TFile *outputfile = new TFile(thiscut_plotpath+"/hists.root", "RECREATE");
    
    cout
    << endl
    << "################### Writing in Directory " << HistPath[i_cut] << " ###################" << endl
    << endl;

    //==== Make rebin/y_axis/x_axis file for this cut here
    MakeRebins();
    MakeYAxis();
    MakeXAxis();
    
    
    TString temp_suffix = HistPath[i_cut];
    TString DirName = temp_suffix;//.Remove(0,1);
    

    for(i_var = 0; i_var < HistNames.size(); i_var++){

      if( find( CutVarSkips.begin(), CutVarSkips.end(), make_pair(HistPath[i_cut], HistNames[i_var]) ) != CutVarSkips.end() ){
        continue;
      }
      
      cout << "[Drawing " << HistNames[i_var] << "]" << endl;
      
      TH1D* MC_stacked_staterr = NULL;
      TH1D* MC_stacked_allerr = NULL;
      THStack* MC_stacked = new THStack("MC_stacked", "");
      TH1D* hist_data = NULL;
      vector<TH1D*> hist_signal;
      

      TLegend *lg;
      if(drawratio.at(i_cut)){
        //==== CR
        if(signal_mass.size()==0) lg = new TLegend(0.60, 0.55, 0.96, 0.92);
        //==== SR
        else lg = new TLegend(0.60, 0.35, 0.96, 0.92);
      }
      else{
        //==== CR
        if(signal_mass.size()==0) lg = new TLegend(0.60, 0.55, 0.95, 0.92);
        //==== SR
        else lg = new TLegend(0.55, 0.50, 0.93, 0.90);
      }

      clear_legend_info();
      
      signal_survive_mass.clear();

      bool AnyEntry = false;
      map< TString, TH1D * > map_hist_y;
      
      for(i_file = 0; i_file < bkglist.size()+1+signal_mass.size(); i_file++){ // +1 for data
	
        TString filepath, current_sample, signal_name_for_tex;
        
        //==== root file path name
        //==== bkg
        if( i_file < bkglist.size() ){
          TString tmp = bkglist[i_file];
          if(bkglist[i_file].Contains("fake") || bkglist[i_file].Contains("chargeflip")) tmp += "_"+PrimaryDataset[i_cut];
          filepath = infilepath + "/"+filename_prefix+"_"+SkimName+"_"+tmp+filename_suffix;
	  cout << "filepath = " << filepath << endl;
          current_sample = bkglist[i_file];
        }
        //==== data for i_file = bkglist.size()
        else if( i_file == bkglist.size() ){
          filepath = infilepath + "/"+filename_prefix + "_"+SkimName+"_data.root";
          current_sample = "data";
	  cout << "filepath =  " << filepath <<endl;
	  
        }
        //==== signal starting from i_file = bkglist.size()+1
        else{

          int signal_index = i_file-bkglist.size()-1;

          //==== if cut is optimized cut, only draw that signal
          if(HistPath[i_cut].Contains("cutHN")){
            TString tmpcut = "_cutHN"+TString::Itoa(signal_mass[signal_index],10);
            if(HistPath[i_cut]!=tmpcut) continue;
          }
          //==== else, follow signal_draw
          else{
            if(!signal_draw[signal_index]) continue;
          }

          if(DoDebug) cout << "signal_index = " << signal_index << " => mass = " << signal_mass[signal_index] << endl;

          TString WhichChannel = "MuMu1";
          if(HistPath[i_cut].Contains("DiElectron")) WhichChannel = "ElEl";
          if(HistPath[i_cut].Contains("EMu")) WhichChannel = "MuEl";

          TString WhichChannel_for_tex = WhichChannel;

          //==== TChannel
          if( signal_mass[signal_index] < 0 ){
            WhichChannel = "HeavyNeutrinoTo"+WhichChannel+"_Tchannel_M";
          }
          else{
            WhichChannel = "HN"+WhichChannel+"_";
          }
          TString string_signal_mass = WhichChannel+TString::Itoa(abs(signal_mass[signal_index]),10);

          signal_name_for_tex = "SchHN"+WhichChannel_for_tex+TString::Itoa(abs(signal_mass[signal_index]),10);
          if(signal_mass[signal_index] < 0 ) signal_name_for_tex = "TchHN"+WhichChannel_for_tex+TString::Itoa(abs(signal_mass[signal_index]),10);

          filepath = "./rootfiles/"+data_class+"/Signal/"+filename_prefix+"_SK"+string_signal_mass+filename_suffix;
          //cout << filepath << endl;
          current_sample = string_signal_mass;
        }
	
        if(DoDebug){
          cout
          << "filepath = " << filepath << endl
          << "hisname = " << HistPath[i_cut]+"/"+HistNames[i_var] << endl;
        }
        
        //==== get root file
        if(gSystem->AccessPathName(filepath)){
          if(DoDebug){
            cout << "No file : " << filepath << endl;
          }
          continue;
        }
        TFile* file = new TFile(filepath);
        if( !file ){
          if(DoDebug){
            cout << "No file : " << filepath << endl;
          }
          continue;
        }

        TDirectory *dir = (TDirectory *)file->Get(DirName);
        if(!dir){
          if(DoDebug){
            cout << "No Directory : " << file->GetName() << "\t" << DirName << endl;
            file->ls();
          }
          file->Close();
          delete file;
          continue;
        }
        file->cd(DirName);

        //==== full histogram name
        TString fullhistname = HistNames[i_var];
        
        //==== get histogram
        TH1D* hist_temp = (TH1D*)dir->Get(fullhistname);
        if(!hist_temp || hist_temp->GetEntries() == 0){
          if(DoDebug){
            cout << "No histogram : " << current_sample << endl;
          }
          file->Close();
          delete file;
          continue;
        }
        AnyEntry = true;

        //==== set histogram name, including sample name
        hist_temp->SetName(fullhistname+"_"+current_sample);

        //==== rebin here
	
	vector<double> vrebin= GetRebinVariableBins();
	if(vrebin.size() > 1){

	  if(DoDebug){
	    cout << "Using Variable binning" << endl;
	    for(auto rb : vrebin) cout << "--- " << rb << endl;
	  }

	  double TMParray[vrebin.size()];
	  std::copy(vrebin.begin(), vrebin.end(), TMParray);
	  hist_temp = (TH1D *)hist_temp->Rebin(vrebin.size()-1, "hnew1", TMParray);
	}
	else hist_temp->Rebin( n_rebin() );
        
      
        //==== set X-axis range
        SetXaxisRange(hist_temp);

        //==== make overflows bins
        TH1D *hist_final = MakeOverflowBin(hist_temp);
	
	
	double ScaleBkg = (i_file < bkglist.size()) ? GetScale(bkglist[i_file]) : 1 ;
	//cout << "Scale " << bkglist[i_file]  << endl;
	//	cout << "Scalinging histogram " << bkgScale[i_file] << endl;
	hist_final->Scale(ScaleBkg);
	

        //==== Remove Negative bins
        TAxis *xaxis = hist_final->GetXaxis();
        for(int ccc=1; ccc<=xaxis->GetNbins(); ccc++){
          if(DoDebug) cout << current_sample << "\t["<<xaxis->GetBinLowEdge(ccc) <<", "<<xaxis->GetBinUpEdge(ccc) << "] : " << hist_final->GetBinContent(ccc) << endl;
          if(hist_final->GetBinContent(ccc)<0){
            hist_final->SetBinContent(ccc, 0.);
            hist_final->SetBinError(ccc, 0.);
          }
        }
	
        TString current_MCsector = "";
        //==== Set Attributes here
        //==== bkg
        if( i_file < bkglist.size() ){
          //==== get which MC sector
          current_MCsector = find_MCsector();
          int n_bins = hist_final->GetXaxis()->GetNbins();
          if(!MC_stacked_allerr){

            const Double_t *xcopy=hist_final->GetXaxis()->GetXbins()->GetArray();
            MC_stacked_allerr = new TH1D("MC_stacked_allerr", "", n_bins, xcopy);
            MC_stacked_staterr = new TH1D("MC_stacked_staterr", "", n_bins, xcopy);

          }
          hist_final->SetFillColor(map_sample_string_to_legendinfo[current_MCsector].second);
          hist_final->SetLineColor(map_sample_string_to_legendinfo[current_MCsector].second);

	  //==== MC Norm Scaling

          //==== Add star error histogram now, just before adding systematic
          MC_stacked_staterr->Add(hist_final);

          //==== Now Add systematic to histograms

          double ThisSyst = 0.;
          if( current_sample.Contains("fake") ) ThisSyst = 0.3;
          else if( current_sample.Contains("chargeflip") ) ThisSyst = 0.1;
          else{
            double mcnorm = 0.1;
	    double lumi =  0.04;///analysisInputs.CalculatedSysts["Luminosity"];
            ThisSyst = sqrt( mcnorm*mcnorm + lumi*lumi );
	  }

          for(int i=1; i<=n_bins; i++){

            double error_syst = ThisSyst*(hist_final->GetBinContent(i));
            double error_sumw2 = hist_final->GetBinError(i);
            double error_combined = sqrt( error_syst*error_syst + error_sumw2*error_sumw2 );

            hist_final->SetBinError(i, error_combined);
          }

          MC_stacked->Add(hist_final);
          MC_stacked_allerr->Add(hist_final);
        }

        //==== data for i_file = bkglist.size()
        else if( i_file == bkglist.size() ){
          hist_final->SetMarkerStyle(20);
          hist_final->SetMarkerSize(1.6);
          TString temp_hist_name(hist_final->GetName());
          hist_data = (TH1D*)hist_final->Clone();
        }
        //==== signal starting from i_file = bkglist.size()+1
        else if( i_file > bkglist.size() ){
          int signal_index = i_file-bkglist.size()-1;
          if(DoDebug) cout << "signal index = " << signal_index << ", mass = " << signal_mass[signal_index] << endl;
          hist_final->SetLineColor(signal_color[signal_index]);
          hist_final->SetLineWidth(3);
          if( signal_mass[signal_index] < 0 ) hist_final->SetLineStyle(3);
          TString temp_hist_name(hist_final->GetName());
          //hist_final->SetName(temp_hist_name+"_signal_"+TString::Itoa(signal_mass[signal_index], 10));
          //hist_final->SetName(temp_hist_name+"_signal_"+TString::Itoa(signal_mass[signal_index], 10));
          //==== scaling signal
          double this_coupling_constant = coupling_constant(signal_mass[signal_index]);
/*
  if( HistPath[i_cut].Contains("High_OneFatJet") || histname[i_var].Contains("fj") ){
            this_coupling_constant *= 0.1;
          }
*/
          hist_final->Scale( k_factor*this_coupling_constant/(1.*TMath::Power(10,log_of_generation_mixing)) );
          hist_signal.push_back( (TH1D*)hist_final->Clone() );
          signal_survive_mass.push_back(signal_mass[signal_index]);
        }
        else{
          cout << "[Warning] attirubte setting, i_file > total sample size? This should not happen!" << endl;
        }

	
	fill_legend(lg, hist_final);

        if(HistNames[i_var]=="NJets"){
          cout << "current_sample = " << current_sample << endl;
          TString alias = "";
          if(current_sample.Contains("data")) alias = "data";
          else if(current_sample.Contains("HN")) alias = signal_name_for_tex;
          else{
            alias = map_sample_string_to_legendinfo[current_MCsector].first;
          }
          cout << "==> alias = " << alias << endl;

          if( map_hist_y.find(alias) == map_hist_y.end() ){
            map_hist_y[alias] = new TH1D(alias, "", 1, 0., 1);
          }

          map_hist_y[alias]->Add(hist_final);
        }
        
        file->Close();
        delete file;
        
        if(DoDebug) cout << "end of this sample" << endl;
        
      } // END loop over samples

      if(HistNames[i_var]=="NJets"){
        MakeTexFile(map_hist_y,HistNames[i_var]);
      }

      if(!AnyEntry) continue;
      if(DoDebug) cout << "[Draw Canvas]" << endl;
      
      if(!drawdata.at(i_cut) && hist_data){
        TString tmpname = hist_data->GetName();
        hist_data = (TH1D*)MC_stacked_allerr->Clone();
        hist_data->SetName(tmpname);
        hist_data->SetMarkerStyle(20);
        hist_data->SetMarkerSize(1.6);
        hist_data->SetMarkerColor(kBlack);
        hist_data->SetLineColor(kBlack);
      }
      
      draw_canvas(MC_stacked, MC_stacked_staterr, MC_stacked_allerr, hist_data, hist_signal, lg, drawdata.at(i_cut), outputfile);

      //==== legend is already deleted in draw_canvas()
      //delete lg; 
      
    } // END loop over variables

    outputfile->Close();
    system("rm "+thiscut_plotpath+"/hists.root");


  } // END loop over cuts
  
  
}

void HNL_Efficiency_Plotter::make_bkglist(){
  
  if(bkglist.size() > 0) {
    cout << "NOTE make_bkglist called multiple times, exiting..." << endl;
    return;
  }
  
  for(unsigned int i=0; i<samples_to_use.size(); i++){
    MCsector_first_index.push_back( bkglist.size() );
    if(DoDebug) cout << "[make_bkglist] " << "MCsector_first_index.push_back(" <<  bkglist.size() << ")" << endl;
    bkglist.insert(bkglist.end(),
                   map_sample_string_to_list[samples_to_use.at(i)].begin(),
                   map_sample_string_to_list[samples_to_use.at(i)].end()
                   );

  }

  cout << "We will use :" << endl;
  for(unsigned int i=0; i<bkglist.size(); i++) cout << " " << bkglist[i] << endl;
}

void HNL_Efficiency_Plotter::SetRebins(TString filepath){

  cout << "[HNL_Efficiency_Plotter::SetRebins] Get rebins from " << filepath << endl;

  map< TString, map<TString, int> > ALL_rebins;

  string elline;
  ifstream in(filepath);
  while(getline(in,elline)){
    std::istringstream is( elline );
    TString cut;
    TString histname;
    double value;
    is >> cut;
    is >> histname;
    is >> value;
    (ALL_rebins[cut])[histname] = value;
  }

  path_rebins = filepath+"tmp/";
  gSystem->mkdir(path_rebins, kTRUE);
  for(map< TString, map<TString, int> >::iterator thiscut=ALL_rebins.begin(); thiscut!=ALL_rebins.end(); thiscut++){
    ofstream skeleton_rebins(path_rebins+thiscut->first, ios::trunc);
    for(map<TString, int>::iterator thishistname=(thiscut->second).begin(); thishistname!=(thiscut->second).end(); thishistname++){
      skeleton_rebins<<(thishistname->first)<<"\t"<<(thishistname->second)<<endl;
    }
    skeleton_rebins.close();
  }

}


void HNL_Efficiency_Plotter::SetYAxis(TString filepath){

  cout << "[HNL_Efficiency_Plotter::SetYAxis] Get Yaxis from " << filepath << endl;

  map< TString, map<TString, double> > ALL_y_maxs;

  string elline;
  ifstream in(filepath);
  while(getline(in,elline)){
    std::istringstream is( elline );
    TString cut;
    TString histname;
    double value;
    is >> cut;
    is >> histname;
    is >> value;
    (ALL_y_maxs[cut])[histname] = value;
  }

  path_y_axis = filepath+"tmp/";
  gSystem->mkdir(path_y_axis, kTRUE);
  for(map< TString, map<TString, double> >::iterator thiscut=ALL_y_maxs.begin(); thiscut!=ALL_y_maxs.end(); thiscut++){
    ofstream skeleton_y_maxs(path_y_axis+thiscut->first, ios::trunc);
    for(map<TString, double>::iterator thishistname=(thiscut->second).begin(); thishistname!=(thiscut->second).end(); thishistname++){
      skeleton_y_maxs<<(thishistname->first)<<"\t"<<(thishistname->second)<<endl;
    }
    skeleton_y_maxs.close();
  }
  
}

void HNL_Efficiency_Plotter::SetXAxis(TString filepath){

  cout << "[HNL_Efficiency_Plotter::SetXAxis] Get Xaxis from " << filepath << endl;

  map< TString, map<TString, vector<double>> > ALL_x_axis;

  string elline;
  ifstream in(filepath);
  while(getline(in,elline)){
    std::istringstream is( elline );
    TString cut;
    TString histname;
    double value_min, value_max;
    is >> cut;
    is >> histname;
    is >> value_min;
    is >> value_max;
    (ALL_x_axis[cut])[histname].push_back(value_min);
    (ALL_x_axis[cut])[histname].push_back(value_max);
  }

  path_x_axis = filepath+"tmp/";

  gSystem->mkdir(path_x_axis, kTRUE);
  for(map< TString, map<TString, vector<double>> >::iterator thiscut=ALL_x_axis.begin(); thiscut!=ALL_x_axis.end(); thiscut++){
    ofstream skeleton_x_axis(path_x_axis+thiscut->first, ios::trunc);
    for(map<TString, vector<double>>::iterator thishistname=(thiscut->second).begin(); thishistname!=(thiscut->second).end(); thishistname++){
      skeleton_x_axis<<(thishistname->first)<<"\t"<<(thishistname->second).at(0)<<"\t"<<(thishistname->second).at(1)<<endl;
    }
    skeleton_x_axis.close();
  }

}

vector<double> HNL_Efficiency_Plotter::GetRebinVariableBins(){

  TString cut = HistPath[i_cut];
  TString var = HistNames[i_var];

  map< TString, vector<double> >::iterator it = temp_vrebins.find( var );
  if( it != temp_vrebins.end() ){
    vector<double> TORETURN = it->second;
    return TORETURN;
  }
  else return {1.};
}

void HNL_Efficiency_Plotter::MakeRebins(){

  temp_rebins.clear();
  temp_vrebins.clear();
  cout << "Rebins.size() = " << Rebins.size() << endl;
  //  if(Rebins.size() > 0){
  //  for(unsigned int i =0; i < Rebins.size(); i++) temp_rebins[HistNames[i]] = Rebins[i];
  //  return;
  // }
  
  if(Rebins.size() > 0){ 
    for(unsigned int i =0; i < Rebins.size(); i++)  {
      temp_vrebins[HistNames[i]] = Rebins[i];       
      if (Rebins[i].size()<1) temp_rebins[HistNames[i]] = 1;
      else  if (Rebins[i].size()== 1) temp_rebins[HistNames[i]] = Rebins[i].at(0);
      else temp_rebins[HistNames[i]] = -1;
    }
  }
  

  string elline;
  ifstream in(path_rebins+HistPath[i_cut]);
  while(getline(in,elline)){
    std::istringstream is( elline );
    TString histname;
    double value;
    is >> histname;
    is >> value;
    //==== if -999, don't set
    if(value!=-999){
      temp_rebins[histname] = value;
    }
  }

}

void HNL_Efficiency_Plotter::MakeYAxis(){


 temp_y_maxs.clear();
 
 
 if(Ymaxs.size() > 0){
   for(unsigned int i =0; i < Ymaxs.size(); i++) temp_y_maxs[HistNames[i]] = Ymaxs[i];
   return;
 }


  string elline;
  ifstream in(path_y_axis+HistPath[i_cut]);
  while(getline(in,elline)){
    std::istringstream is( elline );
    TString histname;
    double value;
    is >> histname;
    is >> value;
    //==== if -999, don't set
    if(value!=-999){
      temp_y_maxs[histname] = value;
    }
  }

}

void HNL_Efficiency_Plotter::MakeXAxis(){

  temp_x_mins.clear();
  temp_x_maxs.clear();

  if(Xmaxs.size() > 0){
    for(unsigned int i =0; i < Xmins.size(); i++) temp_x_mins[HistNames[i]] = Xmins[i];
    for(unsigned int i =0; i < Xmaxs.size(); i++) temp_x_maxs[HistNames[i]] = Xmaxs[i];
    return;
  }

  string elline;
  ifstream in(path_x_axis+HistPath[i_cut]);
  while(getline(in,elline)){
    std::istringstream is( elline );
    TString histname;
    double value_min, value_max;
    is >> histname;
    is >> value_min;
    is >> value_max;
    //==== if -999, don't set
    if(value_min!=-999){
      temp_x_mins[histname] = value_min;
    }
    if(value_max!=-999){
      temp_x_maxs[histname] = value_max;
    }
  }

}

TString HNL_Efficiency_Plotter::find_MCsector(){
  for(unsigned int i=0; i<MCsector_first_index.size()-1; i++){
    if(MCsector_first_index.at(i) <= i_file && i_file < MCsector_first_index.at(i+1)){
      if(DoDebug) cout << "[find_MCsector] returned MCsector is " << samples_to_use.at(i) << endl;
      return samples_to_use.at(i);
    }
  }
  if(DoDebug) cout << "[find_MCsector] returned MCsector is " << samples_to_use.back() << endl;
  return samples_to_use.back();
}

void HNL_Efficiency_Plotter::clear_legend_info(){
  hist_for_legend_bkg.clear();
  hist_for_legend_signal.clear();
  MCsector_survive.clear();
  for(int i=0; i<samples_to_use.size(); i++){
    MCsector_survive[samples_to_use.at(i)] = false;
  }

}

double HNL_Efficiency_Plotter::coupling_constant(int mass){

  TString cut = HistPath[i_cut];

  double scale(1.);
  if(CurrentSC==SR2){  }

  if( coupling_constants.find( make_pair(cut, mass) ) != coupling_constants.end() ){
    if(DoDebug) cout << "cut = " << cut << ", mass = " << mass << " => coupling constant = " << coupling_constants[make_pair(cut, mass)] << endl;
    return coupling_constants[make_pair(cut, mass)];
  }
  else return TMath::Power(10,log_of_generation_mixing);

}

void HNL_Efficiency_Plotter::fill_legend(TLegend* lg, TH1D* hist){
  //==== here, hist_for_legned = {"A", "B", "D"}
  //==== now, push_back data and signal to make
  //==== hist_for_legned = {"A", "B", "D", "data", "HN40", "HN50", "HN60"}
  
  //==== bkg
  if( i_file < bkglist.size() ){
    TString current_MCsector = find_MCsector();
    if(DoDebug) cout << "[fill_legend] current_MCsector is " << current_MCsector << endl;
    if( !MCsector_survive[current_MCsector] ){
      if(DoDebug) cout << "[fill_legend] => is saved" << endl;
      hist_for_legend_bkg.push_back((TH1D*)hist->Clone());
      MCsector_survive[current_MCsector] = true;
    }
  }
  //==== data
  else if( i_file == (int)bkglist.size() ){
    hist_for_legend_data = (TH1D*)hist->Clone();
    if(DoDebug) cout << "Data added in hist_for_legend" << endl;
  }
  //==== signals
  else if( i_file > (int)bkglist.size() ){
    hist_for_legend_signal.push_back((TH1D*)hist->Clone());
    if(DoDebug) cout << "Signal added in hist_for_legend" << endl;
  }
  else{
    cout << "[Warning] fill_legend, i_file > total sample size? This should not happen!" << endl;
  }


}

void HNL_Efficiency_Plotter::draw_legend(TLegend* lg, bool DrawData){
  // Example :
  //                      0    1    2    3
  // samples_to_use   = {"A", "B", "C", "D"}
  // MCsector_survive = { T ,  T ,  F ,  T }
  //
  //                       0    1    2      3        4          5
  // hist_for_legend are {"A", "B", "D", "data", "signal1", "signal2"}
  // i_data = 6 - 2 - 1 = 3
  
  if(DoDebug) cout << "[draw_legend] printing MCsector_survive" << endl;
  for(auto it = MCsector_survive.begin(); it != MCsector_survive.end(); ++it){
    if(DoDebug) cout << "[draw_legend] " << it->first << " is " << it->second << endl;
  }
  for(int i=samples_to_use.size()-1, j=hist_for_legend_bkg.size()-1; i>=0; i--){
    if(MCsector_survive[samples_to_use.at(i)]){
      if(DoDebug) cout << "[draw_legend] " << samples_to_use.at(i) << " is added in legend" << endl;
      lg->AddEntry(hist_for_legend_bkg.at(j), map_sample_string_to_legendinfo[samples_to_use.at(i)].first, "f");
      j--;
    }
  }

  //==== Signal
  if(CurrentSC==no_class){
    for(unsigned int i=0; i<signal_survive_mass.size(); i++){
      int this_mass = signal_survive_mass.at(i);
      if( find(signal_mass.begin(), signal_mass.end(), this_mass) != signal_mass.end()){
        lg->AddEntry(hist_for_legend_signal.at(i), legend_coupling_label(this_mass), "l");
      }
    }

  }
  else{

    for(int i_sigcl=0; i_sigcl<AllSignalClasses.size(); i_sigcl++){

      signal_class this_cl = AllSignalClasses.at(i_sigcl);

      if(CurrentSC==this_cl){

        for(unsigned int i=0; i<signal_survive_mass.size(); i++){
          int this_mass = signal_survive_mass.at(i);
          if( find(map_class_to_signal_mass[this_cl].begin(), map_class_to_signal_mass[this_cl].end(), this_mass) != map_class_to_signal_mass[this_cl].end()){
            lg->AddEntry(hist_for_legend_signal.at(i), legend_coupling_label(this_mass), "l");
          }
        }

      }

    }

  }

  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->Draw();
}


void HNL_Efficiency_Plotter::draw_comp_canvas(TH1D *hist_def, TH1D *hist_comp, TLegend *legend ,TFile *outputf){

  if(!hist_def) return;  
  if(!hist_comp) return;  
  

  //==== y=0 line                                                                                                                                            
  double x_0[2], y_0[2];
  x_0[0] = -5000;  y_0[0] = 0;
  x_0[1] = 5000;  y_0[1] = 0;
  TGraph *g0 = new TGraph(2, x_0, y_0);
  //==== y=1 line                                                                                                                                            
  double x_1[2], y_1[2];
  x_1[0] = 5000;  y_1[0] = 1;
  x_1[1] = -5000;  y_1[1] = 1;
  TGraph *g1 = new TGraph(2, x_1, y_1);

  //==== If we draw data, prepare top/bottom pads                                                                                                            
  TCanvas* c1 = new TCanvas(FullHistNames[i_var], "", 800, 800);
  c1->Draw();
  c1->cd();
  
  TPad *c1_up;
  TPad *c1_down;
  c1_up = new TPad("c1", "", 0, 0.25, 1, 1);
  c1_down = new TPad("c1_down", "", 0, 0, 1, 0.25);
  
  canvas_margin(c1, c1_up, c1_down);
  
  c1_up->Draw();
  c1_down->Draw();
  c1_up->cd();


  //==== empty histogram for axis                                                                                                                            
  TH1D *hist_empty = (TH1D*)hist_def->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");

  //=== get dX                                                                                                                                               
  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();
  TString YTitle;
  if(Rebins[0].size() > 1) YTitle = "Events / bin";
  else  YTitle = DoubleToString(dx);

  hist_empty->GetYaxis()->SetTitle(YTitle);
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.2;

  hist_empty->Draw("histsame");
  //==== hide X Label for top plot                                                                                                                           
  hist_empty->GetXaxis()->SetLabelSize(0);

  //==== bkg                                                                                                                                                 
  if(!hist_def){
    cout << "[No Background]" << endl;
    return;
  }


  //////-----
  hist_def->Draw("histsame");
  hist_comp->Draw("histsame");

  //==== ymax                                                                                                                                                
  double AutoYmax = max( GetMaximum(hist_def), GetMaximum(hist_comp) );
  hist_empty->GetYaxis()->SetRangeUser( Ymin, YmaxScale*AutoYmax );

  //==== legend                                                                                                                                              
  c1_up->cd();
  legend->AddEntry(hist_def ,  LegendNames[0] + " [1]", "l");
  legend->AddEntry(hist_comp , LegendNames[i_var+1]+ " [2]", "l");
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->Draw();

  hist_empty->Draw("axissame");

  //==== Ratio          
  //==== MC-DATA                                                                                                                                           
  c1_down->cd();

  TString name_suffix = hist_def->GetName();

  TH1D *ratio_point = (TH1D *)hist_def->Clone();
  ratio_point->SetName(name_suffix+"_central");

  TH1D *tmp_ratio_point = (TH1D *)hist_def->Clone();
  tmp_ratio_point->Divide(hist_comp);
  TGraphAsymmErrors *gr_ratio_point = new TGraphAsymmErrors(tmp_ratio_point);
  gr_ratio_point->SetName("gr_"+name_suffix+"_central");
  gr_ratio_point->SetLineWidth(2.0);
  gr_ratio_point->SetMarkerSize(0.);
  gr_ratio_point->SetLineColor(kBlack);

  TH1D *ratio_staterr = (TH1D *)hist_def->Clone();
  ratio_staterr->SetName(name_suffix+"_staterr");
  TH1D *ratio_allerr = (TH1D *)hist_def->Clone();
  ratio_allerr->SetName(name_suffix+"_allerr");


  ratio_allerr->SetMaximum(2.);
  ratio_allerr->SetMinimum(0.);
  ratio_allerr->GetXaxis()->SetTitle(x_title[0]);
  ratio_allerr->GetYaxis()->SetTitle("Ratio [1]/[2]");
  ratio_allerr->SetFillColor(kOrange);
  ratio_allerr->SetMarkerSize(0);
  ratio_allerr->SetMarkerStyle(0);
  ratio_allerr->SetLineColor(kWhite);
  ratio_allerr->Draw("E2same");
  hist_axis(hist_empty, ratio_allerr);

  ratio_staterr->SetFillColor(kCyan);
  ratio_staterr->SetMarkerSize(0);
  ratio_staterr->SetMarkerStyle(0);
  ratio_staterr->SetLineColor(kWhite);
  ratio_staterr->Draw("E2same");

  ratio_point->Draw("p9histsame");
  gr_ratio_point->Draw("p0same");

  TLegend *lg_ratio = new TLegend(0.7, 0.8, 0.9, 0.9);
  //lg_ratio->SetFillStyle(0);                                                                                                                             
  //lg_ratio->SetBorderSize(0);                                                                                                                            
  lg_ratio->SetNColumns(2);
  lg_ratio->AddEntry(ratio_staterr, "Stat.", "f");
  lg_ratio->AddEntry(ratio_allerr, "Stat.+Syst.", "f");
  //lg_ratio->AddEntry(ratio_point, "Obs./Pred.", "p");                                                                                                    
  //lg_ratio->Draw();

  ratio_allerr->Draw("axissame");

  //==== y=1 line                                                                                                                                          
  g1->Draw("same");             

  //==== write lumi on the top                                                                                                                               
  c1->cd();

  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");

  mkdir(thiscut_plotpath);
  c1->SaveAs(thiscut_plotpath+"/"+HistNames[0]+"_vs_"+HistNames[i_var+1]+".pdf");
  c1->SaveAs(thiscut_plotpath+"/"+HistNames[0]+"_vs_"+HistNames[i_var+1]+".png");
  outputf->cd();
  c1->Write();

  TString FixName = HistNames[0]+"_vs_"+ HistNames[i_var+1];

  if(CopyToWebsite)SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistNames[0]+"_vs_"+HistNames[i_var+1],"",AnalyserName,MacroName,Era);

  delete legend;
  delete c1;  


}


void HNL_Efficiency_Plotter::draw_canvas(THStack *mc_stack, TH1D *mc_staterror, TH1D *mc_allerror, TH1D *hist_data, vector<TH1D *> hist_signal, TLegend *legend, bool DrawData, TFile *outputf){

  if(!hist_data) return;

  //==== signal_class
  CurrentSC = no_class;
  
  //==== y=0 line
  double x_0[2], y_0[2];
  x_0[0] = -5000;  y_0[0] = 0;
  x_0[1] = 5000;  y_0[1] = 0;
  TGraph *g0 = new TGraph(2, x_0, y_0);
  //==== y=1 line
  double x_1[2], y_1[2];
  x_1[0] = 5000;  y_1[0] = 1;
  x_1[1] = -5000;  y_1[1] = 1;
  TGraph *g1 = new TGraph(2, x_1, y_1);

  //==== If we draw data, prepare top/bottom pads
  TCanvas* c1 = new TCanvas(HistNames[i_var], "", 800, 800);
  c1->Draw();
  c1->cd();

  TPad *c1_up;
  TPad *c1_down;
  c1_up = new TPad("c1", "", 0, 0.25, 1, 1);
  c1_down = new TPad("c1_down", "", 0, 0, 1, 0.25);

  if(drawratio.at(i_cut)){
    canvas_margin(c1, c1_up, c1_down);

    c1_up->Draw();
    c1_down->Draw();
    c1_up->cd();
  }
  else{
    canvas_margin(c1);
  }

  //==== HOTFIX Rebin zero-bkgd
  bool IsMergeZeroBackground = false;
  if(MergeZeroBins){
    vector<double> new_binval = GetRebinZeroBackground(mc_stack, mc_staterror, mc_allerror, hist_data, hist_signal);
    if(new_binval.size()>0){
      IsMergeZeroBackground = true;

      const int new_nbins = new_binval.size()-1;
      double new_binvals[new_nbins+1];
      for(int i=0;i<new_nbins+1;i++) new_binvals[i] = new_binval.at(i);

      THStack *new_mc_stack = new THStack("new_mc_stack", "");
      TList *list_stack = mc_stack->GetHists();
      for(int i=0; i<list_stack->Capacity(); i++){
        TH1D *this_hist = (TH1D *)list_stack->At(i);
        this_hist = (TH1D *)this_hist->Rebin(new_nbins, "hnew1", new_binvals);
        new_mc_stack->Add(this_hist);
      }
      mc_stack = new_mc_stack;

      mc_staterror = (TH1D *)mc_staterror->Rebin(new_nbins, "hnew1", new_binvals);
      mc_allerror = (TH1D *)mc_allerror->Rebin(new_nbins, "hnew1", new_binvals);
      hist_data = (TH1D *)hist_data->Rebin(new_nbins, "hnew1", new_binvals);

      for(unsigned int i=0; i<hist_signal.size(); i++){
        hist_signal.at(i) = (TH1D *)hist_signal.at(i)->Rebin(new_nbins, "hnew1", new_binvals);
      }
    }
  }

  //==== empty histogram for axis
  TH1D *hist_empty = (TH1D*)mc_stack->GetHists()->At(0)->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");
  //=== get dX
  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();
  TString YTitle = DoubleToString(dx);
  if(IsMergeZeroBackground) YTitle = "Events / bin";
  //  if(Rebins[i_cut].size() > 1) YTitle = "Events / bin";
  
  hist_empty->GetYaxis()->SetTitle(YTitle);
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.2;
  cout << UseLogy.size() << endl;
  if(UseLogy.at(i_cut)>0){
    Ymin = UseLogy.at(i_cut);
    YmaxScale = 10;
    hist_empty->SetMinimum(UseLogy.at(i_cut));

    if(drawratio.at(i_cut)) c1->SetLogy();
    else c1_up->SetLogy();
  }
  hist_empty->Draw("histsame");
  //==== hide X Label for top plot
  if(drawratio.at(i_cut)) hist_empty->GetXaxis()->SetLabelSize(0);
  
  //==== bkg
  if(!mc_stack->GetHists()){
    cout << "[No Background]" << endl;
    return;
  }
  mc_stack->Draw("histsame");

  //==== signal
  if(CurrentSC==no_class){

    for(unsigned int i=0; i<signal_survive_mass.size(); i++){
      int this_mass = signal_survive_mass.at(i);
      if( find(signal_mass.begin(), signal_mass.end(), this_mass) != signal_mass.end()){
        hist_signal[i]->Draw("histsame");
      }
    }

  }
  else{

    for(int i_sigcl=0; i_sigcl<AllSignalClasses.size(); i_sigcl++){

      signal_class this_cl = AllSignalClasses.at(i_sigcl);

      if(CurrentSC==this_cl){

        for(unsigned int i=0; i<signal_survive_mass.size(); i++){
          int this_mass = signal_survive_mass.at(i);
          if( find(map_class_to_signal_mass[this_cl].begin(), map_class_to_signal_mass[this_cl].end(), this_mass) != map_class_to_signal_mass[this_cl].end()){
            hist_signal[i]->Draw("histsame");
          }
        }

      }

    }

  }

  //==== background err
  mc_allerror->SetMarkerColorAlpha(kAzure-9, 0);
  mc_allerror->SetFillStyle(3013);
  mc_allerror->SetFillColor(kBlack);
  mc_allerror->SetLineColor(0);
  mc_allerror->Draw("sameE2");

  //==== Draw Data at last
  vector<float> err_up_tmp;
  vector<float> err_down_tmp;
  const double alpha = 1 - 0.6827;
  TGraphAsymmErrors *gr_data = new TGraphAsymmErrors(hist_data);
  for(int i=0; i<gr_data->GetN(); ++i){
    int N = gr_data->GetY()[i];
    double L =  (N==0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,N,1.));
    double U =  (N==0) ? ( ROOT::Math::gamma_quantile_c(alpha,N+1,1) ) : ( ROOT::Math::gamma_quantile_c(alpha/2,N+1,1) );
    if( N!=0 ){
      gr_data->SetPointEYlow(i, N-L );
      gr_data->SetPointEXlow(i, 0);
      gr_data->SetPointEYhigh(i, U-N );
      gr_data->SetPointEXhigh(i, 0);
      err_down_tmp.push_back(N-L);
      err_up_tmp.push_back(U-N);
     }
    else{
      double zerodata_err_low = 0.1;
      double zerodata_err_high = 1.8;

      double xlow = gr_data->GetX()[i]-gr_data->GetEXlow()[i];
      double xhigh = gr_data->GetX()[i]+gr_data->GetEXhigh()[i];
      if(ZeroDataCheckCut(xlow,xhigh)){
        zerodata_err_low = 0.;
        zerodata_err_high = 0.;
      }

      gr_data->SetPointEYlow(i, zerodata_err_low);
      gr_data->SetPointEXlow(i, 0.);
      gr_data->SetPointEYhigh(i, zerodata_err_high);
      gr_data->SetPointEXhigh(i, 0.);
      err_down_tmp.push_back(zerodata_err_low);
      err_up_tmp.push_back(zerodata_err_high);
    }
  }
  gr_data->SetLineWidth(2.0);
  gr_data->SetMarkerSize(0.);
  gr_data->SetMarkerColor(kBlack);
  gr_data->SetLineColor(kBlack);
  hist_data->Draw("phistsame");
  gr_data->Draw("p0same");

  //==== ymax
  double AutoYmax = max( GetMaximum(gr_data), GetMaximum(mc_allerror) );
  //hist_empty->GetYaxis()->SetRangeUser( default_y_min, y_max() );
  hist_empty->GetYaxis()->SetRangeUser( Ymin, YmaxScale*AutoYmax );

  //==== legend
  legend->AddEntry(mc_allerror, "Stat.+syst. uncert.", "f");
  if(DrawData){
    legend->AddEntry(hist_data, "Data", "pe");
  }
  else{
    legend->AddEntry(hist_data, "Total Background", "pe");
  }
  if(drawratio.at(i_cut)) c1_up->cd();
  draw_legend(legend, DrawData);
  hist_empty->Draw("axissame");

  //==== Ratio

  if(!drawratio.at(i_cut)){
    hist_axis(hist_empty);
    hist_empty->GetXaxis()->SetTitle(x_title[i_var]);
  }
  else{
    //==== MC-DATA
    c1_down->cd();
    TString name_suffix = hist_data->GetName();
    TH1D *ratio_point = (TH1D *)hist_data->Clone();
    ratio_point->SetName(name_suffix+"_central");

    TH1D *tmp_ratio_point = (TH1D *)hist_data->Clone();
    tmp_ratio_point->Divide(mc_allerror);
    TGraphAsymmErrors *gr_ratio_point = new TGraphAsymmErrors(tmp_ratio_point);
    gr_ratio_point->SetName("gr_"+name_suffix+"_central");
    gr_ratio_point->SetLineWidth(2.0);
    gr_ratio_point->SetMarkerSize(0.);
    gr_ratio_point->SetLineColor(kBlack);

    TH1D *ratio_staterr = (TH1D *)hist_data->Clone();
    ratio_staterr->SetName(name_suffix+"_staterr");
    TH1D *ratio_allerr = (TH1D *)hist_data->Clone();
    ratio_allerr->SetName(name_suffix+"_allerr");
    for(int i=1; i<=ratio_point->GetXaxis()->GetNbins(); i++){
      //==== FIXME for zero? how?
      if(mc_allerror->GetBinContent(i)!=0){

        //==== ratio point
        //==== BinContent = Data/Bkgd
        //==== BinError = DataError/Bkgd
        ratio_point->SetBinContent( i, ratio_point->GetBinContent(i) / mc_allerror->GetBinContent(i) );
        ratio_point->SetBinError ( i, ratio_point->GetBinError(i) / mc_allerror->GetBinContent(i) );

        if(err_down_tmp.at(i-1)  !=0.) {
          gr_ratio_point->SetPointEYlow(i-1, err_down_tmp.at(i-1) / mc_allerror->GetBinContent(i) );
          gr_ratio_point->SetPointEXlow(i-1, 0);
          gr_ratio_point->SetPointEYhigh(i-1, err_up_tmp.at(i-1) /mc_allerror->GetBinContent(i));
          gr_ratio_point->SetPointEXhigh(i-1, 0);
        }
        else{
          gr_ratio_point->SetPointEYlow(i-1, 0);
          gr_ratio_point->SetPointEXlow(i-1, 0);
          gr_ratio_point->SetPointEYhigh(i-1, 1.8 / mc_allerror->GetBinContent(i));
          gr_ratio_point->SetPointEXhigh(i-1, 0);
        }
        //==== ratio staterr
        //==== BinContent = 1
        //==== BinError = Bkgd(Stat)Error/Bkgd
        ratio_staterr->SetBinContent( i, 1. );
        ratio_staterr->SetBinError( i, mc_staterror->GetBinError(i)/ mc_staterror->GetBinContent(i) );
        //==== ratio allerr
        //==== BinContent = 1
        //==== BinError = Bkgd(Stat+Syst)Error/Bkgd
        ratio_allerr->SetBinContent( i, 1. );
        ratio_allerr->SetBinError( i, mc_allerror->GetBinError(i)/ mc_allerror->GetBinContent(i) );
      }
    }

    ratio_allerr->SetMaximum(2.);
    ratio_allerr->SetMinimum(0.);
    ratio_allerr->GetXaxis()->SetTitle(x_title[i_var]);
    ratio_allerr->GetYaxis()->SetTitle("#frac{Obs.}{Pred.}");
    ratio_allerr->SetFillColor(kOrange);
    ratio_allerr->SetMarkerSize(0);
    ratio_allerr->SetMarkerStyle(0);
    ratio_allerr->SetLineColor(kWhite);
    ratio_allerr->Draw("E2same");
    hist_axis(hist_empty, ratio_allerr);

    ratio_staterr->SetFillColor(kCyan);
    ratio_staterr->SetMarkerSize(0);
    ratio_staterr->SetMarkerStyle(0);
    ratio_staterr->SetLineColor(kWhite);
    ratio_staterr->Draw("E2same");

    ratio_point->Draw("p9histsame");
    gr_ratio_point->Draw("p0same");

    TLegend *lg_ratio = new TLegend(0.7, 0.8, 0.9, 0.9);
    //lg_ratio->SetFillStyle(0);
    //lg_ratio->SetBorderSize(0);
    lg_ratio->SetNColumns(2);
    lg_ratio->AddEntry(ratio_staterr, "Stat.", "f");
    lg_ratio->AddEntry(ratio_allerr, "Stat.+Syst.", "f");
    //lg_ratio->AddEntry(ratio_point, "Obs./Pred.", "p");
    lg_ratio->Draw();

    ratio_allerr->Draw("axissame");

    //==== y=1 line
    g1->Draw("same");
  }

  //==== write lumi on the top
  c1->cd();

  if(!MakePaperPlot){
    TLatex latex_CMSPriliminary, latex_Lumi;
    latex_CMSPriliminary.SetNDC();
    latex_Lumi.SetNDC();
    latex_CMSPriliminary.SetTextSize(0.035);
    latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
    latex_Lumi.SetTextSize(0.035);
    if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
    if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
    if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
    if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
    if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
    if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)"); 

    TString str_channel = GetStringChannelRegion(LeptonChannels.at(i_cut), RegionType.at(i_cut));
    TLatex channelname;
    channelname.SetNDC();
    channelname.SetTextSize(0.037);
    channelname.DrawLatex(0.2, 0.88, str_channel);
  }
  else{
    //==== This is for Paper
    TLatex latex_CMSPriliminary, latex_Lumi;
    latex_CMSPriliminary.SetNDC();
    latex_Lumi.SetNDC();
    latex_CMSPriliminary.SetTextSize(0.050);
    latex_CMSPriliminary.DrawLatex(0.20, 0.89, "#font[62]{CMS}");
    latex_Lumi.SetTextSize(0.035);
    latex_Lumi.SetTextFont(42);
    if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.72, 0.96, "19.5 fb^{-1} (13 TeV)");
    if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.72, 0.96, "16.8 fb^{-1} (13 TeV)");
    if(Era=="2016") latex_Lumi.DrawLatex(0.72, 0.96, "36.3 fb^{-1} (13 TeV)");
    if(Era=="2017") latex_Lumi.DrawLatex(0.72, 0.96, "41.5 fb^{-1} (13 TeV)");
    if(Era=="2018") latex_Lumi.DrawLatex(0.72, 0.96, "59.9 fb^{-1} (13 TeV)");
    if(Era=="Run2") latex_Lumi.DrawLatex(0.72, 0.96, "137.9 fb^{-1} (13 TeV)");


  }

  mkdir(thiscut_plotpath);
  c1->SaveAs(thiscut_plotpath+"/"+HistNames[i_var]+".pdf");
  c1->SaveAs(thiscut_plotpath+"/"+HistNames[i_var]+".png");
  outputf->cd();
  c1->Write();

  HistResults.push_back(thiscut_plotpath+"/"+HistNames[i_var]+".pdf");

  TString FixName = HistPath[i_cut];
  FixName = FixName.ReplaceAll("/","_");
  LxplusHistResults.push_back("https://jalmond.web.cern.ch/jalmond/SNU/WebPlots/HNL/"+AnalyserName+"/" + MacroName +"/"+ Era +"/" +FixName + "/"+HistNames[i_var]+".pdf");


  if(CopyToWebsite)SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistNames[i_var],HistPath[i_cut],AnalyserName,MacroName,Era);
  
  delete legend;
  delete c1;
}

int HNL_Efficiency_Plotter::n_rebin(){
  
  TString cut = HistPath[i_cut];
  TString var = HistNames[i_var];
  
  map< TString, int >::iterator it = temp_rebins.find( var );
  if( it != temp_rebins.end() ){
    int TORETURN = it->second;
    //temp_rebins.erase( it );
    if(DoDebug) cout << "cut = " << cut << ", var = " << var << " => rebins = " << TORETURN << endl;
    return TORETURN;
  }
  else return 1;
  
}

double HNL_Efficiency_Plotter::y_max(){
  
  TString cut = HistPath[i_cut];
  TString var = HistNames[i_var];

  map< TString, double >::iterator it = temp_y_maxs.find( var );
  if( it != temp_y_maxs.end() ){
    //cout << "cut = " << cut << ", var = " << var << " => rebins = " << temp_rebins[cut] << endl;
    double TORETURN = it->second;
    //temp_y_maxs.erase( it );
    return TORETURN;
  }
  else return default_y_max;

}

void HNL_Efficiency_Plotter::SetXaxisRange(TH1D* hist){
  
  if(FullHistNames.size() > 0) {
    hist->GetXaxis()->SetRangeUser(Xmins[0], Xmaxs[0]);
    return;
  }
  TString cut = HistPath[i_cut];
  TString var = HistNames[i_var];
  
  double this_x_min = hist->GetXaxis()->GetBinLowEdge(1);
  double this_x_max = hist->GetXaxis()->GetBinUpEdge( hist->GetXaxis()->GetNbins() );

  map< TString, double >::iterator it = temp_x_mins.find( var );
  if( it != temp_x_mins.end() ){
    //cout << "cut = " << cut << ", var = " << var << " => x_min = " << temp_x_mins[var] << endl;
    this_x_min = it->second;
    //temp_x_mins.erase( it );
  }
  it = temp_x_maxs.find( var );
  if( it != temp_x_maxs.end() ){
    //cout << "cut = " << cut << ", var = " << var << " => x_max = " << temp_x_maxs[var] << endl;
    this_x_max = it->second;
    //temp_x_maxs.erase( it );
  }
  
  hist->GetXaxis()->SetRangeUser(this_x_min, this_x_max);
}

void HNL_Efficiency_Plotter::SetXaxisRange(THStack* mc_stack){
  
  TString cut = HistPath[i_cut];
  TString var = HistNames[i_var];
  
  double this_x_min = mc_stack->GetXaxis()->GetBinLowEdge(1);
  double this_x_max = mc_stack->GetXaxis()->GetBinUpEdge( mc_stack->GetXaxis()->GetNbins() );
  
  map< TString, double >::iterator it = temp_x_mins.find( var );
  if( it != temp_x_mins.end() ){
    //cout << "cut = " << cut << ", var = " << var << " => x_min = " << temp_x_mins[var] << endl;
    this_x_min = it->second;
    //temp_x_mins.erase( it );
  }
  it = temp_x_maxs.find( var );
  if( it != temp_x_maxs.end() ){
    //cout << "cut = " << cut << ", var = " << var << " => x_max = " << temp_x_maxs[var] << endl;
    this_x_max = it->second;
    //temp_x_maxs.erase( it );
  }
 
  mc_stack->GetXaxis()->SetRangeUser(this_x_min, this_x_max);
}

void HNL_Efficiency_Plotter::SetXaxisRangeBoth(THStack* mc_stack, TH1D* hist){

  TString cut = HistPath[i_cut];
  TString var = HistNames[i_var];
  
  double this_x_min = mc_stack->GetXaxis()->GetBinLowEdge(1);
  double this_x_max = mc_stack->GetXaxis()->GetBinUpEdge( mc_stack->GetXaxis()->GetNbins() );
  
  map< TString, double >::iterator it = temp_x_mins.find( var );
  if( it != temp_x_mins.end() ){
    //cout << "cut = " << cut << ", var = " << var << " => x_min = " << temp_x_mins[var] << endl;
    this_x_min = it->second;
    //temp_x_mins.erase( it );
  }
  it = temp_x_maxs.find( var );
  if( it != temp_x_maxs.end() ){
    //cout << "cut = " << cut << ", var = " << var << " => x_max = " << temp_x_maxs[var] << endl;
    this_x_max = it->second;
    //temp_x_maxs.erase( it );
  }

  mc_stack->GetXaxis()->SetRangeUser(this_x_min, this_x_max);
  hist->GetXaxis()->SetRangeUser(this_x_min, this_x_max);
}

TH1D* HNL_Efficiency_Plotter::MakeOverflowBin(TH1D* hist){

  //==== 0    1                                    n_bin_origin
  //====      |---------------------------------------|
  //====             bin_first      bin_last
  //====                |-------------|  
  //==== |   |                                        |   |
  //==== under                                         over
  //==== flow                                          flow
  //==== |<------------>|             |<----------------->|
  //====  all underflow                   all overflow

  //==== Original NBins
  int n_bin_origin = hist->GetXaxis()->GetNbins();
  //==== Changed NBins
  int bin_first = hist->GetXaxis()->GetFirst();
  int bin_last = hist->GetXaxis()->GetLast();
  int n_bin_inrange = bin_last-bin_first+1;
  
  TString XTitle=hist->GetXaxis()->GetTitle();
  TString YTitle=hist->GetYaxis()->GetTitle();

  vector<TString> labels;
  

  double x_first_lowedge = hist->GetXaxis()->GetBinLowEdge(bin_first);
  double x_last_upedge = hist->GetXaxis()->GetBinUpEdge(bin_last);

  double Allunderflows = hist->Integral(0, bin_first-1);
  double Allunderflows_error = hist->GetBinError(0);
  Allunderflows_error = Allunderflows_error*Allunderflows_error;

  if(SetLabels){
    for(unsigned int i=1; i<= n_bin_origin+1; i++){
      labels.push_back(hist->GetXaxis()->GetBinLabel(i));
    }
  }
  
  for(unsigned int i=1; i<=bin_first-1; i++){
    Allunderflows_error += (hist->GetBinError(i))*(hist->GetBinError(i));
  }
  Allunderflows_error = sqrt(Allunderflows_error);

  double Alloverflows = hist->Integral(bin_last+1, n_bin_origin+1);
  double Alloverflows_error = hist->GetBinError(n_bin_origin+1);
  Alloverflows_error = Alloverflows_error*Alloverflows_error;
  for(unsigned int i=bin_last+1; i<=n_bin_origin; i++){
    Alloverflows_error += (hist->GetBinError(i))*(hist->GetBinError(i));
  }
  Alloverflows_error = sqrt(Alloverflows_error);

  //==== Make X-bin array
  Double_t temp_xbins[n_bin_inrange+1];
  int counter=0;
  for(int i=bin_first;i<=bin_last;i++){
    temp_xbins[counter] = hist->GetXaxis()->GetBinLowEdge(i);
    counter++;
  }
  temp_xbins[n_bin_inrange+1-1] = hist->GetXaxis()->GetBinUpEdge(bin_last);
  const Double_t *xcopy=temp_xbins;

  TH1D *hist_out = new TH1D(hist->GetName(), hist->GetTitle(), n_bin_inrange, xcopy);
  for(unsigned int i=1; i<=n_bin_inrange; i++){
    double this_content = hist->GetBinContent(bin_first-1+i);
    double this_error = hist->GetBinError(bin_first-1+i);
    //cout << "["<<hist_out->GetXaxis()->GetBinLowEdge(i)<<", "<<hist_out->GetXaxis()->GetBinUpEdge(i)<<"] : "<<this_content<<endl;

    //==== underflows
    if(i==1){
      this_content += Allunderflows;
      this_error = TMath::Sqrt( this_error*this_error + Allunderflows_error*Allunderflows_error );
    }

    //==== overflows
    if(i==n_bin_inrange){
      this_content += Alloverflows;
      this_error = TMath::Sqrt( this_error*this_error + Alloverflows_error*Alloverflows_error );
    }
    
    hist_out->SetBinContent(i, this_content);
    hist_out->SetBinError(i, this_error);
  }
  
  if(SetLabels){
    for(unsigned int i=1; i<= n_bin_origin+1; i++){
      hist_out->GetXaxis()->SetBinLabel(i,labels[i-1]);
    }
  }


  return hist_out;
  
}

TString HNL_Efficiency_Plotter::DoubleToString(double dx){

  //cout << "[HNL_Efficiency_Plotter::DoubleToString] var = " << HistNames[i_var] << endl;
  //cout << "[HNL_Efficiency_Plotter::DoubleToString] unit = " << units[i_var] << endl;
  //cout << "[HNL_Efficiency_Plotter::DoubleToString] dx = " << dx << endl;

  //==== onebin
  if(units[i_var]=="int"){
    return "Events";
  }
  else{

    int dx_int = int(dx);
    int dx_first_two_digits = int((dx-dx_int)*100);
    int dx_first_three_digits = int((dx-dx_int)*1000);

    //==== has integer
    if(dx_int!=0){
      //==== but no digits
      if(dx_first_two_digits==0){
        return "Events / "+TString::Itoa(dx_int,10)+" "+units[i_var];
      }
      //=== also has digits
      else{
        return "Events / "+TString::Itoa(dx_int,10)+"."+TString::Itoa(dx_first_two_digits,10)+" "+units[i_var];
      }
    }
    //=== no integer
    else{
      return "Events / 0."+TString::Itoa(dx_first_two_digits,10)+" "+units[i_var];
    }
  }

  return "Events";

}


TString HNL_Efficiency_Plotter::legend_coupling_label(int mass){
 
  //cout << "mass = " << mass << endl;
  //cout << " coupling = " << coupling_const.at(signal_survive_index[mass]) << endl;
  double log_scale = TMath::Log10(coupling_constant(mass)/(1.*TMath::Power(10,log_of_generation_mixing)));
  double log_coupling = TMath::Log10(coupling_constant(mass));
  //cout << " log coupling = " << log_scale << endl;

  TString channel = "s-ch.";
  if(mass<0) channel = "t-ch.";

  mass = abs(mass);

  TString V2 = "#||{V_{N#mu}}^{2}";
  if(PrimaryDataset[i_cut]=="DoubleEG") V2 = "#||{|V_{Ne}}^{2}";
  if(PrimaryDataset[i_cut]=="MuonEG") V2 = "#||{V_{Nl}}^{2}";

  //if(log_coupling == 0) return channel+" HN"+TString::Itoa(mass, 10)+", "+V2+"=1";
  //else return channel+" HN"+TString::Itoa(mass, 10)+", "+V2+"=10^{"+TString::Itoa(log_coupling, 10)+"}";

  if(log_coupling == 0) return "m_{N} = "+TString::Itoa(mass, 10)+" GeV, "+V2+"=1";
  else return "m_{N} = "+TString::Itoa(mass, 10)+" GeV, "+V2+"=10^{"+TString::Itoa(log_coupling, 10)+"}";

  //==== FIXME FOR NEXT UPDATE
  //if(log_coupling == 0) return "m_{N} = "+TString::Itoa(mass, 10)+" GeV ("+channel+"), "+V2+"=1";
  //else return "m_{N} = "+TString::Itoa(mass, 10)+" GeV ("+channel+"), "+V2+"=10^{"+TString::Itoa(log_coupling, 10)+"}";

}

void HNL_Efficiency_Plotter::mkdir(TString path){
  
  if( !gSystem->mkdir(path, kTRUE) ){
    cout
    << "###################################################" << endl
    << "Directoy " << path << " is created" << endl
    << "###################################################" << endl
    << endl;
  }
  
}

double HNL_Efficiency_Plotter::GetHistValue(TH1D * ht, TString hn){
  
  if(hn=="NEvents") return ht->GetBinContent(1);
  else return ht->Integral(0,ht->GetNbinsX()+1);
  
}

double HNL_Efficiency_Plotter::GetHistError(TH1D * ht, TString hn){

  if(hn=="NEvents") return ht->GetBinError(1);
  else {
    Double_t err;
    Double_t hint = ht->IntegralAndError(0,ht->GetNbinsX()+1, err);
    return err;
  }
  return 0;

}
TString HNL_Efficiency_Plotter::FixLatex(TString origSt){
  TString fixSt=origSt;
  if(fixSt.Contains("P_T")) fixSt.ReplaceAll("P_T","$\\mathrm{P}_{T}$");
  return fixSt;
}
void HNL_Efficiency_Plotter::MakeTexFile(map< TString, TH1D * > hs, TString Hist_For_CutFlow){

  if(DoDebug) cout << "Running MakeTexFile : size = " << hs.size() << endl; 
  
  TString texfilepath = thiscut_plotpath+"/tex/";
  mkdir(texfilepath);
  CutFlowResults.push_back(texfilepath+"/Yields.pdf");
  ofstream ofile_tex(texfilepath+"/Yields.tex",ios::trunc);
  ofile_tex.setf(ios::fixed,ios::floatfield);
  ofile_tex << "\\documentclass[10pt]{article}" << endl;
  ofile_tex << "\\usepackage{epsfig,subfigure,setspace,xtab,xcolor,array,colortbl}" << endl;
  ofile_tex << "\\begin{document}" << endl;
  ofile_tex << "\\input{"+texfilepath+"/Table.txt}" << endl;
  ofile_tex << "\\end{document}" << endl;

  ofstream ofile(texfilepath+"/Table.txt",ios::trunc);
  ofile.precision(3);
  ofile.setf(ios::fixed,ios::floatfield); 
  ofile << "\\begin{table}[!tbh]" << endl;
  ofile << "  \\caption{" << endl;
  ofile << "    Yields in Region " << FixLatex(RegionType.at(i_cut)) << endl;
  ofile << "  }" << endl;
  ofile << "  \\begin{center}" << endl;
  ofile << "    \\begin{tabular}{c|c}" << endl;
  ofile << "\\hline" << endl;
  ofile << " & Yields \\\\" << endl;
  ofile << "\\hline" << endl;
  ofile << "\\hline" << endl;

  TH1D *hist_bkgd = new TH1D("hist_bkgd", "", 1., 0., 1.);
  bool HasSignal = false;
  for(map< TString, TH1D * >::iterator it = hs.begin(); it != hs.end(); it++){
    TString name = it->first;

    if(name == "X + #gamma") name = "$X + \\gamma$";
    if(name == "Z + #gamma") name = "Z $+ \\gamma$";
    if(name == "W + #gamma") name = "W $+ \\gamma$";
    if(name == "top + #gamma") name = "top $+ \\gamma$";
    if(name == "ttbar") name = "$\\mathrm{t}  \\bar{t}$";
    if(name == "ttbar_ll") name = "$\\mathrm{t}  \\bar{t}$";
    if(name.Contains("data")) continue;
    if(name.Contains("HN")){
      HasSignal = true;
      continue;
    }
    TH1D *h_bkgd = it->second;
    ofile << name << " & $"<<GetHistValue(h_bkgd,Hist_For_CutFlow)<<" \\pm "<<GetHistError(h_bkgd,Hist_For_CutFlow)<<"$ \\\\" << endl;
    cout << name << " & $"<<GetHistValue(h_bkgd,Hist_For_CutFlow)<<" \\pm "<<GetHistError(h_bkgd,Hist_For_CutFlow)<<"$ \\\\" << endl;
    hist_bkgd->Add(h_bkgd);
  }
  ofile << "\\hline" << endl;
  ofile << "Total & $" << GetHistValue(hist_bkgd,Hist_For_CutFlow) << " \\pm " << GetHistError(hist_bkgd,Hist_For_CutFlow) << "$ \\\\" << endl;
  ofile << "\\hline" << endl;
  
  TH1D *hist_data = hs["data"];
  if(!drawdata.at(i_cut) && hist_data)         hist_data = (TH1D*) hist_bkgd->Clone();
  

  if(hist_data){
    ofile << "Data & $" << GetHistValue(hist_data,Hist_For_CutFlow) <<  "$ \\\\" << endl;
    ofile << "\\hline" << endl;
  }
  else{
   TH1D *hist_empty = new TH1D("hist_data", "", 1, 0., 1.);
   hist_data = (TH1D*)hist_empty->Clone();
   ofile << "Data & $" << 0 << "$ \\\\" << endl;
   ofile << "\\hline" << endl;
  }

  double signif = (GetHistValue(hist_data,Hist_For_CutFlow) - GetHistValue(hist_bkgd,Hist_For_CutFlow)) / sqrt( (GetHistError(hist_bkgd,Hist_For_CutFlow)*GetHistError(hist_bkgd,Hist_For_CutFlow)) + (GetHistError(hist_data,Hist_For_CutFlow)*GetHistError(hist_data,Hist_For_CutFlow)) );

  ofile << "Significance & $" <<signif<<" \\sigma$ \\\\" << endl;

  if(HasSignal){
    ofile << "\\hline" << endl;
    for(map< TString, TH1D * >::iterator it = hs.begin(); it != hs.end(); it++){
    TString name = it->first;
    if(name.Contains("HN")){
      TH1D *h_bkgd = it->second;

      //==== name = SchHNElEl500
      TString SorT = "";
      if(name.Contains("Sch")) SorT = "S";
      else if(name.Contains("Tch")) SorT = "T";

      TString LeptonChannel = "";
      if(name.Contains("MuMu")) LeptonChannel = "MuMu";
      if(name.Contains("EE"))  LeptonChannel = "ElEl";
      if(name.Contains("EMu")) LeptonChannel = "ElMu";

      TString MassString = name;
      MassString.Replace(0,9,"");
      int ThisMass = MassString.Atoi();
      if(SorT=="T") ThisMass = -ThisMass;

      double log_coupling = TMath::Log10(coupling_constant(ThisMass));

      name = SorT+"-channel "+LeptonChannel+" $"+MassString+"$ GeV, $V^{2} = 10^{"+TString::Itoa(log_coupling,10)+"}$";

      ofile << name+" & $" << GetHistValue(hist_bkgd,Hist_For_CutFlow) << " \\pm " << GetHistError(hist_bkgd,Hist_For_CutFlow) << "$ \\\\" << endl;
      }
    }
  }

  ofile << "\\hline" << endl;
  ofile << "\\hline" << endl;
  ofile << "    \\end{tabular}" << endl;
  ofile << "  \\end{center}" << endl;
  ofile << "\\end{table}" << endl;

  system("latex "+texfilepath+"/Yields.tex");
  system("dvipdf Yields.dvi");
  system("rm *aux");
  system("rm *log");
  system("rm *dvi");
  system("mv Yields.pdf "+texfilepath);
  
  TString FixName = HistPath[i_cut];
  FixName = FixName.ReplaceAll("/","_");
  LxplusCutFlowResults.push_back("https://jalmond.web.cern.ch/jalmond/SNU/WebPlots/HNL/"+AnalyserName+"/"+ MacroName + "/"+ Era +"/" + FixName + "/Yields.pdf");

  if(CopyToWebsite) CopyLXPLUSCutFlow(texfilepath+"/Yields.pdf",HistPath[i_cut],AnalyserName,MacroName,Era);

}



void HNL_Efficiency_Plotter::Summary(){
  
  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
  cout << " "<< endl;
  cout << "HNL_Efficiency_Plotter Ran StackPlots : With Data. Results sent to : " << endl;

  if(CopyToWebsite){
    cout << "Hists sent to : "  << endl;
    for (auto i : LxplusHistResults) cout << i << endl;
    cout << "Yield table sent to : "  << endl;
    for (auto i : LxplusCutFlowResults) cout << i << endl;

  }

}


TString HNL_Efficiency_Plotter::GetStringChannelRegion(int A, TString B){

  //==== channel type

  TString channel = "";
  //==== A = 1 : mm
  //==== A = 2 : ee
  //==== A = 3 : em

  if(A==20) channel = "l^{#pm}l^{#pm}";
  if(A==21) channel = "#mu^{#pm}#mu^{#pm}";
  if(A==22) channel = "e^{#pm}e^{#pm}";
  if(A==23) channel = "e^{#pm}#mu^{#pm}";
  if(A==30) channel = "3l";
  if(A==40) channel = "4l";

  TString region = "";
  //==== B = 1 : Preselection 
  //==== B = 20 : Low
  //==== B = 21 : Low + two jet
  //==== B = 22 : Low + one jet
  //==== B = 30 : High
  //==== B = 31 : High + two jet
  //==== B = 32 : High + fat jet
  if(B=="1") region = "Preselection";
  else if(B=="31") region = "High Mass SR1";
  else if(B=="32") region = "High Mass SR2";
  else if(B=="WZ") region = "WZ CR";
  else region = B;

  //return channel+" "+region;
  return "#splitline{"+channel+"}{"+region+"}";


}

bool HNL_Efficiency_Plotter::ZeroDataCheckCut(double xlow, double xhigh){

  if(HistPath[i_cut].Contains("_Low_")){
    if(HistNames[i_var]=="m_lljj_lljjWclosest"){
      if(xlow>=300) return true;
    }
    if(HistNames[i_var]=="m_llj"){
      if(xlow>=300) return true;
    }
  }

  return false;

}

void HNL_Efficiency_Plotter::ScaleSample(TString samplename, double scale_value){

  map_sample_string_to_scale[samplename]=scale_value;
  

}
double HNL_Efficiency_Plotter::GetScale(TString samplename){

  map<TString, double>::iterator it = map_sample_string_to_scale.find(samplename);

  if(it != map_sample_string_to_scale.end()) {
    cout << "Scale " << samplename <<  " -->" << it->second << endl;
    return it->second;
  }
  cout << "Scale " <<samplename <<  " -->" << 1 << endl;

  return 1;
}

void HNL_Efficiency_Plotter::SetupSampleInfo(){

  map_sample_string_to_list.clear();
  map_sample_string_to_legendinfo.clear();


  map_sample_string_to_list["Prompt"] = {"Prompt"};

  map_sample_string_to_list["DY"] = {"DYJets","DYJets10to50_MG"};
  map_sample_string_to_list["WJets"] = {"WJets"};
  map_sample_string_to_list["VV_excl"] = {
    //"WZTo3LNu_mllmin01",
    "WZTo3LNu_powheg",
    "ZZTo4L_powheg", "ggZZto2e2mu", "ggZZto2e2nu", "ggZZto2e2tau", "ggZZto2mu2nu", "ggZZto2mu2tau", "ggZZto4e", "ggZZto4mu", "ggZZto4tau", "ggHtoZZ",
  };
  map_sample_string_to_list["VV_incl"] = {"WZ_pythia", "ZZ_pythia", "WW_pythia"};
  map_sample_string_to_list["WZ_excl"] = {"WZTo3LNu_powheg"};
  map_sample_string_to_list["ZZ_excl"] = {"ZZTo4L_powheg", "ggZZto2e2mu", "ggZZto2e2nu", "ggZZto2e2tau", "ggZZto2mu2nu", "ggZZto2mu2tau", "ggZZto4e", "ggZZto4mu", "ggZZto4tau"};
  map_sample_string_to_list["VVV"] = {"WWW", "WWZ", "WZZ", "ZZZ"};
  

  map_sample_string_to_list["ttbar"] = {"TTLL_powheg","TTLJ_powheg","SingleTop_tW_top_NoFullyHad","SingleTop_tW_antitop_NoFullyHad"};
  map_sample_string_to_list["ttbar_ll"] = {"TTLL_powheg"};
  map_sample_string_to_list["ttV"] = {"ttW", "ttZ", "ttH_nonbb"}; //FIXME ttH into ttV
  map_sample_string_to_list["ttH"] = {"ttH_nonbb"};
  map_sample_string_to_list["top"] = {"ttW", "ttZ", "ttH_nonbb"};
  map_sample_string_to_list["Xgamma"] = {"TG", "TTG", "ZGto2LG", "WGtoLNuG_weighted"};
  map_sample_string_to_list["WW_double"] = {"WWTo2L2Nu_DS", "WpWpEWK", "WpWpQCD"};
  map_sample_string_to_list["ttV_lep"] = {"ttWToLNu", "ttZToLL_M-1to10"};
  map_sample_string_to_list["NonPrompt"] = {"NonPrompt"};
  map_sample_string_to_list["chargeflip"] = {"chargeflip"};
  map_sample_string_to_list["Conv"] = {"Conv"};

  map_sample_string_to_legendinfo["Prompt"] = make_pair("Prompt", kGreen);
  map_sample_string_to_legendinfo["DY"] = make_pair("DY", kYellow);
  map_sample_string_to_legendinfo["WJets"] = make_pair("WJets", kGreen);
  map_sample_string_to_legendinfo["VV_excl"] = make_pair("diboson", kSpring-1);
  map_sample_string_to_legendinfo["VV_incl"] = make_pair("diboson", kSpring-1);
  map_sample_string_to_legendinfo["WZ_excl"] = make_pair("WZ", kGreen);
  map_sample_string_to_legendinfo["ZZ_excl"] = make_pair("ZZ", kRed-7);
  map_sample_string_to_legendinfo["VVV"] = make_pair("triboson", kSpring+10);
  map_sample_string_to_legendinfo["ttbar"] = make_pair("ttbar", kRed);
  map_sample_string_to_legendinfo["ttbar_ll"] = make_pair("ttbar", kRed);
  map_sample_string_to_legendinfo["ttV"] = make_pair("ttV", kOrange);
  map_sample_string_to_legendinfo["ttH"] = make_pair("ttH", kOrange);
  map_sample_string_to_legendinfo["top"] = make_pair("top", kRed);
  map_sample_string_to_legendinfo["Xgamma"] = make_pair("X + #gamma", kSpring-7);
  map_sample_string_to_legendinfo["Conv"] = make_pair("X + #gamma", kSpring-7);
  map_sample_string_to_legendinfo["Xgamma_noDY"] = make_pair("X + #gamma", kSpring-7);
  map_sample_string_to_legendinfo["WW_double"] = make_pair("DoubleWW", 74);
  map_sample_string_to_legendinfo["ttV_lep"] = make_pair("ttV", kOrange);
  map_sample_string_to_legendinfo["NonPrompt"] = make_pair("Misid. lepton bkgd.", 870);
  map_sample_string_to_legendinfo["chargeflip"] = make_pair("Mismeas. charge bkgd.", kYellow);
  map_sample_string_to_legendinfo["prompt"] = make_pair("Prompt bkgd.", kSpring-1);



}


void HNL_Efficiency_Plotter::LeptonChannel(TString ch){

  LeptonChannels.clear();
  for(int i=0; i < HistPath.size() ; i++){
    if(ch=="MuMu") LeptonChannels.push_back(21);
    if(ch=="EE") LeptonChannels.push_back(22);
    if(ch=="EMu") LeptonChannels.push_back(23);
    if(ch=="MuMuMu") LeptonChannels.push_back(24);
    if(ch=="EEE") LeptonChannels.push_back(25);
    if(ch=="EMuL") LeptonChannels.push_back(26);
  }
  
}
void HNL_Efficiency_Plotter::UseLogyAll(double v){
  UseLogy.clear();
  for(int i=0; i < HistPath.size() ; i++) UseLogy.push_back(v);
}

void HNL_Efficiency_Plotter::ApplyMCNormSFAll(bool v){
  ApplyMCNormSF.clear();
  for(int i=0; i < HistPath.size()  ; i++)ApplyMCNormSF.push_back(v);
}
void HNL_Efficiency_Plotter::DrawRatioAll(bool v){

  drawratio.clear();
  for(int i=0; i < HistPath.size()  ; i++) drawratio.push_back(v);
}

void HNL_Efficiency_Plotter::DrawDataAll(bool v){

  drawdata.clear();
  for(int i=0; i < HistPath.size()  ; i++) drawdata.push_back(v);
}

vector<double> HNL_Efficiency_Plotter::GetRebinZeroBackground(THStack *mc_stack, TH1D *mc_staterror, TH1D *mc_allerror, TH1D *hist_data, vector<TH1D *> &hist_signal){

  int original_nbins = mc_allerror->GetXaxis()->GetNbins();
  vector<double> original_binval;
  vector<double> new_binval;

  for(int i=1; i<=original_nbins; i++){
    original_binval.push_back(mc_allerror->GetXaxis()->GetBinLowEdge(i));
  }
  original_binval.push_back(mc_allerror->GetXaxis()->GetBinUpEdge(original_nbins));

  int next_nonzero_bin = 2;
  //==== PUSH1) first bin low-edge
  new_binval.push_back(original_binval.at(0));
  for(int i=2; i<=original_nbins; i++){
    if(mc_allerror->GetBinContent(i)>0){
      next_nonzero_bin = i;
      break;
    }
  }

  int next_zero_bin = -999;
  //cout << "[Plotter::GetRebinZeroBackground] mc_allerror->GetBinContent(original_nbins) = " << mc_allerror->GetBinContent(original_nbins) << endl;
  for(int i=next_nonzero_bin+1; i<=original_nbins; i++){

    //==== HOTFIX ee mll has empty bins at Z-peak
    if(HistNames[i_var]=="m_ll"){
      if(original_binval.at(i)<150) continue;
    }

    if(mc_allerror->GetBinContent(i)<=0){
      next_zero_bin = i;
      break;
    }
  }

  //cout << "[Plotter::GetRebinZeroBackground] next_zero_bin = " << next_zero_bin << endl;
  //==== PUSH Rest
  if(next_zero_bin<0){
    for(int j=next_nonzero_bin; j<original_binval.size(); j++){
      new_binval.push_back(original_binval.at(j));
    }
  }
  else{

    if(next_nonzero_bin+1==next_zero_bin){
      new_binval.push_back(original_binval.at( original_binval.size()-1 ));
    }
    else{

      for(int j=next_nonzero_bin; j<next_zero_bin-2; j++){
	new_binval.push_back(original_binval.at(j));
      }
      new_binval.push_back(original_binval.at( original_binval.size()-1 ));

    }

  }

  /*
    cout << "[Plotter::GetRebinZeroBackground] Printing new bin vals" << endl;
  for(int i=0;i<new_binval.size(); i++){
  cout << new_binval.at(i) << endl;
  }
  */

  return new_binval;

}


TString HNL_Efficiency_Plotter::Scan2DHists(TH2D* h1, TH2D* h2){
  
  //// scan h1 which is prompt  Fake vs CF

  double PromptIntegral = h1->Integral();
  double FakeIntegral   = h2->Integral();
  cout << "_________________________________________________________________________________________________________________________________________________________" << endl;
  double k_sob=0;
  TString IDOpt="";
  for(int ccc=1; ccc<=h1->GetXaxis()->GetNbins()+1; ccc++) {

    for(int ddd=1; ddd<=h1->GetYaxis()->GetNbins()+1; ddd++) {

      
      double sig = h1->Integral(ccc, h1->GetXaxis()->GetNbins()+1, ddd, h1->GetYaxis()->GetNbins()+1)/ PromptIntegral;
      double bkg = h2->Integral(ccc, h2->GetXaxis()->GetNbins()+1, ddd, h2->GetYaxis()->GetNbins()+1) / FakeIntegral ;
      double bkgN = h2->Integral(ccc, h2->GetXaxis()->GetNbins()+1, ddd, h2->GetYaxis()->GetNbins()+1)  ;
      double sob = sig / sqrt(sig*200 + bkgN+1);
      if(sob > k_sob){
	if(sig > 0.7){
	  k_sob=sob;
	  IDOpt="Fake_"+to_string(h1->GetXaxis()->GetBinLowEdge(ccc)) + "_CF_"+to_string(h1->GetYaxis()->GetBinLowEdge(ddd)) ;
	}
      }
      cout << "Fake MVA > "  << h1->GetXaxis()->GetBinLowEdge(ccc) << " CF MVA > " << h1->GetYaxis()->GetBinLowEdge(ddd) <<  "  Sig Eff = " << sig  <<  "  Fake Eff=" <<  bkg  <<  " sob = " << sob << endl;
      
    }
  }
  cout << "Opt bin = " << IDOpt << endl;

  return IDOpt;
}

TH1D* HNL_Efficiency_Plotter::GetCutEfficiency(TH1D *hist_default){

  TH1D *hist_new = new  TH1D ( "1bin","", 1, -1, 1);

  double Eff =  hist_default->GetBinContent(2)/ hist_default->GetBinContent(1);
  
  hist_new->SetBinContent(1, Eff);

  return hist_new;
}


TH1D* HNL_Efficiency_Plotter::GetScanEfficiency(TH1D *hist_default){

  TH1D *hist_new = (TH1D*)hist_default->Clone();
  
  double HistIntegral = hist_default->Integral();
  

  TAxis *xaxis = hist_default->GetXaxis();
  for(int ccc=1; ccc<=xaxis->GetNbins(); ccc++){

    //cout <<  "Scan " << ccc <<  " - " << xaxis->GetNbins()+1  <<"  " << hist_default->Integral(ccc, xaxis->GetNbins()+1) << " " << HistIntegral << endl;
    if(ccc==1){
      hist_new->SetBinContent(ccc, hist_default->Integral(ccc, xaxis->GetNbins()+1) / HistIntegral);
      hist_new->SetBinError(ccc, 0);
    }
    else{
      hist_new->SetBinContent(ccc, hist_default->Integral(ccc+1, xaxis->GetNbins()+1) / HistIntegral);
      hist_new->SetBinError(ccc, 0);

    }
  }
  //  hist_new->SetBinContent(, hist_default->Integral(ccc, xaxis->GetNbins()) / HistIntegral);
  //hist_new->SetBinError(ccc, 0);

  
  return hist_new;
}


TH1D* HNL_Efficiency_Plotter::GetScanEfficiency2D(TH1D *hist_default, TH1D *hist_comp){

  TH1D *hist_new = (TH1D*)hist_default->Clone();

  double HistIntegral = hist_comp->Integral();


  TAxis *xaxis = hist_default->GetXaxis();
  for(int ccc=1; ccc<=xaxis->GetNbins(); ccc++){

    //cout <<  "Scan " << ccc <<  " - " << xaxis->GetNbins()+1  <<"  " << hist_default->Integral(ccc, xaxis->GetNbins()+1) << " " << HistIntegral << endl;                                                                                  
    if(ccc==1){
      hist_new->SetBinContent(ccc, hist_default->Integral(ccc, xaxis->GetNbins()+1) / HistIntegral);
      hist_new->SetBinError(ccc, 0);
    }
    else{
      hist_new->SetBinContent(ccc, hist_default->Integral(ccc+1, xaxis->GetNbins()+1) / HistIntegral);
      hist_new->SetBinError(ccc, 0);

    }
  }
  //  hist_new->SetBinContent(, hist_default->Integral(ccc, xaxis->GetNbins()) / HistIntegral);                                                                                                                                             
  //hist_new->SetBinError(ccc, 0);                                                                                                                                                                                                          


  return hist_new;
}


void HNL_Efficiency_Plotter::draw_hists_canvas(vector<TH1D*> hists , vector<TString> legNames,  TString HistName,TString dirName){

  if(!hists[0]) return;


  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;


  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);


  TH1D* hist_default = hists[0];
  hist_axis(hist_default);

  TLegend *lg= new TLegend(0.65, 0.75, 0.9, 0.85);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.025);

  TLegend *lg2= new TLegend(0.4, 0.75, 0.65, 0.85);
  lg2->SetFillStyle(0);
  lg2->SetBorderSize(0);
  lg2->SetTextSize(0.025);

  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);

  TH1D *hist_empty = (TH1D*)hist_default->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.4;

  hist_empty->GetYaxis()->SetRangeUser(0., 1.4);
  hist_axis(hist_empty);

  hist_empty->Draw("histsame");

  TGraphAsymmErrors *gr1 = new TGraphAsymmErrors(hist_default);
  gr1->SetLineWidth(2.0);
  gr1->SetMarkerSize(0.);
  gr1->SetLineColor(kRed);
  gr1->Draw("plsame");
  lg->AddEntry(gr1, legNames[0], "pl");
  
  for(int ig =1; ig < hists.size(); ig++){
    TGraphAsymmErrors *gr = new TGraphAsymmErrors(hists[ig]);
    gr->SetLineWidth(2.0);
    if(ig >= hists.size() /2)    gr->SetLineStyle(2.0);

    gr->SetMarkerSize(0.);
    if(ig < hists.size() /2) gr->SetLineColor(HNL_Efficiency_Plotter::GetColor(ig));
    else gr->SetLineColor(HNL_Efficiency_Plotter::GetColor(ig- (hists.size() /2)));
    gr->Draw("plsame");
    if(ig >= hists.size() /2)    lg2->AddEntry(gr, legNames[ig], "pl");
    else     lg->AddEntry(gr, legNames[ig], "pl");
  }
  double x_1[2], y_1[2];
  x_1[0] = 5000;  y_1[0] = 1;
  x_1[1] = -5000;  y_1[1] = 1;
  TGraph *gr3 = new TGraph(2, x_1, y_1);
  gr3->Draw("same");


  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);
  latex_result.DrawLatex(0.2, 0.9,HistName);


  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");

  //  lg->SetNColumns(2);
  lg->Draw();
  lg2->Draw();
  mkdir(thiscut_plotpath);

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");
  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);



}




void HNL_Efficiency_Plotter::draw_hists_canvas_pt(vector<TH1D*> hists , vector<TString> legNames,  TString HistName,TString dirName){

  if(!hists[0]) return;


  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;


  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);


  TH1D* hist_default = hists[0];
  hist_axis(hist_default);

  TLegend *lg= new TLegend(0.65, 0.75, 0.9, 0.85);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.025);

  TLegend *lg2= new TLegend(0.4, 0.75, 0.65, 0.85);
  lg2->SetFillStyle(0);
  lg2->SetBorderSize(0);
  lg2->SetTextSize(0.025);

  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);

  TH1D *hist_empty = (TH1D*)hist_default->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.5;

  if(hist_default->GetMaximum() > 1) YmaxScale = hist_default->GetMaximum()  *2;
  for(int ig =1; ig < hists.size(); ig++) if(hists[ig]->GetMaximum() > YmaxScale) YmaxScale = hists[ig]->GetMaximum()  *2;
  hist_empty->GetYaxis()->SetRangeUser(0., YmaxScale);
  hist_axis(hist_empty);

  hist_empty->Draw("histsame");

  gStyle->SetErrorX(0);

  TGraphAsymmErrors *gr1 = new TGraphAsymmErrors(hist_default);
  gr1->SetLineWidth(2.0);
  gr1->SetMarkerColor(HNL_Efficiency_Plotter::GetColor(0));
  gr1->SetLineColor(kRed);
  gr1->Draw("pl0same");
  lg->AddEntry(gr1, legNames[0], "pl");

  for(int ig =1; ig < hists.size(); ig++){
    TGraphAsymmErrors *gr = new TGraphAsymmErrors(hists[ig]);
    gr->SetLineWidth(2.0);
    gr->SetMarkerStyle(33+ig);
    //    gr->SetMarkerSize(0.);
    gr->SetMarkerColor(HNL_Efficiency_Plotter::GetColor(ig+1));
    gr->SetLineColor(HNL_Efficiency_Plotter::GetColor(ig+1));
    gr->Draw("pl0same");

    lg->AddEntry(gr, legNames[ig], "pl");
  }
  double x_1[2], y_1[2];
  x_1[0] = 5000;  y_1[0] = 1;
  x_1[1] = -5000;  y_1[1] = 1;
  TGraph *gr3 = new TGraph(2, x_1, y_1);
  gr3->Draw("same");


  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);
  latex_result.DrawLatex(0.2, 0.9,HistName);


  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");

  //  lg->SetNColumns(2);                                                                                                                                                                                                                                                                                                                                                                                                                                                                
  lg->Draw();
  lg2->Draw();
  mkdir(thiscut_plotpath);

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");
  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);



}



double  HNL_Efficiency_Plotter::draw_SvsB_canvas(vector<TH1D*> hists , vector<TString> IDs, vector<TString> legNames,  TString HistName, TString dirName){
  
  if(!hists[0]) return -1;
  
  
  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;


  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);


  TH1D* hist_default = hists[0];
  

  TLegend *lg;
  if(legNames.size() < 3 ) lg = new TLegend(0.55, 0.85, 0.93, 0.93);
  else lg = new TLegend(0.55, 0.80, 0.93, 0.93);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);


  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();
  
  canvas_margin(c1);
  
  TH1D *hist_empty = new TH1D("Empty","EMPTY", 100, 0., 100.);

  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.4;

  hist_empty->GetYaxis()->SetTitle("1- #epsilon_{Bkg}");
  hist_empty->GetXaxis()->SetTitle("#epsilon_{prompt}");
  hist_empty->GetXaxis()->SetRangeUser(0., 1.);
  hist_empty->GetYaxis()->SetRangeUser(0., 1.40);

  hist_axis(hist_empty);
  hist_empty->Draw("histsame");
  
  
  int Nbins = hists[0]->GetXaxis()->GetNbins()+1;
  
  double x[Nbins], y[Nbins], xlow[Nbins] ,xup[Nbins], ylow[Nbins], yup[Nbins];

  double Eff90S(0),Eff90B(0),Eff95S(0),Eff95B(0), Score95(0),Score90(0), Eff98S(0),Eff98B(0), Score98(0),Eff80S(0),Eff80B(0), Score80(0);
  TAxis *xaxis = hists[0]->GetXaxis();
  for(int ccc=1; ccc<=xaxis->GetNbins()+1; ccc++){
    x[ccc-1] = hists[0]->GetBinContent(ccc);
    y[ccc-1] = 1-hists[1]->GetBinContent(ccc);
    xlow[ccc-1] = 0;
    xup[ccc-1] = 0;
    ylow[ccc-1] = 0;
    yup[ccc-1] = 0;

    if(DoDebug) cout << ccc << " " << hists[0]->GetBinContent(ccc) << " " << hists[1]->GetBinContent(ccc) << endl;

    if(hists[0]->GetBinContent(ccc) < 0.98 * hists[0]->GetBinContent(1) && Eff98S == 0) {

      Eff98S = hists[0]->GetBinContent(ccc);
      Eff98B = hists[1]->GetBinContent(ccc);
      Score98 = hists[0]->GetBinCenter(ccc);
    }

    
    if(hists[0]->GetBinContent(ccc) < 0.95* hists[0]->GetBinContent(1) && Eff95S == 0) {

      Eff95S = hists[0]->GetBinContent(ccc);
      Eff95B = hists[1]->GetBinContent(ccc);
      Score95 = hists[0]->GetBinCenter(ccc);
    }
    if(hists[0]->GetBinContent(ccc) < 0.9* hists[0]->GetBinContent(1) && Eff90S ==0) {

      Eff90S = hists[0]->GetBinContent(ccc);
      Eff90B = hists[1]->GetBinContent(ccc);
      Score90 = hists[0]->GetBinCenter(ccc);
    }

    if(hists[0]->GetBinContent(ccc) < 0.8* hists[0]->GetBinContent(1) && Eff80S ==0) {

      Eff80S = hists[0]->GetBinContent(ccc);
      Eff80B = hists[1]->GetBinContent(ccc);
      Score80 = hists[0]->GetBinCenter(ccc);
    }

  }


  TGraphAsymmErrors* gr1 = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);

  gr1->SetLineWidth(2.0);
  gr1->SetMarkerSize(0.);
  gr1->SetLineColor(kRed);
  gr1->Draw("plsame");
  lg->AddEntry(gr1, legNames[0], "pl");

  /// Get AUC
  bool PlotAUC(true);
  double AUC_cum=0;
  TH1D* hAUC = new TH1D ("","", gr1->GetN(), 0, 1.);
  if(PlotAUC){

    auto nPoints = gr1->GetN(); // number of points in your TGraph
    Double_t ax[nPoints],ay[nPoints];

    for(int i=0; i < nPoints; ++i)        gr1->GetPoint(i, ax[i], ay[i]);
    for(int i=0; i < nPoints-1; ++i){
      double integral = (ax[i]  - ax[i+1]) * (ay[i] + ay[i+1])/2;
      AUC_cum=AUC_cum+integral;
    }
  }
  cout << "AUC integral = " << AUC_cum << endl;
  
  TString  AUC;
  if(PlotAUC) AUC = DToString(AUC_cum,4);
  
  int ileg(1);
  double HNTight_P(0);
  double HNTight_C(0);
  
  double HNTight_UL_P(0);
  double HNTight_UL_C(0);

  for(auto ID : IDs){
    cout << svsb_prompt_path << " " << svsb_prompt_hist +ID << endl;
    TH1D *hist_prompt = GetCutEfficiency(ConstructHist(svsb_prompt_path,  svsb_prompt_hist +ID));
    TH1D *hist_comp = GetCutEfficiency(ConstructHist(svsb_comp_path,    svsb_comp_hist  + ID ));

    double x2[1], y2[1], xlow2[1] ,xup2[1], ylow2[1], yup2[1];
    if (ID == "HNTightV2") {
      HNTight_P = hist_prompt->GetBinContent(1);
      HNTight_C = 1- hist_comp->GetBinContent(1);
    }
    if (ID.Contains("HNL_ULID")){
      HNTight_UL_P = hist_prompt->GetBinContent(1);
      HNTight_UL_C = 1- hist_comp->GetBinContent(1);
    }

    cout << "hist_prompt->GetBinContent(1) = " << hist_prompt->GetBinContent(1) << endl;
    cout << "hist_comp->GetBinContent(1) = " <<hist_comp->GetBinContent(1) << endl;
    x2[0]= hist_prompt->GetBinContent(1);
    y2[0]= 1- hist_comp->GetBinContent(1);
    xlow2[0]= 0;
    xup2[0]= 0;
    ylow2[0]= 0;
    yup2[0]= 0;
    TGraphAsymmErrors* gr2 = new TGraphAsymmErrors(1, x2, y2, xlow2, xup2, ylow2, yup2);
    
    gr2->SetLineWidth(2.0);
    gr2->SetMarkerSize(1.5);
    gr2->SetMarkerStyle(34);
    gr2->SetMarkerColor(GetColor(ileg));
    gr2->Draw("psame");
    lg->AddEntry(gr2, legNames[ileg], "p");
    ileg++;
  }

  double x_1[2], y_1[2];
  x_1[0] = 5000;  y_1[0] = 1;
  x_1[1] = -5000;  y_1[1] = 1;
  TGraph *gr3 = new TGraph(2, x_1, y_1);
  gr3->Draw("same");
  
  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);
  latex_result.DrawLatex(0.2, 0.9,HistName);
  if(PlotAUC)latex_result.DrawLatex(0.75, 0.75,"AUC = " +AUC);


  cout << "DoubleToString" << endl;
  TString POGout = "HNTightV2 ["+ DToString(HNTight_P)+ ","+ DToString(1-HNTight_C,3)+"]";
  latex_result.DrawLatex(0.2, 0.82,POGout);
  TString ULout = "HNL_ULID ["+ DToString(HNTight_UL_P)+ ","+ DToString(1-HNTight_UL_C,3)+"]";
  if(HNTight_UL_P > 0)latex_result.DrawLatex(0.2, 0.84,ULout);

  TString st98 = "#epsilon_{prompt} 98% : #epsilon_{bkg} = " + DToString(Eff98B,3) + " score = " + DToString(Score98,3);
  TString st95 = "#epsilon_{prompt} 95% : #epsilon_{bkg} = " + DToString(Eff95B,3) + " score = " + DToString(Score95,3);
  TString st90 = "#epsilon_{prompt} 90% : #epsilon_{bkg} = " + DToString(Eff90B,3) + " score = " + DToString(Score90,3);
  TString st80 = "#epsilon_{prompt} 80% : #epsilon_{bkg} = " + DToString(Eff80B,3) + " score = " + DToString(Score80,3);

  latex_result.DrawLatex(0.2, 0.8,st98);
  latex_result.DrawLatex(0.2, 0.78,st95);
  latex_result.DrawLatex(0.2, 0.76,st90);
  latex_result.DrawLatex(0.2, 0.74,st80);

  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");

  //  lg->SetNColumns(2);                                                                                                                                                                                                                             
  lg->Draw();
  mkdir(thiscut_plotpath);

  c1->SaveAs(thiscut_plotpath+"/"+HistName+"_SEffvsBEff.png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+"_SEffvsBEff.pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName+"_SEffvsBEff", dirName,AnalyserName,MacroName,Era);
  
  return AUC_cum;


}


void HNL_Efficiency_Plotter::MakeAUCHist(map<TString,double> AUCMap,TString HistName,TString dirName, TString Label){
  
  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);

  if(DoDebug) cout << "plotpath = " << plotpath << "  " << dirName << " --> " << thiscut_plotpath << endl;



  TH1D* h = new TH1D("","", AUCMap.size(), 0, AUCMap.size());
  
  int n(0);
  double minAUC(1);
  double maxAUC(0);
  TString string_maxAUC;
  for(auto imap : AUCMap) {
    n++;
    h->GetXaxis()->SetBinLabel(n, imap.first);
    h->SetBinContent(n, imap.second);
    h->GetYaxis()->SetTitle("AUC");
    if(imap.second < minAUC) minAUC = imap.second;
    if(imap.second > maxAUC){
      maxAUC = imap.second;
      string_maxAUC =imap.first;
    }
  }
  

  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();
  //  c1->SetLogy();
  canvas_margin(c1);

  TH1D *hist_empty = (TH1D*)h->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");
  
  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.2;
  

  if(minAUC > 0.95)hist_empty->GetYaxis()->SetRangeUser(minAUC-0.005,1);
  else   if(minAUC > 0.9)hist_empty->GetYaxis()->SetRangeUser(minAUC-0.02,1);
  else  hist_empty->GetYaxis()->SetRangeUser(minAUC-0.1,1);
  hist_axis(hist_empty);
  hist_empty->GetXaxis()->SetLabelSize(0.02);

  hist_empty->Draw("histsame");
  //  gStyle->SetPaintTextFormat(“4.3f”);
  h->Draw("textsame");
  
  TGraphAsymmErrors *gr_ratio_point = new TGraphAsymmErrors(h);
  gr_ratio_point->SetLineWidth(2.0);
  gr_ratio_point->SetMarkerSize(0.);
  gr_ratio_point->SetLineColor(kRed);
  gr_ratio_point->Draw("p0same");

  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");


  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);
  latex_result.DrawLatex(0.2, 0.9,Label);

  TString  AUClabel = "Max AUC : " + string_maxAUC+ " [" +TString(DToString(maxAUC,4)) +"]";
  latex_result.DrawLatex(0.2, 0.15,AUClabel);

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);

}

void HNL_Efficiency_Plotter::SaveHist(TH1D* hist, TString legname, TString HistName, TString dirName){

  cout    << "################### SaveHist [" << HistName << "]  ###################" << endl;
  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;
  

  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);

  TH1D* hist_default = hist;

  TLegend *lg = new TLegend(0.55, 0.80, 0.93, 0.93);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);

  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);

  TH1D *hist_empty= (TH1D*)hist_default->Clone();

  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->GetXaxis()->SetTitle("Lepton p_{T} (GeV)");
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.4;

  hist_axis(hist_empty);
  hist_empty->GetYaxis()->SetRangeUser(0, 1.4);
  hist_empty->GetXaxis()->SetRangeUser(15, 200);

  hist_empty->Draw("histsame");
  hist_default->Draw("histsame");
  lg->Draw();

  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");

  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);
  //  latex_result.DrawLatex(0.2, 0.9,Label);                                                                                                                                                                                                                                 

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);

}


void HNL_Efficiency_Plotter::SaveHists(vector<TH1D*>hists, vector<TString> legname, TString HistName, TString dirName, vector<TString> scales){

  cout    << "################### SaveHist [" << HistName << "]  ###################" << endl;
  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);

  TH1D* hist_default = hists[0];

  TLegend *lg = new TLegend(0.55, 0.80, 0.93, 0.93);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);

  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);

  TH1D *hist_empty= (TH1D*)hist_default->Clone();

  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 0;

  for(auto i : hists) {
    if(i->GetMaximum() > YmaxScale) YmaxScale = i->GetMaximum()*1.4;
  }
  hist_axis(hist_empty);

  hist_empty->GetYaxis()->SetRangeUser(0, YmaxScale);


  hist_empty->Draw("histsame");

  for(int i=0 ; i < hists.size(); i++){
    hists[i]->SetLineColor(HNL_Efficiency_Plotter::GetColor(i));
    hists[i]->SetLineWidth(3.);
    hists[i]->Draw("histsame");
    lg->AddEntry(hists[i], legname[i],"l");
  }
  lg->Draw();

  double x_1[2], y_1[2];
  x_1[0] = 5000;  y_1[0] = 1;
  x_1[1] = -5000;  y_1[1] = 1;
  TGraph *gr3 = new TGraph(2, x_1, y_1);
  gr3->Draw("same");


  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");

  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);

  for(unsigned int il =0 ; il < scales.size(); il++) latex_result.DrawLatex(0.2, 0.9-0.05*il, scales[il]);

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);

}

double HNL_Efficiency_Plotter::GetCutFromEff(TH2D* h , int xbin, double WP){

  TAxis *axis = h->GetYaxis();
  double HistIntegral = h->Integral(xbin,xbin,0,axis->GetNbins()+1);
  
  for(int ddd=1; ddd<= axis->GetNbins()+1; ddd++){
    double CumInt = h->Integral(xbin,xbin,ddd,axis->GetNbins()+1);

    if( (CumInt/HistIntegral) <= WP) {
      return axis->GetBinCenter(ddd);
      
    }
  }
  
  return -1;
}

Double_t HNL_Efficiency_Plotter::fitf(Double_t *x,Double_t *par) {

  Double_t fitval = par[0] - par[1]*TMath::Exp( -(x[0] / par[2]));
  return fitval;
}


void HNL_Efficiency_Plotter::Draw_MVA_WP(vector<TH2D*>hists, vector<TString> histsname,  double WP,TString HistName, TString dirName){

  cout    << "################### SaveHist [" << HistName << "]  ###################" << endl;
  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);


  //TH1D* hist_default = hists[0];

  TLegend *lg = new TLegend(0.55, 0.80, 0.93, 0.93);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);


  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);


  TH1D *hist_empty = new TH1D("Empty","EMPTY", 500, 0., 500.);

  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.4;

  hist_empty->GetYaxis()->SetTitle("MVA Score");
  hist_empty->GetXaxis()->SetTitle("Lepton pt");
  hist_empty->GetYaxis()->SetRangeUser(-1., 1.40);
  hist_empty->GetXaxis()->SetRangeUser(10., 200);

  hist_axis(hist_empty);
  hist_empty->Draw("histsame");


  cout << "Calculate Cut for " << WP*100 << " % " << endl;

  TH2D* h2 =  hists[0];
  int Nbins = h2->GetXaxis()->GetNbins();
  double x[Nbins], y[Nbins], xlow[Nbins] ,xup[Nbins], ylow[Nbins], yup[Nbins];
  
  for(int i=0 ; i < h2->GetXaxis()->GetNbins(); i++){
    
    double MVA_CUT = GetCutFromEff(h2, i, WP);

    x[i] = h2->GetXaxis()->GetBinCenter(i);
    if(MVA_CUT == -1) y[i] = -1;
    else y[i] = MVA_CUT;
    xlow[i] = 0;
    xup[i] = 0;
    ylow[i] = 0;
    yup[i] = 0;
  }
  lg->Draw();
  
  TF1 *func1 = new TF1("fitFcn1", "[0] - [1]*TMath::Exp( -( x  / [2]) )", 40, 150);
  TF1 *func2 = new TF1("fitFcn2", "[0] - [1]*TMath::Exp( -( x  / [2]) )", 10, 40);

  func1->SetParameters(0.56,2,10);  
  func2->SetParameters(0.6,2,10);  

  TGraphAsymmErrors* gr1 = new TGraphAsymmErrors(Nbins, x, y, xlow, xup, ylow, yup);

  //  gr1->Fit("fitFcn");

  gr1->SetLineWidth(2.0);
  gr1->SetMarkerSize(0.);
  gr1->SetLineColor(kRed);
  gr1->Draw("plsame");

  cout << "FIT" << endl;
  gr1->Fit("fitFcn1","R","",30,150);
  //gr1->Fit("fitFcn2","R+","",10,30);
  

  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");


  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);
  //  latex_result.DrawLatex(0.2, 0.9,Label);                                                                                                                                                                                                                                 

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);



  
}
void HNL_Efficiency_Plotter::SaveHistsWithRatio(vector<TH1D*>hists, vector<TString> legname, TString HistName, TString dirName, vector<TString> scales){

  cout    << "################### SaveHist [" << HistName << "]  ###################" << endl;
  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);

  TH1D* hist_default = hists[0];
  TH1D* hist_comp = hists[1];
  
  //==== y=0 line                                                                                                                                                 
  double x_0[2], y_0[2];
  x_0[0] = -5000;  y_0[0] = 0;
  x_0[1] = 5000;  y_0[1] = 0;
  TGraph *g0 = new TGraph(2, x_0, y_0);
  //==== y=1 line                                                                                                                                                 
  double x_1[2], y_1[2];
  x_1[0] = 5000;  y_1[0] = 1;
  x_1[1] = -5000;  y_1[1] = 1;
  TGraph *g1 = new TGraph(2, x_1, y_1);


  TLegend *lg = new TLegend(0.55, 0.70, 0.93, 0.853);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);

  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  TPad *c1_up;
  TPad *c1_down;
  c1_up = new TPad("c1", "", 0, 0.25, 1, 1);
  c1_down = new TPad("c1_down", "", 0, 0, 1, 0.25);

  canvas_margin(c1, c1_up, c1_down);

  c1_up->Draw();
  c1_down->Draw();
  c1_up->cd();



  TH1D *hist_empty= (TH1D*)hist_default->Clone();

  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 0;

  for(auto i : hists) {
    if(i->GetMaximum() > YmaxScale) YmaxScale = i->GetMaximum()*1.4;
  }
  hist_axis(hist_empty);
  hist_empty->GetYaxis()->SetRangeUser(0, YmaxScale);

  
  hist_empty->Draw("histsame");

  for(int i=0 ; i < hists.size(); i++){
    hists[i]->SetLineColor(HNL_Efficiency_Plotter::GetColor(i));
    hists[i]->SetLineWidth(3.);
    hists[i]->Draw("histsame");
    lg->AddEntry(hists[i], legname[i],"l");
  }
  lg->Draw();



  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");

  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);

  for(unsigned int il =0 ; il < scales.size(); il++) latex_result.DrawLatex(0.2, 0.85-0.05*il, scales[il]);
  //==== Ratio                                                                                                                                                    
  //==== MC-DATA                                                                                                                                                  
  c1_down->cd();

  TString name_suffix = hist_default->GetName();

  TH1D *ratio_point = (TH1D *)hist_default->Clone();
  ratio_point->Divide(hist_comp);
  ratio_point->SetName(name_suffix+"_central");

  TH1D *tmp_ratio_point = (TH1D *)hist_default->Clone();
  tmp_ratio_point->Divide(hist_comp);
  TGraphAsymmErrors *gr_ratio_point = new TGraphAsymmErrors(tmp_ratio_point);
  gr_ratio_point->SetName("gr_"+name_suffix+"_central");
  gr_ratio_point->SetLineWidth(2.0);
  gr_ratio_point->SetMarkerSize(0.);
  gr_ratio_point->SetLineColor(kBlack);


  TH1D *ratio_staterr = (TH1D *)ratio_point->Clone();
  ratio_staterr->SetName(name_suffix+"_staterr");

  ratio_staterr->SetFillColor(kCyan);
  ratio_staterr->SetMarkerSize(0);
  ratio_staterr->SetMarkerStyle(0);
  ratio_staterr->SetMaximum(2.);
  ratio_staterr->SetMinimum(0.);
  ratio_staterr->GetYaxis()->SetTitle("Ratio [1]/[2]");
  ratio_staterr->SetLineColor(kWhite);
  ratio_staterr->Draw("E2same");
  hist_axis(hist_empty, ratio_staterr);

  ratio_point->Draw("p9histsame");
  //  gr_ratio_point->Draw("p0same");

  TLegend *lg_ratio = new TLegend(0.7, 0.8, 0.8, 0.9);
  lg_ratio->SetNColumns(2);
  lg_ratio->AddEntry(ratio_staterr, "Stat.", "f");
  lg_ratio->Draw();                                                                                                                                             

  ratio_staterr->Draw("axissame");

  //==== y=1 line                                                                                                                                                 
  g1->Draw("same");

  //==== write lumi on the top                                                                                                                                    
  c1->cd();



  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);

}


void  HNL_Efficiency_Plotter::DrawEfficiency( vector<TH1D*> hists, vector<TString> legNames, TString HistName, TString dirName){


  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;


  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ dirName;
  mkdir(thiscut_plotpath);


  TH1D* hist_default = hists[0];

  TLegend *lg;
  if(legNames.size() < 3 ) lg = new TLegend(0.55, 0.85, 0.93, 0.93);
  else lg = new TLegend(0.55, 0.80, 0.93, 0.93);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);


  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);

  TH1D *hist_empty= (TH1D*)hist_default->Clone();

  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();

  hist_empty->GetXaxis()->SetTitle("Lepton p_{T} (GeV)");
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.4;
  
  hist_axis(hist_empty);
  hist_empty->GetYaxis()->SetRangeUser(0, 1.4);
  hist_empty->GetXaxis()->SetRangeUser(15, 500);

  hist_empty->Draw("histsame");
  
  for(int i=0 ; i < hists.size(); i++){
    hists[i]->SetLineColor(HNL_Efficiency_Plotter::GetColor(i));
    hists[i]->SetLineWidth(3.);
    hists[i]->Draw("histsame");
    lg->AddEntry(hists[i], legNames[i],"l");
  }
  lg->Draw();

  
  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");


  TLatex latex_result;
  latex_result.SetNDC();
  latex_result.SetTextSize(0.02);
  //  latex_result.DrawLatex(0.2, 0.9,Label);

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, dirName,AnalyserName,MacroName,Era);

}


void HNL_Efficiency_Plotter::draw_SignalEff_canvas(TGraphAsymmErrors *gSR1,TGraphAsymmErrors *gSR2,TGraphAsymmErrors *gSR3,TGraphAsymmErrors *gSR4 , TString HistName, TString Era, TString Tag){
  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;

  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ HistName;
  mkdir(thiscut_plotpath);

  hist_axis(gSR1);

  if(!Tag.Contains("SR")){
    
    gSR1->SetLineWidth(2);
    gSR1->SetMarkerSize(1.);
    gSR1->SetLineColor(kBlack);
    
    gSR2->SetLineWidth(2);
    gSR2->SetLineStyle(4);
    gSR2->SetMarkerSize(1.);
    gSR2->SetLineColor(kBlack);
    
    gSR3->SetLineWidth(2);
    gSR3->SetMarkerSize(1.);
    gSR3->SetLineColor(kBlue);
    
    gSR4->SetLineWidth(2);
    gSR4->SetLineStyle(4);
    gSR4->SetMarkerSize(1.);
    gSR4->SetLineColor(kBlue);
  }
  else{

    
    gSR1->SetLineWidth(2);
    gSR1->SetMarkerSize(1.);
    gSR1->SetLineColor(kBlack);

    gSR2->SetLineWidth(2);
    gSR2->SetMarkerSize(1.);
    gSR2->SetLineColor(kBlue);

    gSR3->SetLineWidth(2);
    gSR3->SetMarkerSize(1.);
    gSR3->SetLineStyle(4);
    gSR3->SetLineColor(kBlack);

    gSR4->SetLineWidth(2);
    gSR4->SetLineStyle(4);
    gSR4->SetMarkerSize(1.);
    gSR4->SetLineColor(kBlue);
  }

  TCanvas* c1 = new TCanvas(HistName, "", 1200, 800);
  c1->Draw();
  c1->cd();
  canvas_margin(c1);
  c1->SetRightMargin( 0.11 );                            
  gSR1->GetHistogram()->GetYaxis()->SetRangeUser(0,120);
  //  gSR1->GetHistogram()->GetXaxis()->SetRangeUser(85,20000);
  gSR1->GetHistogram()->GetXaxis()->SetTitle("m_{N} Gev");
  gSR1->GetHistogram()->GetYaxis()->SetTitle("#epsilon_{Signal}");
  
  TH1D *hist_empty = new TH1D("","",20000,0,20000);
  hist_empty->SetName("DUMMY_FOR_AXIS");
  hist_axis(hist_empty);
  hist_empty->GetYaxis()->SetRangeUser(0,120);
  hist_empty->GetXaxis()->SetRangeUser(100,20000);
  if(Tag.Contains("SR1")){
    hist_empty->GetYaxis()->SetRangeUser(0,80);
    hist_empty->GetXaxis()->SetRangeUser(100,2000);
  }
  if(Tag.Contains("SR2")){
    hist_empty->GetYaxis()->SetRangeUser(0,60);
    hist_empty->GetXaxis()->SetRangeUser(500,20000);
  }
  hist_empty->GetXaxis()->SetTitle("m_{N} Gev");
  hist_empty->GetYaxis()->SetTitle("#epsilon_{Signal} [%]");
  hist_empty->Draw();
  gSR1->Draw("same");
  gSR2->Draw("same");
  gSR3->Draw("same");
  gSR4->Draw("same");
  
  cout << "gSR1 = " << gSR1->Integral() << " gSR2 = " << gSR2->Integral() << endl;

  TLegend *lg = new TLegend(0.55, 0.8, 0.93, 0.93);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);
  if(!Tag.Contains("SR")){
    lg->AddEntry(gSR1,"VBF+DY","l");
    lg->AddEntry(gSR2,"VBF+DY [Prev. ID]","l");
    lg->AddEntry(gSR3,"SSWW","l");
    lg->AddEntry(gSR4,"SSWW   [Prev. ID]","l");
  }
  else{
    lg->AddEntry(gSR1,"VBF+DY [ee]","l");
    lg->AddEntry(gSR3,"VBF+DY [#mu#mu]","l");
    lg->AddEntry(gSR2,"SSWW [ee]","l");
    lg->AddEntry(gSR4,"SSWW [#mu#mu]","l");

  }
  lg->Draw();

  TLatex channelname;
  channelname.SetNDC();
  channelname.SetTextSize(0.03);
  if(Tag.Contains("SR1"))channelname.DrawLatex(0.2, 0.88,"SR1");
  else   if(Tag.Contains("SR2"))channelname.DrawLatex(0.2, 0.88,"SR2");
  else   if(Tag.Contains("SR3"))channelname.DrawLatex(0.2, 0.88,"SR3");
  else if(Tag.Contains("SR1"))channelname.DrawLatex(0.2, 0.88,"Preselection");

  mkdir(thiscut_plotpath);

  TString HNAME = "SignalEff_"+Tag ;
  c1->SetLogx();

  c1->SaveAs(thiscut_plotpath+"/"+HNAME+".pdf");


}



void HNL_Efficiency_Plotter::draw_SignalEff_canvas(TGraphAsymmErrors *gSR1,TGraphAsymmErrors *gSR2 , TString HistName, TString Era, TString Tag){
  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;

  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ HistName;
  mkdir(thiscut_plotpath);

  hist_axis(gSR1);

  gSR1->SetLineWidth(2);
  gSR1->SetMarkerSize(1.);
  gSR1->SetLineColor(kBlack);
  
  gSR2->SetLineWidth(2);
  gSR2->SetMarkerSize(1.);
  gSR2->SetLineColor(kBlue);
  
  TCanvas* c1 = new TCanvas(HistName, "", 1200, 800);
  c1->Draw();
  c1->cd();
  canvas_margin(c1);
  c1->SetRightMargin( 0.11 );
  gSR1->GetHistogram()->GetYaxis()->SetRangeUser(0,20);
  //  gSR1->GetHistogram()->GetXaxis()->SetRangeUser(85,20000);                                                                                                        
  
  gSR1->GetHistogram()->GetXaxis()->SetTitle("m_{N} Gev");
  gSR1->GetHistogram()->GetYaxis()->SetTitle("#epsilon_{Signal}");

  TH1D *hist_empty = new TH1D("","",20000,0,20000);
  hist_empty->SetName("DUMMY_FOR_AXIS");
  hist_axis(hist_empty);
  hist_empty->GetYaxis()->SetRangeUser(0,20);
  hist_empty->GetXaxis()->SetRangeUser(10,100);
  
  hist_empty->GetXaxis()->SetTitle("m_{N} Gev");
  hist_empty->GetYaxis()->SetTitle("#epsilon_{Signal} [%]");
  hist_empty->Draw();
  
  gSR1->Draw("same");
  gSR2->Draw("same");

  cout << "gSR1 = " << gSR1->Integral() << " gSR2 = " << gSR2->Integral() << endl;
  
  TLegend *lg = new TLegend(0.55, 0.8, 0.93, 0.93);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);
  lg->AddEntry(gSR1,"Top ID [ee]","l");
  lg->AddEntry(gSR2,"POG ID [ee]","l");
  
  lg->Draw();
  
  TLatex channelname;
  channelname.SetNDC();
  channelname.SetTextSize(0.03);
  channelname.DrawLatex(0.2, 0.88,"Preselection");
  
  mkdir(thiscut_plotpath);
  
  TString HNAME = "SignalEff_"+Tag ;
  //  c1->SetLogx();

  c1->SaveAs(thiscut_plotpath+"/"+HNAME+".pdf");
  

}


void HNL_Efficiency_Plotter::draw_IDSF_Muon_canvas(TH1D *hist_default_MuonUnc, TString HistName, TString Era, TString Tag,double tmp_ymax){

  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;

  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ HistName;
  mkdir(thiscut_plotpath);

  /// Fix axis                                                                                                                                                                                                    
  hist_axis(hist_default_MuonUnc);
  TCanvas* c1 = new TCanvas(HistName, "", 1200, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);
  c1->SetRightMargin( 0.11 );                                                                                                                                                                                    \

  TH1D *hist_empty = (TH1D*)hist_default_MuonUnc->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = 7;
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.2;

  hist_empty->GetYaxis()->SetTitleOffset(1.20);

  hist_empty->SetLineWidth(3.);
  hist_empty->SetLineColor(kRed);
  hist_empty->GetYaxis()->SetTitle("Stat. + Syst. Uncertainty (%)");

  hist_empty->GetYaxis()->SetRangeUser(0,tmp_ymax);
  hist_empty->Draw("axis");

  TGraphAsymmErrors *gr1 = new TGraphAsymmErrors(hist_empty);
  gr1->SetLineWidth(2.0);
  gr1->SetMarkerSize(1.5);
  //  gr1->SetMarkerStyle(4.);
  gr1->SetLineColor(kRed);
  gr1->SetMarkerColor(kRed);
  gr1->Draw("lpsame0");



  TLegend *lg= new TLegend(0.75, 0.60, 0.85, 0.750);
  //  lg->SetFillStyle(0);                                                                                                                                                                                                                                                                                                 
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);

  double nBins = 32.;
  double nPtBins = 8.;
  if(Tag == "ID"){
    nBins = 36.;
    nPtBins = 9.;
  }
  double nTLineLower = double(tmp_ymax*0.87);
  
  TLine* l = new TLine(0,nTLineLower,nBins,nTLineLower);
  l->Draw();

  TLine* lv1 = new TLine(nPtBins,0,nPtBins,tmp_ymax);
  lv1->SetLineStyle(4);
  lv1->Draw();

  TLine* lv2 = new TLine(2*nPtBins,0,2*nPtBins,tmp_ymax);
  lv2->SetLineStyle(4);
  lv2->Draw();
  TLine* lv3 = new TLine(3*nPtBins,0,3*nPtBins,tmp_ymax);
  lv3->SetLineStyle(4);
  lv3->Draw();


  lg->Draw();

  TLatex channelname;
  channelname.SetNDC();
  channelname.SetTextSize(0.03);
  channelname.DrawLatex(0.22, 0.88,"0.0-0.9");
  channelname.DrawLatex(0.4, 0.88,"0.9-1.2");
  channelname.DrawLatex(0.58, 0.88,"1.2-2.1");
  channelname.DrawLatex(0.76, 0.88,"2.1-2.4");

  channelname.DrawLatex(0.92, 0.88,"#eta");
  channelname.DrawLatex(0.92, 0.1,"p_{T} (GeV)");
  channelname.SetTextSize(0.03);
  channelname.DrawLatex(0.75, 0.8,Era);
  
  mkdir(thiscut_plotpath);

  TString HNAME = "Muon_"+Era ;

  c1->SaveAs(thiscut_plotpath+"/"+HNAME+"_"+Tag+"_Sys.pdf");



}

void HNL_Efficiency_Plotter::draw_IDSF_Syst_canvas(TH1D *hist_default_EtaPlus, TH1D *hist_default_EtaMinus, TString HistName, TString Era, double tmp_ymax){

  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;


  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ HistName;
  mkdir(thiscut_plotpath);
  
  /// Fix axis 
  hist_axis(hist_default_EtaPlus);
  TCanvas* c1 = new TCanvas(HistName, "", 1200, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);
  c1->SetRightMargin( 0.11 );                                                                                                                                                                                                                                                

  TH1D *hist_empty = (TH1D*)hist_default_EtaPlus->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = 7;
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.2;

  hist_empty->GetYaxis()->SetTitleOffset(1.20);                                                                                                                                                                                                                                     
  
  hist_empty->SetLineWidth(3.);
  hist_empty->SetLineColor(kRed);
  hist_empty->GetYaxis()->SetTitle("Stat. + Syst. Uncertainty (%)");
  
  hist_empty->GetYaxis()->SetRangeUser(0,tmp_ymax);
  hist_empty->Draw("axis");

  TGraphAsymmErrors *gr1 = new TGraphAsymmErrors(hist_empty);
  gr1->SetLineWidth(2.0);
  gr1->SetMarkerSize(1.5);
  gr1->SetMarkerStyle(4.);
  gr1->SetLineColor(kRed);
  gr1->Draw("psame0");
  
  TGraphAsymmErrors *gr2 = new TGraphAsymmErrors(hist_default_EtaMinus);
  gr2->SetLineWidth(2.0);
  gr2->SetMarkerSize(1.5);
  gr2->SetMarkerStyle(22);
  gr2->SetLineColor(kBlue);
  gr2->SetLineStyle(4);
  gr2->Draw("p0same");

  TLegend *lg= new TLegend(0.75, 0.60, 0.85, 0.750);
  //  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);
  lg->AddEntry(gr1,"#eta plus","l");
  lg->AddEntry(gr2,"#eta minus","l");
    
  TLine* l = new TLine(0,tmp_ymax-4,36,tmp_ymax-4);
  l->Draw();
  
  TLine* lv1 = new TLine(9,0,9,tmp_ymax);
  lv1->SetLineStyle(4);
  lv1->Draw();
  
  TLine* lv2 = new TLine(18,0,18,tmp_ymax);
  lv2->SetLineStyle(4);
  lv2->Draw();
  TLine* lv3 = new TLine(27,0,27,tmp_ymax);
  lv3->SetLineStyle(4);
  lv3->Draw();


  lg->Draw();

  TLatex channelname;
  channelname.SetNDC();
  channelname.SetTextSize(0.03);
  if(HistName.Contains("Muon")){
    channelname.DrawLatex(0.22, 0.88,"0.0-0.8");  
    channelname.DrawLatex(0.4, 0.88,"0.8-1.2");  
    channelname.DrawLatex(0.58, 0.88,"1.2-2.1");  
    channelname.DrawLatex(0.76, 0.88,"2.1-2.4");  
  }
  else{
    channelname.DrawLatex(0.22, 0.88,"0.0-0.8");
    channelname.DrawLatex(0.4, 0.88,"0.8-1.479");
    channelname.DrawLatex(0.58, 0.88,"1.566-2.");
    channelname.DrawLatex(0.76, 0.88,"2.-2.5");

  }
  channelname.DrawLatex(0.92, 0.88,"#eta");
  channelname.DrawLatex(0.92, 0.1,"p_{T} (GeV)");
  channelname.SetTextSize(0.03);
  if(HistName.Contains("Muon"))channelname.DrawLatex(0.7, 0.8,Era);
  else channelname.DrawLatex(0.75, 0.8,Era);

  mkdir(thiscut_plotpath);
  
  TString HNAME = "Electron_"+Era ;
  if(HistName.Contains("Muon"))  HNAME = "Muon_"+Era ;
  c1->SaveAs(thiscut_plotpath+"/"+HNAME+"_IDSys.pdf");



}



void HNL_Efficiency_Plotter::draw_IDEff_canvas(TH1D *hist_default,  TH1D *hist_comp , TString HistName, TString hname_1, TString hname_2,TString Era, double tmp_ymax){

  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;


  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;


  thiscut_plotpath = plotpath+"/"+ HistName;
  mkdir(thiscut_plotpath);

  /// Fix axis                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
  hist_axis(hist_default);
  TCanvas* c1 = new TCanvas(HistName, "", 1200, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);
  c1->SetRightMargin( 0.11 );

  TH1D *hist_empty = (TH1D*)hist_default->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");

  double dx = 7;
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.2;

  hist_empty->GetYaxis()->SetTitleOffset(1.20);

  hist_empty->SetLineWidth(3.);
  hist_empty->SetLineColor(kRed);
  hist_empty->GetYaxis()->SetTitle("ratio eff [official/Private]");

  hist_empty->GetYaxis()->SetRangeUser(0.4,tmp_ymax);
  hist_empty->Draw("axis");

  //  cout << "hist_empty error = " << hist_empty->GetBinError(10) << endl;

  TGraphAsymmErrors *gr1 = new TGraphAsymmErrors(hist_empty);
  gr1->SetLineWidth(2.0);
  gr1->SetMarkerSize(1.5);
  gr1->SetMarkerStyle(4.);
  gr1->SetLineColor(kRed);
  gr1->Draw("p0same");

  TGraphAsymmErrors *gr2 = new TGraphAsymmErrors(hist_comp);
  gr2->SetLineWidth(2.0);
  gr2->SetMarkerSize(1.5);
  gr2->SetMarkerStyle(22);
  gr2->SetLineColor(kBlue);
  gr2->SetLineStyle(4);
  gr2->Draw("pEsame");

  TLegend *lg= new TLegend(0.6, 0.60, 0.75, 0.80);
  //  lg->SetFillStyle(0);                                                                                                                                                                                                                                                                                                                                                                                                                                                               
  lg->SetBorderSize(0);
  lg->SetTextSize(0.03);
  lg->AddEntry(gr1,hname_1,"l");
  lg->AddEntry(gr2,hname_2,"l");

  TLine* l = new TLine(0,tmp_ymax-4,60,tmp_ymax-4);
  l->Draw();

  for(int x =1; x < 8 ; x++){
    TLine* lv1 = new TLine(x*6,0,x*6,tmp_ymax);
    lv1->SetLineStyle(4);
    lv1->Draw();
  }

  TLine* l1 = new TLine(0,1 ,48,1);
  l1->Draw();


  lg->Draw();

  TLatex channelname;
  channelname.SetNDC();
  channelname.SetTextSize(.02);
  channelname.DrawLatex(0.18, 0.88,"#eta bin 1");
  channelname.DrawLatex(0.27, 0.88,"#eta bin 2");
  channelname.DrawLatex(0.36, 0.88,"#eta bin 3");
  channelname.DrawLatex(0.45, 0.88,"#eta bin 4");
  channelname.DrawLatex(0.54, 0.88,"#eta bin 5");
  channelname.DrawLatex(0.63, 0.88,"#eta bin 6");
  channelname.DrawLatex(0.72, 0.88,"#eta bin 7");
  channelname.DrawLatex(0.85, 0.88,"#eta bin 8");

  channelname.DrawLatex(0.92, 0.1,"p_{T} (GeV)");
  channelname.SetTextSize(0.03);
  if(HistName.Contains("Muon"))channelname.DrawLatex(0.7, 0.8,Era);
  else channelname.DrawLatex(0.75, 0.8,Era);

  mkdir(thiscut_plotpath);

  TString HNAME = "Electron_"+Era ;
  if(HistName.Contains("Muon"))  HNAME = "Muon_"+Era ;
  c1->SaveAs(thiscut_plotpath+"/"+HNAME+"_IDSys.pdf");



}


void HNL_Efficiency_Plotter::draw_hist_canvas(TH1D *hist_default,  TString HistName){

  
  cout    << "################### draw_hist_canvas [" << HistName << "]  ###################" << endl;


  cout
    << endl
    << "################### Writing in Directory " << thiscut_plotpath << " ###################" << endl
    << endl;

  
  thiscut_plotpath = plotpath+"/"+ HistName;
  mkdir(thiscut_plotpath);

  

  hist_axis(hist_default);

  TLegend *lg= new TLegend(0.2, 0.80, 0.3, 0.90);
  lg->SetFillStyle(0);                                                                                                                            
  lg->SetBorderSize(0);                                                                                                                    
  
  lg->SetTextSize(0.03);


  TCanvas* c1 = new TCanvas(HistName, "", 800, 800);
  c1->Draw();
  c1->cd();

  canvas_margin(c1);
  
  TH1D *hist_empty = (TH1D*)hist_default->Clone();
  hist_empty->SetName("DUMMY_FOR_AXIS");
  
  double dx = (hist_empty->GetXaxis()->GetXmax() - hist_empty->GetXaxis()->GetXmin())/hist_empty->GetXaxis()->GetNbins();
  
  hist_empty->SetLineWidth(0);
  hist_empty->SetLineColor(0);
  hist_empty->SetMarkerSize(0);
  hist_empty->SetMarkerColor(0);
  double Ymin = default_y_min;
  double YmaxScale = 1.2;

  
  hist_empty->Draw("histsame");
  //hist_default->Draw("hist");
						
  
  TGraphAsymmErrors *gr_ratio_point = new TGraphAsymmErrors(hist_default);
  gr_ratio_point->SetLineWidth(2.0);
  gr_ratio_point->SetMarkerSize(0.);
  gr_ratio_point->SetLineColor(kRed);
  gr_ratio_point->Draw("p0same");
  
  TLatex latex_CMSPriliminary, latex_Lumi;
  latex_CMSPriliminary.SetNDC();
  latex_Lumi.SetNDC();
  latex_CMSPriliminary.SetTextSize(0.035);
  latex_CMSPriliminary.DrawLatex(0.15, 0.96, "#font[62]{CMS} #font[42]{#it{#scale[0.8]{Preliminary}}}");
  latex_Lumi.SetTextSize(0.035);
  if(Era=="2016preVFP")latex_Lumi.DrawLatex(0.7, 0.96, "19.5 fb^{-1} (13 TeV)");
  if(Era=="2016postVFP")latex_Lumi.DrawLatex(0.7, 0.96, "16.8 fb^{-1} (13 TeV)");
  if(Era=="2016") latex_Lumi.DrawLatex(0.7, 0.96, "36.3 fb^{-1} (13 TeV)");
  if(Era=="2017") latex_Lumi.DrawLatex(0.7, 0.96, "41.5 fb^{-1} (13 TeV)");
  if(Era=="2018") latex_Lumi.DrawLatex(0.7, 0.96, "59.9 fb^{-1} (13 TeV)");
  if(Era=="Run2") latex_Lumi.DrawLatex(0.7, 0.96, "137.9 fb^{-1} (13 TeV)");


  lg->SetNColumns(2);
  lg->AddEntry(gr_ratio_point, HistName, "pl");
  lg->Draw();
  mkdir(thiscut_plotpath);

  c1->SaveAs(thiscut_plotpath+"/"+HistName+".png");
  c1->SaveAs(thiscut_plotpath+"/"+HistName+".pdf");

  SaveAndCopyLXPLUS(c1,thiscut_plotpath+"/"+HistName, HistName,AnalyserName,MacroName,Era);


  
}
