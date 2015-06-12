////////////////////////////////////////////////////////////////
//                                                            //
// glutWindow.c++                                             //
// beta version 0.3 - 9/9/97)                                 //
//                                                            //
// George Stetten and Korin Crawford                          //
// copyright given to the public domain                       //
//                                                            //
// Please email comments to email@stetten.com                 //
//                                                            //
////////////////////////////////////////////////////////////////

#include "glutWindow.h"

GlutWindow::GlutWindow(void){

}

GlutWindow::~GlutWindow(){

}

void GlutWindow::CallBackDisplayFunc(void){

                             //dummy function
}

void GlutWindow::CallBackIdleFunc(void){

                             //dummy function
}

void GlutWindow::CallBackKeyboardFunc(unsigned char key, int x, int y){

   key; x; y;                //dummy function
}

void GlutWindow::CallBackMotionFunc(int x, int y){

   x; y;                     //dummy function
}

void GlutWindow::CallBackMouseFunc(int button, int state, int x, int y){

   button; state; x; y;      //dummy function
}

void GlutWindow::CallBackPassiveMotionFunc(int x, int y){

   x; y;                     //dummy function
}

void  GlutWindow::CallBackReshapeFunc(int w, int h){

   w; h;                     //dummy function
}
   
void GlutWindow::CallBackSpecialFunc(int key, int x, int y){

   key; x; y;
}   

void GlutWindow::CallBackVisibilityFunc(int visible){

   visible;                  //dummy function
}

void GlutWindow::SetWindowID(int newWindowID){

   windowID = newWindowID;
}

int GlutWindow::GetWindowID(void){

   return( windowID );
}







