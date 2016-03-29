
#ifndef __MathToolsC__cQuaternion__
#define __MathToolsC__cQuaternion__

#include "MathToolsC/cFloat.h"
#include "MathToolsC/cVector3D.h"

typedef struct
{
  union {

    struct {
        cFloat w ;
        cFloat x ;
        cFloat y ;
        cFloat z ;
    };

    struct {
        cFloat W ;
        cVector3D V ;
    };

    cFloat v[4] ;

  };
  
} cQuaternion ;


typedef struct
{
    union {
        struct {
            cFloat yaw ;
            cFloat pitch ;
            cFloat roll ;
        };
        cFloat v[3] ;
    };
} cEulerAngle ;


cQuaternion* cMultQuat(cQuaternion* left, cQuaternion* right,cQuaternion*result )  ;
cQuaternion* cConjQuat(cQuaternion* q, cQuaternion* qConj) ;
cQuaternion* cNormalizeQuat (cQuaternion* q, cQuaternion* qNorm) ;
cVector3D* cRotVectorByQuat ( cVector3D *v, cQuaternion*q, cVector3D *vRot) ;


cQuaternion* cMakeRotQuat(cFloat radian, cFloat AxisX, cFloat AxisY, cFloat AxisZ,cQuaternion*result) ;
cQuaternion* cXYZToQuat(cFloat PosX, cFloat PosY, cFloat PosZ,cQuaternion*result) ;
cQuaternion* cVectorToQuat (cVector3D *v, cQuaternion*result ) ;

cQuaternion* cEulerToQuat( cFloat roll, cFloat pitch, cFloat yaw, cQuaternion* result) ;
cEulerAngle* cQuatToEuler ( cQuaternion*quat , cEulerAngle* angle ) ;



#endif
