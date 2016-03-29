
#include <iostream>
using std::cout ;
using std::endl ;

#include "InverseKinematics/Separate3DOFConstraint.h"
#include "InverseKinematics/Joint.h"
#include "InverseKinematics/fabrik.h"


static bool SimilarVector ( const Vector3D& V1, const Vector3D& V2 )
{
    Float minLen = V1.getLength() < V2.getLength() ? V1.getLength() : V2.getLength() ;
    Float deltaLen = (V1 - V2).getLength() ;
    if ( deltaLen / minLen < 0.0001 )
        return true ;
    else
        return false ;
}



void Separate3DOFConstraint::setParam(void*_param)
{
    param = *(Param*)_param ;

    bool lastAxisInv ;
    if ( SimilarVector ( Vector3D::Cross(param.Axis[0] , param.Axis[1]) , param.Axis[2] ) )
    {
        lastAxisInv = true ;
        paramLegal = true ;
    }
    else if ( SimilarVector ( Vector3D::Cross(param.Axis[0] , param.Axis[1]) , - param.Axis[2] ) )
    {
        lastAxisInv = false ;
        paramLegal = true ;
    }
    else
    {
        paramLegal = false ;
    }



    if ( paramLegal )
    {
        // the second axis can only rotate between -90 to 90 .
        if ( ! ( param.DOFs[0].minAngle >= -180 && param.DOFs[0].minAngle <= 180 &&
                 param.DOFs[0].maxAngle >= -180 && param.DOFs[0].maxAngle <= 180 &&
                 param.DOFs[2].minAngle >= -180 && param.DOFs[2].minAngle <= 180 &&
                 param.DOFs[2].maxAngle >= -180 && param.DOFs[2].maxAngle <= 180 &&
                 param.DOFs[1].minAngle >= -90 &&
                 param.DOFs[1].maxAngle <= 90  )  )

            paramLegal = false ;
    }


    if ( paramLegal )
    {
        BWDParam = param ;
        if ( lastAxisInv )
        {
            BWDParam.Axis[2] = - param.Axis[2] ;
            BWDParam.DOFs[2].maxAngle = - BWDParam.DOFs[2].minAngle ;
            BWDParam.DOFs[2].minAngle = - BWDParam.DOFs[2].maxAngle ;
        }
        FWDParam.DOFs[0] = BWDParam.DOFs[2] ;
        FWDParam.DOFs[1] = BWDParam.DOFs[1] ;
        FWDParam.DOFs[2] = BWDParam.DOFs[0] ;
        FWDParam.Axis[0] = - BWDParam.Axis[2] ;
        FWDParam.Axis[1] = - BWDParam.Axis[1] ;
        FWDParam.Axis[2] = - BWDParam.Axis[0] ;

        setInvAxis ( FWDParam ) ;
        setInvAxis ( BWDParam ) ;
    }

}


void Separate3DOFConstraint::setInvAxis ( Param& param )
{
    Vector3D Vx, Vy, Vz ;
    Vector3D X(1,0,0), Y(0,1,0), Z(0,0,1) ;

    // YXZ order .
    Vy = param.Axis[0] ;
    Vx = param.Axis[1] ;
    Vz = param.Axis[2] ;

    // invAxis are also in YXZ order
    param.invAxis[0] = Vector3D ( Vector3D::Dot(Y,Vx), Vector3D::Dot(Y,Vy), Vector3D::Dot(Y,Vz) ) ;
    param.invAxis[1] = Vector3D ( Vector3D::Dot(X,Vx), Vector3D::Dot(X,Vy), Vector3D::Dot(X,Vz) ) ;
    param.invAxis[2] = Vector3D ( Vector3D::Dot(Z,Vx), Vector3D::Dot(Z,Vy), Vector3D::Dot(Z,Vz) ) ;
}


Vector3D Separate3DOFConstraint::mapVector ( const Vector3D& Vec, const Vector3D* axis )
{
    Float X,Y,Z ;

    // axis are in YXZ order
    Y = Vector3D::Dot ( Vec, axis[0] ) ;
    X = Vector3D::Dot ( Vec, axis[1] ) ;
    Z = Vector3D::Dot ( Vec, axis[2] ) ;

    return Vector3D(X,Y,Z) ;
}


void Separate3DOFConstraint::constraintEuler(EulerAngle& E, const Param& param)
{
    Float minAngle , maxAngle, angle ;
    minAngle = param.DOFs[0].minAngle ;
    maxAngle = param.DOFs[0].maxAngle ;
    angle = E.getYaw() ;
    if ( ( minAngle > maxAngle && (angle < maxAngle || angle > minAngle) )  ||
         ( minAngle <= maxAngle && (angle < maxAngle && angle > minAngle) ) )
    {
        // good angle , no need to modify .
    }
    else
    {
        if ( fabs(angle-maxAngle) < fabs(angle-minAngle) )
            angle = maxAngle ;
        else
            angle = minAngle ;
    }
    E.setYaw(angle);


    minAngle = param.DOFs[1].minAngle ;
    maxAngle = param.DOFs[1].maxAngle ;
    angle = E.getPitch() ;
    if (  angle < maxAngle && angle > minAngle  )
    {
        // good angle , no need to modify .
    }
    else
    {
        if ( fabs(angle-maxAngle) < fabs(angle-minAngle) )
            angle = maxAngle ;
        else
            angle = minAngle ;
    }
    E.setPitch(angle);



    minAngle = param.DOFs[2].minAngle ;
    maxAngle = param.DOFs[2].maxAngle ;
    angle = E.getRoll() ;
    if ( ( minAngle > maxAngle && (angle < maxAngle || angle > minAngle) )  ||
         ( minAngle <= maxAngle && (angle < maxAngle && angle > minAngle) ) )
    {
        // good angle , no need to modify .
    }
    else
    {
        if ( fabs(angle-maxAngle) < fabs(angle-minAngle) )
            angle = maxAngle ;
        else
            angle = minAngle ;
    }
    E.setRoll(angle);
}

void Separate3DOFConstraint::constraintQuat(Quaternion& qLocal, const Param& param)
{
    Vector3D axis0, axis ;
    axis0 = Vector3D (qLocal.getX(), qLocal.getY(), qLocal.getZ() ) ;
    axis = mapVector ( axis0, param.Axis ) ;

    Quaternion Q (qLocal.getW(), axis.getX(), axis.getY(), axis.getZ() ) ;
    EulerAngle E = Q.toEuler().RadToDegree() ;
    constraintEuler(E, param) ;
    qLocal = E.DegreeToRad().toQuat() ;

    axis0 = Vector3D (qLocal.getX(), qLocal.getY(), qLocal.getZ() ) ;
    axis = mapVector ( axis0, param.invAxis ) ;

    qLocal = Quaternion ( qLocal.getW(), axis.getX(), axis.getY(), axis.getZ() ) ;
}


void Separate3DOFConstraint::runFWDConstraint()
{
    if ( ! paramLegal )
        return ;

    Joint* jointToConstrain = joint->getFWDJoint() ;
    if ( NULL == jointToConstrain )
        return ;


    Quaternion qlParent ;
    Quaternion qwParent = joint->getQwFWD() ;
    Quaternion qwChild = joint->getQwBWD() ;
    qlParent = qwChild.conj() * qwParent ;

    constraintQuat(qlParent,FWDParam);

    qwParent = qwChild * qlParent ;
    joint->setQwFWD(qwParent);
    jointToConstrain->setQwBWD(qwParent);

    Vector3D V1 = jointToConstrain->getFabrik()->getInitBones() [jointToConstrain->getID()] ;
    Vector3D V2 = Quaternion::rotVbyQ ( V1, qwParent ) ;
    Vector3D newP = Vector3D ( joint->getPosition() ) - V2 ;
    jointToConstrain->setPosition ( newP )  ;
    return ;
}



void Separate3DOFConstraint::runBWDConstraint()
{
    if ( ! paramLegal )
        return ;

    Joint* jointToConstrain = joint->getBWDJoint() ;
    if ( NULL == jointToConstrain )
        return ;

    Quaternion qlChild ;
    Quaternion qwParent = joint->getQwFWD() ;
    Quaternion qwChild = joint->getQwBWD() ;
    qlChild = qwParent.conj() * qwChild ;
    constraintQuat(qlChild,BWDParam);

    qwChild = qwParent * qlChild ;
    joint->setQwBWD(qwChild);
    jointToConstrain->setQwFWD(qwChild);

    Vector3D V1 = joint->getFabrik()->getInitBones() [joint->getID()] ;
    Vector3D V2 = Quaternion::rotVbyQ ( V1, qwChild ) ;
    Vector3D newP = Vector3D ( joint->getPosition() ) + V2 ;
    jointToConstrain->setPosition ( newP )  ;
    return ;

}

