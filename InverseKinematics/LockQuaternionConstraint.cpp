
#include "MathToolsCpp/Vector3D.h"
#include "InverseKinematics/LockQuaternionConstraint.h"
#include "InverseKinematics/Joint.h"
#include "InverseKinematics/fabrik.h"

void LockQuaternionConstraint::runFWDConstraint()
{
    Joint* jointToConstrain = joint->getFWDJoint() ;
    if ( NULL == jointToConstrain )
        return ;

    Quaternion q = param.q ;
    joint->setQwFWD(q);
    jointToConstrain->setQwBWD(q);
    Vector3D V1 = jointToConstrain->getFabrik()->getInitBones() [jointToConstrain->getID()] ;
    Vector3D V2 = Quaternion::rotVbyQ ( V1, q ) ;
    Vector3D newP = Vector3D ( joint->getPosition() ) - V2 ;
    jointToConstrain->setPosition ( newP )  ;
    return ;
}

void LockQuaternionConstraint::runBWDConstraint()
{
    Joint* jointToConstrain = joint->getBWDJoint() ;
    if ( NULL == jointToConstrain )
        return ;

    Quaternion q = param.q ;
    joint->setQwBWD(q);
    jointToConstrain->setQwFWD(q);
    Vector3D V1 = joint->getFabrik()->getInitBones() [joint->getID()] ;
    Vector3D V2 = Quaternion::rotVbyQ ( V1, q ) ;
    Vector3D newP = Vector3D ( joint->getPosition() ) + V2 ;
    jointToConstrain->setPosition ( newP )  ;
    return ;
}




void LockQuaternionConstraint::setParam(void*param)
{
    this->param = * ( (Param*)param ) ;
}

