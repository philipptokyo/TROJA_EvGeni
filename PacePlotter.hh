#ifndef PacePlotter_hh
#define PacePlotter_hh

#include "LibPerso.h"

#define thbins 18
#define enbins 30
  

class PacePlotter
{
  public:
  
  void parse(char filename[100]);
  
  PacePlotter();
  ~PacePlotter();

  TH1D* MakeCSHist();
  TH2D* Make2DHist();
  
  Int_t GetEnergyBins() const {return enbins;}
  Int_t GetThetaBins() const {return thbins;}
  Double_t GetThetaCenter(Int_t bin) {return thc[bin];}
  Double_t GetEnergyCenter(Int_t bin) {return enc[bin];}
  Double_t GetEThCounts(Int_t thbin, Int_t enbin) {return enth[thbin][enbin];}
  Double_t GetdSdO(Int_t thbin) {return dSdO[thbin];}
  Int_t GetPaceEvents() {return paceevents;}
  
  
  private:

  Int_t paceevents;
  Double_t thc[thbins]; // theta center
  Double_t enc[enbins]; // energy center
  Double_t enth[thbins][enbins]; // [angular bins][energy bins] 
  Double_t dSdO[thbins];
  
};

#endif
