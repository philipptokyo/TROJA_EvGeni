

#include "FrescoPlotter.hh"


using namespace std;


FrescoPlotter::FrescoPlotter(){
    cout << "Error: wrong constructor used for FrescoPlotter! InputInfo needs to be passed!" << endl;
}


FrescoPlotter::FrescoPlotter(InputInfo* info){
  fInfo=info;
}

FrescoPlotter::~FrescoPlotter(){
  delete fInfo;
}


TH1F* FrescoPlotter::CreateHistogram(){
	
	
	
	
	
	
	
	cout << "Welcome to plotter of fresco output" << endl;
	
	
	
	Bool_t verbose=false;
	//verbose=true;
	
	
	const Int_t columns=23;
        const Int_t arraySize = 200;
	
	
	
// 	cout << "Input file: " << info->infilename << endl;
// 	cout << "Root file:  " << info->rootfilename << endl;
	
	
	char cTemp[columns][100];
	string line;
	
	
	
	//variables to read from text file
	Float_t angle[arraySize]={-1.0};
	Float_t crossSection[arraySize]={-1.0};
	
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
		
		
		//if((strcmp(cTemp[0],"CROSS")==0) && (strcmp(cTemp[1],"SECTIONS")==0) && (strcmp(cTemp[2],"FOR")==0) && (strcmp(cTemp[3],"OUTGOING")==0) && (strcmp(cTemp[4],"96Sr")==0) && (strcmp(cTemp[6],"p")==0) && (strcmp(cTemp[8],"state")==0) && (strcmp(cTemp[10],"1")==0)){
		//if((strcmp(cTemp[0],"CROSS")==0) && (strcmp(cTemp[1],"SECTIONS")==0) && (strcmp(cTemp[2],"FOR")==0) && (strcmp(cTemp[3],"OUTGOING")==0) && (strcmp(cTemp[4],"96Sr")==0) && (strcmp(cTemp[6],"p")==0) && (strcmp(cTemp[8],"state")==0) && (strcmp(cTemp[10],"2")==0)){
		if((strcmp(cTemp[0],"CROSS")==0) && (strcmp(cTemp[1],"SECTIONS")==0) && (strcmp(cTemp[2],"FOR")==0) && (strcmp(cTemp[3],"OUTGOING")==0) && (strcmp(cTemp[4],"13C")==0) && (strcmp(cTemp[6],"207Pb")==0) && (strcmp(cTemp[8],"state")==0) && (strcmp(cTemp[10],"1")==0)){
			
                        if(verbose){
                          cout << "Found 96Sr (d,p)" << endl;
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
                                  angle[entry] = atof(cTemp[0]);
                                  crossSection[entry] = atof(cTemp[4]);
                                  if(verbose){
                                    cout << "Extracted: entry number " << entry << ", angle " << angle[entry] << ", cross section " << crossSection[entry] << endl;
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

        Float_t deg2rad = TMath::Pi()/180.0;
        Float_t min = angle[0]*deg2rad;
        Float_t max = (angle[entry-1]+(angle[entry-1]-angle[0])/entry)*deg2rad;
	
        TH1F *histCS=new TH1F("CS","Cross Sections", entry, min, max);
        for(Int_t e=0; e<entry; e++){
          histCS->SetBinContent(e+1,crossSection[e]);
        }
//	histCS->Write("histCS");
	
//	histCS->Draw();
	
//	outfile->Close();
	
	
	
	
	
	
	
	return histCS;
}


