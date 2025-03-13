#include <TFile.h>
#include <TDirectory.h>
#include <TH1D.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TSystemDirectory.h>
#include <TSystemFile.h>
#include <TSystem.h>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>

void ProcessHistogram(TFile *root_file, const std::string& filename, const std::string& dir_path, const std::string& hist_path) {
    TDirectory *root_dir = root_file->GetDirectory(dir_path.c_str());
    if (!root_dir) {
        std::cerr << "Directory not found: " << dir_path << std::endl;
        return;
    }
    std::string file_dir = filename;
    file_dir.erase(file_dir.end()-5,file_dir.end());

    if (gSystem->AccessPathName((file_dir + "/" + dir_path).c_str()) != 0) {
        gSystem->Exec(("mkdir -p ./"+file_dir + "/" + dir_path).c_str());
    }
    TH1D *nominal_hist = dynamic_cast<TH1D*>(root_dir->Get(hist_path.c_str()));
    if (!nominal_hist) {
        std::cerr << "Histogram not found: " << hist_path << std::endl;
        return;
    }

    std::vector<TH1D*> pdf_hists;
    for (int n = 0; n < 100; ++n) {
        std::string pdf_hist_name = hist_path + "_Syst_PDF" + std::to_string(n);
        TH1D *pdf_hist = dynamic_cast<TH1D*>(root_dir->Get(pdf_hist_name.c_str()));
        if (pdf_hist) {
            pdf_hists.push_back(pdf_hist);
        } else {
            std::cerr << "PDF histogram not found: " << pdf_hist_name << std::endl;
        }
    }

    // Prepare canvas and legend
    TCanvas *canvas = new TCanvas("canvas", "PDF Uncertainty", 800, 600);
    gStyle->SetOptStat(0);
//    canvas->Divide(1, 2);
//    canvas->cd(1);
    nominal_hist->SetLineColor(kBlack);
    nominal_hist->SetLineWidth(2);
//    nominal_hist->Draw("HIST");
    TH1D* ratio_nominal_hist = (TH1D*)nominal_hist->Clone("ratio_nominal");
    ratio_nominal_hist->Divide(nominal_hist);
    TH1D* ratio_pdf_hist;
//    canvas->cd(2);
    // Calculate uncertainty and plot as blue band
    int n_bins = nominal_hist->GetNbinsX();
    TH1D *upper_band = (TH1D*)nominal_hist->Clone("upper_band");
    TH1D *lower_band = (TH1D*)nominal_hist->Clone("lower_band");
    for (int i = 1; i <= n_bins; ++i) {
        double s0 = nominal_hist->GetBinContent(i);
        double uncertainty = 0;

        if (filename.find("VBF") != std::string::npos) {
            std::vector<double> pdf_bin_values;
            double avg = 0;

            for (auto pdf_hist : pdf_hists) {
                double bin_val = pdf_hist->GetBinContent(i);
                pdf_bin_values.push_back(bin_val);
                avg += bin_val;
            }
            avg /= pdf_bin_values.size();

            for (const auto& s_k : pdf_bin_values) {
                uncertainty += (s_k - avg) * (s_k - avg);
            }
            uncertainty = std::sqrt(uncertainty / 99);
        } else {
            for (auto pdf_hist : pdf_hists) {
                double s_k = pdf_hist->GetBinContent(i);
                uncertainty += (s_k - s0) * (s_k - s0);
            }
            uncertainty = std::sqrt(uncertainty);
        }

        upper_band->SetBinContent(i, s0 + uncertainty);
        lower_band->SetBinContent(i, s0 - uncertainty);
    }
    upper_band->SetLineColor(kBlue);
    lower_band->SetLineColor(kBlue);
    upper_band->SetLineWidth(2);
    lower_band->SetLineWidth(2);
    TH1D *up_ratio_hist = (TH1D*)upper_band->Clone("upper_ratio_hist");
    TH1D *low_ratio_hist = (TH1D*)lower_band->Clone("lower_ratio_hist");
    low_ratio_hist->Divide(nominal_hist);
    up_ratio_hist->Divide(nominal_hist);


    ratio_nominal_hist->GetYaxis()->SetRangeUser(0.97, 1.03);
    ratio_nominal_hist->Draw("HIST");
    // Plot each PDF histogram
    for (auto pdf_hist : pdf_hists) {
//        canvas->cd(1);
        pdf_hist->SetLineColor(kCyan);
        pdf_hist->SetLineWidth(1);
//        pdf_hist->Draw("HIST SAME");
        ratio_pdf_hist = (TH1D*)pdf_hist->Clone("ratio_PDF");
        ratio_pdf_hist->Divide(nominal_hist);
//        canvas->cd(2);
        ratio_pdf_hist->Draw("HIST SAME");
    }
//    nominal_hist->Draw("HIST SAME");
    ratio_nominal_hist->Draw("HIST SAME");




//    upper_band->Draw("HIST SAME");
//    lower_band->Draw("HIST SAME");



    low_ratio_hist->SetTitle("Low Ratio to Nominal");
    low_ratio_hist->Draw("SAME HIST");
    up_ratio_hist->SetTitle("Up Ratio to Nominal");
    up_ratio_hist->Draw("SAME HIST");



    // Save the canvas
    std::cout << file_dir << endl;
    std::string save_name = file_dir + "/" + dir_path + hist_path + ".png";
    canvas->SaveAs(save_name.c_str());

    delete canvas;
    delete upper_band;
    delete lower_band;
}


void PDF_syst() {
    std::string directory = "./FullRun2/";
    std::cout << "Starting analysis on directory: " << directory << std::endl;

    TSystemDirectory dir(directory.c_str(), directory.c_str());
    TList *files = dir.GetListOfFiles();
    if (!files) {
        std::cerr << "Error: Could not open directory " << directory << std::endl;
        return;
    }

    TSystemFile *file;
    TIter next(files);
    while ((file = (TSystemFile*)next())) {
        std::string filename = file->GetName();
        if (filename.find(".root") == std::string::npos) continue;  // Process only .root files
//        if (filename.find("100_") >= filename.size() and filename.find("400_") >= filename.size() and filename.find("500_") >= filename.size() and filename.find("1000_") >= filename.size() and filename.find("2000_") >= filename.size() and filename.find("10000_") >= filename.size()) continue;  // Process only .root files
        std::cout << filename << endl;
        // Open the ROOT file
        TFile *root_file = TFile::Open((directory + "/" + filename).c_str());
        if (!root_file || root_file->IsZombie()) {
            std::cerr << "Cannot open file: " << filename << std::endl;
            continue;
        }

        // Define types and mass values
        std::vector<std::string> types = {"EE", "EMu", "MuMu"};
        std::vector<std::string> hist_names_ee = {"ElectronSR1", "ElectronSR2", "ElectronSR3", "ElectronSR1_SingleBin", "ElectronSR2_SingleBin", "ElectronSR3_SingleBin"};
        std::vector<std::string> hist_names_emu = {"ElectronMuonSR1", "ElectronMuonSR2", "ElectronMuonSR3", "ElectronMuonSR1_SingleBin", "ElectronMuonSR2_SingleBin", "ElectronMuonSR3_SingleBin"};
        std::vector<std::string> hist_names_mumu = {"MuonSR1", "MuonSR2", "MuonSR3", "MuonSR1_SingleBin", "MuonSR2_SingleBin", "MuonSR3_SingleBin"};
        std::vector<std::string> hist_names_BDT_ee = {"ElectronSRBDT", "ElectronSR3BDT", "ElectronSRBDT_SingleBin", "ElectronSR3BDT_SingleBin"};
        std::vector<std::string> hist_names_BDT_emu = {"ElectronMuonSRBDT", "ElectronMuonSR3BDT", "ElectronMuonSRBDT_SingleBin", "ElectronMuonSR3BDT_SingleBin"};
        std::vector<std::string> hist_names_BDT_mumu = {"MuonSRBDT", "MuonSR3BDT", "MuonSRBDT_SingleBin", "MuonSR3BDT_SingleBin"};
        std::vector<std::string> masses = {"M100", "M125", "M150", "M200", "M250", "M300", "M400", "M500", "M85", "M90", "M95"};

        // Loop over directories in file
        for (auto dir_name : {"LimitExtraction", "LimitExtractionBDT"}) {
            // Loop over types
            for (const auto& type : types) {
                std::vector<std::string> hist_names;

                // If in "LimitExtractionBDT", add mass loop
                if (std::string(dir_name) == "LimitExtractionBDT") {
                    if (type == "EE") hist_names = hist_names_BDT_ee;
                    else if (type == "EMu") hist_names = hist_names_BDT_emu;
                    else if (type == "MuMu") hist_names = hist_names_BDT_mumu;
                    for (const auto& mass : masses) {
                        for (const auto& hist_name : hist_names) {
                            std::string hist_path = std::string(dir_name) + "/HNL_ULID/" + type + "/" + mass + "/LimitBins/";
                            ProcessHistogram(root_file,filename,hist_path, hist_name);
                        }
                    }
                } else { // Otherwise just run for each hist_name in "LimitExtraction"
                    if (type == "EE") hist_names = hist_names_ee;
                    else if (type == "EMu") hist_names = hist_names_emu;
                    else if (type == "MuMu") hist_names = hist_names_mumu;
                    for (const auto& hist_name : hist_names) {
                        std::string hist_path = std::string(dir_name) + "/HNL_ULID/" + type + "/LimitBins/";
                        ProcessHistogram(root_file,filename,hist_path, hist_name);
                    }
                }
            }
        }
        root_file->Close();
    }
}

