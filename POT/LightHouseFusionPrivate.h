#ifndef LIGHTHOUSEFUSIONPRIVATE_H
#define LIGHTHOUSEFUSIONPRIVATE_H

#include "stdint.h"
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Matrix.h"

class LightHousePOTPrivate ;
class LightHouseFusionPrivate
{
    friend class IKMonitor ;
    friend class LightHouseFusion ;

private:
    // a private constructor to make sure that only the parent(friend)
    // interface class can instance an object of this class .
    LightHouseFusionPrivate(LightHousePOTPrivate*) ;
    ~LightHouseFusionPrivate() ;

    void allocResource() ;
    void releaseResource() ;
    void reallocResource() ;
    Float getHistoryWeight(Float dt) ;
    void reset () ;
    bool Filter () ;// {return true;}
    bool Fuse ( bool Fuse_Pos, bool Fuse_Ori ) ;
    bool Fuse2 ( bool Fuse_Pos, bool Fuse_Ori ) ;
    Vector3D weightCenter(const Float* weight, const Vector3D* v, int n) ;
    Quaternion weightCenter(const Float* weight,  const Quaternion* q, int n) ;


    // Algorithm parameters, the performance depends on these .
    Float OffsetDampingTime ;
    Float HistoryConfidenceDampingTime ;
    bool PosSmooth ;
    bool OriSmooth ;
    Float SmoothRatio ;

    // Vars
    Vector3D PosOffset ;
//    AxisAngle OriOffset ;
    Quaternion OriOffset ;
    struct HistoryData {
        Vector3D MappedPos ;
        Vector3D FusedPos ;
        Vector3D FinalLhPos ;
        Quaternion MappedOri ;
        Quaternion FusedOri ;
        Quaternion FinalLhOri ;
        uint32_t TS ;
        int StationID ;
        bool valid ;
        bool registered ;
    } *History ;

    HistoryData* lastHistory() ;
    HistoryData* lastRegistered() ;
    int lastHistoryIdx() ;
    int lastRegisteredIdx() ;
    LightHousePOTPrivate* d ;
};



#endif // LIGHTHOUSEFUSIONPRIVATE_H

