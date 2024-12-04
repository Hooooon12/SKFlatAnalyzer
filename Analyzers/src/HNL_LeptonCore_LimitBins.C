#include "HNL_LeptonCore.h"

void HNL_LeptonCore::DefineLimitBins(){
  
  //// Define SR 1 binning to allow nevents >= 1 per bin per era
  
  vector<double> sr1bins_mm;
  vector<double> sr1bins_ee;
  vector<double> sr1bins_em;

  //// Define CR1 binning      
  vector<double> cr1bins = { 0.,400,  2000.};

  if(DataEra == "2016preVFP") {
    sr1bins_mm = { 0., 400,  2000.};
    sr1bins_ee = { 0., 400, 500,  2000.};
    sr1bins_em = { 0., 400, 500,  600, 2000.};
  }
  if(DataEra == "2016postVFP") {
    sr1bins_mm = { 0., 400,  2000.};
    sr1bins_ee  = { 0.,400, 500, 750,  2000.};
    sr1bins_em = { 0., 400, 500, 2000.};
  }
  if(DataEra == "2017") {
    sr1bins_mm  = { 0., 300., 500,  600., 2000.};
    sr1bins_ee  = { 0., 400, 500,  750., 2000.};
    sr1bins_em  = { 0., 300.,400, 500,  600, 800., 2000.};
  }
  if(DataEra == "2018") {
    sr1bins_mm = { 0.,  300.,400., 500, 750., 2000.};
    sr1bins_ee  = { 0., 300.,400., 500, 625,  750., 2000.};
    sr1bins_em = { 0.,  300.,400., 500, 600, 800., 2000.};
  }

  vector<TString> MuMu_SR1_BinTags, EE_SR1_BinTags ,EMu_SR1_BinTags, InvMETCR1_BinTags, BJetCR1_BinTags;
  for(unsigned int i_d =1 ; i_d <  sr1bins_mm.size(); i_d++)  MuMu_SR1_BinTags.push_back("SR1_MNbin"+to_string(i_d));
  for(unsigned int i_d =1 ; i_d <  sr1bins_ee.size(); i_d++)  EE_SR1_BinTags.push_back("SR1_MNbin"+to_string(i_d));
  for(unsigned int i_d =1 ; i_d <  sr1bins_em.size(); i_d++)  EMu_SR1_BinTags.push_back("SR1_MNbin"+to_string(i_d));
  for(unsigned int i_d =1 ; i_d <  cr1bins.size(); i_d++)  InvMETCR1_BinTags.push_back("InvMETCR1_MNbin"+to_string(i_d));
  for(unsigned int i_d =1 ; i_d <  cr1bins.size(); i_d++)  BJetCR1_BinTags.push_back("BJetCR1_MNbin"+to_string(i_d));
  map_bins_labels ["SR1_MuMu"]= MuMu_SR1_BinTags;
  map_bins_labels ["SR1_EE"]  = EE_SR1_BinTags;
  map_bins_labels ["SR1_EMu"] = EMu_SR1_BinTags;

  map_bins_boundaries  ["SR1_MuMu"] = sr1bins_mm;
  map_bins_boundaries  ["SR1_EE"]   = sr1bins_ee;
  map_bins_boundaries  ["SR1_EMu"]  = sr1bins_em;

  //// CR
  map_bins_labels ["InvMETCR1"]       = InvMETCR1_BinTags;
  map_bins_labels ["BJetCR1"]         = BJetCR1_BinTags;
  map_bins_boundaries  ["InvMETCR1"]  = cr1bins;
  map_bins_boundaries  ["BJetCR1"]    = cr1bins;

  //// Define SR 2 binning  
  
  if(DataYear == 2016) {
    map_bins_labels ["SR2"] = {"SR2_HTLTbin1", "SR2_HTLTbin2", "SR2_HTLTbin3", "SR2_HTLTbin4"};
    map_bins_labels ["InvMETCR2"] = {"InvMETCR2_HTLTbin1", "InvMETCR2_HTLTbin2", "InvMETCR2_HTLTbin3"};
    map_bins_labels ["BJetCR2"] = {"BJetCR2_HTLTbin1"};
  }
  else {
    map_bins_labels ["SR2"] = {"SR2_HTLTbin1", "SR2_HTLTbin2", "SR2_HTLTbin3", "SR2_HTLTbin4", "SR2_HTLTbin5", "SR2_HTLTbin6","SR2_HTLTbin7"};
    map_bins_labels ["InvMETCR2"] = {"InvMETCR2_HTLTbin1", "InvMETCR2_HTLTbin2", "InvMETCR2_HTLTbin3"};
    map_bins_labels ["BJetCR2"] = {"BJetCR2_HTLTbin1","BJetCR2_HTLTbin2"};
  }

  //// Define SR 3 binning                                                                                                                              

  vector<TString> minBinsSR3={"SR3_bin1","SR3_bin2","SR3_bin3","SR3_bin4","SR3_bin5","SR3_bin6","SR3_bin7","SR3_bin8", "SR3_bin9","SR3_bin10","SR3_bin11","SR3_bin12","SR3_bin13","SR3_bin14","SR3_bin15","SR3_bin16","SR3_bin17","SR3_bin18","SR3_bin19","SR3_bin20"};

  std::vector<TString> MuMu_SR3 = minBinsSR3;
  std::vector<TString> EE_SR3   = minBinsSR3;
  std::vector<TString> EMu_SR3  = minBinsSR3;
  
  EE_SR3.insert(EE_SR3.end(), {"SR3_bin21","SR3_bin22","SR3_bin23","SR3_bin24","SR3_bin25"}); // Append {bin20-24}
  EMu_SR3.insert(EMu_SR3.end(), {"SR3_bin21","SR3_bin22","SR3_bin23"}); // Append {bin20-21}   

  map_bins_labels ["SR3_MuMu"] = MuMu_SR3;
  map_bins_labels ["SR3_EE"]   = EE_SR3;
  map_bins_labels ["SR3_EMu"]  = EMu_SR3;

  std::vector<TString> InvMETCR3 = {"InvMETCR3_bin1","InvMETCR3_bin2","InvMETCR3_bin3","InvMETCR3_bin4","InvMETCR3_bin5","InvMETCR3_bin6","InvMETCR3_bin7","InvMETCR3_bin8", "InvMETCR3_bin9","InvMETCR3_bin10"};
  map_bins_labels ["InvMETCR3"]    = InvMETCR3;
 

  std::vector<TString> BJetCR3 = {"BJetCR3_bin1","BJetCR3_bin2","BJetCR3_bin3","BJetCR3_bin4","BJetCR3_bin5","BJetCR3_bin6","BJetCR3_bin7","BJetCR3_bin8", "BJetCR3_bin9","BJetCR3_bin10"};
  map_bins_labels ["BJetCR3"]    = BJetCR3;


  

		 
  return ;
  
}

vector<double> HNL_LeptonCore::GetLimitBinBoundary(TString SR, TString channel){

  TString key = (channel == "") ? SR : SR+"_"+channel;
  
  map<TString,vector<double> >::iterator mit = map_bins_boundaries.find(key);
  if(mit == map_bins_boundaries.end()) {
    cout << "GetLimitBinBoundary " << key << endl;
    exit(EXIT_FAILURE);
  } 
  vector<double> vcuts = mit->second;

  return vcuts;
}

vector<TString> HNL_LeptonCore::GetLimitLabels(TString SR, TString channel){
  
  TString key = (channel == "") ? SR : SR+"_"+channel;

  map<TString,vector<TString> > ::iterator mit = map_bins_labels.find(key);
  if(mit == map_bins_labels.end()) {
    cout << "GetLimitLabels  " << SR+"_"+channel << endl;
    
    exit(EXIT_FAILURE);
  }

  vector<TString> SRlabels = mit->second;

  return SRlabels;
}


TString HNL_LeptonCore::GetSR3StringBin(TString RegionTag, TString channel, bool LowJet, double met2_st, double LT, double ll_dphi){
  
  /// Detailed binning for High Mass SR3

  if(RegionTag.Contains("InvMET")){
    if(LowJet){
      if(ll_dphi > 2.5){
	if(LT<= 150)       return RegionTag+"_bin1";
	else if(LT<= 250)  return RegionTag+"_bin2";
	else               return RegionTag+"_bin3";
      }
      else{
	if(LT<= 150)  return RegionTag+"_bin4";
	else          return RegionTag+"_bin5";
      }
    }
    else{
      if(LT < 150)       return RegionTag+"_bin6";
      else if(LT < 200)  return RegionTag+"_bin7";
      else if(LT < 300)  return RegionTag+"_bin8";
      else if(LT < 450)  return RegionTag+"_bin9";
      else               return RegionTag+"_bin10";
    }
    return "";
  }

  if(RegionTag.Contains("BJet")){
    if(LowJet){
      if(ll_dphi > 2.5){
	if(met2_st < 5){
	  if(LT<= 150)       return RegionTag+"_bin1";
	  else               return RegionTag+"_bin2";
	}
	else {
	  if(LT<= 150)       return RegionTag+"_bin3";
          else               return RegionTag+"_bin4";
	}
      }
      else{
        if(LT<= 150)  return RegionTag+"_bin5";
        else          return RegionTag+"_bin6";
      }
    }
    else{
      if(LT < 150)       return RegionTag+"_bin7";
      else if(LT < 200)  return RegionTag+"_bin8";
      else if(LT < 450)  return RegionTag+"_bin9";
      else               return RegionTag+"_bin10";
    }
    return "";
  }

  if(channel == "MuMu"){
    
    if(DataYear == 2016){

      if(LowJet){
	////////// LOWJET BINS
	if(met2_st < 5){
	  if(ll_dphi > 2.5){
	    if(LT<= 150)       return RegionTag+"_bin1";
	    else if(LT<= 200)  return RegionTag+"_bin2";
	    else               return RegionTag+"_bin3";
	  }
	  else{
	    if(LT<= 150)  return RegionTag+"_bin4";
	    else if(LT<= 200)  return RegionTag+"_bin5";
	    else          return RegionTag+"_bin6";
	  }
	}
	else{
	  if( LT<= 150 )  return RegionTag+"_bin7";
	  else if( LT<= 200 )  return RegionTag+"_bin8";
	  else            return RegionTag+"_bin9";
	}
      }
      else{
	//#################### HighJET BINS  

	if(met2_st < 5){
          if(LT < 150)       return RegionTag+"_bin10";
          else if(LT < 175)  return RegionTag+"_bin11";
          else if(LT < 200)  return RegionTag+"_bin12";
          else if(LT < 250)  return RegionTag+"_bin13";
          else if(LT < 350)  return RegionTag+"_bin14";
          else if(LT < 450)  return RegionTag+"_bin15";
          else               return RegionTag+"_bin16";
        }
        else{
          if(LT < 150)  return RegionTag+"_bin17";
          else if(LT < 200)  return RegionTag+"_bin18";
          else if(LT < 300)  return RegionTag+"_bin19";
          else  return RegionTag+"_bin20";
        }/// MET            
      }
    }
    
    else if (DataYear == 2017){
      if(LowJet){

	////////// LOWJET BINS  /////////////////////////////                                                                                                                                                                                  
	if(met2_st < 5){
	  if(ll_dphi > 2.5){
	    if(LT<= 200)       return RegionTag+"_bin1";
	    else if(LT<= 250)  return RegionTag+"_bin2";
	    else               return RegionTag+"_bin3";
	  }
	  else{
	    if(LT<= 150)        return RegionTag+"_bin4";
	    else if(LT<= 200)   return RegionTag+"_bin5";
	    else                return RegionTag+"_bin6";
	  }
	}
	else{
	  if( LT<= 150 )        return RegionTag+"_bin7";
	  else if( LT<= 250 )   return RegionTag+"_bin8";
	  else  return RegionTag+"_bin9";
	}
      }
      else{
	//#################### HighJET BINS                                                                                                                                                                       


	if(met2_st < 5){
          if(LT < 150)       return RegionTag+"_bin10";
          else if(LT < 175)  return RegionTag+"_bin11";
          else if(LT < 200)  return RegionTag+"_bin12";
          else if(LT < 250)  return RegionTag+"_bin13";
          else if(LT < 350)  return RegionTag+"_bin14";
          else if(LT < 450)  return RegionTag+"_bin15";
          else               return RegionTag+"_bin16";
        }
        else{
          if(LT < 150)  return RegionTag+"_bin17";
          else if(LT < 200)  return RegionTag+"_bin18";
          else if(LT < 300)  return RegionTag+"_bin19";
          else  return RegionTag+"_bin20";
        }/// MET            
      }
    }
    else if(DataYear == 2018){
      if(LowJet){
        ////////// LOWJET BINS  /////////////////////////////                                                                                                                                                  
	if(met2_st < 5){
	  if(ll_dphi > 2.5){
	    if(LT<= 200)          return RegionTag+"_bin1";
	    else if(LT<= 250)     return RegionTag+"_bin2";
	    else if(LT<= 300)     return RegionTag+"_bin3";
	    else       return RegionTag+"_bin4";
	  }
	  else{
	    if(LT<= 200)         return RegionTag+"_bin5";
	    else            return RegionTag+"_bin6";
	  }
	}
	else{
	  if( LT<= 200 )   return RegionTag+"_bin7";
	  else if( LT<= 300 )   return RegionTag+"_bin8";
	  else  return RegionTag+"_bin9";
	}
      }
      else{
	//#################### HighJET BINS                                                                                                                                                                      

	if(met2_st < 5){
          if(LT < 150)       return RegionTag+"_bin10";
          else if(LT < 175)  return RegionTag+"_bin11";
          else if(LT < 200)  return RegionTag+"_bin12";
          else if(LT < 250)  return RegionTag+"_bin13";
          else if(LT < 350)  return RegionTag+"_bin14";
          else if(LT < 450)  return RegionTag+"_bin15";
          else               return RegionTag+"_bin16";
        }
        else{
          if(LT < 150)  return RegionTag+"_bin17";
          else if(LT < 200)  return RegionTag+"_bin18";
          else if(LT < 300)  return RegionTag+"_bin19";
          else  return RegionTag+"_bin20";
        }/// MET                                    
	
      }
    }
  }
  if(channel == "EE"){
    
    if(DataYear == 2016) {
      if(LowJet){
        ////////// LOWJET BINS  /////////////////////////////                                                                                                                                                    
	if(met2_st < 5){
	  if(ll_dphi > 2.5){
	    if(LT<= 150)             return RegionTag+"_bin1";
	    else if(LT<= 200)        return RegionTag+"_bin2";
	    else if(LT<= 250)        return RegionTag+"_bin3";
	    else if(LT<= 400)        return RegionTag+"_bin4";
	    else if(LT<= 500)        return RegionTag+"_bin5";
	    else if(LT<= 600)        return RegionTag+"_bin6";
	    else                     return RegionTag+"_bin7";
	  }
	  else{
	    if(LT<= 150)         return RegionTag+"_bin8";
	    else if(LT<= 250)    return RegionTag+"_bin9";
	    else                 return RegionTag+"_bin10";
	  }
	}
	else{
	  if( LT<= 200 )        return RegionTag+"_bin11";
	  else if( LT<= 300 )   return RegionTag+"_bin12";
	  else  return RegionTag+"_bin13";
	}
      }
      else{
	//#################### HighJET BINS                                                                                                                                                                       
	if(met2_st < 5){
	  if(LT < 150)       return RegionTag+"_bin14";
	  else if(LT < 175)  return RegionTag+"_bin15";
	  else if(LT < 200)  return RegionTag+"_bin16";
	  else if(LT < 250)  return RegionTag+"_bin17";
	  else if(LT < 300)  return RegionTag+"_bin18";
	  else if(LT < 350)  return RegionTag+"_bin19";
	  else if(LT < 450)  return RegionTag+"_bin20";
	  else               return RegionTag+"_bin21";
	}
	else{
	  if(LT < 150)  return RegionTag+"_bin22";
	  else if(LT < 200)  return RegionTag+"_bin23";
	  else if(LT < 300)  return RegionTag+"_bin24";
	  else  return RegionTag+"_bin25";
	}/// MET
      }
    }/// 2016
    if(DataYear == 2017){
      if(LowJet){
        ////////// LOWJET BINS  /////////////////////////////                                                                                                                                                                                                                 
        if(met2_st < 5){
          if(ll_dphi > 2.5){
            if(LT<= 150)             return RegionTag+"_bin1";
            else if(LT<= 200)        return RegionTag+"_bin2";
            else if(LT<= 250)        return RegionTag+"_bin3";
            else if(LT<= 400)        return RegionTag+"_bin4";
            else if(LT<= 500)        return RegionTag+"_bin5";
            else if(LT<= 600)        return RegionTag+"_bin6";
            else                     return RegionTag+"_bin7";
          }
          else{
            if(LT<= 150)         return RegionTag+"_bin8";
            else if(LT<= 250)    return RegionTag+"_bin9";
            else                 return RegionTag+"_bin10";
          }
        }
        else{
          if( LT<= 200 )        return RegionTag+"_bin11";
          else if( LT<= 300 )   return RegionTag+"_bin12";
          else  return RegionTag+"_bin13";
        }
      }
      else{
        //#################### HighJET BINS                                                                                                                                                                                                                                   
        if(met2_st < 5){
          if(LT < 150)       return RegionTag+"_bin14";
          else if(LT < 175)  return RegionTag+"_bin15";
          else if(LT < 200)  return RegionTag+"_bin16";
          else if(LT < 250)  return RegionTag+"_bin17";
          else if(LT < 350)  return RegionTag+"_bin18";
          else if(LT < 450)  return RegionTag+"_bin19";
          else if(LT < 550)  return RegionTag+"_bin20";
          else               return RegionTag+"_bin21";
	}
	else{
          if(LT < 150)  return RegionTag+"_bin22";
          else if(LT < 200)  return RegionTag+"_bin23";
          else if(LT < 300)  return RegionTag+"_bin24";
          else  return RegionTag+"_bin25";
	}/// MET                                                                                                                                                                                                                                                              
      } 
      return "";

    } 
    if(DataYear == 2018){
      if(LowJet){
        ////////// LOWJET BINS  /////////////////////////////                                                                                                                                                                                                                 
        if(met2_st < 5){
          if(ll_dphi > 2.5){
            if(LT<= 150)             return RegionTag+"_bin1";
            else if(LT<= 200)        return RegionTag+"_bin2";
            else if(LT<= 250)        return RegionTag+"_bin3";
            else if(LT<= 400)        return RegionTag+"_bin4";
            else if(LT<= 500)        return RegionTag+"_bin5";
            else if(LT<= 600)        return RegionTag+"_bin6";
            else                     return RegionTag+"_bin7";
          }
          else{
            if(LT<= 150)         return RegionTag+"_bin8";
            else if(LT<= 250)    return RegionTag+"_bin9";
            else                 return RegionTag+"_bin10";
          }
        }
        else{
          if( LT<= 200 )        return RegionTag+"_bin11";
          else if( LT<= 300 )   return RegionTag+"_bin12";
          else  return RegionTag+"_bin13";
        }
      }
      else{
        //#################### HighJET BINS                                                                                                                                                                                                                                   
        if(met2_st < 5){
          if(LT < 150)       return RegionTag+"_bin14";
          else if(LT < 175)  return RegionTag+"_bin15";
          else if(LT < 200)  return RegionTag+"_bin16";
          else if(LT < 250)  return RegionTag+"_bin17";
          else if(LT < 350)  return RegionTag+"_bin18";
          else if(LT < 450)  return RegionTag+"_bin19";
          else if(LT < 600)  return RegionTag+"_bin20";
          else               return RegionTag+"_bin21";
	}
	else{
          if(LT < 150)  return RegionTag+"_bin22";
          else if(LT < 300)  return RegionTag+"_bin23";
          else if(LT < 400)  return RegionTag+"_bin24";
          else  return RegionTag+"_bin25";
	}/// MET                                                                                                                                                                                                                                                              
      } 
    }
    return "";
  }
  if(channel == "EMu"){
    if(DataYear == 2016){
      if(LowJet){
	////////// LOWJET BINS  /////////////////////////////       

	if(met2_st < 5){
          if(ll_dphi > 2.5){
            if(LT<= 150)             return RegionTag+"_bin1";
            else if(LT<= 200)        return RegionTag+"_bin2";
            else if(LT<= 250)        return RegionTag+"_bin3";
            else if(LT<= 300)        return RegionTag+"_bin4";
            else if(LT<= 400)        return RegionTag+"_bin5";
            else                     return RegionTag+"_bin6";
          }
          else{
            if(LT<= 150)         return RegionTag+"_bin7";
            else if(LT<= 200)    return RegionTag+"_bin8";
            else if(LT<= 250)    return RegionTag+"_bin9";
            else                 return RegionTag+"_bin10";
          }
        }
        else{
          if( LT<= 150 )        return RegionTag+"_bin11";
          else if( LT<= 250 )   return RegionTag+"_bin12";
          else if( LT<= 350 )   return RegionTag+"_bin13";
          else  return RegionTag+"_bin14";
        }

      }
      else{

	//#################### HighJET BINS                                                                                                                                                                       

	if(met2_st < 5){
          if(LT < 150)       return RegionTag+"_bin15";
          else if(LT < 200)  return RegionTag+"_bin16";
          else if(LT < 250)  return RegionTag+"_bin17";
          else if(LT < 300)  return RegionTag+"_bin18";
          else               return RegionTag+"_bin19";
        }
        else{
          if(LT < 150)       return RegionTag+"_bin20";
          else if(LT < 200)  return RegionTag+"_bin21";
          else if(LT < 300)  return RegionTag+"_bin22";
          else  return RegionTag+"_bin23";
        }/// MET     
      }
    } 
    if(DataYear == 2017){
     
      if(LowJet){
	if(met2_st < 5){
	  if(ll_dphi > 2.5){
	    if(LT<= 150)             return RegionTag+"_bin1";
	    else if(LT<= 200)        return RegionTag+"_bin2";
	    else if(LT<= 250)        return RegionTag+"_bin3";
	    else if(LT<= 300)        return RegionTag+"_bin4";
	    else if(LT<= 400)        return RegionTag+"_bin5";
	    else                     return RegionTag+"_bin6";
	  }
	  else{
	    if(LT<= 150)         return RegionTag+"_bin7";
	    else if(LT<= 200)    return RegionTag+"_bin8";
	    else if(LT<= 250)    return RegionTag+"_bin9";
	    else                 return RegionTag+"_bin10";
	  }
	}
	else{
	  if( LT<= 150 )        return RegionTag+"_bin11";
	  else if( LT<= 250 )   return RegionTag+"_bin12";
	  else if( LT<= 350 )   return RegionTag+"_bin13";
	  else  return RegionTag+"_bin14";
	}
      }
      else{
	
	//#################### HighJET BINS                                                                                                                                                                                                                                   
	
	if(met2_st < 5){
	  if(LT < 150)       return RegionTag+"_bin15";
	  else if(LT < 200)  return RegionTag+"_bin16";
	  else if(LT < 300)  return RegionTag+"_bin17";
	  else if(LT < 350)  return RegionTag+"_bin18";
	  else               return RegionTag+"_bin19";
	}
	else{
	  if(LT < 150)  return RegionTag+"_bin20";
	  else if(LT < 200)  return RegionTag+"_bin21";
	  else if(LT < 300)  return RegionTag+"_bin22";
	  else  return RegionTag+"_bin23";
	}/// MET  
      }
    }   
    
    if(DataYear == 2018){
      if(LowJet){
	if(met2_st < 5){
          if(ll_dphi > 2.5){
            if(LT<= 150)             return RegionTag+"_bin1";
            else if(LT<= 175)        return RegionTag+"_bin2";
            else if(LT<= 250)        return RegionTag+"_bin3";
            else if(LT<= 300)        return RegionTag+"_bin4";
            else if(LT<= 350)        return RegionTag+"_bin5";
            else                     return RegionTag+"_bin6";
          }
          else{
            if(LT<= 150)         return RegionTag+"_bin7";
            else if(LT<= 200)    return RegionTag+"_bin8";
            else if(LT<= 300)    return RegionTag+"_bin9";
            else                 return RegionTag+"_bin10";
          }
        }
        else{
          if( LT<= 200 )        return RegionTag+"_bin11";
          else if( LT<= 300 )   return RegionTag+"_bin12";
          else if( LT<= 400 )   return RegionTag+"_bin13";
          else  return RegionTag+"_bin14";
        }
      }
      else{
	if(met2_st < 5){
          if(LT < 150)       return RegionTag+"_bin15";
          else if(LT < 200)  return RegionTag+"_bin16";
          else if(LT < 300)  return RegionTag+"_bin17";
          else if(LT < 400)  return RegionTag+"_bin18";
          else               return RegionTag+"_bin19";
        }
        else{
          if(LT < 150)       return RegionTag+"_bin20";
          else if(LT < 300)  return RegionTag+"_bin21";
          else if(LT < 400)  return RegionTag+"_bin22";
          else  return RegionTag+"_bin23";
        }
      }	
    }
    return "";
  }
  
  return "";
  
}


vector<TString> HNL_LeptonCore::GetBDTLimitLabels(TString key){

  map<TString,vector<TString> > ::iterator mit = map_BDT_bins_labels.find(key);
  if(mit == map_BDT_bins_labels.end()) {
    cout << "GetBDTLimitLabels " << key << endl;
    exit(EXIT_FAILURE);
  }
  vector<TString> SRlabels = mit->second;

  return SRlabels;
}


double HNL_LeptonCore::GetLimitBin(TString region, vector<Lepton*> leps, vector<Jet> AK4Jets, vector<FatJet> AK8_JetColl , Event ev, double & nbins_reg){


  double Binvalue=0;
  
  
  if(region=="CR_SR1_Inv"){

    Particle l1J =  *leps[0] +  AK8_JetColl[0];
    if(l1J.M() < 200) Binvalue=0.5;
    else if(l1J.M() < 500) Binvalue=1.5;
    else if(l1J.M() < 750) Binvalue= 2.5;
    else if(l1J.M() < 1000) Binvalue= 3.5;
    else Binvalue= 4.5;
    //// 3 bins                                                                                                                                                  
    nbins_reg=5;
    return Binvalue;
  }
  if(region=="CR_SR2_Inv"){
    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 1) Binvalue = 0.5;
    else if(SR2BinValue < 2) Binvalue = 1.5;
    else   if(SR2BinValue < 5) Binvalue = 2.5;
    else   Binvalue = 3.5;

    nbins_reg=4;
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

  if(region.Contains("CR_SR2")){

    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 2) Binvalue = 0.5;
    else   if(SR2BinValue < 5) Binvalue = 1.5;
    else   Binvalue = 2.5;

    nbins_reg=3;
    return Binvalue;

  }

  if(region.Contains("CR_SR3")){
    if((AK4Jets.size()<2) && (leps[1]->Pt()  < 50)) Binvalue=0.5;
    else   if((AK4Jets.size()<2) && (leps[1]->Pt()  < 150)) Binvalue=1.5;
    else   if((AK4Jets.size()<2)) Binvalue=2.5;
    else   {
      Particle llJJ =  *leps[0] + *leps[1]+AK4Jets[0]+AK4Jets[1];
      if(llJJ.M() < 200) Binvalue= 3.5;
      else     if(llJJ.M() < 400) Binvalue= 4.5;
      else Binvalue= 5.5;
    }
    nbins_reg=6;
    return Binvalue;
    
  }

  if(region.Contains("CR_CF_SR1")){

    Particle l1JJ =  *leps[0] + AK8_JetColl[0];

    if(l1JJ.M() < 200) Binvalue= 0.5;
    else     if(l1JJ.M() < 750) Binvalue= 1.5;
    else Binvalue= 2.5;
    nbins_reg=3;
    return Binvalue;
    
  }

  if(region.Contains("CR_CF_SR2")){

    double SR2BinValue = leps[0]->HTOverPt();
    if(SR2BinValue < 2) Binvalue=0.5;
    else if(SR2BinValue < 5) Binvalue=1.5;
    else if(SR2BinValue < 10) Binvalue=2.5;
    nbins_reg=3;
    return Binvalue;
   

  }
  
  if(region.Contains("CR_CF_SR3")){

    if(leps[0]->Pt() < 50) Binvalue = 0.5;
    else if(leps[0]->Pt() < 200) Binvalue = 1.5;
    else Binvalue = 2.5;
    nbins_reg=3;
    return Binvalue;

  }

  if(region.Contains("CR_WW_SR2")){
    double VBFBinValue = leps[0]->HTOverPt();
    if(VBFBinValue < 1) Binvalue= 0.5;
    else     if(VBFBinValue < 2) Binvalue= 1.5;
    else     if(VBFBinValue < 5) Binvalue= 2.5;
    else Binvalue= 3.5;
    nbins_reg=4;

    return Binvalue;
  }

  cout << "No Limit Region set" << endl;
  exit(EXIT_FAILURE);

}

