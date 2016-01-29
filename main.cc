/**
 * 
 * main.cc of makeEvents from Reactions code
 * 
 * 
 **/





#include "main.hh"
#include "InputInfo.hh"
#include "Nucleus.hh"
#include "Kinematics.hh"
#include "LibPerso.h"

using namespace std;




Int_t main(Int_t argc, char **argv){
  
  
  
  
  
  
  // if root shall stop the program before it finished, comment this in
  TApplication *theApp=new TApplication("theApp",  0, 0);
  
  
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

  
  Float_t beamE=info->fBeamEnergy;    // in MeV/u
  Float_t beamX=0.0, beamY=0.0, beamZ=0.0; // position in mm
  Float_t beamA=0.0, beamB=0.0; // angle in mrad
  Float_t beamTheta=0.0, beamPhi=0.0; // in rad
  Int_t beamNoEvents=0;
  


  Int_t projA=info->fProjA;
  Int_t projZ=info->fProjZ;
  Int_t targetA=info->fTargetA;
  Int_t targetZ=info->fTargetZ;
  Int_t lightA=info->fLightA;
  Int_t lightZ=info->fLightZ;
  
  const Int_t maxNumberOfStates=5;
  Int_t numberOfStates=info->fNumberOfStates;
  if(numberOfStates>maxNumberOfStates){
    cout << "Error: maximum number of states is " << maxNumberOfStates << "! You requested " << numberOfStates << endl;
    return 0;
  }
  Float_t maxExEnergy=info->fMaxExEnergy;

  Float_t stateEnergy[maxNumberOfStates]={0.0};
  if(numberOfStates>1){
    for(Int_t s=0; s<numberOfStates; s++){
      stateEnergy[s]=(Float_t)s * maxExEnergy / (Float_t)(numberOfStates-1);
      printf("  state %d, energy %f\n", s, stateEnergy[s]);
      if(!(info->ProfileBeamE()) && stateEnergy[s]>beamE){
        cout << "Error: state energy is larger than beam energy!" << endl;
        return 0;
      }
    }
  }else{
    // stateEnergy[0] is already 0
  }
  
  Float_t projMass=0.0, targetMass=0.0, lightMass=0.0, heavyMass=0.0, qValue=0.0; 
  
  if(targetZ!=lightZ){
    cout << "Dev Info: only neutron transfer is implemented so far!" << endl;
    cout << "Target Z and light ejectile Z are not the same (" << targetZ << " and " << lightZ << ", resp.)!" << endl;
    cout << "Choose another reaction channel!" << endl;
    return 0;
  }
  
  char* massFile = (char*)"/home/philipp/programme/reaction/mass.dat";

  Nucleus* proj = new Nucleus(projZ, projA-projZ, massFile);
  Nucleus* targ = new Nucleus(targetZ, targetA-targetZ, massFile);
  Nucleus* reco = new Nucleus(lightZ, lightA-lightZ, massFile);
  Nucleus* ejec = new Nucleus(projZ, projA-projZ+(targetA-lightA), massFile);

  projMass=proj->GetMass();
  targetMass=targ->GetMass();
  lightMass=reco->GetMass();
  heavyMass=ejec->GetMass();


  Kinematics* reaction[maxNumberOfStates];
  for(Int_t s=0; s<numberOfStates; s++){
     reaction[s] = new Kinematics(proj, targ, reco, ejec, beamE*projA, stateEnergy[s]);
  }

  qValue=reaction[0]->GetQValue();
  
   
  printf("Obtained masses: projectile %f, target %f, light ejectile %f, heavy ejectile %f; Q-value %f\n", projMass, targetMass, lightMass, heavyMass, qValue); 
  
   

  // make some cross checks of the inputs
  if(!(info->HaveOedoSimFileName()) && 
      ((info->ProfileBeamE()) || (info->ProfileBeamX()) || (info->ProfileBeamY()) || (info->ProfileBeamA()) || (info->ProfileBeamB()) )){
    cout << "ERROR! Beam profiling requested, but no root file for beam profile is given!" << endl;
    return 0;
  }




  TRandom3 *randomizer=new TRandom3();  
  randomizer->SetSeed(0);
  
  
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
    return 0;
  }

   
  if(info->HaveOedoSimFileName()){
    cout << "Opening beam profile file ..." << endl;
    fileBeamProfile=TFile::Open(info->fOedoSimFileName, "read");

    if(!fileBeamProfile){
      cout << "Error! OEDO beam simulation file not found! " << endl;
      cout << "Please check the file name 'beam_profile_file_oedo' in the input text file!" << endl;
      return 0;
    }

    cout << "Getting Tree ..." << endl;
    treeBeamProfile=(TTree*)fileBeamProfile->Get("events"); // shortened sim file
    
    if(!treeBeamProfile){
      cout << "Tree 'events' not found in root file!" << endl;
      return 0;
    }

    //treeBeamProfile->SetBranchAddress("fhxout", fhxout); // full sim file
    treeBeamProfile->SetBranchAddress("mass", &projA); // shortened sim file
    treeBeamProfile->SetBranchAddress("z", &projZ);  // shortened sim file
    treeBeamProfile->SetBranchAddress("energy", &beamE);  // shortened sim file
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
           lightPhi=0.0; // phi in rad
  Int_t lightPdgId=2212; //proton
  //Int_t heavyPdgId=1000501330; // 133Sn
  Int_t eventNumber=0;

  Float_t excEn=0.0, missMass=0.0;
  //Float_t heavyMass2=0.0;

  TTree *events = new TTree();
  events->Branch("eventNumber", &eventNumber, "eventNumber/I");
  
  // light ejectile data
  events->Branch("lightPdgId",&lightPdgId, "lightPdgId/I");
  events->Branch("lightEnergy",&lightEnergy, "lightEnergy/D");
  events->Branch("lightTheta",&lightTheta, "lightTheta/D");
  events->Branch("lightPhi",&lightPhi, "lightPhi/D");
  
  // projectile data
  events->Branch("beamEnergy", &beamE, "beamEnergy/F");
  events->Branch("beamX", &beamX, "beamX/F");
  events->Branch("beamY", &beamY, "beamY/F");
  events->Branch("beamZ", &beamZ, "beamZ/F");
  events->Branch("beamA", &beamA, "beamA/F");
  events->Branch("beamB", &beamB, "beamB/F");
  events->Branch("beamTheta", &beamTheta, "beamTheta/F");
  events->Branch("beamPhi", &beamPhi, "beamPhi/F");
  events->Branch("excitationEnergy", &excEn, "excitationEnergy/F");
  //events->Branch("heavyMass2", &heavyMass2, "heavyMass2/F");
  events->Branch("missingMass", &missMass, "missingMass/F");


  // determine PDG ID of light ejectile
  // todo: if the light ejectile is not the same for each event,
  //       than this part needs to be moved to the event loop
  char lightPdgIdName[20];
  sprintf(lightPdgIdName,"1000%02d%03d0",lightZ,lightA);
  lightPdgId=atoi(lightPdgIdName);
  //printf("Outgoing PDG ID: char %s, int %i\n", tmp, lightPdgId);
  
  
  if(info->fNumberEvents<0){
    printf("Error! Invalid number of events given: %i\nAboring...\n",info->fNumberEvents);
    abort();
  }else{
    printf("Start generating events...\n");
  }
  





  // ****************************************************************************************
  // ********************** start generating events *****************************************
  // ****************************************************************************************



  TStopwatch* watch = new TStopwatch();
  watch->Start();

  Int_t ignoredState=0; // counter for events which have at least one excited state ignored

  for(Int_t i=0; i<info->fNumberEvents; i++){

    eventNumber=i; // for the tree
    
     if(i%10000==0){
       printf("%i events generated (%i requested)\n", i, info->fNumberEvents);
     }


    // get beam information
    if(info->HaveOedoSimFileName()){ // todo: check if any information is needed from tree before reading it

      // shortened oedo sim file contains only good events
      Int_t rndmEvnt = (Int_t)randomizer->Uniform(beamNoEvents);
      treeBeamProfile->GetEvent(rndmEvnt);

    }else{ // no beam profile

      // these lines are somehow redundant
      beamA=0.0;
      beamB=0.0;
      beamX=0.0;
      beamY=0.0;
      beamZ=0.0;
      beamE=info->fBeamEnergy;
    }


    // calculate theta and phi of beam at target
    TVector3 vBeam(0.0, 0.0, 1.0);
    
    if(info->ProfileBeamA()){
      vBeam.RotateY(beamA/1000.0);
    }else{
      beamA=0.0;
    }

    if(info->ProfileBeamB()){
      vBeam.RotateX(beamB/1000.0);
    }else{
      beamB=0.0;
    }

    beamTheta=vBeam.Theta();
    beamPhi=vBeam.Phi();

    if(!info->ProfileBeamX()){
      beamX=0.0;
    }
    if(!info->ProfileBeamY()){
      beamY=0.0;
    }


    // get the light ejectile energy for a certain theta
    // this energy depends on the beam energy

    // first: get the theta
    // at the moment: only uniform theta distribution
    // todo: add physics here!!!!!
    lightTheta=randomizer->Uniform(180.0);
    
    // phi uniform
    lightPhi=randomizer->Uniform(2.0*TMath::Pi());


//// for testing    
//    lightTheta=90.0;
//    lightPhi=0.0;




    if(info->ProfileBeamE()){

      Int_t maxState=numberOfStates;
      Kinematics* reactionTemp[maxNumberOfStates];
      for(Int_t s=0; s<numberOfStates; s++){
        if(beamE>stateEnergy[s]){
          reactionTemp[s] = new Kinematics(proj, targ, reco, ejec, beamE*projA, stateEnergy[s]);
        }else{
          maxState=s;
          ignoredState++;
          cout << "Warning: state at " << stateEnergy[s] << " MeV is larger than beam energy (" << beamE << " MeV)! This state will be ignored! Using only " << maxState << " states!" << endl;
          break;
        }
      }
      
      Int_t st=(Int_t)randomizer->Uniform(maxState);
      lightEnergy=reactionTemp[st]->ELab(lightTheta/180.0*TMath::Pi(),2);

      for(Int_t s=0; s<maxState; s++){
        delete reactionTemp[s];
      }


    }else{
      beamE=info->fBeamEnergy;
      Int_t s=(Int_t)randomizer->Uniform(numberOfStates);
      //printf("  taking index %d",s);
      lightEnergy=reaction[s]->ELab(lightTheta/180.0*TMath::Pi(),2);
      //printf("  ELab is %f\n", lightEnergy);

      TVector3 dir(0,0,1);
      dir.SetTheta(lightTheta/180.0*TMath::Pi());
      TLorentzVector rec(dir,lightEnergy*1000+lightMass*1000);
      if(rec.Mag()>0)
        rec.SetRho( sqrt( (lightEnergy+lightMass)*(lightEnergy+lightMass) - lightMass*lightMass )*1000 );
      excEn = reaction[s]->GetExcEnergy(rec)/1000.0; // MeV
      // printf("excitation energy %f\n", excEn);

    }


    // continue in rad
    // as required for geant4
    lightTheta*=TMath::Pi()/180.0;
    
    // phi uniform
    lightPhi=randomizer->Uniform(2.0*TMath::Pi());

    TVector3 direction(0.0, 0.0, -1.0);

    direction.SetMagThetaPhi(1.0, lightTheta, lightPhi);

    // rotate to beam direction
    direction.RotateY(beamA/1000.0);
    direction.RotateX(beamB/1000.0);

    lightTheta=direction.Theta();
    lightPhi=direction.Phi();

    //lightPdgId=2212; //proton














    // reconstruct missing mass

    //projMass=122855.922;
    //lightMass=938.279;
    //heavyMass=123793.125;
    // conclusion: shift in missing mass comes from nuclear masses
    // but not the spread


    Float_t projGamma = (beamE * (Float_t)projA) / projMass + 1.0;
    //Float_t projBeta = TMath::Sqrt(1.0 - 1.0/(projGamma*projGamma));
    Float_t projMomentum = projMass * TMath::Sqrt(projGamma*projGamma - 1.0);

    Float_t lightGamma = lightEnergy/lightMass + 1.0;
    //Float_t lightBeta = TMath::Sqrt(1.0 - 1.0/(lightGamma*lightGamma));
    Float_t lightMomentum = lightMass * TMath::Sqrt(lightGamma*lightGamma - 1.0);

    Float_t cmBeta = projMomentum / (beamE*(Float_t)projA + projMass + targetMass );
    Float_t cmEnergy = TMath::Sqrt(projMass*projMass + targetMass*targetMass + 2.0*targetMass*(projMass*projGamma));
    
    TVector3 cmV(0.0, 0.0, cmBeta);
    TVector3 lightV(0.0, 0.0, 1.0);
    lightV.SetMagThetaPhi(lightMomentum, lightTheta, lightPhi);
    TLorentzVector lightL(0.0, 0.0, 0.0, 1.0);
    lightL.SetVect(lightV);
    lightL.SetE(lightEnergy+lightMass);

    //printf("light px %f, py %f, pz %f, e %f (lab)\n", lightL.Px(), lightL.Py(), lightL.Pz(), lightL.E());
    
    lightL.Boost(-cmV);

    //printf("light px %f, py %f, pz %f, e %f (cm)\n", lightL.Px(), lightL.Py(), lightL.Pz(), lightL.E());


    TLorentzVector heavyL;
    heavyL.SetVect(-lightL.Vect());
    heavyL.SetE(cmEnergy - lightL.E());

    //printf("heavy px %f, py %f, pz %f, e %f (cm), mag %f, mass %f, exc en %f\n", heavyL.Px(), heavyL.Py(), heavyL.Pz(), heavyL.E(), heavyL.M(), heavyMass, heavyL.M()-heavyMass);

    //heavyL.Boost(cmV);

    missMass = heavyL.M()-heavyMass;

    //printf("heavy px %f, py %f, pz %f, e %f (lab), mag %f, mass %f, exc en %f\n", heavyL.Px(), heavyL.Py(), heavyL.Pz(), heavyL.E(), heavyL.M(), heavyMass, heavyL.M()-heavyMass);


    //printf("\n");



    events->Fill();

  } // end of loop
 
  cout << endl; 
  cout << "Event generation done! " << eventNumber+1 << " events generated. " << ignoredState << " events with reduced number of excited states." << endl;

  watch->Stop();
  cout << "Took: real time " << watch->RealTime() << "sec., CPU time " << watch->CpuTime() << " sec." << endl;
  cout << endl; 

  outfile->cd();

  events->Write("events");

  outfile->Close();
  
  printf("Events written to file '%s'\n", info->fOutFileNameMakeEvents);
  printf("Thank you and good by!\n");
  
  // if histograms shall be plotted, run theApp
  // otherwise program closes
//  theApp->Run();
  delete theApp;  
  
  return 0;
}


