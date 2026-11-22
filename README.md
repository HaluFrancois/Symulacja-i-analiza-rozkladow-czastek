# Rozkłady cząstek w zderzeniach pp przy 13.6 TeV (Pythia 8 + ROOT)

Generacja zdarzeń minimum bias (SoftQCD) w Pythii 8, analiza cząstek naładowanych stanu końcowego w ROOT i wykresy rozkładów N, pT, η, φ.

## Pliki

- `generate.cc` - generuje zdarzenia i zapisuje cząstki naładowane stanu końcowego do `events.root`
- `analyze.cc` - wypełnia histogramy i zapisuje je do `analysis.root`
- `draw_plots.C` - makro ROOT rysujące wykresy do katalogu `plots/` (PNG i PDF)
- `analysis.root` - histogramy dla 100 000 zdarzeń
- `plots/` - wykresy w formatach PNG i PDF
- `presentation.pdf` - prezentacja z wykresami
- `makefile` - skrypt pozwalający uruchomić wszystkie pliki jedną komendą

## Wymagania

- C++17
- ROOT 6 (wersja 6.40.04)
- Pythia 8 (wersja 8.318)

## Uruchomienie i instrukcja pliku makefile
Upewnienie się że root-config i pythia8-config są w PATH.
Za pomocą skryptu: `make run`. 
Krok po kroku:
```bash
make
./generate 100000 12345 events.root 
./analyze events.root analysis.root
root -l -b -q 'draw_plots.C("analysis.root", "plots")'
```

`make clean` usuwa skompilowane programy.


## Szczegóły analizy
- Ziarno generatora jest ustawiane jawnie (`Random:seed`, domyślnie 12345). 
- Analizowane są cząstki z `isFinal()` i `isCharged()`.
- Kategorie: `all` i `ch` (wszystkie naładowane), `pip`, `pim` (|PDG| = 211), `Kp`, `Km` (321), `p`, `pbar` (2212).
- Histogramy: `hN_*` (0 - 400, 400 binów), `hPt_*` (0 - 10 GeV, 100), `hEta_*` (−5 - 5, 100), `hPhi_*` (0 - π, 64), oraz `hNevents` z liczbą zdarzeń.
- Pythia podaje φ w przedziale (−π, π], histogram stworzono dla |φ|.
- W makrze rozkłady są normalizowane na zdarzenie; pT, η i φ dodatkowo na szerokość binu. Wykres N pokazuje P(N).
- Makro robi 8 wykresów: dla każdej zmiennej jeden z wszystkimi naładowanymi (`*_ch`) i jeden z π±, K±, p/p̄ (`*_species`).