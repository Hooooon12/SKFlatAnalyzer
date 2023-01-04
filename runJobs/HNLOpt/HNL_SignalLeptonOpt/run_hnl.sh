### SETUP RUN OPTIONS
analyzer=HNL_SignalLeptonOpt
rundir=HNLOpt/HNL_SignalLeptonOpt

# PATHS                                                                                                                                                    
runPATH=${SKFlat_WD}/runJobs/${rundir}/
sigpath=${SKFlat_WD}/runJobs/SampleLists/Signals/
mcpath=${SKFlat_WD}/runJobs/SampleLists/Bkg/
datapath=${SKFlat_WD}/runJobs/SampleLists/DATA/

#SKIM                                                                                                                                                     
skim=' '

# JOB CONFIG                                                                                                                                               
njobs=5
nLargejobs=15
njobs_sig=5
njobs_data=5

nmax=300

if [[ $1 == "NP_ELECTRON_CF_HighPt" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='RunEE,ELID_CF,HighPt,BB'
	source ${runPATH}/run_hnl.sh Electron ${Flag} ${i}

        Flag2='RunEE,ELID_CF,HighPt,EC'
	source ${runPATH}/run_hnl.sh Electron ${Flag2} ${i}

	Flag3='RunEE,ELID_CF,HighPt,FullEta'
        source ${runPATH}/run_hnl.sh Electron ${Flag3} ${i}

    done
fi

if [[ $1 == "NP_ELECTRON_CF_FullPt" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='RunEE,ELID_CF,FullPt'
        source ${runPATH}/run_hnl.sh Electron ${Flag} ${i}

    done
fi

if [[ $1 == "NP_ELECTRON" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='RunEE,ELID_NP'
        source ${runPATH}/run_hnl.sh Electron ${Flag} ${i}

    done
fi


#############################################################################                                                                                                                                                               
#############################################################################                                                                                                                                                               
#############################################################################     

if [[ $1 == "NP_MUON_HighPt" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do    
	
	Flag='MuID_NP,HighPt,BB'
	source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}
	
	Flag2='MuID_NP,HighPt,EC'
	source ${runPATH}/run_hnl.sh Muon ${Flag2} ${i}
	
    done
fi


if [[ $1 == "NP_MUON_CONV" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='Mu_Conv'
        source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}

    done
fi



if [[ $1 == "NP_MUON_FullEta_HighPt" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='MuID_NP,HighPt,FullEta'
        source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}

    done
fi

if [[ $1 == "NP_MUON_Full_HighPt" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='MuID_NP,HighPt,Full'
        source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}

    done
fi


if [[ $1 == "NP_MUON_FullPt" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

	Flag='MuID_NP,FullPt,BB'
	source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}

	
	Flag2='MuID_NP,FullPt,EC'
	source ${runPATH}/run_hnl.sh Muon ${Flag2} ${i}

    done

fi


if [[ $1 == "NP_MUON_FullEtaSlope" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='MuID_NP,FullPt,FullEtaSlope'
        source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}


    done

fi

if [[ $1 == "NP_MUON_FullEtaExp" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='MuID_NP,FullPt,FullEtaExp'
        source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}


    done

fi

if [[ $1 == "Mu_Final" ]]; then

    declare  -a era_list=("2016postVFP" "2016preVFP" "2017" "2018")

    for i in "${era_list[@]}"
    do

        Flag='Mu_Final'
        source ${runPATH}/run_hnl.sh Muon ${Flag} ${i}


    done

fi





#############################################################################
#############################################################################
#############################################################################




if [[ $1 == "Muon" ]]; then
    
    
    source ${runPATH}/run_hnl.sh MC  ${3} ${2}  
    
    source ${runPATH}/run_hnl.sh Signals ${3} ${2} 
    
    ConvFlag=${2}',RunConv'

    source ${runPATH}/run_hnl.sh Conv ${3} ${ConvFlag} 
    

fi

if [[ $1 == "Electron" ]]; then

    # If no inut then this is ran                                                                                                                        
    source ${runPATH}/run_hnl.sh Signals ${3} ${2} 
    source ${runPATH}/run_hnl.sh MC  ${3} ${2}
    
    CFFlag=${2}',RunCF'
    source ${runPATH}/run_hnl.sh CF ${3} ${CFFlag}
    ConvFlag=${2}',RunConv'
    source ${runPATH}/run_hnl.sh Conv ${3} ${ConvFlag}
    

fi



if [[ $1 == "MC" ]]; then

    SKFlat.py -a $analyzer  -l $mcpath/MCOpt.txt  -n $njobs  --nmax ${nmax}   -e ${2}  --userflags ${3} --skim SkimTree_HNMultiLep &
    SKFlat.py -a $analyzer  -l $mcpath/MCOptLarge.txt  -n $nLargejobs  --nmax ${nmax}   -e ${2}  --userflags ${3} --skim SkimTree_HNMultiLep &

fi

if [[ $1 == "Signals" ]]; then

    SKFlat.py -a $analyzer  -l $sigpath/SSWWOpt.txt  -n $njobs_sig  --nmax ${nmax}  -e ${2}  --userflags ${3}  &
    SKFlat.py -a $analyzer  -l $sigpath/DYOpt.txt    -n $njobs_sig  --nmax ${nmax}   -e ${2}  --userflags ${3} &
    SKFlat.py -a $analyzer  -l $sigpath/VBFOpt.txt   -n $njobs_sig  --nmax ${nmax}   -e ${2}  --userflags ${3} &

fi


if [[ $1 == "CF" ]]; then

    SKFlat.py -a $analyzer  -l $mcpath/CF.txt  -n $njobs  --nmax ${nmax}   -e ${2} --skim SkimTree_HNMultiLep  --userflags ${3} &
    #SKFlat.py -a $analyzer  -l $datapath/DATA_${i}.txt  -n ${njobs_data}  --nmax ${nmax}   -e ${i} --userflags RunCF  &                                 

fi


if [[ $1 == "Conv" ]]; then

    SKFlat.py -a $analyzer  -l $mcpath/Conv.txt  -n 5  --nmax ${nmax}   -e ${2} --skim SkimTree_HNMultiLep  --userflags ${3} &
fi



if [[ $1 == "TMP" ]]; then
    
    Flag='MuID_NP,FullPt,FullEtaExp'
    
    SKFlat.py -a $analyzer  -i DYTypeI_DF_M2000_private  -n 2  --nmax ${nmax}  -e 2017  --userflags $Flag  &

fi
