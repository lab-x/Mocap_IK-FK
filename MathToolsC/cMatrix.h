#ifndef __MathToolsC__cMatrix__
#define __MathToolsC__cMatrix__

#include "MathToolsC/cFloat.h"


int cSolveLinearEquations ( cFloat*x, cFloat*y, int n, int xStep,
                            cFloat* result ) ;


void cTransSelfNxNMatrix (cFloat*m, int n, int step) ;



void cTransMatrix2 ( cFloat *m, cFloat *mT, int mRow, int mCol,
                     int mStep, int mTStep ) ;


void cDotMatrix ( cFloat*left, cFloat*right, cFloat*result,
                  int Row, int Col,
                  int leftStep, int rightStep, int resultStep ) ;


void cMultMatrix ( cFloat*left, cFloat*right, cFloat*result,
                   int leftR, int leftC_rightR, int rightC,
                   int leftStep, int rightStep, int resultStep ) ;


void cMatrixLeftMultSelfTrans ( cFloat*m, cFloat*result,
                                int mRow, int mCol,
                                int mStep, int resultStep ) ;


void cMatrixRightMultSelfTrans ( cFloat*m, cFloat*result,
                                 int mRow, int mCol,
                                 int mStep, int resultStep ) ;

void cMatrixALeftMultBTrans ( cFloat*A, cFloat*B, cFloat*result,
                              int aRow, int abCol, int bRow,
                              int aStep, int bStep, int resultStep ) ;


void cMatrixARightMultBTrans ( cFloat*A, cFloat*B, cFloat*result,
                               int abRow, int aCol, int bCol,
                               int aStep, int bStep, int resultStep ) ;


void cSVDnxn ( cFloat *A, cFloat *S, cFloat *V, int n ) ;


void cPrintMatrix ( cFloat*m, int mRow, int mCol, int mStep ) ;


#endif

