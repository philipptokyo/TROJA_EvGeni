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
//  TApplication *theApp=new TApplication("theApp",  0, 0);
  
  
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
  TFile* outfile = new TFile(info->fRootfilename, "recreate");
  outfile->cd();
  histAll->Write();

  


  // generate events and write them to tree
//   Double_t x=0.0, 
//            y=0.0, 
//            z=0.0; 
  Double_t e=0.0, // energy in MeV 
           t=0.0, // theta in rad
           p=0.0; // phi in rad
  Int_t eventNumber=0, pdgID=2212;

  TTree *events = new TTree();
  events->Branch("eventNumber", &eventNumber, "eventNumber/I");
//   events->Branch("x",&x, "x/D");
//   events->Branch("y",&y, "y/D");
//   events->Branch("z",&z, "z/D");
  events->Branch("energy",&e, "energy/D");
  events->Branch("theta",&t, "theta/D");
  events->Branch("phi",&p, "phi/D");
  events->Branch("pdgID",&pdgID, "pdgID/I");


  //printf("Got energy %f, theta %f\n", en, th);
  
  if(info->fNumberEvents<0){
    printf("Error! Invalid number of events given: %i\nAboring...\n",info->fNumberEvents);
    abort();
  }else{
    printf("Start generating events.\n");
  }

  for(Int_t i=0; i<info->fNumberEvents; i++){

    eventNumber=i;

    if(i%100000==0){
      printf("%i events generated (%i requested)\n", i, info->fNumberEvents); 
    }

    t=181.0;
    while(t>180.0){
      histAll->GetRandom2(t,e);
    }

    t*=TMath::Pi()/180.0;
    
    p=randomizer->Uniform(2.0*TMath::Pi());
    
    TVector3 direction(0.0, 0.0, -1.0);
    direction.SetMag(1.0);

    direction.SetPhi(p);
    direction.SetTheta(t);

    //x=direction.X();
    //y=direction.Y();
    //z=direction.Z();

    //printf("Got energy %lf, theta %lf, phi %lf,  x %lf, y %lf, z %lf\n", e, t, p, x, y, z);

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
  
  
  return 0;
}


