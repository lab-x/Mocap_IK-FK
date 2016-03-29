#include "InverseKinematics/Joint.h"
#include "InverseKinematics/fabrik.h"
#include "InverseKinematics/LockOrientationConstraint.h"
/*
 * For Knee joints -> constrint Thigh Twist == Shank Twist
 * Known Bug: For Elbow Joint with TwistAngle-/+90 --> Not a continuous Qtwist
 *
 */
static const double RtoD = 180.0/3.1415926 ;
static const double DtoR = 3.1415926/180.0 ;

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
////////////////////////////////////
/// \brief LockOrientationConstraint::runFWDConstraint
///     this part is deployed by legs -> Knee Joints
///
void LockOrientationConstraint::runFWDConstraint()
{
    if(joint->getID() == 2)
    {
        Joint* jointToConstrain = joint->getFWDJoint() ;
        if ( NULL == jointToConstrain )
            return ;
        Position Pjoint_BWD = joint->getBWDJoint()->getPosition();
        Position Pjoint_this =  joint->getPosition();
        Position Pjoint_FWD = joint->getFWDJoint()->getPosition();
        Vector3D b_FWD = Vector3D((Pjoint_this-Pjoint_FWD).getX(), (Pjoint_this-Pjoint_FWD).getY(), (Pjoint_this-Pjoint_FWD).getZ());
        Vector3D b_BWD = Vector3D((Pjoint_BWD-Pjoint_this).getX(), (Pjoint_BWD-Pjoint_this).getY(), (Pjoint_BWD-Pjoint_this).getZ());
        Vector3D InitBone_FWD = jointToConstrain->getFabrik()->getInitBones() [jointToConstrain->getID()];
        Vector3D InitBone_BWD = jointToConstrain->getFabrik()->getInitBones() [joint->getID()];
        Quaternion Q_Rot_BWD = Quaternion::v2q(InitBone_BWD, b_BWD);
        Quaternion Q_Twist_BWD = Q_Rot_BWD.conj() * (joint->getQwBWD()) ;
        param.Yaw = Q_Twist_BWD;
///////////////////////////////////////////////////////////////////////
if(Q_Twist_BWD.getX()<0){
    Q_Twist_BWD = Quaternion(-Q_Twist_BWD.getW(), -Q_Twist_BWD.getX(),-Q_Twist_BWD.getY(),-Q_Twist_BWD.getZ());
}
///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
        Quaternion Q_Rot_FWD = Quaternion::v2q(InitBone_FWD, b_FWD);
        // if (joint->getFabrik()->getChainID() == 3||joint->getFabrik()->getChainID() == 4)
        Quaternion QW_FWD = Q_Twist_BWD * Q_Rot_FWD;

        joint->setQwFWD(QW_FWD);
        jointToConstrain->setQwBWD(QW_FWD);
        Vector3D Vbone_FWD = Quaternion::rotVbyQ ( InitBone_FWD, QW_FWD) ;
        Vector3D P_FWD = Vector3D ( joint->getPosition() ) - Vbone_FWD ;
        jointToConstrain->setPosition ( P_FWD )  ;
    }
    return ;
}

void LockOrientationConstraint::runBWDConstraint()
{
    if(joint->getID() == 1)
    {
        Joint* jointToConstrain = joint->getBWDJoint() ;
        if ( NULL == jointToConstrain )
            return ;
        Position Pjoint_BWD = joint->getBWDJoint()->getPosition();
        Position Pjoint_this =  joint->getPosition();
        Position Pjoint_FWD = joint->getFWDJoint()->getPosition();
        Vector3D b_BWD = Vector3D((Pjoint_BWD-Pjoint_this).getX(), (Pjoint_BWD-Pjoint_this).getY(), (Pjoint_BWD-Pjoint_this).getZ());
        Vector3D b_FWD = Vector3D((Pjoint_this-Pjoint_FWD).getX(), (Pjoint_this-Pjoint_FWD).getY(), (Pjoint_this-Pjoint_FWD).getZ());

        Quaternion Q_FWD_2_BWD = Quaternion::v2q(b_FWD, b_BWD);
        //FWD Quaternion(Shoulder Orientation) -to- BWD Rotation(UpperArm Rotation)
        Quaternion QW_BWD = Q_FWD_2_BWD * (joint->getQwFWD()) ;
        joint->setQwBWD(QW_BWD);
        jointToConstrain->setQwFWD(QW_BWD);

        Vector3D Initbone_BWD = joint->getFabrik()->getInitBones() [joint->getID()] ;
        Vector3D Vbone_BWD = Quaternion::rotVbyQ ( Initbone_BWD, QW_BWD ) ;
        Vector3D P_BWD = Vector3D ( joint->getPosition() ) + Vbone_BWD ;
        jointToConstrain->setPosition ( P_BWD )  ;
        return ;
    }
    return;
}

void LockOrientationConstraint::setParam(void*param)
{
    // do nothing.
}

