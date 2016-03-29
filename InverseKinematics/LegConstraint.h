#ifndef LEGCONSTRAINT_H
#define LEGCONSTRAINT_H
#include <cmath>
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "InverseKinematics/IKConstraint.h"

class LegConstraint : public IKConstraint
{
public:
    struct Param {
        Side side;
        Quaternion q;
    };
    LegConstraint(){
        IKConstraintTypeMagic = 5 ;
    }
private:
    Param param ;
 public:
    virtual void runFWDConstraint();
    virtual void runBWDConstraint();
    virtual void setParam(void *param) ;
};

#endif // LEGCONSTRAINT_H
