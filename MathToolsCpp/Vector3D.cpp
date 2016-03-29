
#include "MathToolsCpp/Vector3D.h"

Vector3D::Vector3D(){
    values[0] = values[1] = values[2] = 0 ;
}

Vector3D::Vector3D(Float x, Float y, Float z) {
    values[0] = x ;
    values[1] = y ;
    values[2] = z ;
}

Vector3D::Vector3D(const Float V[]){
    values[0] = V[0] ;
    values[1] = V[1] ;
    values[2] = V[2] ;
}

Vector3D::Vector3D(const Vector3D& other ) {
    values[0] = other.values[0] ;
    values[1] = other.values[1] ;
    values[2] = other.values[2] ;
}



void Vector3D::setValues(Float X, Float Y, Float Z) {
    values[0]=X;
    values[1]=Y;
    values[2]=Z;
}

Float Vector3D::getLength() const {
    return sqrt ( getX() * getX() + getY() * getY() + getZ() * getZ() ) ;
}

Float Vector3D::getNorm() const {
    return getX() * getX() + getY() * getY() + getZ() * getZ() ;
}

const Vector3D& Vector3D::SetLength(Float length) {
    if ( getLength() > 0 ) {
        *this = *this * (length / getLength()) ;
    }
    return *this ;
}

const Vector3D& Vector3D::normalize () {
    SetLength(1) ;
    return *this;
}

Vector3D Vector3D::getNormalized () const {
    Vector3D V = *this ;
    return V.normalize() ;
}


Vector3D operator+(const Vector3D& lhs, const Vector3D& rhs) {
    return Vector3D( lhs.values[0] + rhs.values[0], lhs.values[1] + rhs.values[1], lhs.values[2] + rhs.values[2]);
}

Vector3D operator-(const Vector3D& lhs) {
    return Vector3D(-lhs.values[0], -lhs.values[1], -lhs.values[2]);
}

Vector3D operator-(const Vector3D& lhs, const Vector3D& rhs) {
    return Vector3D( lhs.values[0] - rhs.values[0], lhs.values[1] - rhs.values[1], lhs.values[2] - rhs.values[2]);
}

Vector3D operator*(const Vector3D& lhs, const Float& num) {
     return Vector3D(lhs.values[0] * num, lhs.values[1] * num, lhs.values[2] * num);
}

Vector3D operator*(const Float& num, const Vector3D& rhs ) {
     return Vector3D(rhs.values[0] * num, rhs.values[1] * num, rhs.values[2] * num);
}

Vector3D operator/(const Vector3D& lhs, const Float& num) {
    return Vector3D(lhs.values[0] / num, lhs.values[1] / num, lhs.values[2] / num);
}

Vector3D & Vector3D::operator=(const Vector3D& rhs) {
    values[0] = rhs.values[0] ;
    values[1] = rhs.values[1] ;
    values[2] = rhs.values[2] ;
    return *this ;
}

Vector3D & Vector3D::operator+=(const Vector3D& rhs) {
    *this = *this + rhs ;
    return *this ;
}

Vector3D & Vector3D::operator*=(const float& rhs)
{
    *this = *this * rhs ;
    return *this ;
}


Vector3D & Vector3D::operator-=(const Vector3D& rhs) {
    *this = *this - rhs ;
    return *this ;
}



Float Vector3D::Dot ( const Vector3D& vec1, const Vector3D& vec2 ){
    return vec1.getX() * vec2.getX() +
            vec1.getY() * vec2.getY() +
            vec1.getZ() * vec2.getZ();
}

Vector3D Vector3D::Cross(const Vector3D& vec1, const Vector3D& vec2){
    return Vector3D(vec1.getY() * vec2.getZ() - vec1.getZ() * vec2.getY(),
                   vec1.getZ() * vec2.getX() - vec1.getX() * vec2.getZ(),
                   vec1.getX() * vec2.getY() - vec1.getY() * vec2.getX());
}

