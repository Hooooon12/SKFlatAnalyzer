# Print the cutflows

#!/bin/bash

SIGNAL=$1

if [[ $SIGNAL == "SSWW" ]]
  then FILE="SSWW_SSWW_HN_1500.root"
elif [[ $SIGNAL == "DY" ]]
  then FILE="SSWW_DYTypeI_SS_MuMu_M1500.root"
elif [[ $SIGNAL == "VBF" ]]
  then FILE="SSWW_VBFTypeI_SS_MuMu_M1500.root"
fi

echo "===================SSWW, HN SR cutflow...======================"
echo ""
echo "Signal process : ${SIGNAL}TypeI"
echo ""
echo "********************SR********************"
echo -e "TH1D* h1 = (TH1D*)SR->Get(\"Number_Events_SSWWrep\");\nfor(int i=1; i<20; i++) cout << h1->GetBinContent(i)/h1->GetBinContent(1) << endl;\n.q;" | root -l ${FILE}
echo "********************SR_inv********************"
echo -e "TH1D* h1 = (TH1D*)SR_inv->Get(\"Number_Events_SSWWrep\");\nfor(int i=1; i<20; i++) cout << h1->GetBinContent(i)/h1->GetBinContent(1) << endl;\n.q;" | root -l ${FILE}
echo "********************highSR1********************"
echo -e "TH1D* h1 = (TH1D*)highSR1->Get(\"Number_Events_SSWWrep\");\nfor(int i=1; i<20; i++) cout << h1->GetBinContent(i)/h1->GetBinContent(1) << endl;\n.q;" | root -l ${FILE}
echo "********************highSR1_1jet********************"
echo -e "TH1D* h1 = (TH1D*)highSR1_1jet->Get(\"Number_Events_SSWWrep\");\nfor(int i=1; i<20; i++) cout << h1->GetBinContent(i)/h1->GetBinContent(1) << endl;\n.q;" | root -l ${FILE}
echo "********************highSR2********************"
echo -e "TH1D* h1 = (TH1D*)highSR2->Get(\"Number_Events_SSWWrep\");\nfor(int i=1; i<20; i++) cout << h1->GetBinContent(i)/h1->GetBinContent(1) << endl;\n.q;" | root -l ${FILE}
