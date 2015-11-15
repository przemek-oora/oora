#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <memory.h>
#include <malloc.h>
#include "papi.h"
#include <sched.h>

#define INDEX 100
#define SIZE 512


int mm1(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  int i,j,k; 
  double sum = 0;
  for (i = 0; i < SIZE; i++) { //rows in multiply
    for (j = 0; j < SIZE; j++) { //columns in multiply
      for (k = 0; k < SIZE; k++) { //columns in first and rows in second
	    sum = sum + first[i][k]*second[k][j];
	  } 
          multiply[i][j] = sum;
	  sum = 0;
    }
  }
  return 0;
}


int mm2(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  double sum = 0;
  for (i = 0; i < SIZE; i++) { //rows in multiply
    for (j = 0; j < SIZE; j++) { //columns in multiply
      for (k = 0; k < SIZE; k++) { //columns in first and rows in second
	    sum = sum + first[i][k]*second[k][j];
	  } 
          multiply[i][j] = sum;
	  sum = 0;
    }
  }
  return 0;
}

int mm3(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  register unsigned int local_size=SIZE; //<- OPT 2
  double sum = 0;
  for (i = 0; i < local_size; i++) { //rows in multiply
    for (j = 0; j < local_size; j++) { //columns in multiply
      for (k = 0; k < local_size; k++) { //columns in first and rows in second
	    sum = sum + first[i][k]*second[k][j];
	  } 
          multiply[i][j] = sum;
	  sum = 0;
    }
  }
  return 0;
}

int mm4(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  register unsigned int local_size=SIZE; //<- OPT 2
  double sum = 0;
  for (i = SIZE; i-- ; ) { //rows in multiply <- OPT 3
    for (j = SIZE; j-- ;) { //columns in multiply
      for (k = SIZE; k-- ; ) { //columns in first and rows in second
	    sum = sum + first[i][k]*second[k][j];
	  } 
          multiply[i][j] = sum;
	  sum = 0;
    }
  }
  return 0;
}


int mm5(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  register double sum = 0;
  for (i = SIZE; i-- ; ) { //rows in multiply <- OPT 3
    for (j = SIZE; j-- ;) { //columns in multiply
      for (k = 0; k < SIZE; ) {
        if(k<SIZE-8) { //<- OPT 4
	  sum = sum + first[i][k]*second[k][j]
	   + first[i][k+1]*second[k+1][j]
	   + first[i][k+2]*second[k+2][j]
	   + first[i][k+3]*second[k+3][j]
	   + first[i][k+4]*second[k+4][j]
	   + first[i][k+5]*second[k+5][j]
	   + first[i][k+6]*second[k+6][j]
	   + first[i][k+7]*second[k+7][j];
  	  k=k+8;
        }
        else {
	  sum = sum + first[i][k]*second[k][j];
	  k++;
	}
      }
      multiply[i][j] = sum;
      sum = 0;
    }
  }
  return 0;
}

int mm6(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  register double sum = 0;
  for (i = SIZE; i-- ; ) { //rows in multiply <- OPT 3
    for (j = SIZE; j-- ;) { //columns in multiply
      for (k=0; k<SIZE; ){
      if(k<SIZE-8) { //<- OPT 4
  sum = sum + first[i][k]*second[j][k] //<- OPT 5 (k<->j)
  + first[i][k+1]*second[j][k+1]
  + first[i][k+2]*second[j][k+2]
  + first[i][k+3]*second[j][k+3]
  + first[i][k+4]*second[j][k+4]
  + first[i][k+5]*second[j][k+5]
  + first[i][k+6]*second[j][k+6]
  + first[i][k+7]*second[j][k+7];
  k=k+8;
      }
      else {
  sum = sum + first[i][k]*second[j][k];
  k++;
      }
      }
      multiply[i][j] = sum;
      sum=0.0;
    }
  }
  return 0;
}

int mm7(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  register unsigned int local_size=SIZE; //<- OPT 2
  double sum = 0;
  for (i = 0; i<SIZE ; i++ ) { //rows in multiply <- OPT 3
    for (j=0; j<SIZE; ) {
      for (k=0; k<SIZE; ) {

  sum = sum + first[i][k]*second[j][k] //<- OPT 5 (k<->j)
     + first[i][k+1]*second[j][k+1]
   + first[i][k+2]*second[j][k+2]
   + first[i][k+3]*second[j][k+3]
   + first[i][k+4]*second[j][k+4]
   + first[i][k+5]*second[j][k+5]
   + first[i][k+6]*second[j][k+6]
   + first[i][k+7]*second[j][k+7];
        multiply[i][j]+=sum; sum=0;

  sum = sum + first[i][k]*second[j+1][k] //<- OPT 5 (k<->j+1)
     + first[i][k+1]*second[j+1][k+1]
   + first[i][k+2]*second[j+1][k+2]
   + first[i][k+3]*second[j+1][k+3]
   + first[i][k+4]*second[j+1][k+4]
   + first[i][k+5]*second[j+1][k+5]
   + first[i][k+6]*second[j+1][k+6]
   + first[i][k+7]*second[j+1][k+7];
        multiply[i][j+1]+=sum; sum=0;

  sum = sum + first[i][k]*second[j+2][k] //<- OPT 5 (k<->j+2)
     + first[i][k+1]*second[j+2][k+1]
   + first[i][k+2]*second[j+2][k+2]
   + first[i][k+3]*second[j+2][k+3]
   + first[i][k+4]*second[j+2][k+4]
   + first[i][k+5]*second[j+2][k+5]
   + first[i][k+6]*second[j+2][k+6]
   + first[i][k+7]*second[j+2][k+7];
        multiply[i][j+2]+=sum; sum=0;

  sum = sum + first[i][k]*second[j+3][k] //<- OPT 5 (k<->j+3)
     + first[i][k+1]*second[j+3][k+1]
   + first[i][k+2]*second[j+3][k+2]
   + first[i][k+3]*second[j+3][k+3]
   + first[i][k+4]*second[j+3][k+4]
   + first[i][k+5]*second[j+3][k+5]
   + first[i][k+6]*second[j+3][k+6]
   + first[i][k+7]*second[j+3][k+7];
        multiply[i][j+3]+=sum; sum=0;

  sum = sum + first[i][k]*second[j+4][k] //<- OPT 5 (k<->j+4)
     + first[i][k+1]*second[j+4][k+1]
   + first[i][k+2]*second[j+4][k+2]
   + first[i][k+3]*second[j+4][k+3]
   + first[i][k+4]*second[j+4][k+4]
   + first[i][k+5]*second[j+4][k+5]
   + first[i][k+6]*second[j+4][k+6]
   + first[i][k+7]*second[j+4][k+7];
        multiply[i][j+4]+=sum; sum=0;

  sum = sum + first[i][k]*second[j+5][k] //<- OPT 5 (k<->j+5)
     + first[i][k+1]*second[j+5][k+1]
   + first[i][k+2]*second[j+5][k+2]
   + first[i][k+3]*second[j+5][k+3]
   + first[i][k+4]*second[j+5][k+4]
   + first[i][k+5]*second[j+5][k+5]
   + first[i][k+6]*second[j+5][k+6]
   + first[i][k+7]*second[j+5][k+7];
        multiply[i][j+5]+=sum; sum=0;

  sum = sum + first[i][k]*second[j+6][k] //<- OPT 5 (k<->j+6)
     + first[i][k+1]*second[j+6][k+1]
   + first[i][k+2]*second[j+6][k+2]
   + first[i][k+3]*second[j+6][k+3]
   + first[i][k+4]*second[j+6][k+4]
   + first[i][k+5]*second[j+6][k+5]
   + first[i][k+6]*second[j+6][k+6]
   + first[i][k+7]*second[j+6][k+7];
        multiply[i][j+6]+=sum; sum=0;

  sum = sum + first[i][k]*second[j+7][k] //<- OPT 5 (k<->j+7)
     + first[i][k+1]*second[j+7][k+1]
   + first[i][k+2]*second[j+7][k+2]
   + first[i][k+3]*second[j+7][k+3]
   + first[i][k+4]*second[j+7][k+4]
   + first[i][k+5]*second[j+7][k+5]
   + first[i][k+6]*second[j+7][k+6]
   + first[i][k+7]*second[j+7][k+7];
        multiply[i][j+7]+=sum; sum=0;
  
  k=k+8;
      }
      j=j+8;
    }
  }
  return 0;
}


int mm8(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  double sum = 0;
  for (i = 0; i<SIZE ; i++ ) { //rows in multiply <- OPT 3
    for (j=0; j<SIZE; ) {
      for (k=0; k<SIZE; ) {

        register double f1=first[i][k];
        register double f2=first[i][k+1];
        register double f3=first[i][k+2];
        register double f4=first[i][k+3];
        register double f5=first[i][k+4];
        register double f6=first[i][k+5];
        register double f7=first[i][k+6];
        register double f8=first[i][k+7];
  sum = sum + f1*second[j][k] //<- OPT 5 (k<->j)
  + f2*second[j][k+1]
  + f3*second[j][k+2]
  + f4*second[j][k+3]
  + f5*second[j][k+4]
  + f6*second[j][k+5]
  + f7*second[j][k+6]
  + f8*second[j][k+7];
        multiply[i][j]+=sum; sum=0;

  sum = sum + f1*second[j+1][k] //<- OPT 5 (k<->j+1)
  + f2*second[j+1][k+1]
  + f3*second[j+1][k+2]
  + f4*second[j+1][k+3]
  + f5*second[j+1][k+4]
  + f6*second[j+1][k+5]
  + f7*second[j+1][k+6]
  + f8*second[j+1][k+7];
        multiply[i][j+1]+=sum; sum=0;

  sum = sum + f1*second[j+2][k] //<- OPT 5 (k<->j+2)
  + f2*second[j+2][k+1]
  + f3*second[j+2][k+2]
  + f4*second[j+2][k+3]
  + f5*second[j+2][k+4]
  + f6*second[j+2][k+5]
  + f7*second[j+2][k+6]
  + f8*second[j+2][k+7];
        multiply[i][j+2]+=sum; sum=0;

  sum = sum + f1*second[j+3][k] //<- OPT 5 (k<->j+3)
  + f2*second[j+3][k+1]
  + f3*second[j+3][k+2]
  + f4*second[j+3][k+3]
  + f5*second[j+3][k+4]
  + f6*second[j+3][k+5]
  + f7*second[j+3][k+6]
  + f8*second[j+3][k+7];
        multiply[i][j+3]+=sum; sum=0;

  sum = sum + f1*second[j+4][k] //<- OPT 5 (k<->j+4)
  + f2*second[j+4][k+1]
  + f3*second[j+4][k+2]
  + f4*second[j+4][k+3]
  + f5*second[j+4][k+4]
  + f6*second[j+4][k+5]
  + f7*second[j+4][k+6]
  + f8*second[j+4][k+7];
        multiply[i][j+4]+=sum; sum=0;

  sum = sum + f1*second[j+5][k] //<- OPT 5 (k<->j+5)
  + f2*second[j+5][k+1]
  + f3*second[j+5][k+2]
  + f4*second[j+5][k+3]
  + f5*second[j+5][k+4]
  + f6*second[j+5][k+5]
  + f7*second[j+5][k+6]
  + f8*second[j+5][k+7];
        multiply[i][j+5]+=sum; sum=0;

  sum = sum + f1*second[j+6][k] //<- OPT 5 (k<->j+6)
  + f2*second[j+6][k+1]
  + f3*second[j+6][k+2]
  + f4*second[j+6][k+3]
  + f5*second[j+6][k+4]
  + f6*second[j+6][k+5]
  + f7*second[j+6][k+6]
  + f8*second[j+6][k+7];
        multiply[i][j+6]+=sum; sum=0;

  sum = sum + f1*second[j+7][k] //<- OPT 5 (k<->j+6)
  + f2*second[j+7][k+1]
  + f3*second[j+7][k+2]
  + f4*second[j+7][k+3]
  + f5*second[j+7][k+4]
  + f6*second[j+7][k+5]
  + f7*second[j+7][k+6]
  + f8*second[j+7][k+7];
        multiply[i][j+7]+=sum; sum=0;

  k=k+8;
      }
      j=j+8;
    }
  }
  return 0;
}

int mm9(double first[][SIZE], double second[][SIZE], double multiply[][SIZE])
{
  register unsigned int i,j,k; //<- OPT 1
  double sum = 0;
  for (i = 0; i<SIZE ; i++ ) { //rows in multiply <- OPT 3
    for (j=0; j<SIZE; ) {
      for (k=0; k<SIZE; ) {
        register double f1=first[i][k];
        register double f2=first[i][k+1];
        register double f3=first[i][k+2];
        register double f4=first[i][k+3];
        register double f5=first[i][k+4];
        register double f6=first[i][k+5];
        register double f7=first[i][k+6];
        register double f8=first[i][k+7];
        register double f9=first[i][k+8];
        register double f10=first[i][k+9];
        register double f11=first[i][k+10];
        register double f12=first[i][k+11];
        register double f13=first[i][k+12];
        register double f14=first[i][k+13];
        register double f15=first[i][k+14];
        register double f16=first[i][k+15];
  sum = sum + f1*second[j][k] //<- OPT 5 (k<->j)
  + f2*second[j][k+1]
  + f3*second[j][k+2]
  + f4*second[j][k+3]
  + f5*second[j][k+4]
  + f6*second[j][k+5]
  + f7*second[j][k+6]
  + f8*second[j][k+7]
  + f9*second[j][k+8]
  + f10*second[j][k+9]
  + f11*second[j][k+10]
  + f12*second[j][k+11]
  + f13*second[j][k+12]
  + f14*second[j][k+13]
  + f15*second[j][k+14]
  + f16*second[j][k+15];
        multiply[i][j]+=sum; sum=0;

  sum = sum + f1*second[j+1][k] //<- OPT 5 (k<->j+1)
  + f2*second[j+1][k+1]
  + f3*second[j+1][k+2]
  + f4*second[j+1][k+3]
  + f5*second[j+1][k+4]
  + f6*second[j+1][k+5]
  + f7*second[j+1][k+6]
  + f8*second[j+1][k+7]
  + f9*second[j+1][k+8]
  + f10*second[j+1][k+9]
  + f11*second[j+1][k+10]
  + f12*second[j+1][k+11]
  + f13*second[j+1][k+12]
  + f14*second[j+1][k+13]
  + f15*second[j+1][k+14]
  + f16*second[j+1][k+15];
        multiply[i][j+1]+=sum; sum=0;

  sum = sum + f1*second[j+2][k] //<- OPT 5 (k<->j+2)
  + f2*second[j+2][k+1]
  + f3*second[j+2][k+2]
  + f4*second[j+2][k+3]
  + f5*second[j+2][k+4]
  + f6*second[j+2][k+5]
  + f7*second[j+2][k+6]
  + f8*second[j+2][k+7]
  + f9*second[j+2][k+8]
  + f10*second[j+2][k+9]
  + f11*second[j+2][k+10]
  + f12*second[j+2][k+11]
  + f13*second[j+2][k+12]
  + f14*second[j+2][k+13]
  + f15*second[j+2][k+14]
  + f16*second[j+2][k+15];
        multiply[i][j+2]+=sum; sum=0;

  sum = sum + f1*second[j+3][k] //<- OPT 5 (k<->j+3)
  + f2*second[j+3][k+1]
  + f3*second[j+3][k+2]
  + f4*second[j+3][k+3]
  + f5*second[j+3][k+4]
  + f6*second[j+3][k+5]
  + f7*second[j+3][k+6]
  + f8*second[j+3][k+7]
  + f9*second[j+3][k+8]
  + f10*second[j+3][k+9]
  + f11*second[j+3][k+10]
  + f12*second[j+3][k+11]
  + f13*second[j+3][k+12]
  + f14*second[j+3][k+13]
  + f15*second[j+3][k+14]
  + f16*second[j+3][k+15];
        multiply[i][j+3]+=sum; sum=0;

  sum = sum + f1*second[j+4][k] //<- OPT 5 (k<->j+4)
  + f2*second[j+4][k+1]
  + f3*second[j+4][k+2]
  + f4*second[j+4][k+3]
  + f5*second[j+4][k+4]
  + f6*second[j+4][k+5]
  + f7*second[j+4][k+6]
  + f8*second[j+4][k+7]
  + f9*second[j+4][k+8]
  + f10*second[j+4][k+9]
  + f11*second[j+4][k+10]
  + f12*second[j+4][k+11]
  + f13*second[j+4][k+12]
  + f14*second[j+4][k+13]
  + f15*second[j+4][k+14]
  + f16*second[j+4][k+15];
        multiply[i][j+4]+=sum; sum=0;

  sum = sum + f1*second[j+5][k] //<- OPT 5 (k<->j+5)
  + f2*second[j+5][k+1]
  + f3*second[j+5][k+2]
  + f4*second[j+5][k+3]
  + f5*second[j+5][k+4]
  + f6*second[j+5][k+5]
  + f7*second[j+5][k+6]
  + f8*second[j+5][k+7]
  + f9*second[j+5][k+8]
  + f10*second[j+5][k+9]
  + f11*second[j+5][k+10]
  + f12*second[j+5][k+11]
  + f13*second[j+5][k+12]
  + f14*second[j+5][k+13]
  + f15*second[j+5][k+14]
  + f16*second[j+5][k+15];
        multiply[i][j+5]+=sum; sum=0;

  sum = sum + f1*second[j+6][k] //<- OPT 5 (k<->j+6)
  + f2*second[j+6][k+1]
  + f3*second[j+6][k+2]
  + f4*second[j+6][k+3]
  + f5*second[j+6][k+4]
  + f6*second[j+6][k+5]
  + f7*second[j+6][k+6]
  + f8*second[j+6][k+7]
  + f9*second[j+6][k+8]
  + f10*second[j+6][k+9]
  + f11*second[j+6][k+10]
  + f12*second[j+6][k+11]
  + f13*second[j+6][k+12]
  + f14*second[j+6][k+13]
  + f15*second[j+6][k+14]
  + f16*second[j+6][k+15];
        multiply[i][j+6]+=sum; sum=0;

  sum = sum + f1*second[j+7][k] //<- OPT 5 (k<->j+6)
  + f2*second[j+7][k+1]
  + f3*second[j+7][k+2]
  + f4*second[j+7][k+3]
  + f5*second[j+7][k+4]
  + f6*second[j+7][k+5]
  + f7*second[j+7][k+6]
  + f8*second[j+7][k+7]
  + f9*second[j+7][k+8]
  + f10*second[j+7][k+9]
  + f11*second[j+7][k+10]
  + f12*second[j+7][k+11]
  + f13*second[j+7][k+12]
  + f14*second[j+7][k+13]
  + f15*second[j+7][k+14]
  + f16*second[j+7][k+15];
        multiply[i][j+7]+=sum; sum=0;

  sum = sum + f1*second[j+8][k] //<- OPT 5 (k<->j+8)
  + f2*second[j+8][k+1]
  + f3*second[j+8][k+2]
  + f4*second[j+8][k+3]
  + f5*second[j+8][k+4]
  + f6*second[j+8][k+5]
  + f7*second[j+8][k+6]
  + f8*second[j+8][k+7]
  + f9*second[j+8][k+8]
  + f10*second[j+8][k+9]
  + f11*second[j+8][k+10]
  + f12*second[j+8][k+11]
  + f13*second[j+8][k+12]
  + f14*second[j+8][k+13]
  + f15*second[j+8][k+14]
  + f16*second[j+8][k+15];
        multiply[i][j+8]+=sum; sum=0;

  sum = sum + f1*second[j+9][k] //<- OPT 5 (k<->j+9)
  + f2*second[j+9][k+1]
  + f3*second[j+9][k+2]
  + f4*second[j+9][k+3]
  + f5*second[j+9][k+4]
  + f6*second[j+9][k+5]
  + f7*second[j+9][k+6]
  + f8*second[j+9][k+7]
  + f9*second[j+9][k+8]
  + f10*second[j+9][k+9]
  + f11*second[j+9][k+10]
  + f12*second[j+9][k+11]
  + f13*second[j+9][k+12]
  + f14*second[j+9][k+13]
  + f15*second[j+9][k+14]
  + f16*second[j+9][k+15];
        multiply[i][j+9]+=sum; sum=0;

  sum = sum + f1*second[j+10][k] //<- OPT 5 (k<->j+10)
  + f2*second[j+10][k+1]
  + f3*second[j+10][k+2]
  + f4*second[j+10][k+3]
  + f5*second[j+10][k+4]
  + f6*second[j+10][k+5]
  + f7*second[j+10][k+6]
  + f8*second[j+10][k+7]
  + f9*second[j+10][k+8]
  + f10*second[j+10][k+9]
  + f11*second[j+10][k+10]
  + f12*second[j+10][k+11]
  + f13*second[j+10][k+12]
  + f14*second[j+10][k+13]
  + f15*second[j+10][k+14]
  + f16*second[j+10][k+15];
        multiply[i][j+10]+=sum; sum=0;

  sum = sum + f1*second[j+11][k] //<- OPT 5 (k<->j+11)
  + f2*second[j+11][k+1]
  + f3*second[j+11][k+2]
  + f4*second[j+11][k+3]
  + f5*second[j+11][k+4]
  + f6*second[j+11][k+5]
  + f7*second[j+11][k+6]
  + f8*second[j+11][k+7]
  + f9*second[j+11][k+8]
  + f10*second[j+11][k+9]
  + f11*second[j+11][k+10]
  + f12*second[j+11][k+11]
  + f13*second[j+11][k+12]
  + f14*second[j+11][k+13]
  + f15*second[j+11][k+14]
  + f16*second[j+11][k+15];
        multiply[i][j+11]+=sum; sum=0;

  sum = sum + f1*second[j+12][k] //<- OPT 5 (k<->j+12)
  + f2*second[j+12][k+1]
  + f3*second[j+12][k+2]
  + f4*second[j+12][k+3]
  + f5*second[j+12][k+4]
  + f6*second[j+12][k+5]
  + f7*second[j+12][k+6]
  + f8*second[j+12][k+7]
  + f9*second[j+12][k+8]
  + f10*second[j+12][k+9]
  + f11*second[j+12][k+10]
  + f12*second[j+12][k+11]
  + f13*second[j+12][k+12]
  + f14*second[j+12][k+13]
  + f15*second[j+12][k+14]
  + f16*second[j+12][k+15];
        multiply[i][j+12]+=sum; sum=0;

  sum = sum + f1*second[j+13][k] //<- OPT 5 (k<->j+13)
  + f2*second[j+13][k+1]
  + f3*second[j+13][k+2]
  + f4*second[j+13][k+3]
  + f5*second[j+13][k+4]
  + f6*second[j+13][k+5]
  + f7*second[j+13][k+6]
  + f8*second[j+13][k+7]
  + f9*second[j+13][k+8]
  + f10*second[j+13][k+9]
  + f11*second[j+13][k+10]
  + f12*second[j+13][k+11]
  + f13*second[j+13][k+12]
  + f14*second[j+13][k+13]
  + f15*second[j+13][k+14]
  + f16*second[j+13][k+15];
        multiply[i][j+13]+=sum; sum=0;

  sum = sum + f1*second[j+14][k] //<- OPT 5 (k<->j+14)
  + f2*second[j+14][k+1]
  + f3*second[j+14][k+2]
  + f4*second[j+14][k+3]
  + f5*second[j+14][k+4]
  + f6*second[j+14][k+5]
  + f7*second[j+14][k+6]
  + f8*second[j+14][k+7]
  + f9*second[j+14][k+8]
  + f10*second[j+14][k+9]
  + f11*second[j+14][k+10]
  + f12*second[j+14][k+11]
  + f13*second[j+14][k+12]
  + f14*second[j+14][k+13]
  + f15*second[j+14][k+14]
  + f16*second[j+14][k+15];
        multiply[i][j+14]+=sum; sum=0;

  sum = sum + f1*second[j+15][k] //<- OPT 5 (k<->j+15)
  + f2*second[j+15][k+1]
  + f3*second[j+15][k+2]
  + f4*second[j+15][k+3]
  + f5*second[j+15][k+4]
  + f6*second[j+15][k+5]
  + f7*second[j+15][k+6]
  + f8*second[j+15][k+7]
  + f9*second[j+15][k+8]
  + f10*second[j+15][k+9]
  + f11*second[j+15][k+10]
  + f12*second[j+15][k+11]
  + f13*second[j+15][k+12]
  + f14*second[j+15][k+13]
  + f15*second[j+15][k+14]
  + f16*second[j+15][k+15];
        multiply[i][j+15]+=sum; sum=0;

  k=k+16;
      }
      j=j+16;
    }
  }
  return 0;
}


static void test_fail(char *file, int line, char *call, int retval);

int main(int argc, char **argv) {
  extern void dummy(void *);
  float matrixa[INDEX][INDEX], matrixb[INDEX][INDEX], mresult[INDEX][INDEX];
  float real_time, proc_time, mflops;
  long long flpins;
  int retval;
  int i,j,k,l;

long long fp_ops[9];
long long ld_ins[9];

int events[2] = {PAPI_FP_OPS, PAPI_LD_INS};
long long values[2] = {0,};
int eventSet = PAPI_NULL;
int papi_err;

//set afiniti for CPU 1
cpu_set_t  mask;
CPU_ZERO(&mask);
CPU_SET(1, &mask);
int result = sched_setaffinity(0, sizeof(mask), &mask);  //0 for actual pid
if (result != 0){
	printf("affinity error");
	exit(-1);
}


if (PAPI_library_init(PAPI_VER_CURRENT) != PAPI_VER_CURRENT) {
	fprintf(stderr, "PAPI is unsupported.\n");
	exit(-1);
}

if ((papi_err = PAPI_create_eventset(&eventSet)) != PAPI_OK) {
	fprintf(stderr, "Could not create event set: %s\n", PAPI_strerror(papi_err));
	PAPI_shutdown();
	exit(-1);
}


// initialize papi event set

for (i=0; i < 2; ++i) {
	if ((papi_err = PAPI_add_event(eventSet, events[i])) != PAPI_OK ) {
		fprintf(stderr, "Could not add event: %s\n", PAPI_strerror(papi_err));
		fflush(stderr);
	}
}

double first[SIZE][SIZE];
double second[SIZE][SIZE];
double multiply[SIZE][SIZE];
int iret;

for (j = 0; j < 9; j++){
	for (k = 0; k < SIZE; k++) { //rows in first
   		for (l = 0; l < SIZE; l++) { //columns in first
     			first[k][l]=k+l;
      			second[l][k]=k-l;
      			multiply[k][l]=0.0;
   		 }
  	}

 
if ((papi_err = PAPI_start(eventSet)) != PAPI_OK) {
	fprintf(stderr, "Could not start counters: %s\n", PAPI_strerror(papi_err));
	PAPI_shutdown();
	exit(-1);
}
 /* Setup PAPI library and begin collecting data from the counters */
//if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK)
	//test_fail(__FILE__, __LINE__, "PAPI_flops", retval);
	
	switch(j)
	{
	case 0:
		iret=mm1(first,second,multiply);
		break;
	case 1:
		iret=mm2(first,second,multiply);
		break;
        case 2:
		iret=mm3(first,second,multiply);
                break;
        case 3:
		iret=mm4(first,second,multiply);
                break;
        case 4:
		iret=mm5(first,second,multiply);
                break;
        case 5:
		iret=mm6(first,second,multiply);
                break;
        case 6:
		iret=mm7(first,second,multiply);
                break;
        case 7:
		iret=mm8(first,second,multiply);
                break;
        case 8:
		iret=mm9(first,second,multiply);
                break;
	default:
		return;


	}
	if ((papi_err = PAPI_stop(eventSet, values)) != PAPI_OK) {
		fprintf(stderr, "Could not get values: %s\n", PAPI_strerror(papi_err));
	}
  //if((retval=PAPI_flops( &real_time, &proc_time, &flpins, &mflops))<PAPI_OK)
   // test_fail(__FILE__, __LINE__, "PAPI_flops", retval);

 // printf("Real_time:\t%f\nProc_time:\t%f\nTotal flpins:\t%lld\nMFLOPS:\t\t%f\n",
 // real_time, proc_time, flpins, mflops);
 // printf("%s\tPASSED\n", __FILE__);
 
	printf("Performance counters for factorization stage: \n");
	printf("\tFP OPS: %ld\n", values[0]);
	printf("\tLD INS: %ld\n", values[1]);
	
	fp_ops[j] = values[0];
	ld_ins[j] = values[1];

}

FILE *f = fopen("results.txt", "w");
if (f == NULL){
	printf("Error opening file!\n");	
	PAPI_shutdown();
	exit(1);
}

fprintf(f, "FP OPS\n\n");

for(j = 0; j < 9; j++){
	fprintf(f, "%lld\n", fp_ops[j]);
}

fprintf(f, "\n\nLD INS\n\n");

for(j = 0; j < 9; j++){
	fprintf(f, "%lld\n", ld_ins[j]);
}

fclose(f);
/* Initialize the Matrix arrays */
/*  for ( i=0; i<INDEX*INDEX; i++ ){
    mresult[0][i] = 0.0;
    matrixa[0][i] = matrixb[0][i] = rand()*(float)1.1; }
*/



  /* Matrix-Matrix multiply */
 /* for (i=0;i<INDEX;i++)
   for(j=0;j<INDEX;j++)
    for(k=0;k<INDEX;k++)
      mresult[i][j]=mresult[i][j] + matrixa[i][k]*matrixb[k][j];
*/
/* Collect the data into the variables passed in */
if ((papi_err = PAPI_start(eventSet)) != PAPI_OK) {
	fprintf(stderr, "Could not start counters: %s\n", PAPI_strerror(papi_err));
	PAPI_shutdown();
	exit(-1);
}

 PAPI_shutdown();
  exit(0);
}

static void test_fail(char *file, int line, char *call, int retval){
    printf("%s\tFAILED\nLine # %d\n", file, line);
    if ( retval == PAPI_ESYS ) {
        char buf[128];
        memset( buf, '\0', sizeof(buf) );
        sprintf(buf, "System error in %s:", call );
        perror(buf);
    }
    else if ( retval > 0 ) {
        printf("Error calculating: %s\n", call );
    }
    else {
        printf("Error in %s: %s\n", call, PAPI_strerror(retval) );
    }
    printf("\n");
    exit(1);
}
