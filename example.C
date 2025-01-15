#include "BeamTiming.h"
#include "BeamTiming.C"
#include <iostream>
    
void example() {
	TString dir = "./r00010";
	
	BeamTiming bt;
	//bt.SetNsigma(2);		// Default : bt.SetNsigma(5);
	bt.GetBeamTimings(dir); // bt.GetBeamTimings(TString dir, int filetype, int threshold, bool figures)

	std::cout << bt.GetNbeams() << " beams are found." << std::endl;
	std::cout << "3rd pulse timing : " << bt.GetTimings() . at(3) << std::endl;
	std::cout << "3rd pulse counts : " << bt.GetCounts() . at(3) << std::endl;
	std::cout << "3rd pulse arising timing : " << bt.GetArisings() . at(3) << std::endl;
	std::cout << "3rd pulse falling timing : " << bt.GetFallings() . at(3) << std::endl;

	if ( bt.isBeamTiming(1700) ) {
		std::cout << Form("%i is in the beam bunch time window", 1700) << std::endl;
	}
	else { std::cout << "1700 is not in the beam nunch time window" << std::endl; }
	
	if ( bt.isBeamTiming(55) ) {
		std::cout << Form("%i is in the beam bunch time window", 55) << std::endl;
	}
	else { std::cout << "55 is not in the beam nunch time window" << std::endl; }
}
