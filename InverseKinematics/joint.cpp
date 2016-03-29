//
//  Joint.cpp
//  InverseKinematics
//
//  Created by MengTsao on 2/11/15.
//  Copyright (c) 2015 MengTsao. All rights reserved.
//

#include "InverseKinematics/Joint.h"
#include "InverseKinematics/fabrik.h"


Joint::Joint(){
    position = Position();
    Constraint = NULL ;
    constraintType = NoConstraint ;
    fabrik = NULL ;
    id = 0 ;
}

Joint::Joint(Position P) {
    position = P;
    Constraint = NULL ;
    constraintType = NoConstraint ;
    fabrik = NULL ;
    id = 0 ;

}

Joint::Joint(Joint Joint[]) {
    position = Joint->position;
    Constraint = NULL ;
    constraintType = NoConstraint ;
    fabrik = NULL ;
    id = 0 ;

}

Joint::Joint( const Joint& other ) {
    position = other.position;
    Constraint = NULL ;
    constraintType = NoConstraint ;
    fabrik = NULL ;
    id = 0 ;

}


Joint& Joint::operator=(const Joint& rhs) {
        this->position = rhs.position;
    return *this;
}

Position Joint::getPosition(){
    return this->position;
}

void Joint::setPosition(Position rhs){
    this->position = rhs;
}


void Joint::addConstraint(ConstraintType Type, void*param)
{
    switch (Type)
    {
    case NoConstraint:
        removeConstraint();
        constraintType = Type ;
        break;
    case LockQuaternion:
        if ( constraintType != Type )
        {
            if ( Constraint )
                delete Constraint ;
            Constraint = new LockQuaternionConstraint() ;
        }
        Constraint->setParam (param);
        Constraint->setJoint(this);
        constraintType = Type ;
        break;
    case MaxAngle:
        if ( constraintType != Type )
        {
            if ( Constraint )
                delete Constraint ;
            Constraint = new MaxAngleConstraint() ;
        }
        Constraint->setParam (param);
        Constraint->setJoint(this);
        constraintType = Type ;
        break;
    case LockOrientation:
        if ( constraintType != Type )
        {
            if ( Constraint )
                delete Constraint ;
            Constraint = new LockOrientationConstraint() ;
        }
        Constraint->setParam (param);
        Constraint->setJoint(this);
        constraintType = Type ;
        break;

    case UpperArm:
        if ( constraintType != Type )
        {
            if ( Constraint )
                delete Constraint ;
            Constraint = new UpperArmConstraint() ;
        }
        Constraint->setParam (param);
        Constraint->setJoint(this);
        constraintType = Type ;
        break;
    case Leg:
        if ( constraintType != Type )
        {
            if ( Constraint )
                delete Constraint ;
            Constraint = new LegConstraint() ;
        }
        Constraint->setParam (param);
        Constraint->setJoint(this);
        constraintType = Type ;
        break;

    default:
        constraintType = UnKnownConstraint ;
        return;
    }
}

void Joint::addConstraint(IKConstraint*c)
{
    if (c)
    {
        c->setJoint(this);
        Constraint = c ;
        constraintType = UnKnownConstraint ;
    }
}


Joint* Joint::getFWDJoint()
{
    if ( 0 == id )
        return NULL ;

    int i = id - 1 ;
    return fabrik->getJoints()+i;
}

Joint* Joint::getBWDJoint()
{
    if ( id == fabrik->getJointsCount()-1 )
        return NULL ;

    int i = id + 1 ;
    return fabrik->getJoints()+i;
}

void Joint::setConstraintParam(void*param)
{
    Constraint->setParam(param);
}


void Joint::removeConstraint() {
    if(Constraint)
        delete Constraint;
    Constraint = NULL ;
    constraintType = NoConstraint ;
}

