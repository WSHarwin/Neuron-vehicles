/// @todo Get perspective transform sorted out!
/// @todo Fix the camera to one of the vehicles
#include <iostream>
#include <cmath>

//#include "glutMaster.h"
#include "nrWindow.h"
#include "bvglobalini.h"


//using namespace std;
//using std::ifstream;
//using std::ofstream;
//using std::ios;
using std::cout;
using std::endl;
using std::string;

void Set_lights();
void Draw_lights();
void writevehiclelogs(string fname,char var,int lnum);

GLfloat light0_position[] ={1.0, -0.1, 2.0, 0.0};
GLfloat light1_position[] ={-1.0, 0.0, 2.0, 0.0};

enum {
  NONE, START, STOP, NAMES, NOLIGHT, VOLUME, SHADOW, QUIT, AERIALVIEW,GROUNDVIEW,RELOADINI,SAVELOGS,SHOWTITLES};
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


void drawRobot(float x,float y,float theta,float eyeangle,float robotlengthA,float robothalfwidth,float eyeposx, int eyeconeang,string & title, int flag);



GlutMaster     * glutMaster;
NRWindow     * Window      = 0;
GLfloat orthratio=2.5;
//GLfloat aspect=1;
GLfloat globalphi = 0;     /* in degrees (-150) */
GLfloat globaltheta = 0;  /* in degrees (30)*/
GLfloat cameraX=0;
GLfloat cameraY=0;
GLfloat cameraZ=10;


extern bool aerialview;
extern int viewheight;
extern int viewwidth;
extern readinifile GS; //global singleton - initialised from a file.
bool showtitles=true;

//-------------------------------------------------

void
showMessage(GLfloat x, GLfloat y, GLfloat z, string & message)
{
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glTranslatef(x, y, z);
  glScalef(.003, .003, .003);
  for (int ii=0; ii<message.length(); ++ii){
    glutStrokeCharacter(GLUT_STROKE_ROMAN,message.at(ii));    
  }

  /*  while (*message) {
    glutStrokeCharacter(GLUT_STROKE_ROMAN, *message);
    message++;
    }*/
  glEnable(GL_LIGHTING);
  glPopMatrix();
}


#define MAXWHPTS 100
GLenum initwheelpts=GL_TRUE;
GLfloat Lwheel[MAXWHPTS][3],Rwheel[MAXWHPTS][3],wheelN[MAXWHPTS][3];

static void init_wheel(int n, GLfloat radius, GLfloat width)
{
  int i;
  GLfloat ang;
  //  if (n>=MAXWHPTS){printf("error %d\n",__LINE__);exit(1);}
  if (n>=MAXWHPTS){  cout << "error " << __LINE__ <<endl ;exit(1);}

  for(i=0;i<n;i++){
    ang=(i*2.0*M_PI)/((GLfloat)n);
    Lwheel[i][0]=radius*sin(ang);
    Lwheel[i][1]=width/2;    
    Lwheel[i][2]=radius*cos(ang);
    Rwheel[i][0]=radius*sin(ang);
    Rwheel[i][1]=-width/2;    
    Rwheel[i][2]=radius*cos(ang);
    wheelN[i][0]=cos(ang);
    wheelN[i][1]=0;    
    wheelN[i][2]=sin(ang);
   }
  initwheelpts=GL_FALSE;
}


/* Create vertices for a wheel with axis along y. */ 
/* Could perhaps use the gluCylinder comand. */
static void drawWheel(int n, GLfloat radius, GLfloat width)
{
  int i,j;

  if (initwheelpts) init_wheel(n,radius,width); // should go in init section eventually
  glBegin(GL_TRIANGLE_FAN);

  glNormal3f(0.0, 1.0, 0.0);
  glVertex3f(0.0,width/2,0.0);
  for(i=0;i<n;i++){
    glVertex3fv(&Lwheel[i][0]);
  }
  glVertex3fv(&Lwheel[0][0]);
  glEnd();

  glBegin(GL_TRIANGLE_FAN);
  glNormal3f(0.0, -1.0, 0.0);
  glVertex3f(0.0,-width/2,0.0);
  for(i=0;i<n;i++){
    glVertex3fv(&Rwheel[i][0]);
   }
  glVertex3fv(&Rwheel[0][0]);
  glEnd();

  glBegin(GL_QUAD_STRIP);
  for(i=0,j=1;j<n;i++,j++){
    glNormal3fv(&wheelN[i][0]); 
    glVertex3fv(&Lwheel[i][0]);
    glVertex3fv(&Rwheel[i][0]);
    glVertex3fv(&Lwheel[j][0]);
    glVertex3fv(&Rwheel[j][0]);
  }
  glNormal3fv(&wheelN[n-1][0]);
  glVertex3fv(&Lwheel[n-1][0]);
  glVertex3fv(&Rwheel[n-1][0]);
  glVertex3fv(&Lwheel[0][0]);
  glVertex3fv(&Rwheel[0][0]);
  glEnd();
}

GLfloat bright_red_ambient[] = {.8, 0.0, 0.0, 1.0};
GLfloat bright_red_specular[] ={0.9, 0.1, 0.1, 1.0};
GLfloat red_ambient[] = {0.5, 0.0, 0.0, 1.0};
GLfloat red_plastic_diffuse[] ={0.5, 0.0, 0.0,1.0}, 
  red_plastic_specular[] ={0.7, 0.6, 0.6, 1.0}, 
  red_plastic_shininess = 32.0;
GLfloat no_ambient[]={0.0, 0.0, 0.0, 1.0};
GLfloat mat_specular[]={1.0, 1.0, 1.0, 1.0};
GLfloat wheel_ambient[] = {0.70, 0.7, 0.0, 1.0};// wheels are yellow.
GLfloat wheel_specular[] ={0.70, 0.7, 0.0, 1.0};
GLfloat blue_ambient[] = {0.0, 0.0, 0.7, 1.0};
GLfloat blue_specular[] = {0.0, 0.0, 0.7, 1.0};
GLfloat yellow_ambient[] = {1.0, 1.0, 0.0, 1.0};
GLfloat yellow_specular[] ={1.0, 1.0, 0.0, 1.0};
GLfloat bright_green_ambient[] = {0.0, 0.9, 0.0, 1.0};
GLfloat bright_green_specular[] ={0.1, 0.9, 0.1, 1.0};
GLfloat lblue_ambient[] = {0.0, 0.7, 0.7, 1.0};
GLfloat lblue_specular[] ={0.0, 0.7, 0.7, 1.0};
//GLfloat green_ambient[] = {0.0, 0.2, 0.0, 1.0};
GLfloat green_specular[] ={0.00, 0.2, 0.00, 1.0};
/// @todo sort out colours and lights so that a wider range of hues are possible!
/// From KColorChooser
GLfloat gold[]={255,215,0};
GLfloat magenta[]={255,0,255};
GLfloat pink[]={255, 192, 203};
GLfloat sienna[]={160, 82, 45};
GLfloat turquoise[]={64, 224, 208};
GLfloat violet[]={238, 130, 238};
GLfloat wheat[]={245, 222, 179};
GLfloat chocolate[]={210, 105, 30};

/// Draw Object selector (i.e. a sphere to show which vehicle has been selected)
void
drawObjectSelector(float x,float y)
{
//  bool haslight=true;
  
  // Draw a box body
  glPushMatrix();
    glTranslatef(x,y,0);
 //   glPushMatrix();                             // Turn on the selector
          glTranslatef(0,0.0,0.1);
	  glMaterialfv(GL_FRONT, GL_SPECULAR, red_plastic_specular);
	  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	  glutSolidSphere(0.1,8,8);
 //   glPopMatrix();      
  glPopMatrix();

}


// Draw Fixed light
void
drawFixedLight(float x,float y)
{
  glPushMatrix();
    glTranslatef(x,y,0);
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
      glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
      glutSolidCube(0.2);
  glPopMatrix();
}


/// Draw a Mobile robot given state vector - looks like eye angle is
/// in radians and eye cone angle is in degrees!
void
drawRobot(float x,float y,float theta,float eyeangle,int eyeconeang,string & title, int flag)
{
  //float robothalfwidth=.15,robotlength=.5,wheelrad=.1,eyepos_x=0.4;
  float robotlength=.5,robothalfwidth=.15,eyepos_x=0.4;
  drawRobot(x,y,theta,eyeangle,robotlength,robothalfwidth,eyepos_x,eyeconeang,title, flag);

}


void
drawRobot(float x,float y,float theta,float eyeangle, float robotlengthA,float robothalfwidthA, float eyeposA, int eyeconeang,string & title, int flag)
{

  //  GLfloat x, GLfloat y, GLfloat theta;
  bool haslight=false;
  GLfloat robothalfwidth=.15,robotlength=.5,wheelrad=.1,eyepos_x=0.4;

  robotlength=robotlengthA;
  if(robothalfwidth<0.02)
    robothalfwidth=0.02;
  else
    robothalfwidth=robothalfwidthA;
  eyepos_x=eyeposA;


  //Eye cone 
  if (eyeconeang<30)eyeconeang=30;
  if (eyeconeang>130)eyeconeang=130;
  GLfloat eyeconeht=0.2-(eyeconeang-30)*.0015;//0.5 to 0.2
  GLfloat eyeconebase=0.1+(eyeconeang-30)*.001;	//0.1 to 0.2
  
  // Draw a box body
  glPushMatrix();//+1
    glTranslatef(x,y,0);
/*      if(haslight){
        glPushMatrix();                             // has a light
          glTranslatef(0,0.0,0.1);
	  glMaterialfv(GL_FRONT, GL_SPECULAR, red_plastic_specular);
	  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	  glutSolidSphere(0.1,8,8);
        glPopMatrix();
      }*/
//    if(false){
//	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
//        glutSolidCube(0.2);
//    }else{

    //if(title!=NULL)showMessage(0.3,0.0, 0.5,title);

    if(showtitles)showMessage(0.3,0.0, 0.5,title);
      glRotatef(theta*180/M_PI, 0.0, 0.0, 1.0);
      glPushMatrix();                             // Left wheel +2
        glTranslatef(0,robothalfwidth,wheelrad);
	glMaterialfv(GL_FRONT, GL_SPECULAR, wheel_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, wheel_ambient);
	drawWheel(16,wheelrad,0.1);
      glPopMatrix();//+1
      glPushMatrix();                             // Right wheel+2
        glTranslatef(0,-robothalfwidth,wheelrad);
	drawWheel(16,wheelrad,0.1);
      glPopMatrix();//+1
      glPushMatrix();                            // Right eye +2
        if (flag!=SPECULATRIX){
        glTranslatef(eyepos_x,-robothalfwidth,wheelrad*2);
	glMaterialfv(GL_FRONT, GL_SPECULAR, lblue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	glutSolidSphere(eyeconebase/2,8,8);
	glRotatef(-90, sin(eyeangle), cos(eyeangle), 0.0);
	  //          glTranslatef(.45,-robothalfwidth,wheelrad*2);
	  //	  glRotatef(-90, 0.4, 0.80, 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_ambient);
	glutSolidCone(eyeconebase,eyeconeht,5,4);// base .1 height .2
	}
      glPopMatrix();//+1
      glPushMatrix();                            // Left eye +2
        if (flag==SPECULATRIX){
          glTranslatef(eyepos_x,0,wheelrad*2);
        }else{
          glTranslatef(eyepos_x,robothalfwidth,wheelrad*2);
        }
	glMaterialfv(GL_FRONT, GL_SPECULAR, lblue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	glutSolidSphere(eyeconebase/2,8,8);
	glRotatef(-90, -sin(eyeangle), cos(eyeangle), 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue_ambient);
//	glutSolidCone(.1,.2,5,4);
//	glutSolidCylinder(eyeconebase,.1,5,4);
	glutSolidCone(eyeconebase,eyeconeht,5,4);
      glPopMatrix();//+1

      glPushMatrix();                             // Body+2
        glTranslatef(robotlength/3,0.0,0.12);
	glScalef(robotlength,2.0*robothalfwidth,wheelrad+.02);
//	if (robot->thing == PREDATOR || robot->thing == AGGRESSLIGHTSEEKER){
	switch (flag){
	  case FIXEDLIGHT://FIXEDLIGHT
	    //break;
	  case SHYLIGHTSEEKER: // SHYLIGHTSEEKER
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, lblue_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, lblue_specular);
	    break;
	  case AGGRESSLIGHTSEEKER: // AGGRESSLIGHTSEEKER
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, green_specular);
	    break;
	  case SHYLIGHTPHOBE: //SHYLIGHTPHOBE
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bright_green_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, bright_green_specular);
	    break;
	  case PREY: //PREY
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, yellow_specular);
	    break;
	  case PREDATOR: // PREDATOR
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bright_red_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, bright_red_specular);
	    break;
	  case SPECULATRIX: // Speculatrix
	    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, bright_red_ambient);
	    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	    break;
	  default: 
	    break;
	
	}//end of else
	glutSolidCube(1);// Body of robot
      glPopMatrix();//+1
  //  }
    glPopMatrix();

}

void drawBoundary(void){/// todo fix the bounary def from brait.ini
  GLfloat boundary=GS.worldboundary;
  GLfloat boundary_white[] = {1.0, 1.0, 0.9, 1.0};//
  glMaterialfv(GL_FRONT, GL_SPECULAR, boundary_white);

  glLineWidth(2.5); 
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_LINE_STRIP);
  glVertex3f(boundary,boundary, 0.5);
  glVertex3f(-boundary,boundary, 0.5);
  glVertex3f(-boundary,-boundary, 0.5);
  glVertex3f(boundary,-boundary, 0.5);
  glVertex3f(boundary,boundary, 0.5);
  glEnd();
}

void drawFloorTile(GLfloat x,GLfloat y,GLfloat w){
  static GLfloat floorNormal[]={0.0, 0.0, 1.0};
  GLfloat V1[3],V2[3],V3[3],V4[3];
  V1[0]=x+w;V1[1]=y+w;V1[2]=0;
  V2[0]=x-w;V2[1]=y+w;V2[2]=0;
  V3[0]=x-w;V3[1]=y-w;V3[2]=0;
  V4[0]=x+w;V4[1]=y-w;V4[2]=0;
  glBegin(GL_POLYGON);
   glNormal3fv(floorNormal);
   glVertex3fv(V1);
   glVertex3fv(V2);
   glVertex3fv(V3);
   glVertex3fv(V4);
  glEnd();
}

void drawFloor(void){
  //GLfloat tw=6/5;//half the tile width
  GLfloat tw=2;//half the tile width
  GLfloat floor_specular_brown[] = {0.42, .30, 0.06, 1.0};//brown
  GLfloat floor_specular_green[] = {0.1, .30, 0.0, 1.0};//green
  glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular_brown);
  glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, no_ambient);

  for (int ii=-2;ii<=2;ii++){
    for (int jj=-2;jj<=2;jj++){
      if((ii*3+jj)%2){// In fact only need (ii+jj)%2, 3 opens the way for more complex patterns
	glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular_brown);
      }else{
	glMaterialfv(GL_FRONT, GL_SPECULAR, floor_specular_green);
      }
      drawFloorTile(ii*2*tw,jj*2*tw,tw);
    }
  }
  drawBoundary();

}


//----------------------------------------------------



void
menu(int mode)
{
  cout << "mode " <<mode<<endl;
  if (mode == VOLUME)cout <<"volume selected"<<endl;
  if (mode == QUIT) exit(0);
  if (mode == STOP) Window->StopMoving(glutMaster);   // stop idle function
  if (mode == START) Window->StartMoving(glutMaster);   // stop idle function
  if (mode == AERIALVIEW) aerialview=true;
  if (mode == GROUNDVIEW) aerialview=false;
  if (mode == SHOWTITLES) if(showtitles)showtitles=false;else showtitles=true;
  if (mode == RELOADINI){
    if (GS.ini("bv.ini",0))
      cout << "Warning: could not open bv.ini, using defaults "<<endl;
  }
  if (mode == SAVELOGS){
	writevehiclelogs("pos1.mat",'p',1);
	writevehiclelogs("eye2.mat",'e',2);
	if(GS.logging)
	  cout <<"Matlab dump"<<endl;
  }

  //  rendermode = mode;
  //  glutPostRedisplay();
  //if (aerialview) cout << "aerial"<<endl;
  //if (!aerialview) cout << "ground"<<endl;
}

void drawLights(){
  glPushMatrix();
    glColor4f(1.0, 0.0, 0.0, 1.0);
    glTranslatef(light1_position[0],light1_position[1],light1_position[2]);
    glutWireSphere(0.2, 10, 10);
  glPopMatrix();
  glPushMatrix();
    glTranslatef(light0_position[0],light0_position[1],light0_position[2]);
    glutWireSphere(0.2, 10, 10);
  glPopMatrix();
  //  cout <<"Draw lights"<<endl;
}



void
Set_lights(){
  GLfloat white_light[] ={1.0, 1.0, 1.0, 1.0};
  GLfloat grey_light[] ={0.3, 0.4, 0.5, 1.0};

  glClearColor(0.0, 0.0, 0.0, 0.0);

  glShadeModel(GL_SMOOTH);
  //GL_SMOOTH else glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glLightfv(GL_LIGHT0,GL_POSITION,light0_position);
  glLightfv(GL_LIGHT0,GL_DIFFUSE,white_light);
  glLightfv(GL_LIGHT0,GL_SPECULAR,white_light);

  glLightfv(GL_LIGHT1,GL_POSITION,light1_position);
  glLightfv(GL_LIGHT1,GL_DIFFUSE,grey_light);
  glLightfv(GL_LIGHT1,GL_SPECULAR,grey_light);
}




NRWindow::NRWindow(GlutMaster * glutMaster,
                       int setWidth, int setHeight,
                       int setInitPositionX, int setInitPositionY,
                       char * title){
  
  width  = setWidth;               
  height = setHeight;
  
  initPositionX = setInitPositionX;
  initPositionY = setInitPositionY;
  
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(initPositionX, initPositionY);
  glViewport(0, 0, width, height);   // This may have to be moved to after the next line on some platforms

  glutMaster->CallGlutCreateWindow(title, this);
  
   //   Set_lights();
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  GLdouble aspect=(1.0*viewwidth)/viewheight;
  glLoadIdentity();
  gluPerspective(45.0, aspect, 0.001, 100.0); // perspective transformation (changing this will require changing the 830 constant used in 's' select)

  //  glOrtho(-80.0, 80.0, -80.0, 80.0, -500.0, 500.0);
  //glOrtho(-8.0, 8.0, -8.0, 8.0, -100.0, 100.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  Set_lights();
   //   Draw_lights();
   //   glRotatef(60, 1, 1, 1);


  glutCreateMenu(menu);
  // glutAddMenuEntry("No Shadows", NONE);
  glutAddMenuEntry("Start world", START);
  glutAddMenuEntry("Stop world", STOP);
  glutAddMenuEntry("Aerial view", AERIALVIEW);
  glutAddMenuEntry("Ground view", GROUNDVIEW);
  glutAddMenuEntry("Toggle titles", SHOWTITLES);
  glutAddMenuEntry("Reload ini file", RELOADINI);
  glutAddMenuEntry("Save log files", SAVELOGS);
  // glutAddMenuEntry("Toggle names", NAMES);
  // glutAddMenuEntry("No Light", NOLIGHT);
  // glutAddMenuEntry("Show Volume", VOLUME);
  // glutAddMenuEntry("Shadows", SHADOW);
  glutAddMenuEntry("Quit", QUIT);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

}

NRWindow::~NRWindow(){

   glutDestroyWindow(windowID);
}




void NRWindow::CallBackReshapeFunc(int w, int h){
  viewheight=h;
  viewwidth=w;

  glMatrixMode(GL_PROJECTION); // projection matrix is active
  GLdouble aspect=(1.0*viewwidth)/viewheight;
  //glLoadIdentity();
  //gluPerspective(45.0, aspect, 0.001, 100.0); // perspective transformation
  glMatrixMode(GL_MODELVIEW); // return to modelview mode

  cout << "Reshape called" <<endl;
  // oct14 - need to clean up the following.
  //float ratio =  ((float) w) / ((float) h); // window aspect ratio
  //glLoadIdentity(); // reset the projection
  //gluPerspective(45.0, ratio, 0.1, 100.0); // perspective transformation
  //  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  //glViewport(0, 0, w, h); // set viewport (drawing area) to entire window


  int val;
  if (w<h)val=w;else val=h;

  width = val;
  height= val;
//  glViewport(0, 0, (GLsizei) width, (GLsizei) height);
//  glMatrixMode(GL_MODELVIEW); // return to modelview mode
  //  glViewport(0, 0, width, height); 
  CallBackDisplayFunc();
}



//--------------------------------------------------



