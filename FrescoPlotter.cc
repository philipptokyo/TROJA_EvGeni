

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


void FrescoPlotter::CreateHistograms(){
	
	
	
	
	
	
	
	cout << "Welcome to plotter of fresco output" << endl;
	
	
	
	Bool_t verbose=false;
	//verbose=true;
	
	
	const Int_t columns=23;
        const Int_t arraySize = 200;
        const Float_t deg2rad = TMath::Pi()/180.0;
	
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
	Float_t angle[maxNumberOfStates+1][arraySize]={{-1.0}};
	Float_t crossSection[maxNumberOfStates+1][arraySize]={{-1.0}};
	
        ifstream fin;
	
	//check if input file exists
	ifstream FileTest(fInfo->fOutFileNameFresco);
	if(!FileTest){
		cout << "Can not open file '" << fInfo->fOutFileNameFresco << "'! Does it exist?" << endl;
		abort();
	}else{
		printf("Opening file '%s'\n", fInfo->fOutFileNameFresco);
		fin.open(fInfo->fOutFileNameFresco);
	}
	
	
	//variables for writing to root file
	
	const ULong64_t stopper=10000; //
	
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
                  printf("Got reaction: %s\n", cTemp[0]);
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
                  
                  printf("proj = %s, targ = %s, reco = %s, ejec = %s\n", fProj, fTarg, fReco, fEjec);

                  // get number of nucleons in projectile
                  fProjA = atoi(fProj);
                  fEjecA = atoi(fEjec);

                  fBeamEnergy = atof(cTemp[5]);
                  printf("Beam energy is %f MeV/u, %f MeV \n", fBeamEnergy, fBeamEnergy*fProjA);

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

                  fNumberOfStates=atoi(cTemp[6]);
                  printf("Found %i states \n", fNumberOfStates);
                  if(fNumberOfStates>maxNumberOfStates){
                    printf("Error: Found more states (%d) than array size allows (%d). Please incease the array size (FrescoPlotter)\n", fNumberOfStates, maxNumberOfStates);
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
                        }
                        if((strcmp(cTemp[4], fEjec)==0) && (strcmp(cTemp[6], fReco)==0) && (strcmp(cTemp[8],"state")==0) ){
                          index = atoi(cTemp[10]);
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
                                  angle[index][entry] = atof(cTemp[0]);
                                  crossSection[index][entry] = atof(cTemp[4]);
                                  if(verbose){
                                    cout << "Extracted: entry number " << entry << ", angle " << angle[index][entry] << ", cross section " << crossSection[index][entry] << endl;
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
	
	fin.close();
	
	cout << counter << " lines processed. End of input file. \nFound " << entry << " entries for cross section" << endl;
	
	
 //       outfile->cd();	
        	
	
	cout << "Creating Histograms for cross sections" << endl;

        Float_t min = angle[0][0]*deg2rad;
        Float_t max = (angle[0][entry-1]+(angle[0][entry-1]-angle[0][0])/entry)*deg2rad;
	
        //fHistCSelast = new TH1F("CSelast","Cross Sections, elastic scattering", entry, min, max);
        //for(Int_t e=0; e<entry; e++){
        //  fHistCSelast->SetBinContent(e+1, crossSection[0][e]);
        //}


        for(Int_t s=0; s<maxNumberOfStates+1; s++){
          sprintf(cTemp[0], "CSstate%02d", s);
          if(s==0){
            sprintf(cTemp[1], "Cross Sections, elastic scattering");
          }else{
            sprintf(cTemp[1], "Cross Sections, transfer to state %d", s);
          }
          fHistCS[s]=new TH1F(cTemp[0] ,cTemp[1] , entry, min, max);

          for(Int_t e=0; e<entry; e++){
            fHistCS[s]->SetBinContent(e+1, crossSection[s][e]);
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
  for(Int_t s=0; s<fNumberOfStates; s++){
    fInfo->fStateEnergy[s] = fStateEnergy[s+1];
  }

  fInfo->fBeamEnergy = fBeamEnergy;
  printf("Beam energy set to %f MeV/u (%f MeV)\n", fInfo->fBeamEnergy, fInfo->fBeamEnergy * fInfo->fProjA);

  if((fProjA != fInfo->fProjA) || (fEjecA != (fInfo->fProjA + fInfo->fTargetA - fInfo->fLightA))){
    printf("Error: Projectile and/or Ejectile are not the same in input text file and fresco output text file:\n");
    printf("Input:  projectile A = %d, ejectile A = %d\n", fInfo->fProjA, (fInfo->fProjA + fInfo->fTargetA - fInfo->fLightA));
    printf("Fresco: projectile A = %d, ejectile A = %d\n", fProjA, fEjecA);
    abort();
  }

  return;

}
