/*

    Qt OpenGL Tutorial - Lesson 07

    nehewidget.h
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

#ifndef NEHEWIDGET_H
#define NEHEWIDGET_H

#include <QGLWidget>

#include "math.h"
#include <QKeyEvent>
#include <QTimer>
#include <QPointF>

#include "InverseKinematics/fabrik.h"
#include "HumanIK/HumanIK.h"
#include "InverseKinematics/InputAdaptor.h"
extern "C"
{
    #include "MathToolsC/cQuaternion.h"
}



class Virtual3dCharacterViewer : public QGLWidget
{
    Q_OBJECT

public:

  Virtual3dCharacterViewer( QWidget* parent = 0,  bool fs = false , bool keyLimited = false );
  ~Virtual3dCharacterViewer();



protected:

  void initializeGL();
  void paintGL();
  void resizeGL( int width, int height );
  
  void keyPressEvent( QKeyEvent *e );
  void keyReleaseEvent( QKeyEvent *e );
  void loadGLTextures();


  void mousePressEvent(QMouseEvent * event) ;
  void mouseMoveEvent(QMouseEvent * event) ;
  void mouseReleaseEvent(QMouseEvent * event) ;

#ifndef Q_OS_MAC
  void wheelEvent(QWheelEvent * event) ;
#endif

  QPoint LeftButtonPressPos ;
  QPoint LeftButtonReleasePos ;
  QPoint RightButtonPressPos ;
  QPoint RightButtonReleasePos ;
  bool LeftButtonPressed ;
  bool RightButtonPressed ;


private:

  void drawCoordinateAxes (Float Len=1) ;
  void drawOrderlyCuboid() ;
  void drawSingleColorCube(Vector3D Color=Vector3D(1,1,1)) ;
  void getNormal(GLfloat X1, GLfloat Y1, GLfloat Z1 ,
                 GLfloat X2, GLfloat Y2, GLfloat Z2 ,
                 GLfloat *X, GLfloat *Y, GLfloat *Z ) ;


  GLfloat leftBottomX ;
  GLfloat leftBottomY ;
  GLfloat leftBottomZ ;     // 后左下角坐标。坐标值数值最小的点的坐标

  GLfloat xLength ;
  GLfloat yLength ;
  GLfloat zLength ;

protected:

  bool fullscreen;
  QTimer scanTimer ;   // 图像刷新定时器
private slots:
  void show3D();


signals:
//    void jointSelected () ;
//    void jointUnselected();
    void jointMoved();


protected:

  GLuint texture[3];
  bool ENABLE_GL_TEXTURE_2D ;
  GLuint filter;
  
  bool light;




    // kinematics

  // public types
public :
    typedef QPointF Vector2D ;


  bool isComport;


private :


/////
    HumanIK humanIK ;
///



  void triggerIK () ;




  float distanceToRightButtonPressPos ( Vector2D V )
  {
      float dx, dy ;
      dx = V.x() - RightButtonPressPos.x() ;
      dy = V.y() - RightButtonPressPos.y() ;
      return sqrt ( dx*dx + dy*dy ) ;
  }



    struct {
        EulerAngle Ori ;
        Vector3D Pos ;
    } WorldInCamera, WorldInCameraBackup;

    GLfloat WorldMat[4][4] ;
    GLfloat WorldMatInv[4][4] ;

    // EulerAngle of Each Segment .
    // distance between joins of the virtual animated character .
    struct SkeletonModel {
        float LenHipToChest ;
        float LenHipToThigh ;
        float LenThighToKnee ;
        float LenKneeToAnkle ;
        float LenChestToNeck ;
        float LenNeckToShoulder ;
        float LenShoulderToElbow ;
        float LenElbowToWrist ;
        float LenNeckToHead ;
    };

    struct Character3DBody {
    public:
        EulerAngle HipOri ;
        EulerAngle ChestOri ;
        EulerAngle LeftShoulderOri ;
        EulerAngle LeftUpperArmOri ;
        EulerAngle LeftLowerArmOri ;
        EulerAngle LeftHandOri ;
        EulerAngle RightShoulderOri ;
        EulerAngle RightUpperArmOri ;
        EulerAngle RightLowerArmOri ;
        EulerAngle RightHandOri ;
        EulerAngle HeadOri ;
        EulerAngle LeftThighRootOri ;
        EulerAngle LeftThighOri ;
        EulerAngle LeftShankOri ;
        EulerAngle LeftFootOri ;
        EulerAngle RightThighRootOri ;
        EulerAngle RightThighOri ;
        EulerAngle RightShankOri ;
        EulerAngle RightFootOri ;


        Vector3D HipPos ;
        Vector3D NeckPos ;
        Vector3D HeadPos ;
        Vector3D LeftWristPos ;
        Vector3D RightWristPos ;
        Vector3D LeftAnklePos ;
        Vector3D RightAnklePos ;

        Vector3D HipPosInCamera ;
        Vector3D NeckPosInCamera ;
        Vector3D HeadPosInCamera ;
        Vector3D LeftWristPosInCamera ;
        Vector3D RightWristPosInCamera ;
        Vector3D LeftAnklePosInCamera ;
        Vector3D RightAnklePosInCamera ;

        Vector2D HipPosInScreen ;
        Vector2D NeckPosInScreen ;
        Vector2D HeadPosInScreen ;
        Vector2D LeftWristPosInScreen ;
        Vector2D RightWristPosInScreen ;
        Vector2D LeftAnklePosInScreen ;
        Vector2D RightAnklePosInScreen ;

        Vector3D HipPosRaw ;
        Vector3D NeckPosRaw ;
        Vector3D HeadPosRaw ;
        Vector3D LeftWristPosRaw ;
        Vector3D RightWristPosRaw ;
        Vector3D LeftAnklePosRaw ;
        Vector3D RightAnklePosRaw ;

        HumanIK::SkeletonModel Model ;
        GLfloat WorldMatInv[4][4] ;
        Virtual3dCharacterViewer* parent ;

    } ;


    Character3DBody FKResult, IKResult ;
    bool ShowFK, ShowIK ;
    void showFK(bool show=true) {ShowFK=show;}
    void showIK(bool show=true) {ShowIK=show;}
    struct MarkCube {
        bool show ;
        Vector3D Color ;
        Vector3D Pos ;
        EulerAngle Ori ;
    } MarkCubePOT[7], MarkCubeFK[7], MarkCubeFKResult[7], MarkCubeDebugLeftArm[7] ;
    MarkCube* getMarkCubeGroup(int Group) {
        switch(Group)
        {
        case 0: return MarkCubePOT ;
        case 1: return MarkCubeFK  ;
        case 2: return MarkCubeFKResult ;
        case 3: return MarkCubeDebugLeftArm ;
        default: return NULL;
        }
    }

    void drawMarkCube ( int i, int Group=0 ) ;
    void enableMarkCube ( int i, int Group=0 ) {getMarkCubeGroup(Group)[i].show = true ;}
    void disableMarkCube ( int i,int Group=0 ) {getMarkCubeGroup(Group)[i].show = false ;}
    void setMarkCube ( int i, Vector3D Pos, EulerAngle Ori, int Group=0 ) {
        getMarkCubeGroup(Group)[i].Pos = Pos ;
        getMarkCubeGroup(Group)[i].Ori = Ori ;
    }


    static const float PI = 3.14159265f ;




    Vector3D SelectedJointPosInCamera ;
    Vector3D *pSelectedJointPosRaw ;
    bool JointSelected ;
    typedef enum {
        NO_SELECTED_JOINT = 0 ,
        HIP_JOINT ,
        NECK_JOINT ,
        HEAD_JOINT,
        LEFT_WRIST_JOINT,
        RIGHT_WRIST_JOINT,
        LEFT_ANKLE_JOINT,
        RIGHT_ANKLE_JOINT
    } t_SelectedJointID ;
    t_SelectedJointID SelectedJointID ;

    QString JointName (t_SelectedJointID id)
    {
        switch(id)
        {
        case HIP_JOINT :
            return QString("Hip");
        case NECK_JOINT :
            return QString ("Neck");
        case HEAD_JOINT:
            return QString("Head");
        case LEFT_WRIST_JOINT:
            return QString("Left Wrist");
        case RIGHT_WRIST_JOINT:
            return QString("Right Wrist");
        case LEFT_ANKLE_JOINT:
            return QString("Left Ankle");
        case RIGHT_ANKLE_JOINT:
            return QString("Right Ankle");
        default:
            return QString("non joint");
        }
    }

    void updateBonesOriWithIKresult (Character3DBody &body, Vector3D offset=Vector3D(0,0,0)) ;

    // other bone parameters .



    inline void initDefaultJointsDistance ()
    {
        HumanIK::setDefaultModel(IKResult.Model) ;
        HumanIK::setDefaultModel(FKResult.Model) ;
    }



    void initVirtualBody()
    {
        EulerAngle InitAngle (0.0f, 0.0f, 0.0f) ;
        Character3DBody * p = &FKResult ;
        p->HipOri = p->ChestOri = p->LeftShoulderOri = p->LeftUpperArmOri =
                p->LeftLowerArmOri = p->LeftHandOri = p->RightShoulderOri =
                p->RightUpperArmOri = p->RightLowerArmOri = p->RightHandOri =
                p->HeadOri = p->LeftThighRootOri = p->LeftThighOri =
                p->LeftShankOri = p->LeftFootOri = p->RightThighRootOri =
                p->RightThighOri = p->RightShankOri = p->RightFootOri = InitAngle ;
        p->HipPos = Vector3D(1,0,0) ;

        p = &IKResult ;
        p->HipOri = p->ChestOri = p->LeftShoulderOri = p->LeftUpperArmOri =
                p->LeftLowerArmOri = p->LeftHandOri = p->RightShoulderOri =
                p->RightUpperArmOri = p->RightLowerArmOri = p->RightHandOri =
                p->HeadOri = p->LeftThighRootOri = p->LeftThighOri =
                p->LeftShankOri = p->LeftFootOri = p->RightThighRootOri =
                p->RightThighOri = p->RightShankOri = p->RightFootOri = InitAngle ;
        p->HipPos = Vector3D(-1,0,0) ;
    }


public:
    void glDrawBody(Character3DBody& body) ;


    void drawHip (Character3DBody& body) ;
    void drawChest (Character3DBody& body) ;

    void drawHead (Character3DBody& body);

    void drawLeftThighRoot (Character3DBody& body) ;
    void drawLeftThigh (Character3DBody& body) ;
    void drawLeftShank (Character3DBody& body) ;
    void drawLeftFoot (Character3DBody& body) ;

    void drawRightThighRoot (Character3DBody& body) ;
    void drawRightThigh (Character3DBody& body) ;
    void drawRightShank (Character3DBody& body) ;
    void drawRightFoot (Character3DBody& body) ;

    void drawLeftShoulder (Character3DBody& body) ;
    void drawLeftUpperArm (Character3DBody& body) ;
    void drawLeftLowerArm (Character3DBody& body) ;
    void drawLeftHand (Character3DBody& body) ;

    void drawRightShoulder (Character3DBody& body) ;
    void drawRightUpperArm (Character3DBody& body) ;
    void drawRightLowerArm (Character3DBody& body) ;
    void drawRightHand (Character3DBody& body) ;




    void LocateHip (Character3DBody& body) ;

    void RotateHip (Character3DBody& body) ;
    void RotateChest (Character3DBody& body) ;

    void RotateHead (Character3DBody& body);

    void RotateLeftThighRoot (Character3DBody& body) ;
    void RotateLeftThigh (Character3DBody& body) ;
    void RotateLeftShank (Character3DBody& body) ;
    void RotateLeftFoot (Character3DBody& body) ;

    void RotateRightThighRoot (Character3DBody& body) ;
    void RotateRightThigh (Character3DBody& body) ;
    void RotateRightShank (Character3DBody& body) ;
    void RotateRightFoot (Character3DBody& body) ;

    void RotateLeftShoulder (Character3DBody& body) ;
    void RotateLeftUpperArm (Character3DBody& body) ;
    void RotateLeftLowerArm (Character3DBody& body) ;
    void RotateLeftHand (Character3DBody& body) ;

    void RotateRightShoulder (Character3DBody& body) ;
    void RotateRightUpperArm (Character3DBody& body) ;
    void RotateRightLowerArm (Character3DBody& body) ;
    void RotateRightHand (Character3DBody& body) ;



public:

    static Vector2D QPointToVector2D ( QPoint p )
    {
        Vector2D v ;
        v.setX(p.x()) ; v.setY(p.y()) ;
        return v ;
    }


    static void MatrixMul_44X44(float M1[][4], float M2[][4], float M[][4])
    {
        int i, j;
        for (i=0;i<4;i++)
        {
            for (j=0;j<4;j++)
            {
                // 第 i 列第 j 行 M[i][j]
                M[i][j] = M1[0][j]*M2[i][0] +
                        M1[1][j]*M2[i][1] +
                        M1[2][j]*M2[i][2] +
                        M1[3][j]*M2[i][3] ;
            }
        }
    }

    static Vector3D TransformVector (float M1[][4], Vector3D V)
    {
        int j ;
        float M2[4] ;
        float M[4] ;
        Vector3D r ;
        M2[0] = V.getX() ;
        M2[1] = V.getY() ;
        M2[2] = V.getZ() ;
        M2[3] = 1 ;
        for (j=0; j<4; j++)
        {
            M[j] = M1[0][j]*M2[0] +
                    M1[1][j]*M2[1] +
                    M1[2][j]*M2[2] +
                    M1[3][j]*M2[3] ;
        }
        r.setX(M[0]) ;
        r.setY(M[1]) ;
        r.setZ(M[2]) ;
        return r ;
    }
    static void InvTransformation ( float Ms[][4], float Md[][4] )
    {
        int i, j ;
        for (i=0; i<3; i++)
        {
            for (j=0; j<3; j++)
            {
                Md[i][j] = Ms[j][i] ;
            }
        }

        for (i=0;i<3;i++)
        {
            /*
            Md[3][i] = - Ms[0][i] * Ms[3][0] -
                    Ms[1][i] * Ms[3][1] -
                    Ms[2][i] * Ms[3][2] ;
            */
            Md[3][i] = - Md[0][i] * Ms[3][0] -
                    Md[1][i] * Ms[3][1] -
                    Md[2][i] * Ms[3][2] ;
        }

        Md[0][3] = Md[1][3] = Md[2][3] = 0 ;
        Md[3][3] = 1 ;
    }


public:
    Vector3D getCurTranslation ()
    {
        GLfloat M[4][4] ;
        glGetFloatv(GL_MODELVIEW_MATRIX,  (GLfloat*)M);
        GLfloat POS [4] = { 0.0, 0.0, 0.0, 1.0 };
        GLfloat newPOS[4] ;
        int i;
        for ( i=0; i<4; i++ )
        {
            newPOS[i] = M[0][i] * POS[0] +
                    M[1][i] * POS[1] +
                    M[2][i] * POS[2] +
                    M[3][i] * POS[3] ;
        }
        Vector3D r ;
        r.setValues( newPOS[0], newPOS[1], newPOS[2] ) ;
        return r ;
    }


    Vector2D getScreenPos ( Vector3D pos )
    {
        float x = pos.getX() ;
        float y = pos.getY() ;
        float z = pos.getZ() ;

        float X, Y ;
        Y = y/(-z) ;
        int w = width(), h=height();
        X = x/(-z)/((float)w/h) ;

        X *= ((float)w/2) ;
        X += ((float)w/2) ;
        Y *= (-(float)h/2) ;
        Y += ((float)h/2) ;

        Vector2D r ;
        r.setX(X) ;
        r.setY(Y) ;
        return r ;
    }

    Vector3D getHomogeneous ( Vector2D screenPos, float Z )
    {
        float x = screenPos.x() ;
        float y = screenPos.y() ;

        Vector3D r ;
        float w = width(), h=height();
        r.setZ(Z) ;
        r.setY ( ( h/2 - y ) / (h/2) * (-Z) ) ;
        r.setX ( ( x - w/2 ) / (h/2) * (-Z) ) ;
        return r ;
    }



    friend class IKMonitor ;
};

#endif//NEHEWIDGET_H
