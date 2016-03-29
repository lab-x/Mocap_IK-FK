
#ifndef __MathToolsCpp__Quaternion__
#define __MathToolsCpp__Quaternion__
#include <cmath>
#include "MathToolsCpp/cppFloat.h"
#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Matrix.h"


enum EulerOrder {
       EulerOrder_YXZ,
       EulerOrder_ZYX,
       EulerOrder_XZY,
       EulerOrder_XYZ,
       EulerOrder_YZX,
       EulerOrder_ZXY
   };

class EulerAngle ;
class AxisAngle ;
class Quaternion {


    // CONSTRUCTORs
public:
    Quaternion() ;
    Quaternion(Float w, Float x, Float y, Float z) ;
    Quaternion(const Float quaternion[]) ;
    Quaternion(const Quaternion& other ) ;

    // OTHER PUBLIC FUNCTIONs
    const Float* getValues() const { return values ; }
    Float getW() const { return values[0]; }
    Float getX() const { return values[1]; }
    Float getY() const { return values[2]; }
    Float getZ() const { return values[3]; }
    Quaternion & operator=(const Quaternion& rhs) ;
    friend Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) ;
    friend Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs) ;
    friend Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs) ;
    friend Quaternion operator-(const Quaternion& rhs) ;
    friend Quaternion operator*(const Quaternion& lhs, const Float r) ;
    friend Quaternion operator*(const Float r, const Quaternion& rhs) ;
    void   normalize() ;
    Quaternion conj() const ;

    Vector3D   rotVector (const Vector3D& v) const ;
    EulerAngle toEuler (EulerOrder order = EulerOrder_YXZ) const ;
    AxisAngle  toAxisAngle() const ;
    Matrix Quat2Mat() const ;
    static Quaternion mat2Quat(const Matrix& M);

    // PUBLIC STATICs
public :
    static Quaternion v2q(const Vector3D& vec1, const Vector3D& vec2);
    static Vector3D rotVbyQ (const Vector3D& v, const Quaternion& q);
    static Quaternion slerp1(const Quaternion& p, const Quaternion& q, const Float t);
    static Quaternion slerp(const Quaternion& p, const Quaternion& q, const Float t);
    static Quaternion Slerp(const Quaternion& start, const Quaternion& end, const Float endWeight);
    static Float Dot (const Quaternion& q1, const Quaternion& q2) ;

private:
    Float values[4];
    static const Float _1_ ;
    static const Float _0_ ;
    static const Float R2D ;
    static const Float D2R ;
};


Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs) ;


class EulerAngle
{

public :

    EulerAngle() {
        Yaw  =  Pitch  =  Roll  =  0 ;
    }
    EulerAngle(Float yaw, Float pitch, Float roll){
        Yaw = yaw ;  Pitch = pitch ; Roll = roll ;
    }

public :
    Float getYaw() const {return Yaw ;}
    Float getPitch() const {return Pitch ;}
    Float getRoll() const {return Roll ;}
    void  setYaw(Float Yaw) {this->Yaw = Yaw ;}
    void  setPitch(Float Pitch) {this->Pitch = Pitch ;}
    void  setRoll(Float Roll) {this->Roll = Roll ;}

    EulerAngle DegreeToRad () const {
        return EulerAngle( getYaw()*D2R, getPitch()*D2R, getRoll()*D2R ) ;
    }
    EulerAngle RadToDegree () const {
        return EulerAngle( getYaw()*R2D, getPitch()*R2D, getRoll()*R2D ) ;
    }

    Quaternion toQuat(EulerOrder order=EulerOrder_YXZ) const;
private:
    Float Yaw, Pitch, Roll ;
    static const Float D2R ;
    static const Float R2D ;

};


class AxisAngle {

public :
    AxisAngle() {
        Axis = Vector3D (0,0,1) ;
        Angle = 0 ;
    }

    AxisAngle ( const Vector3D& axis, Float angle ){
        Axis = axis ; Angle = angle ;
    }

public :
    const Vector3D & getAxis() const {return Axis ;}
    Float getAngle() const {return Angle;}
    void  setAngle(Float angle) { Angle = angle;}
    void  setAxis(const Vector3D& axis) { Axis = axis ; }

    AxisAngle DegreeToRad () const {
        return AxisAngle( Axis, Angle*D2R ) ;
    }
    AxisAngle RadToDegree () const {
        return AxisAngle( Axis, Angle*R2D ) ;
    }

    Quaternion toQuat () const ;

private:
    Vector3D Axis ;
    Float Angle ;
    static const Float D2R ;
    static const Float R2D ;
};


#endif
