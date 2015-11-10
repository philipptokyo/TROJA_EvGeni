#ifndef InputInfo_h
#define InputInfo_h

#include "LibPerso.h"

class InputInfo
{
	public:
	
	Float_t fBeamEnergy;
	
	char fInfilenameFromReaction[200];
	
	char fRootfilename[200];
	
	Int_t fNumberEvents;
	
	void parse(char filename[100]);
	
	InputInfo();
	~InputInfo();
	
	
	
};

#endif
