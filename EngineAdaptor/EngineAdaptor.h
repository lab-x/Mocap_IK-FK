#ifndef ENGINEADAPTOR_H
#define ENGINEADAPTOR_H

#include "MathToolsCpp/Quaternion.h"
class EngineAdaptor
{

public:

    enum EngineType {
        UNITY,
        UNREAL
    } ;
    static void setCurEngine(EngineType engine) ;
    static EngineType getCurEngine() ;
    static EulerAngle SysQuat2EngineEuler (const Quaternion & q) ;


private:
    // a private constructor to prevent an unexpected instance of
    // this STATIC class .
    EngineAdaptor(){}
};

#endif // ENGINEADAPTOR_H
