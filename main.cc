/**
 * 
 * main.cc of generator_talys
 * 
 * 
 **/




// #include <iostream>
// #include <stdio.h>      /* printf */
// #include <stdlib.h>     /* atof */
// #include <math.h>       /* sin cos */

// #include <sstream>

#include "main.h"
#include "InputInfo.h"

#include "LibPerso.h"

using namespace std;




Int_t main(Int_t argc, char **argv){
	
	
	
	
	
	
	
// 	TApplication *theApp=new TApplication("theApp",  0, 0);
	
	
	cout << "Welcome to converter from talys output to rootfile" << endl;
	
	
	cout << "Input arguments: " << endl;
	for (int i = 1; i < argc; i++) {
		cout << argv[i] << endl;
	}
	
	if(argc!=2){
		cout << "Please give 1 input argument: text file with input information, e.g. './generator_talys input/input.txt' " << endl; 
		return 0;
	}
	
	
	
	InputInfo *info = new InputInfo();
	
	//get information from textfile
	info->parse(argv[1]);
	
	
// 	//check if dicebox file exists
// 	TFile *fileDicebox=new TFile();
// 	
// 	
// 	
// 	ifstream FileTestDicebox(info->rootfilenameDicebox);
// 	if(!FileTestDicebox){
// 		cout << "Can not open file '" << info->rootfilenameDicebox << "'! Does it exist?" << endl;
// 		return 0;
// 	}else{
// // 		gDirectory->pwd();
// 		cout << "Opening file '" << info->rootfilenameDicebox << "'" << endl;
// 		fileDicebox = TFile::Open(info->rootfilenameDicebox,"read");
// // 		cout << "file open " << endl;
// 	}
// 	
// 	
// 	
// 	cout << "Getting tree obtained from Dicebox" << endl;
// 	
// 	TTree *treeGammas;
// 	if(fileDicebox->GetListOfKeys()->Contains("tree")){
// 		treeGammas = (TTree*)fileDicebox->Get("tree");
// 	}else{
// 		cout << "Tree 'tree' with gamma data not found" << endl;
// 		abort();
// 	}
// 	
	
	
	
	
	
	
	
	Bool_t verbose=false;
	if(info->verbosity > 0){verbose=true;}
	
	
	const Int_t columns=23;
	const Int_t rows=100;
	const Int_t nuclei=7;
	
	
	
// 	cout << "Input file: " << info->infilename << endl;
// 	cout << "Root file:  " << info->rootfilename << endl;
	
	
	char cTemp[columns][100];
	string line;
	
	
	//variables to read from text file
	Float_t spin[nuclei][columns]={{NAN}};
	Int_t nucleusZ[nuclei]={0}, nucleusN[nuclei]={0};
	char nucleusName[nuclei][100]={{""}};
	Float_t nucleusCS[nuclei]={0.0};
	
	Float_t popul[nuclei][columns][rows]={{{NAN}}}; //[id][spin][entries]
	Float_t neutronEnergies[nuclei][2][rows]={{{NAN}}}; //[id][0=energy, 1=cs][entries]
	
	//counters
	Int_t nucl=0; //count no of isotopes/nuclei in file
	Int_t linesPopul[nuclei]={0}, linesNeutronEnergies[nuclei]={0}; //no of rows
	
	ifstream fin;
	
	//check if input file exists
	ifstream FileTest(info->infilenameFromTalys);
	if(!FileTest){
		cout << "Can not open file '" << info->infilenameFromTalys << "'! Does it exist?" << endl;
		return 0;
	}else{
		printf("Opening file '%s'\n", info->infilenameFromTalys);
		fin.open(info->infilenameFromTalys);
	}
	
	
	//variables for writing to root file
	
	//aux
	ULong64_t counter=0;
// 	const ULong64_t stopper=999999999; //max lines to read (6 lines per event in dicebox output)
// 	const ULong64_t stopper=25; //for testing
	const ULong64_t stopper=info->maxLines; //
	
	if(verbose){cout << "Stopping after " << stopper << " lines." << endl;}
	
	Bool_t exclusiveData=false;
	
	
	
	//create the root file 
	// for the histograms obtained from talys
	TFile *outfile = new TFile(info->rootfilenameTalys, "recreate");

	
	cout << "Parsing of talys file" << endl;
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
		
		
		if((strcmp(cTemp[0],"Population")==0) && (strcmp(cTemp[1],"of")==0) && (strcmp(cTemp[2],"Z=")==0) && (strcmp(cTemp[7],"before")==0) && (strcmp(cTemp[8],"decay:")==0)){
			
			if(nucl>=nuclei){
				cout << "Found more nuclei (" << nucl+1 << ") than array size allows (" << nuclei << ")! Increase value for 'nuclei'!" << endl;
				abort();
			}
			
// 			//reset variables
// 			for(Int_t i=0; i<columns; i++){
// 				
// 				spin[i]=NAN;
// 				
// 				for(Int_t j=0; j<rows; j++){
// 					popul[nucl][i][j]=NAN;
// 				}
// 				
// 			}
			
			//this line contains information which nucleus is populated 
			nucleusZ[nucl]=atoi(cTemp[3]);
			nucleusN[nucl]=atoi(cTemp[5]);
// 			sprintf(nucleusName[nucl],cTemp[6]);
			strcpy(nucleusName[nucl],cTemp[6]);
			
			
			if(verbose){cout << "Z=" << nucleusZ[nucl] << ", N=" << nucleusN[nucl] << ", name is " << nucleusName[nucl] << endl;}
			
			//the next two lines are not of interest
			getline(fin,line);
			getline(fin,line);
			
			//the following line contains information about the spin and parity of the populated state
			getline(fin,line);
			std::istringstream iss(line);
			for(Int_t i=0; i<columns; i++){
				iss >> cTemp[i];
				if(verbose){cout << cTemp[i] << " ";}
				
			}
			if(verbose){cout << endl;}
			
			if(verbose){cout << "Extracted spins: " << endl;}
			for(Int_t i=3; i<columns; i++){
				
				spin[nucl][i]=atof(cTemp[2*i-2]);
				
				if(verbose){cout << spin[nucl][i] << " ";}
			}
			if(verbose){cout << endl;}
			
			//one more empty line
			getline(fin,line);
			
			
			
			
			//loop over lines of interest (population information)
			sprintf(cTemp[0],"dummy");
// 			lines=0;
			
			while((strcmp(cTemp[0]," ")!=0)){
				
				if(linesPopul[nucl]>=rows){
					cout << "ERROR: reached maximum number of bins (" << rows << ") in population spectrum! Please increase value 'rows'!" << endl;
					abort();
				}
				
				//reset chars
				for(Int_t i=0; i<columns; i++){sprintf(cTemp[i]," ");}
				
				//collect population information
				getline(fin,line);
// 				cout << line.c_str() << endl;
				
				std::istringstream iss(line);
				for(Int_t i=0; i<columns; i++){
					iss >> cTemp[i];
// 					if(verbose){cout << cTemp[i] << " ";}
					
				}
// 				if(verbose){cout << endl;}
				
				if(verbose){cout << "Converted population:" << endl;}
				for(Int_t i=0; i<columns; i++){
					popul[nucl][i][linesPopul[nucl]]=atof(cTemp[i]);
					if(verbose){cout << popul[nucl][i][linesPopul[nucl]] << " ";}
				}
				if(verbose){cout << endl;}
				
				linesPopul[nucl]++;
				
				
				
			} //loop over population information
			
			linesPopul[nucl]--; //last line is void
			
			if(verbose){cout << "Found " << linesPopul[nucl] << " lines" << endl;}
			
			
			
			
			nucl++;
			
			
		}
		
		//get total production cross sections
		//information about daughter nuclei need to be already collected 
		if((strcmp(cTemp[0],"4.")==0) && (strcmp(cTemp[1],"Residual")==0) && (strcmp(cTemp[2],"production")==0) && (strcmp(cTemp[3],"cross")==0) && (strcmp(cTemp[4],"sections")==0)){
			
			//ignore the following 6 lines
			for(Int_t i=0; i<6; i++){getline(fin,line);}
			
			while(!(strcmp(cTemp[0],"b.")==0) && !(strcmp(cTemp[1],"per")==0) && !(strcmp(cTemp[2],"mass")==0)){
				
				getline(fin,line);
				std::istringstream iss(line);
				for(Int_t i=0; i<columns; i++){
					iss >> cTemp[i];
// 					cout << cTemp[i] << " ";
					
				}
// 				cout << endl;
				
				if((strcmp(cTemp[4],"0")==0)){
					
					for(Int_t n=0; n<nucl; n++){
						if((strcmp(cTemp[2],nucleusName[n])==0)){
							nucleusCS[n]=atof(cTemp[3]);
							if(verbose){cout << nucleusName[n] << ":" << nucleusCS[n] << endl;}
						}
					}
					
				}
				
			}
			
			
		} //total cross sections
		
		
		
		//getting neutron emission spectra
		//ignore inclusive spectra
		if((strcmp(cTemp[0],"6b.")==0) && (strcmp(cTemp[1],"Exclusive")==0) && (strcmp(cTemp[2],"spectra")==0)){exclusiveData=true;}
		
		
		if((strcmp(cTemp[0],"Emitted")==0) && (strcmp(cTemp[1],"particles")==0) && exclusiveData){
			
			getline(fin,line);
			getline(fin,line);
			
			
			std::istringstream iss(line);
			for(Int_t i=0; i<columns; i++){
				iss >> cTemp[i];
				
			}
				
			//select neutron data
			if((strcmp(cTemp[1],"0")==0) && (strcmp(cTemp[2],"0")==0) && (strcmp(cTemp[3],"0")==0) && (strcmp(cTemp[4],"0")==0) && (strcmp(cTemp[5],"0")==0)){
				
				//select neutron channel
				if(atof(cTemp[0])>0 && atof(cTemp[0])<nucl){
					
					
					
					//get index of daughter nucleus
					Int_t nucl2=0;
					for(nucl2=0; nucl2<nucl; nucl2++){
						if(nucleusN[0]-nucleusN[nucl2]-atoi(cTemp[0]) == 0){
							if(verbose){
								cout << line << endl;
								cout << "Daughter nucleus is " << nucleusName[nucl2] << endl;
								cout << endl;
							}
							break;
						}
					}
					
					for(Int_t i=0; i<5; i++){getline(fin,line);}
					
					
					while(!(strcmp(cTemp[0]," ")==0)){
						
						if(linesNeutronEnergies[nucl2]>=rows){
							cout << "ERROR: reached maximum number of lines (" << rows << ") in neutron energy spectrum! Please increase value 'rows'!" << endl;
							abort();
						}
						
						
						sprintf(cTemp[0], " ");
						
						getline(fin,line);
						
						std::istringstream iss(line);
						if(verbose){cout << linesNeutronEnergies[nucl2] << ": ";}
						for(Int_t i=0; i<columns; i++){
							iss >> cTemp[i];
							if(verbose){cout << "'" << cTemp[i] << "'" << " ";}
							
						}
						if(verbose){cout << endl;}
						
						neutronEnergies[nucl2][0][linesNeutronEnergies[nucl2]]=atof(cTemp[0]);
						neutronEnergies[nucl2][1][linesNeutronEnergies[nucl2]]=atof(cTemp[2]);
						
						
						linesNeutronEnergies[nucl2]++;
						
					}
					
					linesNeutronEnergies[nucl2]--;
					if(verbose){cout << "total lines: " << linesNeutronEnergies[nucl2] << endl;}
				}
				
			}
			
		}
		
		
		//count lines
		counter++;
		if(counter>stopper){
			cout << "Reached maximum number of lines (" << counter << " > " << stopper << ")" << endl;
			break;
		}
		
		if(counter%100000==0){
			cout << counter << " lines processed. " << endl;
		}
		
	} //end of reading input file
	
	fin.close();
	
	cout << counter << " lines processed. End of input file. Found " << nucl << " nuclei." << endl;
	
	
	
	
	
	cout << "Creating Histograms for residual production cross sections" << endl;
	TH1F *histCS=new TH1F("ResProdCS","Residual Production Cross Sections (neutron removal only)",nucl,0.0,((Float_t)nucl));
	Int_t binCS=0;
	for(Int_t n=0; n<nucl; n++){
		if(nucleusZ[n]==nucleusZ[0]){
			binCS=nucleusN[0]-nucleusN[n]+1; //bin 0 is underflow
			histCS->SetBinContent(binCS,nucleusCS[n]);
			
		}
	}
	histCS->Write("histCS");
	
	
	
	cout << "Creating Histograms for population and spins" << endl;
	
	
	TH1F *histPopul[nucl];
	TH2F *histPopulSpins[nucl];
	
	TDirectory *dir[nucl];
	TH1F *histSpins[nucl][rows];
	
	
	for(Int_t n=0; n<nucl; n++){
		
		outfile->cd();
		
		//determine binning
		Int_t nbins=(Int_t)(popul[n][1][linesPopul[n]-1]/(info->binWidth/1000.0))+1;
		if(verbose){cout << "Binning " << nucleusName[n] << ": " << nbins << " bins from 0.0 to " << popul[n][1][linesPopul[n]-1] << endl;}
		
		histPopul[n]=new TH1F(
					Form("histPopul_Z%i_N%i",nucleusZ[n],nucleusN[n]),
					Form("histPopul_Z%i_N%i_%s",nucleusZ[n],nucleusN[n],nucleusName[n]),
					nbins, 0.0, popul[n][1][linesPopul[n]-1]
					);
		
		histPopulSpins[n]=new TH2F(
					Form("histPopulSpins_Z%i_N%i",nucleusZ[n],nucleusN[n]),
					Form("histPopulSpins_Z%i_N%i_%s",nucleusZ[n],nucleusN[n],nucleusName[n]),
					nbins, 0.0, popul[n][1][linesPopul[n]-1],
					20, 0.0, 10.0
					);
		
		//fill histogram
		for(Int_t l=0; l<linesPopul[n]; l++){
			Int_t bin = histPopul[n]->Fill(popul[n][1][l]);
			histPopul[n]->SetBinContent(bin, popul[n][2][l]);
		}
		
		histPopul[n]->Write(Form("histPopul_Z%i_N%i",nucleusZ[n],nucleusN[n]));
		
		
		
		dir[n]=outfile->mkdir(Form("histSpins_Z%i_N%i",nucleusZ[n],nucleusN[n]));
		dir[n]->cd();
		
		for(Int_t l=0; l<linesPopul[n]; l++){
			
			histSpins[n][l]=new TH1F(
								Form("histSpins_Z%i_N%i_Bin%i",nucleusZ[n],nucleusN[n],(Int_t)popul[n][0][l]),
								Form("histSpins_Z%i_N%i_Bin%i_%s",nucleusZ[n],nucleusN[n],(Int_t)popul[n][0][l],nucleusName[n]),
								20, 0.0, 10.0
								);
			
			histSpins[n][l]->SetTitle(Form("%s_E=%f",nucleusName[n], popul[n][1][l]));
			
			
			for(Int_t c=0; c<columns; c++){
				
				Int_t bin=histSpins[n][l]->Fill(spin[n][c]);
				histSpins[n][l]->SetBinContent(bin, popul[n][c][l]);
				
				bin=histPopulSpins[n]->Fill(popul[n][1][l],spin[n][c]);
				histPopulSpins[n]->SetBinContent(bin,popul[n][c][l]);
				
			}
			
			histSpins[n][l]->Write(Form("histSpins_Z%i_N%i_Bin%i",nucleusZ[n],nucleusN[n],(Int_t)popul[n][0][l]));
			
		}
		
		outfile->cd();
		histPopulSpins[n]->Write(Form("histPopulSpins_Z%i_N%i",nucleusZ[n],nucleusN[n]));
	}
	
	
	
	
	cout << "Creating Histograms for neutron energies" << endl;
	TH1F *histNeutronEnergies[nucl]; //talys spectrum
	TH1F *histNeutronEnergiesGen[nucl]; //sampled spectrum
	
	for(Int_t n=1; n<nucl; n++){//n=0 is mother nucleus without neutron emission
		
		//create histograms only for neutron removal without charge changing
		if(nucleusZ[0]-nucleusZ[n]!=0){continue;}
		
// 		cout << "lines: " << linesNeutronEnergies[n] << ", energy: " << neutronEnergies[n][0][linesNeutronEnergies[n]-1] << endl;
		Int_t nbins=(Int_t)(neutronEnergies[n][0][linesNeutronEnergies[n]-1]/(info->binWidth/1000.0))+1;
		if(verbose){cout << "Binning " << nucleusName[n] << ": " << nbins << " bins from 0.0 to " << neutronEnergies[n][0][linesNeutronEnergies[n]-1] << endl;}
		
// 		if(nbins<=0){
// 			cout << "nucl=" << nucl << endl;
// 			cout << "lines: " << linesNeutronEnergies[n] << ", energy: " << neutronEnergies[n][0][linesNeutronEnergies[n]-1] << endl;
// 			cout << "Binning " << nucleusName[n] << ": " << nbins << " bins from 0.0 to " << neutronEnergies[n][0][linesNeutronEnergies[n]-1] << endl;
// 			abort();
// 		}
		
		histNeutronEnergies[n]=new TH1F(
					Form("histNeutronEnergies_Z%i_N%i",nucleusZ[n],nucleusN[n]),
					Form("histPopul_Z%i_N%i_%s",nucleusZ[n],nucleusN[n],nucleusName[n]),
					nbins, 0.0, neutronEnergies[n][0][linesNeutronEnergies[n]-1]
					);
		
		histNeutronEnergiesGen[n]=(TH1F*)histNeutronEnergies[n]->Clone();
		
		//fill histogram
		for(Int_t i=0; i<linesNeutronEnergies[n]; i++){
			Int_t bin = histNeutronEnergies[n]->Fill(neutronEnergies[n][0][i]);
			
// 			Float_t scale=1.0;
// 			if(i>0){
// 				scale=neutronEnergies[n][0][i]-neutronEnergies[n][0][i-1];
// 			}else{
// 				scale=neutronEnergies[n][0][i];
// 			}
			histNeutronEnergies[n]->SetBinContent(bin, neutronEnergies[n][1][i]);
			histNeutronEnergiesGen[n]->SetBinContent(bin, 0.0);
		}
		
		histNeutronEnergies[n]->Write(Form("histNeutronEnergies_Z%i_N%i",nucleusZ[n],nucleusN[n]));
		
		
	}
	
	
	
// 	outfile->Close();
	cout << "Root file with Talys data written." << endl;
	cout << endl;
	
	
	/**
	 * 
	 * the following part is from main.cc of sampler
	 * 
	 * maybe the codes should be splitted to several classes
	 * but at the moment this workaround is more practical
	 * because a lot of variables/infos/... have to be passed
	 * 
	 * 
	 * 
	 * */
	
	
	
	//get dicebox data file
	const Int_t maxGammaMul=10; //as in generator_dicebox
	
	TFile *fileDicebox = new TFile();
	
	ifstream FileTest2(info->rootfilenameDicebox);
	if(!FileTest2){
		cout << "Can not open file '" << info->rootfilenameDicebox << "'! Does it exist?" << endl;
		return 0;
	}else{
		gDirectory->pwd();
		cout << "Opening file '" << info->rootfilenameDicebox << "'" << endl;
		fileDicebox = TFile::Open(info->rootfilenameDicebox,"read");
		cout << "file open " << endl;
	}
	
	cout << "Getting tree obtained from Dicebox - ";
	
	TTree *treeGammas;
	if(fileDicebox->GetListOfKeys()->Contains("tree")){
		treeGammas = (TTree*)fileDicebox->Get("tree");
	}else{
		cout << "Tree 'tree' with gamma data not found" << endl;
		abort();
	}
	
	Int_t entries=treeGammas->GetEntries();
	cout << "Found " << entries << " entries in tree. " << endl;
	
	Float_t   dbInitialJP;
	Float_t   dbInitialLevel;
	Int_t     dbGammaMul;
	Float_t   dbGammaEn[maxGammaMul];
	Float_t   dbGammaEnSum;
	
	treeGammas->SetBranchAddress("initialJP", &dbInitialJP);
	treeGammas->SetBranchAddress("initialLevel", &dbInitialLevel);
	treeGammas->SetBranchAddress("gammaMul", &dbGammaMul);
	treeGammas->SetBranchAddress("gammaEn", dbGammaEn);
	treeGammas->SetBranchAddress("gammaEnSum", &dbGammaEnSum);
	
	
	
	
	
	
	Int_t events=info->numberEvents; //number of events to generate
	
	TRandom3 *randomizer=new TRandom3();
	randomizer->SetSeed(0);
	
	///todo: excitation energy has to be extracted from talys file
	Float_t initialExciationEnergy=30.0; //generated in talys
// 	Float_t initialExciationEnergy=18.0; //generated in talys
	
	Double_t spinFinal=0.0;  //spin of final state before gamma emission
	Float_t erel=0.0;
	Double_t energyFinal=0.0; //energy of final state before gamma emission
	Float_t energyNeutrons[nuclei]={0.0}; //energy of each neutron
	
	Int_t fra_A=0, fra_Z=0;
	Float_t fra_mass=NAN;
	Float_t fra_dx=0.0, fra_dy=0.0, fra_dz=0.0;
	Float_t fra_p=0.0, fra_px=0.0, fra_py=0.0, fra_pz=0.0;
	
	
	
	Int_t nNeutrons=0;     //number of neutrons emitted
	Float_t n_dx[nuclei]={NAN}, n_dy[nuclei]={NAN}, n_dz[nuclei]={NAN};
	Float_t n_p[nuclei]={NAN}, n_px[nuclei]={NAN}, n_py[nuclei]={NAN}, n_pz[nuclei]={NAN};
	
	
	Int_t gammaMul=0;
	Float_t gammaEnergy[maxGammaMul]={NAN};
	Float_t gamma_theta[maxGammaMul]={NAN}, gamma_phi[maxGammaMul]={NAN};
	Float_t gamma_dx[maxGammaMul]={NAN}, gamma_dy[maxGammaMul]={NAN}, gamma_dz[maxGammaMul]={NAN};
	Float_t gamma_px[maxGammaMul]={NAN}, gamma_py[maxGammaMul]={NAN}, gamma_pz[maxGammaMul]={NAN};
	
	//create tree with generated events
	TFile *fileEvents = new TFile(info->filenameEvents, "recreate");
	fileEvents->cd();
	
	TTree *treeEvents = new TTree("treeEvents","treeEvents");
	
	treeEvents->Branch("nNeutrons",&nNeutrons,"nNeutrons/I");
	treeEvents->Branch("energyNeutrons",energyNeutrons,Form("energyNeutrons[%i]/F",nuclei));
	
	treeEvents->Branch("energyBeforeGammaDecay",&energyFinal,"energyFinal/D"); //sampled from population spectra
	treeEvents->Branch("spinBeforeGammaDecay",&spinFinal,"spinFinal/D");
	treeEvents->Branch("eRel",&erel,"erel/F");
	
	treeEvents->Branch("fra_dx",&fra_dx,"fra_dx/F");
	treeEvents->Branch("fra_dy",&fra_dy,"fra_dy/F");
	treeEvents->Branch("fra_dz",&fra_dz,"fra_dz/F");
	treeEvents->Branch("fra_p",&fra_p,"fra_p/F");
	treeEvents->Branch("fra_px",&fra_px,"fra_px/F");
	treeEvents->Branch("fra_py",&fra_py,"fra_py/F");
	treeEvents->Branch("fra_pz",&fra_pz,"fra_pz/F");
	treeEvents->Branch("fra_mass",&fra_mass,"fra_mass/F");
	treeEvents->Branch("fra_A",&fra_A,"fra_A/I");
	treeEvents->Branch("fra_Z",&fra_Z,"fra_Z/I");
	
	treeEvents->Branch("n_dx",n_dx,Form("n_dx[%i]/F",nuclei));
	treeEvents->Branch("n_dy",n_dy,Form("n_dy[%i]/F",nuclei));
	treeEvents->Branch("n_dz",n_dz,Form("n_dz[%i]/F",nuclei));
	treeEvents->Branch("n_p",n_p,Form("n_p[%i]/F",nuclei));
	treeEvents->Branch("n_px",n_px,Form("n_px[%i]/F",nuclei));
	treeEvents->Branch("n_py",n_py,Form("n_py[%i]/F",nuclei));
	treeEvents->Branch("n_pz",n_pz,Form("n_pz[%i]/F",nuclei));
	
	treeEvents->Branch("gammaMul",&gammaMul,"gammaMul/I");
	treeEvents->Branch("gammaEnergy",gammaEnergy,Form("gammaEnergy[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_theta",gamma_theta, Form("gamma_theta[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_phi",gamma_phi, Form("gamma_phi[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_dx",gamma_dx, Form("gamma_dx[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_dy",gamma_dy, Form("gamma_dy[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_dz",gamma_dz, Form("gamma_dz[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_px",gamma_px, Form("gamma_px[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_py",gamma_py, Form("gamma_py[%i]/F",maxGammaMul));
	treeEvents->Branch("gamma_pz",gamma_pz, Form("gamma_pz[%i]/F",maxGammaMul));
	
	
	
	///todo:
	//masses have to be defined at a reasonable place
	Float_t nucleusMass[nuclei]={NAN}; //fragment masses in GeV/c2
	nucleusMass[5]=118.191493; //Sn-127
	nucleusMass[4]=119.123128; //Sn-128
	nucleusMass[3]=120.057372; //Sn-129
	nucleusMass[2]=120.989330; //Sn-130
	nucleusMass[1]=121.923697; //Sn-131
	nucleusMass[0]=122.855922; //Sn-132
// 	nucleusMass[133]=123.793125; //Sn-133
// 	nucleusMass[134]=124.729149; //Sn-134
	
	Float_t massNeutron = 0.939573;
	
	
	//ascii output for r3broot
	ofstream fout;
	fout.open("output/events.txt");
	
	
	
	
	cout << "Generating neutron and gamma decay events... (This may take a while)" << endl;
	
// 	TGraph *gPopul=new TGraph();
// 	TGraph *gSpin =new TGraph();
	
	//the sampling is done from the 2d histograms for population and spin
	
	
	TLorentzVector vNeutrons[nucl];
	for(Int_t n=0; n<nucl; n++){
		vNeutrons[n].SetXYZM(0.0, 0.0, 0.0, massNeutron);
	}
	
	
	for(Long64_t e=0; e<events; e++){
		
		if(e%10000==0){
			Float_t percent=(Float_t)e/(Float_t)events*100.0;
			cout << e << " events generated (" << percent << "%)" << endl;
		}
		
		
		//reset variables
		
		energyFinal=NAN;
		spinFinal=NAN;
		
		nNeutrons=0;
		for(Int_t i=0; i<nuclei; i++){
			
			energyNeutrons[i]=NAN;
			n_dx[i]=NAN; n_dy[i]=NAN; n_dz[i]=NAN;
			n_p[i]=NAN;
			n_px[i]=NAN; n_py[i]=NAN; n_pz[i]=NAN;
			
		}
		
		fra_dx=NAN; fra_dy=NAN; fra_dz=NAN;
		fra_p=NAN;
		fra_px=NAN; fra_py=NAN; fra_pz=NAN;
		fra_mass=NAN;
		fra_A=0;
		fra_Z=0;
		
		gammaMul=0;
		for(Int_t i=0; i<maxGammaMul; i++){
			gammaEnergy[i]=NAN;
			
			gamma_theta[i]=NAN;
			gamma_phi[i]=NAN;
			
			gamma_dx[i]=NAN; gamma_dy[i]=NAN; gamma_dz[i]=NAN;
			gamma_px[i]=NAN; gamma_py[i]=NAN; gamma_pz[i]=NAN;
			
		}
		
		
		
		//sample decay channel
		///todo: random numbers are always from the same seed; select random seed
		nNeutrons=(Int_t)histCS->GetRandom();
		
		Int_t nDecChannel=nucleusN[0]-nucleusN[nNeutrons]; //I guess introducing this variable is useless
		
		//get spin and excitation energy of populated state
// 		energyFinal=histPopul[nDecChannel]->GetRandom();
		histPopulSpins[nDecChannel]->GetRandom2(energyFinal,spinFinal);
		
		//round spins off
		if(nDecChannel%2==0){
			spinFinal=(Int_t)spinFinal; //integral spins
		}else{
			spinFinal= ((Int_t)(spinFinal*2.0))/2.0; //half-integral spins
		}
		
		fra_mass=nucleusMass[nDecChannel];
		fra_A=nucleusN[nDecChannel]+nucleusZ[nDecChannel];
		fra_Z=nucleusZ[nDecChannel];
		
		
		
		//for Lorentz boost
		TVector3 beamDir(0.0, 0.0, info->beamBeta);
		
		
		if(nDecChannel!=0){//if neutron decay takes place
			
			//determine relative energy betweeen fragment and neutrons
			Float_t Sxn = nDecChannel*massNeutron - (nucleusMass[0] - nucleusMass[nDecChannel]);
			erel = initialExciationEnergy/1000.0 - Sxn - energyFinal/1000.0; //in GeV
			
			if(verbose){cout << " Sxn=" << Sxn << " erel=" << erel << " sum=E init = " << energyFinal+Sxn+erel << endl;}
			
			
			
			
			//generate with genbod / phase space
			///todo: discuss the physics relevance of this method
			//the kinetic energies of neutrons and fragments are distributed randomly according to phase space
			TLorentzVector vErel, vFragment, W;
			
			vFragment.SetXYZM(0.0, 0.0, 0.0, nucleusMass[nDecChannel]);
			vErel.SetXYZM(0.0, 0.0, 0.0, erel);
			
			W = vFragment + vErel;
			for(Int_t nn=0; nn<nDecChannel; nn++){W += vNeutrons[nn];}
			
			
			//Lorentz boost
			W.Boost(beamDir);
			
			//todo (optional): straggling in target could be implemented here as well
			
			//define array with rest masses for TGenPhaseSpace
			Double_t masses[nuclei+1] = { 0.0 };
			masses[0]=nucleusMass[nDecChannel];
			for(Int_t nn=0; nn<nDecChannel; nn++){masses[nn+1]=massNeutron;}
			
			TGenPhaseSpace decay;
			
			//check if the decay is allowed
			Bool_t decayed = decay.SetDecay(W, nDecChannel+1, masses);
			
			if(!decayed){
				cout << "ERROR: System can not decay!" << endl;
				cout << " Sxn=" << Sxn << " erel=" << erel << " sum (= E init) = " << energyFinal+Sxn+erel << endl;
				abort();
			}
			
			//generate the actual decay
			decay.Generate(); 
			
			TLorentzVector *vProduct0; //fragment
			TLorentzVector *vProducts[nuclei]; //neutrons
			
			//get fragment data
			vProduct0 = decay.GetDecay(0);
			fra_dx=vProduct0->Px()/vProduct0->P();
			fra_dy=vProduct0->Py()/vProduct0->P();
			fra_dz=vProduct0->Pz()/vProduct0->P();
			fra_p=vProduct0->P();
			fra_px=vProduct0->Px();
			fra_py=vProduct0->Py();
			fra_pz=vProduct0->Pz();
			
			//get neutron data
			for(Int_t nn=0; nn<nDecChannel; nn++){
				vProducts[nn] = decay.GetDecay(nn+1); //first is fragment
				
				if(verbose){cout << "  Product " << nn << ": E=" << vProducts[nn]->E() << ", M=" << vProducts[nn]->M() << ", Ekin=" << vProducts[nn]->E()-vProducts[nn]->M() << endl;}
				
				
				energyNeutrons[nn]=vProducts[nn]->E()-vProducts[nn]->M();
				n_dx[nn]=vProducts[nn]->X()/vProducts[nn]->P();
				n_dy[nn]=vProducts[nn]->Y()/vProducts[nn]->P();
				n_dz[nn]=vProducts[nn]->Z()/vProducts[nn]->P();
				n_p[nn]=vProducts[nn]->P();
				n_px[nn]=vProducts[nn]->Px();
				n_py[nn]=vProducts[nn]->Py();
				n_pz[nn]=vProducts[nn]->Pz();
				
				histNeutronEnergiesGen[nDecChannel]->Fill(energyNeutrons[nn]);
				
				
			}
			
			
		}else{ //no neutron decay, gamma only
			TLorentzVector *vFragment=new TLorentzVector();
			
			vFragment->SetXYZM(0.0, 0.0, 0.0, nucleusMass[nDecChannel]);
			
			//do only the Lorentz boost
			vFragment->Boost(beamDir);
			
			fra_dx=vFragment->X()/vFragment->P();
			fra_dy=vFragment->Y()/vFragment->P();
			fra_dz=vFragment->Z()/vFragment->P();
			fra_p=vFragment->P();
			fra_px=vFragment->Px();
			fra_py=vFragment->Py();
			fra_pz=vFragment->Pz();
			
		}
		
		
		//getting gamma cascades
		Int_t rndmev=(Int_t)(randomizer->Uniform()*entries);
		treeGammas->GetEntry(rndmev);
		
		///todo: initial JP and initial energy (i.e. gamma sum energy) has to be matched
		//workaround at the moment: just rescale gamma energies and ignore JP
		
		//keep the gamma multiplicity from dicebox
		gammaMul=dbGammaMul; 
		
		//if the populated state is close to zero
		//than assume the populated state is the ground state
		if(energyFinal < (info->binWidth)/1000.0){
			gammaMul=0; // no gammas
		}
		
		///todo: this has to be fixed!
		Float_t rescale=energyFinal/dbGammaEnSum;
// 		cout << "dbGammaMul=" << dbGammaMul << " gammaMul=" << gammaMul << " dbGammaEnSum=" << dbGammaEnSum << "	energyFinal=" << energyFinal << "	rescale=" << rescale << endl;
		
		for(Int_t g=0; g<gammaMul; g++){
			
			///todo: this has to be fixed!
			//rescale the gamma energies to adjsut the gamma sum energy 
			//to the remaining excitation energy after neutron emission
			gammaEnergy[g] = dbGammaEn[g]*rescale/1000.0; //in GeV
			
			
			//sample the distribution of the gamma flight directions
			//it is assumed that the distribution is uniform in spherical coordinates
			//radius is 1.0
			gamma_theta[g]=randomizer->Uniform()*TMath::Pi();
			gamma_phi[g]=randomizer->Uniform()*TMath::Pi()*2.0-TMath::Pi();
			
			//get kartesian coordinates
			gamma_dx[g]=TMath::Sin(gamma_theta[g])*TMath::Cos(gamma_phi[g]);
			gamma_dy[g]=TMath::Sin(gamma_theta[g])*TMath::Sin(gamma_phi[g]);
			gamma_dz[g]=TMath::Cos(gamma_theta[g]);
			
			
			//doppler boost the gammas
			TLorentzVector *vGamma=new TLorentzVector(gammaEnergy[g]*gamma_dx[g], gammaEnergy[g]*gamma_dy[g], gammaEnergy[g]*gamma_dz[g], gammaEnergy[g]);
			
			vGamma->Boost(beamDir);
			
			
			//get boosted values
			//kartesian coordinates
			gamma_dx[g]=vGamma->X()/vGamma->P();
			gamma_dy[g]=vGamma->Y()/vGamma->P();
			gamma_dz[g]=vGamma->Z()/vGamma->P();
			
			gamma_px[g]=vGamma->X();
			gamma_py[g]=vGamma->Y();
			gamma_pz[g]=vGamma->Z();
			
			//Doppler boosted gamma energy
			gammaEnergy[g]=vGamma->P();
			
			//spherical coordinates
			gamma_theta[g]=TMath::ACos(gamma_dz[g]);
			gamma_phi[g]=TMath::ATan2(gamma_dy[g], gamma_dx[g]);
			
// 			//Doppler boost of gamma energies
// 			energy=1.0/(beam_gamma*(1.0-beam_beta*TMath::Cos(theta)))*gammaEnergy;
// 			if(info->beamBeta>0.0){gammaEnergy[g] = 1.0/(info->beamGamma*(1.0-info->beamBeta*TMath::Cos(gamma_theta[g])))*gammaEnergy[g];}
// 			
// 			gamma_dx[g]=randomizer->Uniform()*2.0-1.0; //uniform in the range (-1 ... +1)
// 			gamma_dy[g]=randomizer->Uniform()*2.0-1.0; //uniform in the range (-1 ... +1)
// 			gamma_dz[g]=TMath::Sqrt(gamma_dx[g]*gamma_dx[g] + gamma_dy[g]*gamma_dy[g]);
			
		}
		
		
		//finally, save the events
		treeEvents->Fill();
		
		
		//write the events to text file for r3broot
		Int_t numberOfProducts = 1+nDecChannel+gammaMul; //first is fragment
		
		//first line of the new event
		fout << e << " " << numberOfProducts << " 0. 0." << endl;
		
		//add fragment to output file
		fout << "-1 " << fra_Z << " " << fra_A << " " << fra_px << " " << fra_py << " " << fra_pz << " 0.0 0.0 0.0 " << fra_mass << endl;
		
		//add neutrons
		for(Int_t nn=0; nn<nDecChannel; nn++){
			fout << "1 0 2112 " << n_px[nn] << " " << n_py[nn] << " " << n_pz[nn] << " 0.0 0.0 0.0 " << massNeutron << endl;
		}
		//add gammas
		for(Int_t g=0; g<gammaMul; g++){
			fout << "1 0 22 " << gamma_px[g] << " " << gamma_py[g] << " " << gamma_pz[g] << " 0.0 0.0 0.0 " << 0.0 << endl;
		}
		//todo optional: abkg can be added here
		
	}
	
	cout << events << " events generated (done)" << endl;
	
	fileEvents->cd();
	treeEvents->Write("treeEvents");
	cout << "Generated events are written to tree" << endl;
	
	fileEvents->Close();
	
	
// 	TCanvas *canEnergyNeutrons=new TCanvas("canEnergyNeutrons","Neutron Kinetic Energies");
// 	canEnergyNeutrons->Divide(4,2);
// 	for(Int_t i=0; i<4; i++){
// // 	canEnergyNeutrons->Divide(2,2);
// // 	for(Int_t i=0; i<2; i++){
// 		canEnergyNeutrons->cd(i+1);
// 		histNeutronEnergies[i+1]->Draw();
// // 		histPopul[i+1]->Draw();
// 		
// 		canEnergyNeutrons->cd(i+5);
// 		histNeutronEnergiesGen[i+1]->Draw();
// 		
// 	}
	
	
	
	outfile->Close();
	
	fout.close();
	
	//if histograms shall be plotted, run theApp
	//otherwise program closes
// 	theApp->Run();
	
	
	
	
	
	return 0;
}


