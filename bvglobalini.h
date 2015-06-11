/*!
Initialise global variables for the vehicles from an ini file.

Probably a better way somewhere to do this somewhere, but it works for now.

[This does not seem to appear in doxygen!]
*/

#ifndef BRAITVEHICLE_INIT_H
#define BRAITVEHICLE_INIT_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cmath>
#if defined(_WIN32) || defined(_WIN64)
//if _WIN32 || _WIN64
/* We are on Windows - need to deal with strtok (posix) */
#define _CRT_SECURE_NO_WARNINGS
// in fact this does not seem to work so we will do a conditional compile when we use strtok/strtok_s
#endif
using std::cout;
using std::endl;


const int MAX_CHARS_PER_LINE = 512;
const int MAX_TOKENS_PER_LINE = 20;


class readinifile{
public:
  /// These are the global variables 
  float DeltaT;
  float worldboundary;
  int worldtype;
  int viewwidth;
  int viewheight;
  int fechner;
  int logging;
  int brainversion;
  std::vector<float> ShySeeker; 
  std::vector<float> AggressSeeker; 
  std::vector<float> ShyPhobe; 
  std::vector<float> AggressPhobe; 
  std::vector<float> TestVehicle;
  std::vector<float> Speculatrix;
  std::vector<float> Predator; 
  std::vector<float> Prey;
  std::vector<float> BrightPrey;
  float bb[6]; // bounding box (should do this as a vector!)
  //std::string tststr;
  //std::string logfilename;
  //bool aerialview;

public:
  int ini(char const * fname, int debug){// should call once per ini file.
  // create a file-reading object
    std::ifstream fin;
    //cout <<"File name="<<fname<<endl;
    //    fin.open("bv.ini"); // open a file
    fin.open(fname); // open a file
    if (!fin.good()) {
      std::cout << "could not open bv.ini"<<std::endl;
      return 1; // exit if file not found
    }
    // read each line of the file
    while (!fin.eof())
      {
	// read an entire line into memory
	char buf[MAX_CHARS_PER_LINE];
	fin.getline(buf, MAX_CHARS_PER_LINE);
	// parse the line into blank-delimited tokens
	int n = 0; // a for-loop index
	
	// array to store memory addresses of the tokens in buf
	const char* token[MAX_TOKENS_PER_LINE] = {}; // initialize to 0
	char * nextToken; // needed for strtok_s
	
	// parse the line (windows and posix versions) please someone write a C++ parser (eg yaml)
#if defined(_WIN32) || defined(_WIN64)
	token[0] = strtok_s(buf, ":",&nextToken); // first token (visualC)
#else
	token[0] = strtok(buf, ":"); // first token (g++)
#endif
	if (token[0]){ // zero if line is blank
	  for (n = 1; n < MAX_TOKENS_PER_LINE; n++){
#if defined(_WIN32) || defined(_WIN64)
	    token[n] = strtok_s(0, ",",&nextToken); // subsequent tokens (VC)
#else
	    token[n] = strtok(0, ","); // subsequent tokens (g++)
#endif
	    if (!token[n]) break; // no more tokens
	  }
	}
	//Here we have read and split a line
	if (token[0]){ // zero if line is blank
	  if(!strncmp(token[0],"#",1)){// print out comments
	    if (debug>=1) std::cout <<"% "<<token[0]<<std::endl;
	  }
	  else if (!strncmp(token[0],"World boundary",14)) {
	    worldboundary=atof(token[1]);
	  }
	  else if (!strncmp(token[0],"Brain version",13)) {
	    brainversion=atoi(token[1]);
	  }
	  else if (!strncmp(token[0],"Fechner",7)) {
	    std::string str(token[1]);
	    str.erase(str.find(' '), 1);
	    if ((str=="on") || (str== "1") || (str== "true"))
	      fechner=1;//could be more elegant
	    else
	      fechner=0;
	  }
	  else if (!strncmp(token[0],"Logging",7)) {
 	    std::string str(token[1]);
	    str.erase(str.find(' '), 1);// this is suspect for working with both dos and unix text files...
	    if (str == " on" || str=="on" || str== "1" || str== "true")
	      logging=1;//could be more elegant
	    else
	      logging=0;
	  }
	  else if (!strncmp(token[0],"World type",10)) {
	    std::string str(token[1]);
	    str.erase(str.find(' '), 1);
	    if (str=="torus")
	      worldtype=1;//could be more elegant
	    else
	      worldtype=0;
	  }
	  else if (!strncmp(token[0],"Shy Seeker",10)) {
	    if(n==10){
	      ShySeeker.resize(10);
	      for (int jj=0;jj<9;jj++){
		ShySeeker[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Shy Seeker error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Aggressive Seeker",17)) {
	    if(n==9){
	      AggressSeeker.resize(9);
	      for (int jj=0;jj<8;jj++){
		AggressSeeker[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Aggressive SeekerShy Seeker error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Shy Phobe",9)) {
	    if(n==10){
	      ShyPhobe.resize(10);
	      for (int jj=0;jj<9;jj++){
		ShyPhobe[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Shy Phobe error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Aggressive Phobe",16)) {
	    if(n==9){
	      AggressPhobe.resize(9);
	      for (int jj=0;jj<8;jj++){
		AggressPhobe[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Aggressive Phobe error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Test Vehicle",12)) {
	    if(n==10){
	      TestVehicle.resize(10);
	      for (int jj=0;jj<9;jj++){
		TestVehicle[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Test Vehicle error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Speculatrix",11)) {
	    if(n==12){
	      Speculatrix.resize(12);
	      for (int jj=0;jj<11;jj++){
		Speculatrix[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Speculatrix error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Predator",8)) {
	    if(n==10){
	      Predator.resize(10);
	      for (int jj=0;jj<9;jj++){
		Predator[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Predator error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Bright Prey",11)) {
	    if(n==11){
	      BrightPrey.resize(11);
	      for (int jj=0;jj<10;jj++){
		BrightPrey[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Bright Prey error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Prey",4)) {
	    if(n==10){
	      Prey.resize(10);
	      for (int jj=0;jj<9;jj++){
		Prey[jj]=atof(token[jj+1]);
	      }
	    }else{
	      cout <<"Warning: Prey error. n parms="<<n<<endl;
	    }
	  }
	  else if (!strncmp(token[0],"Bbox",4)) {
	    bb[0]=atof(token[1]);
	    bb[1]=atof(token[2]);
	    bb[2]=atof(token[3]);
	    bb[3]=atof(token[4]);
	  }
	  else if (!strncmp(token[0],"Euler integrator",16)) {
	    DeltaT=atof(token[1]);
	  }
	  else{
	    // process (print) the tokens
	    if (debug>=3){
	      for (int i = 0; i < n; i++) // n = #of tokens
		std::cout << "Token[" << i << "] = |" << token[i] <<"|"<< std::endl;
	      std::cout << std::endl;
	    }
	  }
	}
      }//end: file read

    if (ShySeeker.size()==0)std::cout<<"% Warning: Shy Seeker Length=0\n";



    return 0;
  }// end: ini  
  void inspect(){
    cout << "%% Inspect globals\n";
    cout << "DeltaT="<<DeltaT<<";\n";
    cout << "Logging="<<logging<<";\n";
    cout << "World_boundary="<<worldboundary<<";\n";
    cout << "World_type="<<worldtype<<";\n";
    cout << "Shy_Seeker=[";for(int jj=0;jj<8;jj++)cout<<ShySeeker[jj]<<",";cout<<ShySeeker[8]<<"];\n";
    cout << "Aggress_Seeker=[";for(int jj=0;jj<8;jj++){cout<<AggressSeeker[jj];if(jj==7)cout<<"];\n";else cout<<",";}
    cout << "Shy_Phobe=[";for(int jj=0;jj<9;jj++){cout<<ShySeeker[jj];if(jj==8)cout<<"];\n";else cout<<",";}
    cout << "Aggress_Phobe=[";for(int jj=0;jj<8;jj++){cout<<AggressPhobe[jj];if(jj==7)cout<<"];\n";else cout<<",";}
    cout << "Test_Vehicle=[";for(int jj=0;jj<9;jj++){cout<<TestVehicle[jj];if(jj==8)cout<<"];\n";else cout<<",";}
    cout << "Predator=[";for(int jj=0;jj<9;jj++){cout<<Predator[jj];if(jj==8)cout<<"];\n";else cout<<",";}
    cout << "Prey=[";for(int jj=0;jj<9;jj++){cout<<Prey[jj];if(jj==8)cout<<"];\n";else cout<<",";}
    cout << "Bright_Prey=[";for(int jj=0;jj<10;jj++){cout<<BrightPrey.at(jj);if(jj==9)cout<<"];\n";else cout<<",";}
    cout << "Speculatrix=[";for(int jj=0;jj<11;jj++){cout<<Speculatrix.at(jj);if(jj==10)cout<<"];\n";else cout<<",";}

    //<<",";cout<<AggressSeeker[6]<<"];\n";

    //cout << "Bbox=["<< bb[0] << ","<<bb[1] << ","<<bb[2]<< ","<<bb[3]<<"];\n";

    cout << "Brain="<<brainversion<<endl;
    //cout << logfilename<<endl;
  }
 readinifile():ShySeeker(10),AggressSeeker(10),ShyPhobe(10),AggressPhobe(10),TestVehicle(10),Predator(10),Prey(10),BrightPrey(10),Speculatrix(11)
    {/// floats, ints, etc are not initalised by default
      DeltaT=.01;
      worldboundary=6;
      worldtype=0;
      fechner=0;
      logging=0; // logging off by default;
      brainversion=1;
      ShySeeker[0]=AggressSeeker[0]=ShyPhobe[0]=AggressPhobe[0]=Predator[0]=Prey[0]=BrightPrey[0]=0.4;
      ShySeeker[1]=AggressSeeker[1]=ShyPhobe[1]=AggressPhobe[1]=Predator[1]=Prey[1]=BrightPrey[1]=0.0005;
      ShySeeker[2]=AggressSeeker[2]=ShyPhobe[2]=AggressPhobe[2]=Predator[2]=Prey[2]=BrightPrey[2]=0.65;
      ShySeeker[3]=AggressSeeker[3]=ShyPhobe[3]=AggressPhobe[3]=Predator[3]=Prey[3]=BrightPrey[3]=0.0005;
      ShySeeker[6]=AggressSeeker[6]=ShyPhobe[6]=AggressPhobe[6]=Predator[6]=Prey[6]=BrightPrey[6]=25;
      ShySeeker[7]=AggressSeeker[7]=ShyPhobe[7]=AggressPhobe[7]=Predator[7]=Prey[7]=BrightPrey[7]=5;

      Speculatrix[0]=.01;
      Speculatrix[1]=.05;
      Speculatrix[2]=1.5;
      Speculatrix[3]=.4;
      Speculatrix[6]=0.5;
      Speculatrix[6]=30;
    // Fcalibrate=2;// not accessable via bv.ini

    }
};

#endif
