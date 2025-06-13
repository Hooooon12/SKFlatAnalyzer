import os,ROOT
import math
from array import array
from math import log10, floor

import ctypes

def round_to_1(x):
    return round(x, -int(floor(log10(abs(x)))))

def Print(out, line,PrintScreen):
    if PrintScreen:
        print(line)
    out.write(line+"\n")

def Validate(h1,h2):

    if not h1 or not h2:
        print (h1)
        print (h2)
        exit()
    return 

def CalculdateSignificance(method,Nsig, Nbkg,scaleSig):

    Nsig = Nsig*scaleSig
    ###### set Neg bins to 0                                                                                                                                                
    if Nsig < 0:
        Nsig = 0
    if  Nbkg < 0.1:
        Nbkg=0.1

    Signi=0.
    if method == "SB":
        Signi=Nsig/ math.sqrt(Nbkg +1)

    if method == "Punzi":
        Signi= Nsig / ( 1 + math.sqrt(float(Nbkg)))

    if method == "Azimoth":
        Signi=  math.sqrt(2* ((Nsig+Nbkg)*math.log(1+(Nsig/Nbkg)) -Nsig ) )

    return Signi /scaleSig

def PrintSame(st, num):
    
    st=round(st,2)
    lenST = len(str(st))
    if num < lenST:
        num = lenST

    outString=str(st) +  " "*(num-lenST)
    return outString


def GetSignificanceArray(out,h_sig, h_bkg,FOM,scaleSig, PastSignifiance):

    SigBins = []
    BkgBins = []

    for xbin in range(1,h_sig.GetNbinsX()+1):
        SigBins.append(h_sig.GetBinContent(xbin))

    for xbin in range(1,h_bkg.GetNbinsX()+1):
        BkgBins.append(h_bkg.GetBinContent(xbin))

    SignifBins = []

    for x in range(0, len(BkgBins)):
        Bkg=BkgBins[x]
        SigBin = SigBins[x]

        SignifBins.append(CalculdateSignificance(FOM,SigBin,Bkg,scaleSig))

        
    return SignifBins

def PrintString(nonfomrattted_string, length):
    
    return nonfomrattted_string + " " * (length-len(nonfomrattted_string))

def PrintValue(val, rounded, length):
    
    return str(round(val, rounded)) + " " * (length-len(str(round(val, rounded))))


def GetLumiScale(era):
    
    if era == "YearCombined":
        return float((41.5+36.3+59.9)/36.3)

    era = int(era)
    if era == 2016:
        return 1
    
    if era == 2017:
        return float(41.5/36.3)

    if era == 2018:
        return float(59.9/36.3)

def GetSignificance(MergeSR,out,SignalMode,_Era,ID,h_sigDY,h_sigVBF,  h_sigSSWW, h_bkg,FOM,scaleSig, PastSignifiance,PastSig, PastBkg, _mass):
    
    print (ID)

    SigBins     = []
    SigBinsDY   = []
    SigBinsVBF  = []
    SigBinsSSWW = []
    BkgBins = []
    
    SumSR=MergeSR
   
    for xbin in range(1,h_sigDY.GetNbinsX()+1):

        signal_yield=h_sigDY.GetBinContent(xbin)

        if h_sigVBF and "VBF" in SignalMode:
            SigBinsVBF.append(h_sigVBF.GetBinContent(xbin))
            signal_yield=signal_yield+h_sigVBF.GetBinContent(xbin)
        else:
            SigBinsVBF.append(0.)
        
        if h_sigSSWW and "SSWW" in SignalMode:
            SigBinsSSWW.append(h_sigSSWW.GetBinContent(xbin))
            signal_yield=signal_yield+h_sigSSWW.GetBinContent(xbin)
        else:
            SigBinsSSWW.append(0.)
        
        SigBinsDY.append(h_sigDY.GetBinContent(xbin))
        SigBins.append(signal_yield)

    for xbin in range(1,h_bkg.GetNbinsX()+1):
        if not SumSR:
            if h_bkg.GetBinContent(xbin) > 0.1:
                BkgBins.append(h_bkg.GetBinContent(xbin))
            else:
                BkgBins.append(0.1)
        else:
            if h_bkg.GetBinContent(xbin) > 0.:
                BkgBins.append(h_bkg.GetBinContent(xbin))
            else:
                BkgBins.append(0.)


    Signif = 0.
    SignifAz = 0.
    SignifSB = 0.
    SignifP = 0.
    TotalSig= 0.
    TotalBkg= 0.
    SR1Sig=0
    SR1Sig=0
    SR2Sig=0
    SR3Sig=0
    SR1Bkg=0
    SR2Bkg=0
    SR3Bkg=0
    SignifSR1=0
    SignifSR2=0
    SignifSR3=0
    SR1SigDY=0
    SR1SigVBF=0
    SR1SigSSWW=0

    SR2SigDY=0
    SR2SigVBF=0
    SR2SigSSWW=0

    SR3SigDY=0
    SR3SigVBF=0
    SR3SigSSWW=0

    print(("_"*150))
    print((len(BkgBins)))
    print((len(SigBinsDY)))
    print((len(SigBinsVBF)))
    print((len(SigBinsSSWW)))
    for x in range(0, len(BkgBins)):
        Bkg=BkgBins[x]
        SigBin = SigBins[x]
    
        #### INDIVIDUAL SIGs
        SigBinDY   = SigBinsDY[x]
        SigBinVBF  = SigBinsVBF[x]
        SigBinSSWW = SigBinsSSWW[x]


        Signif   = Signif   + CalculdateSignificance(FOM,      SigBin,Bkg,scaleSig)
        SignifAz = SignifAz + CalculdateSignificance("Azimoth",SigBin,Bkg,scaleSig)
        SignifSB = SignifSB + CalculdateSignificance("SB",     SigBin,Bkg,scaleSig)
        SignifP  = SignifP  + CalculdateSignificance("Punzi",  SigBin,Bkg,scaleSig)

        TotalSig = TotalSig + SigBin
        TotalBkg = TotalBkg + Bkg
        Binlab= str(h_sigDY.GetXaxis().GetBinLabel(x+1))
        if "SR1" in Binlab:
            SR1SigDY  = SR1SigDY   + SigBinDY
            SR1SigVBF = SR1SigVBF  + SigBinVBF
            SR1SigSSWW= SR1SigSSWW + SigBinSSWW

            SR1Sig=SR1Sig+SigBin
            SR1Bkg=SR1Bkg+Bkg
            SignifSR1=SignifSR1+CalculdateSignificance(FOM,SigBin,Bkg,scaleSig)
        if "SR2" in Binlab:

            SR2SigDY  = SR2SigDY   + SigBinDY
            SR2SigVBF = SR2SigVBF  + SigBinVBF
            SR2SigSSWW= SR2SigSSWW + SigBinSSWW

            SR2Sig=SR2Sig+SigBin
            SR2Bkg=SR2Bkg+Bkg
            SignifSR2=SignifSR2+CalculdateSignificance(FOM,SigBin,Bkg,scaleSig)

        if "SR3" in Binlab:
            
            SR3SigDY  = SR3SigDY   + SigBinDY
            SR3SigVBF = SR3SigVBF  + SigBinVBF
            SR3SigSSWW= SR3SigSSWW + SigBinSSWW


            SR3Sig=SR3Sig+SigBin
            SR3Bkg=SR3Bkg+Bkg
            SignifSR3=SignifSR3+CalculdateSignificance(FOM,SigBin,Bkg,scaleSig)


        Print(out, "Bin " + PrintString(Binlab,20) + " NSig = " + PrintValue(SigBin,3,10) + " NBkg = " + PrintValue(Bkg,3,10) 
              + " s/sqrt(B+1) = " + PrintValue(CalculdateSignificance("SB",     SigBin,Bkg,scaleSig),3,10) 
              + " Punzi = "       + PrintValue(CalculdateSignificance("Punzi",  SigBin,Bkg,scaleSig),3,10)  
              + " Azimoth = "     + PrintValue(CalculdateSignificance("Azimoth",SigBin,Bkg,scaleSig),3,10),True)

    print(("_"*150))
    print(("Total Signal Yield = " + str(round(TotalSig,3)) + " total bkg = " + PrintValue(TotalBkg,3,10) + " Combined bin Significance : "
           + " s/sqrt(B+1) = "     + PrintValue(CalculdateSignificance("SB",TotalSig,TotalBkg,scaleSig),3,10) 
           + " Punzi = "           + PrintValue(CalculdateSignificance("Punzi",TotalSig,TotalBkg,scaleSig),3,10)
           + " Azimoth = "         + PrintValue(CalculdateSignificance("Azimoth",TotalSig,TotalBkg,scaleSig),3,10) 
           + " (Assumming all bins merged into 1)"))
    

    print(("@"*150))
    print(("Summed FOM for bins in each SR ["+ID+"]:"))
    print(("Mass " + str(_mass) + 
           " SR1 [AK8] NSig " +PrintValue(SR1Sig,3,10) + 
           "[DY " + PrintValue(SR1SigDY,3,10) + " VBF " + PrintValue(SR1SigVBF,3,10)+ " SSWW " + PrintValue(SR1SigSSWW,3,10)+"]"
           + " Nbkg = " + PrintValue(SR1Bkg,3,10) 
           + " MergedFOM = " + PrintValue(CalculdateSignificance("Azimoth",SR1Sig,SR1Bkg,scaleSig),3,10) + " CombinedFOM "+ PrintValue(SignifSR1,3,10)))

    print(("Mass " + str(_mass) +
           " SR2 [VBF] NSig " +PrintValue(SR2Sig,3,10) +
           "[DY " + PrintValue(SR2SigDY,3,10) + " VBF " + PrintValue(SR2SigVBF,3,10)+ " SSWW " + PrintValue(SR2SigSSWW,3,10)+"]"
           + " Nbkg = " + PrintValue(SR2Bkg,3,10)
           + " MergedFOM = " + PrintValue(CalculdateSignificance("Azimoth",SR2Sig,SR2Bkg,scaleSig),3,10) + " CombinedFOM "+ PrintValue(SignifSR2,3,10)))

    print(("Mass " + str(_mass) +
           " SR3 [AK4] NSig " +PrintValue(SR3Sig,3,10) +
           "[DY " + PrintValue(SR3SigDY,3,10) + " VBF " + PrintValue(SR3SigVBF,3,10)+ " SSWW " + PrintValue(SR3SigSSWW,3,10)+"]"
           + " Nbkg = " + PrintValue(SR3Bkg,3,10)
           + " MergedFOM = " + PrintValue(CalculdateSignificance("Azimoth",SR3Sig,SR3Bkg,scaleSig),3,10) + " CombinedFOM "+ PrintValue(SignifSR3,3,10)))
    
    print(("@"*150))

    print(("Mass " + str(_mass) + " EXO 17-028 SR1 [AK4] Sig " 
           + PrintValue(PastSig[0][0]+PastSig[1][0],3,10)  
           + "[ DY " + PrintValue(PastSig[0][0],3,10)+ " VBF "+  PrintValue(PastSig[1][0],3,10)+ "]" 
           + " Nbkg " + PrintValue(PastBkg[0],3,10) 
           + " FOM "    + PrintValue(CalculdateSignificance("Azimoth",PastSig[0][0]+PastSig[1][0],PastBkg[0],scaleSig),3,10)))

    print(("Mass " + str(_mass) + " EXO 17-028 SR2 [AK8] Sig " 
           + PrintValue(PastSig[0][1]+PastSig[1][1],3,10)  
           + "[ DY " + PrintValue(PastSig[0][1],3,10)+" VBF "+ PrintValue(PastSig[1][1],3,10)+ "]" 
           + " Nbkg " + PrintValue(PastBkg[1],3,10) 
           + " FOM "  + PrintValue(CalculdateSignificance("Azimoth",PastSig[0][1]+PastSig[1][1],PastBkg[1],scaleSig),3,10)))
    LumiScale=GetLumiScale(_Era)
    print(("Lumi Scaled: Mass " + str(_mass) + " EXO 17-028 SR1 [AK4] Sig "
           + PrintValue(LumiScale*PastSig[0][0]+LumiScale*PastSig[1][0],3,10)
           + "[ DY " + PrintValue(LumiScale*PastSig[0][0],3,10)+ " VBF "+  PrintValue(LumiScale*PastSig[1][0],3,10)+ "]"
           + " Nbkg " + PrintValue(LumiScale*PastBkg[0],3,10)
           + " FOM "    + PrintValue(CalculdateSignificance("Azimoth",LumiScale*PastSig[0][0]+LumiScale*PastSig[1][0],LumiScale*PastBkg[0],scaleSig),3,10)))

    print(("Lumi Scaled: Mass " + str(_mass) + " EXO 17-028 SR2 [AK8] Sig "
           + PrintValue(LumiScale*PastSig[0][1]+LumiScale*PastSig[1][1],3,10)
           + "[ DY " + PrintValue(LumiScale*PastSig[0][1],3,10)+" VBF "+ PrintValue(LumiScale*PastSig[1][1],3,10)+ "]"
           + " Nbkg " + PrintValue(LumiScale*PastBkg[1],3,10)
           + " FOM "  + PrintValue(CalculdateSignificance("Azimoth",LumiScale*PastSig[0][1]+LumiScale*PastSig[1][1],LumiScale*PastBkg[1],scaleSig),3,10)))


    print(("_"*150))
    print ("Summed signifance of individual bins: Current / EXO-17-0-28 (Ratio)")
    
    print(("Signif {0}  = {1} : {2} ({3}) ".format("Azimoth  ",
                                                   str(round(SignifAz,3)),
                                                   GetSigByType(PastSignifiance,"Azimoth"),
                                                   str(round( (float(SignifAz)/float(GetSigByType(PastSignifiance,"Azimoth"))) ,3)))))
    
    print(("Signif {0}  = {1} : {2} ({3}) ".format("S/sqrt(B)",
                                                   str(round(SignifSB,3)),
                                                   GetSigByType(PastSignifiance,"SB"),
                                                   str(round( (float(SignifSB)/float(GetSigByType(PastSignifiance,"SB"))) ,3)))))

    print(("Signif {0}  = {1} : {2} ({3}) ".format("Punzi    ",
                                                   str(round(SignifP,3)),
                                                   GetSigByType(PastSignifiance,"Punzi"),
                                                   str(round( (float(SignifP)/float(GetSigByType(PastSignifiance,"Punzi"))) ,3)))))

    print(("_"*150))


    if SumSR:

        SR1_Sig=0
        SR2_Sig=0
        SR1_Bkg=0
        SR2_Bkg=0
        SR3a_Bkg=0
        SR3a_Sig=0
        SR3b_Bkg=0
        SR3b_Sig=0
        SR3c_Bkg=0
        SR3c_Sig=0


        for x in range(0, len(BkgBins)):
            Binlab= str(h_sigDY.GetXaxis().GetBinLabel(x+1))
            if "SR1" in Binlab:
                SR1_Sig=SR1_Sig+SigBins[x]
                SR1_Bkg=SR1_Bkg+BkgBins[x]
            if "SR2" in Binlab:
                SR2_Sig=SR2_Sig+SigBins[x]
                SR2_Bkg=SR2_Bkg+BkgBins[x]
            if "SR3" in Binlab:
                
                if x < 13:
                    SR3a_Bkg =SR3a_Bkg +BkgBins[x]
                    SR3a_Sig =SR3a_Sig +SigBins[x]
                elif x < 16:
                    SR3b_Bkg =SR3b_Bkg +BkgBins[x]
                    SR3b_Sig =SR3b_Sig +SigBins[x]
                else:
                    SR3c_Bkg =SR3c_Bkg +BkgBins[x]
                    SR3c_Sig =SR3c_Sig +SigBins[x]

                    
        print("%"*150)
        Print(out, "Merged Bin " + PrintString("SR1",20) + " NSig = " + PrintValue(SR1_Sig,3,10) + " NBkg = " + PrintValue(SR1_Bkg,3,10) + " FOM = " + PrintValue(CalculdateSignificance(FOM,      SR1_Sig,SR1_Bkg,scaleSig), 3, 10),True)
        Print(out, "Merged Bin " + PrintString("SR2",20) + " NSig = " + PrintValue(SR2_Sig,3,10) + " NBkg = " + PrintValue(SR2_Bkg,3,10) + " FOM = " + PrintValue(CalculdateSignificance(FOM,      SR2_Sig,SR2_Bkg,scaleSig), 3, 10),True)
        Print(out, "Merged Bin " + PrintString("SR3a",20) + " FOM = " + PrintValue(CalculdateSignificance(FOM,      SR3a_Sig,SR3a_Bkg,scaleSig), 3, 10),True)
        Print(out, "Merged Bin " + PrintString("SR3b",20) + " FOM = " + PrintValue(CalculdateSignificance(FOM,      SR3b_Sig,SR3b_Bkg,scaleSig), 3, 10),True)
        Print(out, "Merged Bin " + PrintString("SR3c",20) + " FOM = " + PrintValue(CalculdateSignificance(FOM,      SR3c_Sig,SR3c_Bkg,scaleSig), 3, 10),True)

        SummedSR_Signif   =  CalculdateSignificance(FOM,      SR1_Sig,SR1_Bkg,scaleSig) + CalculdateSignificance(FOM,      SR2_Sig,SR2_Bkg,scaleSig)   + CalculdateSignificance(FOM,      SR3a_Sig,SR3a_Bkg,scaleSig) + CalculdateSignificance(FOM,      SR3b_Sig,SR3b_Bkg,scaleSig) + CalculdateSignificance(FOM,      SR3c_Sig,SR3c_Bkg,scaleSig)

        Print(out, "Merged FOM = " + PrintValue(SummedSR_Signif,3,10),True)
        
        return SummedSR_Signif


    return Signif

def GetSigByType(signifs, Tag):
    for x in signifs:
        if x[0] == Tag:
            return round(x[1],3)
        
    exit()

def GetAllObjs(d, basepath="/"):
    
    for key in d.GetListOfKeys():
        kname = key.GetName()
        if key.IsFolder():
            for i in GetAllObjs(d.Get(kname), basepath+kname+"/"):
              yield i
        else:
            yield basepath+kname, d.Get(kname)



def AddHistograms(h_original, h_toAdd, option=''):
  if option=="L":

    h_out = h_original.Clone()
    for ix in range(0, h_original.GetXaxis().GetNbins()):
      y_original = h_original.GetBinContent(ix+1)
      e_original = h_original.GetBinError(ix+1)

      y_toAdd = h_toAdd.GetBinContent(ix+1)
      e_toAdd = h_toAdd.GetBinError(ix+1)

      h_out.SetBinContent(ix+1, y_original+y_toAdd)
      h_out.SetBinError(ix+1, e_original+e_toAdd)

    return h_out

  else:
    h_original.Add(h_toAdd)
    return h_original

def TotalLumiByEra(DataEra):
  
  if DataEra=="2016preVFP":
    return "19.5"
  if DataEra=="2016postVFP":
    return "16.8"
  if DataEra=="2017":
    return "41.5"
  if DataEra=="2018":
    return "59.8"

  if DataEra =="-1":
    return str(19.52+16.81+41.48+59.83)

  else:
    print(("[mylib.py, TotalLumiByEra()] Wrong DataEra : %s"%DataEra))
    return str(19.52+16.81)
    
    
  
def TotalLumi(DataYear):
    
  if DataYear==2016:
    return "36.3"
  if DataYear==2017:
    return "41.5"
  if DataYear==2018:
    return "59.8"

  if DataYear<0:
    return "138"
  else:
    print(("[mylib.py, TotalLumi()] Wrong DataYear : %d"%DataYear))
    return ("36.3")

def LumiError(DataYear):

  print ("Setting Unc. from https://twiki.cern.ch/twiki/bin/viewauth/CMS/TWikiLUM")
  
  if DataYear == -1:
    return 0.016
  
  if DataYear==2016:
    return 0.012
  elif DataYear==2017:
      return 0.023
  elif DataYear==2018:
    return 0.025
  else:
    print(("[mylib.py, LumiError()] Wrong DataYear : %d"%DataYear))
    return 0.

def TotalLumi(DataYear):

  if DataYear==2016:
    return "35.92"
  if DataYear==2017:
    return "41.53"
  if DataYear==2018:
    return "59.74"
  if DataYear<0:
    return "138"
  else:
    print(("[mylib.py, TotalLumi()] Wrong DataYear : %d"%DataYear))
    return "35.9"

def LumiError(DataYear):

  #if DataYear==2016:
  #  return 0.025
  #elif DataYear==2017:
  #  return 0.023
  #elif DataYear==2018:
  #  return 0.025
  #else:
  #  print "[mylib.py, LumiError()] Wrong DataYear : %d"%DataYear
  #  return 0.


  return 0.018

def MakeOverflowBin(hist):

  #### GetXaxis().SetRangeUser() should be done first

  #### 0     1                                          n_bin_origin
  ####     |===|===|===|===|===|===|===|===|===|===|===|===|===|
  ####           bin_first        bin_last
  ####             |===|===|===|===|===|
  #### |   |                                                   |   |
  #### under                                                   over
  #### flow                                                    flow
  #### |<=========>|                   |<=========================>|
  ####  all underflow                           all overflow

  #### Original NBins
  n_bin_origin = hist.GetXaxis().GetNbins()
  #### Changed NBins
  bin_first = hist.GetXaxis().GetFirst()
  bin_last = hist.GetXaxis().GetLast()
  n_bin_inrange = bin_last-bin_first+1

  x_first_lowedge = hist.GetXaxis().GetBinLowEdge(bin_first)
  x_last_upedge = hist.GetXaxis().GetBinUpEdge(bin_last)

  Allunderflows = hist.Integral(0, bin_first-1)
  Allunderflows_error = hist.GetBinError(0)
  Allunderflows_error = Allunderflows_error*Allunderflows_error
  for i in range(1, bin_first-1 +1):
    Allunderflows_error += (hist.GetBinError(i))*(hist.GetBinError(i))
  Allunderflows_error = math.sqrt(Allunderflows_error)

  Alloverflows = hist.Integral(bin_last+1, n_bin_origin+1)
  Alloverflows_error = hist.GetBinError(n_bin_origin+1)
  Alloverflows_error = Alloverflows_error*Alloverflows_error
  for i in range(bin_last+1, n_bin_origin +1):
    Alloverflows_error += (hist.GetBinError(i))*(hist.GetBinError(i))
  Alloverflows_error = math.sqrt(Alloverflows_error)

  #### Make X-bin array
  temp_xbins = [] ## size = n_bin_inrange+1
  counter=0
  for i in range(bin_first,bin_last +1):
    temp_xbins.append( hist.GetXaxis().GetBinLowEdge(i) )
    counter += 1
  temp_xbins.append( hist.GetXaxis().GetBinUpEdge(bin_last) )

  hist_out = ROOT.TH1D(hist.GetName()+'tmp', hist.GetTitle(), n_bin_inrange, array("d",temp_xbins))
  for i in range(1,n_bin_inrange+1):
    this_content = hist.GetBinContent(bin_first-1+i)
    this_error = hist.GetBinError(bin_first-1+i)
    #print '[%d,%d] : %f'%(hist_out.GetXaxis().GetBinLowEdge(i),hist_out.GetXaxis().GetBinUpEdge(i),this_content)

    #### underflows
    if i==1:
      this_content += Allunderflows
      this_error = math.sqrt( this_error*this_error + Allunderflows_error*Allunderflows_error )

    #### overflows
    if i==n_bin_inrange:
      this_content += Alloverflows
      this_error = math.sqrt( this_error*this_error + Alloverflows_error*Alloverflows_error )

    hist_out.SetBinContent(i, this_content)
    hist_out.SetBinError(i, this_error)

  hist_out.SetName(hist.GetName())
  return hist_out

def RebinNMass(hist, region, DataYear):

  lastbin = hist.GetXaxis().GetNbins()
  vec_bins = [100, 200, 300, 400, 500, 750, 1000, 2000,  8000]
  if "SR2" in region:
    vec_bins = [800, 1000, 1200, 1500, 1800, 8000]

  if ('LowWR' in region):
    tmp_vec_bins = [0, 50, 75]

def RebinWRMass(hist, region, DataYear):

  lastbin = hist.GetXaxis().GetNbins()
  vec_bins = [800, 1000, 1200, 1400, 1600, 2000, 2400, 2800, 3200, 8000]
  if "Boosted" in region:
    vec_bins = [800, 1000, 1200, 1500, 1800, 8000]

  #if ('LowWR' in region) or ('DYCR' in region):
  if ('LowWR' in region):
    tmp_vec_bins = [0, 200, 300, 400, 500, 600, 700, 800]

    for b in vec_bins:
      tmp_vec_bins.append(b)
    vec_bins = tmp_vec_bins

  n_bin = len(vec_bins)-1
  hist = hist.Rebin(n_bin, hist.GetName(), array("d", vec_bins) )
  return hist

  return ChangeGeVToTeVXaxis(hist)
  #return hist

def ChangeGeVToTeVXaxis(h):

  x_New = []
  x_New.append( h.GetXaxis().GetBinLowEdge(1)/1000. )
  for ix in range(0, h.GetXaxis().GetNbins()):
    iBin = ix+1
    x_New.append( h.GetXaxis().GetBinUpEdge(iBin)/1000. )
  h_New = ROOT.TH1D(h.GetName(), '', len(x_New)-1, array("d", x_New))

  targetBinWidth = 0.2#TeV

  for ix in range(0, h.GetXaxis().GetNbins()):
    iBin = ix+1

    new_BinContent = h.GetBinContent(iBin)
    new_BinError = h.GetBinError(iBin)

    #this_BinWidth = h_New.GetXaxis().GetBinUpEdge(iBin) - h_New.GetXaxis().GetBinLowEdge(iBin)
    #new_BinContent = new_BinContent/this_BinWidth * targetBinWidth
    #new_BinError = new_BinError/this_BinWidth * targetBinWidth

    h_New.SetBinContent(iBin, new_BinContent)
    h_New.SetBinError(iBin, new_BinError)

  return h_New


def RebinJetPt(hist, region, DataYear):

  lastbin = hist.GetXaxis().GetNbins()
  vec_bins = [0, 40, 100, 200, 400, 600, 800, 1000, 1500, 2000]
  if "Boosted" in region:
    vec_bins = [0, 200, 400, 600, 800, 1000, 2000]

  n_bin = len(vec_bins)-1
  hist = hist.Rebin(n_bin, hist.GetName(), array("d", vec_bins) )
  return hist

def GetMaximum(a, ErrorScale=1.):

  NX = a.GetN()

  maxval = -9999.
  for i in range(0,NX):

    x = ctypes.c_double(0.)
    y = ctypes.c_double(0.)
    #x = ROOT.Double(0.)
    #y = ROOT.Double(0.)

    a.GetPoint(i, x, y)
    yerr_low  = a.GetErrorYlow(i)
    yerr_high = a.GetErrorYhigh(i)

    if (y.value+ErrorScale*yerr_high > maxval):
      maxval = y.value+ErrorScale*yerr_high


  return maxval

def GetAsymmError(MC_stacked_allerr_Up, MC_stacked_allerr_Down):

    NBin = MC_stacked_allerr_Up.GetXaxis().GetNbins()
    x = []
    x_lerr = []
    x_rerr = []
    y = []
    y_lerr = []
    y_rerr = []
    
    for i in range(0,NBin):
        x.append( MC_stacked_allerr_Up.GetXaxis().GetBinCenter(i+1) )
        x_lerr.append( x[i] - MC_stacked_allerr_Up.GetXaxis().GetBinLowEdge(i+1) )
        x_rerr.append( MC_stacked_allerr_Up.GetXaxis().GetBinUpEdge(i+1) - x[i] )
        
        y.append( (MC_stacked_allerr_Up.GetBinContent(i+1) +MC_stacked_allerr_Down.GetBinContent(i+1))/2 )
        y_lerr.append((MC_stacked_allerr_Up.GetBinContent(i+1) - MC_stacked_allerr_Down.GetBinContent(i+1))/2 )
        y_rerr.append((MC_stacked_allerr_Up.GetBinContent(i+1) - MC_stacked_allerr_Down.GetBinContent(i+1))/2 )
      
    out = ROOT.TGraphAsymmErrors(NBin, array("d", x), array("d", y),  array("d", x_lerr), array("d", x_rerr), array("d", y_lerr), array("d", y_rerr))
    return out

def GetDYNormSF(DataYear, channel):

  DYNorm = 1.
  DYNorm_err = 0.

  if "_EMu_" in channel:
    return 1., 0.

  int_channel = -1 ## 0 : ee, 1 : mm
  if "Electron" in channel:
    int_channel = 0
  elif "Muon" in channel:
    int_channel = 1
  else:
    print(('Wrong channel name '+channel))
    exit()

  int_region = -1 ## 0 : SR1, 1 : SR2
  if "SR1" in channel:
    int_region = 0;
  elif "SR2" in channel:
    int_region = 1;
  elif "SR3" in channel:
    int_region = 2;
    
  else:
    print(('Wrong region name '+channel))
    exit()

  #print '%s -> int_channel = %d, int_region = %d'%(channel,int_channel,int_region)

  if DataYear==2016:
    if int_channel==0:
      if int_region==0:
        DYNorm = 0.985967
        DYNorm_err = 0.0472367
      elif int_region==1:
        DYNorm = 1.18924
        DYNorm_err = 0.0593071
      else:
        print ("Wrong DY Norm")
        exit()
    elif int_channel==1:
      if int_region==0:
        DYNorm = 0.971114
        DYNorm_err = 0.0382234
      elif int_region==1:
        DYNorm = 1.00351
        DYNorm_err = 0.0507593
      else:
        print ("Wrong DY Norm")
        exit()
    else:
      print ("Wrong DY Norm")
      exit()
  elif DataYear==2017:
    if int_channel==0:
      if int_region==0:
        DYNorm = 1.03281
        DYNorm_err = 0.0502737
      elif int_region==1:
        DYNorm = 1.30754
        DYNorm_err = 0.0653227
      else:
        print ("Wrong DY Norm")
        exit()
    elif int_channel==1:
      if int_region==0:
        DYNorm = 1.03805
        DYNorm_err = 0.0405121
      elif int_region==1:
        DYNorm = 1.19954
        DYNorm_err = 0.0592457
      else:
        print ("Wrong DY Norm")
        exit()
    else:
      print ("Wrong DY Norm")
      exit()
  elif DataYear==2018:
    if int_channel==0:
      if int_region==0:
        DYNorm = 1.00002
        DYNorm_err = 0.05551
      elif int_region==1:
        DYNorm = 1.30778
        DYNorm_err = 0.0581163
      else:
        print ("Wrong DY Norm")
        exit()
    elif int_channel==1:
      if int_region==0:
        DYNorm = 1.02359
        DYNorm_err = 0.0481667
      elif int_region==1:
        DYNorm = 1.16497
        DYNorm_err = 0.0496011
      else:
        print ("Wrong DY Norm")
        exit()
    else:
      print ("Wrong DY Norm")
      exit()

  return DYNorm, DYNorm_err

  #if 'DYCR' in channel:
  #  return DYNorm, DYNorm_err
  #  return DYNorm, 0.30*DYNorm
  #else:
  #  return DYNorm, 0.30*DYNorm

def GetSignalXsec(filepath,  mN):

  lines = open(filepath).readlines()
  for line in lines:
    words = line.split()

    if len(words) != 2:
      continue
    if ( int(words[0])==int(mN) ):
      return float(words[1])
  print(('Xsec not found for mN=%d'%(mN)))



def GetMCUncertainty(SampleName):
  
  if "DY" in SampleName:
    return 0.08
  elif "WZ" in SampleName:
    return 0.1
  elif "ZZ" in SampleName:
    return 0.1
  elif "Top" in SampleName:
    return 0.1
  else:
    return 0.3

      
def GetNormSF(DataYear, sample):

  WORKING_DIR = os.environ["HNDILEPTONWORKSPACE_DIR"]
  CorrFile = WORKING_DIR + '/data/'+os.getenv("SKFlatV")+'/'+str(DataYear)+'/MCSF.txt'

  lines = open(CorrFile).readlines()
  MCSF=1.
  for line in lines:
    words = line.strip('\n').split()
    if words[0] == sample:
      MCSF = words[1]


  return float(MCSF)

def GetKFactor(mN, Era='2016preVFP', lepch=0):

  ##==== lepch : 0=ee, 1=mm, 2=em

  WORKING_DIR = os.environ["PLOTTER_WORKING_DIR"]
  dataset =  os.environ["SKANVERSION"]
  ENV_PLOT_PATH =  os.environ["PLOT_PATH"]

  massstring = "mN"+str(mN)

  ##==== get averaged kfactor
  lines = open(WORKING_DIR+"/data/"+dataset+"/"+str(Year)+"/AveragedKFactor.txt").readlines()
  this_avg_kfactor = -1.;
  for line in lines:
    words = line.strip('\n').split()
    this_mass = words[0]
    avg_k_ee = float(words[1])
    avg_k_mm = float(words[2])

    if massstring==this_mass:
      if lepch==0:
        this_avg_kfactor = avg_k_ee;
      elif lepch==1:
        this_avg_kfactor = avg_k_mm;
      else:
        print(("[mylib.py][GetKFactor] Wrong lepch = "+str(lepch)))
      break

  if this_avg_kfactor<0:
    print(("[mylib.h][GetKFactor] this_avg_kfactor = "+str(this_avg_kfactor)))

  return this_avg_kfactor



def GetColorByInt(x):

    import ROOT
    vCol = []
    vCol.append(ROOT.kRed)
    vCol.append(800)
    vCol.append(870)
    vCol.append(ROOT.kSpring-1)
    vCol.append(ROOT.kGray)
    vCol.append(ROOT.kViolet)
    vCol.append(ROOT.kYellow+4)
    vCol.append(ROOT.kCyan)
    vCol.append(ROOT.kBlue-2)
    vCol.append(ROOT.kGreen-2)
    vCol.append(ROOT.kGreen+2)
    vCol.append(ROOT.kOrange-2)

    if(x > len(vCol)):
        return ROOT.kBlack
    return vCol[x]



def GetColor(bkg):

    if bkg == "CF":
        return 74

    if bkg == "SSPrompt":
        return 8

    if bkg == "VV":
        return ROOT.kSpring-1

    if bkg == "WZ":
        return ROOT.kSpring-1

    if bkg == "ZZ":
        return ROOT.kBlue-1

    if "VVV" in bkg:
        return ROOT.kSpring+10

    if "XG" in bkg:
        return ROOT.kSpring-7

    if "ZG" in bkg:
        return ROOT.kSpring-7

    if "WG" in bkg:
        return ROOT.kGreen-7

    if "WJet"in bkg:
        return ROOT.kGreen

    if "Top" in bkg:
        return ROOT.kRed

    if "DY" in bkg:
        return ROOT.kYellow

    if "wwpp" in bkg:
        return ROOT.kGray

    if "fake" in bkg:
        return 870

    if "NonPrompt" in bkg:
        return 870

    if bkg == "total_background":
        return ROOT.kGreen+1

    if bkg == "TTLL":
        return ROOT.kRed

    if bkg == "TW":
        return ROOT.kGray

    print(("Failed to  find " + bkg))

