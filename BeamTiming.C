#include "BeamTiming.h"
#include <iostream>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TF1.h>
#include <TSystem.h>
#include <TROOT.h>

BeamTiming::BeamTiming() : nBeams(0), nsigma(5.0) {}

int BeamTiming::GetNbeams() const {
    return nBeams;
}

std::vector<double> BeamTiming::GetTimings() const {
    return timings;
}

std::vector<double> BeamTiming::GetCounts() const {
	return counts;
}

void BeamTiming::SetNsigma(double nsigma2) {
	nsigma = nsigma2;
}

std::vector<double> BeamTiming::GetArisings() const {
    return arisings;
}

std::vector<double> BeamTiming::GetFallings() const {
    return fallings;
}

bool BeamTiming::isBeamTiming(double time) {
    for (int iBeam = 0; iBeam < nBeams; iBeam++) {
        if (arisings . at(iBeam) < time && time < fallings . at(iBeam)) {
            return true;
        }
    }
    return false;
}

//void BeamTiming::GetBeamTimings(TString dir, const int filetype = 0, const int threshold = 1, bool figures = false) {
void BeamTiming::GetBeamTimings(TString dir, const int filetype, double threshold, bool figures) {
	gROOT -> SetBatch(kTRUE);

	timings . clear();
	counts . clear();
	arisings . clear();
	fallings . clear();
	
	TString cut;
	if ( filetype == 0 ) { cut = "height > 2000 && width < 100"; }
	if ( filetype == 1 ) { cut = "height > 2000 && fwhm < 100"; }
	if ( filetype == 2 ) { cut = "h > 2000 && w < 100"; }
	TString rundir = dir(dir.Length() - 6, dir.Length());
	std::cout << "Now processing " << rundir << std::endl;

	TH1D *h1 = new TH1D("h1", "The number of large pulses at time t;time (TDC); counts", 1024 / 8, 0, 4096);
	TH1D *h1_diff = new TH1D("h1_diff", "h1_diff;time (TDC); counts", 1024 / 8, 0, 4096);
	h1 -> SetStats(0);
	h1_diff -> SetStats(0);

	///////////////////////////////////////////////////////////////////////
	// Accumulate large pulses in h1
	///////////////////////////////////////////////////////////////////////
	dir = dir + "/";
	TSystemDirectory directory("directory", dir);
	TList *files = directory.GetListOfFiles();
	TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
	TString badtrees;

	if (files) {
		TSystemFile *file;
		TIter next(files);

		while ( ( file = (TSystemFile *) next() ) ) {
				TString filename = file -> GetName();
				if ( file -> IsDirectory() || ! filename.EndsWith("root") ) continue;
				TFile *input = new TFile(dir + filename.Data(), "rb");

				for (int ich = 0; ich < 16; ich++) {
					TTree *tree;
					if ( filetype == 0 ) { tree = (TTree *) input -> Get(Form("ch%i/pul_ch%i", ich, ich)); }
					if ( filetype == 1 ) { tree = (TTree *) input -> Get(Form("ch%i/pulse_ch%i", ich, ich)); }
					if ( filetype == 2 ) { tree = (TTree *) input -> Get("tree"); }
					if ( !tree ) { badtrees += TString::Format("%s ", filename.Data()); break; }

					TH1D *h1_temp = new TH1D("h1_temp", "h1_temp;time (TDC); counts", 1024 / 8, 0, 4096);
					if ( filetype == 0 ) { tree -> Draw("ZCtime1 >> h1_temp", cut); }
					if ( filetype == 1 ) { tree -> Draw("istime >> h1_temp", cut); }
					if ( filetype == 2 ) { 
						tree -> Draw("t >> h1_temp", cut);
						h1 -> Add(h1_temp);
						delete h1_temp;
						delete tree; 
						break;
					}
					h1 -> Add(h1_temp);

					delete h1_temp;
					delete tree;	
				}
				delete input;
				c1 -> Clear();
			}
		if ( badtrees.Length() > 1 ) {
			std::cout << "Ignoring " << badtrees.Data() << std::endl;
		}
	}
//	}

	///////////////////////////////////////////////////////////////////////
	// Get RMS value of the histogram and fill differentiated histogram
	///////////////////////////////////////////////////////////////////////
	int maxbin = h1 -> GetNbinsX();
	double x1, x2, y1, y2, dx, dy, rms = 0;
	timings.push_back(0);
	for (int ibin = 1; ibin < maxbin; ibin++) {
		x1 = h1 -> GetBinCenter(ibin - 1);
		x2 = h1 -> GetBinCenter(ibin);
		y1 = h1 -> GetBinContent(ibin - 1);
		y2 = h1 -> GetBinContent(ibin);

		dx = x2 - x1;
		dy = y2 - y1;

		h1_diff -> SetBinContent(ibin, dy/dx);
		rms += pow(dy/dx, 2);
	}
	rms /= maxbin;
	rms = sqrt(rms);

	///////////////////////////////////////////////////////////////////////
	// Find the beam timings and the number of bunhces
	///////////////////////////////////////////////////////////////////////
	int binx1, binx2;
	for (int ibin = 0; ibin < maxbin - 1; ibin++) {
		x1 = h1_diff -> GetBinCenter(ibin);
		x2 = h1_diff -> GetBinCenter(ibin + 1);
		y1 = h1_diff -> GetBinContent(ibin);
		y2 = h1_diff -> GetBinContent(ibin + 1);
		binx1 = h1_diff -> GetBin(ibin);
		binx2 = h1_diff -> GetBin(ibin + 1);

		if ( x1 - timings.at(timings.size() - 1) > 100 && y1 > threshold * rms ) { 
			int ix1 = ibin;
			int ix2 = ibin + 1;
			int iy1 = h1_diff -> GetBinContent(ix1);
			int iy2 = h1_diff -> GetBinContent(ix2);
			double max = ix1;

			// if iy2 < 0, set max = ix1
			// if iy2 > 0 and iy2 > iy1, set max = ix2
			while (iy2 > 0) {
				iy1 = h1_diff -> GetBinContent(ix1);
				iy2 = h1_diff -> GetBinContent(ix2);

				y1 = h1 -> GetBinContent(ix1);
				y2 = h1 -> GetBinContent(ix2);

				if (y1 < y2) { max = ix2;}

				ix1 += 1;
				ix2 += 1;
			}	
			timings.push_back(h1_diff->GetBinCenter(max));
			counts.push_back(h1->GetBinContent(max));
		}
	}

	//cout << Form("%zu beam timings are found", timing.size() - 1) << endl;
	h1 -> Draw();
	for (int ibeam = 1; ibeam < timings.size(); ibeam++){
		TF1 *f1 = new TF1("f1", "gaus", timings.at(ibeam) - 100, timings.at(ibeam) + 100);
		h1 -> Fit("f1", "RQ+");
		double mean = f1 -> GetParameter(1);
		double sigma = f1 -> GetParameter(2);
		double start = mean - nsigma * sigma;
		double end = mean + nsigma * sigma;

		arisings.push_back(start);
		fallings.push_back(end);

		if (figures) {
			TLine *line1 = new TLine(start, 0, start, h1 -> GetMaximum());
			TLine *line2 = new TLine(end, 0, end, h1 -> GetMaximum());
			line1 -> SetLineColor(kBlack);
			line2 -> SetLineColor(kBlack);
			line1 -> Draw("same");
			line2 -> Draw("same");
		}
		delete f1;
	}
	
	timings.erase(timings.begin());
	nBeams = timings.size();

	for (int ibeam = 0; ibeam < timings.size(); ibeam++) {
		if ( arisings . at(ibeam) > timings . at(ibeam) || fallings . at(ibeam) < timings . at(ibeam) || arisings . at(ibeam) < 0 || fallings . at(ibeam) < 0 || arisings . at(ibeam) > 4096 || fallings .at(ibeam) > 4500 ) {
			std::cerr << "Error : Wrong rising (or falling) timing. Check that large pulses are collected enough." << std::endl; break;
		}
	}

	h1 -> SetTitle(Form("%s nBeams : %i", rundir.Data(), nBeams));
	c1 -> Update();
	
	if ( figures ) { c1 -> SaveAs(Form("./beamtiming_%s.png", rundir.Data()), "png"); } 
	
	delete c1;
	delete h1;
	delete h1_diff;
}

void BeamTiming::GetBeamTimings(TString dir, const int filetype, double threshold) {
	GetBeamTimings(dir, filetype, threshold, false);
}

void BeamTiming::GetBeamTimings(TString dir, const int filetype, bool figures) {
	GetBeamTimings(dir, filetype, 1, figures);
}

void BeamTiming::GetBeamTimings(TString dir, const int filetype) {
	GetBeamTimings(dir, filetype, 1, false);
}

void BeamTiming::GetBeamTimings(TString dir, bool figures) {
	GetBeamTimings(dir, 1, 1, figures);
}

void BeamTiming::GetBeamTimings(TString dir) {
	GetBeamTimings(dir, 1, 1, false);
};


