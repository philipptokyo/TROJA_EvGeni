 
#ifndef FrescoPlotter_hh
#define FrescoPlotter_hh

#include "LibPerso.h"

#include "InputInfo.hh"

class FrescoPlotter
{
  
  public:
  
  FrescoPlotter();
  FrescoPlotter(InputInfo* info);
  ~FrescoPlotter();

  TH1F* CreateHistogram();

  private:
  InputInfo* fInfo;


};

#endif
