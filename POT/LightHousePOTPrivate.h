#ifndef LIGHTHOUSEPOTPRIVATE_H
#define LIGHTHOUSEPOTPRIVATE_H

#include "stdint.h"
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Matrix.h"


class LightHouseFusion ;
class LightHousePOTPrivate {

    friend class IKMonitor ;


    friend class LightHousePOT ;

public:
     LightHouseFusion* getFusion() const ;
     void setLhPos(const Vector3D& pos) ;
     void setLhQuat(const Quaternion& q) ;


     const Vector3D & lhPos () const ;
     const Quaternion & lhQuat () const ;
     int stationNum () const ;
     const Vector3D & latestMappedP () const ;
     const Quaternion & latestMappedQ () const ;
     int latestStationID () const ;
     uint32_t latestTimeStamp () const ;
     uint32_t preTimeStamp () const ;
     Float tsLSB() const ;


private:
    // a private constructor to make sure that only the parent(friend)
    // interface class can instance an object of this class .
    LightHousePOTPrivate () ;
    ~LightHousePOTPrivate () ;


    void allocResource() ;
    void releaseResource() ;
    void reallocResource() ;


    //// for Result
    Vector3D LhPos ;
    Quaternion LhQuat ;
    bool solve() ;


    //// for Input Raw Data
    int StationNum ;
    Float TS_LSB ;
    struct RawInput {
        Vector3D Pos ;
        Quaternion Ori ;
        int PreFrames ;
        int Frames ;
        int FPS ;
    } *LatestRaw ;  // LatestRaw [StationNum] ;
    void recordRaw ( const Vector3D& rawP, const Quaternion& rawQ, int stationID, uint32_t ts ) ;
    Vector3D LatestMappedP ;
    Quaternion LatestMappedQ ;
    int PreStationID ;
    int LatestStationID ;
    uint32_t LatestTimeStamp ;
    uint32_t PreTimeStamp ;

    //// for Station-Fusion
    friend class LightHouseFusion ;
    LightHouseFusion* Fusion ;
    bool FusePos, FuseOri ;

    //// for Map
    Matrix* TransMat ;    // Matrix TransMat [StationNum]
    bool* TransMatReady ; // bool TransMatReady [StationNum]
    Quaternion* TransQuat ;// Quaternion TransQuat [StationNum] ;
    void generateTransQuat(int idx) ;

};

#endif // LIGHTHOUSEPOTPRIVATE_H

