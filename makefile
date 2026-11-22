CXX = g++
CXXFLAGS = -O2 -Wall -std=c++17

ROOT = $(shell root-config --cflags --libs)
PYTHIA = $(shell pythia8-config --cxxflags --libs)

all: generate analyze

generate: generate.cc
	$(CXX) $(CXXFLAGS) $< -o $@ $(PYTHIA) $(ROOT)

analyze: analyze.cc
	$(CXX) $(CXXFLAGS) $< -o $@ $(ROOT)

run: all
	./generate 100000 12345 events.root
	./analyze events.root analysis.root
	root -l -b -q 'draw_plots.C("analysis.root", "plots")'

clean:
	rm -f generate analyze

.PHONY: all run clean