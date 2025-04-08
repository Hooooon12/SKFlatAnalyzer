import os
import subprocess

eras = ["2016preVFP", "2016postVFP", "2017", "2018"]
base_path = "/data6/Users/jalmond/SKFlatOutput/Run2UltraLegacy_v3/HNL_ControlRegion_Plotter"

commands_template = [
    ("CF", "MultiLepton__RunCF__/DATA/HNL_ControlRegion_Plotter_SkimTree_DileptonBDT_*"),
    ("Fake", "MultiLepton__RunFake__/DATA/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_*"),
    ("Data", "MultiLepton__/DATA/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_*"),
    ("Conv", "MultiLepton__RunConv__/HNL_ControlRegion_Plotter_SkimTree_*"),
    ("ggZZ", [
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_GluGluToZZto4e.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_GluGluToZZto2e2tau.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_GluGluToZZto2e2mu.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_GluGluToZZto4mu.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_GluGluToZZto2mu2tau.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_GluGluHToZZTo4L.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_VBF_HToZZTo4L.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_GluGluToZZto4tau.root"
    ]),
    ("qqZZ", ["MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_ZZTo4L_powheg.root"]),
    ("TTV", [
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_ttZToLLNuNu.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_ttWToLNu.root"
    ]),
    ("WW", [
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WpWp_EWK.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WpWp_QCD.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WWTo2L2Nu_DS.root"
    ]),
    ("Other", [
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_tZq.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_TTTT.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_ttHToNonbb.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_VHToNonbb.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_TTZZ.root"
    ]),
    ("VVV", [
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WZZ.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_ZZZ.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WWZ.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WWW.root"
    ]),
    ("WZ", [
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WZ_EWK.root",
        "MultiLepton__RunPrompt__/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_WZTo3LNu_mllmin4p0_powheg.root"
    ])
]


commands_template = [
    ("Data", "MultiLepton__/DATA/HNL_ControlRegion_Plotter_SkimTree_HNMultiLepBDT_*"),
]

for era in eras:
    era_path = os.path.join(base_path, era)
    os.makedirs(era, exist_ok=True)  # Ensure output directory exists

    for tag, input_files in commands_template:
        output_file = f"{era}/HNL_ControlRegion_Plotter_{tag}.root"

        # If the input is a wildcard pattern (string), expand with full path
        if isinstance(input_files, str):
            input_path = os.path.join(base_path, era, input_files)
            cmd = f"hadd {output_file} {input_path}"
        else:
            input_paths = [os.path.join(era_path, f) for f in input_files]
            cmd = f"hadd {output_file} " + " ".join(input_paths)

        print(f"Running: {cmd}")
        subprocess.run(cmd, shell=True, check=True)
