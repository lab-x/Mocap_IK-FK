#ifndef __MathToolsC__cVector3D__
#define __MathToolsC__cVector3D__

#include "stdint.h"
#include "MathToolsC/cFloat.h"


typedef struct {

    union{
        struct {
            cFloat X ;
            cFloat Y ;
            cFloat Z ;
        };

        cFloat V[3] ;
    };

} cVector3D ;


typedef struct {

    union{
        struct {
            union
            {
                struct {
                    cFloat X ;
                    cFloat Y ;
                    cFloat Z ;
                };

                cVector3D Vec ;
            } ;

            cFloat _1 ;
        };

        cFloat V[4] ;
    };

} cVector3DHomo ;




cVector3D* cZeroVector (cVector3D *v);
cVector3D* cXYZToVector (cFloat X, cFloat Y, cFloat Z, cVector3D *v);
cVector3D* cFloatArrToVector (cFloat*V, cVector3D *v) ;


cFloat cDotVector (cVector3D *vl, cVector3D *vr) ;
cVector3D* cCrossVector (cVector3D *vl, cVector3D *vr, cVector3D *v) ;
cVector3D* cAddVector (cVector3D *vl, cVector3D *vr, cVector3D *v) ;
cVector3D* cSubVector (cVector3D *vl, cVector3D *vr, cVector3D *v) ;
cVector3D* cNegativeVector (cVector3D *vl, cVector3D *v) ;
cVector3D* cNormalizeVector (cVector3D *v) ;
cVector3D* cScalarMulVector (cFloat s, cVector3D *vl, cVector3D *v) ;


cFloat cGetVectorNorm (cVector3D *v) ;
cFloat cGetVectorLength (cVector3D *v) ;




#endif

