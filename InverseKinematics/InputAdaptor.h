#ifndef __InverseKinematics__INPUTADAPTOR__
#define __InverseKinematics__INPUTADAPTOR__

#include "POT.h"
#include "InverseKinematics/HumanFK.h"
#include "HumanIK/HumanIK.h"

class InputAdaptor
{
friend class IKMonitor;
public:

//struct DebugLeftArm{
//    Vector3D PNeck;
//    Vector3D PShoulder;
//    Vector3D PElbow;
//    Vector3D PWrist;
//    Quaternion QRoot; //Shoulder
//    Quaternion QLinker; //UpperArm
//    Quaternion QThis;  //ForeArm
//};
struct PNeck{
    Vector3D LeftWrist;
    Vector3D RightWrist;
    Vector3D Legs;
    Vector3D P_Neck;
};

struct ResultBuffer{
       Vector3D LatestPNeck;
       Vector3D P_Neck;
       Vector3D P_LeftWrist;
       Vector3D P_RightWrist;
       Vector3D P_LeftAnkle;
       Vector3D P_RightAnkle;
       Vector3D P_Head;

       Vector3D fkP_Neck;
       Vector3D fkP_LeftWrist;
       Vector3D fkP_RightWrist;
       Vector3D fkP_LeftAnkle;
       Vector3D fkP_RightAnkle;
       Vector3D fkP_Head;

       Vector3D fkResult_Neck;
       Vector3D fkResult_LeftWrist;
       Vector3D fkResult_RightWrist;

       Vector3D fusedP_Neck;
       Vector3D fusedP_LeftWrist;
       Vector3D fusedP_RightWrist;
       Vector3D fusedP_LeftAnkle;
       Vector3D fusedP_RightAnkle;
       Vector3D fusedP_Head;

       Quaternion Q_Neck;
       Quaternion Q_LeftWrist;
       Quaternion Q_RightWrist;
       Quaternion Q_LeftAnkle;
       Quaternion Q_RightAnkle;
       Quaternion Q_Head;
       Quaternion Q_LeftElbow;
       Quaternion Q_RightElbow;

       int NACnt_Neck;
       int NACnt_LeftWrist;
       int NACnt_RightWrist;
       int NACnt_LeftAnkle;
       int NACnt_RightAnkle;

       bool isMissing_Neck;
       bool isMissing_LeftWrist;
       bool isMissing_RightWrist;
       bool isMissing_LeftAnkle;
       bool isMissing_RightAnkle;
   };
    struct Output {
        Vector3D P_Neck;
        Vector3D P_LeftWrist;
        Vector3D P_RightWrist;
        Vector3D P_LeftAnkle;
        Vector3D P_RightAnkle;
        Vector3D P_Head;

        Quaternion Q_Neck;
        Quaternion Q_LeftWrist;
        Quaternion Q_RightWrist;
        Quaternion Q_LeftAnkle;
        Quaternion Q_RightAnkle;
        Quaternion Q_Head;
    };

    struct SkeletonModel {
        float LenHipToChest ;
        float LenHipToThigh ;
        float LenThighToKnee ;
        float LenKneeToAnkle ;
        float LenChestToNeck ;
        float LenNeckToShoulder ;
        float LenShoulderToElbow ;
        float LenElbowToWrist ;
        float LenNeckToHead ;
    };

    struct JointOffset {
        Vector3D offset_LeftWrist ;
        Vector3D offset_RightWrist ;
        Vector3D offset_LeftAnkle ;
        Vector3D offset_RightAnkle ;
        Vector3D offset_Head ;
        Vector3D offset_Neck ;
    };

    struct IKResult{
        EulerAngle HipOri ;
        EulerAngle ChestOri ;
        EulerAngle LeftShoulderOri ;
        EulerAngle LeftUpperArmOri ;
        EulerAngle LeftLowerArmOri ;
        EulerAngle LeftHandOri ;
        EulerAngle RightShoulderOri ;
        EulerAngle RightUpperArmOri ;
        EulerAngle RightLowerArmOri ;
        EulerAngle RightHandOri ;
        EulerAngle HeadOri ;
        EulerAngle LeftThighRootOri ;
        EulerAngle LeftThighOri ;
        EulerAngle LeftShankOri ;
        EulerAngle LeftFootOri ;
        EulerAngle RightThighRootOri ;
        EulerAngle RightThighOri ;
        EulerAngle RightShankOri ;
        EulerAngle RightFootOri ;


        Vector3D HipPos ;
        Vector3D NeckPos ;
        Vector3D HeadPos ;
        Vector3D LeftWristPos ;
        Vector3D RightWristPos ;
        Vector3D LeftAnklePos ;
        Vector3D RightAnklePos ;
    };

public:
    InputAdaptor() ;
    ~InputAdaptor(){}
    //bool input ( const Input& data ) ;
    const Output& output () const { return LatestOutput ; }
    void resetOutput () ;

   // IKResult ikResult;
    void process(int id, HumanIK& ik, POT** pot, int Mode, int IKSrc);
    void fuseAuxData(Vector3D& P){ }


//    void setupFK();
    void setFKModel(const HumanIK::SkeletonModel &model ) {
        HumanFK::SkeletonModel m;
        m.LenChestToNeck = model.LenChestToNeck;
        m.LenElbowToWrist = model.LenElbowToWrist;
        m.LenHipToChest = model.LenHipToChest;
        m.LenHipToThigh = model.LenHipToThigh;
        m.LenKneeToAnkle = model.LenKneeToAnkle;
        m.LenNeckToHead = model.LenNeckToHead;
        m.LenNeckToShoulder = model.LenNeckToShoulder;
        m.LenShoulderToElbow = model.LenShoulderToElbow;
        m.LenThighToKnee = model.LenThighToKnee;

        humanFK.setModel(m);
    }

    void setFKOffset(const HumanIK::JointOffset &offset ){
        HumanFK::JointOffset o;
        o.offset_Head = offset.offset_Head;
        o.offset_LeftAnkle = offset.offset_LeftAnkle;
        o.offset_LeftWrist = offset.offset_LeftWrist;
        o.offset_Neck = offset.offset_Neck;
        o.offset_RightAnkle = offset.offset_RightAnkle;
        o.offset_RightWrist = offset.offset_RightWrist;
        humanFK.setJointOffset(o);
    }
    void setFKQinit(const HumanFK::InitQuat& Q4FK){
        humanFK.setQinit(Q4FK);
    }
    void setFKPinit(const HumanFK::InitPos& P4FK){
        humanFK.setPinit(P4FK);
    }
    void initResultBuffer(const HumanFK::InitPos& P4FK){
        resultBuffer.LatestPNeck = resultBuffer.P_Neck = resultBuffer.fkP_Neck = resultBuffer.fusedP_Neck = P4FK.Pinit_Neck;
        resultBuffer.P_LeftWrist  = resultBuffer.fkP_LeftWrist  = resultBuffer.fusedP_LeftWrist  = P4FK.Pinit_LeftWrist;
        resultBuffer.P_RightWrist = resultBuffer.fkP_RightWrist = resultBuffer.fusedP_RightWrist = P4FK.Pinit_RightWrist;
        resultBuffer.P_LeftAnkle  = resultBuffer.fkP_LeftAnkle  = resultBuffer.fusedP_LeftAnkle  = P4FK.Pinit_LeftAnkle;
        resultBuffer.P_RightAnkle = resultBuffer.fkP_RightAnkle = resultBuffer.fusedP_RightAnkle = P4FK.Pinit_RightAnkle;

        resultBuffer.Q_LeftWrist = humanFK.Qinit.Qinit_LeftWrist;
        resultBuffer.Q_RightWrist = humanFK.Qinit.Qinit_RightWrist;
        resultBuffer.Q_LeftAnkle = humanFK.Qinit.Qinit_LeftAnkle;
        resultBuffer.Q_RightAnkle = humanFK.Qinit.Qinit_RightAnkle;
        resultBuffer.Q_LeftElbow = humanFK.Qinit.Qinit_LeftElbow;
        resultBuffer.Q_RightElbow = humanFK.Qinit.Qinit_RightElbow;

}

    void setFKCalibrateFlag(bool flag){
         humanFK.setCalibrate(flag);
    }
    void setTposeP(const Vector3D NeckPos){
        resultBuffer.LatestPNeck = NeckPos;
    }

private:
   // Vector3D PNeck;
   // Quaternion QNeck;
    SkeletonModel Model ;
    JointOffset Offset;
    ResultBuffer resultBuffer;
    Output LatestOutput;
    bool LatestAuxFlag ;
    HumanFK humanFK;
    // HumanIK humanIK;

    void setResultBufferQ(int id, Quaternion Q);
    void setResultBufferP(int id, Vector3D P, bool LatestAuxFlag);

    void GetPosition(bool LHValid, int id, POT** pot,  HumanIK& ik);

    void computeIK(int id, HumanIK& ik, int Mode, int InputSrc);

    void CheckMissingLH(int id, bool LHValid);
               /*    void CheckMissingLH(int id, const uint32_t prev, const uint32_t cur);*/


private:
    bool EnableAux ;
    PNeck pNeck;
    //DebugLeftArm debugLeftArm;
};

#endif // __InverseKinematics__INPUTADAPTOR_H
