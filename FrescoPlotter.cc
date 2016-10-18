

#include "FrescoPlotter.hh"


using namespace std;


FrescoPlotter::FrescoPlotter(){
    cout << "Error: wrong constructor used for FrescoPlotter! InputInfo needs to be passed!" << endl;
}


FrescoPlotter::FrescoPlotter(InputInfo* info){
  fInfo=info;
  fNumberOfStates=-1;

}

FrescoPlotter::~FrescoPlotter(){
  delete fInfo;
}



//void FrescoPlotter::CreateHistograms(){
//  Int_t fileIndex=600;
//  CreateHistograms(fileIndex);
//}



//void FrescoPlotter::CreateHistograms(Int_t fileIndex){
void FrescoPlotter::CreateHistograms(){
	
	
	cout << "Welcome to plotter of fresco output" << endl;
	
	
	
	Bool_t verbose=false;
	//verbose=true;
	
	
	const Int_t columns=23;
        const Int_t arraySize = 200;
        //const Int_t energyBinsMax=20; // in header
        const Float_t deg2rad = TMath::Pi()/180.0;
        //const Float_t deg2rad = 1.0;

        Int_t energyBin=-1, energyBinsFresco=0; // counter for energy bins and its value from fresco file
        Float_t energyMin=0.0;
        Float_t energyMax=0.0;
	
        ////todo: put this in header
        //char proj[10], targ[10], reco[10], ejec[10];
        //Int_t projA=0, ejecA=0;
        //Int_t numberOfStates=0;
        ////const Int_t maxNumberOfStates=10;
        //Float_t stateEnergy[maxNumberOfStates]={0.0};

	
	
// 	cout << "Input file: " << info->infilename << endl;
// 	cout << "Root file:  " << info->rootfilename << endl;
	
	
	char cTemp[columns][100];
	string line;
	
	
	
	//variables to read from text file
	//Float_t angle[maxNumberOfStates+1][arraySize]={{-1.0}};
	//Float_t crossSection[maxNumberOfStates+1][arraySize]={{-1.0}};
	Float_t angle[energyBinsMax][maxNumberOfStates+1][arraySize]={{{-1.0}}};
	Float_t crossSection[energyBinsMax][maxNumberOfStates+1][arraySize]={{{-1.0}}};
	
        ifstream fin;
	
	//check if input file exists
        char fileName[500];
        sprintf(fileName,"%s",fInfo->fOutFileNameFresco);
        sprintf(fileName,"%s",fInfo->fOutFileNameFresco);

	//ifstream FileTest(fInfo->fOutFileNameFresco);
	ifstream FileTest(fileName);
	if(!FileTest){
		//cout << "Can not open file '" << fInfo->fOutFileNameFresco << "'! Does it exist?" << endl;
		cout << "Can not open file '" << fileName << "'! Does it exist?" << endl;
		abort();
	}else{
		//printf("Opening file '%s'\n", fInfo->fOutFileNameFresco);
		//fin.open(fInfo->fOutFileNameFresco);
		printf("Opening file '%s'\n", fileName);
		fin.open(fileName);
	}
	
	
	//variables for writing to root file
	
	const ULong64_t stopper=1000000; //
	
	if(verbose){cout << "Stopping after " << stopper << " lines." << endl;}
	
	
	//create the root file 
	// for the histograms obtained from talys
//	TFile *outfile = new TFile(argv[2], "recreate");
        
        ULong64_t counter = 0;	
        Int_t entry = 0;
	cout << "Parsing of fresco file" << endl;
	//reading input file
	while(!fin.eof())  {
		
		getline(fin,line);
		
		if(verbose){
			cout << "Parsing line " << counter << " (max " << stopper << "):" << endl;
			cout << line.c_str() << endl;
		}
		
		
		//reset values
		for(Int_t i=0; i<columns; i++){sprintf(cTemp[i]," ");}
		
		
		
		//get rows and write them to chars
		std::istringstream iss(line);
		for(Int_t i=0; i<columns; i++){
			iss >> cTemp[i];
		}
	        
                // get reaction	
                if(counter==3 ){
                  printf("FrescoPlotter: found reaction: %s\n", cTemp[0]);
                  Int_t pos[3]={-1};
                  string sReac = cTemp[0];
                  
                  pos[0]=sReac.find("(");
                  pos[1]=sReac.find(",");
                  pos[2]=sReac.find(")");
                  
                  sReac.copy(fProj,pos[0],0);
                  sReac.copy(fTarg,pos[1]-pos[0], pos[0]+1);
                  sReac.copy(fReco,pos[2]-pos[1], pos[1]+1);
                  sReac.copy(fEjec,sReac.size()-pos[2], pos[2]+1);
                  
                  fProj[pos[0]]='\0';
                  fTarg[pos[1]-pos[0]-1]='\0';
                  fReco[pos[2]-pos[1]-1]='\0';
                  fEjec[sReac.size()-pos[2]-1]='\0';
                  
                  printf("FrescoPlotter: Projectile = %s, Target = %s, Recoiled particle = %s, Ejectile = %s\n", fProj, fTarg, fReco, fEjec);

                  // get number of nucleons in projectile
                  fProjA = atoi(fProj);
                  fEjecA = atoi(fEjec);

                  //fBeamEnergy = atof(cTemp[5]);
                  //printf("Beam energy is %f MeV/u, %f MeV \n", fBeamEnergy, fBeamEnergy*fProjA);

                }

                // get the beam energy given in the fresco input file

                if( (strcmp(cTemp[0],"0Lab.")==0) && (strcmp(cTemp[1],"ENERGY")==0) && (strcmp(cTemp[2],"ranges")==0) && (strcmp(cTemp[3],":")==0) ){
                  getline(fin,line);
                  getline(fin,line);
                  //printf("Debug: I am at line '%s'\n", line.c_str());

		  std::istringstream iss(line);
		  for(Int_t i=0; i<columns; i++){
		  	iss >> cTemp[i];
		  }
                  energyMin=atof(cTemp[1]);
                  energyMax=atof(cTemp[3]);
                  energyBinsFresco = atoi(cTemp[5]) + 1; // fresco gives the intervals
                  printf("FrescoPlotter: found beam energy distribution: %d bins from %f MeV to %f MeV\n", energyBinsFresco, energyMin, energyMax);

                  if(energyBinsFresco > energyBinsMax){
                    printf("FrescoPlotter: found more energy bins than allowed (%d)! Please increase the number of bins 'energyBinsMax' in FrescoPlotter.hh\n", energyBinsMax);
                    abort();
                  }


                  //abort();
                }



                if( (strcmp(cTemp[0],"INCOMING")==0) && (strcmp(cTemp[1],fProj)==0) && (strcmp(cTemp[2],";")==0) && (strcmp(cTemp[3],"LABORATORY")==0) && (strcmp(cTemp[4],fProj)==0) && (strcmp(cTemp[5],"ENERGY")==0) ){
                  
                  energyBin++;
                  
                  fBeamEnergy[energyBin] = atof(cTemp[7])/fProjA;
                  printf("FrescoPlotter: found beam energy bin %d: %f MeV/u, %f MeV \n", energyBin, fBeamEnergy[energyBin], fBeamEnergy[energyBin]*fProjA);
		
                  if(verbose){
		    printf("energy bin is increased to %d\n", energyBin);	
                  }
                }

                // get number of states and their energies in outgoing channel
                if((strcmp(cTemp[0],"***********")==0) && (strcmp(cTemp[1],"PARTITION")==0) && (strcmp(cTemp[2],"NUMBER")==0) && (strcmp(cTemp[3],"2")==0)){

                  getline(fin,line);
                  getline(fin,line);
                  //printf("Line: %s\n", line.c_str());

		  std::istringstream iss2(line);
                  for(Int_t i=0; i<columns; i++){
                    iss2 >> cTemp[i];
                    //printf("%s ", cTemp[i]);
                  }

                  if(strcmp(cTemp[6],"STATES=")==0){
                    fNumberOfStates=atoi(cTemp[7]);
                  }else{
                    fNumberOfStates=atoi(cTemp[6]);
                  }


//printf("cTemp[6] = %s\n", cTemp[6]);
//                  string sStates = cTemp[6];
//                  cTemp[6][sStates.size()-1]='\0'; // remove the "F" in the end
//printf("cTemp[6] = %s\n", cTemp[6]);

//                  fNumberOfStates=atoi(cTemp[6]);
                    printf("FrescoPlotter: Found %i states \n", fNumberOfStates);

                  if(fNumberOfStates>maxNumberOfStates){
                    printf("Error: Found more states (%d) than array size allows (%d). Please incease the array size (in InputInfo)\n", fNumberOfStates, maxNumberOfStates);
                    abort();
                  }

                  while(strcmp(cTemp[0],"************************************************************************************************************************************")!=0){
                    getline(fin,line);
                    std::istringstream iss3(line);
                    for(Int_t i=0; i<columns; i++){
                      iss3 >> cTemp[i];
                      //printf("%s ", cTemp[i]);
                    }

                    fStateEnergy[0]=0.0; // index 0 is elasitc


                    if((strcmp(cTemp[1],"J=")==0) && (strcmp(cTemp[4],"E=")==0)){
                      fStateEnergy[atoi(cTemp[0])] = atof(cTemp[5]); 
                        printf("Found state %d with energy %f\n", atoi(cTemp[0]), fStateEnergy[atoi(cTemp[0])]);
                    }


                    counter++;
                    if(counter>stopper){
                      cout << "Reached maximum number of lines (" << counter << " > " << stopper << ")" << endl;
                      break;
                    }
                  }


                } // end of number of states and energies
		
		if((strcmp(cTemp[0],"CROSS")==0) && (strcmp(cTemp[1],"SECTIONS")==0) && (strcmp(cTemp[2],"FOR")==0) && (strcmp(cTemp[3],"OUTGOING")==0 )){
                        
                        entry = 0;
		        
                        Int_t index=-1;
                        
                        if((strcmp(cTemp[4], fProj)==0) && (strcmp(cTemp[6], fTarg)==0) && (strcmp(cTemp[8],"state")==0) && (strcmp(cTemp[10],"1")==0)){
                          index = 0; // elastic
                          if(verbose){
                            printf("FrescoPlotter: found elastic scattering cross sections\n");
                          }
                        }
                        if((strcmp(cTemp[4], fEjec)==0) && (strcmp(cTemp[6], fReco)==0) && (strcmp(cTemp[8],"state")==0) ){
                          index = atoi(cTemp[10]);
                          if(verbose){
                            printf("FrescoPlotter: found transfer to state %d cross sections\n", index);
                          }
                        }
                        
			
			//the next line is not of interest
			getline(fin,line);
			
			
			
			
			
			//loop over lines of interest (population information)
			sprintf(cTemp[0],"dummy");
// 			lines=0;
			
			while((strcmp(cTemp[0]," ")!=0)){
				
				
				//reset chars
				for(Int_t i=0; i<columns; i++){sprintf(cTemp[i]," ");}
				
				//collect population information
				getline(fin,line);
 				//cout << line.c_str() << endl;
				
				std::istringstream iss(line);
				for(Int_t i=0; i<columns; i++){
					iss >> cTemp[i];
 					if(verbose){cout << cTemp[i] << " ";}
					
				}
				
                                if(strcmp(cTemp[1],"deg.:")==0){
                                  angle[energyBin][index][entry] = atof(cTemp[0]);
                                  crossSection[energyBin][index][entry] = atof(cTemp[4]);
                                  if(verbose){
                                    cout << "Extracted: entry number " << entry << ", angle " << angle[energyBin][index][entry] << ", cross section " << crossSection[energyBin][index][entry] << endl;
                                  }
                                  entry++;
                                }
				
				
				
				
			} //loop over population information
			
			
		}
		
	        
                
                	
		//count lines
		counter++;
		if(counter>stopper){
			cout << "Reached maximum number of lines (" << counter << " > " << stopper << ")" << endl;
			break;
		}
		

	} //end of reading input file
	
        energyBin++; // started counting from -1, due to convinence with fresco file structure


	fin.close();
        
        fBeamEnergyBins = energyBin;
        if(energyBinsFresco==1){
          fBeamEnergyBins=energyBinsFresco;
        }
	cout << counter << " lines processed. End of input file. \nFound " << energyBin << " entries for beam energy each with "  << entry << " entries for cross section" << endl;
	
	
 //       outfile->cd();	
        	
	
	cout << "Creating Histograms for cross sections" << endl;

        //Float_t angleMin = angle[0][0][0]*deg2rad;
        //Float_t angleMax = (angle[0][0][entry-1]+(angle[0][0][entry-1]-angle[0][0][0])/entry)*deg2rad;
        Float_t angleMin = angle[0][0][0];
        Float_t angleMax = (angle[0][0][entry-1]+(angle[0][0][entry-1]-angle[0][0][0])/entry);
	

        for(Int_t e=0; e<energyBin; e++){
          for(Int_t s=0; s<maxNumberOfStates+1; s++){
            sprintf(cTemp[0], "CSdO_energy%02d_state%02d", e, s);
            if(s==0){
              sprintf(cTemp[1], "dSigma/dOmega CM for beam energy %02d: %f AMeV, elastic scattering", e, fBeamEnergy[e]);
            }else{
              sprintf(cTemp[1], "dSigma/dOmega CM for beam energy %02d: %f AMeV, transfer to state %d", e, fBeamEnergy[e], s);
            }
            fHist1dCSdO[e][s]=new TH1F(cTemp[0] ,cTemp[1] , entry, angleMin, angleMax);
            fHist1dCSdO[e][s]->GetXaxis()->SetTitle("#vartheta_{CM}");
            fHist1dCSdO[e][s]->GetYaxis()->SetTitle("d#sigma/d#Omega in mb/sr");

            for(Int_t es=0; es<entry; es++){
              fHist1dCSdO[e][s]->SetBinContent(es+1, crossSection[e][s][es]);  // ds/dO
            }
            
            sprintf(cTemp[0], "CSdT_energy%02d_state%02d", e, s);
            if(s==0){
              sprintf(cTemp[1], "dSigma/dTheta CM for beam energy %02d: %f AMeV, elastic scattering", e, fBeamEnergy[e]);
            }else{
              sprintf(cTemp[1], "dSigma/dTheta CM for beam energy %02d: %f AMeV, transfer to state %d", e, fBeamEnergy[e], s);
            }
            fHist1dCSdT[e][s]=new TH1F(cTemp[0] ,cTemp[1] , entry, angleMin, angleMax);
            fHist1dCSdT[e][s]->GetXaxis()->SetTitle("#vartheta_{CM}");
            fHist1dCSdT[e][s]->GetYaxis()->SetTitle("d#sigma/#vartheta in mb/rad");

            for(Int_t es=0; es<entry; es++){
              //fHist1dCSdT[e][s]->SetBinContent(es+1, crossSection[e][s][es]);  // ds/dO
              fHist1dCSdT[e][s]->SetBinContent(es+1, crossSection[e][s][es]*TMath::Sin(angle[e][s][es]*deg2rad)*2.0*TMath::Pi()); // ds/dt
              //if(angle[e][s][es]>0){printf("Angle %f, ds/dO %f, ds/dt %f\n", angle[e][s][es], crossSection[e][s][es], crossSection[e][s][es]*TMath::Sin(angle[e][s][es]*deg2rad));}
            }

          }
        }


        for(Int_t s=0; s<maxNumberOfStates+1; s++){
          sprintf(cTemp[0], "CSdO_state%02d", s);
          if(s==0){
            sprintf(cTemp[1], "dSigma/dOmega CM for elastic scattering");
          }else{
            sprintf(cTemp[1], "dSigma/dOmega CM for transfer to state %d", s);
          }

          fHist2dCSdO[s] = new TH2F(cTemp[0], cTemp[1], entry, angleMin, angleMax, energyBin, energyMin, energyMax);
          for(Int_t e=0; e<energyBin; e++){
            for(Int_t es=0; es<entry; es++){
              Int_t bb=fHist2dCSdO[s]->GetBin(es, e);
              fHist2dCSdO[s]->SetBinContent(bb, crossSection[e][s][es]);
              //fHistCS2d[s]->SetBinContent(bb, crossSection[e][s][es]*TMath::Sin(angle[e][s][es]*deg2rad));
              fHist2dCSdO[s]->GetXaxis()->SetTitle("#vartheta_{CM}");
              fHist2dCSdO[s]->GetYaxis()->SetTitle("E_{beam} in MeV");
              fHist2dCSdO[s]->GetZaxis()->SetTitle("d#sigma/d#Omega in mb/sr");
              //fHistCS2d[s]->GetZaxis()->SetTitle("d#sigma/#vartheta in mb/rad");
            }
          }
          

          sprintf(cTemp[0], "CSdT_state%02d", s);
          if(s==0){
            sprintf(cTemp[1], "dSigma/dTheta CM for elastic scattering");
          }else{
            sprintf(cTemp[1], "dSigma/dTheta CM for transfer to state %d", s);
          }

          fHist2dCSdT[s] = new TH2F(cTemp[0], cTemp[1], entry, angleMin, angleMax, energyBin, energyMin, energyMax);
          for(Int_t e=0; e<energyBin; e++){
            for(Int_t es=0; es<entry; es++){
              Int_t bb=fHist2dCSdT[s]->GetBin(es, e);
              //fHist2dCSdT[s]->SetBinContent(bb, crossSection[e][s][es]);
              fHist2dCSdT[s]->SetBinContent(bb, crossSection[e][s][es]*TMath::Sin(angle[e][s][es]*deg2rad)*2.0*TMath::Pi());
              fHist2dCSdT[s]->GetXaxis()->SetTitle("#vartheta_{CM}");
              fHist2dCSdT[s]->GetYaxis()->SetTitle("E_{beam} in MeV");
              //fHist2dCSdT[s]->GetZaxis()->SetTitle("d#sigma/d#Omega in mb/sr");
              fHist2dCSdT[s]->GetZaxis()->SetTitle("d#sigma/#vartheta in mb/rad");
            }
          }

        }


//	histCS->Write("histCS");
	
//	histCS->Draw();
	
//	outfile->Close();
	
	
	return;

}

void FrescoPlotter::UpdateInput(){
  /**
  This function overwrites the level scheme given in InputInfo
  according to the data extracted from fresco output
  */
  
  if(fNumberOfStates==-1){
    printf("Warning: Update of input information about excited states is not possible, because updated information are not available. (Maybe fresco output file hasn't been read, yet?)\nSkipping the update...\n");
    return;
  }
  
  printf("Info: FrescoPlotter overwrites the excited states given in input textfile!\n");

  fInfo->fNumberOfStates = fNumberOfStates;
//  for(Int_t s=0; s<fNumberOfStates; s++){   // is this wrong?
//    fInfo->fStateEnergy[s] = fStateEnergy[s+1]; // is this wrong?
  for(Int_t s=0; s<fNumberOfStates+1; s++){
    fInfo->fStateEnergy[s] = fStateEnergy[s];
  }

  if(fBeamEnergyBins>2){
    //fInfo->fBeamEnergy = fBeamEnergy[(Int_t)(fBeamEnergyBins/2)]; 
  }else{
    fInfo->fBeamEnergy = fBeamEnergy[0]; 
  }
  printf("Beam energy set to %f AMeV (%f MeV)\n", fInfo->fBeamEnergy, fInfo->fBeamEnergy * fInfo->fProjA);

  if((fProjA != fInfo->fProjA) || (fEjecA != (fInfo->fProjA + fInfo->fTargetA - fInfo->fLightA))){
    printf("Error: Projectile and/or Ejectile are not the same in input text file and fresco output text file:\n");
    printf("Input:  projectile A = %d, ejectile A = %d\n", fInfo->fProjA, (fInfo->fProjA + fInfo->fTargetA - fInfo->fLightA));
    printf("Fresco: projectile A = %d, ejectile A = %d\n", fProjA, fEjecA);
    abort();
  }

  return;

}



TGraph* FrescoPlotter::HistCMToGraphLab(TH1F* hist){
  Float_t beamEnergy = fInfo->fBeamEnergy;
  return HistCMToGraphLab(hist, beamEnergy);
}

TGraph* FrescoPlotter::HistCMToGraphLab(TH1F* hist, Float_t beamEnergyMeV){
  char* massFile = (char*)"/home/philipp/programme/makeEvents/mass.dat";

  Int_t projA=fInfo->fProjA;
  Int_t projZ=fInfo->fProjZ;
  Int_t targetA=fInfo->fTargetA;
  Int_t targetZ=fInfo->fTargetZ;
  Int_t lightA=fInfo->fLightA;
  Int_t lightZ=fInfo->fLightZ;

  Nucleus* proj = new Nucleus(projZ, projA-projZ, massFile);
  Nucleus* targ = new Nucleus(targetZ, targetA-targetZ, massFile);
  Nucleus* reco = new Nucleus(lightZ, lightA-lightZ, massFile);
  Nucleus* ejec = new Nucleus(projZ, projA-projZ+(targetA-lightA), massFile);

  Kinematics* reaction = new Kinematics(proj, targ, reco, ejec, beamEnergyMeV, 0.0);

  TGraph* graf = new TGraph();

  for(Int_t b=0; b<hist->GetXaxis()->GetNbins(); b++){
    Float_t tCm = hist->GetXaxis()->GetBinCenter(b)/180.0*TMath::Pi();
    Float_t cs = hist->GetBinContent(b);

    Float_t tlab = reaction->Angle_cm2lab(reaction->GetVcm(2), TMath::Pi()-tCm);
    graf->SetPoint(b, tlab*180.0/TMath::Pi(), cs);

  }

  return graf;
}

