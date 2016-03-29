#ifndef POTPRIVATE_H
#define POTPRIVATE_H

#include "LightHousePOT.h"

class POTPrivate
{
    friend class IKMonitor ;
    friend class POT ;


public:
    LightHousePOT* getLhpot() ;



private:
    // a private constructor to make sure that only the parent(friend)
    // interface class can instance an object of this class .
    POTPrivate() ;
    ~POTPrivate() ;

    //// components
    LightHousePOT* lhpot ;


    //// for Result
    bool LhStarted ;
    Vector3D FinalPos;
    Quaternion FinalQuat;
    Quaternion RefQuat ;

    //// algorithm parameters .
    bool  UsingGyro ;
    float GyroRegressRatio ;

    //// for Input Raw Data
    Quaternion LatestRawQg ;
    Quaternion PreRawQg ;
    bool LatestLhValid ;

    //// functions
    bool processRaw ( const Vector3D& rawP, const Quaternion& rawQ,
                      const Quaternion& rawQgyro, int LHStationID,
                      uint32_t ts, bool LHValid ) ;
    void recordRaw ( const Quaternion& rawQgyro, bool LHValid ) ;
    void enableGyroFusion () ;
    void disbleGyroFusion () ;
    void reset() ;
};

#endif // POTPRIVATE_H
