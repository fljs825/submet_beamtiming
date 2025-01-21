#include "BeamTiming.h"
#include "BeamTiming.C"
#include <iostream>
    
void test() {
	
	///////////////////////////////////////////////
	// Load BeamTiming Class
	///////////////////////////////////////////////
	BeamTiming bt;
	for (int irun = 1; irun <= 56; irun++) {
		TString dir;
		if (irun < 10) { dir = Form("./r0000%i", irun); }
		if (irun >= 10) { dir = Form("./r000%i", irun); }

		////////////////////////////////////////////////////////////////////////////////////////////
		// GetBeamTimings(TString directory, const int filetype, const int threshold, bool image);
		// filetype == 0 : previous version tree												  
		// filetype == 1 : recent version tree (pulse-based)												  
		// filetype == 2 : recent version tree (event-based)									  
		// default threshold == 3 : recommended threshold range 1 <= threshold <= 5			  
		// bool image == true : save beam timing finder result as a png file.					  
		// bool image == false : not save beam timing finder result								  
		////////////////////////////////////////////////////////////////////////////////////////////
		//bt.SetNsigma(5); // Set broadness of the beam time range. Default : 5
		bt.GetBeamTimings(dir, 1, 3, true);
		
		////////////////////////////////////////////////////////////////////////////////////////////
		// bt.GetNbeams() returns the number of discovered beams.
		////////////////////////////////////////////////////////////////////////////////////////////
		std::cout << bt.GetNbeams() << " beams are found" << std::endl;

		////////////////////////////////////////////////////////////////////////////////////////////
		// bt.GetTimings(), bt.GetCounts(), bt.GetArisings(), bt.GetFallings() example
		////////////////////////////////////////////////////////////////////////////////////////////
		std::vector<double> timings = bt.GetTimings();
		std::vector<double> counts = bt.GetCounts();
		std::vector<double> arisings = bt.GetArisings();
		std::vector<double> fallings = bt.GetFallings();

		////////////////////////////////////////////////////////////
		// Print beam timings
		////////////////////////////////////////////////////////////
		for (int i = 0; i < bt.GetNbeams(); i++) {
			std::cout << "iBeams : " << i 
				<< " | counts : " << counts . at(i) 
				<< " | arising : " << arisings . at(i) 
				<< " | peak timing : " << timings . at(i) 
				<< " | falling : " << fallings.at(i)
				<< std::endl;
		}

		////////////////////////////////////////////////
		// Check "isBeamTiming" works well
		////////////////////////////////////////////////
		TFile *file = new TFile(Form("%s/b1.root", dir.Data()), "READ");
		if ( !file || file->IsZombie() ) { 
			std::cerr << "Error: Cannot open file: " << Form("%s/b1.root", dir.Data()) << std::endl;
			continue;
		}

		TTree *tree = (TTree *) file -> Get("ch0/pulse_ch0");
		int n = tree -> GetEntries();
		double istime, zctime1;
		tree -> SetBranchAddress("istime", &istime);
		
		TCanvas *c1 = new TCanvas("c1", "c1", 800, 600);
		TH1D *h1 = new TH1D("h1", "h1", 100, 0, 4096);
		for (int i = 0; i < n; i++) {
			tree -> GetEntry(i);
			if ( bt.isBeamTiming(istime) ) { // Fill histogram if istime is in the beam timings
				h1 -> Fill(istime);
			}
		}
		h1 -> Draw();
		c1 -> SaveAs("figure1.png", "png");

		delete h1;
		delete c1;
	}
}

