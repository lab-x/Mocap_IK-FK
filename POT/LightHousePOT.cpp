#include "LightHousePOT.h"
#include "LightHousePOTPrivate.h"
#include "LightHouseFusion.h"
#include "LightHouseFusionPrivate.h"



LightHousePOTPrivate::LightHousePOTPrivate ()
{
    StationNum = 0 ;
    LatestRaw = NULL ;
    TransMat = NULL ;
    TransMatReady = NULL ;
    TransQuat = NULL ;
    Fusion = new LightHouseFusion(this) ;
    FuseOri = true ;
    FusePos = true ;
    TS_LSB  =  0.001 ;
}

LightHousePOTPrivate::~LightHousePOTPrivate ()
{
    releaseResource();
    delete Fusion ;
}

void LightHousePOTPrivate::allocResource()
{
    if ( StationNum == 0 )
    {
        LatestRaw = NULL ;
        TransMat = NULL ;
        TransMatReady = NULL ;
        TransQuat = NULL ;
//        CaliRaw = NULL ;
        return ;
    }

    LatestRaw = new RawInput[StationNum] ;
    TransMat = new Matrix[StationNum] ;
    TransMatReady = new bool[StationNum] ;
    TransQuat = new Quaternion[StationNum] ;
//    CaliRaw = new CaliInput[StationNum] ;

    int i ;
    for ( i=0; i<StationNum; i++ )
    {
        LatestRaw[i].Ori = Quaternion (1,0,0,0) ;
        LatestRaw[i].Pos = Vector3D (0,0,0) ;
        LatestRaw[i].PreFrames = 0 ;
        LatestRaw[i].Frames = 0 ;
        TransMat[i] = Matrix(3,4) ;
        TransQuat[i] = Quaternion (1,0,0,0) ;
        TransMatReady[i] = false ;
    }

    Fusion->allocResource();
}

void LightHousePOTPrivate::releaseResource()
{
    Fusion->releaseResource();
    if ( LatestRaw ) { delete []LatestRaw ; LatestRaw = NULL ; }
    if ( TransMat ) { delete []TransMat ; TransMat = NULL ; }
    if ( TransMatReady ) { delete []TransMatReady ; TransMatReady = NULL ; }
    if ( TransQuat ) { delete []TransQuat ; TransQuat = NULL ; }
//    if ( CaliRaw ) { delete []CaliRaw ; CaliRaw = NULL ; }
}
void LightHousePOTPrivate::reallocResource()
{
    releaseResource() ;
    allocResource() ;
}


void LightHousePOTPrivate::recordRaw ( const Vector3D& rawP, const Quaternion& rawQ,
                                       int stationID, uint32_t ts )
{
    int idx = stationID - 1 ;

    PreStationID = LatestStationID ;
    LatestStationID = stationID ;
    PreTimeStamp = LatestTimeStamp ;
    LatestTimeStamp = ts ;

    LatestRaw[idx].Pos = rawP ;
    LatestRaw[idx].Ori = rawQ ;
    LatestRaw[idx].Frames ++  ;
}

void LightHousePOTPrivate::generateTransQuat(int stationID)
{
    int i, j;
    int idx = stationID - 1 ;
    Matrix R(3,3) ;
    for ( i=0; i<3; i++ )
        for ( j=0; j<3; j++ )
            R(i, j) = TransMat[idx](i, j);

    TransQuat[idx] = Quaternion::mat2Quat(R) ;
}


bool LightHousePOTPrivate::solve()
{
    Matrix rawP = Matrix(4,1);
    Quaternion rawQ ;
    Matrix Axis;

    int idx = LatestStationID - 1 ;
    rawQ = LatestRaw[idx].Ori ;
    rawQ.normalize();
    rawP(0,0) = LatestRaw[idx].Pos.getX();
    rawP(1,0) = LatestRaw[idx].Pos.getY();
    rawP(2,0) = LatestRaw[idx].Pos.getZ();
    rawP(3,0) = 1;
    Axis = TransMat[idx] * rawP ;
    LatestMappedP = Vector3D(Axis(0,0), Axis(1,0), Axis(2,0));
    LatestMappedQ = TransQuat[idx] * rawQ ;    


    LatestMappedQ.normalize();

    bool ret = Fusion->Fuse ( FusePos, FuseOri ) ;
    if ( !FusePos )
        LhPos = LatestMappedP ;
    if ( !FuseOri )
        LhQuat = LatestMappedQ ;

    if ( !FusePos && !FuseOri )
        return true ;

    return ret ;
}

LightHouseFusion* LightHousePOTPrivate::getFusion() const
{
    return Fusion ;
}

void LightHousePOTPrivate::setLhPos(const Vector3D& pos)
{
    LhPos = pos ;
}

void LightHousePOTPrivate::setLhQuat(const Quaternion& q)
{
    LhQuat = q ;
}


const Vector3D & LightHousePOTPrivate::lhPos () const
{
    return LhPos ;
}

const Quaternion & LightHousePOTPrivate::lhQuat () const
{
    return LhQuat ;
}

int LightHousePOTPrivate::stationNum () const
{
    return StationNum ;
}

const Vector3D & LightHousePOTPrivate::latestMappedP () const
{
    return LatestMappedP ;
}

const Quaternion & LightHousePOTPrivate::latestMappedQ () const
{
    return LatestMappedQ ;
}

int LightHousePOTPrivate::latestStationID () const
{
    return LatestStationID ;
}

uint32_t LightHousePOTPrivate::latestTimeStamp () const
{
    return LatestTimeStamp ;
}

uint32_t LightHousePOTPrivate::preTimeStamp () const
{
    return PreTimeStamp ;
}

Float LightHousePOTPrivate::tsLSB() const
{
    return TS_LSB ;
}


LightHousePOT::LightHousePOT()
{
    d = new LightHousePOTPrivate ;
}

LightHousePOT::~LightHousePOT()
{
    delete d ;
}

void LightHousePOT::setStationNum( int StationNum )
{
    d->StationNum = StationNum ;
    d->reallocResource() ;
}

int LightHousePOT::getStationNum () const
{
    return d->StationNum ;
}

void LightHousePOT::setTransMat (Float* Mat, int stationID)
{
    int idx = stationID - 1 ;

    int i, j;
    for ( i=0; i<3; i++ )
        for ( j=0; j<4; j++ )
            d->TransMat[idx](i,j) = Mat[i*4+j] ;
    d->generateTransQuat (stationID);
    d->TransMatReady[idx] = true ;
}



void LightHousePOT::setTransMat (const Matrix & Mat, int stationID)
{
    int idx = stationID - 1 ;

    int i, j;
    for ( i=0; i<3; i++ )
        for ( j=0; j<4; j++ )
            d->TransMat[idx](i,j) = Mat(i,j) ;

    d->generateTransQuat(stationID);
    d->TransMatReady[idx] = true ;
}


bool LightHousePOT::processRaw ( const Vector3D& rawP, const Quaternion& rawQ, int stationID, uint32_t ts )
{
    d->recordRaw ( rawP, rawQ, stationID, ts ) ;
    if ( ! isCalibrated() )
        return false ;

    return d->solve() ;
}

Vector3D LightHousePOT::getPos() const
{
    return d->LhPos ;
}

Quaternion LightHousePOT::getOri() const
{
    return d->LhQuat ;
}

void LightHousePOT::calibrate(int stationID, const CaliInput& raw)
{
    Matrix TransMat(3,4) ;

    Vector3D OA = raw.A - raw.O ;
    Vector3D OB = raw.B - raw.O ;
    Vector3D X = OA.getNormalized() ;
    Vector3D Y = Vector3D::Cross(OB,OA).getNormalized() ;
    Vector3D Z = Vector3D::Cross(X,Y).getNormalized() ;
    Float T0 = raw.offset.getX() - Vector3D::Dot(raw.O,X) ;
    Float T1 = raw.offset.getY() - Vector3D::Dot(raw.O,Y) ;
    Float T2 = raw.offset.getZ() - Vector3D::Dot(raw.O,Z) ;


    TransMat(0,0) = X.getX() ;
    TransMat(0,1) = X.getY() ;
    TransMat(0,2) = X.getZ() ;
    TransMat(0,3) = T0 ;

    TransMat(1,0) = Y.getX() ;
    TransMat(1,1) = Y.getY() ;
    TransMat(1,2) = Y.getZ() ;
    TransMat(1,3) = T1 ;

    TransMat(2,0) = Z.getX() ;
    TransMat(2,1) = Z.getY() ;
    TransMat(2,2) = Z.getZ() ;
    TransMat(2,3) = T2 ;

    setTransMat(TransMat,stationID);
}


bool LightHousePOT::isCalibrated() const
{
    int i ;
    bool ok = true ;
    for ( i=0; i<getStationNum(); i++ )
    {
        ok = ok && d->TransMatReady[i] ;
    }

    return ok ;
}

void LightHousePOT::resetFusion()
{
    d->Fusion->reset();
}

void LightHousePOT::enablePosFusion ()
{
    d->FusePos = true;
}
void LightHousePOT::enableOriFusion ()
{
    d->FuseOri = true;
}
void LightHousePOT::disablePosFusion ()
{
    d->FusePos = false;
}
void LightHousePOT::disableOriFusion ()
{
    d->FuseOri = false;
}
bool LightHousePOT::isPosFusionEnabled() const
{
    return d->FusePos ;
}
bool LightHousePOT::isOriFusionEnabled() const
{
    return d->FuseOri ;
}

Vector3D LightHousePOT::getFusionPosOffset () const
{
//    return d->Fusion->p->PosOffset ;
    return Vector3D(0,0,0) ;
}
Quaternion LightHousePOT::getFusionOriOffset () const
{
//    return d->Fusion.OriOffset.toQuat() ;
    return Quaternion(1,0,0,0) ;
}



const Vector3D& LightHousePOT::getLatestRawPos (int stationID) const
{
    int idx = stationID - 1 ;
    return d->LatestRaw[idx].Pos ;
}
const Quaternion& LightHousePOT::getLatestRawOri (int stationID) const
{
    int idx = stationID - 1 ;
    return d->LatestRaw[idx].Ori ;
}
const Matrix& LightHousePOT::getTransMat(int stationID) const
{
    int idx = stationID - 1 ;
    return d->TransMat[idx] ;
}
bool LightHousePOT::LoadTransMats (const Matrix &A)
{
    if(A.rowSize() != d->StationNum * 3 || A.colSize() != 4)
    {
        printf("TransMat A Is Not Correct!");
        return false;
    }

    Matrix m(3,4) ;
    for ( int idx=0; idx<d->StationNum; idx++ )
    {
        int stationID = idx + 1 ;
        for ( int i=0; i<3; i++ )
            for ( int j=0; j<4; j++ )
                m(i,j) = A(3*idx+i,j) ;
        setTransMat(m,stationID);
    }

    return true;
}


LightHousePOTPrivate* LightHousePOT::Priv() const
{
    return d ;
}
