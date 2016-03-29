#include "InputAdaptor.h"

InputAdaptor::InputAdaptor()
{
    resetOutput () ;
    EnableAux = true ;
}

void InputAdaptor::resetOutput () {
    Output* p = &LatestOutput ;
    p->P_Head = p->P_LeftAnkle = p->P_LeftWrist = p->P_Neck = p->P_RightAnkle =
            p->P_RightWrist = Vector3D(0,0,0);
    p->Q_Head = p->Q_LeftAnkle = p->Q_LeftWrist = p->Q_Neck = p->Q_RightAnkle =
            p->Q_RightWrist = Quaternion(1,0,0,0);
}

void InputAdaptor::process(int id,  HumanIK& ik, POT** pot, int Mode, int IKSrc)
{
    if(!humanFK.isFKCalibrated())
        return ;
    Quaternion Q;
    Q = pot[id - 1]->getOri();
    setResultBufferQ(id, Q);
    bool LHValid = pot[id-1]->latestLhValid();
    CheckMissingLH(id, LHValid);
    GetPosition(LHValid, id, pot, ik);
    computeIK(id, ik, Mode, IKSrc);
    return ;
}

void InputAdaptor::setResultBufferQ(int id, Quaternion Q)
{
    switch(id){
    case 1:
        resultBuffer.Q_LeftWrist = Q;
    break;
    case 2:
        resultBuffer.Q_RightWrist = Q;
        break;
    case 3:
        resultBuffer.Q_LeftAnkle = Q;
        break;
    case 4:
        resultBuffer.Q_RightAnkle = Q;
        break;
    case 5:
        resultBuffer.Q_Neck = Q;
        break;
    case 6:
        resultBuffer.Q_LeftElbow = Q;
        return;
    case 7:
        resultBuffer.Q_RightElbow = Q;
        return;
    default:
        break;
    }
    return;
}

void InputAdaptor::setResultBufferP(int id, Vector3D P, bool LatestAuxFlag)
{
    switch(id){
    case 1:
        if(LatestAuxFlag)
            resultBuffer.fkP_LeftWrist = P;
        else
        {
            resultBuffer.P_LeftWrist = P;       //real P
//////////////////////Calc PNeck use Pin_LeftWrist
            Quaternion Qrot = resultBuffer.Q_LeftWrist* humanFK.getQinit(HumanFK::Joint_LeftWrist).conj();
            Vector3D offset = Quaternion::rotVbyQ(humanFK.Offsets.offset_LeftWrist, Qrot);
            Vector3D Pin  = P + offset;
            Vector3D dist = humanFK.output().NeckPos - humanFK.output().LeftWristPos;
            pNeck.LeftWrist = Pin + dist;
        }
        break;
    case 2:
        if(LatestAuxFlag)
            resultBuffer.fkP_RightWrist = P;
        else
        {
            resultBuffer.P_RightWrist = P;      //real P
//////////////////////Calc PNeck use Pin_RightWrist
            Quaternion Qrot = resultBuffer.Q_RightWrist* humanFK.getQinit(HumanFK::Joint_RightWrist).conj();
            Vector3D offset = Quaternion::rotVbyQ(humanFK.Offsets.offset_RightWrist, Qrot);
            Vector3D Pin  = P + offset;
            Vector3D dist = humanFK.output().NeckPos - humanFK.output().RightWristPos;
            pNeck.RightWrist = Pin + dist;
        }
        break;
    case 3:
        if(LatestAuxFlag)
            resultBuffer.fkP_LeftAnkle = P;
        else
        {
            resultBuffer.P_LeftAnkle = P;       //real P
//////////////////////Calc PNeck use Pin_Neck
            Quaternion Qrot = resultBuffer.Q_LeftAnkle* humanFK.getQinit(HumanFK::Joint_LeftAnkle).conj();
            Vector3D offset_Left = Quaternion::rotVbyQ(humanFK.Offsets.offset_LeftAnkle, Qrot);
            Vector3D Pin_Left  = P + offset_Left;

            Qrot =  resultBuffer.Q_RightAnkle* humanFK.getQinit(HumanFK::Joint_RightAnkle).conj();
            Vector3D offset_Right = Quaternion::rotVbyQ(humanFK.Offsets.offset_RightAnkle, Qrot);
            Vector3D Pin_Right = resultBuffer.P_RightAnkle + offset_Right;

            Vector3D center = ( Pin_Left + Pin_Right ) / 2;
            pNeck.Legs = Vector3D(center.getX(), 0, center.getZ());
        }
        break;
    case 4:
        if(LatestAuxFlag)
            resultBuffer.fkP_RightAnkle = P;
        else
        {
            resultBuffer.P_RightAnkle = P;      //real P
//////////////////////Calc PNeck use Pin_Neck
            Quaternion Qrot = resultBuffer.Q_LeftAnkle* humanFK.getQinit(HumanFK::Joint_LeftAnkle).conj();
            Vector3D offset_Left = Quaternion::rotVbyQ(humanFK.Offsets.offset_LeftAnkle, Qrot);
            Vector3D Pin_Left  = resultBuffer.P_LeftAnkle  + offset_Left;

            Qrot =  resultBuffer.Q_RightAnkle* humanFK.getQinit(HumanFK::Joint_RightAnkle).conj();
            Vector3D offset_Right = Quaternion::rotVbyQ(humanFK.Offsets.offset_RightAnkle, Qrot);
            Vector3D Pin_Right = P + offset_Right;

            Vector3D center = ( Pin_Left + Pin_Right ) / 2;
            pNeck.Legs = Vector3D(center.getX(), 0, center.getZ());
        }
        break;
    case 5:
        if(LatestAuxFlag)
            resultBuffer.fkP_Neck = resultBuffer.P_Neck;
        else
        {
            resultBuffer.P_Neck = P;            //real P
//////////////////////Calc PNeck use Pin_Neck
            Quaternion Qrot = resultBuffer.Q_Neck* humanFK.getQinit(HumanFK::Joint_Neck).conj();
            Vector3D offset = Quaternion::rotVbyQ(humanFK.Offsets.offset_Neck, Qrot);
            pNeck.P_Neck  = P  + offset;
         }
        resultBuffer.LatestPNeck = resultBuffer.P_Neck;
        break;
    default:
        break;
    }
    return;
}

void InputAdaptor::GetPosition(bool LHValid, int id, POT** pot,  HumanIK& ik)
{
    Vector3D P, offset;
    if (LHValid)
    {
        LatestAuxFlag = false ;
        P = pot[id - 1]->getPos();
        setResultBufferP(id, P, LatestAuxFlag);
        if(id == 5)
            return;
    }
    LatestAuxFlag = true;
    HumanFK::InputFrame Din;
    switch(id){
    case 1:
        Din.id = HumanFK::Joint_LeftWrist;

        Din.Qthis   = resultBuffer.Q_LeftWrist * humanFK.getQinit(HumanFK::Joint_LeftWrist).conj();    // debugLeftArm.QThis  =
        Din.Qlinker = resultBuffer.Q_LeftElbow * humanFK.getQinit(HumanFK::Joint_LeftElbow).conj();    // debugLeftArm.QLinker=
        Din.Qroot   = resultBuffer.Q_Neck * humanFK.getQinit(HumanFK::Joint_Neck).conj();              // debugLeftArm.QRoot =
        Din.Proot   = resultBuffer.fkResult_Neck;                                                      // debugLeftArm.PNeck =
        humanFK.compute(Din); //humanFK.computeForDebug (Din, debugLeftArm.PNeck, debugLeftArm.PShoulder, debugLeftArm.PElbow, debugLeftArm.PWrist);
        offset = Quaternion::rotVbyQ(humanFK.getOffset(HumanFK::Joint_LeftWrist), Din.Qthis);
        resultBuffer.fkResult_LeftWrist = humanFK.output().LeftWristPos ;
        P = resultBuffer.fkResult_LeftWrist - offset;
        break;
    case 2:
        Din.id = HumanFK::Joint_RightWrist;

        Din.Qthis   = resultBuffer.Q_RightWrist * humanFK.getQinit(HumanFK::Joint_RightWrist).conj();
        Din.Qlinker = resultBuffer.Q_RightElbow * humanFK.getQinit(HumanFK::Joint_RightElbow).conj();
        Din.Qroot   = resultBuffer.Q_Neck  * humanFK.getQinit(HumanFK::Joint_Neck).conj();
        Din.Proot   = resultBuffer.fkResult_Neck;
        humanFK.compute(Din);
        offset = Quaternion::rotVbyQ(humanFK.getOffset(HumanFK::Joint_RightWrist), Din.Qthis);
        resultBuffer.fkResult_RightWrist = humanFK.output().RightWristPos ;
        P = resultBuffer.fkResult_RightWrist - offset;
        break;
    case 5:
        Din.id = HumanFK::Joint_Neck;
        Din.Qthis   = resultBuffer.Q_Neck * humanFK.getQinit(HumanFK::Joint_Neck).conj();
        Din.Qlinker = Quaternion(1,0,0,0);
        Din.Qroot = ik.output().HipQuat;
        Din.Proot = ik.output().HipPos;
        humanFK.compute(Din);
        offset = Quaternion::rotVbyQ(humanFK.getOffset(HumanFK::Joint_Neck), Din.Qthis);
        resultBuffer.fkResult_Neck = humanFK.output().NeckPos;
        P = resultBuffer.fkP_Neck - offset;
        break;
    default:
        return ;
    }
    setResultBufferP(id, P, LatestAuxFlag);
}

void InputAdaptor::computeIK(int id, HumanIK& ik, int Mode, int InputSrc)
{
    HumanIK::RawDataFrame rawData ;
    if(Mode == 1)
    {
        if(id == 1)
        {
            rawData.id = HumanIK::Joint_LeftWrist ;
            if(InputSrc == 0)
            //if(resultBuffer.isMissing_LeftWrist)
                rawData.Pos = resultBuffer.fkP_LeftWrist;  //P
            else if(InputSrc == 1)
            //else
                rawData.Pos = resultBuffer.P_LeftWrist;

            rawData.Q = resultBuffer.Q_LeftWrist;                 //pot[idx]->getHeadQuat();
            ik.input(rawData) ;
#ifdef BothArm
            ik.compute(HumanIK::CALC_LEFT_ARM);

            rawData.id = HumanIK::Joint_Neck;
            rawData.Pos = resultBuffer.LatestPNeck;
            rawData.Q = resultBuffer.Q_Neck ;
            ik.input(rawData) ;
            ik.compute(HumanIK::CALC_HIP);
#endif
        }
        else if(id == 2)
        {
            rawData.id = HumanIK::Joint_RightWrist ;
            if(InputSrc == 0)
            //if(resultBuffer.isMissing_RightWrist)
                rawData.Pos = resultBuffer.fkP_RightWrist;
            else if(InputSrc == 1)
            //else
                rawData.Pos = resultBuffer.P_RightWrist;

            rawData.Q = resultBuffer.Q_RightWrist;
            ik.input(rawData) ;
#ifdef BothArm
            ik.compute(HumanIK::CALC_RIGHT_ARM);

            rawData.id = HumanIK::Joint_Neck;
            rawData.Pos = resultBuffer.LatestPNeck;
            rawData.Q = resultBuffer.Q_Neck ;
            ik.input(rawData) ;
            ik.compute(HumanIK::CALC_HIP);
#endif
        }
#ifdef WholeBody
        else if(id ==3 )
        {
            rawData.id = HumanIK::Joint_LeftAnkle;
            if(InputSrc == 0)
                rawData.Pos = resultBuffer.P_LeftAnkle;
            else if(InputSrc == 1)
                rawData.Pos = resultBuffer.P_LeftAnkle;
            rawData.Q = resultBuffer.Q_LeftAnkle;
            ik.input(rawData) ;

            rawData.id = HumanIK::Joint_Neck;
            rawData.Pos = resultBuffer.LatestPNeck;
            rawData.Q = resultBuffer.Q_Neck ;
            ik.input(rawData) ;
            //ik.compute(HumanIK::CALC_HIP);
        }
        else if(id == 4)
        {
            rawData.id = HumanIK::Joint_RightAnkle;
            if(InputSrc == 0)
                rawData.Pos = resultBuffer.P_RightAnkle;
            else if(InputSrc == 1)
                rawData.Pos = resultBuffer.P_RightAnkle;
            rawData.Q = resultBuffer.Q_RightAnkle;

            ik.input(rawData) ;
            rawData.id = HumanIK::Joint_Neck;
            rawData.Pos = resultBuffer.LatestPNeck;
            rawData.Q = resultBuffer.Q_Neck ;
            ik.input(rawData) ;
        }
        ik.compute(HumanIK::CALC_Execpt_HEAD);
#endif
        ik.compute(HumanIK::CALC_HIP);
    }

    else if(Mode == 2)
    {
        if(id == 1)
        {
            rawData.id = HumanIK::Joint_LeftWrist ;
            if(InputSrc == 0)
            //if(resultBuffer.isMissing_LeftWrist)
                rawData.Pos = resultBuffer.fkP_LeftWrist;  //P
            else if(InputSrc == 1)
            //else
                rawData.Pos = resultBuffer.P_LeftWrist;

            rawData.Q = resultBuffer.Q_LeftWrist;                 //pot[idx]->getHeadQuat();
            ik.input(rawData) ;
#ifdef BothArm
            ik.compute(HumanIK::CALC_LEFT_ARM);

            rawData.id = HumanIK::Joint_Neck;
            rawData.Pos = resultBuffer.LatestPNeck;
            rawData.Q = resultBuffer.Q_Neck ;
            ik.input(rawData) ;
            ik.compute(HumanIK::CALC_HIP);
#endif
        }
        else if(id == 2)
        {
            rawData.id = HumanIK::Joint_RightWrist ;
            if(InputSrc == 0)
            //if(resultBuffer.isMissing_RightWrist)
                rawData.Pos = resultBuffer.fkP_RightWrist;
            else if(InputSrc == 1)
            //else
                rawData.Pos = resultBuffer.P_RightWrist;

            rawData.Q = resultBuffer.Q_RightWrist;
            ik.input(rawData) ;
#ifdef BothArm
            ik.compute(HumanIK::CALC_RIGHT_ARM);

            rawData.id = HumanIK::Joint_Neck;
            rawData.Pos = resultBuffer.LatestPNeck;
            rawData.Q = resultBuffer.Q_Neck ;
            ik.input(rawData) ;
            ik.compute(HumanIK::CALC_HIP);
#endif
        }
    }
    return;
}

void InputAdaptor::CheckMissingLH(int id, bool LHValid)
{
    switch(id){
    case 1:
        if(LHValid)
        {
            resultBuffer.NACnt_LeftWrist = 0;
            resultBuffer.isMissing_LeftWrist = false;
        }
        else
        {
            resultBuffer.NACnt_LeftWrist ++;
            if(resultBuffer.NACnt_LeftWrist > 4)
                resultBuffer.isMissing_LeftWrist = true;
        }
        break;
    case 2:
        if(LHValid)
        {
            resultBuffer.NACnt_RightWrist = 0;
            resultBuffer.isMissing_RightWrist = false;
        }
        else
        {
            resultBuffer.NACnt_RightWrist ++;
            if(resultBuffer.NACnt_RightWrist > 4)
                resultBuffer.isMissing_RightWrist = true;
        }
        break;
    case 3:
        if(LHValid)
        {
            resultBuffer.NACnt_LeftAnkle = 0;
            resultBuffer.isMissing_LeftAnkle = false;
        }
        else
        {
            resultBuffer.NACnt_LeftAnkle ++;
            if(resultBuffer.NACnt_LeftAnkle > 4)
                resultBuffer.isMissing_LeftWrist = true;
        }
        break;
    case 4:
        if(LHValid)
        {
            resultBuffer.NACnt_RightAnkle = 0;
            resultBuffer.isMissing_RightAnkle = false;
        }
        else
        {
            resultBuffer.NACnt_RightAnkle ++;
            if(resultBuffer.NACnt_RightAnkle > 4)
                resultBuffer.isMissing_RightWrist = true;
        }
        break;
    case 5:
        if(LHValid)
        {
            resultBuffer.NACnt_Neck = 0;
            resultBuffer.isMissing_Neck = false;
        }
        else
        {
            resultBuffer.NACnt_Neck ++;
            if(resultBuffer.NACnt_Neck > 4)
                resultBuffer.isMissing_Neck = true;
        }
        break;
    default:
        return;
    }
    return;
}


/*
void InputAdaptor::CheckMissingLH(int id, const uint32_t prev, const uint32_t cur)
{
    Float TS_LSB = 0.001 ;
    Float dt = TS_LSB * ( cur - prev ) ;
    switch(id)
    {
    case 1:
        if(dt > 0.03)
        {
            resultBuffer.isMissing_LeftWrist = true;
            printf("%d\t%f\n",id,dt);
        }
        else
            resultBuffer.isMissing_LeftWrist = false;
        break;
    case 2:
        if(dt > 0.03)
            resultBuffer.isMissing_RightWrist = true;
        else
            resultBuffer.isMissing_RightWrist = false;
        break;
    case 3:
        if(dt > 0.03)
            resultBuffer.isMissing_LeftAnkle = true;
        else
            resultBuffer.isMissing_LeftAnkle = false;
        break;
    case 4:
        if(dt > 0.03)
            resultBuffer.isMissing_RightAnkle = true;
        else
            resultBuffer.isMissing_RightAnkle = false;
        break;
    case 5:
        if(dt > 0.03)
        {
            resultBuffer.isMissing_Neck = true;
            printf("%d\t%f\n",id,dt);
        }
        else
            resultBuffer.isMissing_Neck = false;
        break;
    default:
        break;
    }


}
*/
