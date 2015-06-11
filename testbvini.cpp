#include <iostream>
#include "bvglobalini.h"



readinifile GS; //global singleton


int main(int argc, char *argv[]){
  //readini defaults;
  //std::string infilename;
  char const * filename="bv.ini";
  int debug=0;
  if (argc>1){
    debug=atoi(argv[1]);
    cout << "debug="<<debug<<endl;
    if (argc>2){
      filename=argv[2];
    }
  }
  if (GS.ini(filename,debug)){
    cout << "Warning: could not open bv.ini, using defaults "<<endl;
    //return 1;
  }
  GS.inspect();
  //cout <<"speedcal="<<GS.speed_cal<<endl;

  return 0;
}
