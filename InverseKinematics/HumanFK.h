#ifndef __InverseKinematics__HumanFK__
#define __InverseKinematics__HumanFK__


//#ifndef HUMANFK_H
//#define HUMANFK_H
#include "InverseKinematics/Joint.h"
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"

class HumanFK
{
    friend class IKMonitor;
    friend class InputAdaptor;
    /**  public types **/
public:
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

    struct InitQuat {
        Quaternion Qinit_LeftWrist ;
        Quaternion Qinit_RightWrist ;
        Quaternion Qinit_LeftAnkle ;
        Quaternion Qinit_RightAnkle ;
        Quaternion Qinit_Head ;
        Quaternion Qinit_Neck ;

        Quaternion Qinit_LeftElbow ;
        Quaternion Qinit_RightElbow ;
    };
    struct InitPos {
        Vector3D Pinit_LeftWrist ;
        Vector3D Pinit_RightWrist ;
        Vector3D Pinit_LeftAnkle ;
        Vector3D Pinit_RightAnkle ;
        Vector3D Pinit_Head ;
        Vector3D Pinit_Neck ;

        Vector3D Pinit_LeftElbow ;
        Vector3D Pinit_RightElbow ;
    };

    struct JointOffset {
        Vector3D offset_LeftWrist ;
        Vector3D offset_RightWrist ;
        Vector3D offset_LeftAnkle ;
        Vector3D offset_RightAnkle ;
        Vector3D offset_Head ;
        Vector3D offset_Neck ;
    };

    struct Body{
        Vector3D P_LeftWrist ;
        Vector3D P_LeftElbow ;
        Vector3D P_LeftShoulder ;

        Vector3D P_Head ;
        Vector3D P_Neck ;

        Vector3D P_RightShoulder;
        Vector3D P_RightElbow ;
        Vector3D P_RightWrist ;

        Vector3D P_Hip;
        Vector3D P_LeftPelvis;
        Vector3D P_LeftKnee ;
        Vector3D P_LeftAnkle ;
        Vector3D P_RightPelvis ;
        Vector3D P_RightKnee ;
        Vector3D P_RightAnkle ;
    };

    enum JointID {
        Joint_LeftWrist ,
        Joint_RightWrist ,
        Joint_LeftElbow ,
        Joint_RightElbow ,
        Joint_Neck,
        Joint_Head ,
        Joint_LeftAnkle ,
        Joint_RightAnkle ,
    };

    struct RawDataFrame {
        JointID id ;
        Quaternion Q;
        Vector3D P;
    };
    struct InputFrame{
        JointID id ;
        Quaternion Qthis ;
        Quaternion Qlinker ;
        Quaternion Qroot ;
        Vector3D Proot;
    };

    struct OutputFrame {
        Vector3D LeftWristPos ;
        Vector3D RightWristPos ;
        Vector3D NeckPos ;
        Vector3D HeadPos ;
        Vector3D LeftAnklePos ;
        Vector3D RightAnklePos ;
        /*
        Vector3D LeftElbowPos;
        Vector3D LeftShoulderPos;
        Vector3D RightShoulderPos;
        Vector3D RightElbowPos;
        Vector3D LeftThighPos;
        Vector3D LeftKneePos;
        Vector3D HipPos;
        Vector3D RightThighPos;
        Vector3D RightKneePos;

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
        */
    };

    struct Chain{
        int id;
        float LenB0;
        float LenB1;
        float LenB2;
        Vector3D B0;
        Vector3D B1;
        Vector3D B2;
    };

    /** public interfaces **/
public:
    HumanFK();

    ~ HumanFK(){}

    void setModel(const SkeletonModel& m);

    void setCalibrate(bool flag){
        isTposeCalibrated = flag;
    }

    bool isFKCalibrated(){
        return isTposeCalibrated;
    }

    void setQinit(const InitQuat &Q){
        Qinit = Q ;
        isTposeCalibrated= true;
    }

    void setPinit(const InitPos &P){
        Pinit = P ;
    }

    Quaternion getQinit(JointID id);

    void setJointOffset(JointOffset& offset){Offsets = offset;}

    Vector3D getOffset(JointID id);

    void compute (const InputFrame& Din);  //void computeForDebug (const InputFrame& Din, Vector3D& P0,Vector3D& P1,Vector3D& P2 ,Vector3D& P3 );

    const OutputFrame& output () const { return LatestOutput ; }

 bool updateFKResult(int id, Vector3D P);
    /**  private methods & properties**/
private:
    SkeletonModel Model ;
    InitQuat Qinit;
    InitPos  Pinit;
    JointOffset Offsets;
    Chain Chains[5];
    OutputFrame LatestOutput;
    RawDataFrame LatestRawDataFrame[10];
    InputFrame LatestInput[6];
    bool isTposeCalibrated;
    void resetOutput () ;
    void setDefaultModel() ;
    void setDefaultOffset();
    void updateChains();


    int getJointRawDataIdx (JointID id) const {
        switch (id) {
        case Joint_LeftWrist :  return 0 ;
        case Joint_RightWrist :  return 1 ;
        case Joint_LeftAnkle :  return 2 ;
        case Joint_RightAnkle :  return 3 ;
        case Joint_Neck :  return 4 ;
        //case Joint_Head :  return 5 ;
        default: return -1 ;
        }
    }
};

#endif // HUMANFK_H








