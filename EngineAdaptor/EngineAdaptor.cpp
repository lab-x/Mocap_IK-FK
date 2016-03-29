
#include "EngineAdaptor.h"

static EngineAdaptor::EngineType GlobalEngine ;


void EngineAdaptor::setCurEngine(EngineType engine)
{
    GlobalEngine = engine ;
}

EngineAdaptor::EngineType EngineAdaptor::getCurEngine()
{
    return GlobalEngine ;
}


/******************************************************
 * System Quaternion to Game-Engine EulerAngle .
 **/

static EulerAngle SysQuat2_UNITY_Euler(const Quaternion & q)
{
    return q.toEuler().RadToDegree() ;
}
static EulerAngle SysQuat2_UNREAL_Euler(const Quaternion & q)
{
    Quaternion qAux (q.getW(), q.getX(), q.getZ(), -q.getY()) ;
    EulerAngle eAux = qAux.toEuler(EulerOrder_XYZ).RadToDegree() ;
    return EulerAngle(-eAux.getYaw(), eAux.getPitch(), -eAux.getRoll()) ;
}
EulerAngle EngineAdaptor::SysQuat2EngineEuler (const Quaternion & q)
{
    switch (getCurEngine())
    {
    case UNITY:
        return SysQuat2_UNITY_Euler(q) ;
    case UNREAL:
        return SysQuat2_UNREAL_Euler(q) ;
    default:
        return EulerAngle(0,0,0) ;
    }
}
/********************************************************/




