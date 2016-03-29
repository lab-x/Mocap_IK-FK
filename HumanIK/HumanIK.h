#ifndef __HumanIK__HumanIK__
#define __HumanIK__HumanIK__


#include "InverseKinematics/fabrik.h"

//#define WholeBody
#define BothArm

class HumanIK {
    friend class IKMonitor;
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
    };
    struct InitPos {
        Vector3D Pinit_LeftWrist ;
        Vector3D Pinit_RightWrist ;
        Vector3D Pinit_LeftAnkle ;
        Vector3D Pinit_RightAnkle ;
        Vector3D Pinit_Head ;
        Vector3D Pinit_Neck ;
    };

    struct JointOffset {
        Vector3D offset_LeftWrist ;
        Vector3D offset_RightWrist ;
        Vector3D offset_LeftAnkle ;
        Vector3D offset_RightAnkle ;
        Vector3D offset_Head ;
        Vector3D offset_Neck ;
    };


    enum EndJointID {
        Joint_LeftWrist ,
        Joint_RightWrist ,
        Joint_LeftAnkle ,
        Joint_RightAnkle ,
        Joint_Neck ,
        Joint_Head
    };

    struct RawDataFrame {
        Quaternion Q ;
        Vector3D Pos ;
        EndJointID id ;
    };

    struct OutputFrame {
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

        Quaternion HipQuat;
        Quaternion ChestQuat ;
        Quaternion LeftShoulderQuat ;
        Quaternion LeftUpperArmQuat ;
        Quaternion LeftLowerArmQuat ;
        Quaternion LeftHandQuat ;
        Quaternion RightShoulderQuat ;
        Quaternion RightUpperArmQuat ;
        Quaternion RightLowerArmQuat ;
        Quaternion RightHandQuat ;
        Quaternion HeadQuat ;
        Quaternion LeftThighRootQuat ;
        Quaternion LeftThighQuat ;
        Quaternion LeftShankQuat ;
        Quaternion LeftFootQuat ;
        Quaternion RightThighRootQuat ;
        Quaternion RightThighQuat ;
        Quaternion RightShankQuat ;
        Quaternion RightFootQuat ;
        Vector3D HipPos ;
    };

    enum ChainMask {
        CALC_LEFT_ARM = (0x1 << 0),
        CALC_RIGHT_ARM = (0x1 << 1),
        CALC_HEAD = (0x1 << 2),
        CALC_LEGS = (0x1 << 3),
        CALC_HIP = (0x1 << 4),

        CALC_WHOLE_BODY = (CALC_LEFT_ARM | CALC_RIGHT_ARM | CALC_HEAD | CALC_LEGS),
        CALC_Execpt_HEAD = (~CALC_HEAD)&(CALC_WHOLE_BODY)
    } ;

    /** public interfaces **/
public:
    HumanIK () ;
    ~HumanIK(){}

    void setQinit(const InitQuat &Q){
        Qinit = Q ;
        isTposeCalibrated= true;
    }
    void setPinit(const InitPos &P){
        Pinit = P ;
    }
    void setCalibrate(bool flag){
        isTposeCalibrated = flag;
    }

    void setModel(const SkeletonModel &m, float modelSizeRatio=1,
                  Vector3D initOffset=Vector3D(0,0,0)) ;

    void ModelAdjustment(int Mode);
    const SkeletonModel& getModel() const {return Model;}
    const JointOffset& getTOffset() const {return TOffset;}

    bool input ( const RawDataFrame& rawData ) ;
    void compute ( int chainMask=CALC_Execpt_HEAD ) ;
    const OutputFrame& output () const { return LatestOutput ; }
    void resetOutput () ;

//    void setOutput_LeftArmOri(Float Y, Float X, Float Z){
//        LatestOutput.LeftUpperArmOri = EulerAngle(Y,X,Z);
//    }

    /** public static interfaces **/
public :
    static SkeletonModel& setDefaultModel (SkeletonModel &m) ;

    /** interfaces for developers **/
public :
    Fabrik* getFabrik() {return IKChains;}

    /**  private methods **/
private:
    void setupIK ( float DistanceRatio = 1 , Vector3D initOffset = Vector3D(0,0,0) ) ;
    void setIKGoal ( Fabrik*  pFabrik, Vector3D inpGoal, Quaternion* pQ ) ;
    void updateIKResult (int chainMask);

    int getJointRawDataIdx (EndJointID id) const {
        switch (id) {
        case Joint_LeftWrist :  return 0 ;
        case Joint_RightWrist :  return 1 ;
        case Joint_LeftAnkle :  return 2 ;
        case Joint_RightAnkle :  return 3 ;
        case Joint_Neck :  return 4 ;
        case Joint_Head :  return 5 ;
        default: return -1 ;
        }
    }
    const Vector3D& getNeckPosRaw() const {
        return LatestJonitsRawData [getJointRawDataIdx(Joint_Neck)].Pos ;
    }
    void setDefaultOffset();

    /**  private properties **/
private:

    Quaternion HipYaw;
    SkeletonModel Model ;
    InitQuat Qinit;
    InitPos Pinit;
    bool isTposeCalibrated;
    //Vector3D TOffset[6];
    JointOffset TOffset;

    Fabrik IKChains[6] ;
    // chain 0 : left arm
    // chain 1 : right arm
    // chain 2 : left leg
    // chain 3 : right leg
    // chain 4 : head to neck
    // chain 5 : hip to neck

    // LatestRawData[id] is the latest rawdata of the Joint whose Joint_ID is 'id' .
    RawDataFrame LatestJonitsRawData[6] ;
    OutputFrame LatestOutput;

#ifdef BothArm
        Quaternion BodyYaw ;
#endif
};

#endif
