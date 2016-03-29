#ifndef __InverseKinematics__LockQuaternionConstraint__
#define __InverseKinematics__LockQuaternionConstraint__

#include "MathToolsCpp/Quaternion.h"
#include "InverseKinematics/IKConstraint.h"


// LockQuaternionConstraint :
// this type of constraint only should be used to the end joint or root joint .
class LockQuaternionConstraint : public IKConstraint
{

public:
    struct Param {
        Quaternion q ;
    };

public:
    LockQuaternionConstraint() {
        IKConstraintTypeMagic = 2 ;
    }
public:
    virtual void runFWDConstraint();
    virtual void runBWDConstraint();

    virtual void setParam(void*param) ;


private:
    Param param ;
};


#endif
