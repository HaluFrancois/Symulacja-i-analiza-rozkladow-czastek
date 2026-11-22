#include "Pythia8/Pythia.h"
#include "TFile.h"
#include "TTree.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
  long long nEvents = (argc > 1) ? std::atoll(argv[1]) : 100000;
  int seed = (argc > 2) ? std::atoi(argv[2]) : 12345;
  std::string outName = (argc > 3) ? argv[3] : "events.root";

  if (nEvents <= 0 || seed <= 0) {
    std::cerr << "Podaj liczbe zdarzen > 0 i seed > 0\n";
    return 1;
  }

  Pythia8::Pythia pythia;
  pythia.readString("Beams:idA = 2212");
  pythia.readString("Beams:idB = 2212");
  pythia.readString("Beams:eCM = 13600.");
  pythia.readString("SoftQCD:all = on");
  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = " + std::to_string(seed));
  pythia.readString("Next:numberShowEvent = 0");

  if (!pythia.init()) {
    std::cerr << "Inicjalizacja Pythii nie powiodla sie\n";
    return 1;
  }

  TFile fout(outName.c_str(), "RECREATE");
  if (fout.IsZombie()) {
    std::cerr << "Nie mozna utworzyc pliku " << outName << "\n";
    return 1;
  }

  TTree tree("events", "pp 13.6 TeV, SoftQCD, naladowane czastki stanu koncowego");
  std::vector<int> pdg;
  std::vector<float> pt, eta, phi;
  tree.Branch("pdg", &pdg);
  tree.Branch("pt", &pt);
  tree.Branch("eta", &eta);
  tree.Branch("phi", &phi);

  long long ev = 0;
  while (ev < nEvents) {
    if (!pythia.next()) continue;

    pdg.clear();
    pt.clear();
    eta.clear();
    phi.clear();

    for (int i = 0; i < pythia.event.size(); ++i) {
      const Pythia8::Particle& p = pythia.event[i];
      if (!p.isFinal() || !p.isCharged()) continue;
      pdg.push_back(p.id());
      pt.push_back(p.pT());
      eta.push_back(p.eta());
      phi.push_back(p.phi());
    }

    tree.Fill();
    ++ev;
  }

  pythia.stat();
  tree.Write();
  fout.Close();

  std::cout << "Wygenerowano " << nEvents << " zdarzen -> " << outName << "\n";
  return 0;
}