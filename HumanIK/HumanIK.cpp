
#include "HumanIK/HumanIK.h"
#include <QDebug>


HumanIK::HumanIK ()
{
    resetOutput () ;
    setModel ( setDefaultModel(Model) ) ;
    setDefaultOffset();
    isTposeCalibrated = false;
}

void HumanIK::resetOutput () {
    OutputFrame* p = &LatestOutput ;
    p->HipOri = p->ChestOri = p->LeftShoulderOri = p->LeftUpperArmOri =
            p->LeftLowerArmOri = p->LeftHandOri = p->RightShoulderOri =
            p->RightUpperArmOri = p->RightLowerArmOri = p->RightHandOri =
            p->HeadOri = p->LeftThighRootOri = p->LeftThighOri =
            p->LeftShankOri = p->LeftFootOri = p->RightThighRootOri =
            p->RightThighOri = p->RightShankOri = p->RightFootOri = EulerAngle(0,0,0) ;
    p->HipPos = Vector3D (0,0,0) ;
}

void HumanIK::setupIK ( float DistanceRatio, Vector3D initOffset )
{
    float FabrikTolerance = 0.001 ;
    float FabrikEps = 0.001 ;

    float y = (Model.LenHipToChest + Model.LenChestToNeck)*DistanceRatio ;
    float x = (Model.LenHipToThigh)*DistanceRatio ;
    //LeftArm:          Fabrik ChainID.1
    do {
        IKChains[0] = Fabrik(1, FabrikTolerance,FabrikEps);
        Position p0 (0,y,0);
        Position p1 ((Model.LenNeckToShoulder)*DistanceRatio,y,0);
        Position p2 ((Model.LenNeckToShoulder+Model.LenShoulderToElbow)*DistanceRatio, y, 0);
        Position p3 ((Model.LenNeckToShoulder+Model.LenShoulderToElbow+Model.LenElbowToWrist)*DistanceRatio, y, 0);
        Joint P0(p0+initOffset);
        Joint P1(p1+initOffset);
        Joint P2(p2+initOffset);
        Joint P3(p3+initOffset);
        IKChains[0].setJoints(P0,P1,P2,P3);
        IKChains[0].setInitJoints(P0,P1,P2,P3);
    } while (0) ;
    //RightArm:         Fabrik ChainID.2
    do {
        IKChains[1] = Fabrik(2, FabrikTolerance,FabrikEps);
        Position p0 (0,y,0);
        Position p1 (-(Model.LenNeckToShoulder)*DistanceRatio,y,0);
        Position p2 (-(Model.LenNeckToShoulder+Model.LenShoulderToElbow)*DistanceRatio, y, 0);
        Position p3 (-(Model.LenNeckToShoulder+Model.LenShoulderToElbow+Model.LenElbowToWrist)*DistanceRatio, y, 0);
        Joint P0(p0+initOffset);
        Joint P1(p1+initOffset);
        Joint P2(p2+initOffset);
        Joint P3(p3+initOffset);
        IKChains[1].setJoints(P0,P1,P2,P3);
        IKChains[1].setInitJoints(P0,P1,P2,P3);
    } while (0) ;
    //LeftLeg:          Fabrik ChainID.3
    do {
        IKChains[2] = Fabrik(3, FabrikTolerance,FabrikEps);
        Position p0 (0,0,0);
        Position p1 (x,0,0);
        Position p2 (x,-(Model.LenThighToKnee)*DistanceRatio, 0);
        Position p3 (x,-(Model.LenThighToKnee+Model.LenKneeToAnkle)*DistanceRatio, 0);
        Joint P0(p0+initOffset);
        Joint P1(p1+initOffset);
        Joint P2(p2+initOffset);
        Joint P3(p3+initOffset);
        IKChains[2].setJoints(P0,P1,P2,P3);
        IKChains[2].setInitJoints(P0,P1,P2,P3);
    } while (0) ;
    //RightLeg:         Fabrik ChainID.4
    do {
        //
        IKChains[3] = Fabrik(4, FabrikTolerance,FabrikEps);
        Position p0 (0,0,0);
        Position p1 (-x,0,0);
        Position p2 (-x,-(Model.LenThighToKnee)*DistanceRatio, 0);
        Position p3 (-x,-(Model.LenThighToKnee+Model.LenKneeToAnkle)*DistanceRatio, 0);
        Joint P0(p0+initOffset);
        Joint P1(p1+initOffset);
        Joint P2(p2+initOffset);
        Joint P3(p3+initOffset);
        IKChains[3].setJoints(P0,P1,P2,P3);
        IKChains[3].setInitJoints(P0,P1,P2,P3);
    } while (0) ;
    //Neck Head
    do {
        IKChains[4] = Fabrik(0, FabrikTolerance,FabrikEps);
        Position p0 (0,y,0);
        Position p1 (0,y+(Model.LenNeckToHead)*DistanceRatio,0);
        Joint P0(p0+initOffset);
        Joint P1(p1+initOffset);
        IKChains[4].setJoints(P0,P1);
        IKChains[4].setInitJoints(P0,P1);
    } while (0) ;
    // Neck Chest Hip
    do {
        IKChains[5] = Fabrik(5, FabrikTolerance,FabrikEps);
        Position p0 (0,y,0);
        Position p1 (0,(Model.LenHipToChest)*DistanceRatio,0);
        Position p2 (0,0,0);
        Joint P0(p0+initOffset);
        Joint P1(p1+initOffset);
        Joint P2(p2+initOffset);
        IKChains[5].setJoints(P0,P1,P2);
        IKChains[5].setInitJoints(P0,P1,P2);
        IKChains[5].addChild(IKChains+2);
        IKChains[5].addChild(IKChains+3);
    } while (0) ;
}

HumanIK::SkeletonModel& HumanIK::setDefaultModel(SkeletonModel &m)
{
    m.LenHipToChest = 0.20f ;       //0.20f;
    m.LenHipToThigh = 0.12f ;       //0.12f;
    m.LenThighToKnee = 0.42f ;      //0.40f;
    m.LenKneeToAnkle = 0.42f ;      //0.40f;
    m.LenChestToNeck = 0.20f ;      //0.20f;
    m.LenNeckToShoulder = 0.20f ;   //0.20f;
    m.LenShoulderToElbow = 0.27f ;  //0.25f;
    m.LenElbowToWrist = 0.20f ;     //0.22f;
    m.LenNeckToHead = 0.30f ;       //0.30f;
    return m ;
}
void HumanIK::setDefaultOffset()
{
    TOffset.offset_Head = Vector3D(0,0,0);
    TOffset.offset_LeftAnkle = Vector3D(0,0,0);
    TOffset.offset_LeftWrist = Vector3D(0,0,0);
    TOffset.offset_Neck = Vector3D(0,0,0);
    TOffset.offset_RightWrist = Vector3D(0,0,0);
    TOffset.offset_RightAnkle = Vector3D(0,0,0);
}
void HumanIK::setModel(const SkeletonModel &m, float modelSizeRatio,Vector3D initOffset )
{
    Model = m ;
    setupIK ( modelSizeRatio, initOffset ) ;
}

bool HumanIK::input ( const RawDataFrame& rawData )
{
    Quaternion QT(1,0,0,0);
    int idx = getJointRawDataIdx(rawData.id) ;
    if(isTposeCalibrated)
    {
        switch (idx) {
        case Joint_LeftWrist :
            QT = Qinit.Qinit_LeftWrist;
            break;
        case Joint_RightWrist :
            QT = Qinit.Qinit_RightWrist;
            break;
        case Joint_LeftAnkle :
            QT = Qinit.Qinit_LeftAnkle;
            break;
        case Joint_RightAnkle :
            QT = Qinit.Qinit_RightAnkle;
            break;
        case Joint_Head :
            QT = Qinit.Qinit_Head;
            break;
        case Joint_Neck :
            QT = Qinit.Qinit_Neck;
            break;
        default:
            return false ;
        }
    }
    LatestJonitsRawData[idx] = rawData ;
    LatestJonitsRawData[idx].Q = rawData.Q * QT.conj();
    return true ;
}

static bool isZeroQuat(const Quaternion& q)
{
    if ( q.getW() == 0 &&
         q.getX() == 0 &&
         q.getY() == 0 &&
         q.getZ() == 0 )
        return true ;
    else
        return false ;
}

// set LatestOutput.HipPos According to new model
void HumanIK::ModelAdjustment(int Mode)
{//--1:Whole body --2: UpperBody
    Vector3D mNeck = LatestJonitsRawData [ getJointRawDataIdx(Joint_Neck) ].Pos ;
    Vector3D mLWrist = LatestJonitsRawData [ getJointRawDataIdx(Joint_LeftWrist) ].Pos ;
    Vector3D mRWrist = LatestJonitsRawData [ getJointRawDataIdx(Joint_RightWrist) ].Pos ;
    Vector3D mLAnkle = LatestJonitsRawData [ getJointRawDataIdx(Joint_LeftAnkle) ].Pos ;
    Vector3D mRAnkle = LatestJonitsRawData [ getJointRawDataIdx(Joint_RightAnkle) ].Pos ;
    Vector3D dNeck, dLWrist, dRWrist, dLAnkle, dRAnkle;

    if(Mode == 1)
    {
        dNeck = Vector3D(0.5 * (mLWrist.getX() + mRWrist.getX()), 0.5 * (mLWrist.getY() + mRWrist.getY()), 0.5 * (mLWrist.getZ() + mRWrist.getZ()));
        Float LArmLen = sqrtf((mLWrist.getX()-dNeck.getX())*(mLWrist.getX()-dNeck.getX()) + (mLWrist.getY()-dNeck.getY())*(mLWrist.getY()-dNeck.getY()) + (mLWrist.getZ()-dNeck.getZ())*(mLWrist.getZ()-dNeck.getZ()));
        Float RArmLen = sqrtf((mRWrist.getX()-dNeck.getX())*(mRWrist.getX()-dNeck.getX()) + (mRWrist.getY()-dNeck.getY())*(mRWrist.getY()-dNeck.getY()) + (mRWrist.getZ()-dNeck.getZ())*(mRWrist.getZ()-dNeck.getZ()));
        Float ArmLen = 0.5* (LArmLen + RArmLen);
        dLWrist = Vector3D(dNeck.getX() + ArmLen, dNeck.getY(), dNeck.getZ());
        dRWrist = Vector3D(dNeck.getX() - ArmLen, dNeck.getY(), dNeck.getZ());

        Float feetCenter = 0.5 * (mLAnkle.getX() + mRAnkle.getX()) ;
        Float CenterOffset = dNeck.getX() - feetCenter;
        Float AnkleHeight = 0.07 ;
        dLAnkle = Vector3D(mLAnkle.getX() + CenterOffset, AnkleHeight, dNeck.getZ());
        dRAnkle = Vector3D(mRAnkle.getX() + CenterOffset, AnkleHeight, dNeck.getZ());

        TOffset.offset_LeftWrist = dLWrist - mLWrist;//Vector3D(0,0,0);//dLWrist - mLWrist;
        TOffset.offset_RightWrist = dRWrist - mRWrist;//Vector3D(0,0,0);//dRWrist - mRWrist;
        TOffset.offset_LeftAnkle = dLAnkle - mLAnkle;
        TOffset.offset_RightAnkle = dRAnkle - mRAnkle;
        TOffset.offset_Neck = dNeck - mNeck;

        Float ModelArmLen = Model.LenNeckToShoulder  + Model.LenShoulderToElbow + Model.LenElbowToWrist ;
        Float ratio = ArmLen / ModelArmLen ;
        Model.LenNeckToShoulder *= ratio ;
        Model.LenShoulderToElbow *= ratio ;
        Model.LenElbowToWrist *= ratio ;

        Float ModelHeight = Model.LenChestToNeck + Model.LenHipToChest + Model.LenThighToKnee + Model.LenKneeToAnkle; //+ AnkleHeight ;
        ratio = dNeck.getY() / ModelHeight ;
        Model.LenChestToNeck *= ratio ;
        Model.LenHipToChest *= ratio ;
        Model.LenThighToKnee *= ratio ;
        Model.LenKneeToAnkle *= ratio ;
        Model.LenHipToThigh = 0.5 * (dLAnkle.getX() - dRAnkle.getX()) ;
        LatestOutput.HipPos = dNeck;
        LatestOutput.HipPos.setY(dNeck.getY() - Model.LenChestToNeck - Model.LenHipToChest);
    }
    else if(Mode == 2)
    {
        dNeck = Vector3D(0.5 * (mLWrist.getX() + mRWrist.getX()), 0.5 * (mLWrist.getY() + mRWrist.getY()), 0.5 * (mLWrist.getZ() + mRWrist.getZ()));
        Float LArmLen = sqrtf((mLWrist.getX()-dNeck.getX())*(mLWrist.getX()-dNeck.getX()) + (mLWrist.getY()-dNeck.getY())*(mLWrist.getY()-dNeck.getY()) + (mLWrist.getZ()-dNeck.getZ())*(mLWrist.getZ()-dNeck.getZ()));
        Float RArmLen = sqrtf((mRWrist.getX()-dNeck.getX())*(mRWrist.getX()-dNeck.getX()) + (mRWrist.getY()-dNeck.getY())*(mRWrist.getY()-dNeck.getY()) + (mRWrist.getZ()-dNeck.getZ())*(mRWrist.getZ()-dNeck.getZ()));
        Float ArmLen = 0.5* (LArmLen + RArmLen);
        dLWrist = Vector3D(dNeck.getX() + ArmLen, dNeck.getY(), dNeck.getZ());
        dRWrist = Vector3D(dNeck.getX() - ArmLen, dNeck.getY(), dNeck.getZ());
        TOffset.offset_Neck = dNeck - mNeck;
        TOffset.offset_LeftWrist = dLWrist - mLWrist;
        TOffset.offset_RightWrist = dRWrist - mRWrist;

        Float ratio = ArmLen / (Model.LenNeckToShoulder + Model.LenShoulderToElbow + Model.LenElbowToWrist);
        Model.LenNeckToShoulder *= ratio;
        Model.LenShoulderToElbow *= ratio;
        Model.LenElbowToWrist *= ratio;

        ratio = dNeck.getY() / (Model.LenChestToNeck + Model.LenHipToChest + Model.LenThighToKnee + Model.LenKneeToAnkle);
        Model.LenChestToNeck *= ratio;
        Model.LenHipToChest  *= ratio;
        Model.LenHipToThigh =Model.LenNeckToShoulder * 0.6;
        Model.LenThighToKnee  *= ratio;
        Model.LenKneeToAnkle *= ratio;

    }
    setModel(Model);
}

void HumanIK::compute ( int chainMask )
{
    OutputFrame& output = LatestOutput ;

    LockOrientationConstraint::ConstraintParam LockOri1, LockOri2;
    LockQuaternionConstraint::Param LockedQ ;
    MaxAngleConstraint::Param MaxAngle ;
    UpperArmConstraint::Param UpperArmParam;
    LegConstraint::Param LegParam1, LegParam2;
    Fabrik* ikChain ;
    Quaternion q , *pQ;
    Vector3D pos ;
    EndJointID id ;
    Vector3D rootPos = LatestJonitsRawData [ getJointRawDataIdx(Joint_Neck) ].Pos ;

    q = LatestJonitsRawData [ getJointRawDataIdx(Joint_Neck) ].Q ;
    LockedQ.q = q ;
    Vector3D offset = Quaternion::rotVbyQ(TOffset.offset_Neck, q);
    rootPos  = rootPos + offset;

const Float testMaxAngle = 0 ;
    UpperArmParam.direction = IKConstraint::BWD;
    UpperArmParam.q = LatestJonitsRawData [ getJointRawDataIdx(Joint_LeftWrist) ].Q ;

MaxAngle.maxAngle = testMaxAngle ;
    IKChains[0].getJoints()[0].setPosition(rootPos);
    IKChains[0].getJoints()[0].setQwFWD(q);
    IKChains[0].getJoints()[0].addConstraint(Joint::MaxAngle,&MaxAngle);
    IKChains[0].getJoints()[1].addConstraint(Joint::UpperArm, &UpperArmParam);

    MaxAngle.maxAngle = testMaxAngle ;
    IKChains[1].getJoints()[0].setPosition(rootPos);
    IKChains[1].getJoints()[0].setQwFWD(q);
    IKChains[1].getJoints()[0].addConstraint(Joint::MaxAngle,&MaxAngle);
    IKChains[1].getJoints()[1].addConstraint(Joint::LockOrientation, NULL);

    MaxAngle.maxAngle = 0 ;
    IKChains[2].getJoints()[0].addConstraint(Joint::MaxAngle,&MaxAngle);
    IKChains[2].getJoints()[2].addConstraint(Joint::LockOrientation, &LockOri1);
    //    IKChains[2].getJoints()[1].addConstraint(Joint::Leg, &LegParam1);

    MaxAngle.maxAngle = 0 ;
    IKChains[3].getJoints()[0].addConstraint(Joint::MaxAngle,&MaxAngle);
    IKChains[3].getJoints()[2].addConstraint(Joint::LockOrientation, &LockOri2);
    //    IKChains[3].getJoints()[1].addConstraint(Joint::Leg, &LegParam2);

    MaxAngle.maxAngle = 60 ;
    IKChains[4].getJoints()[0].setPosition(rootPos);
    IKChains[4].getJoints()[0].setQwFWD(q);
    IKChains[4].getJoints()[0].addConstraint(Joint::MaxAngle,&MaxAngle);

    MaxAngle.maxAngle = 0 ;
    IKChains[5].getJoints()[0].setPosition(rootPos);
    IKChains[5].getJoints()[0].addConstraint(Joint::LockQuaternion,&LockedQ);
    IKChains[5].getJoints()[1].addConstraint(Joint::MaxAngle,&MaxAngle);

    if ( chainMask & CALC_LEFT_ARM  )
    {
        ikChain = IKChains+0 ;
        id = Joint_LeftWrist ;
        q = LatestJonitsRawData [ getJointRawDataIdx(id) ].Q ;
        pos = LatestJonitsRawData [ getJointRawDataIdx(id) ].Pos ;
        pQ = isZeroQuat(q) ? NULL : &q ;
        if(pQ != NULL )
        {
            offset = Quaternion::rotVbyQ(TOffset.offset_LeftWrist, q);
            pos  = pos + offset;
        }
        setIKGoal ( ikChain, pos, pQ );
        ikChain->getTopParent()->computeNEW();
    }

    if ( chainMask & CALC_RIGHT_ARM )
    {
        id = Joint_RightWrist ;
        ikChain = IKChains+1 ;
        q = LatestJonitsRawData [ getJointRawDataIdx(id) ].Q ;
        pos = LatestJonitsRawData [ getJointRawDataIdx(id) ].Pos ;
        pQ = isZeroQuat(q) ? NULL : &q ;
        if(pQ != NULL )
        {
            //offset = Quaternion::rotVbyQ(TOffset[1], q);
            offset = Quaternion::rotVbyQ(TOffset.offset_RightWrist, q);
            pos  = pos + offset;
        }
        setIKGoal ( ikChain, pos, pQ );
        ikChain->getTopParent()->computeNEW();
    }
//CalcHip Only With RootNeck, NoTargetHip
    if ( chainMask & CALC_HIP )
    {
        id = Joint_Neck ;
        Vector3D PNeck, PChest, PHip;
        Quaternion QWChest, QWHip;
        Vector3D BNeck2Chest(0, -1, 0);
        Vector3D BChest2Hip(0, -1, 0);
        QWChest = LatestJonitsRawData [ getJointRawDataIdx(id) ].Q ;
        pQ = isZeroQuat(QWChest) ? NULL : &q ;
        if(pQ == NULL )
        {
            QWChest = Quaternion(1,0,0,0);
        }
        QWHip = Quaternion::slerp(Quaternion(1,0,0,0), QWChest, 0.8);
        Vector3D b_hip(1,0,0);
        Vector3D b_hip_new = Quaternion::rotVbyQ(b_hip, QWHip);
        if(b_hip_new.getZ()!=0)
        {
            Vector3D b_hip_new_noPitch(b_hip_new.getX(), 0, b_hip_new.getZ());
            Quaternion QT = Quaternion::v2q(b_hip_new, b_hip_new_noPitch);
            QWHip = QT * QWHip;
#ifdef BothArm
        BodyYaw = Quaternion::v2q(b_hip, b_hip_new_noPitch);
#endif

        }
        PNeck = rootPos;
        Float LenNeckChest= Model.LenChestToNeck;
        Float LenChest2Hip = Model.LenChestToNeck;
        PChest = PNeck + LenNeckChest * (Quaternion::rotVbyQ(BNeck2Chest, QWChest));
        PHip = PChest + LenChest2Hip * (Quaternion::rotVbyQ(BChest2Hip, QWHip));
        LatestOutput.HipPos =  PHip;
        LatestOutput.HipQuat = QWHip;
        LatestOutput.HipOri =  QWHip.toEuler().RadToDegree() ;
        LatestOutput.ChestQuat = QWHip.conj() * QWChest;
        LatestOutput.ChestOri = output.ChestQuat.toEuler().RadToDegree();//QChest.toEuler().RadToDegree();

        IKChains[2].getJoints()[0].setPosition(PHip);
        IKChains[2].getJoints()[0].setQwBWD(QWHip);
        IKChains[2].getJoints()[1].setQwFWD(QWHip);

        IKChains[3].getJoints()[0].setPosition(PHip);
        IKChains[3].getJoints()[0].setQwBWD(QWHip);
        IKChains[3].getJoints()[1].setQwFWD(QWHip);
    }

    if ( chainMask & CALC_HEAD )
    {
        ikChain = IKChains+4 ;
        id = Joint_Head ;
        q = LatestJonitsRawData [ getJointRawDataIdx(id) ].Q ;
        pos = LatestJonitsRawData [ getJointRawDataIdx(id) ].Pos ;
        pQ = isZeroQuat(q) ? NULL : &q ;
        setIKGoal ( ikChain, pos, pQ );
        ikChain->getTopParent()->computeNEW();
    }

    if ( chainMask & CALC_LEGS )
    {
        ikChain = IKChains+2 ;
        id = Joint_LeftAnkle ;
        q = LatestJonitsRawData [ getJointRawDataIdx(id) ].Q ;
        pos = LatestJonitsRawData [ getJointRawDataIdx(id) ].Pos ;
        pQ = isZeroQuat(q) ? NULL : &q ;
        if(pQ != NULL )
        {
            offset = Quaternion::rotVbyQ(TOffset.offset_LeftAnkle, q);
            pos  = pos + offset;
        }
        setIKGoal ( ikChain, pos, pQ );
        ikChain->computeNEW();
        ikChain = IKChains+3 ;
        id = Joint_RightAnkle ;
        q = LatestJonitsRawData [ getJointRawDataIdx(id) ].Q ;
        pos = LatestJonitsRawData [ getJointRawDataIdx(id) ].Pos ;
        pQ = isZeroQuat(q) ? NULL : &q ;
        if(pQ != NULL )
        {
            offset = Quaternion::rotVbyQ(TOffset.offset_RightAnkle, q);
            pos  = pos + offset;
        }
        setIKGoal ( ikChain, pos, pQ );
        ikChain->computeNEW();
        //LockOrientationConstraint* Left = (LockOrientationConstraint*)(IKChains[2].getJoints()[2].getConstraint());
        //Quaternion yaw1 = Left->getParam();
        //LockOrientationConstraint* Right = (LockOrientationConstraint*)(IKChains[3].getJoints()[2].getConstraint());
        //Quaternion yaw2 = Right->getParam();
        //HipYaw = Quaternion::Slerp(yaw1, yaw2, 0.5);
        ikChain = IKChains+5 ;
        ikChain->computeNEW();
    }
    updateIKResult (chainMask);
}


void HumanIK::updateIKResult (int chainMask)
{
    Fabrik* ik ;
    Quaternion ChestQW , HipQW, ChestQWInv, HipQWInv, QW, QL;
    EulerAngle* Eulers ;//= ik->getEulers();
    Quaternion* Quats ;
    OutputFrame& output = LatestOutput ;
    // IKChains 5 is binded to Body .
    /////////OriginCode/////OriginCode////////////////////////////////

#ifdef WholeBody
    ik = IKChains+5 ;
    Position P = ik->getJoints()[2].getPosition() ;   //PHip
    output.HipPos = P ;
    HipQW = ik->getJoints()[2].getQwFWD() ;
    output.HipQuat = HipQW;
    output.HipOri = HipQW.toEuler().RadToDegree() ;
    ChestQW = ik->getJoints()[0].getQwBWD() ;
    Quaternion ChestQL = HipQW.conj()*ChestQW ;
    output.ChestOri = ChestQL.toEuler().RadToDegree();
    output.ChestQuat = ChestQL ;
    HipQWInv = HipQW.conj();
    ChestQWInv = ChestQW.conj();
#endif
/////////TestTestTestTestTest///////Mode==1///////////////////////
/*  if(chainMask != CALC_HIP)
    {
        ik = IKChains + 0 ;
        Position P = ik->getJoints()[0].getPosition() ;
        output.HipPos = P ;
        HipQW = Quaternion::slerp(Quaternion(1,0,0,0),ik->getJoints()[0].getQwFWD(), 0.5) ;
        output.HipQuat = HipQW;

        output.HipOri = HipQW.toEuler().RadToDegree() ;
        ChestQW = ik->getJoints()[0].getQwBWD() ;   //HipQW;
        Quaternion ChestQL = HipQW.conj()*ChestQW ;
        output.ChestOri = ChestQL.toEuler().RadToDegree();
        output.ChestQuat = ChestQL ;
        HipQWInv = HipQW.conj();
        ChestQWInv = ChestQW.conj();
    }
    else
    {
      HipQW = output.HipQuat;
      ChestQW = output.HipQuat * output.ChestQuat;
      HipQWInv = HipQW.conj();
      ChestQWInv = ChestQW.conj();
    }
    */
/////////TestTestTestTestTest//////////END////////////////////
#ifdef BothArm
    HipQW = output.HipQuat;
    ChestQW = output.HipQuat * output.ChestQuat;
    HipQWInv = HipQW.conj();
    ChestQWInv = ChestQW.conj();
#endif
    // IKChains 0 is binded to Left arm .
    ik = IKChains+0 ;
    Eulers = ik->getEulers();
    Quats = ik->getQL();
    output.LeftUpperArmOri = Eulers[1] ;
    output.LeftLowerArmOri = Eulers[2] ;
    output.LeftUpperArmQuat = Quats[1] ;
    output.LeftLowerArmQuat = Quats[2] ;
    QW = ik->getJoints()[0].getQwBWD();
    QL = ChestQWInv*QW ;
    output.LeftShoulderOri = QL.toEuler().RadToDegree() ;
    output.LeftShoulderQuat = QL ;

    // IKChains 1 is binded to Right arm .
    ik = IKChains+1 ;
    Eulers = ik->getEulers();
    Quats = ik->getQL();
    output.RightUpperArmOri = Eulers[1] ;
    output.RightLowerArmOri = Eulers[2] ;
    output.RightUpperArmQuat = Quats[1] ;
    output.RightLowerArmQuat = Quats[2] ;
    QW = ik->getJoints()[0].getQwBWD();
    QL = ChestQWInv*QW ;
    output.RightShoulderOri = QL.toEuler().RadToDegree() ;
    output.RightShoulderQuat = QL;

#ifdef BothArm

    Quaternion ThighQL = HipQWInv * BodyYaw;
    output.LeftThighOri = ThighQL.toEuler().RadToDegree() ;
   // output.LeftThighOri.setRoll(output.HipOri.getRoll());
    output.LeftShankOri = Quaternion(1,0,0,0).toEuler().RadToDegree();
    output.LeftThighQuat = ThighQL;
    output.LeftShankQuat = Quaternion(1,0,0,0);
    output.LeftThighRootOri = EulerAngle(0,0,0);//output
    output.LeftThighRootQuat = Quaternion(1,0,0,0) ;

    output.RightThighOri = ThighQL.toEuler().RadToDegree() ;
 //   output.RightThighOri.setRoll(output.HipOri.getRoll());
    output.RightShankOri = Quaternion(1,0,0,0).toEuler().RadToDegree();
    output.RightThighQuat = ThighQL;
    output.RightShankQuat = Quaternion(1,0,0,0);
    output.RightThighRootOri = EulerAngle(0,0,0);//output.HipOri ;
    output.RightThighRootQuat = Quaternion(1,0,0,0) ;
#endif

#ifdef WholeBody
        // IKChains 2 is binded to Left leg .
    ik = IKChains+2 ;
    Quats = ik->getQL() ;
    Eulers = ik->getEulers();
    output.LeftThighOri = Eulers[1] ;
    output.LeftShankOri = Eulers[2] ;
    output.LeftThighQuat = Quats[1] ;
    output.LeftShankQuat = Quats[2] ;
    QW = ik->getJoints()[0].getQwBWD();
    QL = HipQWInv*QW ;
    output.LeftThighRootOri = QL.toEuler().RadToDegree() ;
    output.LeftThighRootQuat = QL ;

    // IKChains 3 is binded to Right leg .
    ik = IKChains+3 ;
    Eulers = ik->getEulers();
    Quats = ik->getQL();
    output.RightThighOri = Eulers[1] ;
    output.RightShankOri = Eulers[2] ;
    output.RightThighQuat = Quats[1] ;
    output.RightShankQuat = Quats[2] ;
    QW = ik->getJoints()[0].getQwBWD();
    QL = HipQWInv*QW ;
    output.RightThighRootOri = QL.toEuler().RadToDegree() ;
    output.RightThighRootQuat = QL ;
#endif

    /////////OriginCode/////OriginCode////////////////////////////////
    //   if ( chainMask != CALC_Execpt_HEAD )
    //   {
    //       // IKChains 4 is binded to Head .
    //       ik = IKChains+4 ;
    //       QW = ik->getJoints()[0].getQwBWD();
    //       QL = ChestQWInv*QW ;
    //       output.HeadOri = QL.toEuler().RadToDegree() ;
    //       output.HeadQuat = QL ;
    //   }
    //   else
    //   {
    output.HeadOri = EulerAngle(0,0,0) ;
    output.HeadQuat = Quaternion(1,0,0,0) ;
    //   }
}



void HumanIK::setIKGoal(Fabrik*  pFabrik, Vector3D inpGoal, Quaternion* pQ)
{
    Position goal = inpGoal ;
    Joint GOAL(goal);
    pFabrik->setGoal(GOAL);
    if ( pQ )
    {
        int n = pFabrik->getJointsCount() ;
        LockQuaternionConstraint::Param param ;
        param.q = *pQ ;
        pFabrik->getJoints() [n-1].addConstraint(Joint::LockQuaternion,&param);
    }
    else
    {
        int n = pFabrik->getJointsCount() ;
        pFabrik->getJoints() [n-1].addConstraint(Joint::NoConstraint,NULL);
    }
}

