# Neuron-vehicles
Sensory motor simulation of neuron controlled wheeled vehicles


You are welcome to modify and use this code for non-commercial
purposes providing you credit the origin (a web address is 
sufficient <>http://www.rdg.ac.uk/~shshawin/LN eg<endu>). If you make any clever
changes and are happy for these to be published on the web please let
me know so that I can update the program.  

This program uses opengl and the glut libraries to simulate a number
of 3D wheeled robots that seek out light sources. At a basic level the
robots can be Braitenberg vehicles that is light seekers and light
avoiders. But it is also possible to simulate predator prey
interactions etc.

<>https://github.com/WSHarwin/Neuron-vehicles  Download at<endu>

Versions compiled for Microsoft Windows are available at

http://www.personal.reading.ac.uk/~shshawin/LN/braitenberg.html 

however these have had only very rudimentary testing.


# Compiling

## Compiling under Linux/g++

Program needs libglut3-dev
 sudo apt-get install freeglut3-dev
This should also install opengl (mesa)

Compile with

g++ brait032.cpp nrWindow.cpp glutMaster.cpp glutWindow.cpp  -L/usr/local/lib  -lglut -lGL -lGLU

 - <>http:braitenberg_v032.zip source code in a zip file<endu>

** Compiling under Microsoft windows and VisualC++

Microsoft windows supports openGL but you will need to also install
GLUT or Freeglut. These provide \verb|glut.h, glut32.lib| as well as the
runtime \verb|Glut32.dll|. Freeglut provides in addition, 
\verb|freeglut.h, freeglut_ext.h, freeglut_std.h, freeglut.lib|, and \verb|freeglut.dll|.

To avoid compiling these from source I suggest using a site such as

\url{http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/}
for glut

or
\url{http://www.transmissionzero.co.uk/software/freeglut-devel/}
for freeglut

There may be hints from
http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/


### header file:

Glut header files *.h will go in a directory GL 
try
\begin{verbatim}
C:\Program Files (x86)\Microsoft Visual Studio *\VC\include\GL\glut.h
\end{verbatim}
The '*' matches your version of VS: 12.0 for VS2013, 11.0 for VS2012, 10.0 for VS2010, 9.0 for VS2008. You may have to create the include folder.


### library file:

Library (*.lib) files will go somewhere like


C:\Program Files (x86)\Microsoft Visual Studio *\VC\lib\glut32.lib

If you want to build 64-bit apps, put the 64-bit library file in:

C:\Program Files (x86)\Microsoft Visual Studio *\VC\lib\amd64\glut32.lib



# Running under Linux

Should run from the command line or from a folder

# Running under Windows



To run you will need the Glut32.dll (or the freeglut.dll) either in the same directory as 
brait.exe or in the windows system directory.

% <>http:windowsbrait.gz windows version </a>
% o http:brait.exe executable (brait.exe)</a>
% o http:glut32.dll  glut32.dll</a>
% o <>http:brait.c  brait.zip</a>




# Other operating systems, Mac OS, Android. iOS etc
It is probably possible to compile binaries for Mac OS, but the mobiles is likely to 
be more involved and the software is likely to need a redesign. I am happy to advise any
adventurous individuals who wish to try.


# User interface

This is not a particularly friendly interface. This is a consequence of the decision so as to keep
the code as simple as possible, and allow it to run on multiple
platforms. OpenGL and Glut are the two libraries (other than standard
C++) that are need to compile the code. GLU is also needed for gluLookAt
and gluPerspective but could be replaced.

There are some commands available by right clicking on the playing
field.  A left click is used to select a vehicle or light (although a
bug in the structure does not update the graphics immediately!).

Commands are either single letters, mainly lower case, or short
commands in upper case.  The mouse
is needed for 's' (select) ,'m' (move) and 'a' (add).


bv.ini is a human readable file of parameters that can be changed on
the fly. The first value to change is the Euler integration time
'Euler integrator' To find a working value, set up the environment
with a M.Speculatrix

When you run the executable you should have the playing field. Zoom in
and out with z/Z or o/O add a light with 'a' change it to
M.Speculatrix by typing 'MS' start the simulation with the space-bar or
'Start world' by right clicking on the playing field.  If it runs fast
slow it down by reducing the Euler integrator time, and vice-versa.
reload the bv.ini file with 'l'

Once the speed is reasonable you can change 'MS' into 'SS' 'AS' 'SP'
'AP' these always work on the selected object.  Add lights by moving
the mouse to where you want the light to be and typing 'a'

## Bugs/tips

Having changed bv.ini and reloaded with 'l', you still need to reset a
specific vehicle. Select that vehicle and type in the two letter
command for the new values to take effect. This is (probably) a minor
and fixable bug if anyone wants to contribute code.


## Uppercase two letter (mostly)  commands 
% Can't use OXYZ

Following commands are used to change the vehicles nature

 RL Reset to a light
 AP Aggressive Phobe
 SS Shy Seeker
 SP Shy Phobe
 AS Aggressive Seeker
 DD Aggressive Seeker (DD)
 DP Dumb Prey
 BP Bright Prey
 PR Predator
 TV Test Vehicle
 MS Machina Speculatrix
 DUMP Dump a Matlab file

% SL=savelog?

% \verbatiminput{tmpcmds.txt}
** Lowercase (mostly) single letter commands
A list of keyboard commands:
 - mouse dependent commands
   - a - add a B.V. at the mouse position
   - s - select the nearest N.B.V.
   - m move N.B.V. to mouse

 - general commands
   - $<$ - Change selected N.C.V. to next vehicle in list 
   - $>$ - Change selected N.C.V. to previous vehicle in list 
   -  e - erase the selected N.C.V. 
   -  f - rotate c.c.w.
   -  g - rotate c.w.
   -  h - help
   -  i - information on the selected vehicle
   -  l - load information from the bv.ini file
   -  m - move the item selected to the mouse position
   -  n - set selector to next beast
   -  p - set selector to previous beast
   -  o zoom out (identical to 'z')
   -  O zoom in (identical to 'Z')
   -  q quit (esc will also quit)
   -  r - reset world to flat
   -  spacebar - start simulation
   -  t - tilt down
   -  v - tilt up
   -  x/X y/Y z/Z move camera in x,y,z

Menu commands: (Right mouse click)
  Aerial view/Ground view
  Toggle labels
  Start and stop simulation
  Quit
  (was previously used to switch on lights, etc)


# Tips

In ground view (selected via the menu), lower the camera with Z/z to
get the 'driver's' view.

# Initialisation file
bv.ini is an editable file read in at the start and with the 'l' command.


## bv.ini

Setting values in bv.ini

Time constants are T=M/B linear and I/Brot, for rotations

The inertia  of a solid disk is $Mr^2/2$, so assuming a mass of .5 and a
dimension of .25, inertia would be around .000625

Predators that chase their prey tend to be heavier than their prey.



## testbvini

This is a helper program to ensure that values are being read correctly from
bv.ini. The output should be compatible with Matlab or Octave commands so can be
edited into a script (.m) file.



Written by William Harwin \begin{verbatim}w.s.harwin@reading.ac.uk\end{verbatim}
Copyright 2002-2015.
