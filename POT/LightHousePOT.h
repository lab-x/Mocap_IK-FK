#ifndef LIGHTHOUSEPOT_H
#define LIGHTHOUSEPOT_H

#include "stdint.h"
#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Quaternion.h"


class LightHousePOTPrivate ;
class LightHousePOT
{
    friend class IKMonitor ;


public:
    LightHousePOT();
    ~LightHousePOT();
    void setStationNum( int StationNum ) ;
    int getStationNum () const;


public:
    void setTransMat (Float* Mat, int id);
    void setTransMat (const Matrix & Mat, int id);
    bool processRaw ( const Vector3D& rawP, const Quaternion& rawQ, int stationID, uint32_t ts );
    Vector3D getPos() const  ;
    Quaternion getOri() const ;

public:
    void resetFusion() ;
    void enablePosFusion () ;
    void enableOriFusion () ;
    void disablePosFusion () ;
    void disableOriFusion () ;
    bool isPosFusionEnabled() const ;
    bool isOriFusionEnabled() const ;

    Vector3D getFusionPosOffset () const ;
    Quaternion getFusionOriOffset () const ;


    const Vector3D& getLatestRawPos (int stationID) const ;
    const Quaternion& getLatestRawOri (int stationID) const ;
    const Matrix& getTransMat(int stationID) const ;

    bool LoadTransMats(const Matrix &A);


public:
     struct CaliInput {
         Vector3D offset ;
         Vector3D A ;
         Vector3D O ;
         Vector3D B ;
     } ;
     void calibrate(int stationID, const CaliInput& raw) ;
     bool isCalibrated() const ;


    ////  FOR MORE DETAIL FEATUREs
    LightHousePOTPrivate* Priv() const ;


private:
    LightHousePOTPrivate *d ;
};

#endif // LIGHTHOUSEPOT_H
