//
//  fabrik.h
//  InverseKinematics
//
//  Created by MengTsao on 2/11/15.
//  Copyright (c) 2015 MengTsao. All rights reserved.
//

#ifndef __InverseKinematics__fabrik__
#define __InverseKinematics__fabrik__

#include <stdio.h>
#include "InverseKinematics/Joint.h"
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"



#define FabrikMaxJoins (4)
#define FabrikMaxBones (FabrikMaxJoins-1)
class Fabrik {
    int ChainID; //HeadNeckChestHip:0  LeftArm:1  RightArm:2  LeftLeg:3  RightLeg:4
    Joint goal;
    Joint root;
    Joint initJoints[FabrikMaxJoins];
    Joint joints[FabrikMaxJoins];
    float d[FabrikMaxBones];
    Vector3D initialBone[FabrikMaxBones] ;
    Vector3D bone[FabrikMaxBones];
    Quaternion QW[FabrikMaxBones];
    Quaternion QL[FabrikMaxBones];
    EulerAngle Euler[FabrikMaxBones];
    float tol;
    float epsilon;
    int nJoints ;

    Fabrik* parent ;
    Fabrik* child ;
    Fabrik* left ;
    Fabrik* right ;
    float backwardDiff ;
    float preBackwardDiff ;
    bool isIterationFinished();
    void initIteration();
    void output();
public:
    void setParent(Fabrik*_parent) {parent=_parent;}
    Fabrik* getParent() {return parent;}
    Fabrik* getTopParent() {
        Fabrik*top=this;
        while(top->parent)
            top = top->parent ;
        return top ;
    }
    void computeNEW() ;
    Vector3D* getInitBones(){return initialBone;}
    Vector3D* getBones(){return bone;}

    Quaternion* getQL() {return QL;}
    int getChainID() const {return ChainID;}
    void addChild(Fabrik*newChild)
    {
        Fabrik* cur = child ;
        if (newChild)
        {
            newChild->parent = this ;
            newChild->right = NULL ;
            if (!child)
            {
                newChild->left = NULL ;
                child = newChild ;
            }
            else
            {
                while(cur->right) cur = cur->right ;
                cur->right = newChild ;
                newChild->left = cur ;
            }
        }
    }

    void removeChild(Fabrik*badChild)
    {
        Fabrik* cur = child ;
        if ( !child || !badChild )
            return ;

        if (badChild == child)
        {
            if (child->right)
                child->right->left = NULL ;
            child = child->right ;
            badChild->left = badChild->right = NULL ;
            return ;
        }

        while ( cur )
            if ( cur != badChild )
                cur = cur->right ;
        if (cur)
        {
            if (cur->right)
                cur->right->left = cur->left ;
            cur->left->right = cur->right ;
            badChild->left = badChild->right = NULL ;
        }
    }

private:
    void runForward();
    void runBackward(Position b);

public:
    Fabrik() {parent=child=left=right=NULL;}
    Fabrik(int ID, float tolerance, float eps);

    void setInitJoints(Joint joints[], int nJoints);
    void setInitJoints(Joint one, Joint two);
    void setInitJoints(Joint one, Joint two, Joint three);
    void setInitJoints(Joint one, Joint two, Joint three, Joint four);

    void setJoints(Joint joints[], int nJoints);
    void setJoints(Joint one, Joint two);
    void setJoints(Joint one, Joint two, Joint three);
    void setJoints(Joint one, Joint two, Joint three, Joint four);
    void setGoal(Joint x);
    void setRoot(Joint x);

    Joint* getJoints();
    Joint* getInitJoints();

    EulerAngle* getEulers();
    int getJointsCount (){return nJoints;}
    int getBonesCount () {return nJoints-1;}

    void shrinkEnd();

    void GenBones();
    void GenQW();
    void GenQL();
    void GenEuler();
};


#endif /* defined(__InverseKinematics__fabrik__) */

