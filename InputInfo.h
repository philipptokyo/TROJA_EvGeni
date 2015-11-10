#ifndef InputInfo_h
#define InputInfo_h

#include "LibPerso.h"

class InputInfo
{
	public:
	
	Float_t beamBeta;
	Float_t beamGamma;
	
	char infilenameFromDicebox[200];
	char infilenameFromTalys[200];
	
	char rootfilenameDicebox[200];
	char rootfilenameTalys[200];
	
	char filenameEvents[200];
	
	Int_t numberEvents;
	
// 	Int_t maxGammaMul;
	Long64_t maxLines;
	Int_t verbosity;
	
	Float_t binWidth;
	
	void parse(char filename[100]);
	
	InputInfo();
	~InputInfo();
	
	/*
	 * 
	 * necessary informaiton:
	 * input file name (text file with output of dicebox)
	 * output file name (root file with trees)
	 * 
	 * verbose=false
	 * 
	 * max gamma mul
	 * 
	 * 
	 * 
	 * 
	 * 
	 * */
	
	
};

#endif
