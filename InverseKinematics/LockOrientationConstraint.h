#ifndef LOCKORIENTATIONCONSTRAINT_H
#define LOCKORIENTATIONCONSTRAINT_H

#include <cmath>
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "InverseKinematics/IKConstraint.h"

class LockOrientationConstraint : public IKConstraint
{
public:
    struct ConstraintParam {
        int i;
        Quaternion Yaw;
    };
    LockOrientationConstraint(){
        IKConstraintTypeMagic = 3 ;
    }

private:
    ConstraintParam param ;
 public:
    virtual void runFWDConstraint();
    virtual void runBWDConstraint();
    virtual void setParam(void *param) ;
    Quaternion getParam(){return param.Yaw;}
};

#endif // LOCKORIENTATIONCONSTRAINT_H

/*  CALL
LockRotationConstraint::ConstraintParam param ;
param.Otheta[0]=0;
LockRotationConstraint c;
c.setParam(&param);ss
*/
