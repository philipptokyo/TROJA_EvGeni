 
#ifndef FrescoPlotter_hh
#define FrescoPlotter_hh

#include "LibPerso.h"

#include "InputInfo.hh"

#define maxNumberOfStates 10

class FrescoPlotter
{
  
  public:
  
  FrescoPlotter();
  FrescoPlotter(InputInfo* info);
  ~FrescoPlotter();

  void CreateHistograms();

  // getter
  char GetNameProjectile(){return fProj[10]; }
  char GetNameTarget(){return fTarg[10]; }
  char GetNameRecoil(){return fReco[10]; }
  char GetNameEjectile(){return fEjec[10]; }
  
  Int_t GetMassNumberProjectile(){return fProjA;}
  Int_t GetMassNumberEjectile(){return fEjecA;}
  Int_t GetNumberOfStates() const {return fNumberOfStates;}

  Float_t GetBeamEnergyAMeV(){return fBeamEnergy;}
  Float_t GetBeamEnergyMeV(){return fBeamEnergy*fProjA;}

  TH1F* GetHistogramElastic(){return fHistCSelast;}
  TH1F* GetHistogramState(Int_t s){return fHistCS[s];}

  private:
  InputInfo* fInfo;

  char fProj[10], fTarg[10], fReco[10], fEjec[10];
  Int_t fProjA, fEjecA;
  Float_t fBeamEnergy;
  Int_t fNumberOfStates;
  //const Int_t maxNumberOfStates=10;
  Float_t fStateEnergy[maxNumberOfStates+1]; // index 0 is elasic

  TH1F *fHistCSelast;
  TH1F *fHistCS[maxNumberOfStates];


};

#endif
