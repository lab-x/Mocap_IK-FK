#include "InverseKinematics/HumanFK.h"
#include <QDebug>

HumanFK::HumanFK()
{
    resetOutput();
    setDefaultModel();
    setDefaultOffset();
    isTposeCalibrated = false;
}

void HumanFK::resetOutput ()
{

    OutputFrame* p = &LatestOutput;
    p->HeadPos = p->LeftAnklePos = p->LeftWristPos = p->NeckPos =
            p->RightAnklePos = p->RightWristPos = Vector3D(0, 0, 0);
/*
    p->HipOri = p->ChestOri = p->LeftShoulderOri = p->LeftUpperArmOri =
            p->LeftLowerArmOri = p->LeftHandOri = p->RightShoulderOri =
            p->RightUpperArmOri = p->RightLowerArmOri = p->RightHandOri =
            p->HeadOri = p->LeftThighRootOri = p->LeftThighOri =
            p->LeftShankOri = p->LeftFootOri = p->RightThighRootOri =
            p->RightThighOri = p->RightShankOri = p->RightFootOri = EulerAngle(0,0,0) ;
    p->NeckPos = p->LeftWristPos = p->LeftElbowPos = p->LeftShoulderPos = p->LeftThighPos =
            p->LeftKneePos = p->LeftAnklePos = p->HeadPos = p->HipPos = p->RightShoulderPos =
            p->RightElbowPos = p->RightWristPos = p->RightThighPos = p->RightKneePos = p->RightAnklePos =
            Vector3D (0,0,0) ;
*/
}

void HumanFK::setDefaultModel()
{
    Model.LenHipToChest = 0.33f ;       //0.20f;
    Model.LenHipToThigh = 0.15f ;       //0.12f;
    Model.LenThighToKnee = 0.55f ;      //0.40f;
    Model.LenKneeToAnkle = 0.45f ;      //0.40f;
    Model.LenChestToNeck = 0.19f ;      //0.20f;
    Model.LenNeckToShoulder = 0.20f ;   //0.20f;
    Model.LenShoulderToElbow = 0.27f ;  //0.25f;
    Model.LenElbowToWrist = 0.27f ;     //0.22f;
    Model.LenNeckToHead = 0.30f ;       //0.30f;
    updateChains();
}
void HumanFK::setDefaultOffset()
{
    Offsets.offset_Head = Vector3D(0, 0, 0);
    Offsets.offset_Neck = Vector3D(0, 0, 0);
    Offsets.offset_LeftAnkle = Vector3D(0, 0, 0);
    Offsets.offset_RightAnkle = Vector3D(0, 0, 0);
    Offsets.offset_LeftWrist = Vector3D(0, 0, 0);
    Offsets.offset_RightWrist = Vector3D(0, 0, 0);
}

void HumanFK::setModel(const SkeletonModel& m){
    Model = m;
    updateChains();
}

Vector3D HumanFK::getOffset(JointID id){
    switch(id)
    {
    case Joint_LeftWrist:
        return Offsets.offset_LeftWrist;
    case Joint_RightWrist:
        return Offsets.offset_RightWrist;
    case Joint_LeftAnkle:
        return Offsets.offset_LeftAnkle;
    case Joint_RightAnkle:
        return Offsets.offset_RightAnkle;
    case Joint_Neck:
        return Offsets.offset_Neck;
    default:
        return Vector3D(0,0,0);
    }
}

Quaternion HumanFK::getQinit(JointID id){
    switch(id)
    {
    case Joint_LeftWrist:
        return Qinit.Qinit_LeftWrist;
    case Joint_RightWrist:
        return Qinit.Qinit_RightWrist;
    case Joint_LeftAnkle:
        return Qinit.Qinit_LeftAnkle;
    case Joint_RightAnkle:
        return Qinit.Qinit_RightAnkle;
    case Joint_Neck:
        return Qinit.Qinit_Neck;
    case Joint_LeftElbow:
        return Qinit.Qinit_LeftElbow;
    case Joint_RightElbow:
        return Qinit.Qinit_RightElbow;
    default:
        return Quaternion(1,0,0,0);
    }
}

void HumanFK::compute ( const InputFrame& Din )
{
    Vector3D result, b0, b1, b2;
    int id = getJointRawDataIdx(Din.id);
    Chain Bones = Chains[id];
    b0 = Quaternion::rotVbyQ(Bones.B0, Din.Qroot);
    b1 = Quaternion::rotVbyQ(Bones.B1, Din.Qlinker);
    b2 = Quaternion::rotVbyQ(Bones.B2, Din.Qthis);
    result = Din.Proot + Bones.LenB0 * b0 + Bones.LenB1 * b1 + Bones.LenB2 * b2;
    updateFKResult(id, result);
}

bool HumanFK::updateFKResult(int id, Vector3D P)
{
    OutputFrame& output = LatestOutput ;
    switch(id){
    case 0:
        output.LeftWristPos = P;
        break;
    case 1:
        output.RightWristPos = P;
        break;
    case 2:
        output.LeftAnklePos = P;
        break;
    case 3:
        output.RightAnklePos = P;
        break;
    case 4:
        output.NeckPos = P;
        break;
    default:
        return false;
    }
    return true;
}

void HumanFK::updateChains(){
    //Root To Target Manner
    Chains[0].B0 = Chains[0].B1 = Chains[0].B2 = Chains[2].B0 = Vector3D(1, 0, 0);
    Chains[1].B0 = Chains[1].B1 = Chains[1].B2 = Chains[3].B0 = Vector3D(-1, 0, 0);
    Chains[2].B1 = Chains[2].B2 = Chains[3].B1 = Chains[3].B2 = Vector3D(0, -1, 0);
    Chains[4].B0 = Chains[4].B1 = Chains[4].B2 = Vector3D(0, 1, 0);

    Chains[0].LenB0 = Chains[1].LenB0 = Model.LenNeckToShoulder;
    Chains[0].LenB1 = Chains[1].LenB1 = Model.LenShoulderToElbow;
    Chains[0].LenB2 = Chains[1].LenB2 = Model.LenElbowToWrist;

    Chains[2].LenB0 = Chains[3].LenB0 = Model.LenHipToThigh;
    Chains[2].LenB1 = Chains[3].LenB1 = Model.LenThighToKnee;
    Chains[2].LenB2 = Chains[3].LenB2 = Model.LenKneeToAnkle;

    Chains[4].LenB0 = Model.LenHipToChest;
    Chains[4].LenB1 = 0;
    Chains[4].LenB2 = Model.LenChestToNeck;
}

/*
void HumanFK::computeForDebug (const InputFrame& Din, Vector3D& P0,Vector3D& P1,Vector3D& P2,Vector3D& P3  )
{
    Vector3D b0, b1, b2;
    int id = getJointRawDataIdx(Din.id);
    Chain Bones = Chains[id];
    b0 = Quaternion::rotVbyQ(Bones.B0, Din.Qroot);
    b1 = Quaternion::rotVbyQ(Bones.B1, Din.Qlinker);
    b2 = Quaternion::rotVbyQ(Bones.B2, Din.Qthis);
    P0 =  Din.Proot;
    P1 = Din.Proot + Bones.LenB0 * b0;
    P2 = Din.Proot + Bones.LenB0 * b0 + Bones.LenB1 * b1;
    P3 = Din.Proot + Bones.LenB0 * b0 + Bones.LenB1 * b1 + Bones.LenB2 * b2;
    updateFKResult(id, P3);

}*/
