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
  
  Float_t fhxout[14]={0.0};
  
  Int_t oedoMass=0, oedoCharge=0;
  Float_t oedoE=10.0;    // in MeV/u
  Float_t oedoX=0.0, oedoY=0.0, oedoZ=0.0; // position in mm
  Float_t oedoA=0.0, oedoB=0.0; // angle in mrad
  Float_t oedoTheta=0.0, oedoPhi=0.0; // in rad
  Int_t oedoNoEvents=0;
   
  if(info->HaveOedoSimFileName()){
    cout << "Opening beam profile file ..." << endl;
    fileBeamProfile=TFile::Open(info->fOedoSimFileName, "read");

    if(!fileBeamProfile){
      cout << "Error! OEDO beam simulation file given, but not found! " << endl;
      cout << "Please check the file name in 'beam_profile_file_oedo' or comment out this line!" << endl;
      return 0;
    }

    cout << "Getting Tree ..." << endl;
    treeBeamProfile=(TTree*)fileBeamProfile->Get("Events");
    
    if(!treeBeamProfile){
      cout << "Tree 'Events' not found in root file!" << endl;
      return 0;
    }

    treeBeamProfile->SetBranchAddress("fhxout", fhxout);

    oedoNoEvents=treeBeamProfile->GetEntries();
    cout << oedoNoEvents << " events found in tree " << endl;

    //treeBeamProfile->GetEvent(1);
    //
    //oedoMass=(Int_t)fhxout[0];
    //oedoE=fhxout[4];
    //oedoX=fhxout[10];
    //oedoY=fhxout[11];
    //oedoA=fhxout[12];
    //oedoB=fhxout[13];

    //cout << "Mass " << oedoMass << ", E " << oedoE << ", X " << oedoX << ", Y " << oedoY << ", A " << oedoA << ", B " << oedoB << endl;

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
//  Double_t x=0.0, // in mm 
//           y=0.0, // in mm
//           z=0.0; // in mm
  Double_t e=0.0, // energy in MeV 
           t=0.0, // theta in rad
           p=0.0; // phi in rad
  Int_t eventNumber=0, pdgID=2212;

  TTree *events = new TTree();
  events->Branch("eventNumber", &eventNumber, "eventNumber/I");
  events->Branch("pdgID",&pdgID, "pdgID/I");
//  events->Branch("x",&x, "x/D");
//  events->Branch("y",&y, "y/D");
//  events->Branch("z",&z, "z/D");
  events->Branch("energy",&e, "energy/D");
  events->Branch("theta",&t, "theta/D");
  events->Branch("phi",&p, "phi/D");
  events->Branch("beamMassNumber", &oedoMass, "beamMassNumber/I");
  events->Branch("beamChargeNumber", &oedoCharge, "beamChargeNumber/I");
  events->Branch("beamEnergy", &oedoE, "beamEnergy/F");
  events->Branch("beamX", &oedoX, "beamX/F");
  events->Branch("beamY", &oedoY, "beamY/F");
  events->Branch("beamZ", &oedoZ, "beamZ/F");
  events->Branch("beamA", &oedoA, "beamA/F");
  events->Branch("beamB", &oedoB, "beamB/F");
  events->Branch("beamTheta", &oedoTheta, "beamTheta/F");
  events->Branch("beamPhi", &oedoPhi, "beamPhi/F");


  //printf("Got energy %f, theta %f\n", en, th);
  
  if(info->fNumberEvents<0){
    printf("Error! Invalid number of events given: %i\nAboring...\n",info->fNumberEvents);
    abort();
  }else{
    printf("Start generating events.\n");
  }

  for(Int_t i=0; i<info->fNumberEvents; i++){

    eventNumber=i;

    if(i%1000==0){
      printf("%i events generated (%i requested)\n", i, info->fNumberEvents); 
    }


    // get beam information
    if(info->HaveOedoSimFileName()){ // todo: check if any information is needed from tree before reading it

      fhxout[0]=0.0;

      while((((Int_t)fhxout[0]) != 132) || (((Int_t)fhxout[1]) != 50)  ){

        Int_t rndmEvnt = (Int_t)randomizer->Uniform(oedoNoEvents);

        treeBeamProfile->GetEvent(rndmEvnt);
      }
      
      oedoMass=(Int_t)fhxout[0];
      oedoCharge=(Int_t)fhxout[1];

      if(info->ProfileBeamE()){
        oedoE=fhxout[4];
      }

      if(info->ProfileBeamX()){
        oedoX=fhxout[10];
      }

      if(info->ProfileBeamY()){
        oedoY=fhxout[11];
      }

      if(info->ProfileBeamA()){
        oedoA=fhxout[12];
      }

      if(info->ProfileBeamB()){
        oedoB=fhxout[13];
      }



    }


    // calculate theta and phi of beam at target
    TVector3 vBeam(0.0, 0.0, 1.0);
    vBeam.RotateX(oedoA/1000.0);
    vBeam.RotateY(oedoB/1000.0);

    oedoTheta=vBeam.Theta();
    oedoPhi=vBeam.Phi();





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
    t=randomizer->Uniform(180.0);
    e=graph[g]->Eval(t);


    t*=TMath::Pi()/180.0;
    
    // phi uniform
    p=randomizer->Uniform(2.0*TMath::Pi());
    
    TVector3 direction(0.0, 0.0, -1.0);
    direction.SetMag(1.0);

    direction.SetPhi(p);
    direction.SetTheta(t);


    pdgID=2212; //proton

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


