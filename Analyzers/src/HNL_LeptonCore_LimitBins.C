#include "HNL_LeptonCore.h"

// helper: return mass-specific bins if present and non-empty, else era default                                                              
const vector<double>& HNL_LeptonCore::choose_bins(const map<TString, vector<double>>& per_mass, TString mass)
{
  auto it = per_mass.find(mass);
  if (it != per_mass.end() && !it->second.empty()) return it->second;

  auto it_fallback = per_mass.find("900");
  if (it_fallback != per_mass.end() && !it_fallback->second.empty()) return it_fallback->second;
  
  cout << "choose_bins " << mass << " missing" << endl;
  exit(EXIT_FAILURE);

  
}

void HNL_LeptonCore::DefineLimitBins(){
  
  //// Define SR 1 binning to allow nevents >= 1 per bin per era

  vector<double> sr1bins_ll = {0.,  500, 750,  2000.};

  map<TString, vector<double>> sr1bins_mm_byMass;
  map<TString, vector<double>> sr1bins_ee_byMass;
  map<TString, vector<double>> sr1bins_em_byMass;

  vector<double> sr1bins_mm, sr1bins_ee, sr1bins_em;

  
  if(DataEra == "2016preVFP") {
    sr1bins_mm = { 0., 125.0, 375.0, 435.0, 495.0, 575.0, 755.0  , 2000.};
    sr1bins_ee = { 0., 440.0, 510.0,  605.0, 670.0, 755.0, 985.0 , 2000.};
    sr1bins_em = { 0., 215.0, 495.0, 600.0, 715.0, 860.0, 1030.0, 2000};

    /// Two stes of binning. Alt uses a different more complete scan that checks ALL bin options

    sr1bins_mm_byMass["400"] = {0.0, 270.0, 340.0, 385.0, 465.0, 555.0, 730.0, 5000.0};
    sr1bins_mm_byMass["450"] = {0.0, 330.0, 385.0, 435.0, 515.0, 580.0, 740.0, 5000.0};
    sr1bins_mm_byMass["500"] = {0.0, 330.0, 385.0, 430.0, 485.0, 550.0, 730.0, 5000.0};
    sr1bins_mm_byMass["600"] = {0.0, 115.0, 355.0, 430.0, 485.0, 565.0, 690.0, 5000.0};
    sr1bins_mm_byMass["700"] = {0.0, 120.0, 320.0, 360.0, 445.0, 530.0, 620.0, 5000.0};
    sr1bins_mm_byMass["800"] = {0.0, 115.0, 160.0, 265.0, 435.0, 570.0, 730.0, 5000.0};
    sr1bins_mm_byMass["900"] = {0.0, 115.0, 150.0, 310.0, 445.0, 570.0, 740.0, 5000.0};

    sr1bins_ee_byMass["400"] = {0.0, 365.0, 445.0, 520.0, 590.0, 670.0, 750.0, 5000.0};
    sr1bins_ee_byMass["450"] = {0.0, 365.0, 435.0, 455.0, 490.0, 590.0, 750.0, 5000.0};
    sr1bins_ee_byMass["500"] = {0.0, 365.0, 455.0, 500.0, 565.0, 670.0, 750.0, 5000.0};
    sr1bins_ee_byMass["600"] = {0.0, 500.0, 550.0, 595.0, 655.0, 750.0, 930.0, 5000.0};
    sr1bins_ee_byMass["700"] = {0.0, 475.0, 550.0, 595.0, 625.0, 685.0, 780.0, 5000.0};
    sr1bins_ee_byMass["800"] = {0.0, 360.0, 520.0, 590.0, 635.0, 750.0, 980.0, 5000.0};
    sr1bins_ee_byMass["900"] = {0.0, 345.0, 475.0, 560.0, 635.0, 750.0, 980.0, 5000.0};

    sr1bins_em_byMass["400"] = {0.0, 335.0, 405.0, 440.0, 550.0, 595.0, 865.0, 5000.0};
    sr1bins_em_byMass["450"] = {0.0, 410.0, 450.0, 490.0, 550.0, 595.0, 865.0, 5000.0};
    sr1bins_em_byMass["500"] = {0.0, 410.0, 450.0, 480.0, 550.0, 595.0, 770.0, 5000.0};
    sr1bins_em_byMass["600"] = {0.0, 410.0, 510.0, 580.0, 690.0, 765.0, 950.0, 5000.0};
    sr1bins_em_byMass["700"] = {0.0, 410.0, 510.0, 590.0, 660.0, 805.0, 950.0, 5000.0};
    sr1bins_em_byMass["800"] = {0.0, 115.0, 410.0, 510.0, 620.0, 730.0, 885.0, 5000.0};
    sr1bins_em_byMass["900"] = {0.0, 415.0, 465.0, 585.0, 665.0, 830.0, 1010.0, 5000.0};
   
  }


  if(DataEra == "2016postVFP") {
    sr1bins_mm = { 0., 90.0, 190.0, 420.0, 505.0, 565.0, 765.0, 2000};
    sr1bins_ee = { 0., 470.0, 535.0, 635.0, 700.0, 870.0, 1135.0, 2000}; 
    sr1bins_em = { 0., 470.0, 510.0, 540.0, 660.0, 770.0, 1015.0, 2000};
    
   
    sr1bins_mm_byMass["400"] = {0.0, 350.0, 395.0, 445.0, 505.0, 565.0, 765.0, 5000.0};
    sr1bins_mm_byMass["450"] = {0.0, 330.0, 375.0, 425.0, 495.0, 550.0, 685.0, 5000.0};
    sr1bins_mm_byMass["500"] = {0.0, 215.0, 345.0, 400.0, 460.0, 545.0, 685.0, 5000.0};
    sr1bins_mm_byMass["600"] = {0.0, 205.0, 355.0, 425.0, 500.0, 565.0, 765.0, 5000.0};
    sr1bins_mm_byMass["700"] = {0.0, 125.0, 350.0, 395.0, 460.0, 540.0, 650.0, 5000.0};
    sr1bins_mm_byMass["800"] = {0.0, 135.0, 195.0, 410.0, 460.0, 565.0, 730.0, 5000.0};
    sr1bins_mm_byMass["900"] = {0.0, 130.0, 345.0, 400.0, 460.0, 565.0, 735.0, 5000.0};

    sr1bins_ee_byMass["400"] = {0.0, 340.0, 385.0, 440.0, 555.0, 710.0, 1010.0, 5000.0};
    sr1bins_ee_byMass["450"] = {0.0, 330.0, 420.0, 495.0, 565.0, 825.0, 1095.0, 5000.0};
    sr1bins_ee_byMass["500"] = {0.0, 330.0, 435.0, 480.0, 555.0, 825.0, 1125.0, 5000.0};
    sr1bins_ee_byMass["600"] = {0.0, 465.0, 530.0, 585.0, 645.0, 705.0, 825.0, 5000.0};
    sr1bins_ee_byMass["700"] = {0.0, 420.0, 515.0, 615.0, 680.0, 765.0, 875.0, 5000.0};
    sr1bins_ee_byMass["800"] = {0.0, 490.0, 565.0, 600.0, 690.0, 780.0, 940.0, 5000.0};
    sr1bins_ee_byMass["900"] = {0.0, 485.0, 565.0, 610.0, 715.0, 825.0, 1030.0, 5000.0};

    sr1bins_em_byMass["400"] = {0.0, 350.0, 385.0, 450.0, 675.0, 805.0, 880.0, 5000.0};
    sr1bins_em_byMass["450"] = {0.0, 370.0, 425.0, 500.0, 675.0, 805.0, 880.0, 5000.0};
    sr1bins_em_byMass["500"] = {0.0, 425.0, 475.0, 510.0, 545.0, 590.0, 675.0, 5000.0};
    sr1bins_em_byMass["600"] = {0.0, 515.0, 550.0, 600.0, 685.0, 805.0, 880.0, 5000.0};
    sr1bins_em_byMass["700"] = {0.0, 385.0, 550.0, 605.0, 670.0, 780.0, 880.0, 5000.0};
    sr1bins_em_byMass["800"] = {0.0, 405.0, 550.0, 605.0, 665.0, 765.0, 870.0, 5000.0};
    sr1bins_em_byMass["900"] = {0.0, 415.0, 525.0, 605.0, 675.0, 830.0, 1005.0, 5000.0};

  }
  
  if(DataEra == "2017") {
    sr1bins_mm  = { 0., 450.0, 510.0, 555.0, 610.0, 790.0, 955.0, 2000};
    sr1bins_ee  = { 0., 545.0, 595.0, 670.0, 770.0, 900.0, 1085.0, 2000};
    sr1bins_em  = { 0., 505.0, 580.0, 735.0, 835.0, 940.0, 1215.0, 2000};

   
    sr1bins_mm_byMass["400"] = {0.0, 335.0, 380.0, 420.0, 470.0, 505.0, 540.0, 5000.0};
    sr1bins_mm_byMass["450"] = {0.0, 380.0, 430.0, 480.0, 505.0, 540.0, 755.0, 5000.0};
    sr1bins_mm_byMass["500"] = {0.0, 430.0, 485.0, 520.0, 565.0, 755.0, 930.0, 5000.0};
    sr1bins_mm_byMass["600"] = {0.0, 430.0, 510.0, 545.0, 595.0, 710.0, 930.0, 5000.0};
    sr1bins_mm_byMass["700"] = {0.0, 420.0, 500.0, 520.0, 575.0, 630.0, 745.0, 5000.0};
    sr1bins_mm_byMass["800"] = {0.0, 430.0, 470.0, 540.0, 610.0, 755.0, 925.0, 5000.0};
    sr1bins_mm_byMass["900"] = {0.0, 385.0, 435.0, 480.0, 555.0, 625.0, 825.0, 5000.0};

    sr1bins_ee_byMass["400"] = {0.0, 330.0, 390.0, 435.0, 480.0, 595.0, 885.0, 5000.0};
    sr1bins_ee_byMass["450"] = {0.0, 395.0, 445.0, 500.0, 615.0, 690.0, 720.0, 5000.0};
    sr1bins_ee_byMass["500"] = {0.0, 445.0, 490.0, 560.0, 690.0, 720.0, 885.0, 5000.0};
    sr1bins_ee_byMass["600"] = {0.0, 545.0, 595.0, 655.0, 700.0, 730.0, 905.0, 5000.0};
    sr1bins_ee_byMass["700"] = {0.0, 615.0, 670.0, 720.0, 790.0, 885.0, 1085.0, 5000.0};
    sr1bins_ee_byMass["800"] = {0.0, 545.0, 620.0, 710.0, 770.0, 870.0, 1085.0, 5000.0};
    sr1bins_ee_byMass["900"] = {0.0, 615.0, 690.0, 725.0, 795.0, 885.0, 1075.0, 5000.0};

    sr1bins_em_byMass["400"] = {0.0, 355.0, 385.0, 410.0, 445.0, 490.0, 530.0, 5000.0};
    sr1bins_em_byMass["450"] = {0.0, 410.0, 450.0, 490.0, 515.0, 535.0, 915.0, 5000.0};
    sr1bins_em_byMass["500"] = {0.0, 410.0, 450.0, 480.0, 525.0, 570.0, 915.0, 5000.0};
    sr1bins_em_byMass["600"] = {0.0, 450.0, 530.0, 585.0, 650.0, 765.0, 915.0, 5000.0};
    sr1bins_em_byMass["700"] = {0.0, 530.0, 640.0, 690.0, 765.0, 835.0, 970.0, 5000.0};
    sr1bins_em_byMass["800"] = {0.0, 530.0, 675.0, 725.0, 800.0, 885.0, 1075.0, 5000.0};
    sr1bins_em_byMass["900"] = {0.0, 440.0, 530.0, 660.0, 735.0, 855.0, 1030.0, 5000.0};
    
  }

  if(DataEra == "2018") {
    sr1bins_mm = { 0., 470.0, 555.0, 645.0, 720.0, 840.0, 1045.0, 2000};  
    sr1bins_ee = { 0., 500.0, 530.0, 575.0, 730.0, 945.0, 1260.0, 2000};  
    sr1bins_em = { 0., 440.0, 635.0, 755.0, 930.0, 1045.0, 1315.0, 2000};  

  

    sr1bins_mm_byMass["400"] = {0.0, 365.0, 395.0, 435.0, 460.0, 480.0, 850.0, 5000.0};
    sr1bins_mm_byMass["450"] = {0.0, 370.0, 420.0, 455.0, 495.0, 520.0, 850.0, 5000.0};
    sr1bins_mm_byMass["500"] = {0.0, 395.0, 455.0, 480.0, 525.0, 590.0, 850.0, 5000.0};
    sr1bins_mm_byMass["600"] = {0.0, 480.0, 540.0, 580.0, 635.0, 680.0, 850.0, 5000.0};
    sr1bins_mm_byMass["700"] = {0.0, 540.0, 600.0, 650.0, 710.0, 795.0, 850.0, 5000.0};
    sr1bins_mm_byMass["800"] = {0.0, 395.0, 480.0, 580.0, 670.0, 755.0, 915.0, 5000.0};
    sr1bins_mm_byMass["900"] = {0.0, 480.0, 540.0, 610.0, 670.0, 755.0, 850.0, 5000.0};

    sr1bins_ee_byMass["400"] = {0.0, 325.0, 390.0, 430.0, 465.0, 490.0, 905.0, 5000.0};
    sr1bins_ee_byMass["450"] = {0.0, 395.0, 450.0, 475.0, 520.0, 545.0, 905.0, 5000.0};
    sr1bins_ee_byMass["500"] = {0.0, 390.0, 460.0, 490.0, 530.0, 560.0, 905.0, 5000.0};
    sr1bins_ee_byMass["600"] = {0.0, 500.0, 570.0, 605.0, 645.0, 680.0, 905.0, 5000.0};
    sr1bins_ee_byMass["700"] = {0.0, 580.0, 630.0, 675.0, 710.0, 785.0, 905.0, 5000.0};
    sr1bins_ee_byMass["800"] = {0.0, 580.0, 710.0, 760.0, 845.0, 1015.0, 1150.0, 5000.0};
    sr1bins_ee_byMass["900"] = {0.0, 545.0, 670.0, 735.0, 800.0, 905.0, 1080.0, 5000.0};

    sr1bins_em_byMass["400"] = {0.0, 335.0, 385.0, 440.0, 535.0, 640.0, 665.0, 5000.0};
    sr1bins_em_byMass["450"] = {0.0, 385.0, 445.0, 485.0, 510.0, 640.0, 665.0, 5000.0};
    sr1bins_em_byMass["500"] = {0.0, 445.0, 470.0, 495.0, 555.0, 640.0, 665.0, 5000.0};
    sr1bins_em_byMass["600"] = {0.0, 495.0, 540.0, 595.0, 640.0, 865.0, 960.0, 5000.0};
    sr1bins_em_byMass["700"] = {0.0, 540.0, 620.0, 665.0, 710.0, 790.0, 1085.0, 5000.0};
    sr1bins_em_byMass["800"] = {0.0, 535.0, 665.0, 705.0, 740.0, 895.0, 1085.0, 5000.0};
    sr1bins_em_byMass["900"] = {0.0, 535.0, 665.0, 730.0, 870.0, 990.0, 1165.0, 5000.0};

  }
  
  
  vector<TString> MuMu_SR1_BinTags,  EE_SR1_BinTags ,EMu_SR1_BinTags, CR1_BinTags;
  for(unsigned int i_d =1 ; i_d <  sr1bins_mm.size(); i_d++)  MuMu_SR1_BinTags.push_back("SR1_MNbin"+to_string(i_d));
  for(unsigned int i_d =1 ; i_d <  sr1bins_ee.size(); i_d++)  EE_SR1_BinTags.push_back("SR1_MNbin"+to_string(i_d));
  for(unsigned int i_d =1 ; i_d <  sr1bins_em.size(); i_d++)  EMu_SR1_BinTags.push_back("SR1_MNbin"+to_string(i_d));
  
  
  //// EE/EMu have most bins
  
  map_bins_labels ["SR1_MuMu"]= MuMu_SR1_BinTags;
  map_bins_labels ["SR1_EE"]  = EE_SR1_BinTags;
  map_bins_labels ["SR1_EMu"] = EMu_SR1_BinTags;


  // masses with string labels
  vector<TString> masses_to_prepare = {"400", "450","500", "600", "700", "800","900"  };
  
  
  for (const auto& m : masses_to_prepare) {
    const TString& mass_str = m;
    
    // labels: use the correct tag vectors per channel
    map_bins_labels["SR1_MuMu_" + mass_str] = MuMu_SR1_BinTags;
    map_bins_labels["SR1_EE_"   + mass_str] = EE_SR1_BinTags;
    map_bins_labels["SR1_EMu_"  + mass_str] = EMu_SR1_BinTags;
    
    // boundaries: choose mass-dependent bins if available, else era defaults
    map_bins_boundaries["SR1_MuMu_" + mass_str] = choose_bins(sr1bins_mm_byMass, mass_str);
    map_bins_boundaries["SR1_EE_"   + mass_str] = choose_bins(sr1bins_ee_byMass, mass_str);
    map_bins_boundaries["SR1_EMu_"  + mass_str] = choose_bins(sr1bins_em_byMass, mass_str);


  }
  
  map_bins_boundaries  ["SR1_MuMu"] = sr1bins_mm;
  map_bins_boundaries  ["SR1_EE"]   = sr1bins_ee;
  map_bins_boundaries  ["SR1_EMu"]  = sr1bins_em;

  
  //// CR
  map_bins_labels ["CR1"]       = CR1_BinTags;


  //// Define SR 2 binning  

  map_bins_labels ["SR2"]       = {"SR2_HTLT_Bin1",       "SR2_HTLT_Bin2",       "SR2_HTLT_Bin3",     "SR2_HTLT_Bin4","SR2_HTLT_Bin5","SR2_HTLT_Bin6"};
  map_bins_labels ["CR2"] = {"CR2_HTLT_Bin1", "CR2_HTLT_Bin2", "CR2_HTLT_Bin3"};

  //// Define SR 3 binning                                                                                                                              

  vector<TString> minBinsSR3={"SR3_bin1","SR3_bin2","SR3_bin3","SR3_bin4","SR3_bin5","SR3_bin6","SR3_bin7","SR3_bin8", "SR3_bin9","SR3_bin10","SR3_bin11","SR3_bin12","SR3_bin13","SR3_bin14","SR3_bin15","SR3_bin16","SR3_bin17","SR3_bin18"};

  std::vector<TString> MuMu_SR3 = minBinsSR3;
  std::vector<TString> EE_SR3   = minBinsSR3;
  std::vector<TString> EMu_SR3  = minBinsSR3;
  
  map_bins_labels ["SR3_MuMu"] = MuMu_SR3;
  map_bins_labels ["SR3_EE"]   = EE_SR3;
  map_bins_labels ["SR3_EMu"]  = EMu_SR3;

  std::vector<TString> CR3 = {"CR3_bin1","CR3_bin2","CR3_bin3","CR3_bin4"};
  map_bins_labels ["CR3"]    = CR3;
   

		 
  return ;
  
}

vector<double> HNL_LeptonCore::GetLimitBinBoundary(const TString& SR, TString mass_ref, const TString& channel){


  if(HasFlag("SR_FlavEraDep")){
    if(DataEra == "2016preVFP") {
      if(channel=="MuMu") return { 0., 125.0, 375.0, 435.0, 495.0, 575.0, 755.0 , 2000.};
      if(channel=="EE")   return { 0., 440.0, 510.0, 605.0, 670.0, 755.0, 985.0 , 2000.};
      if(channel=="EMu")  return { 0., 215.0, 495.0, 600.0, 715.0, 860.0, 1030.0, 2000};
    }

    if(DataEra == "2016postVFP") {
      if(channel=="MuMu") return{ 0., 90.0, 190.0, 420.0, 505.0, 565.0, 765.0, 2000};
      if(channel=="EE") return { 0., 470.0, 535.0, 635.0, 700.0, 870.0, 1135.0, 2000};
      if(channel=="EMu")  return { 0., 470.0, 510.0, 540.0, 660.0, 770.0, 1015.0, 2000};
    }
    
    if(DataEra == "2017") {
      if(channel=="MuMu") return { 0., 450.0, 510.0, 555.0, 610.0, 790.0, 955.0, 2000};
      if(channel=="EE") return  { 0., 545.0, 595.0, 670.0, 770.0, 900.0, 1085.0, 2000};
      if(channel=="EMu")  return  { 0., 505.0, 580.0, 735.0, 835.0, 940.0, 1215.0, 2000};
    }
    if(DataEra == "2018") {
      if(channel=="MuMu") return { 0., 470.0, 555.0, 645.0, 720.0, 840.0, 1045.0, 2000};
      if(channel=="EE")   return { 0., 500.0, 530.0, 575.0, 730.0, 945.0, 1260.0, 2000};
      if(channel=="EMu")  return { 0., 440.0, 635.0, 755.0, 930.0, 1045.0, 1315.0, 2000};
      
    }
  }
  if(HasFlag("SR_FlavDep")){
    if(channel=="MuMu") return { 0., 125.0, 375.0, 435.0, 495.0, 575.0, 755.0 , 2000.};
    if(channel=="EE")   return { 0., 440.0, 510.0, 605.0, 670.0, 755.0, 985.0 , 2000.};
    if(channel=="EMu")  return { 0., 215.0, 495.0, 600.0, 715.0, 860.0, 1030.0, 2000};
  }
  if(HasFlag("SingularBinning")){
    return { 0., 125.0, 375.0, 435.0, 495.0, 575.0, 755.0 , 2000.};
  }
  
  TString key = (channel == "") ? SR : SR+"_"+channel+"_"+mass_ref;
  
  map<TString,vector<double> >::iterator mit = map_bins_boundaries.find(key);
  if(mit == map_bins_boundaries.end()) {
    cout << "GetLimitBinBoundary " << key << endl;
    exit(EXIT_FAILURE);
  } 
  vector<double> vcuts = mit->second;

  return vcuts;
}

vector<TString> HNL_LeptonCore::GetLimitLabels(const TString& SR, const TString& channel){
  
  TString key = (channel == "") ? SR : SR+"_"+channel;

  map<TString,vector<TString> > ::iterator mit = map_bins_labels.find(key);
  if(mit == map_bins_labels.end()) {
    cout << "GetLimitLabels  " << SR+"_"+channel << endl;
    
    exit(EXIT_FAILURE);
  }

  vector<TString> SRlabels = mit->second;

  return SRlabels;
}


TString HNL_LeptonCore::GetSR3StringBin(const TString& RegionTag, const TString& channel, bool LowJet, double met2_st, double LT, double ll_dphi){
  
  
  /// Detailed binning for High Mass SR3
  
  if (RegionTag.Contains("CR3")) {
    if (LT < 150) return RegionTag + "_bin1";
    if (LT < 200) return RegionTag + "_bin2";
    if (LT < 300) return RegionTag + "_bin3";
    return RegionTag + "_bin4";
  }
  
  double met2_st_boundary = 5.;
  if (channel == "EE") {
    if (DataYear == 2016) met2_st_boundary = 4.;
    else met2_st_boundary = 3.;
  }
  
  
  TString binPrefix = RegionTag + "_bin";
  double binLimit1 = 0.0, binLimit2 = 0.0, binLimit3 = 0.0, binLimit4 = 0.0, binLimit5 = 0.0;


  if(HasFlag("SingularBinning")){
     if (LowJet) {
        // LowJet Bins                                                                                                                                                      
        if (met2_st < met2_st_boundary) {
          binLimit1 = 150; binLimit2 = 175; binLimit3 = 200; binLimit4 = 300;
        } else {
          binLimit1 = 175; binLimit2 = 250;
        }
      } else {
        // HighJet Bins                                                                                                                                                     
        if (met2_st < met2_st_boundary) {
          binLimit1 = 160; binLimit2 = 180; binLimit3 = 200; binLimit4 = 225; binLimit5 = 275;
        } else {
          binLimit1 = 175; binLimit2 = 200; binLimit3 = 250;
        }
     }
  }
  else if( HasFlag("SR_FlavDep")) {
    if (channel == "MuMu") {
      if (LowJet) {
        // LowJet Bins                                                                                                                                                                                             
        if (met2_st < met2_st_boundary) {
          binLimit1 = 150; binLimit2 = 175; binLimit3 = 200; binLimit4 = 300;
        } else {
          binLimit1 = 175; binLimit2 = 250;
        }
      } else {
        // HighJet Bins                                                                                                                                                                                            
        if (met2_st < met2_st_boundary) {
          binLimit1 = 160; binLimit2 = 180; binLimit3 = 200; binLimit4 = 225; binLimit5 = 275;
        } else {
          binLimit1 = 175; binLimit2 = 200; binLimit3 = 250;
        }
      }
    }
    else if (channel == "EE") {
      if (LowJet) {
	// LowJet Bins                                                                                                                                                                                           
	if (met2_st < met2_st_boundary) {
	  binLimit1 = 200; binLimit2 = 400; binLimit3 = 600; binLimit4 = 900;
	} else {
	  binLimit1 = 200; binLimit2 = 500;
	}
      } else {
	// HighJet Bins                                                                                                                                                                                          
	if (met2_st < met2_st_boundary) {
	  binLimit1 = 300; binLimit2 = 375; binLimit3 = 450; binLimit4 = 550; binLimit5 = 700;
	} else {
	  binLimit1 = 250; binLimit2 = 350; binLimit3 = 400;
	}
      }
    }
    if (channel == "EMu") {
      if (LowJet) {
	// LowJet Bins                                                                                                                                                                                           
	if (met2_st < met2_st_boundary) {
	  binLimit1 = 175; binLimit2 = 250; binLimit3 = 325; binLimit4 = 425;
	} else {
	  binLimit1 = 250; binLimit2 = 400;
	}
      } else {
	// HighJet Bins                                                                                                                                                                                          
	if (met2_st < met2_st_boundary) {
	  binLimit1 = 250; binLimit2 = 290; binLimit3 = 340; binLimit4 = 400; binLimit5 = 500;
	} else {
	  binLimit1 = 200; binLimit2 = 300; binLimit3 = 400;
	}
      }
    }
  }
      
  else {
    if (channel == "MuMu") {
      
      if(DataEra == "2016preVFP") {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 150; binLimit2 = 175; binLimit3 = 200; binLimit4 = 300;
	  } else {
	    binLimit1 = 175; binLimit2 = 250; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 160; binLimit2 = 180; binLimit3 = 200; binLimit4 = 225; binLimit5 = 275;
	  } else {
	    binLimit1 = 175; binLimit2 = 200; binLimit3 = 250; 
	  }
	}
      }
      else   if(DataEra == "2016postVFP") {
	if (LowJet) {
	  // LowJet Bins                                                                                                 
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 130; binLimit2 = 150; binLimit3 = 200; binLimit4 = 300;
	  } else {
	    binLimit1 = 175; binLimit2 = 250;
	  }
	} else {
	  // HighJet Bins                                                                                                
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 130; binLimit2 = 150; binLimit3 = 175; binLimit4 = 200; binLimit5 = 275;
	  } else {
	    binLimit1 = 150; binLimit2 = 175; binLimit3 = 225;
	  }
	}
      }
      else if (DataYear == 2017) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 150; binLimit2 = 200; binLimit3 = 250; binLimit4 = 300;
	  } else {
	    binLimit1 = 200; binLimit2 = 275; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 175; binLimit2 = 200; binLimit3 = 250; binLimit4 = 300; binLimit5 = 375;
	  } else {
	    binLimit1 = 200; binLimit2 = 275; binLimit3 = 350; 
	  }
	}
      } else if (DataYear == 2018) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 200; binLimit2 = 250; binLimit3 = 300; binLimit4 = 375;
	  } else {
	    binLimit1 = 200; binLimit2 = 400; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 200; binLimit2 = 225; binLimit3 = 275; binLimit4 = 325; binLimit5 = 400;
	  } else {
	    binLimit1 = 200; binLimit2 = 350; binLimit3 = 450; 
	  }
	}
      }
    }
  
    if (channel == "EE") {
      if (DataYear == 2016) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 200; binLimit2 = 400; binLimit3 = 600; binLimit4 = 900;
	  } else {
	    binLimit1 = 200; binLimit2 = 500; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 300; binLimit2 = 375; binLimit3 = 450; binLimit4 = 550; binLimit5 = 700;
	  } else {
	    binLimit1 = 250; binLimit2 = 350; binLimit3 = 400;
	  }
	}
      } else if (DataYear == 2017) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 200; binLimit2 = 400; binLimit3 = 600; binLimit4 = 900;
	  } else {
	    binLimit1 = 180; binLimit2 = 500; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 300; binLimit2 = 375; binLimit3 = 450; binLimit4 = 550; binLimit5 = 700;
	  } else {
	    binLimit1 = 300; binLimit2 = 450; binLimit3 = 550;
	  }
	}
      } else if (DataYear == 2018) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 200; binLimit2 = 400; binLimit3 = 600; binLimit4 = 900; 
	  } else {
	    binLimit1 = 200; binLimit2 = 600; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 300; binLimit2 = 400; binLimit3 = 500; binLimit4 = 600; binLimit5 = 800;
	  } else {
	    binLimit1 = 350; binLimit2 = 500; binLimit3 = 600; 
	  }
	}
      }
    }
    
    if (channel == "EMu") {
      if (DataYear == 2016) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 175; binLimit2 = 250; binLimit3 = 325; binLimit4 = 425;
	  } else {
	    binLimit1 = 250; binLimit2 = 400;
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 250; binLimit2 = 290; binLimit3 = 340; binLimit4 = 400; binLimit5 = 500;
	  } else {
	    binLimit1 = 200; binLimit2 = 300; binLimit3 = 400; 
	  }
	}
      } else if (DataYear == 2017) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 175; binLimit2 = 250; binLimit3 = 350; binLimit4 = 450;
	  } else {
	    binLimit1 = 250; binLimit2 = 450; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 250; binLimit2 = 300; binLimit3 = 350; binLimit4 = 400; binLimit5 = 500;
	  } else {
	    binLimit1 = 250; binLimit2 = 350; binLimit3 = 500;
	  }
	}
      } else if (DataYear == 2018) {
	if (LowJet) {
	  // LowJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 250; binLimit2 = 300; binLimit3 = 400; binLimit4 = 500;
	  } else {
	    binLimit1 = 250; binLimit2 = 550; 
	  }
	} else {
	  // HighJet Bins
	  if (met2_st < met2_st_boundary) {
	    binLimit1 = 300; binLimit2 = 350; binLimit3 = 425; binLimit4 = 500; binLimit5 = 700;
	  } else {
	    binLimit1 = 250; binLimit2 = 400; binLimit3 = 500;
	  }
	}
      }
    }
  }
  
  // Determine the correct bin based on the LT value
  if (LowJet) {
    if (met2_st < met2_st_boundary) {
      if (LT <= binLimit1) return binPrefix + "1";
      if (LT <= binLimit2) return binPrefix + "2";
      if (LT <= binLimit3) return binPrefix + "3";
      if (LT <= binLimit4) return binPrefix + "4";
      return binPrefix + "5";
    } else {
      if (LT <= binLimit1) return binPrefix + "6";
      if (LT <= binLimit2) return binPrefix + "7";
      return binPrefix + "8";
    }
  } 
  else {
    if (met2_st < met2_st_boundary) {
      if (LT < binLimit1) return binPrefix + "9";
      if (LT < binLimit2) return binPrefix + "10";
      if (LT < binLimit3) return binPrefix + "11";
      if (LT < binLimit4) return binPrefix + "12";
      if (LT < binLimit5) return binPrefix + "13";
      return binPrefix + "14";
    } else {
      if (LT < binLimit1) return binPrefix + "15";
      if (LT < binLimit2) return binPrefix + "16";
      if (LT < binLimit3) return binPrefix + "17";
      return binPrefix + "18";
    }
  }
  
  return "";
  
}


vector<TString> HNL_LeptonCore::GetBDTLimitLabels(const TString& key){

  map<TString,vector<TString> > ::iterator mit = map_BDT_bins_labels.find(key);
  if(mit == map_BDT_bins_labels.end()) {
    cout << "GetBDTLimitLabels " << key << endl;
    for(auto i : map_BDT_bins_labels) cout << i.first << endl;
    exit(EXIT_FAILURE);
  }
  vector<TString> SRlabels = mit->second;

  return SRlabels;
}


double HNL_LeptonCore::GetLimitBin(const TString& region, const std::vector<Lepton*>& leps, const std::vector<Jet>& AK4Jets,
				   const std::vector<FatJet>& AK8_JetColl, const Event& ev, double& nbins_reg){

  double Binvalue=0;
  
  if(region=="CR_SR3_WZ") {
    double LT = leps[0]->Pt() + leps[1]->Pt();
    if(LT < 100) Binvalue=0.5;
    else     if(LT < 200) Binvalue=1.5;
    else if(LT < 300) Binvalue=2.5;
    else if(LT < 400) Binvalue=3.5;
    else  Binvalue=4.5;
    nbins_reg=5;
    return Binvalue;

  }
  if(region=="CR_SR1_Inv"){

    Particle l1J =  *leps[0] +  AK8_JetColl[0];
    if(l1J.M() < 300) Binvalue=0.5;
    else if(l1J.M() < 500)  Binvalue=1.5;
    else if(l1J.M() < 750)  Binvalue= 2.5;
    else if(l1J.M() < 1000) Binvalue= 3.5;
    else Binvalue= 4.5;
    //// 3 bins                                                                                                                                                  
    nbins_reg=5;
    return Binvalue;
  }
 
  if(region=="CR_SR2_Inv"){
    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 2) Binvalue = 0.5;
    else   if(SR2BinValue < 5) Binvalue = 1.5;
    else   Binvalue = 2.5;

    nbins_reg=3;
    return Binvalue;
    
  }


  if(region == ("CR_SR1_WZ")){
    
    Particle l1J =  *leps[0] +  AK8_JetColl[0];
    if(l1J.M() < 750) Binvalue= 0.5;
    else Binvalue= 1.5;
    //// 3 bins                                                                                                                                                             
    nbins_reg=2;
    return Binvalue;
  }
  


  if(region.Contains("CR_SR1")){

    Particle l1J =  *leps[0] +  AK8_JetColl[0];
    if(l1J.M() < 200) Binvalue=0.5;
    else if(l1J.M() < 750) Binvalue= 1.5;
    else Binvalue= 2.5;    
    //// 3 bins
    nbins_reg=3;
    return Binvalue; 
  }

  if(region == "CR_SR2_WZ"){
    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 2) Binvalue = 0.5;
    else   if(SR2BinValue < 4) Binvalue = 1.5;
    else   Binvalue = 2.5;

    nbins_reg=3;
    return Binvalue;

  }

  if(region =="CR_SR2_WZB") {
    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 3) Binvalue = 0.5;
    else   Binvalue = 1.5;

    nbins_reg=2;
    return Binvalue;

  }
  if(region.Contains("CR_SR2")){

    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 2) Binvalue = 0.5;
    else   if(SR2BinValue < 5) Binvalue = 1.5;
    else   Binvalue = 2.5;

    nbins_reg=3;
    return Binvalue;

  }
  
  cout << "No Limit Region set " << region << endl;
  exit(EXIT_FAILURE);

}

