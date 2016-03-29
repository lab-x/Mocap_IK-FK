
#include "LightHousePOT.h"
#include "LightHousePOTPrivate.h"
#include "LightHouseFusion.h"
#include "LightHouseFusionPrivate.h"

LightHouseFusionPrivate::LightHouseFusionPrivate(LightHousePOTPrivate* _d)
{
    this->d = _d ;
    allocResource() ;
    reset();

    OffsetDampingTime = 3.0 ;
    HistoryConfidenceDampingTime = 0.5 ;
    PosSmooth = true ;
    OriSmooth = true ;
    SmoothRatio = 0.3 ;
}

LightHouseFusionPrivate::~LightHouseFusionPrivate()
{
    releaseResource();
}

void LightHouseFusionPrivate::reset()
{
    int i ;
    for ( i=0; i<d->stationNum(); i++ )
    {
        History[i].valid = false ;
        History[i].registered = false ;
    }

    PosOffset = Vector3D(0,0,0) ;
    OriOffset = Quaternion(1,0,0,0) ;
}




void LightHouseFusionPrivate::allocResource()
{
    if ( 0 == d->stationNum() )
    {
        History = NULL ;
        return ;
    }

    int i ;
    History = new HistoryData [d->stationNum()] ;
    for ( i=0; i<d->stationNum(); i++ )
        History[i].valid = false ;
}

void LightHouseFusionPrivate::releaseResource()
{
    if ( History )
    {
        delete [] History ;
        History = NULL ;
    }
}

void LightHouseFusionPrivate::reallocResource()
{
    releaseResource();
    allocResource();
}


Float LightHouseFusionPrivate::getHistoryWeight(Float dt)
{
    Float weight ;
    Float T = HistoryConfidenceDampingTime ;


    //Float r = dt / (3*T) ;
    Float r = 3*dt / T ;
    if ( dt < 0.05 )    // data within 0.05s is regard as certain reliable.
        weight = 1 ;
    else if ( dt > T )
        weight = 0 ;
    else
        weight = exp(-r*r/2) ;
    return weight ;
}

bool LightHouseFusionPrivate::Filter ()
{
    //////////////////////////////////////////////////////////////////////////
    // filter cases :
    //  * 相邻两次的 mapped position 之差大于某个阈值，阈值 = maxSpeed*dt + δ1 + δ2 ;
    //
    //////////////////////////////////////////////////////////////////////////
    int i ;
    HistoryData* history = lastHistory() ;

    if ( history )
    {
        int DT = d->latestTimeStamp() - history->TS ;
        if ( DT == 0 )
        {
            // timestamp error !!
//printf("LightHouseFusionPrivate::Filter:  TimeStamp Conflict !\n");
            return true ;
        }
        Vector3D dP = d->latestMappedP() - history->MappedPos ;
        Float dt = d->tsLSB() * ( d->latestTimeStamp() - history->TS ) ;
        Float MaxSpeed = 3.0f ;
        Float MaxDiff = 0.3f ;
        //Float MaxDiff = 0.10f ;
        Float MaxDelta = MaxSpeed * dt + MaxDiff ;
        if ( dP.getLength() > MaxDelta )
        {
//printf("LightHouseFusionPrivate::Filter:  BigJump !! Jump Distance is %lf\n", dP.getLength());
            return true ;
        }
        else
            return false ;
    }
    else
    {
        return false ;
    }
}

int LightHouseFusionPrivate::lastHistoryIdx()
{
    int i, n = d->stationNum() ;
    int idx = -1 ;
    //HistoryData* history = NULL ;
    uint32_t newestTS = 0 ;
    for ( i=0; i<n; i++ )
    {
        if ( History[i].valid && History[i].TS > newestTS )
        {
            newestTS = History[i].TS ;
            //history = &(History[i]) ;
            idx = i ;
        }
    }

    //return history ;
    return idx ;
}

int LightHouseFusionPrivate::lastRegisteredIdx()
{
    int i, n = d->stationNum() ;
    int idx = -1 ;
    //HistoryData* history = NULL ;
    uint32_t newestTS = 0 ;
    for ( i=0; i<n; i++ )
    {
        if ( History[i].registered && History[i].TS > newestTS )
        {
            newestTS = History[i].TS ;
            //history = &(History[i]) ;
            idx = i ;
        }
    }

    //return history ;
    return idx ;
}

LightHouseFusionPrivate::HistoryData* LightHouseFusionPrivate::lastHistory()
{
    int idx = lastHistoryIdx() ;
    if ( idx < 0 )
        return NULL ;
    return History + idx ;
}

LightHouseFusionPrivate::HistoryData* LightHouseFusionPrivate::lastRegistered()
{
    int idx = lastRegisteredIdx() ;
    if ( idx < 0 )
        return NULL ;
    return History + idx ;
}



Vector3D LightHouseFusionPrivate::weightCenter(const Float* weight, const Vector3D* v, int n)
{
    Vector3D result = Vector3D (0,0,0) ;
    Float w = 0, r ;
    for ( int i=0; i<n; i++ )
    {
        w += weight[i] ;
        if ( w > 0 )
        {
            r = weight[i]/w ;
            result = result * (1-r)  +  v[i] * r ;
        }
    }

    return result ;
}

Quaternion LightHouseFusionPrivate::weightCenter(const Float* weight, const Quaternion* q, int n)
{
    Quaternion result = Quaternion(1,0,0,0) ;
    Float w = 0 , r ;
    for ( int i=0; i<n; i++ )
    {
        w += weight[i] ;
        if ( w > 0 )
        {
            r = weight[i]/w ;
            result = Quaternion::Slerp(result, q[i], r) ;
        }
    }

    return result ;
}


bool LightHouseFusionPrivate::Fuse ( bool Fuse_Pos, bool Fuse_Ori )
{
    if ( Filter() )
    {
        return false ;
    }

    if ( !Fuse_Pos && !Fuse_Ori )
    {
        return false ;
    }

    int i , n=d->stationNum() ;
    int idx = d->latestStationID() - 1 ;
    Float* weight = new Float[n] ;
    Vector3D* pos = new Vector3D[n] ;
    Quaternion* ori = new Quaternion[n] ;

    for ( i=0; i<n; i++ )
    {
        if ( History[i].valid )
        {
            Float dt = d->tsLSB() * ( d->latestTimeStamp() - History[i].TS ) ;
            weight[i] = getHistoryWeight(dt) ;
            pos[i] = History[i].MappedPos ;
            ori[i] = History[i].MappedOri ;
        }
        else
        {
            weight[i] = 0.0 ;
            pos[i] = Vector3D(0,0,0) ;
            ori[i] = Quaternion(1,0,0,0) ;
        }
    }




    Vector3D curFusedP ;
    Quaternion curFusedQ ;
    HistoryData *history, *lastHistory ;
    history = History + idx ;
    lastHistory = this->lastHistory() ;
    /*
     * Convention(s) :
     *      registered & unregistered :
     *          when we got at least two frames of data ( i.e. two history records )
     *          from a certain station,  we say the station is "registered" ,
     *          or it is "unregistered" ;
     *          however there is an exception beyond this rule, the station who
     *          held the first frame of data would be marked as "registered"
     *          immediately when the first frame came .
     *
     * case JKJ1 :  station-switch happened,  and the newest station had been marked
     *              as registered .
     *
     *      |
     *      | ...
     *      | data from Station J ( Jpre - registered )
     *      | ...
     * time |
     * line | data from Station K ( Kpre - the latest registered Station )
     *      |
     *      | ...(other unregistered stations may or may not exist in this area)
     *      |
     *      | data from Station J ( Jcur, which is the newest data )
     *      ↓
     *
     * the fusion result we want is :
     *
     *      *   when Jpre is new, ( of course Kpre should be newer ), we
     *          trust "Jpre.fused+δ1" (as the fusion result) more ;
     *      *   when Jpre is old, while Kpre is new, we trust "Kpre.fused+δ2"
     *          more ( for a smaller shake on the result ) ;
     *      *   when Jpre and Kpre are all old, we trust "Jcur.mapped" more .
     *
     *      δ1 means "Jcur.mapped - Jpre.mapped"
     *      δ2 means " δ1 * (Jcur.TS - Kpre.TS) / (Jcur.TS - Jpre.TS) "
     *
     * case JKJ2 :  station-switch happened,  and the newest station has a history
     *              record but unregistered .
     *
     *      |
     *      | ...
     *      | data from Station J ( Jpre - first frame from Station J, unregistered )
     *      | ...
     * time |
     * line | data from Station K ( Kpre - the latest registered Station )
     *      |
     *      | ...(other unregistered stations may or may not exist in this area)
     *      |
     *      | data from Station J ( Jcur, which is the newest data )
     *      ↓
     *
     * the fusion result we want is :
     *
     *      *   when Kpre is new, we trust "Kpre+δ2" more ( for a smaller
     *          shake on the result ) ;
     *      *   when Kpre is old, we trust "Jcur.mapped" more .
     *
     *      δ1 means "Jcur.mapped - Jpre.mapped"
     *      δ2 means " δ1 * (Jcur.TS - Kpre.TS) / (Jcur.TS - Jpre.TS) "
     *      besides , we would mark Station J as "registered" here .
     *
     * case KJ1 :   station-switch happened,  and the newest station has no
     *              history record ( of course unregistered ) .
     *
     *      |
     *      | ...
     * time |
     * line | data from Station K ( Kpre - the latest registered Station )
     *      |
     *      | ...(other unregistered stations may or may not exist in this area)
     *      |
     *      | data from Station J ( Jcur, which is the newest data, unregistered )
     *      ↓
     *
     *      only record data, no output here .
     *
     * case KJJ1 :  station-switch may happened or not, and the newest station
     *              has a history record but unregistered .
     *
     *      |
     *      | ...
     *      | data from Station K ( Kpre - the latest registered Station )
     * time | ...
     * line | data from Station J ( Jpre - first frame from Station J, unregistered )
     *      |
     *      | ... (other unregistered stations may or may not exist in this area)
     *      |
     *      | data from Station J ( Jcur, which is the newest data )
     *      ↓
     *
     *      we regard this case the same as case JKJ2 .
     *
     * case JJ1 :  station-switch may happened or not, and the newest station
     *              has a history record but unregistered .
     *
     *      |
     * time | ...
     * line | data from Station J ( Jpre - registered )
     *      |
     *      | ... (other unregistered stations may or may not exist in this area)
     *      |
     *      | data from Station J ( Jcur, which is the newest data )
     *      ↓
     *
     * the fusion result we want is :
     *
     *      *   when Jpre is new, we trust "Jpre.fused+δ1" (as the
     *          fusion result) more ;
     *      *   when Jpre is old, we trust "Jcur.mapped" more .
     *
     *      δ1 means "Jcur.mapped - Jpre.mapped"
     *
     *
     *
     * attentions :
     *      when we say "trust xxx more" here, it doesn't mean we trust xxx
     *      completely, we just have a high confidence on xxx . the definite
     *      confidence distribution need a fitting in the program .
     *      and we couldn't sign a data-frame with a 'new' or 'old' label
     *      simply, because there's no fair & clear boundary between 'new'
     *      and 'old', we can only evaluate the 'fresh-degree' of a frame
     *      by its timestamp ;
     *
    */




    if ( NULL == lastHistory )
    {
        // the first frame will be accecpt directly without fusion .
        curFusedP = d->latestMappedP() ;
        curFusedQ = d->latestMappedQ() ;
        history->valid = true ;
        history->registered = true ;    // mark the first station as registered .
    }

    else
    {
        HistoryData* lastRegistered = this->lastRegistered() ;
        Float wLast, wThis;
        wThis  = weight[idx] ;
        wLast  = weight[lastRegisteredIdx()] ;
//if ( idx != lastRegisteredIdx() )
//     if  ( (wThis != 1.0f || wLast != 1.0f) )
//        printf("wThis: %lf\t wLast: %lf\n", wThis, wLast);

        if ( history->valid )
        {
            if ( history->registered )
            {
                if ( Fuse_Pos )
                {
                    Vector3D dP = d->latestMappedP() - history->MappedPos ;
                    Vector3D P = history->FusedPos + dP ;
                    if ( lastRegistered != history )
                    {
#define USE_SOLUTION_A
#ifdef USE_SOLUTION_A
                        // solution A :
                        Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                    (Float)(d->latestTimeStamp() - history->TS) ;
                        dP = dP * r ;
                        Vector3D P1 = lastRegistered->FusedPos + dP ;
                        P = P * wThis  +  P1 * (1-wThis) ;
#else
                        // solution B :
                        P = P * wThis  +  lastRegistered->FusedPos * (1-wThis) ;
#endif
                    }
                    P = P * wLast  +  d->latestMappedP() * (1-wLast) ;
                    curFusedP = P ;
                }

                if ( Fuse_Ori )
                {
                    Quaternion dQ = d->latestMappedQ() * (history->MappedOri.conj()) ;
                    Quaternion Q =  dQ * (history->FusedOri) ;
                    if ( lastRegistered != history )
                    {
#ifdef USE_SOLUTION_A
                        // solution A :
                        Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                    (Float)(d->latestTimeStamp() - history->TS) ;
                        dQ = Quaternion::Slerp(Quaternion(1,0,0,0), dQ, r) ;
                        Quaternion Q1 = dQ * lastRegistered->FusedOri ;
                        Q = Quaternion::Slerp(Q,lastRegistered->FusedOri,(1-wThis)) ;
#else
                        Q = Quaternion::Slerp(Q,lastRegistered->FusedOri,(1-wThis)) ;
#endif
                    }
                    Q = Quaternion::Slerp(Q, d->latestMappedQ(), (1-wLast)) ;
                    Q.normalize();
                    curFusedQ = Q ;
                }
            }
            else
            {
                if ( Fuse_Pos )
                {
#ifdef USE_SOLUTION_A
                    // solution A :
                    Vector3D dP = d->latestMappedP() - history->MappedPos ;
                    Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                (Float)(d->latestTimeStamp() - history->TS) ;
                    dP = dP * r ;
                    Vector3D P = lastRegistered->FusedPos + dP ;
                    P = P * wLast  +  d->latestMappedP() * (1-wLast) ;
                    curFusedP = P ;
#else
                    // solution B :
                    curFusedP = lastRegistered->FusedPos * wLast +  d->latestMappedP() * (1-wLast) ;
#endif
                }


                if ( Fuse_Ori )
                {
#ifdef USE_SOLUTION_A
                    // solution A :
                    Quaternion dQ = d->latestMappedQ() * (history->MappedOri.conj()) ;
                    Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                (Float)(d->latestTimeStamp() - history->TS) ;
                    dQ = Quaternion::Slerp(Quaternion(1,0,0,0), dQ, r) ;
                    Quaternion Q =  dQ * (lastRegistered->FusedOri) ;
                    Q = Quaternion::Slerp(Q, d->latestMappedQ(), (1-wLast)) ;
                    Q.normalize();
                    curFusedQ = Q ;
#else
                    // solution B :
                    curFusedQ = Quaternion::Slerp(lastRegistered->FusedOri, d->latestMappedQ(), (1-wLast)) ;
#endif
                }

                history->registered = true ;
            }
        }

        else
        {
             history->valid = true ;
        }
    }


    if ( history->registered )
    {
        // move the fused result slowly to the target result .
        Float dt = d->tsLSB() * (d->latestTimeStamp() - d->preTimeStamp()) ;
        Float ratio = 1 - exp(-dt/OffsetDampingTime) ;
        if ( Fuse_Pos )
        {
            weight[idx] = 1 ;
            pos[idx] = d->latestMappedP() ;
            Vector3D targetP = weightCenter(weight,pos,n) ;
            PosOffset = targetP - curFusedP ;
            Vector3D dP = PosOffset * ratio ;
            curFusedP = curFusedP + dP ;

            Vector3D oldLhP = d->lhPos()  ;
            Vector3D newLhP ;

            if ( PosSmooth )
                newLhP = oldLhP * (1-SmoothRatio) + curFusedP * SmoothRatio ;
            else
                newLhP = curFusedP ;

            d->setLhPos(newLhP);
        }

        if ( Fuse_Ori )
        {
            weight[idx] = 1 ;
            ori[idx] = d->latestMappedQ() ;
            Quaternion targetQ = weightCenter(weight,ori,n) ;
            OriOffset = targetQ * curFusedQ.conj() ;
            Quaternion dQ = Quaternion::Slerp(Quaternion(1,0,0,0), OriOffset, ratio) ;
            curFusedQ = dQ * curFusedQ ;
            curFusedQ.normalize();

            Quaternion oldLhQ = d->lhQuat()  ;
            Quaternion newLhQ ;
            if ( OriSmooth )
                newLhQ = Quaternion::Slerp ( oldLhQ, curFusedQ, SmoothRatio ) ;
            else
                newLhQ = curFusedQ ;

            newLhQ.normalize();
            d->setLhQuat(newLhQ);
        }
    }

    // record history .
    if ( Fuse_Pos )
    {
        if ( history->registered )
        {
            history->FusedPos = curFusedP ;
            history->FinalLhPos = d->lhPos() ;
        }
        history->MappedPos = d->latestMappedP() ;
    }
    if ( Fuse_Ori )
    {
        if ( history->registered )
        {
            history->FusedOri = curFusedQ ;
            history->FinalLhOri = d->lhQuat() ;
        }
        history->MappedOri = d->latestMappedQ() ;
    }
    history->TS = d->latestTimeStamp() ;


    delete []ori ;
    delete []pos ;
    delete []weight ;

    return history->registered ;
}

bool LightHouseFusionPrivate::Fuse2 ( bool Fuse_Pos, bool Fuse_Ori )
{
    if ( Filter() )
    {
        return false ;
    }

    if ( !Fuse_Pos && !Fuse_Ori )
    {
        return false ;
    }

    int i , n=d->stationNum() ;
    int idx = d->latestStationID() - 1 ;
    Float* weight = new Float[n] ;
    Vector3D* pos = new Vector3D[n] ;
    Quaternion* ori = new Quaternion[n] ;

    for ( i=0; i<n; i++ )
    {
        if ( History[i].valid )
        {
            Float dt = d->tsLSB() * ( d->latestTimeStamp() - History[i].TS ) ;
            weight[i] = getHistoryWeight(dt) ;
            pos[i] = History[i].MappedPos ;
            ori[i] = History[i].MappedOri ;
        }
        else
        {
            weight[i] = 0.0 ;
            pos[i] = Vector3D(0,0,0) ;
            ori[i] = Quaternion(1,0,0,0) ;
        }
    }




    Vector3D curFusedP ;
    Quaternion curFusedQ ;
    HistoryData *history, *lastHistory ;
    history = History + idx ;
    lastHistory = this->lastHistory() ;




    if ( NULL == lastHistory )
    {
        // the first frame will be accecpt directly without fusion .
        curFusedP = d->latestMappedP() ;
        curFusedQ = d->latestMappedQ() ;
        history->valid = true ;
        history->registered = true ;    // mark the first station as registered .
    }

    else
    {
        HistoryData* lastRegistered = this->lastRegistered() ;
        Float wLast, wThis;
        wThis  = weight[idx] ;
        wLast  = weight[lastRegisteredIdx()] ;
//if ( idx != lastRegisteredIdx() )
//     if  ( (wThis != 1.0f || wLast != 1.0f) )
//        printf("wThis: %lf\t wLast: %lf\n", wThis, wLast);

        if ( history->valid )
        {
            if ( history->registered )
            {
                if ( Fuse_Pos )
                {
                    Vector3D dP = d->latestMappedP() - history->MappedPos ;
                    Vector3D P = history->FusedPos + dP ;
                    if ( lastRegistered != history )
                    {
#define USE_SOLUTION_A
#ifdef USE_SOLUTION_A
                        // solution A :
                        Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                    (Float)(d->latestTimeStamp() - history->TS) ;
                        dP = dP * r ;
                        Vector3D P1 = lastRegistered->FusedPos + dP ;
                        P = P * wThis  +  P1 * (1-wThis) ;
#else
                        // solution B :
                        P = P * wThis  +  lastRegistered->FusedPos * (1-wThis) ;
#endif
                    }
                    P = P * wLast  +  d->latestMappedP() * (1-wLast) ;
                    curFusedP = P ;
                }

                if ( Fuse_Ori )
                {
                    Quaternion dQ = d->latestMappedQ() * (history->MappedOri.conj()) ;
                    Quaternion Q =  dQ * (history->FusedOri) ;
                    if ( lastRegistered != history )
                    {
#ifdef USE_SOLUTION_A
                        // solution A :
                        Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                    (Float)(d->latestTimeStamp() - history->TS) ;
                        dQ = Quaternion::Slerp(Quaternion(1,0,0,0), dQ, r) ;
                        Quaternion Q1 = dQ * lastRegistered->FusedOri ;
                        Q = Quaternion::Slerp(Q,lastRegistered->FusedOri,(1-wThis)) ;
#else
                        Q = Quaternion::Slerp(Q,lastRegistered->FusedOri,(1-wThis)) ;
#endif
                    }
                    Q = Quaternion::Slerp(Q, d->latestMappedQ(), (1-wLast)) ;
                    Q.normalize();
                    curFusedQ = Q ;
                }
            }
            else
            {
                if ( Fuse_Pos )
                {
#ifdef USE_SOLUTION_A
                    // solution A :
                    Vector3D dP = d->latestMappedP() - history->MappedPos ;
                    Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                (Float)(d->latestTimeStamp() - history->TS) ;
                    dP = dP * r ;
                    Vector3D P = lastRegistered->FusedPos + dP ;
                    P = P * wLast  +  d->latestMappedP() * (1-wLast) ;
                    curFusedP = P ;
#else
                    // solution B :
                    curFusedP = lastRegistered->FusedPos * wLast +  d->latestMappedP() * (1-wLast) ;
#endif
                }


                if ( Fuse_Ori )
                {
#ifdef USE_SOLUTION_A
                    // solution A :
                    Quaternion dQ = d->latestMappedQ() * (history->MappedOri.conj()) ;
                    Float r = (Float)(d->latestTimeStamp() - lastRegistered->TS) /
                                (Float)(d->latestTimeStamp() - history->TS) ;
                    dQ = Quaternion::Slerp(Quaternion(1,0,0,0), dQ, r) ;
                    Quaternion Q =  dQ * (lastRegistered->FusedOri) ;
                    Q = Quaternion::Slerp(Q, d->latestMappedQ(), (1-wLast)) ;
                    Q.normalize();
                    curFusedQ = Q ;
#else
                    // solution B :
                    curFusedQ = Quaternion::Slerp(lastRegistered->FusedOri, d->latestMappedQ(), (1-wLast)) ;
#endif
                }

                history->registered = true ;
            }
        }

        else
        {
             history->valid = true ;
        }
    }


    if ( history->registered )
    {
        // move the fused result slowly to the target result .
        Float dt = d->tsLSB() * (d->latestTimeStamp() - d->preTimeStamp()) ;
        Float ratio = 1 - exp(-dt/OffsetDampingTime) ;
        if ( Fuse_Pos )
        {
            weight[idx] = 1 ;
            pos[idx] = d->latestMappedP() ;
            Vector3D targetP = weightCenter(weight,pos,n) ;
            PosOffset = targetP - curFusedP ;
            Vector3D dP = PosOffset * ratio ;
            curFusedP = curFusedP + dP ;

            Vector3D oldLhP = d->lhPos()  ;
            Vector3D newLhP ;

            if ( PosSmooth )
                newLhP = oldLhP * (1-SmoothRatio) + curFusedP * SmoothRatio ;
            else
                newLhP = curFusedP ;

            d->setLhPos(newLhP);
        }

        if ( Fuse_Ori )
        {
            weight[idx] = 1 ;
            ori[idx] = d->latestMappedQ() ;
            Quaternion targetQ = weightCenter(weight,ori,n) ;
            OriOffset = targetQ * curFusedQ.conj() ;
            Quaternion dQ = Quaternion::Slerp(Quaternion(1,0,0,0), OriOffset, ratio) ;
            curFusedQ = dQ * curFusedQ ;
            curFusedQ.normalize();

            Quaternion oldLhQ = d->lhQuat()  ;
            Quaternion newLhQ ;
            if ( OriSmooth )
                newLhQ = Quaternion::Slerp ( oldLhQ, curFusedQ, SmoothRatio ) ;
            else
                newLhQ = curFusedQ ;

            newLhQ.normalize();
            d->setLhQuat(newLhQ);
        }
    }

    // record history .
    if ( Fuse_Pos )
    {
        if ( history->registered )
        {
            history->FusedPos = curFusedP ;
            history->FinalLhPos = d->lhPos() ;
        }
        history->MappedPos = d->latestMappedP() ;
    }
    if ( Fuse_Ori )
    {
        if ( history->registered )
        {
            history->FusedOri = curFusedQ ;
            history->FinalLhOri = d->lhQuat() ;
        }
        history->MappedOri = d->latestMappedQ() ;
    }
    history->TS = d->latestTimeStamp() ;


    delete []ori ;
    delete []pos ;
    delete []weight ;

    return history->registered ;


}



LightHouseFusion::LightHouseFusion ( LightHousePOTPrivate*d )
{
    p = NULL ;
    if ( d )
    {
        p = new LightHouseFusionPrivate(d) ;
    }
}

LightHouseFusion::~LightHouseFusion ()
{
    if (p)
        delete p ;
}


bool LightHouseFusion::Fuse ( bool Fuse_Pos, bool Fuse_Ori )
{
    return p->Fuse(Fuse_Pos, Fuse_Ori);
}

void LightHouseFusion::reset ()
{
    p->reset();
}

void LightHouseFusion::setPosSmooth(bool smooth)
{
    p->PosSmooth=smooth;
}
void LightHouseFusion::setOriSmooth(bool smooth)
{
    p->OriSmooth=smooth;
}

void LightHouseFusion::allocResource()
{
    p->allocResource();
}

void LightHouseFusion::releaseResource()
{
    p->releaseResource();
}



LightHouseFusionPrivate* LightHouseFusion::Priv() const
{
    return p ;
}


