#include "LegConstraint.h"



void LegConstraint::runFWDConstraint()
{
    // DO NOTHING
}

//  最后还要根据thigh的朝向重新计算Hip的Orientation。修正Chest差值的Hip结果。
void LegConstraint::runBWDConstraint()
{
    if(param.side & Left){

    }
    else if(param.side & Right)
    {

    }
    else
        return;

/*
    Joint* jointToConstrain = joint->getBWDJoint() ;
    if ( NULL == jointToConstrain )
        return ;
    //FIND ROTATION(NON-TWISTED BONE)
    Position Pjoint_BWD = joint->getBWDJoint()->getPosition();
    Position Pjoint_this =  joint->getPosition();
    Position Pjoint_FWD = joint->getFWDJoint()->getPosition();
    Vector3D b_BWD = Vector3D((Pjoint_BWD-Pjoint_this).getX(), (Pjoint_BWD-Pjoint_this).getY(), (Pjoint_BWD-Pjoint_this).getZ());
    Vector3D b_FWD = Vector3D((Pjoint_this-Pjoint_FWD).getX(), (Pjoint_this-Pjoint_FWD).getY(), (Pjoint_this-Pjoint_FWD).getZ());
    Quaternion Q_FWD_2_BWD = Quaternion::v2q(b_FWD, b_BWD);
    Quaternion QW_BWD_ZeroTwist = Q_FWD_2_BWD * (joint->getQwFWD()) ;
    //GET ROTATED BONE
    Vector3D Initbone_BWD = joint->getFabrik()->getInitBones() [joint->getID()] ;
    Vector3D Vbone_BWD = Quaternion::rotVbyQ ( Initbone_BWD, QW_BWD_ZeroTwist ) ;
    Vector3D P_BWD = Vector3D ( joint->getPosition() ) + Vbone_BWD ;
    jointToConstrain->setPosition ( P_BWD )  ;
    //USE LowerArm TO COMPUTE FullTwist
    Vector3D Initbone_BWDBWD = joint->getFabrik()->getInitBones() [jointToConstrain->getID()] ;
    Vector3D b_BWDBWD = Quaternion::rotVbyQ ( Initbone_BWDBWD, param.q) ;
    Float Dot = Vector3D::Dot(Vbone_BWD.getNormalized(), b_BWDBWD.getNormalized());
    Quaternion QW_BWD = QW_BWD_ZeroTwist;
    //TwistAngle
    if( Dot > 0.00000001)
    {
        Quaternion Q_BWDBWD_2_BWD = Quaternion::v2q(b_BWDBWD, Vbone_BWD);
        Quaternion QW_BWD_FullTwist = Q_BWDBWD_2_BWD * param.q ;
        float LowerArmWeight = Dot * 0.5 ;
        QW_BWD = Quaternion::slerp1(QW_BWD_ZeroTwist, QW_BWD_FullTwist, (1-LowerArmWeight));
        Vector3D V_zeroTwist = Quaternion::rotVbyQ(Initbone_BWD.getNormalized(), QW_BWD_ZeroTwist);
        Vector3D V_fullTwist = Quaternion::rotVbyQ(Initbone_BWD.getNormalized(), QW_BWD_FullTwist);
      //  printf("V0: %f %f %f \t V1: %f %f %f \n", V_zeroTwist.getX(),V_zeroTwist.getY(),V_zeroTwist.getZ(),V_fullTwist.getX(),V_fullTwist.getY(),V_fullTwist.getZ());
    }
    else
    {
        QW_BWD = QW_BWD_ZeroTwist;
    }
    joint->setQwBWD(QW_BWD);
    jointToConstrain->setQwFWD(QW_BWD);
    */
    return;
}

void LegConstraint::setParam(void*param)
{
    this->param = *((Param*)param);
}



