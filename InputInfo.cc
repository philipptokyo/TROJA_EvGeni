
#include "InputInfo.h"

using namespace std;

InputInfo::InputInfo(){
	
	maxLines=999999999;
	verbosity=0;
	
	numberEvents=100;
	beamBeta=0.0;
	
}

InputInfo::~InputInfo(){
	
}

void InputInfo::parse(char filename[100]){
	
	cout << endl;
	cout << "Parsing of input file" << endl;
	
	
	//check if in/out file exists
	ifstream fin;
	ifstream FileTest(filename);
	if(!FileTest){
		cout << "Can not open file '" << filename << "'! Does it exist?" << endl;
		abort();
	}else{
		cout << "Opening file '" << filename << "'" << endl;
		fin.open(filename);
	}
	
	
	//parse input file line-wise
	string line;
	Int_t counter=0;
	const Int_t stopper=1000; 
	
	const Int_t maxArg=2;
	char temp[maxArg][200];
// 	char *temp[maxArg];
	
	
	
	while(fin.good())  {
		
		
		//reset values
		for(Int_t i=0; i< maxArg; i++){temp[i][0]='\0';}
// 		for(Int_t i=0; i< maxArg; i++){strcpy(temp[i],NULL);}
		
		
		getline(fin,line);
		//printf("Parsing line '%s'\n",line.c_str());
		
		
		
		//skip comments
		line.copy(temp[0],2,0); //first two characters
		temp[0][2]='\0';
		//printf("temp1=%s\n",temp1);
		if(strcmp(temp[0],"//")==0){continue;}
		
		//parse the line
		//write each word to a char
		//delimiter is whitespace
		std::istringstream iss(line);
		for(Int_t i=0; i<maxArg; i++){iss >> temp[i];}
		
		//skip empty lines
		if(strcmp(temp[0],"")==0){continue;}
		
		
		
		//get input parameter:
		
		
		if(strcmp(temp[0],"input_from_dicebox")==0)  {
// 			sprintf(infilenameFromDicebox,temp[1]);
			strcpy(infilenameFromDicebox,temp[1]);
			cout << "Input file for converter is '" << infilenameFromDicebox << "'" << endl;
		}
		else if(strcmp(temp[0],"input_from_talys")==0)  {
// 			sprintf(infilenameFromTalys,temp[1]);
			strcpy(infilenameFromTalys,temp[1]);
			cout << "Input file for converter is '" << infilenameFromTalys << "'" << endl;
		}
		else if(strcmp(temp[0],"output_rootfile_dicebox")==0){
// 			sprintf(rootfilenameDicebox,temp[1]);
			strcpy(rootfilenameDicebox,temp[1]);
			cout << "Output root file of converter is '" << rootfilenameDicebox << "'" << endl;
		}
		else if(strcmp(temp[0],"output_rootfile_talys")==0){
// 			sprintf(rootfilenameTalys,temp[1]);
			strcpy(rootfilenameTalys,temp[1]);
			cout << "Output root file of converter is '" << rootfilenameTalys << "'" << endl;
		}
		else if(strcmp(temp[0],"output_events")==0){
// 			sprintf(filenameEvents,temp[1]);
			strcpy(filenameEvents,temp[1]);
			cout << "Output file with generated events is '" << filenameEvents << "'" << endl;
		}
		else if(strcmp(temp[0],"beam_beta")==0){
			beamBeta=atof(temp[1]);
			beamGamma=1.0/TMath::Sqrt(1.0-(beamBeta*beamBeta));
			cout << "Beam velocity is set to '" << beamBeta << "'" << endl;
		}
		else if(strcmp(temp[0],"bin_width")==0){
			binWidth=atof(temp[1]);
			cout << "Bin width is set to " << binWidth << " keV" <<  endl;
		}
// 		else if(strcmp(temp[0],"max_gamma_levels")==0){
// 			maxGammaMul=atoi(temp[1]);
// 			cout << "Maximum number of gammas from cascade is set to " << maxGammaMul << endl;
// 		}
		else if(strcmp(temp[0],"max_lines")==0){
			maxLines=atoi(temp[1]);
			cout << "Maximum lines to read from input file is set to " << maxLines << endl;
		}
		else if(strcmp(temp[0],"number_events")==0){
			numberEvents=atoi(temp[1]);
			cout << "Number of events to generate is set to " << numberEvents << endl;
		}
		else if(strcmp(temp[0],"verbosity")==0){
			verbosity=atoi(temp[1]);
			cout << "Verbosity level is set to " << verbosity << endl;
		}
		else {
			cout<<"Could not read your input keyword '" << temp[0] << "'. Aborting program."<<endl; 
			abort();
		}
		
		
		//count lines
		counter++;
		if(counter>stopper){
			cout << "Reached " << counter << " lines in file '" << filename << "'! Stopping!" << endl;
			abort();
		}
	} // end of reading input file
	
	cout << endl;
	
	
}
