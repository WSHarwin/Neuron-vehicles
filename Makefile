## make exe

#/usr/lib/i386-linux-gnu/libglut.so
#/usr/include/GL/freeglut.h
#/usr/include/GL/glut.h

CPPFILES=brait032.cpp glutMaster.cpp glutWindow.cpp nrWindow.cpp
INCLUDEFILES=bvglobalini.h  coordmatrix.h  glutMaster.h  glutWindow.h  NBV.h  nrWindow.h

FILES=Makefile Doxyfile neuronbrains.pdf bv.ini neuronbrains.org testbvini.cpp $(INCLUDEFILES) $(CPPFILES)




foo: # debug version
	g++ -g $(CPPFILES) -L/usr/local/lib -I/usr/include -lglut -lGL -lGLU

exe:
	g++ $(CPPFILES) -L/usr/local/lib -I/usr/include -lglut -lGL -lGLU
	g++ -o testbvini testbvini.cpp

pack:
	zip braitenberg_v032.zip $(FILES)

all: bv.m

doxygen:
	doxygen
	firefox doxy/html/index.html

# Next line should be commented out
# include makehelper
