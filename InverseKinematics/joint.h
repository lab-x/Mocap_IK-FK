//
//  Joint.h
//  InverseKinematics
//
//  Created by MengTsao on 2/11/15.
//  Copyright (c) 2015 MengTsao. All rights reserved.
//

#ifndef __InverseKinematics__Joint__
#define __InverseKinematics__Joint__

#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include "InverseKinematics/IKConstraint.h"
#include "InverseKinematics/LockQuaternionConstraint.h"
#include "InverseKinematics/MaxAngleConstraint.h"
#include "InverseKinematics/LockOrientationConstraint.h"
#include "InverseKinematics/UpperArmConstraint.h"
#include "InverseKinematics/LegConstraint.h"

class Fabrik ;
class Joint {
    Position position;
    Quaternion QwFWD;
    Quaternion QwBWD;

    IKConstraint * Constraint;
    Fabrik* fabrik ;
    int id ;
public:
    void setQwFWD(Quaternion q) {QwFWD=q;}
    void setQwBWD(Quaternion q) {QwBWD=q;}
    Quaternion getQwFWD(){return QwFWD;}
    Quaternion getQwBWD(){return QwBWD;}
    void setFabrik(Fabrik*f){fabrik=f;}
    Fabrik* getFabrik(){return fabrik;}
    void setID(int i){id=i;}
    int getID(){return id;}

    enum ConstraintType {
        UnKnownConstraint,
        NoConstraint,
        LockQuaternion,
        MaxAngle,
        LockOrientation,
        UpperArm,
        Leg
    } constraintType ;

    void addConstraint(ConstraintType Type, void*param) ;
    void addConstraint(IKConstraint*c) ;
    void setConstraintParam(void*param);

    void removeConstraint() ;

    IKConstraint * getConstraint() {
        return Constraint;
    }

    Joint *getFWDJoint() ;
    Joint *getBWDJoint() ;


//call public :
public:
    Position getPosition();
    void setPosition(Position rhs);

public:
    Joint();
    Joint(Position P);

    // TODO: these two copy-constructors don't handle the dynamic
    // pointers (FWDConstraint and BWDConstraint) and 'fabrik' and
    // ConstraintTypes .
    Joint(Joint Joint[]);
    Joint(const Joint& other );

    ~Joint() {
        if(Constraint) delete Constraint;
    }
    Joint& operator=(const Joint& rhs);
    
};

#endif /* defined(__InverseKinematics__Joint__) */






