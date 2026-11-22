#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TROOT.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include <iostream>

struct Zmienna {
  const char* prefix;
  const char* plik;
  const char* tytul;
  const char* osX;
  const char* osY;
  bool logY;
  bool naSzerokosc;  
  double xmax;      
};

const char* tagi[7] = {"ch", "pip", "pim", "Kp", "Km", "p", "pbar"};
const char* etykiety[7] = {"all charged", "#pi^{+}", "#pi^{-}", "K^{+}", "K^{-}", "p", "#bar{p}"};
const int kolory[7] = {kBlack, kBlue + 1, kBlue + 1, kRed + 1, kRed + 1, kGreen + 2, kGreen + 2};
const int markery[7] = {20, 20, 24, 21, 25, 22, 26};

void rysuj(TFile* f, const Zmienna& z, bool gatunki, double nEv, const char* outDir) {
  int od = gatunki ? 1 : 0;
  int doo = gatunki ? 7 : 1;

  TH1D* h[7] = {nullptr};
  double maks = 0.;
  for (int i = od; i < doo; ++i) {
    TH1D* orig = f->Get<TH1D>(Form("%s_%s", z.prefix, tagi[i]));
    if (!orig) {
      std::cerr << "Brak histogramu " << z.prefix << "_" << tagi[i] << "\n";
      return;
    }
    h[i] = (TH1D*)orig->Clone(Form("%s_%s_norm", z.prefix, tagi[i]));
    h[i]->SetDirectory(nullptr);
    h[i]->Scale(1.0 / nEv, z.naSzerokosc ? "width" : "");
    if (h[i]->GetMaximum() > maks) maks = h[i]->GetMaximum();
  }

  const char* sufiks = gatunki ? "species" : "ch";
  TCanvas* c = new TCanvas(Form("c_%s_%s", z.plik, sufiks), z.tytul, 900, 700);
  c->SetLeftMargin(0.14);
  if (z.logY) c->SetLogy();

  TLegend* leg = new TLegend(0.74, 0.65, 0.92, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);

  for (int i = od; i < doo; ++i) {
    h[i]->SetLineColor(kolory[i]);
    h[i]->SetMarkerColor(kolory[i]);
    h[i]->SetMarkerStyle(markery[i]);
    h[i]->SetMarkerSize(0.7);
    if (z.xmax > 0) h[i]->GetXaxis()->SetRangeUser(0., z.xmax);

    if (i == od) {
      TString tytul = z.tytul;
      tytul += gatunki ? ": #pi^{#pm}, K^{#pm}, p/#bar{p}" : ": all charged";
      h[i]->SetTitle(tytul + ";" + z.osX + ";" + z.osY);
      if (z.logY) {
        h[i]->SetMaximum(5. * maks);
      } else {
        h[i]->SetMinimum(0.);
        h[i]->SetMaximum(1.3 * maks);
      }
      h[i]->Draw("E1 P");
    } else {
      h[i]->Draw("E1 P SAME");
    }
    if (gatunki) leg->AddEntry(h[i], etykiety[i], "lp");
  }
  if (gatunki) leg->Draw();

  c->SaveAs(Form("%s/%s_%s.png", outDir, z.plik, sufiks));
  c->SaveAs(Form("%s/%s_%s.pdf", outDir, z.plik, sufiks));
}

void draw_plots(const char* inFile = "analysis.root", const char* outDir = "plots") {
  gROOT->SetBatch(kTRUE);
  gStyle->SetOptStat(0);

  TFile* f = TFile::Open(inFile, "READ");
  if (!f || f->IsZombie()) {
    std::cerr << "Nie mozna otworzyc pliku " << inFile << "\n";
    return;
  }
  TH1D* hNev = f->Get<TH1D>("hNevents");
  if (!hNev || hNev->GetBinContent(1) <= 0) {
    std::cerr << "Brak hNevents w pliku " << inFile << "\n";
    return;
  }
  double nEv = hNev->GetBinContent(1);

  gSystem->mkdir(outDir, kTRUE);

  Zmienna zmienne[4] = {
    {"hN",   "N",   "Multiplicity",       "Multiplicity N", "P(N)", true, false, 250.},
    {"hPt",  "pT",  "Transverse momentum", "p_{T} [GeV]", "1/N_{ev} dN/dp_{T} [GeV^{-1}]", true, true, 0.},
    {"hEta", "eta", "Pseudorapidity",      "#eta", "1/N_{ev} dN/d#eta", false, true, 0.},
    {"hPhi", "phi", "Azimuthal angle",     "#phi [rad]", "1/N_{ev} dN/d#phi [rad^{-1}]", false, true, 0.},
  };

  for (const Zmienna& z : zmienne) {
    rysuj(f, z, false, nEv, outDir);
    rysuj(f, z, true, nEv, outDir);
  }

  f->Close();
  std::cout << "Wykresy zapisane w katalogu " << outDir << "\n";
}