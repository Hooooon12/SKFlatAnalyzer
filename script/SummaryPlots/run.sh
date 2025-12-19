# from /data9/Users/HNL_public/SUS-24-014/User_Codes/AN_Plotter_Tables/SummaryPlots/SR1/combined_srcr_plot_sr1_syst.py
python combined_srcr_plot_sr1_syst.py --base /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr

# from /data9/Users/HNL_public/SUS-24-014/User_Codes/AN_Plotter_Tables/SummaryPlots/SR2/combined_srcr_print_sr2_syst.py
python combined_srcr_print_sr2_syst.py  --base /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv5_BDTV3_SR1_FixRepeatBin_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr --flavours MuMu  --eras 2016preVFP,2016postVFP,2017,2018   --file-pattern M1000_{flavour}_card_input.root  --file-pattern-sig2 Weinberg_{flavour}_card_input.root

# from /data9/Users/HNL_public/SUS-24-014/User_Codes/AN_Plotter_Tables/SummaryPlots/SR3/check_syst.py
python check_syst.py --base /data9/Users/HNL_public/SUS-24-014/LimitInputs/ANv6_NewSignals_HNL_ULIDv2_V3_Strict_15_Bin_RunSyst_Decorr_JetDecorr
