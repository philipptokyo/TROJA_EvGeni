/**
 * 
 * main.cc of makeEvents from Reactions code
 * 
 * 
 **/





#include "main.h"
#include "InputInfo.h"

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
  	cout << "Please give 1 input argument: text file with input information, e.g. './makeEvents input/input.txt' " << endl; 
  	return 0;
  }
  
  
  
  InputInfo *info = new InputInfo();
  
  //get information from textfile
  info->parse(argv[1]);


  // make some cross checks of the inputs
  if(!(info->HaveOedoSimFileName()) && 
      ((info->ProfileBeamE()) || (info->ProfileBeamX()) || (info->ProfileBeamY()) || (info->ProfileBeamA()) || (info->ProfileBeamB()) )){
    cout << "ERROR! Beam profiling requested, but no root file for beam profile is given!" << endl;
    return 0;
  }

  if(info->ProfileBeamE()){
    cout << "WARNING! Profiling of beam energy is requested. " << endl;
    cout << "This option is not yet implemented with respect to the angular distribution of the light particle" << endl;
    cout << "Omitting this option!" << endl;
    cout << endl;
  }















  TRandom3 *randomizer=new TRandom3();  
  randomizer->SetSeed(0);
  
  TFile* infile = TFile::Open(info->fInfilenameFromReaction,"read");
  
  TFile* outfile = new TFile(info->fRootfilename, "recreate");
  
  
  if(!infile){
    
   cout << "Rootfile not found!" << endl;
   return 0;
  
  }
  
  Bool_t foundGraph = true;
  Int_t graphCounter=0;
  const Int_t graphMax=1000;

  TGraph* graph[graphMax];
  for(Int_t i=0; i<graphMax; i++){
    graph[i] = new TGraph();
  }
  
  while(foundGraph){
    
    char tmp[20];
    sprintf(tmp,"EvsTh_lab_%i",graphCounter);
    graph[graphCounter] = (TGraph*)infile->Get(tmp);
    
    //cout << "Looking for TGraph " << tmp << endl;

    if(!graph[graphCounter]){
      foundGraph=false;
    }
    else{
      //cout << "Found TGraph " << tmp << endl;
    
      graphCounter++;
    }

    
    // stopper
    if(graphCounter>100000){
      cout << "Error while finding graphs in rootfile! Aborting..." << endl;
      abort();
    }

  }

  cout << "Found " << graphCounter << " TGraphs" << endl;
  
  
  // get the histograms with beam information 
  // from Oedo simulation by Matsushita
  TFile* fileBeamProfile;
  TTree* treeBeamProfile;
  
  //Float_t fhxout[14]={0.0}; // full sim file
  
  Int_t beamMass=0, beamCharge=0;
  Float_t beamE=10.0;    // in MeV/u
  Float_t beamX=0.0, beamY=0.0, beamZ=0.0; // position in mm
  Float_t beamA=0.0, beamB=0.0; // angle in mrad
  Float_t beamTheta=0.0, beamPhi=0.0; // in rad
  Int_t beamNoEvents=0;



  if(!(info->HaveOedoSimFileName()) && 
      ((info->ProfileBeamE()) || (info->ProfileBeamX()) || (info->ProfileBeamY()) || (info->ProfileBeamA()) || (info->ProfileBeamB()) )){
    cout << "ERROR! Beam profiling requested, but no root file for beam profile is given!" << endl;
    return 0;
  }

   
  if(info->HaveOedoSimFileName()){
    cout << "Opening beam profile file ..." << endl;
    fileBeamProfile=TFile::Open(info->fOedoSimFileName, "read");

    if(!fileBeamProfile){
      cout << "Error! OEDO beam simulation file given, but not found! " << endl;
      cout << "Please check the file name in 'beam_profile_file_oedo' or comment out this line!" << endl;
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
    treeBeamProfile->SetBranchAddress("mass", &beamMass); // shortened sim file
    treeBeamProfile->SetBranchAddress("energy", &beamE);  // shortened sim file
    treeBeamProfile->SetBranchAddress("x", &beamX);       // shortened sim file
    treeBeamProfile->SetBranchAddress("y", &beamY);       // shortened sim file
    treeBeamProfile->SetBranchAddress("a", &beamA);       // shortened sim file
    treeBeamProfile->SetBranchAddress("b", &beamB);       // shortened sim file

    beamNoEvents=treeBeamProfile->GetEntries();
    cout << beamNoEvents << " events found in tree " << endl;


  }
  
  
  
  
  
  
  // create a histogram
  // from the graphs/splines  
  TH1F* hist0 = graph[0]->GetHistogram();

  Int_t nbins = hist0->GetXaxis()->GetNbins();
  Float_t ymax=0.0;

  // get information for a generic bining of the energy (y-axix)
  for(Int_t b=0; b<nbins; b++){

    Float_t center = hist0->GetXaxis()->GetBinCenter(b);
    Float_t content=graph[0]->Eval(center);

    //cout << "Bin " << b << ", center " << center << ", content " << content << endl;
    
    //get maximum energy
    if(content>ymax){
      ymax=content;
    }

  }
  
  // generic bining for the energy (y-axis)
  Float_t ymaxtmp=ymax, oom=0.0;
  while(ymaxtmp>10.){
    ymaxtmp/=10.;
    oom+=1.0;
  }
  
  ymaxtmp=(Int_t)ymaxtmp + 1.0;
  // cout << "ymax " << ymax << ", ymaxtmp " << ymaxtmp << ", oom " << oom  << endl;
  
  // choose 100 keV binning for the energy
  ymax=ymaxtmp*TMath::Power(10.0,oom);
  
  // cout << "binning to " << ymax << endl;
  
  TH2F* histAll=new TH2F("histAll","E vs. Th_lab, all Graphs",
      nbins,hist0->GetXaxis()->GetBinLowEdge(1), hist0->GetXaxis()->GetBinLowEdge(nbins+1),
      (Int_t)(ymax*100.0), 0.0, ymax
      );
  
  
//   for(Int_t i=0; i<nbins; i++){
//     cout << "Bin " << i << " hist0 lower " << hist0->GetXaxis()->GetBinLowEdge(i) << ", histAll " << histAll->GetXaxis()->GetBinLowEdge(i) << endl;
//   }
  
  
  
  // fill the 2d histogram with all graphs
  for(Int_t b=0; b<nbins; b++){
    
    for(Int_t g=0; g<graphCounter; g++){

      Float_t th=histAll->GetXaxis()->GetBinCenter(b);
      histAll->Fill(th,graph[g]->Eval(th));

    }
    
  }
  
  // histAll->Draw("colz");

  histAll->GetXaxis()->SetRangeUser(0.0, 180.0);  
  
  // save histogram to output rootfile
  outfile->cd();
  histAll->Write();
  cout << "All graphs converted to one histogram!" << endl;
  









  // generate events and write them to tree

  // light ejectile position and flight direction
//  Double_t lightX=0.0, // in mm 
//           lightY=0.0, // in mm
//           lightZ=0.0; // in mm
  Double_t lightEnergy=0.0, // energy in MeV 
           lightTheta=0.0, // theta in rad
           lightPhi=0.0; // phi in rad
  Int_t lightPdgId=2212;
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
  events->Branch("beamMassNumber", &beamMass, "beamMassNumber/I");
  events->Branch("beamChargeNumber", &beamCharge, "beamChargeNumber/I");
  events->Branch("beamEnergy", &beamE, "beamEnergy/F");
  events->Branch("beamX", &beamX, "beamX/F");
  events->Branch("beamY", &beamY, "beamY/F");
  events->Branch("beamZ", &beamZ, "beamZ/F");
  events->Branch("beamA", &beamA, "beamA/F");
  events->Branch("beamB", &beamB, "beamB/F");
  events->Branch("beamTheta", &beamTheta, "beamTheta/F");
  events->Branch("beamPhi", &beamPhi, "beamPhi/F");


  //printf("Got energy %f, theta %f\n", en, th);
  
  if(info->fNumberEvents<0){
    printf("Error! Invalid number of events given: %i\nAboring...\n",info->fNumberEvents);
    abort();
  }else{
    printf("Start generating events.\n");
  }

  for(Int_t i=0; i<info->fNumberEvents; i++){

    eventNumber=i;

    if(i%10000==0){
      printf("%i events generated (%i requested)\n", i, info->fNumberEvents); 
    }


    // get beam information
    if(info->HaveOedoSimFileName()){ // todo: check if any information is needed from tree before reading it

      // the following lines are only needed if the full oedo sim file is taken
      // some selections are needed


      // fhxout[0]=0.0;

      // while((((Int_t)fhxout[0]) != 132) || (((Int_t)fhxout[1]) != 50)  ){

      //   Int_t rndmEvnt = (Int_t)randomizer->Uniform(beamNoEvents);

      //   treeBeamProfile->GetEvent(rndmEvnt);
      // }
      // 
      // beamMass=(Int_t)fhxout[0];
      // beamCharge=(Int_t)fhxout[1];

      // if(info->ProfileBeamE()){
      //   beamE=fhxout[4];
      // }

      // if(info->ProfileBeamX()){
      //   beamX=fhxout[10];
      // }

      // if(info->ProfileBeamY()){
      //   beamY=fhxout[11];
      // }

      // if(info->ProfileBeamA()){
      //   beamA=fhxout[12];
      // }

      // if(info->ProfileBeamB()){
      //   beamB=fhxout[13];
      // }



      // shortened oedo sim file contains only good events
      Int_t rndmEvnt = (Int_t)randomizer->Uniform(beamNoEvents);
      treeBeamProfile->GetEvent(rndmEvnt);



    }else{ // no beam profile
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


    if(info->ProfileBeamE()){
      // todo: this option needs to be implemented
      beamE=info->fBeamEnergy;
    }else{
      beamE=info->fBeamEnergy;
    }





//    t=181.0;
//    while(t>180.0){
//      histAll->GetRandom2(t,e);
//    }

    
    // sample energy from graph
    // choose a random graph
    Int_t g=(Int_t)randomizer->Uniform(graphCounter);
    
    if(g<0 || g>=graphCounter){
      cout << "Invalid graph number " << g << endl;
      return 0;
    }
    
    // at the moment: only uniform theta distribution
    // todo: add physics here!!!!!
    lightTheta=randomizer->Uniform(180.0);
    lightEnergy=graph[g]->Eval(lightTheta);

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

    lightPdgId=2212; //proton

    events->Fill();
  }



  outfile->cd();

  events->Write("events");

  outfile->Close();
  
  printf("Events written to file '%s'\n", info->fRootfilename);
  printf("Thank you and good by!\n");
  
  // if histograms shall be plotted, run theApp
  // otherwise program closes
//  theApp->Run();
  delete theApp;  
  
  return 0;
}


