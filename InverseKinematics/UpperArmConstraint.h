#ifndef UPPERARMCONSTRAINT_H
#define UPPERARMCONSTRAINT_H

#include <cmath>
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "InverseKinematics/IKConstraint.h"

class UpperArmConstraint : public IKConstraint
{
public:
    struct Param {
        Direction direction;
        Quaternion q;
    };

    UpperArmConstraint(){
        IKConstraintTypeMagic = 4 ;
    }
private:
    Param param ;
 public:
    virtual void runFWDConstraint();
    virtual void runBWDConstraint();
    virtual void setParam(void *param) ;
};

#endif // UPPERARMCONSTRAINT_H

/*  CALL
UpperArmConstraint::ConstraintParam param ;
param.Otheta[0]=0;
UpperArmConstraint UAC;
UAC.setParam(&param);
*/
