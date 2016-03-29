
#include "MathToolsCpp/Vector3D.h"
#include "InverseKinematics/MaxAngleConstraint.h"
#include "InverseKinematics/Joint.h"
#include "InverseKinematics/fabrik.h"



void MaxAngleConstraint::runFWDConstraint()
{
    Joint* jointToConstrain = joint->getFWDJoint() ;
    if ( NULL == jointToConstrain )
        return ;

    Quaternion dq ;
    Quaternion q1 = joint->getQwBWD() ;
    Quaternion q2 = joint->getQwFWD() ;
    dq = q1.conj()*q2 ;
    AxisAngle axisAngle = dq.toAxisAngle().RadToDegree() ;
    float angle = axisAngle.getAngle() ;
    float maxAngle = param.maxAngle ;
    if (angle > maxAngle)
        angle = maxAngle ;
    else if (angle < -maxAngle)
        angle = -maxAngle ;
    axisAngle.setAngle(angle);
    dq = axisAngle.DegreeToRad().toQuat() ;
    q2 = q1*dq ;
    joint->setQwFWD(q2);
    jointToConstrain->setQwBWD(q2);

    Vector3D V1 = jointToConstrain->getFabrik()->getInitBones() [jointToConstrain->getID()] ;
    Vector3D V2 = Quaternion::rotVbyQ ( V1, q2 ) ;
    Vector3D newP = Vector3D ( joint->getPosition() ) - V2 ;
    jointToConstrain->setPosition ( newP )  ;

    return ;
}

void MaxAngleConstraint::runBWDConstraint()
{
    Joint* jointToConstrain = joint->getBWDJoint() ;
    if ( NULL == jointToConstrain )
        return ;

    Quaternion dq ;
    Quaternion q1 = joint->getQwFWD() ;
    Quaternion q2 = joint->getQwBWD() ;
    dq = q1.conj()*q2 ;
    AxisAngle axisAngle = dq.toAxisAngle().RadToDegree() ;
    float angle = axisAngle.getAngle() ;
    float maxAngle = param.maxAngle ;
    if (angle > maxAngle)
        angle = maxAngle ;
    else if (angle < -maxAngle)
        angle = -maxAngle ;
    axisAngle.setAngle(angle);
    dq = axisAngle.DegreeToRad().toQuat() ;
    q2 = q1*dq ;
    joint->setQwBWD(q2);
    jointToConstrain->setQwFWD(q2);

    Vector3D V1 = joint->getFabrik()->getInitBones() [joint->getID()] ;
    Vector3D V2 = Quaternion::rotVbyQ ( V1, q2 ) ;
    Vector3D newP = Vector3D ( joint->getPosition() ) + V2 ;
    jointToConstrain->setPosition ( newP )  ;
    return ;
}



void MaxAngleConstraint::setParam(void*param)
{
    this->param = * ( (Param*)param ) ;
}

