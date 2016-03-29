#ifndef __InverseKinematics__MocapIMU__
#define __InverseKinematics__MocapIMU__

#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Matrix.h"
#include "memsnode.h"
#include "stdint.h"

class MocapIMU
{
    MemsNode Nodes[17];
    Quaternion QW[17];
    Quaternion QL[17];
    Vector3D EulerOut[17];
    Position HipPosition;

public:

    bool isSlaveIDValid (int8_t id)     { return id>0 && id<18 ;}
    MemsNode* getSlaveNode(int8_t id)   { return ( isSlaveIDValid(id) ? &(Nodes[id-1]) : NULL ) ; }

public:
    MocapIMU();
    void calibrate();
    void compute();
    void genQW();
    void genQL();
    void genEuler();
    bool CalibrationDone() ;

};

#endif
