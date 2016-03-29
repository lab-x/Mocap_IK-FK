#ifndef __MathToolsCpp__Matrix__
#define __MathToolsCpp__Matrix__
#include <cmath>
#include <iostream>
#include "MathToolsCpp/cppFloat.h"
#include "MathToolsCpp/Vector3D.h"
//#include "MathToolsCpp/Quaternion.h"



//#define MathToolsCpp_MaxMatrixSize (0)  // when zero, use dynamic memory .
#define MathToolsCpp_MaxMatrixSize (32)


class Matrix {

private:
    int rows, cols;

#if  0 == MathToolsCpp_MaxMatrixSize
    Float *mat;
#else
    Float mat[MathToolsCpp_MaxMatrixSize] ;
#endif

public:
    // Default empty constructor
    Matrix() ;
    Matrix(int numRows, int numCols);  // Construct a zero matrix
    Matrix(const Matrix &A);  // Copy constructor
    Matrix &operator=(const Matrix &A);  // Copy assignment
#if  0 == MathToolsCpp_MaxMatrixSize
    ~Matrix();
#endif

    int rowSize() const {return rows;}
    int colSize() const {return cols;}

    Float &operator()(int i, int j) const;
    friend Matrix operator+(const Matrix &A, const Matrix &B);
    friend Matrix operator*(const Matrix &A, const Matrix &B);
    friend Matrix operator*(Float a, const Matrix &A);
    friend Matrix operator*(const Matrix &A, Float a);
    Matrix transpose();
    void printMat();
    bool save2file(char *filename) const;
    bool LoadMatFile(char *filename) ;

} ;
#endif // MATRIX_H

/* USAGE:
//    Matrix A(3,4);
//    A(0,0) = 1;A(0,1) = 2; A(0,2) = 3; A(0,3) = 4;
//    A(1,0) = 4; A(1,1) = 3;  A(1,2) = 2;  A(1,3) = 1;
//    A(2,0) = 3;  A(2,1) = 2; A(2,2) = 1; A(2,3) = 0;     A.printMat();
//    Matrix B(4,1);
//    B(0,0) = 1;B(1,0) = 2;  B(2,0) = 3;  B(3,0) = 4;     B.printMat();
//    Matrix C = A*B;    C.printMat();
*/
