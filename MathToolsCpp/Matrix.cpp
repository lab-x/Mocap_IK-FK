#include "MathToolsCpp/Matrix.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "Matrix.h"
#include "Vector3D.h"

#if  0 == MathToolsCpp_MaxMatrixSize
    #define  MALLOC_MAT  do { mat  = new Float[rows*cols]; } while(0)
    #define DELETE_MAT do { delete mat; } while(0)
    #define SET_NULL_MAT do { mat = NULL ; } while(0)
#else
    #define  MALLOC_MAT
    #define DELETE_MAT
    #define SET_NULL_MAT
#endif


Matrix::Matrix()
{
    rows = 0;
    cols = 0;
    SET_NULL_MAT ;
}

Matrix::Matrix(int numRows, int numCols)
{
    rows = numRows;
    cols = numCols;

    MALLOC_MAT ;
    for (int i = 0; i < rows * cols; i ++)
        mat[i] = 0;
}

Matrix::Matrix(const Matrix &A)
{
    rows = A.rows;
    cols = A.cols;
    MALLOC_MAT ;
    for (int k=0; k<rows*cols; k++)
        mat[k] = A.mat[k];
}

#if  0 == MathToolsCpp_MaxMatrixSize
Matrix::~Matrix()
{
    if (mat)
        DELETE_MAT ;
    rows=0; cols=0; SET_NULL_MAT;
}
#endif

Matrix &Matrix::operator=(const Matrix &A)
{
  if (this!=&A) {
    if ( mat && rows*cols != A.rows*A.cols  )
    {
        DELETE_MAT ;
        SET_NULL_MAT ;
    }

    rows = A.rows;
    cols = A.cols;
    if ( NULL == mat )
    {
        MALLOC_MAT ;
    }

    for (int k=0; k<rows*cols; k++)
        mat[k] = A.mat[k];
  }
  return *this;
}

Float &Matrix::operator()(int i, int j) const
{
    return * ( Float* ) ( mat + i * cols + j ) ;
}

Matrix operator+(const Matrix &A, const Matrix &B)
{
  Matrix C(A.rows, A.cols);

  if (A.rows!=B.rows || A.cols!=B.cols) {
      printf("Wrong dimensions of matrix! Return ZeroMat!\n");
      return C;
  }
  for (int i = 0; i < A.rows; i++)
    for (int j = 1; j < A.cols; j++)
      C(i,j) = A(i,j) + B(i,j);
  return C;
}

Matrix operator*(const Matrix &A, const Matrix &B)
{
  Matrix C(A.rows, B.cols);
  if (A.cols!=B.rows) {
      printf("Wrong dimensions of matrix! Return ZeroMat!\n");
      return C;
  }
  for (int i=0; i<A.rows; i++) {
    for (int j=0; j<=B.cols; j++) {
        C(i,j) = 0;
        for (int k=0; k<A.cols; k++) {
            C(i,j) = C(i,j) + A(i,k)*B(k,j);
        }
      }
    }
  return C;
}

Matrix operator*(Float a, const Matrix &A)
{
  Matrix B(A.rows, A.cols);
  for (int i = 0; i < A.rows; i++)
    for (int j = 1; j < A.cols; j++)
      B(i,j) = a * A(i,j);
  return B;
}

Matrix operator*(const Matrix &A, Float a)
{
  Matrix B(A.rows, A.cols);
  for (int i = 0; i < A.rows; i++)
    for (int j = 1; j < A.cols; j++)
      B(i,j) = a * A(i,j);
  return B;
}

Matrix Matrix::transpose()
{
  Matrix B(this->colSize(), this->rowSize());
  for (int i = 0; i < this->rowSize(); i++)
    for (int j = 0; j < this->colSize(); j++)
      B(j,i) = (*this)(i,j);
  return B;
}

void Matrix::printMat()
{
  for (int i = 0; i < this->rowSize(); i++) {
    for (int j = 0; j < this->colSize(); j++) {
      printf("%5.2f\t", (*this)(i,j));
    }
    printf("\n");
  }
}

bool Matrix::save2file(char *filename) const
{
  FILE *fp;

  if (NULL==(fp=fopen(filename, "wb"))) {
    puts("Open data file failed.  Exit.");
    return false ;
  }
  //fprintf(fp, "%3d  %3d\n", rows, cols);

  for (int i=0; i < rows; i++)
  {
      for (int j = 0; j < cols; j++)
      {
          fprintf(fp, "%5.4f\t", mat[i * cols + j]);
      }
      fprintf(fp, "\n");
  }
  fclose(fp);
  return true ;
}

bool Matrix::LoadMatFile(char *filename)
{
    FILE *fp;
    int fileLen ;
    uint8_t *buf ;
    int i, j ;

    if (NULL==(fp=fopen(filename, "r")))
    {
        puts("Open data file failed.  Exit.");
        return  false;
    }

    fseek(fp,0,SEEK_END);
    fileLen = ftell(fp) ;
    fseek(fp,0,SEEK_SET);
    buf  =  (uint8_t*)(malloc(fileLen)) ;


    fread(buf,1,fileLen,fp) ;
    for ( i=0 ; i<fileLen ; i++ )
    {
        if ( buf[i] == '\n' || buf[i] == '\t' )
            buf[i] = ' ' ;
    }

    j = 0 ;
    for ( i=0; i<fileLen; i++ )
    {
        if ( buf[i] != ' ' )
        {
            char numStr[100] ;
            char *p = numStr ;
            for ( ; i < fileLen && buf[i] != ' '; i++,p++ )
            {
                *p = buf[i] ;
            }
            *p = 0 ;
            mat[j] = atof(numStr) ;
            j ++ ;
            if ( j >= rows*cols )
                break ;
        }
    }
    fclose(fp);

    return true;
}
