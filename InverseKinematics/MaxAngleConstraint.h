#ifndef __InverseKinematics__MaxAngleConstraint__
#define __InverseKinematics__MaxAngleConstraint__

#include "MathToolsCpp/Quaternion.h"
#include "InverseKinematics/IKConstraint.h"

class MaxAngleConstraint : public IKConstraint
{

public:
    struct Param {
        float maxAngle ;    // unit is degree , not rad .
    };

public:
    MaxAngleConstraint(){
        IKConstraintTypeMagic = 1 ;
    }
//    MaxAngleConstraint(float angle){
//        IKConstraintTypeMagic = 1 ;
//        maxAngle=angle;
//    }
//    void setMaxAngle(float angle) {maxAngle=angle;}
//    float getMaxAngle() {return maxAngle;}
public:
    virtual void runFWDConstraint();
    virtual void runBWDConstraint();

    virtual void setParam(void*param) ;

private:
    Param param ;

};

#endif
