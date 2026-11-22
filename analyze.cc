#include "TFile.h"
#include "TH1D.h"
#include "TMath.h"
#include "TTree.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

const int nCat = 8;
const char* tags[nCat] = {"all", "ch", "pip", "pim", "Kp", "Km", "p", "pbar"};

int gatunek(int pdg) {
  switch (pdg) {
    case 211:   return 2;
    case -211:  return 3;
    case 321:   return 4;
    case -321:  return 5;
    case 2212:  return 6;
    case -2212: return 7;
  }
  return -1;
}

int main(int argc, char* argv[]) {
  std::string inName = (argc > 1) ? argv[1] : "events.root";
  std::string outName = (argc > 2) ? argv[2] : "analysis.root";

  TFile fin(inName.c_str(), "READ");
  if (fin.IsZombie()) {
    std::cerr << "Nie mozna otworzyc pliku " << inName << "\n";
    return 1;
  }
  TTree* tree = fin.Get<TTree>("events");
  if (!tree) {
    std::cerr << "Brak drzewa 'events' w " << inName << "\n";
    return 1;
  }

  std::vector<int>* pdg = nullptr;
  std::vector<float>*pt = nullptr, *eta = nullptr, *phi = nullptr;
  tree->SetBranchAddress("pdg", &pdg);
  tree->SetBranchAddress("pt", &pt);
  tree->SetBranchAddress("eta", &eta);
  tree->SetBranchAddress("phi", &phi);

  TH1::AddDirectory(kFALSE);
  TH1D *hN[nCat], *hPt[nCat], *hEta[nCat], *hPhi[nCat];
  for (int c = 0; c < nCat; ++c) {
    hN[c]   = new TH1D(Form("hN_%s", tags[c]),   "Multiplicity;N;Events", 400, 0., 400.);
    hPt[c]  = new TH1D(Form("hPt_%s", tags[c]),  "pT;p_{T} [GeV];Particles", 100, 0., 10.);
    hEta[c] = new TH1D(Form("hEta_%s", tags[c]), "Eta;#eta;Particles", 100, -5., 5.);
    hPhi[c] = new TH1D(Form("hPhi_%s", tags[c]), "Phi;#phi [rad];Particles", 64, 0., TMath::Pi());
    hN[c]->Sumw2();
    hPt[c]->Sumw2();
    hEta[c]->Sumw2();
    hPhi[c]->Sumw2();
  }
  TH1D* hNevents = new TH1D("hNevents", "Number of events;;Events", 1, 0., 1.);

  Long64_t nEvents = tree->GetEntries();
  for (Long64_t ev = 0; ev < nEvents; ++ev) {
    tree->GetEntry(ev);
    hNevents->Fill(0.5);

    int n[nCat] = {0};
    for (size_t i = 0; i < pdg->size(); ++i) {
      int kat[3] = {0, 1, gatunek((*pdg)[i])};
      for (int k : kat) {
        if (k < 0) continue;
        n[k]++;
        hPt[k]->Fill((*pt)[i]);
        hEta[k]->Fill((*eta)[i]);
        hPhi[k]->Fill(std::abs((*phi)[i]));
      }
    }
    for (int c = 0; c < nCat; ++c) hN[c]->Fill(n[c]);
  }

  TFile fout(outName.c_str(), "RECREATE");
  if (fout.IsZombie()) {
    std::cerr << "Nie mozna utworzyc pliku " << outName << "\n";
    return 1;
  }
  for (int c = 0; c < nCat; ++c) {
    hN[c]->Write();
    hPt[c]->Write();
    hEta[c]->Write();
    hPhi[c]->Write();
  }
  hNevents->Write();
  fout.Close();

  std::cout << "Przeanalizowano " << nEvents << " zdarzen -> " << outName << "\n";
  return 0;
}