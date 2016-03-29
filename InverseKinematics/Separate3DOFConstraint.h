#ifndef SEPARATE3DOFCONSTRAINT_H
#define SEPARATE3DOFCONSTRAINT_H


#include <cmath>
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "InverseKinematics/IKConstraint.h"


class Separate3DOFConstraint : public IKConstraint
{
public:
    struct DOF {
        bool isTotalFree ;
        Float minAngle ;
        Float maxAngle ;
    };

    struct Param {
        Vector3D Axis[3] ;
        DOF DOFs[3] ;
        Vector3D invAxis[3] ;
    };


public:
    Separate3DOFConstraint() {
        IKConstraintTypeMagic = 6 ;
    }
    ~Separate3DOFConstraint(){}
    virtual void runFWDConstraint();
    virtual void runBWDConstraint();
    virtual void setParam(void*param) ;

private:
    Param param ;

    Param BWDParam ;
    Param FWDParam ;
    bool paramLegal ;

    static void constraintQuat(Quaternion& Q, const Param& param)  ;
    static void constraintEuler(EulerAngle& E, const Param& param)  ;
    static void setInvAxis ( Param& param )  ;
    static Vector3D mapVector ( const Vector3D& Vec, const Vector3D* axis )  ;
};

#endif // SEPARATE3DOFCONSTRAINT_H
