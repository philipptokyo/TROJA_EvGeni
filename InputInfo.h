#ifndef InputInfo_h
#define InputInfo_h

#include "LibPerso.h"

class InputInfo
{
	public:
	
	Float_t fBeamEnergy;
	
	char fInfilenameFromReactions[200];
	
	char fRootfilename[200];
	
	Int_t fNumberEvents;
	
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
