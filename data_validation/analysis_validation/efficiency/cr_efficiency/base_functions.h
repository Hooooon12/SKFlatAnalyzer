#ifndef base_functions_h
#define base_functions_h

#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fstream>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>  
#include <iomanip>
#include "TString.h"
#include "TSystem.h"
#include "TLegend.h"
#include "TFile.h"
#include "TGraphAsymmErrors.h"
#include "TROOT.h"
#include "TH1D.h"

#include "std_functions.h"
#include "list_functions.h"

void SaveAndCopyLXPLUSScan(TCanvas* c, TString outpath, TString newoutpath, TString histname, TString analyser, TString tag, TString era){

  TString save_pdf= outpath+".pdf";
  TString save_png= outpath+".png";
  
  TString outpdf= newoutpath+".pdf";
  TString outpng= newoutpath+".png";
  cout << "outpdf = " << outpdf << endl;
  
  TString lxpath = "/eos/home-j/jalmond/www/"+analyser+"/";
  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  lxpath+= tag+"/";  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  lxpath+= era+"/";  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  TString tag2=histname.ReplaceAll("/","_");
  lxpath+= tag2+"/"; system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");

  TString HTMLLink = "HNL/"+analyser+"/";


  cout << "ssh jalmond@lxplus.cern.ch 'cp  /eos/home-j/jalmond/www/index.php "+lxpath+"'" << endl;

  system("ssh jalmond@lxplus.cern.ch 'cp  /eos/home-j/jalmond/www/index.php "+lxpath+"'");

  cout << "scp " +save_pdf+" jalmond@lxplus.cern.ch:"+lxpath+"/" << endl;
  system("scp " +save_pdf+" jalmond@lxplus.cern.ch:"+lxpath+"/"+outpdf);
  system("scp " +save_png + " jalmond@lxplus.cern.ch:"+lxpath+"/"+outpng);
  cout << "https://jalmond.web.cern.ch/jalmond/SNU/WebPlots/"+HTMLLink+"/" << endl;

}


void SaveAndCopyLXPLUS(TCanvas* c, TString outpath, TString histname, TString analyser, TString tag, TString era){

  TString save_pdf= outpath+".pdf";
  TString save_png= outpath+".png";
  
  TString lxpath = "/eos/home-j/jalmond/www/WebPlots/"+analyser+"/";
  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  lxpath+= tag+"/";  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  lxpath+= era+"/";  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  TString tag2=histname.ReplaceAll("/","_");
  lxpath+= tag2+"/"; system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");

  TString HTMLLink = "HNL/"+analyser+"/";


  cout << "ssh jalmond@lxplus.cern.ch 'cp  /eos/home-j/jalmond/www/index.php "+lxpath+"'" << endl;
  
  system("ssh jalmond@lxplus.cern.ch 'cp  /eos/home-j/jalmond/www/index.php "+lxpath+"'");
  
  cout << "scp " +save_pdf+" jalmond@lxplus.cern.ch:"+lxpath+"/" << endl;
  system("scp " +save_pdf+" jalmond@lxplus.cern.ch:"+lxpath+"/");                    
  system("scp " +save_png + " jalmond@lxplus.cern.ch:"+lxpath+"/");                    
  cout << "https://jalmond.web.cern.ch/jalmond/SNU/WebPlots/"+HTMLLink+"/" << endl; 
  
}

void CopyLXPLUSCutFlow(TString outpath, TString histname, TString analyser, TString tag, TString era){

  TString save_pdf= outpath;

  //tag=tag+histname.ReplaceAll("/","_");

  TString lxpath = "/eos/home-j/jalmond/www/"+analyser+"/";
  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  lxpath+= tag+"/";  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  lxpath+= era+"/";  system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  TString tag2=histname.ReplaceAll("/","_");
  lxpath+= tag2+"/"; system("ssh jalmond@lxplus.cern.ch 'mkdir -p "+lxpath+"'");
  
  cout << "scp " +save_pdf+" jalmond@lxplus.cern.ch:"+lxpath+"/" << endl;
  system("scp " +save_pdf+" jalmond@lxplus.cern.ch:"+lxpath+"/");


}

int CheckFile(TFile* f){
  
  cout << "########################"<< endl;
  cout << "Check file: " << f->GetName() << endl;
  if (f->IsZombie()) return 1;
  if (f->TestBit(1024)) return 2;

  return 0;
}


TFile* GetFile(TString path){
  
  TFile * file_bkg = new TFile((path).Data());
  
  if(CheckFile(file_bkg) > 0) {
    gApplication->Terminate();
  }
  else cout << "CheckFile Success" << endl;
  
  return file_bkg;
}

TCanvas*  MakeCanvas(TString Version, TString tag){
  
  int x = 800;
  int y = 800;
  if(Version==""){ x= 800; y= 800;};

  TCanvas* c1 = new TCanvas(tag,tag, x,y);
 
  return c1;  
  
}
TString GetOutPut(TString analysername, TString era){
  
  TString ENV_PLOT_PATH = getenv("PLOT_PATH");
  TString FLATVERSION = getenv("FLATVERSION");
  TString output = ENV_PLOT_PATH + FLATVERSION + "/"+analysername+"/";
  output+=era+"/";
  
  return output;

}
void SetUpDir(TString analysername, TString era){
  
  PrintSetup(analysername,era);

  TString ENV_MERGEDFILE_PATH = getenv("FILE_MERGED_PATH");

  TString ENV_PLOT_PATH = getenv("PLOT_PATH");
  TString FLATVERSION = getenv("FLATVERSION");

  MakeDir(ENV_PLOT_PATH + FLATVERSION);

  TString output = ENV_PLOT_PATH + FLATVERSION + "/";

  MakeDir(output);
  output+="/"+analysername+"/";
  MakeDir(output);

  output+=era+"/";
  MakeDir(output);

  return;
}


TLegend* MakeLegend(double x1, double x2, double y1, double y2, double size_text=0.04){

  TLegend *lg = new TLegend(x1, x2, y1, y2);
  lg->SetFillStyle(0);
  lg->SetBorderSize(0);
  lg->SetTextSize(size_text);
  return lg;
  
}

int GetHistStyle(int nth_samples){

  vector<int> _style = {1,2,1,2,1,4,3,4,3,4,5,6,7,5,6,7,8,9};
  if(nth_samples > int(_style.size())) return _style[0];
  return _style[nth_samples];

}

Color_t GetHistColor(int nth_samples){
  vector <Color_t > _colors;
  _colors.push_back(kRed);
  _colors.push_back(800);
  _colors.push_back(870);
  _colors.push_back(kSpring-1);
  _colors.push_back(kGray);
  _colors.push_back(kViolet);
  _colors.push_back(kYellow+4);
  _colors.push_back(kCyan);
  _colors.push_back(kBlue-2);
  _colors.push_back(kGreen-2);
  _colors.push_back(kGreen+2);
  _colors.push_back(kOrange-2);

  if(nth_samples > int(_colors.size())) return _colors[0];
  return _colors[nth_samples];
  
}



vector<pair<Color_t,int> >  GetHistColors(int nsamples){
  
  vector<pair<Color_t,int > > _colors;
  for(unsigned int i = 0 ; i < nsamples; i++) _colors.push_back(make_pair(GetHistColor(i),GetHistStyle(i)));
  
  return _colors;
  
}
 
void MakeLatex(TString name, TString input , TString output ){

  TString ENV_FILE_PATH= getenv("HNDILEPTONWORKSPACE_DIR");
  
  TString workdir = ENV_FILE_PATH+ "/Latex/workspace/";
  TString texfile = ENV_FILE_PATH+ "/Latex/workspace/"+name+".tex";
  TString dvifile = name+".dvi"; 
  TString pdffile = name+".pdf"; 

  //// Make TEX file
  ofstream ofile_tex;

  ofile_tex.open(texfile.Data());
  ofile_tex.setf(ios::fixed,ios::floatfield);
  ofile_tex << "\\documentclass[10pt]{article}" << endl;
  ofile_tex << "\\usepackage{epsfig,subfigure,setspace,xtab,xcolor,array,colortbl}" << endl;
  ofile_tex << "\\usepackage{adjustbox}" << endl;   

  ofile_tex << "\\usepackage{pdflscape}" << endl;
  ofile_tex << "\\begin{document}" << endl;
  ofile_tex << "\\input{" + input + "}" << endl;
  ofile_tex << "\\end{document}" << endl;

  TString latex_command = "latex " + texfile;
  TString dvi_command = "dvipdf " + dvifile;
  TString mv_command = "mv " + pdffile + " " + output;

  system((latex_command.Data()));
  system((dvi_command.Data()));
  system((mv_command.Data()));
  system(("rm *aux"));
  system(("rm " + workdir + "/*aux"));
  system(("rm *log"));
  system(("rm " + workdir + "/*log"));
  system(("rm *dvi"));
  system(("rm " + workdir + "/*dvi"));

}

void AllLegendEntry(TLegend* leg, TGraphAsymmErrors* g, TString name, TString type){
    leg->AddEntry(g, name, type);
}
void AllLegendEntry(TLegend* leg, TH1* h, TString name, TString type){
  leg->AddEntry(h, name, type);

}
		    
const vector<string> _getsplit(const string& s, const char& c)
{
  string buff{""};
  vector<string> v;
  
  for(auto n:s)
    {
      if(n != c) buff+=n; else
	if(n == c && buff != "") { v.push_back(buff); buff = ""; }
    }
  if(buff != "") v.push_back(buff);
  
  return v;
}


void           OutMessage(TString s_code, TString  s_file){
  cout << s_code << " : created file " << s_file << endl;
}

bool IsVBFMass(TString mass){

  vector <TString> masses ;
  masses.push_back("500");
  masses.push_back("600");
  masses.push_back("700");
  masses.push_back("800");
  masses.push_back("900");
  masses.push_back("1000");
  masses.push_back("1100");
  masses.push_back("1200");
  masses.push_back("1300");
  masses.push_back("1500");
  masses.push_back("1700");
  masses.push_back("2000");
  masses.push_back("2500");
  masses.push_back("3000");

  for(auto imass: masses){
    if(imass == mass) return true;
  }
  
  return false;
}
bool IsDYMass(TString mass){

  vector <TString> masses ;
  masses.push_back("85");
  masses.push_back("90");
  masses.push_back("95");
  masses.push_back("100");
  masses.push_back("125");
  masses.push_back("200");
  masses.push_back("250");
  masses.push_back("300");
  masses.push_back("400");
  masses.push_back("500");
  masses.push_back("600");
  masses.push_back("700");
  masses.push_back("800");
  masses.push_back("900");
  masses.push_back("1000");
  masses.push_back("1100");
  masses.push_back("1200");
  masses.push_back("1300");
  masses.push_back("1500");
  masses.push_back("1700");
  masses.push_back("2000");
  masses.push_back("2500");
  masses.push_back("3000");
  for(auto imass: masses){
    if(imass == mass) return true;
  }

  return false;

}

bool IsSSWWMass(TString mass){
  
  vector <TString> masses ;
  masses.push_back("500");
  masses.push_back("600");
  masses.push_back("700");
  masses.push_back("800");
  masses.push_back("900");
  masses.push_back("1000");
  masses.push_back("1100");
  masses.push_back("1200");
  masses.push_back("1300");
  masses.push_back("1500");
  masses.push_back("1700");
  masses.push_back("2000");
  masses.push_back("2500");
  masses.push_back("3000");

  masses.push_back("5000");
  //masses.push_back("7500");
  masses.push_back("10000");
  masses.push_back("15000");
  masses.push_back("20000");

  for(auto imass: masses){
    if(imass == mass) return true;
  }

  return false;

}
vector<TString> GetMassType1Strings(vector<TString> ignore_masses, TString channel){

  //TString split_string = ignore_masses;
  //split_string= split_string.ReplaceAll("/"," ");
  TH1* h;
  //vector<TString> v//{_getsplit(string(split_string), ' ')};


  vector <TString> masses ;
  if(channel != "Tchannel"){
    masses.push_back("85");
    masses.push_back("90");
    masses.push_back("95");
    masses.push_back("100");
    masses.push_back("125");
    masses.push_back("200");
    masses.push_back("250");
    masses.push_back("300");
    masses.push_back("400");
  }  
  masses.push_back("500");
  masses.push_back("600");
  masses.push_back("700");
  masses.push_back("800");
  masses.push_back("900");
  masses.push_back("1000");
  masses.push_back("1100");
  masses.push_back("1200");
  masses.push_back("1300");
  masses.push_back("1500");
  masses.push_back("1700");
  masses.push_back("2000");
  masses.push_back("2500");
  masses.push_back("3000");

  masses.push_back("5000");
  masses.push_back("7500");
  masses.push_back("10000");
  masses.push_back("15000");
  masses.push_back("20000");

  
  vector <TString> _masses ;
  for(unsigned int i=0; i < masses.size(); i++){
    bool ignore=false;
    for(unsigned int j=0; j < ignore_masses.size(); j++){
      if(masses[i] == ignore_masses[j]) ignore=true;
    }
    if (!ignore) _masses.push_back(masses[i]);
  }
  return _masses;

}

double Stod(string word){

  double lol = atof(word.c_str());
  return lol;
}

TString DToS(double d){

  std::string str = std::to_string (d);
  str.erase ( str.find_last_not_of('0') + 1, std::string::npos );
  str.erase ( str.find_last_not_of('.') + 1, std::string::npos );

  TString ts_str = TString(str);
  ts_str = ts_str.ReplaceAll(".","p");
  ts_str = ts_str.ReplaceAll("-","neg");

  return ts_str;

}


vector<double> GetMassType1Doubles(vector<TString> ignore_masses, TString channel){

  //TString split_string = ignore_masses;
  //split_string= split_string.ReplaceAll("/"," ");
  TH1* h;
  //vector<string> v{_getsplit(string(split_string), ' ')};

  vector <TString> masses = GetMassType1Strings(ignore_masses, channel);

  vector <double> _masses;
  for(unsigned int i=0; i < masses.size(); i++){
    bool ignore=false;
    for(unsigned int j=0; j < ignore_masses.size(); j++){
      if(masses[i] == ignore_masses[j]) ignore=true;
    }
    if (!ignore) _masses.push_back(atof(masses[i]));
  }
  return _masses;

}



#endif

