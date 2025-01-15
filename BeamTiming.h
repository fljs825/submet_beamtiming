#ifndef BEAMTIMING_MAIN_H
#define BEAMTIMING_MAIN_H

#include <vector>
#include <string>
#include <TString.h>

class BeamTiming {
private:
    std::vector<double> timings;
    std::vector<double> counts;
    std::vector<double> arisings;
    std::vector<double> fallings;
    int nBeams;
	double nsigma;

public: 
	BeamTiming();
	std::vector<double> GetTimings() const;
    std::vector<double> GetCounts() const;
    std::vector<double> GetArisings() const;
	std::vector<double> GetFallings() const;
	int GetNbeams() const;
	bool isBeamTiming(double time);
	void SetNsigma(double);
	//void SetThreshold(double);
	void GetBeamTimings(TString);
	void GetBeamTimings(TString, bool);
	void GetBeamTimings(TString, const int);
	void GetBeamTimings(TString, const int, bool);
	void GetBeamTimings(TString, const int, double);
	void GetBeamTimings(TString, const int, double, bool);
};

#endif

