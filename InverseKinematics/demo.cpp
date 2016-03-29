//
//  main.cpp
//  InverseKinematics
//
//  Created by MengTsao on 2/11/15.
//  Copyright (c) 2015 MengTsao. All rights reserved.
//
#include <iostream>
#include <vector>
#include <fstream>
#include <cmath>
#include "joint.h"
#include "fabrik.h"
#include "Kquaternion.h"
#include "vector3.h"

#define PI 3.14159265
float posX = 2.0f;
float posY = 2.0f;
float posZ = 0.0f;
float fov = -40.0f;
float rotx = 168;
float roty = 180;

int jointNum = 0;
float tolerance = 0.01;
float moveStep = 0.2;
int ID = 0;
float epsilon = moveStep * 10;
float goal[3]; 
std::vector<float> endPosition[3];

//****************************************************
// Global Variables
//****************************************************
Fabrik fabrik(ID, tolerance, epsilon);

void generateLinks() {
    /*
    LenNeckToShoulder = 0.2f ;
    LenShoulderToElbow = 0.2f ;
    LenElbowToWrist = 0.2f ;
    */
    Position p0(0.0001, 0.0001, 10);
    Position p1(10, 0, 10);
    Position p2(20, 0, 10);
    Position p3(30, 0, 10);
    //Position goal(10, 10, -10);
    
    Position goal(50, 10, 0);
    Axes AxesInit;
  
    Joint P0(p0, AxesInit, AxesInit);
    Joint P1(p1, AxesInit, AxesInit);
    Joint P2(p2, AxesInit, AxesInit);
    Joint P3(p3, AxesInit, AxesInit);
    Joint GOAL(goal, AxesInit, AxesInit);
    
    fabrik.setGoal(GOAL);
    fabrik.setJoints(P0, P1, P2, P3);
    jointNum = 4;
}

int DEMO__main(int argc, const char * argv[]) {
    // insert code here...
    
//Initialization Part
    generateLinks();
//-----------------------------LOOP
    fabrik.compute(0, Quaternion());
    Joint* joints = fabrik.getJoints();
    Vector3* Eulers = fabrik.getEulers();
    
    for (int i = 0; i < jointNum; i++) {
        printf("joint %d \nPosition:%lf,%lf,%lf\n",i,joints[i].getPosition().getValues()[0],joints[i].getPosition().getValues()[1],joints[i].getPosition().getValues()[2]);
    }
    for(int i = 0; i<jointNum-1; i++)
    {
        printf("bone %d \nEuler:%lf,%lf,%lf\n",i,Eulers[i].getX(),Eulers[i].getY(),Eulers[i].getZ());
        
    }
    fabrik.setJoints(joints[0],joints[1],joints[2],joints[3]);
    
    Position goal(15, 10, 0);
    Axes AxesInit;
    Joint GOAL(goal, AxesInit, AxesInit);
    
    fabrik.setGoal(GOAL);
//-----------------------------LOOPEND
    
    return 0;
}
