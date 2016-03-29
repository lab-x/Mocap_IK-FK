/*
    Qt OpenGL Tutorial - Lesson 07

    nehewidget.cpp
    v 1.00
    2002/12/19

    Copyright (C) 2002 Cavendish
                       cavendish@qiliang.net
                       http://www.qiliang.net/nehe_qt

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

*/
#include <QDebug>
#include <qimage.h>
#include "math.h"
#include "Virtual3dCharacterViewer.h"
//#include "LockOrientationConstraint.h"
extern "C"
{
    #include "MathToolsC/cQuaternion.h"
}

#include "InverseKinematics/joint.h"
#define TEST_FABRIK_TREE


GLfloat lightAmbient[4] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat lightDiffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lightPosition[4] = { 0.0, 0.0, 2.0, 1.0 };
Virtual3dCharacterViewer::Virtual3dCharacterViewer( QWidget* parent,  bool fs  , bool keyLimited )
    : QGLWidget( parent )
{
    isComport = false ;
    int i ;
    for ( i=0; i<sizeof(MarkCubePOT)/sizeof(MarkCubePOT[0]); i++ )
    {
        MarkCubePOT[i].Color = Vector3D(1,1,1) ;
        MarkCubePOT[i].show = false ;
        MarkCubePOT[i].Ori = EulerAngle(0,0,0) ;
        MarkCubePOT[i].Pos = Vector3D(0,0,0) ;
    }
    for ( i=0; i<sizeof(MarkCubeFK)/sizeof(MarkCubeFK[0]); i++ )
    {
        MarkCubeFK[i].Color = Vector3D(0,1,0) ;
        MarkCubeFK[i].show = false ;
        MarkCubeFK[i].Ori = EulerAngle(0,0,0) ;
        MarkCubeFK[i].Pos = Vector3D(0,0,0) ;
    }
    for ( i=0; i<sizeof(MarkCubeFKResult)/sizeof(MarkCubeFKResult[0]); i++ )
    {
        MarkCubeFKResult[i].Color = Vector3D(0,0,1) ;
        MarkCubeFKResult[i].show = false ;
        MarkCubeFKResult[i].Ori = EulerAngle(0,0,0) ;
        MarkCubeFKResult[i].Pos = Vector3D(0,0,0) ;
    }
    for ( i=0; i<sizeof(MarkCubeDebugLeftArm)/sizeof(MarkCubeDebugLeftArm[0]); i++ )
    {
        MarkCubeDebugLeftArm[i].Color = Vector3D(1,0,1) ;
        MarkCubeDebugLeftArm[i].show = false ;
        MarkCubeDebugLeftArm[i].Ori = EulerAngle(0,0,0) ;
        MarkCubeDebugLeftArm[i].Pos = Vector3D(0,0,0) ;
    }



    MarkCubeFK[0].show = TRUE ;
    MarkCubeFK[0].Pos = Vector3D(1,0,0) ;
    MarkCubeFK[0].Ori = EulerAngle(30,50,78) ;


    showFK();
    showIK();

    RightButtonPressed = LeftButtonPressed = false ;
    JointSelected = false ;
    initDefaultJointsDistance() ;
    initVirtualBody() ;
    IKResult.parent = this ;
    FKResult.parent = this ;

    WorldInCamera.Ori = EulerAngle(0,0,0) ;
    WorldInCamera.Pos.setValues( 0,0,-2) ;

    // new added .
    HumanIK::SkeletonModel m ;
    //humanIK.setModel( HumanIK::setDefaultModel(m), 1, IKResult.HipPos );
    humanIK.setModel( HumanIK::setDefaultModel(m) );


    filter = 0;

    light = false ;  //在initializeGL()中会使能光照



    fullscreen = fs;

    setWindowTitle("Quadcopter Emulator Widget v1.0 ---- Designed by NewThinker_Jiwey" );
    if ( ! parent )
    {
        setGeometry( 120, 120, 640, 480 );
        setMinimumSize(600,400);
    }
    else
    {
        setGeometry( 0, 0, parent->width(), parent->height() );
//        setMinimumSize(600,400);
    }

    if ( fullscreen )
        showFullScreen();

    scanTimer.setInterval(15);
    connect(&scanTimer,SIGNAL(timeout()),this,SLOT(show3D()));
    scanTimer.start();

}
Virtual3dCharacterViewer::~Virtual3dCharacterViewer()
{
}
void Virtual3dCharacterViewer::show3D()
{
    if ( isVisible() )
        updateGL();
}
void Virtual3dCharacterViewer::paintGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glBindTexture( GL_TEXTURE_2D, texture[filter] );


    // switch to camera sight .
    glTranslatef(  WorldInCamera.Pos.getX(),  WorldInCamera.Pos.getY(), WorldInCamera.Pos.getZ() );
    glRotatef( WorldInCamera.Ori.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( WorldInCamera.Ori.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( WorldInCamera.Ori.getRoll() , 0.0, 0.0 , 1.0 ) ;


    glGetFloatv(GL_MODELVIEW_MATRIX,  (GLfloat*)WorldMat);
    InvTransformation ( WorldMat, WorldMatInv ) ;
    drawCoordinateAxes () ;

    if ( ShowIK )
        glDrawBody(IKResult);
    if ( ShowFK )
        glDrawBody(FKResult);

    int i ;
    for ( i=0; i<sizeof(MarkCubePOT)/sizeof(MarkCubePOT[0]); i++ )
    {
        drawMarkCube(i);
    }
    for ( i=0; i<sizeof(MarkCubeFK)/sizeof(MarkCubeFK[0]); i++ )
    {
        drawMarkCube(i,1);
    }
    for ( i=0; i<sizeof(MarkCubeFKResult)/sizeof(MarkCubeFK[0]); i++ )
    {
        drawMarkCube(i,2);
    }
    for ( i=0; i<sizeof(MarkCubeDebugLeftArm)/sizeof(MarkCubeDebugLeftArm[0]); i++ )
    {
        drawMarkCube(i,3);
    }
}
void Virtual3dCharacterViewer::getNormal(GLfloat X1, GLfloat Y1, GLfloat Z1 ,
               GLfloat X2, GLfloat Y2, GLfloat Z2 ,
               GLfloat *X, GLfloat *Y, GLfloat *Z )
{
    *X = Y1*Z2 - Y2*Z1 ;
    *Y = Z1*X2 - Z2*X1 ;
    *Z = X1*Y2 - X2*Y1 ;
    float Normal = (*X)*(*X) + (*Y)*(*Y) + (*Z)*(*Z) ;
    float normal = sqrt(Normal) ;
    float normal_inv = 1.0 / normal ;
    (*X) *= normal_inv ;
    (*Y) *= normal_inv ;
    (*Z) *= normal_inv ;
}
void Virtual3dCharacterViewer::drawCoordinateAxes (Float Len)
{
    glDisable( GL_TEXTURE_2D );
    glBegin( GL_LINES );

    glColor3f( 1.0, 0.0, 0.0 );
    glVertex3f(  0.0,  0.0 , 0.0 );
    glVertex3f(  Len,  0.0 , 0.0 );


    glColor3f( 0.0, 1.0, 0.0 );
    glVertex3f(  0.0,  0.0 , 0.0 );
    glVertex3f(  0.0,  Len , 0.0 );


    glColor3f( 0.0, 0.0, 1.0 );
    glVertex3f(  0.0,  0.0 , 0.0 );
    glVertex3f(  0.0,  0.0 , Len );


    glEnd();

    if ( ENABLE_GL_TEXTURE_2D )
        glEnable( GL_TEXTURE_2D );

}
void Virtual3dCharacterViewer::drawOrderlyCuboid()
{
    GLfloat x1 = leftBottomX + xLength ;
    GLfloat y1 = leftBottomY + yLength ;
    GLfloat z1 = leftBottomZ ;

    GLfloat x2 = leftBottomX ;
    GLfloat y2 = leftBottomY + yLength ;
    GLfloat z2 = leftBottomZ ;

    GLfloat x3 = leftBottomX ;
    GLfloat y3 = leftBottomY + yLength ;
    GLfloat z3 = leftBottomZ + zLength ;

    GLfloat x4 = leftBottomX + xLength ;
    GLfloat y4 = leftBottomY + yLength ;
    GLfloat z4 = leftBottomZ + zLength ;

    GLfloat x5 = leftBottomX + xLength ;
    GLfloat y5 = leftBottomY ;
    GLfloat z5 = leftBottomZ + zLength ;

    GLfloat x6 = leftBottomX ;
    GLfloat y6 = leftBottomY ;
    GLfloat z6 = leftBottomZ + zLength ;

    GLfloat x7 = leftBottomX ;
    GLfloat y7 = leftBottomY ;
    GLfloat z7 = leftBottomZ ;

    GLfloat x8 = leftBottomX + xLength ;
    GLfloat y8 = leftBottomY ;
    GLfloat z8 = leftBottomZ ;

    GLfloat normalX , normalY , normalZ ;

    // 开始画立方体
    glDisable( GL_TEXTURE_2D );

    glBegin( GL_QUADS );

//    glColor3f( 0.0, 0.0, 1.0 );


        // 立方体的顶面
        getNormal ( x2-x1, y2-y1, z2-z1 ,
                    x3-x2, y3-y2, z3-z2 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
        glColor3f( 0.0, 1.0, 0.0 );
        glVertex3f(  x1,  y1 , z1 );
        glVertex3f(  x2,  y2 , z2 );
        glVertex3f(  x3,  y3 , z3 );
        glVertex3f(  x4,  y4 , z4 );


        // 立方体的底面
        getNormal ( x6-x5, y6-y5, z6-z5 ,
                    x7-x6, y7-y6, z7-z6 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
        glColor3f( 1.0, 0.5, 0.0 );
        glVertex3f(  x5,  y5 , z5 );
        glVertex3f(  x6,  y6 , z6 );
        glVertex3f(  x7,  y7 , z7 );
        glVertex3f(  x8,  y8 , z8 );

        // 立方体的前面
        getNormal ( x3-x4, y3-y4, z3-z4 ,
                    x6-x3, y6-y3, z6-z3 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
        glColor3f( 1.0, 0.0, 0.0 );
        glVertex3f(  x4,  y4 , z4 );
        glVertex3f(  x3,  y3 , z3 );
        glVertex3f(  x6,  y6 , z6 );
        glVertex3f(  x5,  y5 , z5 );

        // 立方体的后面
        getNormal ( x7-x8, y7-y8, z7-z8 ,
                    x2-x7, y2-y7, z2-z7 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
        glColor3f( 0.5, 0.5, 0.0 );
        glVertex3f(  x8,  y8 , z8 );
        glVertex3f(  x7,  y7 , z7 );
        glVertex3f(  x2,  y2 , z2 );
        glVertex3f(  x1,  y1 , z1 );

        // 立方体的左面
        getNormal ( x2-x3, y2-y3, z2-z3 ,
                    x7-x2, y7-y2, z7-z2 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
        glColor3f( 0.0, 0.0, 1.0 );
        glVertex3f(  x3,  y3 , z3 );
        glVertex3f(  x2,  y2 , z2 );
        glVertex3f(  x7,  y7 , z7 );
        glVertex3f(  x6,  y6 , z6 );

        // 立方体的右面
        getNormal ( x4-x1, y4-y1, z4-z1 ,
                    x5-x4, y5-y4, z5-z4 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
        glColor3f( 1.0, 0.0, 1.0 );
        glVertex3f(  x1,  y1 , z1 );
        glVertex3f(  x4,  y4 , z4 );
        glVertex3f(  x5,  y5 , z5 );
        glVertex3f(  x8,  y8 , z8 );

    glEnd();

    if ( ENABLE_GL_TEXTURE_2D )
        glEnable( GL_TEXTURE_2D );
}


void Virtual3dCharacterViewer::drawSingleColorCube(Vector3D Color)
{
    GLfloat x1 = leftBottomX + xLength ;
    GLfloat y1 = leftBottomY + yLength ;
    GLfloat z1 = leftBottomZ ;

    GLfloat x2 = leftBottomX ;
    GLfloat y2 = leftBottomY + yLength ;
    GLfloat z2 = leftBottomZ ;

    GLfloat x3 = leftBottomX ;
    GLfloat y3 = leftBottomY + yLength ;
    GLfloat z3 = leftBottomZ + zLength ;

    GLfloat x4 = leftBottomX + xLength ;
    GLfloat y4 = leftBottomY + yLength ;
    GLfloat z4 = leftBottomZ + zLength ;

    GLfloat x5 = leftBottomX + xLength ;
    GLfloat y5 = leftBottomY ;
    GLfloat z5 = leftBottomZ + zLength ;

    GLfloat x6 = leftBottomX ;
    GLfloat y6 = leftBottomY ;
    GLfloat z6 = leftBottomZ + zLength ;

    GLfloat x7 = leftBottomX ;
    GLfloat y7 = leftBottomY ;
    GLfloat z7 = leftBottomZ ;

    GLfloat x8 = leftBottomX + xLength ;
    GLfloat y8 = leftBottomY ;
    GLfloat z8 = leftBottomZ ;

    GLfloat normalX , normalY , normalZ ;

    // 开始画立方体
    glDisable( GL_TEXTURE_2D );

    glBegin( GL_QUADS );

//    glColor3f( 0.0, 0.0, 1.0 );


        // 立方体的顶面
        getNormal ( x2-x1, y2-y1, z2-z1 ,
                    x3-x2, y3-y2, z3-z2 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
 //       glColor3f( 0.0, 1.0, 0.0 );
        glColor3f( Color.getX(), Color.getY(), Color.getZ() );
        glVertex3f(  x1,  y1 , z1 );
        glVertex3f(  x2,  y2 , z2 );
        glVertex3f(  x3,  y3 , z3 );
        glVertex3f(  x4,  y4 , z4 );


        // 立方体的底面
        getNormal ( x6-x5, y6-y5, z6-z5 ,
                    x7-x6, y7-y6, z7-z6 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
//        glColor3f( 1.0, 0.5, 0.0 );
        glVertex3f(  x5,  y5 , z5 );
        glVertex3f(  x6,  y6 , z6 );
        glVertex3f(  x7,  y7 , z7 );
        glVertex3f(  x8,  y8 , z8 );

        // 立方体的前面
        getNormal ( x3-x4, y3-y4, z3-z4 ,
                    x6-x3, y6-y3, z6-z3 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
//        glColor3f( 1.0, 0.0, 0.0 );
        glVertex3f(  x4,  y4 , z4 );
        glVertex3f(  x3,  y3 , z3 );
        glVertex3f(  x6,  y6 , z6 );
        glVertex3f(  x5,  y5 , z5 );

        // 立方体的后面
        getNormal ( x7-x8, y7-y8, z7-z8 ,
                    x2-x7, y2-y7, z2-z7 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
//        glColor3f( 0.5, 0.5, 0.0 );
        glVertex3f(  x8,  y8 , z8 );
        glVertex3f(  x7,  y7 , z7 );
        glVertex3f(  x2,  y2 , z2 );
        glVertex3f(  x1,  y1 , z1 );

        // 立方体的左面
        getNormal ( x2-x3, y2-y3, z2-z3 ,
                    x7-x2, y7-y2, z7-z2 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
//        glColor3f( 0.0, 0.0, 1.0 );
        glVertex3f(  x3,  y3 , z3 );
        glVertex3f(  x2,  y2 , z2 );
        glVertex3f(  x7,  y7 , z7 );
        glVertex3f(  x6,  y6 , z6 );

        // 立方体的右面
        getNormal ( x4-x1, y4-y1, z4-z1 ,
                    x5-x4, y5-y4, z5-z4 ,
                    &normalX , &normalY , &normalZ ) ;
        glNormal3f( normalX, normalY, normalZ );
//        glColor3f( 1.0, 0.0, 1.0 );
        glVertex3f(  x1,  y1 , z1 );
        glVertex3f(  x4,  y4 , z4 );
        glVertex3f(  x5,  y5 , z5 );
        glVertex3f(  x8,  y8 , z8 );

    glEnd();

    if ( ENABLE_GL_TEXTURE_2D )
        glEnable( GL_TEXTURE_2D );
}



void Virtual3dCharacterViewer::initializeGL()
{

  loadGLTextures();

  ENABLE_GL_TEXTURE_2D = true ;
  if ( ENABLE_GL_TEXTURE_2D )
    glEnable( GL_TEXTURE_2D );

  glShadeModel( GL_SMOOTH );
  glClearColor( 0.0, 0.0, 0.0, 0.5 );
  glClearDepth( 1.0 );
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  
  glLightfv( GL_LIGHT1, GL_AMBIENT, lightAmbient );
  glLightfv( GL_LIGHT1, GL_DIFFUSE, lightDiffuse );
  glLightfv( GL_LIGHT1, GL_POSITION, lightPosition );
  
  glEnable( GL_LIGHT1 );

  glColorMaterial(GL_FRONT,GL_DIFFUSE);
  glEnable(GL_COLOR_MATERIAL);

//  glEnable( GL_LIGHTING );
//  light = true ;
}
void Virtual3dCharacterViewer::resizeGL( int width, int height )
{
    if ( height == 0 )
    {
      height = 1;
    }
    glViewport( 0, 0, (GLint)width, (GLint)height );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // 用新接口重实现 gluPerspective
  //  gluPerspective( 45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0 );
    GLfloat zNear = 0.1;
    GLfloat zFar = 100.0;
    GLfloat aspect = (GLfloat)width/(GLfloat)height;
    GLfloat fH = tan(GLfloat(90.0/360.0*3.14159))*zNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
    // 重实现完毕。


    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
}
void Virtual3dCharacterViewer::keyPressEvent( QKeyEvent *e )
{
#define PI 3.1415926

    switch ( e->key() )
    {
    case Qt::Key_F2:
        if ( ! fullscreen )
        {
            fullscreen = true ;
            showFullScreen();
            update();
        }
        break;
    case Qt::Key_Escape:
        if ( fullscreen )
        {
            fullscreen = false ;
            showNormal();
            update();
        }
        break;
    case Qt::Key_L:
        light = !light;
        if ( !light )
        {
          glDisable( GL_LIGHTING );
        }
        else
        {
          glEnable( GL_LIGHTING );
        }
        break;

    case Qt::Key_Up:
        WorldInCamera.Pos.setZ ( WorldInCamera.Pos.getZ() - 0.1 ) ;
        break ;


    case Qt::Key_Down:
        WorldInCamera.Pos.setZ ( WorldInCamera.Pos.getZ() + 0.1 ) ;
        break ;
    default:
        break ;
    }
}
void Virtual3dCharacterViewer::keyReleaseEvent( QKeyEvent *e )
{
    if ( e->isAutoRepeat() )
        return ;



    switch(e->key())
    {
    case Qt::Key_Up:
        break ;
    case Qt::Key_Down:
        break ;
    default:
        break ;
    }
}
void Virtual3dCharacterViewer::mousePressEvent(QMouseEvent * e)
{
    if ( e->button() == Qt::LeftButton )
    {
        setFocus();
        LeftButtonPressed = true ;
        LeftButtonPressPos = e->pos() ;
        WorldInCameraBackup = WorldInCamera ;
    }
    else if ( e->button() == Qt::RightButton )
    {
        RightButtonPressed = true ;
        RightButtonPressPos = e->pos() ;
        float disHip, disNeck, disHead, disLeftWrist, disRightWrist, disLeftAnkle, disRightAnkle ;
        disHip = distanceToRightButtonPressPos ( IKResult.HipPosInScreen ) ;
        disNeck = distanceToRightButtonPressPos ( IKResult.NeckPosInScreen ) ;
        disHead = distanceToRightButtonPressPos ( IKResult.HeadPosInScreen ) ;
        disLeftWrist = distanceToRightButtonPressPos ( IKResult.LeftWristPosInScreen ) ;
        disRightWrist = distanceToRightButtonPressPos ( IKResult.RightWristPosInScreen ) ;
        disLeftAnkle = distanceToRightButtonPressPos ( IKResult.LeftAnklePosInScreen ) ;
        disRightAnkle = distanceToRightButtonPressPos ( IKResult.RightAnklePosInScreen ) ;
        float minDis = disHip ;
        minDis = disNeck < minDis ? disNeck :  minDis ;
        minDis = disHead < minDis ? disHead :  minDis ;
        minDis = disLeftWrist < minDis ? disLeftWrist :  minDis ;
        minDis = disRightWrist < minDis ? disRightWrist :  minDis ;
        minDis = disLeftAnkle < minDis ? disLeftAnkle :  minDis ;
        minDis = disRightAnkle < minDis ? disRightAnkle :  minDis ;

        if ( minDis < 15 )
        {
            JointSelected = true ;

            IKResult.HipPosRaw = IKResult.HipPos ;
            IKResult.HeadPosRaw = IKResult.HeadPos ;
            IKResult.NeckPosRaw = IKResult.NeckPos ;
            IKResult.LeftWristPosRaw = IKResult.LeftWristPos ;
            IKResult.RightWristPosRaw = IKResult.RightWristPos ;
            IKResult.LeftAnklePosRaw = IKResult.LeftAnklePos ;
            IKResult.RightAnklePosRaw = IKResult.RightAnklePos ;

            if ( minDis == disHip )
            {
                SelectedJointPosInCamera = IKResult.HipPosInCamera ;
                pSelectedJointPosRaw = & IKResult.HipPosRaw ;
                SelectedJointID = HIP_JOINT ;
            }
            else if ( minDis == disNeck )
            {
                SelectedJointPosInCamera = IKResult.NeckPosInCamera ;
                pSelectedJointPosRaw = & IKResult.NeckPosRaw ;
                SelectedJointID = NECK_JOINT ;
            }
            else if ( minDis == disHead )
            {
                SelectedJointPosInCamera =  IKResult.HeadPosInCamera ;
                pSelectedJointPosRaw = & IKResult.HeadPosRaw ;
                SelectedJointID = HEAD_JOINT ;
            }
            else if ( minDis == disLeftWrist )
            {
                SelectedJointPosInCamera =  IKResult.LeftWristPosInCamera ;
                pSelectedJointPosRaw = & IKResult.LeftWristPosRaw ;
                SelectedJointID = LEFT_WRIST_JOINT ;
            }
            else if ( minDis == disRightWrist )
            {
                SelectedJointPosInCamera =  IKResult.RightWristPosInCamera ;
                pSelectedJointPosRaw = & IKResult.RightWristPosRaw ;
                SelectedJointID = RIGHT_WRIST_JOINT ;
            }
            else if ( minDis == disLeftAnkle )
            {
                SelectedJointPosInCamera =  IKResult.LeftAnklePosInCamera ;
                pSelectedJointPosRaw = & IKResult.LeftAnklePosRaw ;
                SelectedJointID = LEFT_ANKLE_JOINT ;
            }
            else // if ( minDis == disRightAnkle )
            {
                SelectedJointPosInCamera =  IKResult.RightAnklePosInCamera ;
                pSelectedJointPosRaw = & IKResult.RightAnklePosRaw ;
                SelectedJointID = RIGHT_ANKLE_JOINT ;
            }
        }
    }
}
void Virtual3dCharacterViewer::mouseMoveEvent(QMouseEvent * e)
{
    if (RightButtonPressed && !isComport)
    {
        if ( JointSelected )
        {
            Vector3D newPosInCamera = getHomogeneous ( QPointToVector2D(e->pos()), SelectedJointPosInCamera.getZ() ) ;
            Vector3D newPosInWorld = TransformVector(WorldMatInv, newPosInCamera) ;
            *pSelectedJointPosRaw = newPosInWorld ;
            triggerIK () ;
            jointMoved () ;
        }
    }

    else if ( LeftButtonPressed )
    {
        QPoint p = e->pos() ;
        QPoint p2 = LeftButtonPressPos ;
        float dx, dy;
        dx = p.x() - p2.x() ;
        dy = p.y() - p2.y() ;
        dy /= height() ;
        dx /= height() ;    // get the ratio based on height .
        dy = -dy ;  // 屏幕坐标与空间坐标的y轴相反

        cQuaternion Q1, dQ, Q2 ;
        float rTod = 180 / PI ;
        float dTor = PI / 180 ;
        cEulerToQuat ( WorldInCameraBackup.Ori.getRoll()*dTor, WorldInCameraBackup.Ori.getPitch()*dTor, WorldInCameraBackup.Ori.getYaw()*dTor, &Q1) ;
        float temp = Q1.y ;
        Q1.y = Q1.z ;
        Q1.z = Q1.x ;
        Q1.x = temp ;

        Vector3D v1 ;
        Vector3D v2 ;
        Vector3D v3 ;
        v1 = Vector3D(dx, dy, 0) ;
        v2 = Vector3D(0,0,1) ;
        v3 = Vector3D::Cross(v2,v1).normalize() ;
        float angle = v1.getLength()*PI/2;
        dQ.w = cos(angle/2) ;
        dQ.x = sin(angle/2) * v3.getX() ;
        dQ.y = sin(angle/2) * v3.getY() ;
        dQ.z = sin(angle/2) * v3.getZ() ;


        cMultQuat ( &dQ, &Q1, &Q2 ) ;
        temp = Q2.x ;
        Q2.x = Q2.z ;
        Q2.z = Q2.y ;
        Q2.y = temp ;
        cEulerAngle eu ;
        cQuatToEuler ( &Q2 , &eu ) ; //
        WorldInCamera.Ori = EulerAngle(eu.yaw, eu.pitch, eu.roll) ;
        WorldInCamera.Ori = WorldInCamera.Ori.RadToDegree() ;
    }
}
void Virtual3dCharacterViewer::mouseReleaseEvent(QMouseEvent * e)
{
    if ( e->button() == Qt::LeftButton )
    {
        LeftButtonPressed = false ;
        LeftButtonReleasePos = e->pos() ;
    }
    else if ( e->button() == Qt::RightButton )
    {
        RightButtonPressed = false ;
        RightButtonReleasePos = e->pos() ;
        JointSelected = false ;
        SelectedJointID = NO_SELECTED_JOINT ;
    }
}
#ifndef Q_OS_MAC
void Virtual3dCharacterViewer::wheelEvent(QWheelEvent * e)
{
    QPoint a = e->angleDelta() ;
    float dAngle = a.y() / 8.0f ;   // 角度值

    float dDeep = dAngle / 60 ;  // 60°是 1m
    WorldInCamera.Pos.setZ ( WorldInCamera.Pos.getZ() + dDeep )  ;
}
#endif
void Virtual3dCharacterViewer::loadGLTextures()
{
  QImage tex, buf;
  if ( !buf.load( "./data/Crate.bmp" ) )
  {
    qWarning( "Could not read image file, using single-color instead." );
    QImage dummy( 128, 128, QImage::Format_ARGB32 );
    dummy.fill( qRgba(0,255,0,0) );
    buf = dummy;
  }
  tex = QGLWidget::convertToGLFormat( buf );
  
  glGenTextures( 3, &texture[0] );

  glBindTexture( GL_TEXTURE_2D, texture[0] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
      GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );

  glBindTexture( GL_TEXTURE_2D, texture[1] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
      GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );

  glBindTexture( GL_TEXTURE_2D, texture[2] );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
//  gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, tex.width(), tex.height(),        // Qt5中没有这个函数，所以只能都用glTexImage2D
//      GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );
  glTexImage2D( GL_TEXTURE_2D, 0, 3, tex.width(), tex.height(), 0,
      GL_RGBA, GL_UNSIGNED_BYTE, tex.bits() );
}
/********************************************************
 *
 *
 * Draw Robot
 *
 *
 ********************************************************/
void Virtual3dCharacterViewer::glDrawBody(Character3DBody& body)
{
    glPushMatrix();


    // getHipPos() ; // get Hip's Position from the IK calculator .
    LocateHip (body);
    body.HipPosInCamera = getCurTranslation () ;
    body.HipPosInScreen = getScreenPos(body.HipPosInCamera) ;
    RotateHip(body) ;
    glPushMatrix();     // save the Hip Matrix
    drawHip (body) ;

    glTranslatef( 0.0, body.Model.LenHipToChest, 0.0 ) ;
    RotateChest (body);
    drawChest (body) ;


    glTranslatef(0.0, body.Model.LenChestToNeck, 0.0);

    body.NeckPosInCamera = getCurTranslation () ;
    body.NeckPos = TransformVector( WorldMatInv, body.NeckPosInCamera ) ;
    body.NeckPosInScreen = getScreenPos(body.NeckPosInCamera) ;


    glPushMatrix();     // save the Neck Matrix

    RotateLeftShoulder(body);
    drawLeftShoulder(body);

    glTranslatef(body.Model.LenNeckToShoulder, 0.0, 0.0);
    RotateLeftUpperArm(body);
    drawLeftUpperArm(body);


    glTranslatef(body.Model.LenShoulderToElbow, 0.0, 0.0);
    RotateLeftLowerArm(body);
    drawLeftLowerArm(body);



    glTranslatef(body.Model.LenElbowToWrist, 0.0, 0.0);

    body.LeftWristPosInCamera = getCurTranslation () ;
    Vector3D bias(-1,0,0) ;
    body.LeftWristPos = TransformVector( WorldMatInv, body.LeftWristPosInCamera );
    body.LeftWristPosInScreen = getScreenPos(body.LeftWristPosInCamera) ;

    RotateLeftHand(body);
    drawLeftHand(body);


    glPopMatrix();      // reload the Neck Matrix
    glPushMatrix();     // restore the Neck Matrix again .


    RotateRightShoulder(body);
    drawRightShoulder(body);

    glTranslatef(-body.Model.LenNeckToShoulder, 0.0, 0.0);
    RotateRightUpperArm(body);
    drawRightUpperArm(body);


    glTranslatef(-body.Model.LenShoulderToElbow, 0.0, 0.0);
    RotateRightLowerArm(body);
    drawRightLowerArm(body);

    glTranslatef(-body.Model.LenElbowToWrist, 0.0, 0.0);

    body.RightWristPosInCamera = getCurTranslation () ;
    body.RightWristPos = TransformVector( WorldMatInv, body.RightWristPosInCamera ) ;
    body.RightWristPosInScreen = getScreenPos(body.RightWristPosInCamera) ;

    RotateRightHand(body);
    drawRightHand(body);


    glPopMatrix();      // reload the Neck Matrix again .

    RotateHead(body);
    drawHead (body);
    glTranslatef(0.0, body.Model.LenNeckToHead, 0.0);
    body.HeadPosInCamera = getCurTranslation () ;
    body.HeadPos = TransformVector( WorldMatInv, body.HeadPosInCamera ) ;
    body.HeadPosInScreen = getScreenPos(body.HeadPosInCamera) ;

    glPopMatrix();      // reload the Hip Matrix .
    glPushMatrix();     // restore the Hip Matrix again.

    RotateLeftThighRoot (body);
    drawLeftThighRoot(body);

    glTranslatef(body.Model.LenHipToThigh, 0.0, 0.0);

    RotateLeftThigh (body) ;
    drawLeftThigh(body);

    glTranslatef(0.0, -body.Model.LenThighToKnee, 0.0);

    RotateLeftShank(body);
    drawLeftShank(body);

    glTranslatef(0.0, -body.Model.LenKneeToAnkle, 0.0);
    body.LeftAnklePosInCamera = getCurTranslation();
    body.LeftAnklePos = TransformVector( WorldMatInv, body.LeftAnklePosInCamera ) ;
    body.LeftAnklePosInScreen = getScreenPos(body.LeftAnklePosInCamera) ;

    RotateLeftFoot (body);
    drawLeftFoot(body);


    glPopMatrix();      // reload the Hip Matrix again.


    RotateRightThighRoot (body);
    drawRightThighRoot(body);

    glTranslatef(-body.Model.LenHipToThigh, 0.0, 0.0);

    RotateRightThigh (body) ;
    drawRightThigh(body);

    glTranslatef(0.0, -body.Model.LenThighToKnee, 0.0);

    RotateRightShank(body);
    drawRightShank(body);

    glTranslatef(0.0, -body.Model.LenKneeToAnkle, 0.0);
    body.RightAnklePosInCamera = getCurTranslation();
    body.RightAnklePos = TransformVector( WorldMatInv, body.RightAnklePosInCamera ) ;
    body.RightAnklePosInScreen = getScreenPos(body.RightAnklePosInCamera) ;



    RotateRightFoot (body);
    drawRightFoot(body);


    glPopMatrix();
}
void Virtual3dCharacterViewer::drawHip (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.1f ;

    xLength = 2.0f * body.Model.LenHipToThigh ;
    yLength = rLen * body.Model.LenHipToChest ;
    zLength = 0.08f ;
    leftBottomX = - xLength / 2.0f  ;
    leftBottomY =  yLength / rLen * rStart  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawChest (Character3DBody& body)
{
    float rLen = 0.9f ;
    float rStart = 0.1f ;

    xLength = 2.0f * body.Model.LenHipToThigh ;
    yLength = rLen * body.Model.LenChestToNeck ;
    zLength = 0.08f ;
    leftBottomX = - xLength / 2.0f  ;
    leftBottomY =  yLength / rLen * rStart  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawHead (Character3DBody& body)
{
    float rLen = 0.5f ;
    float rStart = 0.5f ;

    yLength = rLen * body.Model.LenNeckToHead ;
    xLength = yLength ;
    zLength = yLength ;

    leftBottomX = - xLength / 2.0f  ;
    leftBottomY =  yLength / rLen * rStart  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawLeftThighRoot (Character3DBody& body)
{
    // draw nothing
}
void Virtual3dCharacterViewer::drawLeftThigh (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.1f ;

    yLength = rLen * body.Model.LenThighToKnee ;
    xLength = 0.1f ;
    zLength = 0.1f ;

    leftBottomX = - xLength / 2.0f  ;
    leftBottomY = yLength / rLen * ( rStart - 1.0f )  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawLeftShank (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.0f ;

    yLength = rLen * body.Model.LenKneeToAnkle ;
    xLength = 0.1f ;
    zLength = 0.1f ;

    leftBottomX = - xLength / 2.0f  ;
    leftBottomY = yLength / rLen * ( rStart - 1.0f )  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawLeftFoot (Character3DBody& body)
{
    return ;

    yLength = 0.07f;
    xLength = 0.1f ;
    zLength = 0.25f ;

    leftBottomX = - xLength / 2.0f  ;
    leftBottomY = - yLength  ;
    leftBottomZ =  - zLength * 0.3f  ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawRightThighRoot (Character3DBody& body)
{
    // draw nothing
}
void Virtual3dCharacterViewer::drawRightThigh (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.1f ;

    yLength = rLen * body.Model.LenThighToKnee ;
    xLength = 0.1f ;
    zLength = 0.1f ;

    leftBottomX = - xLength / 2.0f  ;
    leftBottomY = yLength / rLen * ( rStart - 1.0f )  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawRightShank (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.0f ;

    yLength = rLen * body.Model.LenKneeToAnkle ;
    xLength = 0.1f ;
    zLength = 0.1f ;

    leftBottomX = - xLength / 2.0f  ;
    leftBottomY = yLength / rLen * ( rStart - 1.0f )  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawRightFoot (Character3DBody& body)
{
    return ;

    yLength = 0.07f;
    xLength = 0.1f ;
    zLength = 0.25f ;

    leftBottomX = - xLength / 2.0f  ;
    leftBottomY = - yLength  ;
    leftBottomZ = - zLength * 0.3f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawLeftShoulder (Character3DBody& body)
{
    // draw nothing .
}
void Virtual3dCharacterViewer::drawLeftUpperArm (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.1f ;

    xLength = rLen * body.Model.LenShoulderToElbow ;
    yLength = 0.05f ;
    zLength = 0.05f ;
    leftBottomX = rStart * xLength / rLen ;
    leftBottomY =  - yLength / 2.0f  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawLeftLowerArm (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.1f ;

    xLength = rLen * body.Model.LenElbowToWrist ;
    yLength = 0.05f ;
    zLength = 0.05f ;
    leftBottomX = rStart * xLength / rLen ;
    leftBottomY =  - yLength / 2.0f  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawLeftHand (Character3DBody& body) {}
void Virtual3dCharacterViewer::drawRightShoulder (Character3DBody& body)
{
    // draw nothing
}
void Virtual3dCharacterViewer::drawRightUpperArm (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.1f ;

    xLength = rLen * body.Model.LenShoulderToElbow ;
    yLength = 0.05f ;
    zLength = 0.05f ;
    leftBottomX = ( rStart - 1.0f ) * xLength / rLen ;
    leftBottomY =  - yLength / 2.0f  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;
}
void Virtual3dCharacterViewer::drawRightLowerArm (Character3DBody& body)
{
    float rLen = 0.8f ;
    float rStart = 0.1f ;

    xLength = rLen * body.Model.LenElbowToWrist ;
    yLength = 0.05f ;
    zLength = 0.05f ;
    leftBottomX = ( rStart - 1.0f ) * xLength / rLen ;
    leftBottomY =  - yLength / 2.0f  ;
    leftBottomZ = - zLength / 2.0f ;
    drawOrderlyCuboid() ;

}
void Virtual3dCharacterViewer::drawMarkCube ( int i, int Group )
{
    MarkCube *group ;
    Vector3D color ;

    switch(Group)
    {
    case 0:
        group = MarkCubePOT ;
        break;
    case 1:
        group = MarkCubeFK ;
        break;
    case 2:
        group = MarkCubeFKResult ;
        break;
    case 3:
        group = MarkCubeDebugLeftArm ;
        break;
    default:
        return ;
    }


    color = group[i].Color ;
    if ( group[i].show )
    {
        glPushMatrix();

        glTranslatef(  group[i].Pos.getX(),  group[i].Pos.getY(), group[i].Pos.getZ()  ) ;
        glRotatef( group[i].Ori.getYaw() , 0.0, 1.0 , 0.0 ) ;
        glRotatef( group[i].Ori.getPitch() , 1.0, 0.0 , 0.0 ) ;
        glRotatef( group[i].Ori.getRoll() , 0.0, 0.0 , 1.0 ) ;

        xLength = 0.04f ;
        yLength = 0.04f ;
        zLength = 0.04f ;
        leftBottomX = - xLength / 2.0f ;
        leftBottomY = - yLength / 2.0f ;
        leftBottomZ = - zLength / 2.0f ;
        drawSingleColorCube(color);
        drawCoordinateAxes(0.15);

        glPopMatrix();
    }
}


void Virtual3dCharacterViewer::drawRightHand (Character3DBody& body) {}

void Virtual3dCharacterViewer::LocateHip (Character3DBody& body)
{
    glTranslatef(  body.HipPos.getX(),  body.HipPos.getY(), body.HipPos.getZ()  ) ;
}
void Virtual3dCharacterViewer::RotateHip (Character3DBody& body)
{
    glRotatef( body.HipOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.HipOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.HipOri.getRoll() , 0.0, 0.0 , 1.0 ) ;
}
void Virtual3dCharacterViewer::RotateChest (Character3DBody& body)
{
    glRotatef( body.ChestOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.ChestOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.ChestOri.getRoll() , 0.0, 0.0 , 1.0 ) ;
}
void Virtual3dCharacterViewer::RotateHead (Character3DBody& body)
{
    glRotatef( body.HeadOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.HeadOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.HeadOri.getRoll() , 0.0, 0.0 , 1.0 ) ;
}
void Virtual3dCharacterViewer::RotateLeftThighRoot (Character3DBody& body)
{
    glRotatef( body.LeftThighRootOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftThighRootOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftThighRootOri.getRoll() , 0.0, 0.0 , 1.0 ) ;
}
void Virtual3dCharacterViewer::RotateLeftThigh (Character3DBody& body)
{
    glRotatef( body.LeftThighOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftThighOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftThighOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateLeftShank (Character3DBody& body)
{
    glRotatef( body.LeftShankOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftShankOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftShankOri.getRoll() , 0.0, 0.0 , 1.0 ) ;
}
void Virtual3dCharacterViewer::RotateLeftFoot (Character3DBody& body)
{
    glRotatef( body.LeftFootOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftFootOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftFootOri.getRoll() , 0.0, 0.0 , 1.0 ) ;
}
void Virtual3dCharacterViewer::RotateRightThighRoot (Character3DBody& body)
{
    glRotatef( body.RightThighRootOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightThighRootOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightThighRootOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateRightThigh (Character3DBody& body)
{
    glRotatef( body.RightThighOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightThighOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightThighOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateRightShank (Character3DBody& body)
{
    glRotatef( body.RightShankOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightShankOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightShankOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateRightFoot (Character3DBody& body)
{
    glRotatef( body.RightFootOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightFootOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightFootOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateLeftShoulder (Character3DBody& body)
{
    glRotatef( body.LeftShoulderOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftShoulderOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftShoulderOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateLeftUpperArm (Character3DBody& body)
{
    glRotatef( body.LeftUpperArmOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftUpperArmOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftUpperArmOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateLeftLowerArm (Character3DBody& body)
{
    glRotatef( body.LeftLowerArmOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftLowerArmOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftLowerArmOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateLeftHand (Character3DBody& body)
{
    glRotatef( body.LeftHandOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.LeftHandOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.LeftHandOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateRightShoulder (Character3DBody& body)
{
    glRotatef( body.RightShoulderOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightShoulderOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightShoulderOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateRightUpperArm (Character3DBody& body)
{
    glRotatef( body.RightUpperArmOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightUpperArmOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightUpperArmOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateRightLowerArm (Character3DBody& body)
{
    glRotatef( body.RightLowerArmOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightLowerArmOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightLowerArmOri.getRoll() , 0.0, 0.0 , 1.0 ) ;

}
void Virtual3dCharacterViewer::RotateRightHand (Character3DBody& body)
{
    glRotatef( body.RightHandOri.getYaw() , 0.0, 1.0 , 0.0 ) ;
    glRotatef( body.RightHandOri.getPitch() , 1.0, 0.0 , 0.0 ) ;
    glRotatef( body.RightHandOri.getRoll() , 0.0, 0.0 , 1.0 ) ;
}

/********************************************************
 *
 *
 *  IK
 *
 *
 ********************************************************/

void Virtual3dCharacterViewer::updateBonesOriWithIKresult (Character3DBody &body, Vector3D offset)
{
//    Character3DBody & body = IKResult ;

    // new added
    body.HipPos = humanIK.output().HipPos + offset ;
    body.HipOri =  humanIK.output().HipOri ;
    body.ChestOri =  humanIK.output().ChestOri ;
    body.LeftShoulderOri =  humanIK.output().LeftShoulderOri ;
    body.LeftUpperArmOri =  humanIK.output().LeftUpperArmOri ;
    body.LeftLowerArmOri =  humanIK.output().LeftLowerArmOri ;
    body.LeftHandOri =  humanIK.output().LeftHandOri ;
    body.RightShoulderOri =  humanIK.output().RightShoulderOri ;
    body.RightUpperArmOri =  humanIK.output().RightUpperArmOri ;
    body.RightLowerArmOri =  humanIK.output().RightLowerArmOri ;
    body.RightHandOri =  humanIK.output().RightHandOri ;
    body.HeadOri =  humanIK.output().HeadOri ;
    body.LeftThighRootOri =  humanIK.output().LeftThighRootOri ;
    body.LeftThighOri =  humanIK.output().LeftThighOri ;
    body.LeftShankOri =  humanIK.output().LeftShankOri ;
    body.LeftFootOri =  humanIK.output().LeftFootOri ;
    body.RightThighRootOri =  humanIK.output().RightThighRootOri ;
    body.RightThighOri =  humanIK.output().RightThighOri ;
    body.RightShankOri =  humanIK.output().RightShankOri ;
    body.RightFootOri =  humanIK.output().RightFootOri ;
    return ;

}

void Virtual3dCharacterViewer::triggerIK ()
{
    //Position goal(10, 10, -10);
    Character3DBody & body = IKResult ;

    // new added .
    HumanIK::RawDataFrame rawData ;
    rawData.id = HumanIK::Joint_Neck ;
    rawData.Pos = body.NeckPosRaw ;
    rawData.Q = Quaternion(1,0,0,0) ;
    humanIK.input(rawData) ;

    // new added .
    rawData.id = HumanIK::Joint_LeftWrist ;
    rawData.Pos = body.LeftWristPosRaw ;
    rawData.Q = Quaternion(0,0,0,0) ;
    humanIK.input(rawData) ;

    // new added .
    rawData.id = HumanIK::Joint_RightWrist ;
    rawData.Pos = body.RightWristPosRaw ;
    rawData.Q = Quaternion(0,0,0,0) ;
    humanIK.input(rawData) ;

    // new added
    rawData.id = HumanIK::Joint_LeftAnkle ;
    rawData.Pos = body.LeftAnklePosRaw ;
    rawData.Q = Quaternion(0,0,0,0) ;
    humanIK.input(rawData) ;

    // new added
    rawData.id = HumanIK::Joint_RightAnkle ;
    rawData.Pos = body.RightAnklePosRaw ;
    rawData.Q = Quaternion(0,0,0,0) ;
    humanIK.input(rawData) ;

    // new added
    rawData.id = HumanIK::Joint_Head ;
    rawData.Pos = body.HeadPosRaw ;
    rawData.Q = Quaternion(0,0,0,0) ;
    humanIK.input(rawData) ;

    humanIK.compute(HumanIK::CALC_HIP);
    humanIK.compute();
    //humanIK.compute(HumanIK::CALC_LEFT_ARM);
    //humanIK.compute(HumanIK::CALC_HIP);

    updateBonesOriWithIKresult(IKResult);
    update();


/*
    Vector3D p ;
    p = HipPosRaw ; qDebug()<<"HipPosRaw:\t"<<p.X<<'\t'<<p.Y<<'\t'<<p.Z ;
    p = HeadPosRaw ; qDebug()<<"HeadPosRaw:\t"<<p.X<<'\t'<<p.Y<<'\t'<<p.Z ;
    p = LeftWristPosRaw ; qDebug()<<"LeftWristPosRaw:\t"<<p.X<<'\t'<<p.Y<<'\t'<<p.Z ;
    p = RightWristPosRaw ; qDebug()<<"RightPosRaw:\t"<<p.X<<'\t'<<p.Y<<'\t'<<p.Z ;
    p = LeftAnklePosRaw ; qDebug()<<"LeftAnklePosRaw:\t"<<p.X<<'\t'<<p.Y<<'\t'<<p.Z ;
    p = RightAnklePosRaw ; qDebug()<<"RightAnklePosRaw:\t"<<p.X<<'\t'<<p.Y<<'\t'<<p.Z ;
    qDebug()<<"SelectedJoint is :\t"<<SelectedJointID;
    qDebug()<<"********************";
*/
}

