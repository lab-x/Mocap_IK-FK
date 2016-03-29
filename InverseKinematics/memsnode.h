#ifndef __InverseKinematics__MemsNode__
#define __InverseKinematics__MemsNode__

#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"

class MemsNode
{
    int ID;
    int PID;
    int isCali;
    Quaternion Qinit;
    Quaternion QCali;
    Quaternion Qraw;
    Quaternion Traw;
    Quaternion Araw;
    Quaternion QW;
    Quaternion QL;
    EulerAngle Euler;

public:
    MemsNode();
    MemsNode(int ID, int PID);
    int getPID();
    int getCaliFlag();
    void setCaliFlag(int flag);
    Quaternion getQraw();
    void setQraw(Quaternion qRaw);
    Quaternion getQT();
    void setQT(Quaternion qT);
    Quaternion getQA();
    void setQA(Quaternion qA);
    Quaternion getQinit();
    void setQinit(Quaternion init);
    Quaternion getQCali();
    void setQCali(Quaternion cali);
    Quaternion getQW();
    void setQW(Quaternion qw);
    Quaternion getQL();
    void setQL(Quaternion ql);
    EulerAngle getEuler();
    void setEuler(EulerAngle angles);
};

#endif
