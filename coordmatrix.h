/***************************************************************************
 *   Copyright (C) 2006-2014 by William Harwin                             *
 *   shshawin@reading.ac.uk                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
/**
\brief Update to coordmatrix class, primarily for coordinate tranforms
\par Coordinate transform matrix class

Suitable for planar coordinate transforms

The structure of the matrix is similar to matlab.
The matrix is stored in columns so for example a generalised matrix of 
vectors is stored as 
\f[ \left[\begin{array}{ c c c c }
s[0] & s[3] & s[6] & ...\\
s[1] & s[4] & s[7] & ...\\
s[2] & s[5] & s[7] & ...\end{array}
\right]
\f]


Note matrices and vectors are subscripted from 0 (against the convention), 
easy enough to change getval to index from 1. 

A 2x2 Homogeneous transform matrix  \f$ A_1 \f$ is of the form
\f[
\left[\begin{array}{ c c c }
R & R & x\\
R & R & y\\
0 & 0 & 1\end{array}
\right]
\f]


Assumptions are that the upper 2x2 is orthogonal, the right hand column is
the origin vector and the last row can be used for scaling.


\par{\bf Methods}

resize
savematlabv4
rows/cols
info
resize
getval
setval
addcolumn
trans (converts to a homogeneous transform)
inverse (only h.t.s)
trans -> ht create 3x3 homogeneous transform
inverse -> htinverse


@todo need to rethink the trans function (converts to homogeneous transform
@todo need to have two inverse functions, adjunct/det, and homogeneous (could have adjinverse and htinverse)
@todo rot(th) create 2x2 rotation transform
@todo transpose
@todo inverse as a calculation. 


\par{\bf Notes}

Uses the std vector class

Can also consider Trenkwalder's vector_math.h for a faster method
Other contendors 
 eigen3 - too big
 GMTK/GGT Generic Graphicss Toolkit http://ggt.sourceforge.net/


May consider keeping a tab on homogeneous transform matrices.



\return the return value


@author William Harwin
@todo put all the coordmatrix routines in a single .h file
*/



#ifndef COORDMATRIX_H
#define COORDMATRIX_H
#include <iostream>
#include <cmath>
#include <vector>
#include <cassert>
#include <fstream>
using std::ofstream;
using std::ios;

class Coordmatrix{
private:
    int mr;
    int nc;
 public:
    std::vector<float> myValues; /// Matrix is stored in column format aka matlab
 public:
    int size() const { return mr * nc; }
    inline int rows() const {return (mr);};	/// returns \b mr (num rows)
    inline int cols() const {return (nc);};	/// returns \b nc (num cols)
    //    bool ht; //true if a homogeneous transform
 public:
 public:// methods
    void info (void) {
      //std::cout <<<<std::endl
      for (int k=0; k<mr*nc; k++) {
          std::cout << myValues[k] << ' ';
      }
      std::cout << std::endl;
    }
    void resize (int r,int c) {
      if(myValues.size()==r*c){
	mr=r;nc=c;
      }
      if(myValues.size()>r*c){// reducing the size 
	myValues.resize(r*c,0);
	mr=r;nc=c;
      }
      if(myValues.size()<r*c){// increase the size 
	myValues.resize(r*c,0);
	mr=r;nc=c;
      }
    }
    /// write the matrix in matlab version4 to an open file. Opening
    /// and closing file must be handled elsewhere -
    /// writematlabv4(fid,name)
    int writematlabv4(std::ofstream & sfid,std::string name){
      typedef struct {
	int type;
	int mrows;
	int ncols;
	int imagf;
	int namelen;
      } Fmat;
      Fmat hdr;
      double * b;
      int len=mr*nc;
      b = new double [len]; 
      for (int ii=0;ii<len;ii++){
	b[ii]=myValues[ii];
      }
      hdr.namelen=name.length()+1;
      hdr.mrows=mr;
      hdr.ncols=nc;
      hdr.imagf=0;
      hdr.type=0;
      sfid.write ((char*)&hdr, sizeof (Fmat));
      sfid.write (name.c_str(), hdr.namelen);
      sfid.write ((char*) b, mr*nc*sizeof (double));
      return 0;
    }
    inline float getval(int ii,int jj) const {
      // alternative is to use return(myValues.at(ii+jj*mr));
      assert(ii<mr);
      assert(jj<nc);
      return(myValues[ii+jj*mr]);
    };

    inline void setval(int ii,int jj,float xx){
      int l;
      assert(ii<mr);
      assert(jj<nc);
      l=ii+jj*mr;
      myValues[l]=xx;
    };

///Addcolumn, adds a column to the matrix
///@todo Need a decision about returning an error
    void addcolumn(float a, float b, float c){/// add column to a three row matrix 
      if (mr==0&& nc==0){
	mr=3;/// allows adding column to an empty matrix
      }
      if (mr == 3){
	int i;
	int len=mr*nc;
	myValues.resize(len+3,0);
	myValues[len]=a;
	myValues[len+1]=b;
	myValues[len+2]=c;
	nc+=1;
      }else std::cerr << "should be a three row matrix"<<std::endl;
    }
    void addcolumn(float a, float b, float c,float d, float e, float f){/// add column to a six row matrix 
      if (mr==0&& nc==0){
	mr=6;/// allows adding column to an empty matrix
      }
      if (mr == 6){
	int i;
	int len=mr*nc;
	myValues.resize(len+6,0);
	myValues[len]=a;
	myValues[len+1]=b;
	myValues[len+2]=c;
	myValues[len+3]=d;
	myValues[len+4]=e;
	myValues[len+5]=f;
	nc+=1;
      }else std::cerr << "should be a three row matrix"<<std::endl;
    }
///Save to a file in matlab version 4 save. File must be open for writing
///@todo Need to write
    void savematlabv4(std::ios *fid){/// add column to a three row matr
      // check for a valid fid (is ios the right format..

    }
    void trans(float th,float x, float y){/// Trans - convert the matrix to a Homeogeneous transform (.cpp)
      if ((mr==3) && (nc==3)){
	myValues[0]=cos(th); myValues[3]=-sin(th); myValues[6]=x;
	myValues[1]=sin(th); myValues[4]= cos(th); myValues[7]=y;
	myValues[2]=0; myValues[5]=0;  myValues[8]=1;
      }else{
	std::cerr<<"Warning: Matrix conversion to homeogeneous transform failed"<< std::endl;
      }
    }
    void trans(float th){/// update the rotation part of the homogeneous transform (.cpp)
      if ((mr==3) && (nc==3)){
	myValues[0]=cos(th); myValues[3]=-sin(th);
	myValues[1]=sin(th); myValues[4]= cos(th);
	myValues[2]=0; myValues[5]=0;  myValues[8]=1;
      }else{
	std::cerr<<"Warning: Matrix conversion to homeogeneous transform failed"<< std::endl;
      }
    }



/*!
Inverse

if\f$ H=\left[\begin{array}{cc}R & x\\0 & 1\end{array}\right]\f$
Then \f$H^{-1}=\left[\begin{array}{cc}R^T & -R^Tx\\0 & 1\end{array}\right]\f$

Calculates the inverse inplace 
*/

    void inverse(){/// assumes the matrix is a Homeogeneous transform 
      if ((mr==3) && (nc==3)){
	float tmp,nx,ny;
	tmp=myValues[3];// transpose R
	myValues[3]=myValues[1];
	myValues[1]=tmp;
	nx=-(myValues[0]*myValues[6]+myValues[3]*myValues[7]);
	ny=-(myValues[1]*myValues[6]+myValues[4]*myValues[7]);
	myValues[6]=nx;
	myValues[7]=ny;
      }else{
	std::cerr<<"Warning: Matrix inverse of this type not yet implimented"<< std::endl;
      }    
    }



/*! Allow printing on cout with the << operator */
    friend std::ostream& operator<<(std::ostream& os,const Coordmatrix& m) {
      os <<"rows="<< m.mr<<", cols= "<<m.nc<<" "<<std::endl;
      for (int r=0;r<m.mr;r++) {
	for (int c=0;c<m.nc;c++) {
	  os << m.getval(r,c) << " ";
	}
	os <<std::endl;
      }
      return os;
    }
/*!
Currently overloading the operators, "=+*-" Could overload /  
*/
///Overloading operator =
    Coordmatrix &operator= (const Coordmatrix& a){
      mr=a.mr;
      nc=a.nc;
      myValues.resize(mr*nc,0);
// might be able to just do this.myValues=a.myValues since = overloaded for vectors
      for(int i=0;i<mr*nc;i++){
	myValues[i]=a.myValues[i];}
      return *this;
    }
/// Overloaded operator +
Coordmatrix operator+ (const Coordmatrix& a){
  Coordmatrix tmp(mr,nc);// this could be a cause of a memory leak - where does it get cleared away?
  if ((a.mr==mr) && (a.nc==nc)){
    for(int i=0;i<a.mr*a.nc;i++){
      tmp.myValues[i] = myValues[i]+a.myValues[i];
    }
  }else{
    std::cerr << "WARNING: Matrices for addition must be the same size"<< std::endl;
  }      
  return tmp;
}
/// Overloaded operator -
 Coordmatrix operator- (const Coordmatrix& a){
   Coordmatrix tmp(mr,nc);// this could be a cause of a memory leak - where does it get cleared away?
   if ((a.mr==mr) && (a.nc==nc)){
     for(int i=0;i<a.mr*a.nc;i++){
       tmp.myValues[i] = myValues[i]-a.myValues[i];
     }
   }else{
     std::cerr << "WARNING: Matrices for subtraction must be the same size"<< std::endl;
   }      
   return tmp;
 }

 Coordmatrix operator* (const Coordmatrix& a){
   int i,j,k;
   float sum,lv,rv;
   Coordmatrix tmp(mr,a.nc);// Resulting matrix is mr*a.nc
   if (nc==a.mr){
     for(i=0;i<mr;i++){
       for(j=0;j<a.nc;j++){
         sum=0;
         for(k=0;k<nc;k++){
	   lv=this->getval(i,k);
	   rv=a.getval(k,j);
	   sum += lv*rv;
	 }
	 tmp.setval(i,j,sum);
       }
     }
   }else {std::cerr << "WARNING: Matrices for multiplication must be compatible"<< std::endl;}
   return tmp;
 }

  //----------------------------constructors-----------------
///< m x n matrix initialised to zero
    Coordmatrix(int ii, int jj):myValues(ii*jj){  
      mr=ii;nc=jj;
    }
///< Construct a diagonal matrix
    Coordmatrix(int ii, float xx):myValues(ii*ii){ 
      int i;
      mr=ii;
      nc=ii;
      for(i=0;i<mr;i++)
	myValues[i*nc+i]=xx; 
    }
///< Construct a 2 element column matrix
    Coordmatrix(float a1, float a2):myValues(2){ 
      mr=2;nc=1;
      myValues[0]=a1;myValues[1]=a2;
    }
///< Construct a 3 element column matrix
 Coordmatrix(float a1, float a2, float a3):myValues(3){ 
      mr=3;nc=1;
      myValues[0]=a1;myValues[1]=a2;myValues[2]=a3;
    }
///< Construct a 4 element column matrix
 Coordmatrix(float a1, float a2, float a3, float a4):myValues(4){ 
      mr=4;nc=1;
      myValues[0]=a1;myValues[1]=a2;myValues[2]=a3;myValues[3]=a4;
    }
// default
    Coordmatrix (): myValues(9) {
      mr=0;nc=0;
      /*for (int k=0; k<9; k++) {
         myValues[k] = k;
	 }*/
    }

};
#endif
