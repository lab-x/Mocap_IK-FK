
#include "stdint.h"
#include "stdio.h"
#include "math.h"
#include "float.h"
#include "MathToolsC/cMatrix.h"


#define FLOAT_MIN (5*FLT_MIN)     // 可接受的最小值
/* 高斯消元法。
 * 时间复杂度： O(row*row*col)，若是方阵即 row=col=n，则时间复杂度是 O(n^3)
 * 参数：
 *      x :  待消元的矩阵，注意是个二级指针，且输入的矩阵内容会被改变
 *      row: 矩阵的行数
 *      col: 矩阵的列数
 *      backEnable: 是否向回消元。如果使能，则矩阵将被消元成对角阵，若未使能，则矩阵被消元成上三角阵
 *      y : 用于求解方程组，代表方程组等号右边的列
 *      sign: 用于求解行列式，指示矩阵在变换前后行列式正负号发生变化。返回 1代表未发生变化
 *      follow: 用于求解矩阵逆。它本身也是个矩阵，一般会输入一个与x一样大的单位矩阵作为follow,它将跟随x发生变化
 * 返回值:
 *      1 : 行列式非0
 */
static int GaussElimination ( cFloat**x, int row, int col, int backEnable,
                       cFloat*y, int* sign, cFloat **follow )
{
  int ret = 1 ;
  int i, j ;

  if ( sign )
    *sign = 1 ;

  for ( i=0; i<row; i++ )
  {
    if ( i >= col )     // 多余的行不再处理。比如共有8行5列，则第5行以后不再处理，因为消元法进行到第5行后，下面的3行已经全变成0了
      break ;

    // 寻找第 i 列的第一个不为 0 值, 放到第 i 行
    for ( j=i; j<row; j++ )
    {
      if ( x[j][i] >= FLOAT_MIN || x[j][i] <= - FLOAT_MIN  )
        break ;
      else
        x[j][i] = 0.0f ;
    }

    if ( j == row )
    {
      // 第 i 列全为0，方程组无解哦，或者行列式为 0，或者不存在逆矩阵 。
      // 跳过第 i 行
      ret = 0 ;
      continue ;
    }
    else if ( j > i )
    {
      // swap row i and row j

      cFloat *temp = x[i] ;
      x[i] = x[j] ;
      x[j] = temp ;

      if (follow)
      {
        cFloat *temp = follow[i] ;
        follow[i] = follow[j] ;
        follow[j] = temp ;
      }

      if (y)
      {
        cFloat temp = y[i] ;
        y[i] = y[j] ;
        y[j] = temp ;
      }

      if (sign)
      {
        *sign  *=  -1 ;   // sign 取反
      }
    }

    // 开始消元，消掉第 i 行以下各行的第 i 列。如果 backEnable 使能，则第 i 行以上各行的第 i 列也会被消掉
    int start ;
    if ( backEnable )
      start = 0 ;
    else
      start = i+1 ;
    for ( j=start; j<row; j++ )
    {
      if (j==i)
        continue ;

      cFloat ratio = -x[j][i] / x[i][i] ;

      //  x[j] = x[i]*ratio + x[j] ;
      int k ;
      for ( k=i; k<col; k++ )
      {
        x[j][k] += ratio*x[i][k] ;
        if ( k==i )
          x[j][k] = 0.0f ;      // 确保第 i 列为 0
        if (follow)
          follow[j][k] += ratio*follow[i][k] ;
      }

      if (y)
        y[j] += ratio*y[i] ;

    }
  }

  return ret ;
}

#define MAX_MatrixDimensional (20)
static cFloat* EquationsXp  [MAX_MatrixDimensional] ;
static cFloat* MatrixFollowp [MAX_MatrixDimensional] ;
static cFloat  EquationsY  [MAX_MatrixDimensional] ;

/**
解线性方程组。
注意为了节省单片机的栈，函数内部使用了静态数组EquationsXp，因此本函数不可重入！！
往PC上移植时可用堆实现
注意，x 指向的数据在本函数中会被改变，result 可以覆盖 i
*/
int cSolveLinearEquations ( cFloat*x, cFloat*y, int n, int xStep, cFloat* result )
{
    if ( n > MAX_MatrixDimensional )
      return 0 ;    // 维数太大

    if ( xStep == 0 )
        xStep = n ;

    int i ;
    cFloat ** px = EquationsXp ;
    cFloat * py = EquationsY ;
    for (i=0;i<n;i++)
    {
      px[i] = x+xStep*i ;
      py[i] = y[i] ;
    }

    if ( GaussElimination ( px, n, n, 1, py, 0, 0 ) )
    {
        for (i=0; i<n; i++)
          result [ i ] = py[i] / px[i][i] ;
        return 1 ;
    }
    else
    {
      return 0 ;
    }
}



void cDotMatrix ( cFloat*left, cFloat*right, cFloat*result,
                  int Row, int Col,
                  int leftStep, int rightStep, int resultStep )
{
    int i, j, k;
    if ( leftStep == 0 )
        leftStep = Col ;
    if ( rightStep == 0 )
        rightStep = Col ;
    if ( resultStep == 0 )
        resultStep = Col ;

    for ( i=0; i<Row; i++ )
        for ( j=0; j<Col; j++ )
        {
            result[i*resultStep+j] = left[i*leftStep+j] * right[i*rightStep+j] ;
        }
}



void cMultMatrix ( cFloat*left, cFloat*right, cFloat*result, int leftR, int leftC_rightR, int rightC, int leftStep, int rightStep, int resultStep )
{
    int i, j, k;
    cFloat sum ;
    if ( leftStep == 0 )
        leftStep = leftC_rightR ;
    if ( rightStep == 0 )
        rightStep = rightC ;
    if ( resultStep == 0 )
        resultStep = rightC ;


    for ( i=0; i<leftR; i++ )
        for ( j=0; j<rightC; j++ )
        {
            sum = 0 ;
            for ( k=0; k<leftC_rightR; k++ )
                sum += left[i*leftStep+k] * right[k*rightStep+j] ;
            result [i*resultStep+j] = sum ;
        }
}


void cMatrixLeftMultSelfTrans ( cFloat*m, cFloat*result, int mRow, int mCol, int mStep, int resultStep )
{
    int i, j, k;
    cFloat sum ;
    if ( mStep == 0 )
        mStep = mCol ;
    if ( resultStep == 0 )
        resultStep = mRow ;


    for ( i=0; i<mRow; i++ )
        for ( j=i; j<mRow; j++ )
        {
            sum = 0 ;
            for ( k=0; k<mCol; k++ )
                sum += m[i*mStep+k] * m[j*mStep+k] ;
            result [i*resultStep+j] = sum ;
            result [j*resultStep+i] = sum ;
        }
}

void cMatrixRightMultSelfTrans ( cFloat*m, cFloat*result, int mRow, int mCol, int mStep, int resultStep )
{
    int i, j, k;
    cFloat sum ;
    if ( mStep == 0 )
        mStep = mCol ;
    if ( resultStep == 0 )
        resultStep = mCol ;


    for ( i=0; i<mCol; i++ )
        for ( j=i; j<mCol; j++ )
        {
            sum = 0 ;
            for ( k=0; k<mRow; k++ )
                sum += m[i+k*mStep] * m[j+k*mStep] ;
            result [i*resultStep+j] = sum ;
            result [j*resultStep+i] = sum ;
        }
}


void cMatrixALeftMultBTrans ( cFloat*A, cFloat*B, cFloat*result, int aRow, int abCol, int bRow, int aStep, int bStep, int resultStep )
{
    int i, j, k;
    cFloat sum ;
    if ( aStep == 0 )
        aStep = abCol ;
    if ( bStep == 0 )
        bStep = abCol ;
    if ( resultStep == 0 )
        resultStep = bRow ;


    for ( i=0; i<aRow; i++ )
        for ( j=0; j<bRow; j++ )
        {
            sum = 0 ;
            for ( k=0; k<abCol; k++ )
                sum += A[i*aStep+k] * B[j*bStep+k] ;
            result [i*resultStep+j] = sum ;
        }
}

void cMatrixARightMultBTrans ( cFloat*A, cFloat*B, cFloat*result, int abRow, int aCol, int bCol, int aStep, int bStep, int resultStep )
{
    int i, j, k;
    cFloat sum ;
    if ( aStep == 0 )
        aStep = aCol ;
    if ( bStep == 0 )
        bStep = bCol ;
    if ( resultStep == 0 )
        resultStep = aCol ;


    for ( i=0; i<bCol; i++ )
        for ( j=0; j<aCol; j++ )
        {
            sum = 0 ;
            for ( k=0; k<abRow; k++ )
                sum += A[j+k*aStep] * B[i+k*bStep] ;
            result [i*resultStep+j] = sum ;
        }
}






void cTransSelfNxNMatrix (cFloat*m, int n, int step)
{
    int i, j;
    if ( step == 0 )
        step = n ;

    for ( i=0; i<n-1; i++ )
        for ( j=i+1; j<n; j++ )
        {
            cFloat temp = m[i*step+j] ;
            m[i*step+j] = m[j*step+i] ;
            m[j*step+i] = temp ;
        }
}



void cTransMatrix2 ( cFloat *m, cFloat *mT, int mRow, int mCol, int mStep, int mTStep )
{
    int i, j;
    if ( mStep == 0 )
        mStep = mCol ;
    if ( mTStep == 0 )
        mTStep = mRow ;

    for ( i=0; i<mRow; i++ )
        for ( j=0; j<mCol; j++ )
            mT[j*mTStep+i] = m[i*mStep+j] ;
}


extern void svd ( cFloat *A, cFloat *S, cFloat *V, int n ) ;
void cSVDnxn ( cFloat *A, cFloat *S, cFloat *V, int n )
{
    //svd ( A, S, V, n ) ;
}



void cPrintMatrix ( cFloat*m, int mRow, int mCol, int mStep )
{
    int i,j;
    if (mStep==0)
        mStep = mCol ;
    printf("\n");
    for (i=0; i<mRow; i++)
    {
        for (j=0; j<mCol; j++)
        {
            printf(" %lf, ", m[i*mStep+j]) ;
        }
        printf("\n");
    }
    printf("\n");
}
