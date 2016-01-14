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
  

  //Float_t fhxout[14]={0.0}; // full sim file
  
  //Int_t beamMass=0, beamCharge=0;
  Float_t beamE=info->fBeamEnergy;    // in MeV/u
  Float_t beamX=0.0, beamY=0.0, beamZ=0.0; // position in mm
  Float_t beamA=0.0, beamB=0.0; // angle in mrad
  Float_t beamTheta=0.0, beamPhi=0.0; // in rad
  Int_t beamNoEvents=0;
  



  //Int_t projA=132, projZ=50, targetA=2, targetZ=1, lightA=1, lightZ=1;
  //Int_t numberOfStates=2;
  //Float_t maxExEnergy=5.0;
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
  for(Int_t s=0; s<numberOfStates; s++){
    stateEnergy[s]=(Float_t)s * maxExEnergy / (Float_t)(numberOfStates-1);
    printf("  state %d, energy %f\n", s, stateEnergy[s]);
    if(!(info->ProfileBeamE()) && stateEnergy[s]>beamE){
      cout << "Error: state energy is larger than beam energy!" << endl;
      return 0;
    }
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

  //printf("ejec A=%d, mass=%f\n", ejec->GetA(), ejec->GetMass());
  //printf("proj A=%d, mass=%f\n", proj->GetA(), proj->GetMass());
  //printf("reco A=%d, mass=%f\n", reco->GetA(), reco->GetMass());
  //printf("targ A=%d, mass=%f\n", targ->GetA(), targ->GetMass());

  projMass=proj->GetMass();
  targetMass=targ->GetMass();
  lightMass=reco->GetMass();
  heavyMass=ejec->GetMass();


  Kinematics* reaction[maxNumberOfStates];
  for(Int_t s=0; s<numberOfStates; s++){
     reaction[s] = new Kinematics(proj, targ, reco, ejec, beamE*projA, stateEnergy[s]);
  }

  qValue=reaction[0]->GetQValue();
  
  //printf("Elab is %f\n", reaction->ELab(90.0/180.0*TMath::Pi(), 2));
















//  cout << "Stopped!" << endl;
//  return 0;
























//  char tempFileName[300], tempCommand[500];
//  //sprintf(tempFileName, "/home/philipp/programme/makeEvents/deleteme.root");
//  sprintf(tempFileName, "%s", info->fOutFileNameReaction);
//  sprintf(tempCommand,"/home/philipp/programme/reaction/Reaction -p %d %d -t %d %d -tr %d %f %d -e %f -o %s ", projA-projZ, projZ, targetA-targetZ, targetZ, targetA-lightA,  maxExEnergy, numberOfStates, beamE*projA, tempFileName);
//
//  cout << "Executing command: " << tempCommand << endl;
//  
//   
//  FILE *fp;
//  char path[1000];
// 
//  /* Open the command for reading. */
//  fp = popen(tempCommand, "r");
//  if (fp == NULL) {
//    printf("Failed to run command\n" );
//    return 0;
//  }
//  
//  char temp[10][500];
//  /* Read the output a line at a time - output it. */
//  while (fgets(path, sizeof(path)-1, fp) != NULL) {
//    //printf("%s", path);
// 
//    istringstream iss(path);
//    for(Int_t i=0; i<10; i++){iss >> temp[i];}
//    
//    // collecting information about the masses
//    if(strcmp(temp[0],"recoil")==0 && strcmp(temp[1],"Z")==0 && strcmp(temp[3],"N")==0){
//      lightMass = atof(temp[5]);
//    }
//    if(strcmp(temp[0],"ejectile")==0 && strcmp(temp[1],"Z")==0 && strcmp(temp[3],"N")==0){
//      heavyMass = atof(temp[5]);
//    }
//    if(strcmp(temp[0],"Qvalue")==0){
//      qValue = atof(temp[1]);
//    }
//    if(strcmp(temp[0],"proj")==0 && strcmp(temp[2],"targ")==0){
//      projMass = atof(temp[1]);
//      targetMass = atof(temp[3]);
//    }
//  }
//
//  /* close */
//  pclose(fp); 
   
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

  
//  TFile* infile = TFile::Open(info->fOutFileNameReaction,"read");
//  
//  if(!infile){
//   cout << "Rootfile from reactions not found!" << endl;
//   return 0;
//  }
//  
//  Bool_t foundGraph = true;
//  Int_t graphCounter=0;
//  const Int_t graphMax=100;
//
//  TGraph* graph[graphMax];
//  for(Int_t i=0; i<graphMax; i++){
//    graph[i] = new TGraph();
//  }
//  
//  
//  char tmp[20];
//
//  while(foundGraph){
//    
//    sprintf(tmp,"EvsTh_lab_%i",graphCounter);
//    graph[graphCounter] = (TGraph*)infile->Get(tmp);
//    
//    //cout << "Looking for TGraph " << tmp << endl;
//
//    if(!graph[graphCounter]){
//      foundGraph=false;
//    }
//    else{
//      //cout << "Found TGraph " << tmp << endl;
//    
//      graphCounter++;
//    }
//
//    
//    // stopper
//    if(graphCounter>graphMax){
//      cout << "Error while finding graphs in rootfile! Aborting..." << endl;
//      abort();
//    }
//
//  }
//
//  cout << "Found " << graphCounter << " TGraphs" << endl;
  
  
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
    //treeBeamProfile=(TTree*)fileBeamProfile->Get("Events"); // full sim file
    treeBeamProfile=(TTree*)fileBeamProfile->Get("events"); // shortened sim file
    
    if(!treeBeamProfile){
      //cout << "Tree 'Events' not found in root file!" << endl;
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
  
  
  
  
  
  
//   // create a histogram
//   // from the graphs/splines  
//   TH1F* hist0 = graph[0]->GetHistogram();
// 
//   Int_t nbins = hist0->GetXaxis()->GetNbins();
//   Float_t ymax=0.0;
// 
//   // get information for a generic bining of the energy (y-axix)
//   for(Int_t b=0; b<nbins; b++){
// 
//     Float_t center = hist0->GetXaxis()->GetBinCenter(b);
//     Float_t content=graph[0]->Eval(center);
// 
//     //cout << "Bin " << b << ", center " << center << ", content " << content << endl;
//     
//     //get maximum energy
//     if(content>ymax){
//       ymax=content;
//     }
// 
//   }
//   
//   // generic bining for the energy (y-axis)
//   Float_t ymaxtmp=ymax, oom=0.0;
//   while(ymaxtmp>10.){
//     ymaxtmp/=10.;
//     oom+=1.0;
//   }
//   
//   ymaxtmp=(Int_t)ymaxtmp + 1.0;
//   // cout << "ymax " << ymax << ", ymaxtmp " << ymaxtmp << ", oom " << oom  << endl;
//   
//   // choose 100 keV binning for the energy
//   ymax=ymaxtmp*TMath::Power(10.0,oom);
//   
//   // cout << "binning to " << ymax << endl;
//   
//   TH2F* histAll=new TH2F("histAll","E vs. Th_lab, all Graphs",
//       nbins,hist0->GetXaxis()->GetBinLowEdge(1), hist0->GetXaxis()->GetBinLowEdge(nbins+1),
//       (Int_t)(ymax*100.0), 0.0, ymax
//       );
//   
//   
// //   for(Int_t i=0; i<nbins; i++){
// //     cout << "Bin " << i << " hist0 lower " << hist0->GetXaxis()->GetBinLowEdge(i) << ", histAll " << histAll->GetXaxis()->GetBinLowEdge(i) << endl;
// //   }
//   
//   
//   
//   // fill the 2d histogram with all graphs
//   for(Int_t b=0; b<nbins; b++){
//     
//     for(Int_t g=0; g<graphCounter; g++){
// 
//       Float_t th=histAll->GetXaxis()->GetBinCenter(b);
//       histAll->Fill(th,graph[g]->Eval(th));
// 
//     }
//     
//   }
//   
//   // histAll->Draw("colz");
// 
//   histAll->GetXaxis()->SetRangeUser(0.0, 180.0);  
//   
//   // save histogram to output rootfile
//   outfile->cd();
//   histAll->Write();
//   cout << "All graphs converted to one histogram!" << endl;
  









  // generate events and write them to tree

  // light ejectile position and flight direction
//  Double_t lightX=0.0, // in mm 
//           lightY=0.0, // in mm
//           lightZ=0.0; // in mm
  Double_t lightEnergy=0.0, // energy in MeV 
           lightTheta=0.0, // theta in rad
           lightPhi=0.0; // phi in rad
  Int_t lightPdgId=2212; //proton
  //Int_t heavyPdgId=1000501330; // 133Sn
  Int_t eventNumber=0;

  TTree *events = new TTree();
  events->Branch("eventNumber", &eventNumber, "eventNumber/I");
  
  // light ejectile data
  events->Branch("lightPdgId",&lightPdgId, "lightPdgId/I");
//  events->Branch("lightX",&lightX, "lightX/D"); // is/should be taken from beamX
//  events->Branch("lightY",&lightY, "lightY/D");
//  events->Branch("lightZ",&lightZ, "lightZ/D");
  events->Branch("lightEnergy",&lightEnergy, "lightEnergy/D");
  events->Branch("lightTheta",&lightTheta, "lightTheta/D");
  events->Branch("lightPhi",&lightPhi, "lightPhi/D");
  
  // projectile data
  //events->Branch("beamMassNumber", &projA, "beamMassNumber/I");
  //events->Branch("beamChargeNumber", &projZ, "beamChargeNumber/I");
  events->Branch("beamEnergy", &beamE, "beamEnergy/F");
  events->Branch("beamX", &beamX, "beamX/F");
  events->Branch("beamY", &beamY, "beamY/F");
  events->Branch("beamZ", &beamZ, "beamZ/F");
  events->Branch("beamA", &beamA, "beamA/F");
  events->Branch("beamB", &beamB, "beamB/F");
  events->Branch("beamTheta", &beamTheta, "beamTheta/F");
  events->Branch("beamPhi", &beamPhi, "beamPhi/F");


  // determine PDG ID of light ejectile
  // todo: if the light ejectile is not the same for each event,
  //       than this part needs to be moved to the event loop
  char lightPdgIdName[20];
  sprintf(lightPdgIdName,"1000%02d%03d0",lightZ,lightA);
  lightPdgId=atoi(lightPdgIdName);
  //printf("Outgoing PDG ID: char %s, int %i\n", tmp, lightPdgId);
  
  //printf("Got energy %f, theta %f\n", en, th);
  
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
    
//    if(info->ProfileBeamE()){
//     if(i%100==0){
//       printf("%i events generated (%i requested)\n", i, info->fNumberEvents); 
//     }
//    }else{
     if(i%10000==0){
       printf("%i events generated (%i requested)\n", i, info->fNumberEvents);
     }
//    }


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

//    TFile* tempFile;
//    TGraph* tempGraph[numberOfStates];

    if(info->ProfileBeamE()){

      //if(beamE<maxExEnergy){
      //  cout << "Beam energy = " << beamE << " is too low! Will be set to " << info->fBeamEnergy << " MeV/u! (todo: fix this!!!!!!!!!!)" << endl;
      //  notProcessed++;
      //  beamE=info->fBeamEnergy;
      //  // bad hack! todo!
      //}

      // run shell command / execute 'reaction'
//      sprintf(tempFileName, "/home/philipp/programme/makeEvents/132Sndp_deleteme.root");
//
//      //sprintf(tempCommand,"/home/philipp/programme/reaction/Reaction -p 82 50 -t 1 1 -tr 1 %f %d -e %f -o %s",maxExEnergy, numberOfStates, beamE*(Float_t)beamMass, tempFileName);
//      //sprintf(tempCommand,"/home/philipp/programme/reaction/Reaction -p 82 50 -t 1 1 -tr 1 %f %d -e %f -o %s > /dev/null",maxExEnergy, numberOfStates, beamE*(Float_t)beamMass, tempFileName);
//      //sprintf(tempCommand,"/home/philipp/programme/reaction/Reaction -p %d %d -t %d %d -tr %d %f %d -e %f -o %s ", projA-projZ, projZ, targetA-targetZ, targetZ, targetA-lightA,  maxExEnergy, numberOfStates, beamE*(Float_t)projA, tempFileName);
//      sprintf(tempCommand,"/home/philipp/programme/reaction/Reaction -p %d %d -t %d %d -tr %d %f %d -e %f -o %s > /dev/null ", projA-projZ, projZ, targetA-targetZ, targetZ, targetA-lightA,  maxExEnergy, numberOfStates, beamE*(Float_t)projA, tempFileName);
//      
//      //cout << "Executing command: " << tempCommand << endl;
//      
//      system(tempCommand);
//      
//      tempFile = TFile::Open(tempFileName,"read");
//      if(!tempFile){
//        cout << "Temporary file is not existing: " << tempFileName << endl;
//        return 0;
//      }
//      //TGraph* tempGraph[numberOfStates];
//      for(Int_t j=0; j<numberOfStates; j++){
//        tempGraph[j]=new TGraph();
//        //char tmp[20];
//        sprintf(tmp,"EvsTh_lab_%i",j);
//        tempGraph[j] = (TGraph*)tempFile->Get(tmp);
//        
//        if(!tempGraph[j]){
//          cout << "Graph not found: " << tmp << endl;
//          return 0;
//
//        }
//      }

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
      lightEnergy=reaction[s]->ELab(lightTheta/180.0*TMath::Pi(),2);
    }


    
//    // at the moment: only uniform theta distribution
//    // todo: add physics here!!!!!
//    lightTheta=randomizer->Uniform(180.0);
//
//    if(info->ProfileBeamE()){
//      Int_t g=(Int_t)randomizer->Uniform(numberOfStates);
//      lightEnergy=tempGraph[g]->Eval(lightTheta);
//    }else{
//      // sample energy from graph
//      // choose a random graph
//      //Int_t g=(Int_t)randomizer->Uniform(graphCounter);  
//      ////if(g<0 || g>=graphCounter){
//      ////  cout << "Invalid graph number " << g << endl;
//      ////  return 0;
//      ////}
//      //lightEnergy=graph[g]->Eval(lightTheta);
//
//
//      Int_t s=(Int_t)randomizer->Uniform(numberOfStates);  
//      lightEnergy=reaction[s]->ELab(lightTheta/180.0*TMath::Pi(),2);
//
//
//    }

    
    //lightEnergy=graph[g]->Eval(lightTheta);             
    //lightEnergy=tempGraph[0]->Eval(lightTheta);

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

    events->Fill();

//    for(Int_t j=0; j<numberOfStates; j++){
//      delete tempGraph[j];
//    }
//    if(info->ProfileBeamE()){
//      tempFile->Close();
//      system(Form("rm -f %s", tempFileName));
//      
//      // profiling is very time consuming a.t.m.
//      // save events for the case of any crash
//      //if(i%1000==0){
//      //  cout << "Info: Writing events to file ..." << endl;
//      //  outfile->cd();
//      //  events->Write("events");
//      //}
//    }

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


