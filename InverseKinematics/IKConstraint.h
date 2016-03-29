#ifndef __InverseKinematics__IKConstraint__
#define __InverseKinematics__IKConstraint__

class Joint ;
class IKConstraint
{
protected:
    int IKConstraintTypeMagic ;
    Joint* joint ;

public :

    IKConstraint():IKConstraintTypeMagic(0) {joint=0;}
    IKConstraint(Joint*_joint):IKConstraintTypeMagic(0) {joint=_joint;}
    virtual ~IKConstraint(){}
    void setJoint(Joint* _joint){joint=_joint;}
    Joint* getJoint(){return joint;}
    int getMagic(){return IKConstraintTypeMagic;}

public :
    enum Side{
        Left = 0x1,
        Right = 0x2
    };

    enum Direction {
        FWD = 0x1,
        BWD = 0x2,
        FWD_BWD  = FWD | BWD
    } ;

public :
    virtual void runFWDConstraint();
    virtual void runBWDConstraint();
    virtual void setParam(void*param);
};

#endif
