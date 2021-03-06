
#include "InputInfo.hh"

using namespace std;

InputInfo::InputInfo(){
  
  fNumberEvents=100;
  fNumberOfCutFiles=0;
  
  fProjA=132;
  fProjZ=50;
  fTargetA=2;
  fTargetZ=1;
  fLightA=1;
  fLightZ=1;
  fNumberOfStates=1;
  //fMaxExEnergy=0.0;
  for(Int_t s=0; s<maxNumberOfStates+1; s++){
    fStateEnergy[s]=0.0;
    fStateSpecFact[s]=1.0;
    fStateGammaMul[s]=0;
    for(Int_t ss=0; ss<maxNumberOfStates+1; ss++){
      fStateGammaEnergies[s][ss]=0.0;
    }
  }
  fBeamEnergy=10.0; // in MeV/u
  
  fAngleMin=0.0;
  fAngleMax=TMath::Pi();
  fElasticDownscale=0.0;
  fIncludeElastic=false;
  
  fHaveOedoSimFileName=false;
  fHaveFrescoFileName=false;
  fFrescoHeaderOnly=false;
  fHavePaceFileName=false;
  fPaceOnly=false;
  fSource=false;
  
  fProfileE=false;	
  fProfileX=false;	
  fProfileY=false;	
  fProfileA=false;	
  fProfileB=false;
  fNoBeamTracking=false;	
  fAddGammas=false;
  
  fResTargetX=0.0;
  fResTargetY=0.0;  
  fResTargetZ=0.0;  
  fResTargetA=0.0;   
  fResTargetB=0.0;  
  //fResDet1X  =0.0;   
  //fResDet1Y  =0.0;  
  //fResDet1Z  =0.0;   
  //fResDet1E  =0.0;   
  //fResDet2E  =0.0;   
  fResBeamE  =0.0;   

  SetTargetMaterial("G4_Galactic");
  SetTargetPosition(0.0, 0.0, 0.0);
  SetTargetSize(60, 60, 0.001);
  SetTargetDensity(0.90);
  SetTargetDensityOffset(0.02);

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
  const Int_t stopper=10000; 
  
  const Int_t maxArg = 2+maxNumberOfStates;
  char temp[maxArg][500];
  
  
  
  while(fin.good())  {
  
  
    //reset values
    for(Int_t i=0; i< maxArg; i++){temp[i][0]='\0';}
    
    
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
    
      // file names	
    if(strcmp(temp[0],"output_rootfile_reaction")==0)  {
      strcpy(fOutFileNameReaction,temp[1]);
      cout << "Output file of reactions is '" << fOutFileNameReaction << "'" << endl;
      cout << "Note that this option is obsolete!" << endl;
    }
    else if(strcmp(temp[0],"output_textfile_fresco")==0)  {
      strcpy(fOutFileNameFresco,temp[1]);
      fHaveFrescoFileName = true;
      cout << "Output file of fresco is '" << fOutFileNameFresco << "'" << endl;
    }
    else if(strcmp(temp[0],"fresco_header_only")==0)  {
      fFrescoHeaderOnly = true;
      cout << "Only Header information is taken from fresco file " << endl;
    }
    else if(strcmp(temp[0],"output_textfile_pace")==0)  {
      strcpy(fOutFileNamePace,temp[1]);
      fHavePaceFileName = true;
      cout << "Output file of pace is '" << fOutFileNamePace << "'" << endl;
    }
    else if(strcmp(temp[0],"pace_only")==0)  {
      fPaceOnly = true;
      cout << "Only pace protons will be simulated " << endl;
    }
    else if(strcmp(temp[0],"output_rootfile_makeEvents")==0){
      strcpy(fOutFileNameMakeEvents,temp[1]);
      cout << "Output file of makeEvents is '" << fOutFileNameMakeEvents << "'" << endl;
    }
    else if(strcmp(temp[0],"output_rootfile_troja")==0){
      strcpy(fOutFileNameTroja,temp[1]);
      cout << "Output file of troja is '" << fOutFileNameTroja << "'" << endl;
    }
    else if(strcmp(temp[0],"input_textfile_geometry")==0){
      strcpy(fInFileNameGeometry,temp[1]);
      cout << "Input file for detector geomety is '" << fInFileNameGeometry << "'" << endl;
    }
    else if(strcmp(temp[0],"output_rootfile_analysis")==0){
      strcpy(fOutFileNameAnalysis,temp[1]);
      cout << "Output file of analysis is '" << fOutFileNameAnalysis << "'" << endl;
    }
    else if(strcmp(temp[0],"beam_profile_file_oedo")==0){
      strcpy(fOedoSimFileName,temp[1]);
      fHaveOedoSimFileName = true;
      cout << "Root file name with OEDO beam profile is set to " << fOedoSimFileName << endl;
    }
    else if(strcmp(temp[0],"dE-E_cut_file_type_name")==0){
      Int_t cutType = atoi(temp[1]);
      if(cutType >= maxCutType){
        cout << "Invalid cut type " << cutType << endl;
        cout << "Available types at the moment: 0 = no transfer / elastic scattering, 1 = one neutron transfer, e.g. (d,p), 2 = two neutron transfer, e.g. (t,p)" << endl;
        abort();
      }
      strcpy(fFileNameCuts[cutType],temp[2]);
      //fNumberOfCutFiles++;
      cout << "Root file with with graphical cuts for dE-E is set to type " << cutType << ", file name "  << fFileNameCuts[cutType] << endl;
    }
    // general parameter for all programs
    else if(strcmp(temp[0],"number_events")==0){
      fNumberEvents=atoi(temp[1]);
      cout << "Number of events is set to " << fNumberEvents << endl;
    }
    // reactions and event generator
    else if(strcmp(temp[0],"projectile_A")==0){
      fProjA=atof(temp[1]);
      cout << "Projectile mass A is set to '" << fProjA << "' u" << endl;
    }
    else if(strcmp(temp[0],"projectile_Z")==0){
      fProjZ=atof(temp[1]);
      cout << "Projectile charge Z is set to '" << fProjZ << "' e" << endl;
    }
    else if(strcmp(temp[0],"target_A")==0){
      fTargetA=atof(temp[1]);
      cout << "Target mass A is set to '" << fTargetA << "' u" << endl;
    }
    else if(strcmp(temp[0],"target_Z")==0){
      fTargetZ=atof(temp[1]);
      cout << "Target charge Z is set to '" << fTargetZ << "' e" << endl;
    }
    else if(strcmp(temp[0],"light_A")==0){
      fLightA=atof(temp[1]);
      cout << "Light ejectile mass A is set to '" << fLightA << "' u" << endl;
    }
    else if(strcmp(temp[0],"light_Z")==0){
      fLightZ=atof(temp[1]);
      cout << "Light ejectile charge Z is set to '" << fLightZ << "' e" << endl;
    }
    else if(strcmp(temp[0],"states")==0){
      fNumberOfStates=atof(temp[1]);
      cout << "Number of states in heavy ejectile is set to " << fNumberOfStates << endl;
      for(Int_t s=0; s<fNumberOfStates; s++){
        fStateEnergy[s+1]=atof(temp[2+s]);
        cout << "State " << s+1 << " at energy " << fStateEnergy[s+1] << " MeV, ";
      }
      cout << endl;
    }
    else if(strcmp(temp[0],"spectroscopic_factors")==0){
      ////fNumberOfStates=atof(temp[1]);
      ////cout << "Number of states in heavy ejectile is set to " << fNumberOfStates << endl;
      //for(Int_t s=0; s<fNumberOfStates; s++){
      //  fStateSpecFact[s+1]=atof(temp[1+s]);
      //  cout << "State " << s+1 << " with spectroscopic factor " << fStateSpecFact[s+1] << ", ";
      //}
      for(Int_t s=0; s<fNumberOfStates+1; s++){
        fStateSpecFact[s]=atof(temp[s+1]);
        cout << "State " << s << " with spectroscopic factor " << fStateSpecFact[s] << ", ";
      }
      cout << endl;
    }
    else if(strcmp(temp[0],"target")==0){
      fTarget.material = temp[1];

      fTarget.center[0] = atof(temp[2]);
      fTarget.center[1] = atof(temp[3]);
      fTarget.center[2] = atof(temp[4]);

      fTarget.size[0] = atof(temp[5]);
      fTarget.size[1] = atof(temp[6]);
      fTarget.size[2] = atof(temp[7]);
      
      fTarget.density = atof(temp[8]);
      fTarget.densityOffset = atof(temp[9]);

      printf("Got target material '%s', center x %f mm, y %f mm, z %f mm, full size x %f mm, y %f mm, z %f mm, Density %f mg/cm3, Density Offset %f mg.cm3\n", fTarget.material.c_str(), fTarget.center[0], fTarget.center[1], fTarget.center[2], fTarget.size[0], fTarget.size[1], fTarget.size[2], fTarget.density, fTarget.densityOffset);

    }
    //else if(strcmp(temp[0],"beam_energy_range")==0){
    //        fBeamEnergyRange[0]=atof(temp[1]);
    //        fBeamEnergyRange[1]=atof(temp[2]);
    //        fBeamEnergyRange[2]=atof(temp[3]);
    //        cout << " " <<  << endl;
    //}
    //else if(strcmp(temp[0],"max_excitation_energy")==0){
    //        fMaxExEnergy=atof(temp[1]);
    //        cout << "Highest excitation energy in heavy ejectile is set to " << fMaxExEnergy << " MeV" << endl;
    //}
    else if(strcmp(temp[0],"theta_range_CM")==0){
      fAngleMin=atof(temp[1])/180.0*TMath::Pi();
      fAngleMax=atof(temp[2])/180.0*TMath::Pi();
      cout << "Theta angles of recoiled particles in CM will be generated in the range from " << fAngleMin << " rad to " << fAngleMax << " rad." << endl;
    }
    else if(strcmp(temp[0],"include_elastic_downscaled")==0){
      fElasticDownscale = TMath::Power(10.0, atof(temp[1]));
      fIncludeElastic=true;
      cout << "Elastic scattering will be included with a downscaling factor of " << fElasticDownscale << endl;
    }
    else if(strcmp(temp[0],"beam_energy")==0){
      fBeamEnergy=atof(temp[1]);
      cout << "Beam energy is set to '" << fBeamEnergy << "' MeV/u" << endl;
    }
    // event generator related stuff
    else if(strcmp(temp[0],"beam_profile_energy")==0){
      fProfileE=true;
      cout << "Beam profile - energy - requested " << endl;
    }
    else if(strcmp(temp[0],"beam_profile_position_x")==0){
      fProfileX=true;
      cout << "Beam profile - x position - requested " << endl;
    }
    else if(strcmp(temp[0],"beam_profile_position_y")==0){
      fProfileY=true;
      cout << "Beam profile - y position - requested " << endl;
    }
    else if(strcmp(temp[0],"beam_profile_angular_a")==0){
      fProfileA=true;
      cout << "Beam profile - a angle (dx) - requested " << endl;
    }
    else if(strcmp(temp[0],"beam_profile_angular_b")==0){
      fProfileB=true;
      cout << "Beam profile - b angle (dy) - requested " << endl;
    }
      // analysis related stuff
    else if(strcmp(temp[0],"no_beam_tracking")==0){
      fNoBeamTracking=true;
      cout << "Beam tracking will not be performed " << endl;
    }
    else if(strcmp(temp[0],"resolution_target_x")==0){
      fResTargetX=atof(temp[1]);
      cout << "Resolution of target X position is set to '" << fResTargetX << "' mm (sigma of a Gaussian)" << endl;
    }
    else if(strcmp(temp[0],"resolution_target_y")==0){
      fResTargetY=atof(temp[1]);
      cout << "Resolution of target Y position is set to '" << fResTargetY << "' mm (sigma of a Gaussian)" << endl;
    }
    else if(strcmp(temp[0],"resolution_target_z")==0){
      fResTargetZ=atof(temp[1]);
      cout << "Resolution of target Z position is set to '" << fResTargetZ << "' mm (sigma of a Gaussian)" << endl;
    }
    else if(strcmp(temp[0],"resolution_target_a")==0){
      fResTargetA=atof(temp[1]);
      cout << "Resolution of target A angle is set to '" << fResTargetA << "' mrad (sigma of a Gaussian)" << endl;
    }
    else if(strcmp(temp[0],"resolution_target_b")==0){
      fResTargetB=atof(temp[1]);
      cout << "Resolution of target B angle is set to '" << fResTargetB << "' mrad (sigma of a Gaussian)" << endl;
    }
    //else if(strcmp(temp[0],"resolution_detector1_x")==0){
    //	fResDet1X=atof(temp[1]);
    //	cout << "Resolution of detector 1 X-position is set to '" << fResDet1X << "' mm (sigma of a Gaussian)" << endl;
    //}
    //else if(strcmp(temp[0],"resolution_detector1_y")==0){
    //	fResDet1Y=atof(temp[1]);
    //	cout << "Resolution of detector 1 Y-position is set to '" << fResDet1Y << "' mm (sigma of a Gaussian)" << endl;
    //}
    //else if(strcmp(temp[0],"resolution_detector1_z")==0){
    //	fResDet1Z=atof(temp[1]);
    //	cout << "Resolution of detector 1 Z-position is set to '" << fResDet1Z << "' mm (sigma of a Gaussian)" << endl;
    //}
    //else if(strcmp(temp[0],"resolution_detector1_e")==0){
    //	fResDet1E=atof(temp[1]);
    //	cout << "Resolution of detector 1 energy is set to '" << fResDet1E << "' MeV (sigma of a Gaussian)" << endl;
    //}
    //else if(strcmp(temp[0],"resolution_detector2_e")==0){
    //	fResDet2E=atof(temp[1]);
    //	cout << "Resolution of detector 2 energy is set to '" << fResDet2E << "' MeV (sigma of a Gaussian)" << endl;
    //}
    else if(strcmp(temp[0],"resolution_beam_e")==0){
      fResBeamE=atof(temp[1]);
      cout << "Resolution of beam energy is set to '" << fResBeamE << "' MeV (sigma of a Gaussian)" << endl;
    }
    else if(strcmp(temp[0],"generate_gammas")==0){
      fAddGammas=true;
      cout << "Adding gammas in event generator" << endl;
    }
    else if(strcmp(temp[0],"gammas")==0){
      Int_t gstate = atoi(temp[1]);
      fStateGammaMul[gstate]=atoi(temp[2]);
      cout << "Level " << gstate << ": " << fStateGammaMul[gstate] << " gammas with energies ";
      for(Int_t gmul=0; gmul<fStateGammaMul[gstate]; gmul++){
        fStateGammaEnergies[gstate][gmul] = atof(temp[3+gmul]);
        cout << fStateGammaEnergies[gstate][gmul] << " MeV, ";
      }
      cout << endl;
    }
    else if(strcmp(temp[0],"source")==0){
      fSource=true;
      if(strcmp(temp[1],"alpha")==0 || atoi(temp[1])==0){
        sprintf(fSourceTypeName, "alpha");
        fSourceTypeId=0;
      }
      else if(strcmp(temp[1],"beta-")==0 || atoi(temp[1])==1){
        sprintf(fSourceTypeName, "beta-");
        fSourceTypeId=1;
      }
      else if(strcmp(temp[1],"beta+")==0 || atoi(temp[1])==2){
        sprintf(fSourceTypeName, "beta+");
        fSourceTypeId=2;
      }
      else if(strcmp(temp[1],"gamma")==0 || atoi(temp[1])==3){
        sprintf(fSourceTypeName, "gamma");
        fSourceTypeId=3;
      }
      else{
        cout << "Unknown source type '" << temp[1] << "'! Please check! " << endl;
        abort();
      }
      fSourceLines=atoi(temp[2]);
      if( !(fSourceLines<maxNumberOfStates) ){
        cout << "Error in number of source energies! Got '" << fSourceLines << "'! Please check!" << endl;
        abort();
      }
      cout << "Got source '" << fSourceTypeName << "' (type " << fSourceTypeId << ") at " << fSourceLines << " energies: ";
      for(Int_t l=0; l<fSourceLines; l++){
        fSourceEnergies[l]=atof(temp[l+3]);
        cout << fSourceEnergies[l] << " MeV, ";
      }
      cout << endl;
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
