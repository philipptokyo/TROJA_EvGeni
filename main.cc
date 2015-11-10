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
// 	TApplication *theApp=new TApplication("theApp",  0, 0);
	
	
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
	
	
	
	
	// if histograms shall be plotted, run theApp
	// otherwise program closes
// 	theApp->Run();
	
	
	return 0;
}


