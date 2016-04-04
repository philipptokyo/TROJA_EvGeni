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
#include "FrescoPlotter.hh"

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

  Int_t numberOfStates=info->fNumberOfStates;
  //Float_t maxExEnergy=info->fMaxExEnergy;
  Float_t stateEnergy[maxNumberOfStates+1]={0.0}; // index 0 is elastic
  
  // for binning of histograms
  Int_t binN;
  Double_t binL, binU;
  TH1F* histCScmFresco[maxNumberOfStates+1];
  
  if(info->HaveFrescoFileName()){
    FrescoPlotter* frescoPlotter = new FrescoPlotter(info);
     
    frescoPlotter->CreateHistograms();
    frescoPlotter->UpdateInput();
    
    //TH1F* histCScmFresco = frescoPlotter->GetHistogramState(1); // todo: remove this one!!

    //TH1F* histCScmFrescoElast = frescoPlotter->GetHistogramElastic();
    //TH1F* histCScmFresco[maxNumberOfStates+1];

    numberOfStates=frescoPlotter->GetNumberOfStates();

    for(Int_t h=0; h<numberOfStates+1; h++){
      histCScmFresco[h] = frescoPlotter->GetHistogramState(h);
    }


    //TCanvas* canCS = new TCanvas();
    //canCS->Divide(4,3);
    ////canCS->cd(1);
    ////histCScmFrescoElast->Draw();
    //for(Int_t h=0; h<numberOfStates+1; h++){
    //  canCS->cd(h+1);
    //  histCScmFresco[h]->Draw();
    //}

    //theApp->Run();


    //histCScmFresco->Draw();
    //theApp->Run();

    // determine binning
    binN = histCScmFresco[0]->GetXaxis()->GetNbins();
    binL = histCScmFresco[0]->GetXaxis()->GetBinLowEdge(0);
    binU = histCScmFresco[0]->GetXaxis()->GetBinUpEdge(binN);
  }else{ // no fresco file
    binN = 180;
    binL = 0.0;
    binU = TMath::Pi();
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


  
  Float_t beamE=info->fBeamEnergy;    // in MeV/u
  Float_t beamX=0.0, beamY=0.0, beamZ=0.0; // position in mm
  Float_t beamA=0.0, beamB=0.0; // angle in mrad
  Float_t beamTheta=0.0, beamPhi=0.0; // in rad
  Int_t beamNoEvents=0;
  


  TRandom3 *randomizer=new TRandom3();  
  randomizer->SetSeed(0);
  

  Int_t projA=info->fProjA;
  Int_t projZ=info->fProjZ;
  Int_t targetA=info->fTargetA;
  Int_t targetZ=info->fTargetZ;
  Int_t lightA=info->fLightA;
  Int_t lightZ=info->fLightZ;
  
  //const Int_t maxNumberOfStates=5; // defined in FrescoPlotter.hh
  if(numberOfStates>maxNumberOfStates){
    cout << "Error: maximum number of states is " << maxNumberOfStates << "! You requested " << numberOfStates << endl;
    return 0;
  }

  printf("Set:\n");
  if(numberOfStates>1){
    for(Int_t s=0; s<numberOfStates+1; s++){
      //stateEnergy[s]=(Float_t)s * maxExEnergy / (Float_t)(numberOfStates-1); // todo: get it from frescoplotter
      stateEnergy[s]=info->fStateEnergy[s];
      printf("  state %d, energy %f\n", s, stateEnergy[s]);

      if(!(info->ProfileBeamE()) && stateEnergy[s]>beamE){
        cout << "Warning: state energy is larger than beam energy! This might buggy!" << endl;
        //return 0;
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
  reaction[0] = new Kinematics(proj, targ, targ, proj, beamE*projA, stateEnergy[0]); // elastic scattering
  for(Int_t s=1; s<numberOfStates+1; s++){
     reaction[s] = new Kinematics(proj, targ, reco, ejec, beamE*projA, stateEnergy[s]);
  }

  qValue=reaction[1]->GetQValue();
  
   
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
           lightThetaCM=0.0,
           lightPhi=0.0; // phi in rad
  Int_t lightPdgId=2212; //proton
  //Int_t heavyPdgId=1000501330; // 133Sn
  Int_t eventNumber=0;

  Int_t state=0; // aux, which state is populated
  Float_t excEn=0.0, missMass=0.0;

  TTree *events = new TTree();
  events->Branch("eventNumber", &eventNumber, "eventNumber/I");
  
  // light ejectile data
  events->Branch("lightPdgId",&lightPdgId, "lightPdgId/I");
  events->Branch("lightEnergy",&lightEnergy, "lightEnergy/D");
  events->Branch("lightTheta",&lightTheta, "lightTheta/D");
  events->Branch("lightThetaCM",&lightThetaCM, "lightThetaCM/D");
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
  events->Branch("state", &state, "state/I");
  events->Branch("excitationEnergy", &excEn, "excitationEnergy/F");
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
    
    if((i-1)%100000==0){
      printf("%i events generating... (%i requested)\n", i, info->fNumberEvents);
    }


    // get beam information
    // get first the information of the beam profile 
    // and reset afterwards those information which are not asked to be profiled
    if(info->HaveOedoSimFileName()){ 

      // shortened oedo sim file contains only good events
      Int_t rndmEvnt = (Int_t)randomizer->Uniform(beamNoEvents);
      treeBeamProfile->GetEvent(rndmEvnt);

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




    // check if all states can be populated
    Int_t maxState=numberOfStates+1;
    for(Int_t s=0; s<numberOfStates+1; s++){
      if(beamE < stateEnergy[s]){
        maxState=s;
        ignoredState++;
        cout << "Warning: state at " << stateEnergy[s] << " MeV is larger than beam energy (" << beamE << " MeV/u)! This state will be ignored! Using only " << maxState << " states!" << endl;
        break;
      }
    }
    
    
    // choose the state populated
    // todo: they should be chosen from total cross section for each populated state
    state=(Int_t)randomizer->Uniform(maxState);

// ignore elastic scattering for bug fixing ...
//state=(Int_t)(randomizer->Uniform(maxState-1))+1;

    
    if(info->HaveFrescoFileName()){
      // take theta distribution from fresco output
      // todo: beam energy profile needs to be taken into account!
      lightTheta=histCScmFresco[state]->GetRandom();
    }else{
      // uniform in CM system
      lightTheta=randomizer->Uniform(TMath::Pi());
    }
    
    lightThetaCM = lightTheta; 


    // phi uniform
    lightPhi=randomizer->Uniform(2.0*TMath::Pi());


    Kinematics* reactionTemp;
    
    if(info->ProfileBeamE()){

      if(state==0){
        reactionTemp = new Kinematics(proj, targ, targ, proj, beamE*projA, stateEnergy[0]);
      }else{
        reactionTemp = new Kinematics(proj, targ, reco, ejec, beamE*projA, stateEnergy[state]);
      }

    }else{
      reactionTemp = reaction[state];
    }
      
    lightTheta = reactionTemp->Angle_cm2lab(-reactionTemp->GetVcm(2), lightTheta); // conversion from cm to lab
        
    lightEnergy=reactionTemp->ELab(lightTheta,2);

      //delete reactionTemp;


    
    //printf("CM to Lab conversion: thetaCM = %f, thetaLab = %f,Vcm = %f, betaCM = %f\n", lightThetaCM, lightTheta, reaction[st]->GetVcm(2), reaction[st]->GetBetacm());



    histCScm->Fill(lightThetaCM);
    histCSlab->Fill(lightTheta);
    histCSlabVScm->Fill(lightThetaCM, lightTheta);

    histCScmDeg->Fill(lightThetaCM*180.0/TMath::Pi());
    histCSlabDeg->Fill(lightTheta*180.0/TMath::Pi());
    histCSlabVScmDeg->Fill(lightThetaCM*180.0/TMath::Pi(), lightTheta*180.0/TMath::Pi());

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
    
    //delete reactionTemp;

    //printf("Light theta cm %f,  lab %f, Vcm %f, energy %f\n", lightThetaCM, lightTheta, reaction[s]->GetVcm(2), lightEnergy);

    


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

    missMass = -heavyL.M()+heavyMass;

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
  if(info->HaveFrescoFileName()){
    for(Int_t s=0; s<numberOfStates+1; s++){
      histCScmFresco[s]->Write(Form("histCScmFresco_%02d", s));
    }
  }
  histCScm->Write("histCScm");
  histCScmDeg->Write("histCScmDeg");
  histCSlab->Write("histCSlab");
  histCSlabDeg->Write("histCSlabDeg");
  histCSlabVScm->Write("histCSlabVScm");
  histCSlabVScmDeg->Write("histCSlabVScmDeg");

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


