/*!
The robot vehicle class
========================

Neuron based vehicles or N Braitenberg vehicles or New brain vehicle

Contains, classess for things, eyes, and binoculus. Monoculus dropped for the moment in favour of a vestigial right eye.

*/

#ifndef BRAITVEHICLE_H
#define BRAITVEHICLE_H
#include <iostream>
#include <sstream>
#include <list>		// list class library
#include "coordmatrix.h"
#include "bvglobalini.h"
using std::cout;
using std::endl;
 
class Binoculus; // needed for the list definition bvl
extern int globalID;
extern std::list<Binoculus> globalBvl;
extern readinifile GS; //global singleton - initialised from a file.



// Light colour: Could do this as a one to one relationship to the photoreceptors, perhaps in a future version
// in the interum  yellow=red+green, cyan=blue+green, magenta=red+blue, orange=red+yellow!
enum{OFF,RED,GREEN,BLUE,YELLOW,CYAN,MAGENTA,WHITE};//light colour
#define FIXEDLIGHT 0
#define SHYLIGHTSEEKER 1
#define AGGRESSLIGHTSEEKER 2
#define AGGRESSLIGHTPHOBE 3
#define SHYLIGHTPHOBE 4
#define PREY 5
#define BRIGHTPREY 6
#define PREDATOR 7
#define SPECULATRIX 8
#define TESTVEHICLE 9


/// All things must have things that attributes an id, colour,
/// halfwidth and type. Mainly used by the eyes so perhaps should also
/// have luminosity or be adapted by shadows.
/// The Thing class was intended to be inherited by Binocului, Monocului and
/// Lights however that looks impractical.
/// requires extern globalID
/// Might be redundant since it is not easy doing a list of things of different classes.
class Thing{
public:
  int id; /*!< unique id for each vehicle */
  int lcolour;	//Light colour  ie red blue green, cerise, aquamarine, turquoise, OFF 
  int vehicletype; // was bovatype, FIXEDLIGHT etc
  int nCollisions; // number of collisions
  float halfwidth; //halfwidth - this is a typical dimension and should relate to the impression on the eye (dim)
  std::string name; // name (will use to display a label for  the vehicle
  int getid(){return(id);}// unique id
  //int getname(){}
  inline int colour(){return(lcolour);}/// return light colour
  inline void setcolour(int col){lcolour=col;}/// set light colour
  friend int renumberthing();
  //----------------------------constructors-----------------
  Thing(){
    nCollisions=0;
    halfwidth=.15;
    id=globalID++;
    lcolour=OFF;
  }
};

//-------------------------------------------------------------
/// Eye class
/// cones Blue, green, red - currently no rods 
/// Simple eye would have num receptors (in each class), total angle of reception, 
/// Complex eye would define each receptor.
class Eye{// eye has a number of receptors, 
  int nreceptors;// this may be redundant since it is also the size of the vectors.
public:
  std::vector<float> BluePR,Bleft,Bright;  /// blue - S-cones
  std::vector<float> GreenPR,Gleft,Gright; /// green - M-cones
  std::vector<float> RedPR,Rleft,Rright; /// Red Yellow - L-cones
  float theta;// eye angle
public:
  Coordmatrix Aeye;
  /// uniform distribution of receptors between +- angle
  void inituniform(int n,float ang){
    nreceptors=n;
    if (n==1){// single receptor
      Bleft[0]=Gleft[0]=Rleft[0]=ang;
      Bright[0]=Gright[0]=Rright[0]=-ang;
    }else if(n==2){// good for a monoculus
      Bleft[0]=Gleft[0]=Rleft[0]=ang;
      Bright[0]=Gright[0]=Rright[0]=Bleft[1]=Gleft[1]=Rleft[1]=0;
      Bright[1]=Gright[1]=Rright[1]=-ang;
    }else if(n==3){
      Bleft[0]=Gleft[0]=Rleft[0]=ang;
      Bright[0]=Gright[0]=Rright[0]=Bleft[1]=Gleft[1]=Rleft[1]=ang/3;
      Bright[1]=Gright[1]=Rright[1]=Bleft[2]=Gleft[2]=Rleft[2]=-ang/3;
      Bright[2]=Gright[2]=Rright[2]=-ang;
    }else{
      cout << "eye error"<<endl;
    }
  }
  /*  void set EyeDiv(float ltheta){
    theta=ltheta;
    Aeye.trans(ltheta,0.0,0.0);
    }*/

//Could name this setEyeAngle (and let the # arguments determine what is called)
  void setEyeDiv(float ltheta,float x,float y){
    theta=ltheta;
    Aeye.trans(ltheta,x,y);
  }
/// update theta and the rotation elements of A. See also setEyeDiv
  inline void setEyeAngle(float th){
    theta=th;
    Aeye.trans(th);// leaves position column untouched.
  }
  inline float getEyeAngle(){return theta;}

public:
  void clearRetina(){
    for (int ii = 0; ii < BluePR.size();ii++){
      BluePR.at(ii)=0;
      GreenPR.at(ii)=0;
      RedPR.at(ii)=0;
    }
  }

  /// computes the impression on the receptors given by an object in
  /// eye space with a dimension dim.
  //
  /// process all items that could be seen.
  /// Not particularly calibrated so will need some scaling to work
  /// two of the 12 comparisons are redundant, since we know d>c and b>a, but are left to retain the symmetry!
  void see(Coordmatrix & P, std::vector<int> & lightcols,float dim){
    clearRetina();
    for (int ii = 0; ii < lightcols.size();ii++){
      float x= P.getval(0,ii);
      float y= P.getval(1,ii);
      float ptheta=atan2(y,x);
      float intensity=1/(x*x+y*y);        
      //float gamma = sqrt(dim*dim*intensity);
      float gamma = dim*sqrt(intensity);// half the visual angle at the eye
      float b=ptheta+gamma;// this values can go above PI : no worries.
      float a=ptheta-gamma;// this values can go below -PI : no worries.
      float d,c;
      //cout << "a="<<a << "b="<<b<<"theta="<<ptheta<<"gamma="<<gamma<<endl;
      //cout << a << ","<<b<<","<<ptheta<<","<<gamma<<endl;
      //if(b>3.1);b=3.1;
      //if(a<-3.1);a=-3.1;

      if (P.getval(2,ii)!=1)cout << "warning check Pgetval"<<endl;
      //cout <<"theta="<<theta*180/M_PI<<endl;
      //cout <<"gamma="<<gamma*180/M_PI<<endl;

      for (int jj=0;jj<nreceptors;jj++){     
      //int jj=0;
	switch(lightcols[ii]){//Is the light is shining? If so what colour.
	case OFF://Ignore
	//cout <<ii<<" is off"<<endl;
	  break;
	case BLUE:
	  d=Bleft[jj];
	  c=Bright[jj];
	  if((d>b)&&(b>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+8*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+8*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+8*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+8*(d-a)*intensity;
	  break;
	case RED:
	  d=Rleft[jj];
	  c=Rright[jj];
	  if((d>b)&&(b>c)&&(c>a)) RedPR[jj] =RedPR.at(jj)+8*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) RedPR[jj] =RedPR.at(jj)+8*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) RedPR[jj] =RedPR.at(jj)+8*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) RedPR[jj] =RedPR.at(jj)+8*(d-a)*intensity;
	  break;
	case GREEN:
	  d=Gleft.at(jj);
	  c=Gright.at(jj);
	  if((d>b)&&(b>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+8*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+8*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+8*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+8*(d-a)*intensity;
	  break;
	case CYAN://Blue + green 
	  d=Gleft[jj];
	  c=Gright[jj];
	  if((d>b)&&(b>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+4*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+4*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) GreenPR[jj] =GreenPR.at(jj)+4*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) GreenPR[jj] =GreenPR.at(jj)+4*(d-a)*intensity;
	  
	  d=Bleft[jj];
	  c=Bright[jj];
	  if((d>b)&&(b>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+4*(b-c)*intensity;
	  if((d>b)&&(b>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+4*(b-a)*intensity;
	  if((b>d)&&(d>c)&&(c>a)) BluePR[jj] =BluePR.at(jj)+4*(d-c)*intensity;
	  if((b>d)&&(d>a)&&(a>c)) BluePR[jj] =BluePR.at(jj)+4*(d-a)*intensity;
	
	  break;
	}
	if (GS.fechner){
	  RedPR[jj]=log(1+RedPR[jj]);// so 0->0
	  GreenPR[jj]=log(1+GreenPR[jj]);
	  BluePR[jj]=log(1+BluePR[jj]);
	}else{
	  if (RedPR[jj]>5000)RedPR[jj]=5000;
	  if (GreenPR[jj]>5000)GreenPR[jj]=5000;
	  if (BluePR[jj]>5000)BluePR[jj]=5000;
	}
      }//end:for jj
    }//end:for ii
    //cout << "-"<<GreenPR[0]<<endl;
  }//end: see

/*----------------------------constructors-----------------*/
  /// Constructor needs a location of the eye, the number and
  /// location of the receptors.
  Eye(const float ltheta)
    :BluePR(6),Bleft(6),Bright(6),GreenPR(6),Gleft(6),Gright(6),RedPR(6),Rleft(6),Rright(6),Aeye(3,3){
    theta=ltheta;
    Aeye.trans(ltheta,0.0,0.0);
    inituniform(1,60*M_PI/180);
  }
};









// Providing we cycle through a single list there does not seem to be
// a place for monoculus (or indeed M.Speculatrix) other than
// as adaptions of Binoculus.
// Placeholder
//class Monoculus: public Thing{
//};











class Binoculus: public Thing{
  float x[76];/// States are    \[ speed, \dot\theta,\theta, x, y \]
  float dotx[76];/// dot v, ddtheta, dtheta, dx, dy;
  Coordmatrix A;
  float BOverMass, BrOverInertia ;
  float mass, inertia;
  float B,  Brot;
  float Ml,Mr; // written to by think, inputs to the actuators.
  std::vector<float> BrainGains; // Tmp brain gains. 


public:

  Eye righteye,lefteye;  // can debate if this should be private or not...

  Coordmatrix log;// default log
  Coordmatrix log2;// second log etc. etc.

public:
  inline float getth(){return(x[2]);}
  inline float getx(){return(x[3]);}
  inline float gety(){return(x[4]);}
  inline float getvelocity(){return(x[0]);}
  inline float getturnrate(){return(x[1]);}
  inline float getEyeDiv(){return(lefteye.theta);}// candidate for removal
  inline void resetstates(){x[0]=x[1]=x[5]=0;dotx[0]=dotx[1]=dotx[2]=dotx[3]=dotx[4]=dotx[5]=0;}
  //  friend void printWidthand( Box & box );
  void updateA(){A.trans(x[2],x[3],x[4]);}
  void updatespecu(){// M. Speculatrix states
    // input is the photocell, outputs are steering rotation speed, and drive wheel speed
    float intensity=lefteye.GreenPR.at(0);
    float vs=GS.Speculatrix.at(2); //  constant forward velocity
    lefteye.inituniform(1,GS.Speculatrix[6]*M_PI/180);
    //cout <<"intensity="<<intensity<<endl;
    if (intensity < GS.Speculatrix[0]){
      //lefteye.theta=lefteye.theta+GS.Speculatrix[1];
      //lefteye.Aeye.trans(lefteye.theta);// this is a bit of a hack, should have a routine to update the ht when you update theta
      lefteye.setEyeAngle(lefteye.theta+GS.Speculatrix[1]);

      //vs=vs/4;
      vs=vs*GS.Speculatrix[5];
    }

    //if (lefteye.GreenPR.at(0) > 0) cout << lefteye.GreenPR[0]<<endl;
    float alpha=lefteye.theta;
    //cout <<"alpha="<<alpha<<endl;
    //float alpha=GS.Speculatrix[6];
    float h=GS.Speculatrix[3]; 
    float vf=vs*cos(alpha);
    float omega=vs*sin(alpha)/h;

    dotx[0]=0;// steering wheel is rotating at a constant speed
    dotx[1]=0;
    dotx[2]=omega;
    dotx[3]=vf*cos(x[2]);
    dotx[4]=vf*sin(x[2]);

    //cout << "alpha="<<alpha<<endl;
  }
  void updatestate(){/// for states 0,1,2 $\dot{x}=Ax$

    // inputs (limit to 5N - should probably reduce to 2/3)
    if(Ml>5)Ml=5;
    if(Ml<-5)Ml=-5;
    if(Mr>5)Mr=5;
    if(Mr<-5)Mr=-5;
    float F=(Ml+Mr);
    float T=(-Ml+Mr)*0.05;// implies 10 cm wheel base

    /// States =[ speed, \dot\theta,\theta, x, y ]
    // normalise theta (x[2]), allows easier checking of the orientation of the vehicle.
    while (x[2] > M_PI)
      x[2]=x[2]-2*M_PI;
    while(x[2]< -M_PI)
      x[2]=x[2]+2*M_PI;
    float af=B/mass;float at=Brot/inertia;

    
    dotx[0]=-af*x[0]+F/mass;
    dotx[1]=-at*x[1]+T/inertia;
    dotx[2]=x[1];
    dotx[3]=x[0]*cos(x[2]);
    dotx[4]=x[0]*sin(x[2]);
  }
  void updatelog(){ // may not need this method since, if logs are public, we can access directly!
    log.addcolumn(x[2],x[3],x[4]); // theta x y
  }
  void look(){/// update the receptors
    std::list<Binoculus>::iterator itr;
    std::vector<int> lightcolours;  /// colours for each vehicle
    Coordmatrix lightposns, bluel,greenl,redl;// yellowl,
    Coordmatrix A1inv(3,3),ALeyeinv(3,3),AReyeinv(3,3),ans;

    // computer the coordinate matrices needed
    A1inv=A;A1inv.inverse();
    ALeyeinv=lefteye.Aeye;ALeyeinv.inverse();
    AReyeinv=righteye.Aeye;AReyeinv.inverse();
    for (itr=globalBvl.begin(); itr != globalBvl.end(); itr++) {/// Check against every other vehicle
      if((*itr).id !=id){// if not yourself
	lightposns.addcolumn((*itr).getx(),(*itr).gety(),1.0);
	lightcolours.push_back((*itr).colour());
      }// end: if
    }//end:for
    if (lightposns.cols()>0){
      //cout <<"---"<<name<<"---, id="<<id<<",";
      //cout << "id="<<id<<",";
      ans=(ALeyeinv*A1inv)*lightposns; 
      lefteye.see(ans, lightcolours,0.15);// implies a dimension of 10cm (should make this the halfwidth)
      //cout<<"left eye="<<ans;
      ans=(AReyeinv*A1inv)*lightposns;	
      righteye.see(ans, lightcolours,0.15);
      if(GS.logging)log2.addcolumn(lefteye.BluePR[0],lefteye.GreenPR[0],lefteye.RedPR[0],righteye.BluePR[0],righteye.GreenPR[0],righteye.RedPR[0]);

      //cout <<endl;
      //cout <<"right eye="<<ans;
      //log2.addcolumn(id,ans.myValues[0],ans.myValues[1]); // log id x y
      //log2.addcolumn(id,righteye.BluePR[0],righteye.GreenPR[0],righteye.RedPR[0],ans.myValues[0],ans.myValues[1]); // log id 
    }else{// Nothing to see, nothing to see, move along
      lefteye.clearRetina();
      righteye.clearRetina();
    }
  }

  void relocate(float xx, float yy){ // magically move vehicle to x,y
    x[3]=xx;
    x[4]=yy;
    updateA();
  }
  void think(){
    Ml=0;
    Mr=0;
    if (vehicletype==SHYLIGHTSEEKER){
      float K1=GS.ShySeeker[7];
      float K2=GS.ShySeeker[8];
      Ml=K1/(lefteye.GreenPR[0]+.1)-K2;
      Mr=K1/(righteye.GreenPR[0]+.1)-K2;
    }
    if (vehicletype==AGGRESSLIGHTSEEKER){
      float K1=GS.AggressSeeker[7];
      Mr=lefteye.GreenPR[0]*K1;
      Ml=righteye.GreenPR[0]*K1;
    }
    if (vehicletype==AGGRESSLIGHTPHOBE){
      float K1=GS.AggressPhobe[7];
      Ml=lefteye.GreenPR[0]*K1;
      Mr=righteye.GreenPR[0]*K1;
    }
    if (vehicletype==SHYLIGHTPHOBE){
      float K1=GS.ShyPhobe[7];
      float K2=GS.ShyPhobe[8];
      Mr=K1/(lefteye.GreenPR[0]+.1)-K2;
      Ml=K1/(righteye.GreenPR[0]+.1)-K2;
    }
    if (vehicletype==PREDATOR){
      float K1=GS.Predator[7];
      Mr=lefteye.BluePR[0]*K1;
      Ml=righteye.BluePR[0]*K1;
      if ((abs(getvelocity())<.001)&& abs(getturnrate())<.001){// would be good to do this from the receptors
	Mr=.04;
      }
    }
    if (vehicletype==PREY){
      float K1=GS.Prey[7];
      float K2=GS.Prey[8];
      Ml=K1/(lefteye.GreenPR[0]+.1)-K2;
      Mr=K1/(righteye.GreenPR[0]+.1)-K2;
    }
    if (vehicletype==TESTVEHICLE){
      float K1=GS.TestVehicle[7];
      Mr=lefteye.GreenPR[0]*K1;
      Ml=righteye.GreenPR[0]*K1;
    }
    if (vehicletype==SPECULATRIX){
      //doing this in updatespecu
      //Ml=(lefteye.GreenPR[0]);// debugging...
    }
    if (vehicletype==BRIGHTPREY){
      //MR=(.01/(greenlefteyeresponse+.1)-.03)+(13*redlefteyeresponse);
      //ML=(.01/(greenrighteyeresponse+.1)-.03)+(13*redrighteyeresponse);

      float K1=GS.BrightPrey[7];
      float K2=GS.BrightPrey[8];
      float K3=GS.BrightPrey.at(9);
      //if (rand()<RAND_MAX/8)cout << " ";
      //cout << K1<<","<<K2<<","<<K3<<endl;
      
      Ml=K1/(lefteye.GreenPR[0]+.1)-K2+(K3*lefteye.RedPR[0]);
      Mr=K1/(righteye.GreenPR[0]+.1)-K2+(K3*righteye.RedPR[0]);
    }
    /*    if (id==999){// April
	Ml=.005;
	Mr=0.001;
	}*/
  }
  void info(){

    cout <<"Name="<<name<<"\nid="<<id<<endl;
    cout <<"Vehicle type "<<vehicletype <<"\nnumber of collisions="<<  nCollisions<<"\nHalf width="<< halfwidth<<endl;

    cout <<"Colour =";
    if (lcolour==OFF)cout<<"off "<<endl;
    else if (lcolour==RED)cout<<"red "<<endl;
    else if (lcolour==BLUE)cout<<"blue "<<endl;
    else if (lcolour==GREEN)cout<<"green "<<endl;
    else if (lcolour==CYAN)cout<<"cyan "<<endl;
    else if (lcolour==YELLOW)cout<<"yellow "<<endl;
    else cout<<"(need to update) ="<<lcolour<<endl;

    cout << A<<endl;
  }
  inline int getid(){return(id);}
  void setEyeDivergence(float angle,float x,float y){
    lefteye.setEyeDiv(angle,x,y);
    righteye.setEyeDiv(-angle,x,-y);
  }
  /*
  void setSSgains(float K1,float K2){
    BrainGains[0]=K1;
    BrainGains[1]=K2;
    }*/
  void setMIBs(float lM,float lI,float lB,float lBrot){
    mass=lM;
    inertia=lI;
    B=lB;
    Brot=Brot;
  }
  friend class Coordmatrix;
  friend void eulerstep(Binoculus &nbv);
  friend void worldphysics(Binoculus &nbv);
  friend void boundarycollisions(Binoculus &nbv);
  friend void boundaryfolding(Binoculus &nbv);
  //----------------------------constructors-----------------
  Binoculus(const char * argname,float theta,float xx, float yy, int colour)/// Binoculus constructor
    :righteye(-0.45),lefteye(0.45),A(3,3),BrainGains(9){// dawn robot weighs 390g+140g of batteries
    mass=0.4;inertia=.0005;// appears to be a problem when inertia is too small (16<B/I<166 (around 25)! ) ...(could be we are spinning)
    //B=.1;Brot=.00001;
    B=0.64;Brot=0.004;
    name=argname;
    vehicletype=FIXEDLIGHT;
    x[0]=0;x[1]=0;x[2]=theta;x[3]=xx,x[4]=yy;
    lcolour=colour;
    A.trans(x[2],x[3],x[4]);
  }
};

#endif

