/* 
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 */

/*! \mainpage Neuron controlled vehicles

You are welcome to modify and use this code for research, teaching or
any non critical applications.  Please credit the origin (a web
address is sufficient  http://www.reading.ac.uk/~shshawin ). If you make any
inspired changes and are happy for these to be published on the web
please let me know so that I can update the program.

This code uses glut (freeglut) and is based on pre version 3.0 opengl.
An Object Oriented wrapper is provided by glutMaster although not
strictly necessary.

Copyright William Harwin 2002-2015
\author W.S.Harwin (w.s.harwin at reading.ac.uk)

\section SECA User Commands


See also brait.ini for start up conditions 

\section SECB Proposed features and bugs

(Oh the power of the comma)

Need to still check the eye model, there may be light leaking in from the back. [2/15: seems ok and may only be a problem with eyes that have receptors over more than 360 degress!]


Clean up the dynamics/kinematics

Have an array of eyes. Possibly need a vector of receptor values
(cones/proprioceptprs etc), of actuators (agonist and antagonists),
and iternal states for the 'design a brain' extension.


Add the 'design a brain' facilities.



Other sensors eg a collision, wheel proprioception, and a fatigue
function (all sensed by a neuron of course)

Calculate the actual loop-time [2/15: Now implimented, available with
the 'i' command].  Start using this as the simulation time.


\section SECC Wishful thinking

Would be nice to get glutSpecialFunc working

would be nice to get a webgl version/variant?

Add obsticles to the arena.

Add a Morris water maze

*/

/*! \page intro Introduction


The programming style is functional rather than elegant and I have no doubt 
missed out on some of the subtleties of C++ programming. If you have a better
way of doing things, please advise - or better still please implement.


*/



/*
@todo Camera on the vehicle [done, although would be useful if it saw what the animal saw]
@todo Vectors to lights
@todo behaviours

Learning/optimization
*/

char version[]="3.2  Feb2015";
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


//#include <algorithm>    // std::remove_if
#include <fstream>
//#include <cstring>
#include <sstream>
#include <string>
#include <iostream>
#include <list>		// list class library
#include "glutMaster.h"
#include "coordmatrix.h"  // matricies for coordinate transforms and data logging
#include "NBV.h"  // the vehicle class
#include "bvglobalini.h" // for initialising variables from bv.ini
#include "nrWindow.h"

#ifndef M_PI
#define M_PI 3.1415926535897932
#endif


//using namespace std;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::iterator;
using std::list;
using std::ofstream;
using std::string;


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



//void drawRobot(float x,float y,float theta,float eyeangle,int eyeconeang, char * title, int flag);
void drawRobot(float x,float y,float theta,float eyeangle,int eyeconeang, string & title, int flag);
void drawRobot(float x,float y,float theta,float eyeangle,float robotlengthA,float robothalfwidth,float eyeposx, int eyeconeang,string & title, int flag);
void drawFixedLight(float x,float y);
void drawFloor();
//void drawLights();
void drawObjectSelector(float x,float y);
void showMessage(GLfloat x, GLfloat y, GLfloat z, char *message);
void writevehiclelogs(string  fname,char  var,int  lnum);
void makealight();
void makeashyseeker();
void makeashyphobe();
void makeanagressiveseeker();
void makeanagressivephobe();
void makeapredator();
void makeaprey();
void makeabrightprey();


extern NRWindow     * Window ;
extern GlutMaster     * glutMaster;
//extern GLfloat orthratio;
//extern GLfloat aspect;
extern GLfloat globalphi;
extern GLfloat globaltheta;
extern GLfloat cameraX;
extern GLfloat cameraY;
extern GLfloat cameraZ;



// --------------------Globals --------------
//list<Binoculus> bvl;
list<Binoculus>::iterator globalNVselect; 
list<Binoculus> globalBvl;
int globalID=1;//unique id
string globalkbuf;
int globaltime=0; // experimental for the moment.
int globallooptime=0;
readinifile GS; //global singleton - initialised from a file.
bool aerialview=true;
int viewwidth=820;
int viewheight=680;
//bool movetoggle=false;

//----------------------------------------------------------------------------


//------------------------------------------------------------------------




//----------------------------------------------------------------------------

void NRWindow::StartMoving(GlutMaster * glutMaster){

   glutMaster->SetIdleToCurrentWindow();
   glutMaster->EnableIdleFunction();
}

void NRWindow::StopMoving(GlutMaster * glutMaster){

   glutMaster->SetIdleToCurrentWindow();
   glutMaster->DisableIdleFunction();
}

// Can't get this to work...
void NRWindow::CallBackSpecialFunc(unsigned char key, int x, int y)
{
  switch (key) {
  case GLUT_KEY_UP : cout <<"Glut key up"<<endl; break;
  case GLUT_KEY_DOWN : cout <<"Glut key down"<<endl; break;
  default:
    cout << "Special character  " << key << " at "<< x <<", "<<y<<endl;
    Window->StopMoving(glutMaster);   // stop idle function
    break;
  }
  cout << "Special currently not getting called..."<<endl;
}

void NRWindow::CallBackMouseFunc(int button, int state, int mx, int my){

  if (button == GLUT_LEFT_BUTTON) {
    if (state == GLUT_DOWN) {
      cout << "down  " << button << " is at "<< state <<endl;
    }
    if (state == GLUT_UP) {
      cout << "up  " << button << " is at "<< state <<"x="<<mx<<"y="<<my<<endl;

      // direct copy from 's'

    float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
    float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
    list<Binoculus>::iterator iter;
    
    float nearest=9999.0;
    float delx,dely,d2;
    for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++){
      delx=(*iter).getx()-xx;
      dely=(*iter).gety()-yy;
      d2=delx*delx+dely*dely;
      if (d2<nearest){globalNVselect=iter;nearest=d2;}
    }



    }
  }
}


void NRWindow::CallBackKeyboardFunc(unsigned char key, int mx, int my)
{
  switch (key) {
  case 'a':/// Add a light
    {
      float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
      float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
      cout <<"xx="<<xx<<",yy="<<yy<<endl;
      if (xx>GS.worldboundary)xx=GS.worldboundary;
      if (xx<-GS.worldboundary)xx=-GS.worldboundary;
      if (yy>GS.worldboundary)yy=GS.worldboundary;
      if (yy<-GS.worldboundary)yy=-GS.worldboundary;
      Binoculus newvehicle("A",-3*M_PI/4,xx,yy,GREEN); 
      //newbeast.change(FIXEDLIGHT);
      //newbeast.settheta();
      globalBvl.push_back (newvehicle);
      globalNVselect=globalBvl.end();
      globalNVselect--;
      
      //globalNVselect=globalBvl.end()--;
    }
    break;
  case 'b':// not sure what this is for...
    aerialview=true;
    GLfloat gx,gy;
    gx=(*globalNVselect).getx();
    gy=(*globalNVselect).gety();
    break;
  case 'e':/// Erase/delete NCV
      if (globalBvl.size()>1){
        globalBvl.erase(globalNVselect);
        globalNVselect=globalBvl.begin();
      }
      break;
  case 'f':/// rotate world  counter clockwise
      globalphi +=1;
      break;
  case 'g':/// rotate world clockwise
      globalphi +=-1;
      break;
  case 'h':/// help
    GLdouble pmatrix[16],mvmatrix[16];
    //int i;
    cout << "  Braitenberg Vehicle Simulator\n  -----------------------------\n";
    cout << "Version "<<version <<endl;
    glGetDoublev(GL_PROJECTION_MATRIX,pmatrix);
    glGetDoublev(GL_MODELVIEW_MATRIX,mvmatrix);
    cout << "Projection Matrix"<< endl;
    for(int i=0;i<4;i++)
      cout << pmatrix[i]<<"\t"<<pmatrix[i+4]<<"\t"<<pmatrix[i+8]<<"\t"<<pmatrix[i+12]<<endl;
    cout <<endl;
    cout <<"ModelView Matrix"<<endl;
    for(int i=0;i<4;i++)
      cout << mvmatrix[i]<<"\t"<<mvmatrix[i+4]<<"\t"<<mvmatrix[i+8]<<"\t"<<mvmatrix[i+12]<<endl;
    cout <<endl;
    //cout << "x="<<x<<",y="<<y<<endl;
    break;
  case 'i':/// Information on selected vehicle
    (*globalNVselect).info();
    cout << "loop time (mS) ="<<globallooptime<<endl;
    cout << "elapsed time (mS) ="<<globaltime<<endl;
    break;

  case 'l':/// Reload bv.ini
    if (GS.ini("bv.ini",0))
      cout << "Warning: could not open bv.ini, using defaults "<<endl;
    GS.inspect();
    break;

  case 'm':/// Move a light or beast
      {
	float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
	float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
	(*globalNVselect).relocate(xx,yy);
      }
      break;

  case 'n':/// Select next vehicle
      if (globalBvl.size()>1){
        globalNVselect++;
        if (globalNVselect==globalBvl.end())globalNVselect=globalBvl.begin();
      }
      break;
  case 'p':/// Select previous vehicle
      if (globalBvl.size()>1){
        if (globalNVselect==globalBvl.begin())globalNVselect=globalBvl.end();
        globalNVselect--;
      }
      break;

  case 'r':/// Reset the world
      globalphi = globaltheta=0;
      cameraX=cameraY=0;
      break;
  case 's':/// Select the nearest NCV 
    {
    float yy=(viewheight/2-my)*cameraZ/830.0+cameraY;
    float xx=(mx-viewwidth/2)*cameraZ/830.0+cameraX;
    list<Binoculus>::iterator iter;
    
    float nearest=9999.0;
    float delx,dely,d2;
    for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++){
      delx=(*iter).getx()-xx;
      dely=(*iter).gety()-yy;
      d2=delx*delx+dely*dely;
      if (d2<nearest){globalNVselect=iter;nearest=d2;}
    }
  }
    break;


  case 'q':/// quit
  case 27:///  quit
    exit(1);
    break;
  case 'o':/// currently identical to 'z'
    cameraZ=cameraZ+1.5;
    //    orthratio=orthratio*1.260;
    //changeview();
    break;
  case 'O':/// currently identical to 'Z'
    cameraZ=cameraZ-1.5;
    //orthratio=orthratio/1.260;
    //changeview();
    break;
  case 't':/// rotate towards
    globaltheta +=1;
    break;

  case 'v':/// rotate away
      globaltheta +=-1;
      break;

  case 'x':/// move camera +ve x
    cameraX=cameraX+.5;
    break;
  case 'X':/// move camera -ve x
    cameraX=cameraX-.5;
    break;
  case 'Y':/// move camera -ve y
    cameraY=cameraY-.5;
    break;
  case 'y':/// move camera +ve y
    cameraY=cameraY+.5;
    break;
  case 'Z':/// Camera out
    cameraZ=cameraZ-2;
    if (cameraZ<0)
      cameraZ=1.5;
    //CallBackDisplayFunc();
//    changeview();
    break;
  case 'z':/// Camera in
    cameraZ=cameraZ+2;
    //CallBackDisplayFunc();
    //changeview();
    break;
  case ' ':/// start simulation
    cout << "setting idle to moving" <<endl;
    Window->StartMoving(glutMaster);   // enable idle function
    break;
  case '>':
    if ((*globalNVselect).vehicletype <7)(*globalNVselect).vehicletype++;
    //      (*ncvselect).incncv();
      break;
  case '<':
    if ((*globalNVselect).vehicletype >0)(*globalNVselect).vehicletype--;
    //(*ncvselect).decncv();
      break;
    break;
  case 'A':/// Two letter command
  case 'B':/// Two letter command
  case 'D':/// Two letter command
  case 'L':/// Save the log files
  case 'M':/// Two letter command
  case 'P':/// Two letter command
  case 'R':/// Two letter command
  case 'S':/// Two letter command
  case 'T':/// Two letter command
  case 'U':/// Two letter command
  case 'V':/// Two letter command
  case '1':/// Two letter command
  case '2':/// Two letter command
  case '3':/// Two letter command
  case '4':/// Two letter command
    {
      //globalkbuf.append(""+key);
      globalkbuf +=key;
      //cout << key<<"->"<<globalkbuf<<endl;
      cout << "->"<<globalkbuf<<endl;
      if(globalkbuf.find("RL")!=std::string::npos){ // Reset to a light
	(*globalNVselect).resetstates();
	makealight();
	globalkbuf.clear();
      }
      if(globalkbuf.find("DUMP")!=std::string::npos){ // Dump a matlab file
	writevehiclelogs("pos1.mat",'p',1);
	writevehiclelogs("eye2.mat",'e',2);
	cout <<"Matlab dump"<<endl;
	globalkbuf.clear();
      }
      if(globalkbuf.find("AP")!=std::string::npos){// Aggressive Phobe
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=AGGRESSLIGHTPHOBE;
	(*globalNVselect).name="Aggressive Phobe";
	(*globalNVselect).setEyeDivergence(GS.AggressPhobe.at(6)*M_PI/180,0,0);
	(*globalNVselect).setcolour(BLUE);
	cout <<"found AP"<<endl;
	globalkbuf.clear();
      }
      if(globalkbuf.find("SS")!=std::string::npos){// Shy Seeker
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=SHYLIGHTSEEKER;
	(*globalNVselect).name="Shy Seeker";
	(*globalNVselect).setEyeDivergence(GS.ShySeeker.at(6)*M_PI/180,.5,.15);
	(*globalNVselect).setcolour(BLUE);
	//makeashyseeker();
	cout <<"found SS"<<endl;
	globalkbuf.clear();
      }
      if(globalkbuf.find("SP")!=std::string::npos){// Shy Phobe
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=SHYLIGHTPHOBE;
	(*globalNVselect).name="Shy Phobe";
	(*globalNVselect).setEyeDivergence(GS.ShyPhobe.at(6)*M_PI/180,0,0);
	(*globalNVselect).setcolour(BLUE);
	cout <<"found SP"<<endl;
	globalkbuf.clear();
      }
      if(globalkbuf.find("AS")!=std::string::npos){// Aggressive Seeker
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=AGGRESSLIGHTSEEKER;
	(*globalNVselect).name="Aggressive Seeker";
	(*globalNVselect).setEyeDivergence(GS.AggressSeeker.at(6)*M_PI/180,0,0);
	(*globalNVselect).setcolour(RED);
	cout <<"found DP"<<endl;
	globalkbuf.clear();
      }

      if(globalkbuf.find("DD")!=std::string::npos){// Agressive Seeker (DD)
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=AGGRESSLIGHTSEEKER;
	makeanagressiveseeker();
	cout <<"found AS"<<endl;
	globalkbuf.clear();
      }





      if(globalkbuf.find("DP")!=std::string::npos){// Dumb Prey
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=PREY;
	(*globalNVselect).name="Dumb Prey";
	(*globalNVselect).setEyeDivergence(GS.Prey.at(6)*M_PI/180,0.5,0.15);
	(*globalNVselect).setcolour(BLUE);
	cout <<"found DP"<<endl;
	globalkbuf.clear();
      }
      if(globalkbuf.find("BP")!=std::string::npos){// Bright Prey
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=BRIGHTPREY;
	(*globalNVselect).name="Bright Prey";
	(*globalNVselect).setEyeDivergence(GS.BrightPrey.at(6)*M_PI/180,0.5,0.15);
	(*globalNVselect).setcolour(BLUE);
	cout <<"found BP"<<endl;
	globalkbuf.clear();
      }
      if(globalkbuf.find("PR")!=std::string::npos){// Predator
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=PREDATOR;
	(*globalNVselect).name="Predator";
	(*globalNVselect).setEyeDivergence(GS.Predator.at(6)*M_PI/180,0,0);
	(*globalNVselect).setcolour(RED);
	cout <<"found PR"<<endl;
	globalkbuf.clear();
      }
      if(globalkbuf.find("MS")!=std::string::npos){// Test Vehicle
	cout <<"Machina Specularix\n";
	(*globalNVselect).resetstates();
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=SPECULATRIX;
	(*globalNVselect).name="M.Speculatrix";
	(*globalNVselect).setEyeDivergence(GS.Predator.at(6)*M_PI/180,0,0);
	cout << "eye field set to "<<GS.Predator.at(6)<<"degrees"<<endl;
	//(*globalNVselect).setcolour(RED);
	(*globalNVselect).setcolour(GREEN);
	(*globalNVselect).lefteye.inituniform(1,60*M_PI/180);

	globalkbuf.clear();
      }
      if(globalkbuf.find("TV")!=std::string::npos){// Test Vehicle
	(*globalNVselect).resetstates();
	(*globalNVselect).vehicletype=TESTVEHICLE;
	(*globalNVselect).name="Test Vehicle";
	(*globalNVselect).setEyeDivergence(GS.TestVehicle.at(6)*M_PI/180,0,0);
	(*globalNVselect).setcolour(BLUE);
	cout <<"found TV"<<endl;
	globalkbuf.clear();
      }

    }
    break;
  default:
    cout << "character  " << key << " at "<< mx <<", "<<my<<endl;
    Window->StopMoving(glutMaster);   // stop idle function
    break;
  }
  //glutPostRedisplay();
  NRWindow::CallBackDisplayFunc();
}


void NRWindow::CallBackDisplayFunc(void){
  list<Binoculus>::iterator iter;
  float xx,yy,th;
  //int eyecone;
  //float eyeangle;

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();/// @todo should be able to reset the proj or the other matrix so as to avoid this push/pop!


   glRotatef(globaltheta, 1.0, 0.0, 0.0);
   glRotatef(globalphi, 0.0, 0.0, 1.0);

//   Draw_lights();

/// glutSolidTeapot(10);
/// Teapot coordinate frame, x-axis out of spout , y-axis out of lid, z-axis to rightl


//   drawRobot(1.5,1.0,1.0,90.0*M_PI/180.0,90,"test",1);
//   drawRobot(0,-2,0,0,230,NULL,3);
   drawFloor();
   //   drawLights();      
//   drawObjectSelector(0,0);


   for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++) {
     xx=(*iter).getx();
     yy=(*iter).gety();
     th=(*iter).getth();

     if((*iter).vehicletype==FIXEDLIGHT){
       drawFixedLight(xx,yy);
     }else{
       float eyediv=(*iter).getEyeDiv();
       // should save this info in eyes... (*iter).getRetinaAngle()*180/M_PI; For now=60degrees

       // this version uses vehicle dimensions .5,.3.6,drawRobot(xx,yy,th,eyediv,.5,.3,.6,60,(*iter).name,(*iter).vehicletype);

       drawRobot(xx,yy,th,eyediv,60,(*iter).name,(*iter).vehicletype);
     }

     /*     if((*iter).btype()==FIXEDLIGHT)

     else{
       eyeangle=(*iter).eyeth();
       eyecone=(*iter).eye_cone();

       drawRobot(xx,yy,th,eyeangle,eyecone,(*iter).title(),(*iter).btype());
       }*/
//      drawObjectSelector(xx,yy);
   }//end:for

  xx=(*globalNVselect).getx();
  yy=(*globalNVselect).gety();
  drawObjectSelector(xx,yy);

  /* attempt to get labels to orient and draw right.	 
  for (iter=bovalist.begin(); iter != bovalist.end(); iter++){
    if((*iter).btype()!=FIXEDLIGHT){
      xx=(*iter).x();
      yy=(*iter).y();
      showMessage(xx,yy,0,(*iter).title());
    }
    }*/
  
  glPopMatrix();

  glLoadIdentity();
  // Option to put the camera on the selected robot
  if (aerialview){
    glTranslatef(-cameraX,-cameraY,-cameraZ);
  }else{// needs work, is tracking the vehicle but not the orientation...
    // could have three options and use also the glTranslatef here with xx/yy
    GLdouble s= sin((*globalNVselect).getth());
    GLdouble c= cos((*globalNVselect).getth());
    //cout << (*globalNVselect).getth()<<" c="<<c<<"s="<<s<<endl;
    gluLookAt(xx-2.5*c,yy-2.5*s,cameraZ/2.0,
	      xx,yy,0.5,
	      0,0,1);
  }
  glFlush();
  glutSwapBuffers();
}//end: CallBackDisplayFunc

/*!
Routine to cycle through vehicles and see what lights they see
This is the Glutmaster idle function
*/
void NRWindow::CallBackIdleFunc(void){
  list<Binoculus>::iterator iter,lightiter;

    for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++) {
      if ((*iter).vehicletype!=FIXEDLIGHT){
      (*iter).look();
      //if (GS.brainversion==1)(*iter).think();
      (*iter).think();
      if ((*iter).vehicletype == SPECULATRIX){
	(*iter).updatespecu();
	//(*iter).lefteye.clearRetina();// probably pointless, but trying to track down a bug.
      }else{
	(*iter).updatestate();
      }
      if(GS.logging)(*iter).log.addcolumn((*iter).getth(),(*iter).getx(),(*iter).gety()); // theta x y
      }
    }
    for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++) {
      if ((*iter).vehicletype!=FIXEDLIGHT){
      eulerstep(* iter);// don't euler step lights
      worldphysics(* iter);
      }
    }
    //}
    {
      int previoustime=globaltime;
      globaltime = glutGet(GLUT_ELAPSED_TIME);
      globallooptime=globaltime-previoustime;
    }
  CallBackDisplayFunc();
}//end: callbackidlefunction

//-----------------------------------------------------------------------------

void makealight(){
  (*globalNVselect).vehicletype=FIXEDLIGHT;
  (*globalNVselect).name="L";
  (*globalNVselect).setcolour(GREEN);
}

void makeashyseeker(){
  (*globalNVselect).vehicletype=SHYLIGHTSEEKER;
  (*globalNVselect).name="Shy Seeker";
  (*globalNVselect).setEyeDivergence(GS.ShySeeker[4]*M_PI/180,0.4,0.1);
  (*globalNVselect).setMIBs(GS.ShySeeker[0],GS.ShySeeker[1],GS.ShySeeker[2],GS.ShySeeker[3]);// set the mass, inertia and damping
  //(*globalNVselect).setSSgains(GS.ShySeeker[5],GS.ShySeeker[6]);
  (*globalNVselect).setcolour(BLUE);

}

void makeanagressiveseeker(){
  (*globalNVselect).vehicletype=AGGRESSLIGHTSEEKER;
  (*globalNVselect).name="Aggressive Seeker";
  (*globalNVselect).setEyeDivergence(.45,0,0);
  (*globalNVselect).setcolour(BLUE);
}

/// Eulerstep. May need version for Monoculus, or a mechanism to allow integration of both 
// Alternatively, we can move it back into the class and call with deltat
void eulerstep(Binoculus &nbv){
  //  float deltat=.01;
  float deltat=GS.DeltaT;
  //nbv.updatestate();
 
  for (int ii=0;ii<5;ii++){// integrate 5 states
    nbv.x[ii]=nbv.x[ii]+nbv.dotx[ii]*deltat;
  }
  nbv.updateA();
}

/// World physics
void boundaryfolding(Binoculus &nbv){

  float rwall=GS.worldboundary;
  float lwall=-GS.worldboundary;
  float twall=GS.worldboundary;
  float bwall=-GS.worldboundary;
   
 //left wall
  if (nbv.x[3]>rwall)
    nbv.x[3]+=(lwall-rwall);
  if (nbv.x[3]<lwall)
    nbv.x[3]+=(rwall-lwall);
  if (nbv.x[4]>twall)
    nbv.x[4]+=(bwall-twall);
  if (nbv.x[4]<bwall)
    nbv.x[4]+=(twall-bwall);
}
void boundarycollisions(Binoculus &nbv){
  /*  float rwall=1.7;
  float lwall=-1.7;
  float twall=1.7;
  float bwall=-1.7;
  */ 
  float rwall=GS.worldboundary;
  float lwall=-GS.worldboundary;
  float twall=GS.worldboundary;
  float bwall=-GS.worldboundary;
   
 //left wall
  if (nbv.x[3]>rwall && nbv.x[2] > -M_PI/2 && nbv.x[2] < M_PI/2){
    nbv.x[2]=M_PI-nbv.x[2];
  }
  //right wall
  if (nbv.x[3]<lwall && (nbv.x[2] > M_PI/2 || nbv.x[2] < -M_PI/2)){
    nbv.x[2]=M_PI-nbv.x[2];
  }

  //top wall
  if (nbv.x[4]>twall && nbv.x[2]>0 ){
    nbv.x[2]=-nbv.x[2];
  }
  //bottom wall
  if (nbv.x[4]<bwall && nbv.x[2]<0 ){
    nbv.x[2]=-nbv.x[2];
  }
}


void worldphysics(Binoculus &nbv){
  if (GS.worldtype==1)// torus world
    boundaryfolding(nbv);
  else
    boundarycollisions(nbv);
}

/// Write out the logs for all the vehicles to a matlab/octave file
/// Candidate for a friend function to binoculus, writelog(name,var,lognum)
void writevehiclelogs(string fname,char var,int lnum){
  list<Binoculus>::iterator iter;

  std::ofstream sfid(fname.c_str(), ios::out | ios::binary);

  if(GS.logging){
  if (sfid.fail()){
    cout<<"Could not save file "+fname<<endl;
  }else{
    for (iter=globalBvl.begin(); iter != globalBvl.end(); iter++) {
      //-char buffer2[100];
      //-sprintf(buffer2,"V%c%d",var,(*iter).getid());
      std::ostringstream Convert;
      Convert << "V"<<var<<(*iter).getid();
      std::string buffer2= Convert.str();
      //cout << buffer2<<endl;
      if (lnum==2)
	(*iter).log2.writematlabv4(sfid,buffer2);
      else
	(*iter).log.writematlabv4(sfid,buffer2);
      cout <<"Writing log "<<lnum<<" for "<<(*iter).name <<endl;
    }
    sfid.close();
  }
  }else{// not logging
    cout << "Please set logging on in bv.ini"<<endl;
  }
}



//---------------------------------------------------------------


// Main function for the program
int main(int argc, char *argv[]){
  list<Binoculus>::iterator iter;
  char const * filename="bv.ini";

  if (GS.ini(filename,0))
    cout << "Warning: could not open bv.ini, using defaults "<<endl;
  GS.inspect();

  // Output the list values, by repeatedly getting the item from
  // the "front" of the list, outputting it, and removing it
  // from the front of the list.

  Binoculus Alice("Alice",-M_PI/4,0,0,GREEN);
  //Binoculus SS("Shy Seeker",-M_PI/4,.56,-.52,OFF);
  Binoculus April("April",0.523599,-.85,-.85,GREEN); //30degrees
  //Binoculus George("George",-.78,-0.85,0.85,OFF);


  globalBvl.push_back (Alice);
  //globalBvl.push_back (SS);
  globalBvl.push_back (April);
  globalNVselect=globalBvl.begin();/// Need to keep at least one item in the list


  /*
  addbeast(-1.5,-1.0,FIXEDLIGHT);
  addbeast(-1.0,-0.5,SHYLIGHTSEEKER);
  addbeast(-0.5,0.0,AGGRESSLIGHTSEEKER);
  addbeast(0.0,0.5,SHYLIGHTPHOBE);
  addbeast(0.5,1.0,PREY);
  addbeast(-2.0,1.0,BRIGHTPREY);
  addbeast(1.0,1.5,PREDATOR);
  addbeast(0.0,0.0,FIXEDLIGHT);
  addbeast(0.0,2.2,FIXEDLIGHT);
  addbeast(2.0,2.2,FIXEDLIGHT);

  ncvselect=bovalist.begin();/// Need to keep at least one item in the list
  */
  // Create constant iterator for list.

  // Iterate through list and output each element.
  /*
  for (iter=bovalist.begin(); iter != bovalist.end(); iter++)
  {
    (*iter).status(1);
  }
  */
  
  //  dographicsstuff();
  char windowtitle[]="Neuro robots";
   glutMaster   = new GlutMaster();    

   Window = new NRWindow(glutMaster,
			       viewwidth, viewheight,    // width, height
			       10, 0,    // initPosition (x,y)
			       windowtitle); // title
   

   glutMaster->CallGlutMainLoop();

  return EXIT_SUCCESS;
}
