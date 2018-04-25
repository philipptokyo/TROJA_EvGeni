/**
 * 
 * main.cc of makeEvents from Reactions code
 * 
 * 
 **/





#include "main.hh"
#include "InputInfo.hh"
#include "Nucleus.hh"
#include "Compound.hh"
#include "Reconstruction.hh"
#include "Kinematics.hh"
#include "LibPerso.h"
#include "FrescoPlotter.hh"
#include "PacePlotter.hh"

using namespace std;




Int_t main(Int_t argc, char **argv){
  
  
  // if root shall stop the program before it finished, comment this in
  TApplication *theApp=new TApplication("theApp",  0, 0);
  
  TStopwatch* watch = new TStopwatch();
  watch->Start();
  
  cout << "Welcome to event maker from Reactions code" << endl;
  
  
  cout << "Input arguments: " << endl;
  for (int i = 1; i < argc; i++) {
  	cout << argv[i] << endl;
  }
  
  if(argc!=2){
  	cout << "Please give 1 input argument: text file with input information, e.g. './makeEvents input.txt' " << endl; 
  	return 0;
  }
  
  
  
  InputInfo *info = new InputInfo();
  
  //get information from textfile
  info->parse(argv[1]);
  
  char tmp1[200], tmp2[200];

  Int_t numberOfStates=info->fNumberOfStates;
  //Float_t maxExEnergy=info->fMaxExEnergy;
  Float_t stateEnergy[maxNumberOfStates+1]={0.0}; // index 0 is elastic
  
  
  // for binning of histograms
  const Int_t maxBeamEnergyBins=200; // todo: has to be the same as in FrescoPlotter header
  Int_t beamEnergyBins=1; // todo: define this in InputInfo
  Int_t binN;
  Double_t binL, binU;
  TH1F* hist1dCSSPstates[maxBeamEnergyBins]; 
  TH1F* hist1dCSdOcmFresco[maxBeamEnergyBins][maxNumberOfStates+1];
  TH1F* hist1dCSdTcmFresco[maxBeamEnergyBins][maxNumberOfStates+1];
  TGraph* graf1dCSdTlab[maxBeamEnergyBins][maxNumberOfStates+1];
  TH1F* hist1dCSdTcmFrescoCut[maxBeamEnergyBins][maxNumberOfStates+1];
  TH2F* hist2dCSdOcmFresco[maxNumberOfStates+1];
  TH2F* hist2dCSdOcmxEFresco[maxNumberOfStates+1];
  TH2F* hist2dCSdTcmFresco[maxNumberOfStates+1];
  TH2F* hist2dCSdTcmxEFresco[maxNumberOfStates+1];
  //TH2F* histCScmFresco2dCut[maxNumberOfStates+1];
  TH2F* histETh = new TH2F("histETh", "Energy vs. Theta_lab", 360, 0, 180, 200, 0, 100);
  
  //TH1D* hist1dCSdOlabdegPace;
  TH2D* histEThdegPace;
  PacePlotter* pace = new PacePlotter();
  if(info->HavePaceFileName()){
    pace->parse(info->fOutFileNamePace);
    //hist1dCSdOlabdegPace = pace->MakeCSHist();
    histEThdegPace = pace->Make2DHist();
  }

  //histEThdegPace->Draw("colz");
  //theApp->Run();



  FrescoPlotter* frescoPlotter = new FrescoPlotter(info);
  if(info->HaveFrescoFileName()){
     
    frescoPlotter->CreateHistograms();
//printf("before frescoPlotter->UpdateInput() -- numberOfStates = %d\n", numberOfStates);
    frescoPlotter->UpdateInput(); // overwrite levels in InputInfo with data from fresco file
//printf("after  frescoPlotter->UpdateInput() -- numberOfStates = %d\n", numberOfStates);
//printf("after  frescoPlotter->UpdateInput() -- info->fNumberOfStates = %d\n", info->fNumberOfStates);
    
    //TH1F* histCScmFresco = frescoPlotter->GetHistogramState(1); // todo: remove this one!!

    //TH1F* histCScmFrescoElast = frescoPlotter->GetHistogramElastic();
    //TH1F* histCScmFresco[maxNumberOfStates+1];
    
    beamEnergyBins=frescoPlotter->GetBeamEnergyBins();
    numberOfStates=frescoPlotter->GetNumberOfStates();
//printf("frescoPlotter -- numberOfStates = %d\n", numberOfStates);
    
    // determine angle binning
    hist1dCSdOcmFresco[0][0] = frescoPlotter->Get1DHistogramOmegaState(0, 0);
    binN = hist1dCSdOcmFresco[0][0]->GetXaxis()->GetNbins();
    binL = hist1dCSdOcmFresco[0][0]->GetXaxis()->GetBinLowEdge(0);
    binU = hist1dCSdOcmFresco[0][0]->GetXaxis()->GetBinUpEdge(binN);

    Int_t binF=hist1dCSdOcmFresco[0][0]->FindBin(info->fAngleMin*180.0/TMath::Pi()); // cut from bin
    Int_t binT=hist1dCSdOcmFresco[0][0]->FindBin(info->fAngleMax*180.0/TMath::Pi()); // cut to bin

    //Float_t specFact[5]={1.0, 1.0, 1.0, 1.0, 1.0};  // dummy
    ////Float_t specFact[5]={1.0, 0.24, 0.36, 0.02, 2.5}; // 57Cr states, 0=elastic     
    ////Float_t specFact[5]={1.0, 0.44, 0.54, 0.04, 0.45}; // 53Ti states, 0=elastic     
    ////Float_t specFact[5]={1.0, 0.48, 0.89, 0.84, 0.30}; // 50Ca states, 0=elastic     

    for(Int_t h=0; h<numberOfStates+1; h++){
      hist2dCSdOcmFresco[h] = frescoPlotter->Get2DHistogramOmegaState(h);
      hist2dCSdOcmxEFresco[h] = frescoPlotter->Get2DHistogramOmegaxEState(h);
      hist2dCSdTcmFresco[h] = frescoPlotter->Get2DHistogramThetaState(h);
      hist2dCSdTcmxEFresco[h] = frescoPlotter->Get2DHistogramThetaxEState(h);



      printf("Creating single-particle cross section histograms\n");
      for(Int_t e=0; e<beamEnergyBins; e++){

        if(h==0){
          sprintf(tmp1, "histCSSP_energy%02d_states", e);
          sprintf(tmp2, "Integrated SP cross sections vs. state for beam energy %f MeV (%f AMeV)", frescoPlotter->GetBeamEnergyMeV(e), frescoPlotter->GetBeamEnergyAMeV(e));
          hist1dCSSPstates[e] = new TH1F(tmp1, tmp2, numberOfStates+1, 0, numberOfStates+1);
        }
        hist1dCSdOcmFresco[e][h] = frescoPlotter->Get1DHistogramOmegaState(e, h);
        hist1dCSdTcmFresco[e][h] = frescoPlotter->Get1DHistogramThetaState(e, h);
        hist1dCSdTcmFrescoCut[e][h] = (TH1F*)hist1dCSdTcmFresco[e][h]->Clone();
     
  
        if((info->IncludeElastic() && h==0) ){
          //histCSstates->SetBinContent(h+1, (histCScmFresco[h]->Integral(binF, binT))/info->fElasticDownscale);
          //histCSstates[e]->SetBinContent(h+1, (histCScmFresco[e][h]->Integral(0, binN))/info->fElasticDownscale);
          hist1dCSSPstates[e]->SetBinContent(h+1, TMath::Pi()/180.0* (hist1dCSdTcmFresco[e][h]->Integral(0, binN))/info->fElasticDownscale);
          //printf("Histogram %d: integral from %f (bin %d) to %f (bin%d) is %f\n", h, info->fAngleMin, bin1, info->fAngleMax, bin2, histCScmFresco[h]->Integral(bin1, bin2));
        }else if(h>0){
          //histCSstates->SetBinContent(h+1, histCScmFresco[h]->Integral(binF, binT));
          hist1dCSSPstates[e]->SetBinContent(h+1, TMath::Pi()/180.0* hist1dCSdTcmFresco[e][h]->Integral(0, binN) * info->fStateSpecFact[h]); // todo: spec fact in input file!!!!!

          //graf1dCSdTlab[e][h] = new TGraph();
          graf1dCSdTlab[e][h] = frescoPlotter->HistCMToGraphLab(hist1dCSdTcmFresco[e][h], frescoPlotter->GetBeamEnergyMeV(e));

          //if(frescoPlotter->GetBeamEnergyAMeV(e)>10.0){
          //  graf1dCSdTlab[e][h]->Draw("A*C");
          //  theApp->Run();
          //}

        }
  
        for(Int_t b=0; b<binN; b++){
          if(b<binF || b>=binT){
            hist1dCSdTcmFrescoCut[e][h]->SetBinContent(b, 0.0);
          }
        }

  
      }
      printf("done\n");


    }



    //histCScmFresco->Draw();
    //theApp->Run();

  }else{ // no fresco file
    binN = 180;
    binL = 0.0;
    binU = TMath::Pi();
  }

  if(info->FrescoHeaderOnly()){
    info->UnsetFrescoFileName();
  }
  

  TH1F* histCScm = new TH1F("histCScm","", binN, binL, binU);
  TH1F* histCSlab = new TH1F("histCSlab","", binN, binL, binU);
  TH2F* histCSlabVScm = new TH2F("histCSlabVScm", "", binN, binL, binU, binN, binL, binU);

  binN = 180;
  binL = 0.0;
  binU = 180.0;

  TH1F* histCScmDeg = new TH1F("histCScmDeg","", binN, binL, binU);
  TH1F* histCSlabDeg = new TH1F("histCSlabDeg","", binN, binL, binU);
  TH2F* histCSlabVScmDeg = new TH2F("histCSlabVScmDeg", "", binN, binL, binU, binN, binL, binU);


  
  //const Int_t maxNumberOfStates=5; // defined in FrescoPlotter.hh
  if(numberOfStates>maxNumberOfStates){
    cout << "Error: maximum number of states is " << maxNumberOfStates << "! You requested " << numberOfStates << endl;
    abort();
  }

//printf("maxNumberOfStates = %d\n", maxNumberOfStates);
  printf("Set:\n");
  //if(numberOfStates>1){
    for(Int_t s=0; s<numberOfStates+1; s++){
      //stateEnergy[s]=(Float_t)s * maxExEnergy / (Float_t)(numberOfStates-1); // todo: get it from frescoplotter
      stateEnergy[s]=info->fStateEnergy[s];
      printf("  state %d, energy %f\n", s, stateEnergy[s]);

      //if(!(info->ProfileBeamE()) && stateEnergy[s]>beamE){
      //  cout << "Warning: state energy is larger than beam energy! This might buggy!" << endl;
      //  //return 0;
      //}

    }
  //}else{
    // stateEnergy[0] is already 0
  //}
  

  
  Float_t beamE=info->fBeamEnergy;    // in AMeV
  Float_t beamX=0.0, beamY=0.0, beamZ=0.0; // position in mm
  Float_t beamA=0.0, beamB=0.0; // angle in mrad
  Float_t beamTheta=0.0, beamPhi=0.0; // in rad
  Int_t beamNoEvents=0;

  Float_t vertex[3] = {0.0};
  Float_t vertexBeamE = 0.0;
  Float_t vertexPath = 0.0; // path length through target to vertex
  


  TRandom3 *randomizer=new TRandom3();  
  randomizer->SetSeed(0);
  TF1 *sinus = new TF1("sinus","sin(x)",0,TMath::Pi()); 
  //TF1 *sinus = new TF1("sinus","sin(x)",0, TMath::Pi()/2.0); // only in forward direction
  //TF1 *sinus = new TF1("sinus","sin(x)",0, TMath::Pi()/10.0); // only in very forward direction
  //TF1 *sinus = new TF1("sinus","sin(x)",0, 0.0001); // only in most forward direction

  Int_t projA=info->fProjA;
  Int_t projZ=info->fProjZ;
  Int_t targetA=info->fTargetA;
  Int_t targetZ=info->fTargetZ;
  Int_t lightA=info->fLightA;
  Int_t lightZ=info->fLightZ;
  
  Float_t projMass=0.0, targetMass=0.0, lightMass=0.0, heavyMass=0.0, qValue=0.0; 
  
  if(targetZ!=lightZ){
    cout << "Dev Info: only neutron transfer is implemented so far!" << endl;
    cout << "Target Z and light ejectile Z are not the same (" << targetZ << " and " << lightZ << ", resp.)!" << endl;
    cout << "Choose another reaction channel!" << endl;
    abort();
  }
  
  char* massFile = (char*)"/home/philipp/programme/makeEvents/mass.dat";

  Nucleus* proj = new Nucleus(projZ, projA-projZ, massFile);
  Nucleus* targ = new Nucleus(targetZ, targetA-targetZ, massFile);
  Nucleus* reco = new Nucleus(lightZ, lightA-lightZ, massFile);
  Nucleus* ejec = new Nucleus(projZ, projA-projZ+(targetA-lightA), massFile);


  //printf("Nuclear masses: projectile %6.10f, target %6.10f, light ejectile %6.10f, heavy ejectile %6.10f; Q-value %f\n", projMass, targetMass, lightMass, heavyMass, qValue); 

  //printf("Info: overwriting masses of particles!\n"); 
  //proj->SetMass((double)122855.275467); // from geant4
  //reco->SetMass((double)938.272013);  // from geant4
  //targ->SetMass((double)1875.613); // from geant4
  //ejec->SetMass((double)123792.371068); // from geant4

  //printf("Nuclear masses: projectile %6.10f, target %6.10f, light ejectile %6.10f, heavy ejectile %6.10f; Q-value %f\n", projMass, targetMass, lightMass, heavyMass, qValue); 
  
  projMass=proj->GetMass();
  targetMass=targ->GetMass();
  lightMass=reco->GetMass();
  heavyMass=ejec->GetMass();
  
  Kinematics* reaction[maxNumberOfStates+1];
  //reaction[0] = new Kinematics(proj, targ, targ, proj, beamE*projA, stateEnergy[0]); // elastic scattering
  reaction[0] = new Kinematics(proj, targ, targ, proj, beamE*projA, stateEnergy[0]); // elastic scattering
  //printf("state energy %d: %f\n", 0, stateEnergy[0]);
  for(Int_t s=1; s<numberOfStates+1; s++){
      reaction[s] = new Kinematics(proj, targ, reco, ejec, beamE*projA, stateEnergy[s]);
      //printf("state energy %d: %f\n", s, stateEnergy[s]);
  }
  
  //printf("maxNumberOfStates = %d\n", maxNumberOfStates);
  //printf("getting q vaue, number of states is %i\n", numberOfStates);
  qValue=reaction[1]->GetQValue();


  // for energy loss calculation in target
  Compound* comTarg = new Compound((char*)"CD2");
  //char* massFile = (char*)"/home/philipp/programme/makeEvents/mass.dat";

  // define a thickness for the constructor
  //Double_t mm2mgcm = 0.819*100.0;
  Double_t mm2mgcm = info->GetTargetDensity()*100.0;
  Reconstruction* recons = new Reconstruction(proj, comTarg, mm2mgcm*2.0); // mg/cm2, Recon takes half






  
   
  printf("Nuclear masses: projectile %6.10f, target %6.10f, light ejectile %6.10f, heavy ejectile %6.10f; Q-value %f\n", projMass, targetMass, lightMass, heavyMass, qValue); 
  

  // make some cross checks of the inputs
  if(!(info->HaveOedoSimFileName()) && 
      ((info->ProfileBeamE()) || (info->ProfileBeamX()) || (info->ProfileBeamY()) || (info->ProfileBeamA()) || (info->ProfileBeamB()) )){
    cout << "ERROR! Beam profiling requested, but no root file for beam profile is given!" << endl;
    abort();
  }



  
  TFile* outfile = new TFile(info->fOutFileNameMakeEvents, "recreate");
  
  TTree* treeHeader = new TTree();
  treeHeader->Branch("projA", &projA, "projA/I");
  treeHeader->Branch("projZ", &projZ, "projZ/I");
  treeHeader->Branch("targetA", &targetA, "targetA/I");
  treeHeader->Branch("targetZ", &targetZ, "targetZ/I");
  treeHeader->Branch("lightA", &lightA, "lightA/I");
  treeHeader->Branch("lightZ", &lightZ, "lightZ/I");
  treeHeader->Branch("projMass", &projMass, "projMass/F");
  treeHeader->Branch("targetMass", &targetMass, "targetMass/F");
  treeHeader->Branch("lightMass", &lightMass, "lightMass/F");
  treeHeader->Branch("heavyMass", &heavyMass, "heavyMass/F");
  treeHeader->Branch("qValue", &qValue, "qValue/F");
  
  treeHeader->Fill();
  treeHeader->Write("header");

  
  
  // get the histograms with beam information 
  // from Oedo simulation by Matsushita
  TFile* fileBeamProfile;
  TTree* treeBeamProfile;
  


  if(!(info->HaveOedoSimFileName()) && 
      ((info->ProfileBeamE()) || (info->ProfileBeamX()) || (info->ProfileBeamY()) || (info->ProfileBeamA()) || (info->ProfileBeamB()) )){
    cout << "ERROR! Beam profiling requested, but no root file for beam profile is given!" << endl;
    abort();
  }

   
  if(info->HaveOedoSimFileName()){
    cout << "Opening beam profile file ..." << endl;
    fileBeamProfile=TFile::Open(info->fOedoSimFileName, "read");

    if(!fileBeamProfile){
      cout << "Error! OEDO beam simulation file not found! " << endl;
      cout << "Please check the file name 'beam_profile_file_oedo' in the input text file!" << endl;
      abort();
    }

    cout << "Getting Tree ..." << endl;
    treeBeamProfile=(TTree*)fileBeamProfile->Get("events"); // shortened sim file
    
    if(!treeBeamProfile){
      cout << "Tree 'events' not found in root file!" << endl;
      abort();
    }

    //treeBeamProfile->SetBranchAddress("fhxout", fhxout); // full sim file
    //treeBeamProfile->SetBranchAddress("mass", &projA); // shortened sim file
    //treeBeamProfile->SetBranchAddress("z", &projZ);  // shortened sim file
    treeBeamProfile->SetBranchAddress("energy", &beamE);  // MeV/u, shortened sim file
    treeBeamProfile->SetBranchAddress("x", &beamX);       // shortened sim file
    treeBeamProfile->SetBranchAddress("y", &beamY);       // shortened sim file
    treeBeamProfile->SetBranchAddress("a", &beamA);       // shortened sim file
    treeBeamProfile->SetBranchAddress("b", &beamB);       // shortened sim file

    beamNoEvents=treeBeamProfile->GetEntries();
    cout << beamNoEvents << " events found in tree " << endl;


  }
  
  
  
  
  

  // generate events and write them to tree

  // light ejectile position and flight direction
  Double_t lightEnergy=0.0, // energy in MeV 
           lightTheta=0.0, // theta in rad
           lightThetaCM=0.0,
           lightPhi=0.0; // phi in rad
  Int_t lightPdgId=2212; //proton
  //Int_t heavyPdgId=1000501330; // 133Sn
  Int_t eventNumber=0;
  Double_t cs=0;

  Int_t state=0; // aux, which state is populated
  Float_t excEn=0.0, missMass=0.0; // for cross checks with analysis code
  
  Double_t lightP[3], lightPtot, targetP[3], targetPtot, beamP[3], beamPtot, heavyP[3], heavyPtot;
  
  // gamma variables
  const Int_t maxGammas = maxNumberOfStates+1;
  Int_t gammaMul=0;
  Float_t gammaE[maxGammas]={NAN};
  Float_t gammaERest[maxGammas]={NAN};
  Float_t gammaTheta[maxGammas]={NAN};
  Float_t gammaThetaRest[maxGammas]={NAN};
  Float_t gammaPhi[maxGammas]={NAN};
  Float_t gammaPhiRest[maxGammas]={NAN};


  TTree *events = new TTree();
  events->Branch("eventNumber", &eventNumber, "eventNumber/I");
  
  // light ejectile data
  events->Branch("lightPdgId",&lightPdgId, "lightPdgId/I");
  events->Branch("lightEnergy",&lightEnergy, "lightEnergy/D");
  events->Branch("lightTheta",&lightTheta, "lightTheta/D");
  events->Branch("lightThetaCM",&lightThetaCM, "lightThetaCM/D");
  events->Branch("lightPhi",&lightPhi, "lightPhi/D");
  events->Branch("lightPhi",&lightPhi, "lightPhi/D");
  events->Branch("lightP",&lightP, "lightP[3]/D");
  events->Branch("lightPtot",&lightPtot, "lightPtot/D");
  events->Branch("targetP",&targetP, "targetP[3]/D");
  events->Branch("targetPtot",&targetPtot, "targetPtot/D");
  
  // projectile data
  events->Branch("beamEnergy", &beamE, "beamEnergy/F");
  events->Branch("beamX", &beamX, "beamX/F");
  events->Branch("beamY", &beamY, "beamY/F");
  events->Branch("beamZ", &beamZ, "beamZ/F");
  events->Branch("beamA", &beamA, "beamA/F");
  events->Branch("beamB", &beamB, "beamB/F");
  events->Branch("beamTheta", &beamTheta, "beamTheta/F");
  events->Branch("beamPhi", &beamPhi, "beamPhi/F");
  events->Branch("beamP",&beamP, "beamP[3]/D");
  events->Branch("beamPtot",&beamPtot, "beamPtot/D");
  events->Branch("heavyP",&heavyP, "heavyP[3]/D");
  events->Branch("heavyPtot",&heavyPtot, "heavyPtot/D");
  events->Branch("vertex", vertex, "vertex[3]/F");
  events->Branch("vertexBeamE", &vertexBeamE, "vertexBeamE/F");
  events->Branch("vertexPath", &vertexPath, "vertexPath/F");

  events->Branch("state", &state, "state/I");
  events->Branch("excitationEnergy", &excEn, "excitationEnergy/F");
  events->Branch("missingMass", &missMass, "missingMass/F");
  events->Branch("crossSection", &cs, "crossSection/D");


  // gamma data
  events->Branch("gammaMul", &gammaMul, "gammaMul/I");
  sprintf(tmp1, "gammaE[%d]/F", maxGammas);
  events->Branch("gammaE", gammaE, tmp1);
  sprintf(tmp1, "gammaTheta[%d]/F", maxGammas);
  events->Branch("gammaTheta", gammaTheta, tmp1);
  sprintf(tmp1, "gammaPhi[%d]/F", maxGammas);
  events->Branch("gammaPhi", gammaPhi, tmp1);

  sprintf(tmp1, "gammaERest[%d]/F", maxGammas);
  events->Branch("gammaERest", gammaERest, tmp1);
  sprintf(tmp1, "gammaThetaRest[%d]/F", maxGammas);
  events->Branch("gammaThetaRest", gammaThetaRest, tmp1);
  sprintf(tmp1, "gammaPhiRest[%d]/F", maxGammas);
  events->Branch("gammaPhiRest", gammaPhiRest, tmp1);





  // determine PDG ID of light ejectile
  // todo: if the light ejectile is not the same for each event,
  //       than this part needs to be moved to the event loop
//  char lightPdgIdName[20];
//  sprintf(lightPdgIdName,"1000%02d%03d0",lightZ,lightA);
//  lightPdgId=atoi(lightPdgIdName);
  //printf("Outgoing PDG ID: char %s, int %i\n", tmp, lightPdgId);
  
  watch->Stop();
  cout << "Pre-processing took: real time " << watch->RealTime() << "sec., CPU time " << watch->CpuTime() << " sec." << endl;
  cout << endl; 
  
  if(info->fNumberEvents<0){
    printf("Error! Invalid number of events given: %i\nAboring...\n",info->fNumberEvents);
    abort();
  }else{
    printf("Start generating events...\n");
  }
  
  
  if(info->Source()){
    if(info->fSourceTypeId == 0){
      lightPdgId=1000020040; // alpha
    }
    else if(info->fSourceTypeId == 1){
      lightPdgId=11; // beta-
    }
    else if(info->fSourceTypeId == 2){
      lightPdgId=-11; // beta+
    }
    else if(info->fSourceTypeId == 3){
      lightPdgId=22; // gamma
    }

    lightThetaCM=NAN;
    beamE=NAN;
    beamA=NAN;
    beamB=NAN;
    beamTheta=NAN;
    beamPhi=NAN;
    excEn=NAN;
    missMass=NAN;
  }




  // ****************************************************************************************
  // ********************** start generating events *****************************************
  // ****************************************************************************************



  watch->Start();

  Int_t ignoredState=0; // counter for events which have at least one excited state ignored

  for(Int_t i=0; i<info->fNumberEvents; i++){
    
    eventNumber=i; // for the tree
    
    if(i%100000==0){
      printf("Generating event %d (%i requested)\n", i, info->fNumberEvents);
    }


    if(info->Source()){
      
      //lightTheta = randomizer->Uniform(0.0, 180.0)/180.0*TMath::Pi();
      lightTheta = sinus->GetRandom();
      //lightTheta = 0.0;
      lightPhi = randomizer->Uniform(0.0, 360.0)/180.0*TMath::Pi();
      state = (Int_t)randomizer->Uniform(info->fSourceLines);
      lightEnergy = info->fSourceEnergies[state];

      beamX = 0.0; 
      beamY = 0.0; 
      beamZ = 0.0; 
      //beamX = randomizer->Uniform(-2.0, 2.0);
      //beamY = randomizer->Uniform(-2.0, 2.0);
      //beamZ = randomizer->Uniform(-0.0005, 0.0005);


      events->Fill();

      continue;

    }




    // get beam information
    // get first the information of the beam profile 
    // and reset afterwards those information which are not asked to be profiled
    if(info->HaveOedoSimFileName()){ 

      // shortened oedo sim file contains only good events
      Int_t rndmEvnt = (Int_t)randomizer->Uniform(beamNoEvents);
      treeBeamProfile->GetEvent(rndmEvnt);

//beamE=randomizer->Uniform(10.0, 15.0);

    }else{ // no beam profile
      
      // these lines are somehow redundant and not needed
      beamA=0.0;
      beamB=0.0;
      beamX=0.0;
      beamY=0.0;
      beamZ=0.0;
      beamE=info->fBeamEnergy;
    }

    // reset/keep information of not profiled beam
    if(!info->ProfileBeamX()){
      beamX=0.0;
    }
    if(!info->ProfileBeamY()){
      beamY=0.0;
    }
    if(!info->ProfileBeamE()){
      beamE=info->fBeamEnergy;
    }

    // calculate theta and phi of beam at target
    TVector3 vBeam(0.0, 0.0, 1.0);
    
    if(info->ProfileBeamA()){
      vBeam.RotateY(beamA/1000.0); // rotate in rad
    }else{
      beamA=0.0;
    }

    if(info->ProfileBeamB()){
      vBeam.RotateX(beamB/1000.0); // rotate in rad
    }else{
      beamB=0.0;
    }

    beamTheta=vBeam.Theta();
    beamPhi=vBeam.Phi();


    // calculate energy loss in target
    Double_t effThick = info->GetTargetSize(2)/vBeam.CosTheta();
    vertexPath = randomizer->Uniform(effThick);

    recons->SetTargetThickness(vertexPath*mm2mgcm);
    //printf("beamE %lf, projA %d, (double)(beamE*(Float_t)projA) %lf\n", beamE, projA, (double)(beamE*(Float_t)projA));
    vertexBeamE=recons->EnergyAfter((double)(beamE*(Float_t)projA), -5)/projA;
    //printf("vertexBeamE %lf\n", vertexBeamE);

    vertex[0]=beamX;
    vertex[1]=beamY;
    vertex[2]= (vertexPath * vBeam.CosTheta()) - (info->GetTargetSize(2)/2.0);


    

    // this is actually deprecated: 
    // check if all states can be populated
    Int_t maxState=numberOfStates+1;
    if(!(info->HaveFrescoFileName()) && !(info->FrescoHeaderOnly()) ){
      for(Int_t s=0; s<numberOfStates+1; s++){
        if(beamE < stateEnergy[s]){
          maxState=s;
          ignoredState++;
          cout << "Warning: state at " << stateEnergy[s] << " MeV is larger than beam energy (" << beamE << " MeV/u)! This state will be ignored! Using only " << maxState << " states!" << endl;
          break;
        }
      }
    }
    
    // choose the state populated
    // they are chosen from total cross section for each populated state
    if(info->HaveFrescoFileName()){

      // for beam energy profile: get the energy bin number first
      //Int_t energyBin = hist2dCSdOcmFresco[0]->GetYaxis()->FindBin(beamE*projA);
      Int_t energyBin = hist2dCSdOcmFresco[0]->GetYaxis()->FindBin(vertexBeamE*projA);
      //printf("Debug: Found bin %d for beam energy %f (maxbins %d)\n", energyBin, beamE*projA, frescoPlotter->GetBeamEnergyBins());
      if(frescoPlotter->GetBeamEnergyBins()-1 < energyBin){
        energyBin=frescoPlotter->GetBeamEnergyBins()-1;
        if(frescoPlotter->GetBeamEnergyBins() > 1){
          //printf("Warning: beam energy %f is not covered by fresco output! Cross section for highest availabe beam energy are used!\n", beamE*projA);
        }
      }
      
      //printf("getting state\n");
      state=(Int_t)hist1dCSSPstates[energyBin]->GetRandom();
      //printf("got state %d\n", state);


      //printf("getting theta and cs\n");
      lightTheta=hist1dCSdTcmFrescoCut[energyBin][state]->GetRandom();
      Int_t csbin=hist1dCSdTcmFrescoCut[energyBin][state]->GetXaxis()->FindBin(lightTheta);
      cs=hist1dCSdTcmFrescoCut[energyBin][state]->GetBinContent(csbin);
      lightTheta*=TMath::Pi()/180.0;
      //printf("got theta %f, sin theta %f, cs %f\n", lightTheta, TMath::Sin(lightTheta), cs);

      //if(TMath::IsNaN(lightTheta)){
      //  printf("integral %f\n", histCScmFrescoCut[energyBin][state]->Integral());
      //  hist1dCSdTcmFrescoCut[energyBin][state]->Draw();
      //  //histCSstates[energyBin]->Draw();
      //  theApp->Run();
      //}

    }else{

      if(info->IncludeElastic()){
        state=(Int_t)randomizer->Uniform(maxState);
      }else{
        state=(Int_t)(randomizer->Uniform(maxState-1))+1;
      }

      // uniform in CM system
      lightTheta=randomizer->Uniform(TMath::Pi());
      //lightTheta=randomizer->Uniform(info->fAngleMin, info->fAngleMax);
      //lightTheta=sinus->GetRandom();
    }
    
    lightThetaCM = lightTheta; // one of them will be boosted, depending on the existence of a fresco file


    // phi uniform
    lightPhi=randomizer->Uniform(2.0*TMath::Pi());


    Kinematics* reactionTemp;
    
    //if(info->ProfileBeamE()){

      if(state==0){
        //reactionTemp = new Kinematics(proj, targ, targ, proj, beamE*projA, stateEnergy[0]);
        reactionTemp = new Kinematics(proj, targ, targ, proj, vertexBeamE*projA, stateEnergy[0]);
      }else{
        //reactionTemp = new Kinematics(proj, targ, reco, ejec, beamE*projA, stateEnergy[state]);
        reactionTemp = new Kinematics(proj, targ, reco, ejec, vertexBeamE*projA, stateEnergy[state]);
      }

    //}else{
    //  reactionTemp = reaction[state];
    //}
    
    // fresco theta distribution is in CM
    // -> boost to lab
    // else, uniform distribution in lab system
    if(info->HaveFrescoFileName()){    
      if(state==0){ 
        lightTheta = reactionTemp->Angle_cm2lab(reactionTemp->GetVcm(2), lightTheta); // conversion from cm to lab
      }else{
        lightTheta = reactionTemp->Angle_cm2lab(reactionTemp->GetVcm(2), TMath::Pi()-lightTheta); // conversion from cm to lab
      }
    }else{
      // boost isotropic lab distribution to CM
      lightThetaCM = TMath::Pi() - reactionTemp->Angle_lab2cm(reactionTemp->GetVcm(2), lightTheta);
    }
        
    lightEnergy=reactionTemp->ELab(lightTheta,2);

    //// here todo, this is a hack, but can be introduced as proton source
    //lightEnergy=5.0;
    
    
    if(info->PaceOnly()){
      state=-1;
      histEThdegPace->GetRandom2(lightTheta, lightEnergy);
      lightTheta*=TMath::Pi()/180.0;
      lightThetaCM=-1.0;

    }

    //printf("CM/Lab conversion (state %d): thetaCM = %f, thetaLab = %f, betaCM = %f, lightEnergy = %f\n", state, lightThetaCM, lightTheta, reaction[state]->GetBetacm(), lightEnergy);

    Int_t a, z;
    if(state==0){ // elastic scattering
      lightMass=targetMass;
      heavyMass=projMass;
      qValue=0.0;
      a=targetA;
      z=targetZ;
    }else{
      lightMass=reco->GetMass();
      heavyMass=ejec->GetMass();
      qValue=reaction[1]->GetQValue();
      a=lightA;
      z=lightZ;
    }
    
    // write the correct PDG ID
    //if(a==1 && z==1){ //proton 
    //  lightPdgId=
    char lightPdgIdName[20];
    sprintf(lightPdgIdName,"1000%02d%03d0", z, a);
    lightPdgId=atoi(lightPdgIdName);


    histCScm->Fill(lightThetaCM);
    histCSlab->Fill(lightTheta);
    histCSlabVScm->Fill(lightThetaCM, lightTheta);

    histCScmDeg->Fill(lightThetaCM*180.0/TMath::Pi());
    histCSlabDeg->Fill(lightTheta*180.0/TMath::Pi());
    histCSlabVScmDeg->Fill(lightThetaCM*180.0/TMath::Pi(), lightTheta*180.0/TMath::Pi());

    histETh->Fill(lightTheta*180.0/TMath::Pi(), lightEnergy);

    //lightEnergy=reaction[s]->ELab(lightTheta/180.0*TMath::Pi(),2);


    // constant kinetic energy for testing reasons
    //lightEnergy=beamE;

    //printf("  ELab is %f\n", lightEnergy);

    
      
    // get excitation energy
    // for later cross-checks
    TVector3 dir(0,0,1);
    //dir.SetTheta(lightTheta/180.0*TMath::Pi());
    dir.SetTheta(lightTheta);
    TLorentzVector rec(dir,lightEnergy*1000+lightMass*1000);
    if(rec.Mag()>0){
      rec.SetRho( sqrt( (lightEnergy+lightMass)*(lightEnergy+lightMass) - lightMass*lightMass )*1000 );
    }
    excEn = reactionTemp->GetExcEnergy(rec)/1000.0; // MeV
    
    //if(state==0){
    //  printf("state %d: theta %f, energy %f, mass %f, rec.rho %f,  ex en %f\n", state, lightTheta, lightEnergy, lightMass, rec.Rho(), excEn);
    //}

    //if((excEn<-0.02) && !(info->PaceOnly())){ // something went wrong
    //  printf("Oops, excitation energy is %f MeV! Redoing this event.\n", excEn);
    //  i--;
    //  continue;
    //}

    //delete reactionTemp;


    TVector3 direction(0.0, 0.0, -1.0);

    direction.SetMagThetaPhi(1.0, lightTheta, lightPhi);

    // rotate to beam direction
    direction.RotateY(beamA/1000.0);
    direction.RotateX(beamB/1000.0);

    lightTheta=direction.Theta();
    lightPhi=direction.Phi();





    // reconstruct missing mass

    //projMass=122855.922;
    //lightMass=938.279;
    //heavyMass=123793.125;
    // conclusion: shift in missing mass comes from nuclear masses
    // but not the spread


    //Float_t projGamma = (beamE * (Float_t)projA) / projMass + 1.0;
    Float_t projGamma = (vertexBeamE * (Float_t)projA) / projMass + 1.0;
    Float_t projBeta = TMath::Sqrt(1.0 - 1.0/(projGamma*projGamma));
    Float_t projMomentum = projMass * TMath::Sqrt(projGamma*projGamma - 1.0);

    Float_t lightGamma = lightEnergy/lightMass + 1.0;
    //Float_t lightBeta = TMath::Sqrt(1.0 - 1.0/(lightGamma*lightGamma));
    Float_t lightMomentum = lightMass * TMath::Sqrt(lightGamma*lightGamma - 1.0);

    //Float_t cmBeta = projMomentum / (beamE*(Float_t)projA + projMass + targetMass );
    Float_t cmBeta = projMomentum / (vertexBeamE*(Float_t)projA + projMass + targetMass );
    Float_t cmEnergy = TMath::Sqrt(projMass*projMass + targetMass*targetMass + 2.0*targetMass*(projMass*projGamma));
    
    TVector3 cmV(0.0, 0.0, cmBeta);
    cmV.RotateY(beamA/1000.0);
    cmV.RotateX(beamB/1000.0);
    
    
    TVector3 lightV(0.0, 0.0, 1.0);
    lightV.SetMagThetaPhi(lightMomentum, lightTheta, lightPhi);
    TLorentzVector lightL(0.0, 0.0, 0.0, 1.0);
    lightL.SetVect(lightV);
    lightL.SetE(lightEnergy+lightMass);

    lightL.Boost(-cmV);


    TLorentzVector heavyL;
    heavyL.SetVect(-lightL.Vect());
    heavyL.SetE(cmEnergy - lightL.E() + heavyMass);

    lightP[0]=lightL.Px();
    lightP[1]=lightL.Py();
    lightP[2]=lightL.Pz();
    lightPtot=TMath::Sqrt(lightP[0]*lightP[0] + lightP[1]*lightP[1] + lightP[2]*lightP[2]);
    heavyP[0]=heavyL.Px();
    heavyP[1]=heavyL.Py();
    heavyP[2]=heavyL.Pz();
    heavyPtot=TMath::Sqrt(heavyP[0]*heavyP[0] + heavyP[1]*heavyP[1] + heavyP[2]*heavyP[2]);
    
    TVector3 projV(0,0,1.0);
    projV.RotateY(beamA/1000.0);
    projV.RotateX(beamB/1000.0);

    TLorentzVector projL(0.0, 0.0, 0.0, 1.0);
    projL.SetVect(projV);
    //projL.SetE(beamE * (Float_t)projA + projMass);
    projL.SetE(vertexBeamE * (Float_t)projA + projMass);
    
    
//    beamP[0]=projL.Px();
//    beamP[1]=projL.Py();
//    beamP[2]=projL.Pz();
//    beamPtot=TMath::Sqrt(beamP[0]*beamP[0] + beamP[1]*beamP[1] + beamP[2]*beamP[2]);
//printf("\nbefore boost: beamPtot %lf, beamE * (Float_t)projA + projMass %f, projL.E() %f\n", beamPtot, beamE * (Float_t)projA + projMass, projL.E());
    
    projL.Boost(cmV);

    beamP[0]=projL.Px();
    beamP[1]=projL.Py();
    beamP[2]=projL.Pz();
    beamPtot=TMath::Sqrt(beamP[0]*beamP[0] + beamP[1]*beamP[1] + beamP[2]*beamP[2]);

    TLorentzVector targetL(0.0, 0.0, 0.0, 1.0);
    targetL.SetVect(-projV);
    targetL.SetE(targetMass);
    targetL.Boost(cmV);

    targetP[0]=targetL.Px();
    targetP[1]=targetL.Py();
    targetP[2]=targetL.Pz();
    targetPtot=TMath::Sqrt(targetP[0]*targetP[0] + targetP[1]*targetP[1] + targetP[2]*targetP[2]);

//printf("cmEnergy %f, lightPtot %lf, targetPtot %lf, beamPtot %lf, heavyPtot %lf\n", cmEnergy, lightPtot, targetPtot, beamPtot, heavyPtot);

    missMass = -heavyL.M()+heavyMass;

    //missMass = -heavyL.M()+heavyMass;
    missMass = -heavyL.M();
    
    if(info->AddGammas()){
      // generate gamma
      gammaMul=info->fStateGammaMul[state];
      //printf("state %d, gammaMul %d\n", state, gammaMul);
      //if(state>1){
      //  gammaMul=1;
      if(gammaMul>0){
        for(Int_t gam=0; gam<gammaMul; gam++){
          //gammaThetaRest[0]=randomizer->Uniform(0, TMath::Pi());
          gammaThetaRest[gam]=sinus->GetRandom();
          gammaPhiRest[gam]=randomizer->Uniform(-TMath::Pi(), TMath::Pi());
          //gammaERest[gam]=stateEnergy[state];
          gammaERest[gam]=info->fStateGammaEnergies[state][gam];

          //printf("generated gamma with energy %f, theta %f, phi %f\n", gammaENoBoost[0], gammaTheta[0], gammaPhi[0]);

          TVector3 vGamma(0,0,1);
          vGamma.SetMagThetaPhi(gammaERest[gam], gammaThetaRest[gam], gammaPhiRest[gam]);
          TLorentzVector lGamma(vGamma, gammaERest[gam]);

          vBeam.SetMag(projBeta);

          lGamma.Boost(vBeam);
          
          gammaE[gam]=lGamma.P();
          gammaTheta[gam]=lGamma.Theta();
          gammaPhi[gam]=lGamma.Phi();

        }

      }else{
        //gammaMul=0;
        for(Int_t i=0; i<maxGammas; i++){
          gammaTheta[i]=NAN;
          gammaPhi[i]=NAN;
          gammaE[i]=NAN;
          gammaThetaRest[i]=NAN;
          gammaPhiRest[i]=NAN;
          gammaERest[i]=NAN;
        }
      }
    }


    events->Fill();

  } // end of loop
 
  cout << endl; 
  cout << "Event generation done! " << eventNumber+1 << " events generated. " << ignoredState << " events with reduced number of excited states." << endl;

  watch->Stop();
  cout << "Took: real time " << watch->RealTime() << "sec., CPU time " << watch->CpuTime() << " sec." << endl;
  cout << endl; 

  outfile->cd();

  events->Write("events");
  if(info->HaveFrescoFileName()){
    // create folders
    outfile->mkdir("states", "Integrated cross sections for each beam energy");
    for(Int_t s=0; s<numberOfStates+1; s++){
      sprintf(tmp1, "state%02d", s);
      if(s==0){
        sprintf(tmp2, "state at energy %f MeV, elastic scattering", info->fStateEnergy[s]);
      }else{
        sprintf(tmp2, "state at energy %f MeV", info->fStateEnergy[s]);
      }
      outfile->mkdir(tmp1, tmp2);
    }
    
    // write histograms
    for(Int_t e=0; e<beamEnergyBins; e++){
      outfile->cd("states");
      hist1dCSSPstates[e]->Write(Form("histCS_SP_energy%02d_states", e));
      for(Int_t s=0; s<numberOfStates+1; s++){
        outfile->cd(Form("state%02d", s));
        hist1dCSdOcmFresco[e][s]->Write(Form("histCS_dOmega_Cm_Fresco_%02d", e));
        hist1dCSdTcmFresco[e][s]->Write(Form("histCS_dTheta_Cm_Fresco_%02d", e));
        hist1dCSdTcmFrescoCut[e][s]->Write(Form("histCS_dTheta_Cm_Fresco_AngleCut_%02d", e));
//        histCScmFrescoCut[e][s]->Write(Form("histCScmFrescoCut _%02d", e));
        if(s>0){graf1dCSdTlab[e][s]->Write(Form("graphCS_dTheta_Lab_Fresco_%02d", e));}
      }
    }
    
    // write 2d histograms
    for(Int_t s=0; s<numberOfStates+1; s++){
      outfile->cd();
      hist2dCSdOcmFresco[s]->Write(Form("histCS_dOmega_Cm_Fresco_2d_%02d", s));
      hist2dCSdOcmxEFresco[s]->Write(Form("histCS_dOmega_xE_Cm_Fresco_2d_%02d", s));
      hist2dCSdTcmFresco[s]->Write(Form("histCS_dTheta_Cm_Fresco_2d_%02d", s));
      hist2dCSdTcmxEFresco[s]->Write(Form("histCS_dTheta_xE_Cm_Fresco_2d_%02d", s));
    }

  }
  histCScm->Write("histCScm");
  histCScmDeg->Write("histCScmDeg");
  histCSlab->Write("histCSlab");
  histCSlabDeg->Write("histCSlabDeg");
  histCSlabVScm->Write("histCSlabVScm");
  histCSlabVScmDeg->Write("histCSlabVScmDeg");
  
  histETh->Write("histETh");

  //histCSlab->Draw();
  //histCSlabVScm->Draw();

  outfile->Close();
  
  printf("Events written to file '%s'\n", info->fOutFileNameMakeEvents);
  printf("Thank you and good by!\n");
  
  // if histograms shall be plotted, run theApp
  // otherwise program closes
//  theApp->Run();
  delete theApp;  
  
  return 0;
}


