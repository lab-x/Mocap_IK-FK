
#include "MathToolsCpp/Quaternion.h"


const Float Quaternion::_1_ = 0.99999999 ;
const Float Quaternion::_0_ = 0.00000001 ;
const Float Quaternion::R2D = 180.0 / 3.1415926 ;
const Float Quaternion::D2R = 3.1415926 / 180.0 ;
const Float EulerAngle::D2R = 3.1415926 / 180 ;
const Float EulerAngle::R2D = 180 / 3.1415926 ;
const Float AxisAngle::D2R = 3.1415926 / 180 ;
const Float AxisAngle::R2D = 180 / 3.1415926 ;


static inline double safeAcos (double Cos)
{
    Cos = Cos > 1 ? 1 : Cos ;
    Cos = Cos < (-1) ? (-1) : Cos ;
    return acos (Cos) ;
}

static inline double safeAsin (double Sin)
{
    Sin = Sin > 1 ? 1 : Sin ;
    Sin = Sin < (-1) ? (-1) : Sin ;
    return asin (Sin) ;
}

Quaternion::Quaternion() {
    values[0] = 1;
    values[1] = 0;
    values[2] = 0;
    values[3] = 0;
}
Quaternion::Quaternion(Float w, Float x, Float y, Float z) {
    values[0] = w;
    values[1] = x;
    values[2] = y;
    values[3] = z;
}
Quaternion::Quaternion(const Float quaternion[]) {
    values[0] = quaternion[0];
    values[1] = quaternion[1];
    values[2] = quaternion[2];
    values[3] = quaternion[3];
}

Quaternion::Quaternion(const Quaternion& other ) {
    values[0] = other.values[0];
    values[1] = other.values[1];
    values[2] = other.values[2];
    values[3] = other.values[3];
}


void Quaternion::normalize() {
    Float tmp = 1 / sqrt ( getW()*getW() + getX()*getX() +
                           getY()*getY() + getZ()*getZ() ) ;
    values[0] *= tmp ;
    values[1] *= tmp ;
    values[2] *= tmp ;
    values[3] *= tmp ;
}

Quaternion & Quaternion::operator=(const Quaternion& rhs) {
    values[0] = rhs.values[0] ;
    values[1] = rhs.values[1] ;
    values[2] = rhs.values[2] ;
    values[3] = rhs.values[3] ;
    return *this ;
}


Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs){

    Quaternion rst;
    rst.values[0] = lhs.values[0] * rhs.values[0] - lhs.values[1] * rhs.values[1] - lhs.values[2] * rhs.values[2] - lhs.values[3] * rhs.values[3];

    rst.values[1] = lhs.values[0] * rhs.values[1] + lhs.values[1] * rhs.values[0] + lhs.values[2] * rhs.values[3] - lhs.values[3] * rhs.values[2];

    rst.values[2] = lhs.values[0] * rhs.values[2] - lhs.values[1] * rhs.values[3] + lhs.values[2] * rhs.values[0] + lhs.values[3] * rhs.values[1];

    rst.values[3] = lhs.values[0] * rhs.values[3] + lhs.values[1] * rhs.values[2] - lhs.values[2] * rhs.values[1] + lhs.values[3] * rhs.values[0];
    return rst;
}

Quaternion operator+(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion rst;
    rst.values[0] = lhs.values[0] + rhs.values[0] ;
    rst.values[1] = lhs.values[1] + rhs.values[1] ;
    rst.values[2] = lhs.values[2] + rhs.values[2] ;
    rst.values[3] = lhs.values[3] + rhs.values[3] ;
    return rst;
}

Quaternion operator-(const Quaternion& lhs, const Quaternion& rhs)
{
    Quaternion rst;
    rst.values[0] = lhs.values[0] - rhs.values[0] ;
    rst.values[1] = lhs.values[1] - rhs.values[1] ;
    rst.values[2] = lhs.values[2] - rhs.values[2] ;
    rst.values[3] = lhs.values[3] - rhs.values[3] ;
    return rst;
}

Quaternion operator-(const Quaternion& rhs)
{
    Quaternion rst;
    rst.values[0] = - rhs.values[0] ;
    rst.values[1] = - rhs.values[1] ;
    rst.values[2] = - rhs.values[2] ;
    rst.values[3] = - rhs.values[3] ;
    return rst;
}

Quaternion operator*(const Quaternion& lhs, const Float r)
{
    Quaternion rst;
    rst.values[0] = lhs.values[0] * r ;
    rst.values[1] = lhs.values[1] * r ;
    rst.values[2] = lhs.values[2] * r ;
    rst.values[3] = lhs.values[3] * r ;
    return rst;
}

Quaternion operator*(const Float r, const Quaternion& rhs)
{
    Quaternion rst;
    rst.values[0] = rhs.values[0] * r ;
    rst.values[1] = rhs.values[1] * r ;
    rst.values[2] = rhs.values[2] * r ;
    rst.values[3] = rhs.values[3] * r ;
    return rst;
}



Quaternion Quaternion::conj() const {
    return Quaternion ( getW(), -getX(), -getY(), -getZ() ) ;
}

Vector3D Quaternion::rotVector (const Vector3D& v) const {
    Quaternion q = *this ;
    Quaternion V(0, v.getX(), v.getY(), v.getZ());
    Quaternion tmp = q*V*q.conj();
    return Vector3D(tmp.getX(), tmp.getY(), tmp.getZ());
}





Quaternion Quaternion::v2q(const Vector3D& vec1, const Vector3D& vec2)
{
    Vector3D V1 = vec1.getNormalized() ;
    Vector3D V2 = vec2.getNormalized() ;

    Vector3D V3 = V1 + V2 ;
    if ( V3.getLength() > _0_ )
    {
        V3.normalize();
        Vector3D Cross = Vector3D::Cross(V1, V3);
        Float w = Vector3D::Dot(V1,V3);
        Quaternion q(w, Cross.getX(), Cross.getY(), Cross.getZ());
        q.normalize();
        return q;
    }
    else
    {
        Vector3D Any1 (1,0,0) ;
        Vector3D Any2 (0,1,0) ;
        Vector3D Cross1 = Vector3D::Cross(V1, Any1);
        Vector3D Cross2 = Vector3D::Cross(V1, Any2);
        Vector3D Cross = Cross1.getLength() > Cross2.getLength() ? Cross1 : Cross2 ;
        Quaternion q(0, Cross.getX(), Cross.getY(), Cross.getZ());
        q.normalize();
        return q;
    }
}

Vector3D Quaternion::rotVbyQ (const Vector3D& v, const Quaternion& q)
{
    Quaternion V(0, v.getX(), v.getY(), v.getZ());
    Quaternion tmp = q*V*q.conj();
    return Vector3D(tmp.getX(), tmp.getY(), tmp.getZ());
}

EulerAngle Quaternion::toEuler (EulerOrder order) const {
    Quaternion q = *this ;

    Float r11, r12, r21, r31,  r32;
    if(order == EulerOrder_YXZ)
    {
        r11 = 2 * (q.getX() * q.getZ() + q.getW() * q.getY());
        r12 = q.getW() * q.getW() - q.getX() * q.getX() - q.getY() * q.getY() + powf(q.getZ(), 2);
        r21 = -2 * (q.getY()*q.getZ() - q.getW() * q.getX());
        r31 = 2*(q.getX() * q.getY() + q.getW()*q.getZ());
        r32 =q.getW() * q.getW() - q.getX() * q.getX() + q.getY() * q.getY() - powf(q.getZ(), 2);
    }
  /////
    else if(order == EulerOrder_ZYX)
    {
        r11 = 2 * (q.getX() * q.getY() + q.getW() * q.getZ());
        r12 = q.getW() * q.getW() + q.getX() * q.getX() - q.getY() * q.getY() - powf(q.getZ(), 2);
        r21 = -2 * (q.getX() * q.getZ() - q.getW() * q.getY());
        r31 = 2 * (q.getY() * q.getZ() + q.getW() * q.getX());
        r32 = q.getW() * q.getW() - q.getX() * q.getX() - q.getY() * q.getY() + powf(q.getZ(), 2);
    }
    else if(order == EulerOrder_ZXY)
    {
        r11 = -2 * (q.getX() * q.getY() - q.getW() * q.getZ());
        r12 = q.getW() * q.getW() - q.getX() * q.getX() + q.getY() * q.getY() - powf(q.getZ(), 2);
        r21 = 2 * (q.getY() * q.getZ() + q.getW() * q.getX());
        r31 = -2 * (q.getX() * q.getZ() - q.getW() * q.getY());
        r32 = q.getW() * q.getW() - q.getX() * q.getX() - q.getY() * q.getY() + powf(q.getZ(), 2);
    }
    else if(order == EulerOrder_YZX)
    {
        r11 = -2 * (q.getX() * q.getZ() - q.getW() * q.getY());
        r12 = q.getW()*q.getW() + q.getX()*q.getX() - q.getY()*q.getY() - q.getZ()*q.getZ();
        r21 = 2 * (q.getX() * q.getY() + q.getW() * q.getZ());
        r31 = -2 * (q.getY() * q.getZ() - q.getW() * q.getX());
        r32 = q.getW()*q.getW() - q.getX()*q.getX() + q.getY()*q.getY() - q.getZ()*q.getZ();
    }
    /////////
    else if(order == EulerOrder_XYZ)
    {
        r11 = -2 * (q.getY() * q.getZ() - q.getW() * q.getX());
        r12 = q.getW() * q.getW() - pow(q.getX(), 2) - q.getY() * q.getY() + powf(q.getZ(), 2);
        r21 = 2 * (q.getX() * q.getZ() + q.getW() * q.getY());
        r31 = -2 * (q.getX() * q.getY() - q.getW() * q.getZ());
        r32 = q.getW() * q.getW() + q.getX() * q.getX() - q.getY() * q.getY() - powf(q.getZ(), 2);
    }
    else if(order == EulerOrder_XZY)
    {
        r11 = 2 * (q.getY() * q.getZ() + q.getW() * q.getX());
        r12 = q.getW() * q.getW() - q.getX() * q.getX() + q.getY() * q.getY() - powf(q.getZ(), 2);
        r21 = -2 * (q.getX() * q.getY() - q.getW() * q.getZ());
        r31 = 2 * (q.getX() * q.getZ() + q.getW() * q.getY());
        r32 = q.getW() * q.getW() + q.getX() * q.getX() - q.getY() * q.getY() - powf(q.getZ(), 2);
    }
    else
    {
        return EulerAngle(0,0,0);
    }
    if ( r21 > _1_ )
        r21 = _1_ ;
    else if ( r21 < -_1_ )
        r21 = -_1_ ;
    return EulerAngle(atan2f(r11, r12), asinf(r21), atan2f(r31,r32));

}

Matrix Quaternion::Quat2Mat() const
{
    Float w,x,y,z;
    w = getW() ;
    x = getX() ;
    y = getY() ;
    z = getZ() ;

    Matrix RotMat(3, 3);
    Float sqw = w * w;
    Float sqx = x * x;
    Float sqy = y * y;
    Float sqz = z * z;

    RotMat(0, 0) = sqw + sqx - sqy - sqz;
    RotMat(1, 1) = sqw - sqx + sqy - sqx;
    RotMat(2, 2) = sqw - sqx - sqy + sqz;
    Float xy = x * y;
    Float zw = z * w;
    RotMat(0, 1) = 2.0 * (xy - zw);
    RotMat(1, 0) = 2.0 * (xy + zw);
    Float xz = x * z;
    Float yw = y * w;
    RotMat(0, 2) = 2.0 * (xz + yw);
    RotMat(2, 0) = 2.0 * (xz - yw);
    Float xw = x * w;
    Float yz = y * z;
    RotMat(1, 2) = 2.0 * (yz - xw);
    RotMat(2, 1) = 2.0 * (yz + xw);
    return RotMat;
}

Quaternion Quaternion::mat2Quat(const Matrix& M)
{
   Matrix A = M;

   int idx, i;
   Float w, x, y, z;
   Float U[4];
   Float max = 0;
   Float absm = 0;

   U[0] = 1 + A(0,0) + A(1,1) + A(2,2);
   U[1] = 1 + A(0,0) - A(1,1) - A(2,2);
   U[2] = 1 - A(0,0) + A(1,1) - A(2,2);
   U[3] = 1 - A(0,0) - A(1,1) + A(2,2);
   for(i=0;i<4;i++)
   {
       absm = fabs(U[i]);
       if(absm > max)
       {
           max = absm;
           idx = i;
       }
   }
   switch(idx)
   {
   case 0:
       w = U[0];
       x = A(2, 1) - A(1, 2);
       y = A(0, 2) - A(2, 0);
       z = A(1, 0) - A(0, 1);
       break;
   case 1:
       w = A(2, 1) - A(1, 2);
       x = U[1];
       y = A(0, 1) + A(1, 0);
       z = A(2, 0) + A(0, 2);
       break;
   case 2:
       w = A(0, 2) - A(2, 0);
       x = A(0, 1) + A(1, 0);
       y = U[2];
       z = A(2, 1) + A(1, 2);
       break;
   case 3:
       w = A(1, 0) - A(0, 1);
       x = A(0, 2) + A(2, 0);
       y = A(2, 1) + A(1, 2);
       z = U[3];
       break;
   default:
       break;
   }
   Quaternion Q=Quaternion(w,x,y,z);
   Q.normalize();
   return Q;
}

AxisAngle Quaternion::toAxisAngle() const {

    Float w,x,y,z;
    Quaternion q = *this ;
    q.normalize();
    w = q.getW() ;
    if ( w > _1_ )
        w = _1_ ;
    else if ( w < -_1_ )
        w = -_1_ ;

    Float SinTheta2 = 1-w*w ;
    if (SinTheta2 < 0 )
        SinTheta2 = 0 ;
    Float s = sqrt(SinTheta2);
    if ( s > _1_ )
        s = _1_ ;

    if ( s < _0_ )
    {
        s = _0_ ;
        x = y = z = 1 ;
    }
    else
    {
        x = q.getX() / s ;
        y = q.getY() / s ;
        z = q.getZ() / s ;
    }
    Float angle = 2 * asin (s) ;
    if ( w < 0 )
        angle = -angle ;
    return AxisAngle (Vector3D(x,y,z).normalize(), angle) ;
}




Quaternion Quaternion::slerp1(const Quaternion& p, const Quaternion& q, const Float t)
{
    Float W,X,Y,Z;
    Quaternion Start = Quaternion(p.getW(), p.getX(), p.getY(), p.getZ());
    Quaternion Stop = Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
    Float dot = Start.getW() * Stop.getW() + Start.getX() * Stop.getX() + Start.getY() * Stop.getY() + Start.getZ() * Stop.getZ() ;

    if(dot < _0_)
    {
        Start = Quaternion(-Start.getW(), -Start.getX(), -Start.getY(), -Start.getZ());
        dot = Start.getW() * Stop.getW() + Start.getX() * Stop.getX() + Start.getY() * Stop.getY() + Start.getZ() * Stop.getZ() ;
    }

    else if (dot > _1_)
    {
        dot = _1_;
    }
    Float Scaler1 = safeAsin(t * dot) / safeAsin(dot);
    Float Scaler2 = safeAsin((1 - t) * dot) / safeAsin(dot);


    W = Scaler1 * Start.getW() + Scaler2 * Stop.getW();
    X = Scaler1 * Start.getX() + Scaler2 * Stop.getX();
    Y = Scaler1 * Start.getY() + Scaler2 * Stop.getY();
    Z = Scaler1 * Start.getZ() + Scaler2 * Stop.getZ();
    Quaternion Q(W, X, Y, Z);
    Q.normalize();
    return Q;
}

Float Quaternion::Dot (const Quaternion& q1, const Quaternion& q2)
{
    return  q1.getW() * q2.getW() +
            q1.getX() * q2.getX() +
            q1.getY() * q2.getY() +
            q1.getZ() * q2.getZ() ;
}

Quaternion Quaternion::Slerp(const Quaternion& startQ, const Quaternion& endQ, Float endWeight)
{
    Quaternion start = startQ ;
    Quaternion end = endQ ;
    Float dot = Dot (start, end) ;
    if ( dot < 0.0 )
    {
        end = - end ;
        dot = - dot ;
    }

    dot = dot > _1_ ? _1_ : dot ;
    endWeight = endWeight > 1.0 ? 1.0 : endWeight ;
    endWeight = endWeight < 0.0 ? 0.0 : endWeight ;

    Float theta = acos(dot) ;
    Float sinTheta = sin(theta) ;
    Float rStart = 1.0 - endWeight , rEnd = endWeight ;
    if ( sinTheta > _0_ )
    {
        rEnd = sin(endWeight*theta) / sinTheta ;
        rStart = sin(theta - endWeight*theta) / sinTheta ;
    }
    Quaternion rst = rStart * start + rEnd * end ;
    //rst.normalize();
    return rst ;
}

Quaternion Quaternion::slerp(const Quaternion& Q1, const Quaternion& Q2, const Float ratio)
{
    Quaternion dQ = Q1.conj() * Q2 ;
    AxisAngle dAA = dQ.toAxisAngle() ;
    dAA.setAngle ( dAA.getAngle()*ratio );
    dQ = dAA.toQuat() ;
    return Q1*dQ ;
}

Quaternion EulerAngle::toQuat(EulerOrder order) const{
    EulerAngle EA = *this ;

    Float ang[3] = {EA.getYaw(), EA.getPitch(), EA.getRoll()};
    Float cang[3] = {cosf(0.5 * ang[0]), cosf(0.5 * ang[1]), cosf(0.5 * ang[2])};
    Float sang[3] = {sinf(0.5 * ang[0]), sinf(0.5 * ang[1]), sinf(0.5 * ang[2])};
    Float w, x, y, z;
    if(order == EulerOrder_YXZ)
    {
        w = cang[0] * cang[1] * cang[2] + sang[0] * sang[1] * sang[2];
        x = cang[0] * sang[1] * cang[2] + sang[0] * cang[1] * sang[2];
        y = sang[0] * cang[1] * cang[2] - cang[0] * sang[1] * sang[2];
        z = cang[0] * cang[1] * sang[2] - sang[0] * sang[1] * cang[2];
    }
    else if(order == EulerOrder_ZYX)
    {
        w = cang[0] * cang[1] * cang[2] + sang[0] * sang[1] * sang[2];
        x = cang[0] * cang[1] * sang[2] - sang[0] * sang[1] * cang[2];
        y = cang[0] * sang[1] * cang[2] + sang[0] * cang[1] * sang[2];
        z = sang[0] * cang[1] * cang[2] - cang[0] * sang[1] * sang[2];
    }
    else if(order == EulerOrder_ZXY)
    {
        w = cang[0] * cang[1] * cang[2] - sang[0] * sang[1] * sang[2];
        x = cang[0] * sang[1] * cang[2] - sang[0] * cang[1] * sang[2];
        y = cang[0] * cang[1] * sang[2] + sang[0] * sang[1] * cang[2];
        z = cang[0] * sang[1] * sang[2] + sang[0] * cang[1] * cang[2];
    }
    else if(order == EulerOrder_YZX)
    {
        w = cang[0] * cang[1] * cang[2] - sang[0] * sang[1] * sang[2];
        x = cang[0] * cang[1] * sang[2] + sang[0] * sang[1] * cang[2];
        y = cang[0] * sang[1] * sang[2] + sang[0] * cang[1] * cang[2];
        z = cang[0] * sang[1] * cang[2] - sang[0] * cang[1] * sang[2];
    }
    else if(order == EulerOrder_XYZ)
    {
        w = cang[0] * cang[1] * cang[2] - sang[0] * sang[1] * sang[2];
        x = cang[0] * sang[1] * sang[2] + sang[0] * cang[1] * cang[2];
        y = cang[0] * sang[1] * cang[2] - sang[0] * cang[1] * sang[2];
        z = cang[0] * cang[1] * sang[2] + sang[0] * sang[1] * cang[2];
    }
    else if(order == EulerOrder_XZY)
    {
        w = cang[0] * cang[1] * cang[2] + sang[0] * sang[1] * sang[2];
        x = sang[0] * cang[1] * cang[2] - cang[0] * sang[1] * sang[2];
        y = cang[0] * cang[1] * sang[2] - sang[0] * sang[1] * cang[2];
        z = cang[0] * sang[1] * cang[2] + sang[0] * cang[1] * sang[2];
    }
    return Quaternion(w, x, y, z);
}


Quaternion AxisAngle::toQuat () const {
    // Float RadAng = Angle*D2R;
    Float RadAng = Angle ;
    Float qw = cos(RadAng/2);
    Float qx = Axis.getX() * sin(RadAng/2);
    Float qy = Axis.getY() * sin(RadAng/2);
    Float qz = Axis.getZ() * sin(RadAng/2);
    Quaternion Q = Quaternion(qw,qx,qy,qz);
    Q.normalize();
    return Q ;
}

