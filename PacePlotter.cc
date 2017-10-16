
#include "PacePlotter.hh"

using namespace std;

PacePlotter::PacePlotter(){
  
  paceevents=0;

  for(Int_t t=0; t<thbins; t++){
    thc[t]=0.0;
    dSdO[t]=0.0;
    for(Int_t e=0; e<enbins; e++){
      enth[t][e]=0.0;
    }
  }

  for(Int_t e=0; e< enbins; e++){
    enc[e]=0.0; // energy center
  }
  
}

PacePlotter::~PacePlotter(){
	
}

void PacePlotter::parse(char filename[100]){
  
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
  const Int_t stopper=10000; 
  
  const Int_t maxArg = 40;
  char temp[maxArg][500];
  
  
  
  while(fin.good())  {
  
  
    //reset values
    for(Int_t i=0; i< maxArg; i++){temp[i][0]='\0';}
    
    
    getline(fin,line);
    //printf("Parsing line '%s'\n",line.c_str());
    
    
    
    ////skip comments
    //line.copy(temp[0],2,0); //first two characters
    //temp[0][2]='\0';
    ////printf("temp1=%s\n",temp1);
    //if(strcmp(temp[0],"//")==0){continue;}
    
    //parse the line
    //write each word to a char
    //delimiter is whitespace
    std::istringstream iss(line);
    for(Int_t i=0; i<maxArg; i++){iss >> temp[i];}
    
    //skip empty lines
    if(strcmp(temp[0],"")==0){continue;}
    
    
    
    //get input parameter:
    
      // file names	
    if((strcmp(temp[0],"Proton")==0) && (strcmp(temp[1],"spectra")==0) && (strcmp(temp[2],"in")==0)  && (strcmp(temp[3],"laboratory")==0) && (strcmp(temp[4],"coordinates")==0) )  {
      paceevents = atoi(temp[5]);
      cout << paceevents << " events registered in pace" << endl;


      // read the data lines
      getline(fin,line);
      getline(fin,line);

      //reset values
      for(Int_t i=0; i<maxArg; i++){temp[i][0]='\0';}

      getline(fin,line); // this line contains angular range, lower value
      //cout << line << endl;
      std::istringstream iss1(line);
      for(Int_t i=0; i<maxArg; i++){
        iss1 >> temp[i];
        if((i>2) && (i<39) && (i%2==1)){
          Int_t j=(i-3)/2;
          thc[j]=atof(temp[i]);
          //cout << "theta bin " << j << " is at " << thc[j] << " deg" << endl;
        }
      }

      //reset values
      for(Int_t i=0; i<maxArg; i++){temp[i][0]='\0';}

      getline(fin,line); // this line contains angular range, upper value
      //cout << line << endl;
      std::istringstream iss2(line);
      for(Int_t i=0; i<maxArg; i++){
        iss2 >> temp[i];
        if((i>1) && (i<38) && (i%2==0)){
          Int_t j=(i-2)/2;
          thc[j] =  ((atof(temp[i]) - thc[j])/2.0) + thc[j];
          //cout << "theta bin " << j << " is at " << thc[j] << " deg" << endl;
        }
      }

      getline(fin,line);
      

      // now the energy ranges start
      for(Int_t e=0; e<enbins; e++){

        //reset values
        for(Int_t i=0; i< maxArg; i++){temp[i][0]='\0';}

        getline(fin,line);
        //cout << line << endl;

        std::istringstream iss3(line);
        for(Int_t i=0; i<maxArg; i++){
          iss3 >> temp[i];

          if(i>2){
            Int_t jj = i-3;
            if(jj>=thbins){break;}
            
            enth[jj][e] = atof(temp[i]); 

            //cout << enth[jj][e] << " ";
            //cout << jj << "/" << enth[jj][e] << " ";

          }

        }
        
        enc[e]=atof(temp[1]) + 0.5;
        //cout << " at energy " << enc[e] << endl;

      }

      getline(fin,line);
      getline(fin,line);
      getline(fin,line);
      

      // dSigam/dOmega
      getline(fin,line);
      //cout << line << endl;

      //reset values
      for(Int_t i=0; i<maxArg; i++){temp[i][0]='\0';}
      
      std::istringstream iss4(line);
      for(Int_t i=0; i<maxArg; i++){
        iss4 >> temp[i];

        if(i>2){
          Int_t jj = i-3;
          if(jj>=thbins){break;}
          
          dSdO[jj] = atof(temp[i]); 

          //cout << dSdO[jj] << " ";

        }
      }


    }
    //else if(strcmp(temp[0],"output_textfile_fresco")==0)  {
    //  strcpy(fOutFileNameFresco,temp[1]);
    //  fHaveFrescoFileName = true;
    //  cout << "Output file of fresco is '" << fOutFileNameFresco << "'" << endl;
    //}
    //else {
    //  cout<<"Could not read your input keyword '" << temp[0] << "'. Aborting program."<<endl; 
    //  abort();
    //}
    
    
    //count lines
    counter++;
    if(counter>stopper){
      cout << "Reached " << counter << " lines in file '" << filename << "'! Stopping!" << endl;
      abort();
    }
  } // end of reading input file

  cout << "PacePlotter: data from file obtained. Histograms can be created now." << endl;
  cout << endl;


}



TH1D* PacePlotter::MakeCSHist(){
  
  TH1D* h1 = new TH1D("dSdO", "dSigma/dOmega", thbins, 0, 180);

  
  for(Int_t t=0; t<thbins; t++){
    Int_t bin = h1->FindBin(thc[t]);
    
    h1->SetBinContent(bin, dSdO[t]);
  }

  //h1->Draw();

  return h1;

}



TH2D* PacePlotter::Make2DHist(){
  
  TH2D* h2 = new TH2D("ETh", "Energy vs. Theta lab", thbins, 0, 180, enbins, 0, 30);

  
  for(Int_t t=0; t<thbins; t++){
  for(Int_t e=0; e<enbins; e++){
    Int_t bin = h2->FindBin(thc[t], enc[e]);
    
    h2->SetBinContent(bin, enth[t][e]);
  }
  }

  //h2->Draw("colz");

  return h2;

}
