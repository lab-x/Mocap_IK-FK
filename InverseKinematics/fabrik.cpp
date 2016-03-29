//
//  fabrik.cpp
//  InverseKinematics
//
//  Created by MengTsao on 2/11/15.
//  Copyright (c) 2015 MengTsao. All rights reserved.
//


#include <iostream>
#include <fstream>
#include <cmath>
#include <stdlib.h>

#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Quaternion.h"
#include "InverseKinematics/Joint.h"
#include "InverseKinematics/fabrik.h"

using std::cout ;
using std::endl ;

Fabrik::Fabrik(int ID, float tolerance, float eps) {
    parent=child=left=right=NULL;
    ChainID = ID;
    tol = tolerance;
    epsilon = eps; //Error for out of bounds correction
}
void Fabrik::setGoal(Joint x) {
    goal = x;
}
void Fabrik::setRoot(Joint x) {
    root = x;
}
void Fabrik::setJoints(Joint inpJoints[], int n)
{
    int i ;
    nJoints = n ;
    for (i=0;i<getJointsCount();i++)
    {
        joints[i] = inpJoints[i] ;
        joints[i].setFabrik(this);
        joints[i].setID(i);
    }

    for (i=0;i<getBonesCount();i++)
    {
        initialBone[i] = Vector3D ( joints[i+1].getPosition() - joints[i].getPosition() ) ;
        bone[i] = initialBone[i] ;
        d[i] = initialBone[i].getLength() ;
        QW[i] = Quaternion(1,0,0,0);
        QL[i] = Quaternion(1,0,0,0);
        Euler[i] = EulerAngle(0,0,0);
    }
}
void Fabrik::setInitJoints(Joint inpJoints[], int n)
{
    int i ;
    for (i = 0; i < n; i++)         //for (i=0;i<getJointsCount();i++)
    {
        initJoints[i] = inpJoints[i] ;
    }
}
void Fabrik::setJoints(Joint one, Joint two, Joint three, Joint four) {

    Joint inpJoints[4] ;
    inpJoints[0] = one ;
    inpJoints[1] = two ;
    inpJoints[2] = three ;
    inpJoints[3] = four ;

    setJoints(inpJoints, 4) ;
}
void Fabrik::setInitJoints(Joint one, Joint two, Joint three, Joint four) {

    Joint inpJoints[4] ;
    inpJoints[0] = one ;
    inpJoints[1] = two ;
    inpJoints[2] = three ;
    inpJoints[3] = four ;

    setInitJoints(inpJoints, 4) ;
}
void Fabrik::setJoints(Joint one, Joint two, Joint three) {
    Joint inpJoints[3];
    inpJoints[0] = one;
    inpJoints[1] = two;
    inpJoints[2] = three;
    setJoints(inpJoints, 3);
}
void Fabrik::setInitJoints(Joint one, Joint two, Joint three) {
    Joint inpJoints[3];
    inpJoints[0] = one;
    inpJoints[1] = two;
    inpJoints[2] = three;
    setInitJoints(inpJoints, 3);
}
void Fabrik::setJoints(Joint one, Joint two) {
    Joint inpJoints[2];
    inpJoints[0] = one;
    inpJoints[1] = two;
    setJoints(inpJoints, 2);
}
void Fabrik::setInitJoints(Joint one, Joint two) {
    Joint inpJoints[2];
    inpJoints[0] = one;
    inpJoints[1] = two;
    setInitJoints(inpJoints, 2);
}

Joint* Fabrik::getJoints() {
    return joints;
}
Joint* Fabrik::getInitJoints() {
    return initJoints;
}
EulerAngle* Fabrik::getEulers() {
    return Euler;
}

//Constraints Mentioned in Paper. May not be correctly implemented.
void Fabrik::shrinkEnd() {
    // Fixes the out-of-reach problem.  Forces the last link to maintain its length,
    // going as close as possible toward the goal.    int n = getJointsCount() ;
    int n = getJointsCount() ;
    float dCalc = (joints[n-1].getPosition() - joints[n-2].getPosition()).getLength();
    float dExact = d[n-2];

    if (dCalc - dExact > 0.0001 || dCalc - dExact < -0.0001) {
        Position vector(goal.getPosition() - joints[n-2].getPosition());
        double factor = dExact / vector.getLength();
        joints[n-1].setPosition(joints[n-2].getPosition()+vector*factor);
        goal = joints[n-1];
    }
}

//BVH Generator
void Fabrik::GenBones(){
    int n = getBonesCount() ; //number of bones
    for(int i = 0; i<n; i++){
        bone[i] = Vector3D ( joints[i+1].getPosition()-joints[i].getPosition() ) ;
    }
}

void Fabrik::GenQW(){
    int n = getBonesCount() ; //number of bones

    for(int i = 0; i<n; i++){
        QW[i] = joints[i].getQwBWD() ;
    }
}


void Fabrik::GenQL() {
    int n = getBonesCount() ; //number of bones
    QL[0] = QW[0];
    for(int i = 1; i<n; i++){
        Quaternion invQParent = QW[i-1].conj() ;
        QL[i] = invQParent * QW[i];
    }
}
void Fabrik::GenEuler(){
    int n = getBonesCount() ; //number of bones
    float RtoD = 180.0/3.1415926;
    for(int i=0; i<n; i++){
        Euler[i] = QL[i].toEuler().RadToDegree() ;
    }
}



#define UseDiffQInFabrik
void Fabrik::runForward()
{
    int i;
    int n = getJointsCount() ;
    float lambda, r;

    GenBones();

    Fabrik *cur = child ;
    if ( child )
    {
        Position gPos(0,0,0);
        int nChild = 0 ;
        while (cur)
        {
            cur->runForward();
            gPos += cur->joints[0].getPosition() ;
            nChild ++ ;
            cur = cur->right ;
        }
        gPos = gPos / nChild ;
        goal.setPosition(gPos);
    }

    //// TODO: how to constraint the parent's end joint with the childs' root ?
    ////
    //////////////

    joints[n-1].setPosition(goal.getPosition()); //-Position(0.00001,0.00001,0.00001));
    for(i = n-2; i>=0; i--){
        r = (joints[i+1].getPosition() - joints[i].getPosition()).getLength();
        lambda = d[i]/r;
        Position P = ((1-lambda) * joints[i+1].getPosition() + lambda * joints[i].getPosition());
        joints[i].setPosition(P);

        Position dP = joints[i+1].getPosition() - joints[i].getPosition() ;

#ifdef UseDiffQInFabrik
        Quaternion qw = Quaternion::v2q(
                    bone[i].getNormalized(),
                    dP.getNormalized() );
        Quaternion qold = joints[i+1].getQwFWD() ;
        joints[i+1].setQwFWD(qw*qold);
        joints[i].setQwBWD(qw*qold);
#else
        Quaternion qw = Quaternion::v2q(
                    Vector3D::Normalize(initialBone[i]),
                    Vector3D::Normalize(Vector3D(dP)) );
        joints[i+1].setQwFWD(qw);
        joints[i].setQwBWD(qw);
#endif

        if ( joints[i+1].getConstraint() )
            joints[i+1].getConstraint()->runFWDConstraint ();
    }
}

void Fabrik::runBackward(Position b)
{
    int i;
    int n = getJointsCount() ;
    float lambda, r;
    Fabrik *cur = child ;

    GenBones();

    joints[0].setPosition(b);
    for(i = 0; i<=n-2; i++){
        r = (joints[i+1].getPosition() - joints[i].getPosition()).getLength();
        lambda = d[i]/r;
        Position P = (1-lambda)*joints[i].getPosition() + lambda* joints[i+1].getPosition();
        joints[i+1].setPosition(P);
        Position dP = joints[i+1].getPosition() - joints[i].getPosition() ;

#ifdef UseDiffQInFabrik

        Quaternion qw = Quaternion::v2q(
                    bone[i].getNormalized(),
                    dP.getNormalized() );
        Quaternion qold = joints[i].getQwBWD() ;
        joints[i+1].setQwFWD(qw*qold);
        joints[i].setQwBWD(qw*qold);
#else
        Quaternion qw = Quaternion::v2q(
                    Vector3D::Normalize(initialBone[i]),
                    Vector3D::Normalize(Vector3D(dP)) );
        joints[i+1].setQwFWD(qw);
        joints[i].setQwBWD(qw);
#endif

        if ( joints[i].getConstraint() )
            joints[i].getConstraint()->runBWDConstraint ();
    }


    if ( child )
    {
        Quaternion qw = joints[n-1].getQwFWD();
        while (cur)
        {
            //// TODO: how to constraint the child's root joint with the parent's end ?
            ////
            //////////////
            cur->getJoints()[0].setQwFWD(qw);


            cur->runBackward(joints[n-1].getPosition());
            cur = cur->right ;
        }
    }
    else
    {
        preBackwardDiff = backwardDiff ;
        backwardDiff = (joints[n-1].getPosition() - goal.getPosition()).getLength() ;
    }
}

bool Fabrik::isIterationFinished ()
{
    Fabrik* cur = child ;
    if (child)
    {
        while (cur)
        {
            if ( cur->isIterationFinished() )
                cur = cur->right ;
            else
                return false ;
        }
        return true ;
    }
    else
    {
        if ( backwardDiff < 0.01 || preBackwardDiff - backwardDiff < 0.0005 )
            return true ;
        else
            return false ;
    }
}

void Fabrik::initIteration()
{
    Fabrik* cur = child ;
    if (child)
    {
        while (cur)
        {
            cur->initIteration() ;
            cur = cur->right ;
        }
    }
    else
    {
        backwardDiff = 10000 ;
        preBackwardDiff = 15000 ;
    }
}

void Fabrik::output()
{
    GenBones();
    GenQW();
    GenQL();
    GenEuler();

    Fabrik* cur = child ;
    while (cur)
    {
        cur->output();
        cur = cur->right ;
    }
}

void  Fabrik::computeNEW() {

    int iter = 0 ;
    Position b = joints[0].getPosition();

    if ( parent )
        return ;    // only the top fabrik (who has no parent) can call this method .
    // TODO: how to handle the UNEACHABLE cases ?
    if (fabs(backwardDiff)>50)
    {
        backwardDiff = backwardDiff ;
    }

    initIteration() ;
    while ( ! isIterationFinished () && iter < 100 )
    {
        iter ++ ;
        runForward () ;
        runBackward (b) ;
    }

////printf("iter %d->\n",iter);
//if (child)
//{
//    //printf("backwardDiff1 %lf\n",child->backwardDiff);
//    //printf("backwardDiff2 %lf\n",child->right->backwardDiff);
//}
//else
//{
//    //printf("backwardDiff0 %lf\n",backwardDiff);
//}

    output() ;
}



