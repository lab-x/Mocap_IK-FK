#ifndef __MathtoolsCpp__Vector3D__
#define __MathtoolsCpp__Vector3D__
 
#include <cmath> 
#include "MathToolsCpp/cppFloat.h"



class Vector3D
{
    Float values[3];
    
    // CONSTRUCTORs
public:
    Vector3D() ;
    Vector3D(Float x, Float y, Float z) ;
    Vector3D(const Float V[]) ;
    Vector3D(const Vector3D& other ) ;

    // OTHER FUNCTIONs
    Float getX() const {return values[0];}
    Float getY() const {return values[1];}
    Float getZ() const {return values[2];}
    void setX(Float X) {values[0]=X;}
    void setY(Float Y) {values[1]=Y;}
    void setZ(Float Z) {values[2]=Z;}
    void setValues(Float X, Float Y, Float Z) ;
    Float getLength() const ;
    Float getNorm() const ;
    const Vector3D& SetLength(Float length) ;
    const Vector3D& normalize () ;
    Vector3D getNormalized () const ;


    // OPERATORs
    friend Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs) ;
    friend Vector3D operator-(const Vector3D& lhs) ;
    friend Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) ;
    friend Vector3D operator*(const Vector3D& lhs, const Float& num) ;
    friend Vector3D operator*(const Float& num, const Vector3D& rhs ) ;
    friend Vector3D operator/(const Vector3D& lhs, const Float& num) ;
    Vector3D & operator+=(const Vector3D& rhs) ;
    Vector3D & operator*=(const float& rhs) ;
    Vector3D & operator-=(const Vector3D& rhs) ;
    Vector3D & operator=(const Vector3D& rhs) ;

    // PUBLIC STATICs
public :
    static Float Dot ( const Vector3D& vec1, const Vector3D& vec2 ) ;
    static Vector3D Cross(const Vector3D& vec1, const Vector3D& vec2) ;

};


Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs) ;
Vector3D operator-(const Vector3D& lhs) ;
Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) ;
Vector3D operator*(const Vector3D& lhs, const Float& num) ;
Vector3D operator*(const Float& num, const Vector3D& rhs ) ;
Vector3D operator/(const Vector3D& lhs, const Float& num) ;

typedef Vector3D Position ;

#endif
