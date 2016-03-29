#include "POT.h"
#include "POTPrivate.h"


#define OPEN_INTERNAL_INTERFACEs


POTPrivate::POTPrivate()
{
    lhpot = new LightHousePOT() ;
    reset();
    GyroRegressRatio = 0.05 ;
    UsingGyro = true ;
    RefQuat = Quaternion(1,0,0,0) ;
}
POTPrivate::~POTPrivate()
{
    delete lhpot ;
}

void POTPrivate::reset()
{
    lhpot->resetFusion();
    LhStarted = false  ;
    FinalPos    = Vector3D (0,0,0) ;
    FinalQuat   = Quaternion(1,0,0,0) ;

    LatestRawQg = Quaternion(1,0,0,0) ;
    PreRawQg    = Quaternion(1,0,0,0) ;
    LatestLhValid = false ;
}

void POTPrivate::enableGyroFusion ()
{
    UsingGyro = true ;
}

void POTPrivate::disbleGyroFusion ()
{
    UsingGyro = false ;
}

void POTPrivate::recordRaw ( const Quaternion& rawQgyro, bool LHValid )
{
   PreRawQg = LatestRawQg ;
   LatestRawQg = rawQgyro ;
   LatestLhValid = LHValid ;
}

bool POTPrivate::processRaw ( const Vector3D& rawP, const Quaternion& rawQ,
                  const Quaternion& rawQgyro, int LHStationID,
                  uint32_t ts, bool LHValid )
{
    bool ret = false ;
    recordRaw ( rawQgyro, LHValid ) ;

    if ( ! LhStarted )
    {
        if ( LHValid )
        {
            if ( lhpot->processRaw(rawP,rawQ, LHStationID, ts) )
            {
                LhStarted = true ;
                FinalPos = lhpot->getPos() ;
                FinalQuat = lhpot->getOri() ;
                ret = true ;
            }
        }
    }

    else
    {
        if ( LHValid && lhpot->processRaw(rawP,rawQ, LHStationID, ts) )
        {
            if ( UsingGyro )
            {
                FinalPos = lhpot->getPos() ;
                Quaternion Q1 = FinalQuat * PreRawQg.conj() * LatestRawQg ;
                Q1.normalize();
                FinalQuat = Quaternion::Slerp (Q1, lhpot->getOri(), GyroRegressRatio) ;
            }
            else
            {
                FinalPos = lhpot->getPos() ;
                FinalQuat = lhpot->getOri() ;
            }
            ret = true ;
        }

        else
        {
            if ( UsingGyro )
            {
                FinalPos = FinalPos ;
                FinalQuat = FinalQuat * PreRawQg.conj() * LatestRawQg ;
                ret = true ;
            }
        }
    }

    return ret ;
}






POT::POT()
{
    d = new POTPrivate() ;
    reset();
}

POT::~POT()
{
    delete d ;
}


void POT::setLHStationNum (int stationNum )
{
    d->lhpot->setStationNum(stationNum);
}

void POT::setLHTransMat(Float* Mat, int stationID)
{
    d->lhpot->setTransMat(Mat,stationID);
}

void POT::setLHTransMat(const Matrix& Mat, int stationID)
{
    d->lhpot->setTransMat(Mat,stationID);
}

const Matrix& POT::getLHTransMat(int stationID)
{
    return d->lhpot->getTransMat(stationID) ;
}

bool POT::LoadLHTransMats(const Matrix& Mat)
{
    d->lhpot->LoadTransMats(Mat) ;
}


bool POT::processRaw ( const Vector3D& rawP, const Quaternion& rawQ,
                  const Quaternion& rawQgyro, int LHStationID,
                  uint32_t ts, bool LHValid )
{
    return d->processRaw(rawP,rawQ,rawQgyro,LHStationID,ts,LHValid) ;
}


void POT::reset()
{
    d->reset();
}

bool POT::latestLhValid()
{
    return d->LatestLhValid ;
}

Vector3D POT::getPos() const
{
    return d->FinalPos ;
}

Quaternion POT::getOri() const
{
//    return d->RefQuat.conj() * d->FinalQuat ; // the order is wrong .
    return d->FinalQuat * d->RefQuat.conj() ;
}


void POT::loadRefOri()
{
    d->RefQuat = d->FinalQuat ;
}

void POT::resetRefOri()
{
    d->RefQuat = Quaternion(1,0,0,0) ;
}




////////////////////////////////////////////////////////////////
//////////////////// INTERNAL_INTERFACEs ///////////////////////

#ifdef OPEN_INTERNAL_INTERFACEs


#include "LightHousePOT.h"
#include "LightHousePOTPrivate.h"
#include "LightHouseFusionPrivate.h"
#include "LightHouseFusion.h"


LightHousePOTPrivate* POT::pLHPOT() const
{
    return d->lhpot->Priv() ;
}

LightHouseFusionPrivate* POT::pLHFusion() const
{
    return d->lhpot->Priv()->getFusion()->Priv() ;
}


POTPrivate* POT::Priv() const
{
    return d ;
}

LightHousePOT* POT::LHPOT() const
{
    return d->lhpot ;
}

LightHouseFusion* POT::LHFusion() const
{
    return d->lhpot->Priv()->getFusion() ;
}


#endif
