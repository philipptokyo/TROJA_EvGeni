 
#ifndef FrescoPlotter_hh
#define FrescoPlotter_hh

#include "LibPerso.h"

#include "InputInfo.hh"
#include "Nucleus.hh"
#include "Kinematics.hh"


// #define maxNumberOfStates 10 // is now defined in InputInfo.hh
#define energyBinsMax 200


class FrescoPlotter
{
  
  public:
  
  FrescoPlotter();
  FrescoPlotter(InputInfo* info);
  ~FrescoPlotter();

  void CreateHistograms();
  void CreateHistograms(Int_t fileIndex);
  void UpdateInput();

  // getter
  char GetNameProjectile(){return fProj[10]; }
  char GetNameTarget(){return fTarg[10]; }
  char GetNameRecoil(){return fReco[10]; }
  char GetNameEjectile(){return fEjec[10]; }
  
  Int_t GetMassNumberProjectile(){return fProjA;}
  Int_t GetMassNumberEjectile(){return fEjecA;}
  Int_t GetNumberOfStates() const {return fNumberOfStates;}
  
  Int_t GetBeamEnergyBins() {return fBeamEnergyBins;}
  Float_t GetBeamEnergyAMeV(Int_t e){return fBeamEnergy[e];}
  Float_t GetBeamEnergyMeV(Int_t e){return fBeamEnergy[e]*fProjA;}

  //TH1F* GetHistogramElastic(){return fHistCSelast;}
  TH1F* Get1DHistogramOmegaState(Int_t e, Int_t s){return fHist1dCSdO[e][s];}
  TH2F* Get2DHistogramOmegaState(Int_t s){return fHist2dCSdO[s];}
  TH1F* Get1DHistogramThetaState(Int_t e, Int_t s){return fHist1dCSdT[e][s];}
  TH2F* Get2DHistogramThetaState(Int_t s){return fHist2dCSdT[s];}

  TGraph* HistCMToGraphLab(TH1F* hist);
  TGraph* HistCMToGraphLab(TH1F* hist, Float_t beamEnergy);

  private:
  InputInfo* fInfo;

  char fProj[10], fTarg[10], fReco[10], fEjec[10];
  Int_t fProjA, fEjecA;

  Int_t fBeamEnergyBins;
  Float_t fBeamEnergy[energyBinsMax];
  Int_t fNumberOfStates;
  //const Int_t maxNumberOfStates=10;
  Float_t fStateEnergy[maxNumberOfStates+1]; // index 0 is elasic (should be 0)

  //TH1F *fHistCSelast;
  TH1F *fHist1dCSdO[energyBinsMax][maxNumberOfStates+1]; // dSigma/dOmega in CM
  TH2F *fHist2dCSdO[maxNumberOfStates+1];
  TH1F *fHist1dCSdT[energyBinsMax][maxNumberOfStates+1]; // dSigma/dTheta*2pi in CM
  TH2F *fHist2dCSdT[maxNumberOfStates+1];


};

#endif
