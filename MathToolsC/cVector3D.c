
#include "math.h"
#include "MathToolsC/cVector3D.h"

cVector3D* cZeroVector (cVector3D *v)
{
    v->X = 0.0 ;
    v->Y = 0.0 ;
    v->Z = 0.0 ;
    return v ;
}

cVector3D* cXYZToVector (cFloat X, cFloat Y, cFloat Z, cVector3D *v)
{
    v->X = X ;
    v->Y = Y ;
    v->Z = Z ;
    return v ;
}

cVector3D* cFloatArrToVector (cFloat*V, cVector3D *v)
{
    v->X = V[0] ;
    v->Y = V[1] ;
    v->Z = V[2] ;
    return v ;
}



cFloat cDotVector (cVector3D *vl, cVector3D *vr)
{
    return (vl->X) * (vr->X) + (vl->Y) * (vr->Y) + (vl->Z) * (vr->Z) ;
}

cVector3D* cCrossVector (cVector3D *vl, cVector3D *vr, cVector3D *v)
{
    v->Z = (vl->X) * (vr->Y) - (vr->X) * (vl->Y) ;
    v->X = (vl->Y) * (vr->Z) - (vr->Y) * (vl->Z) ;
    v->Y = (vl->Z) * (vr->X) - (vr->Z) * (vl->X) ;
    return v ;
}

cVector3D* cAddVector (cVector3D *vl, cVector3D *vr, cVector3D *v)
{
    v->X = (vl->X) + (vr->X) ;
    v->Y = (vl->Y) + (vr->Y) ;
    v->Z = (vl->Z) + (vr->Z) ;
    return v ;
}

cVector3D* cSubVector (cVector3D *vl, cVector3D *vr, cVector3D *v)
{
    v->X = (vl->X) - (vr->X) ;
    v->Y = (vl->Y) - (vr->Y) ;
    v->Z = (vl->Z) - (vr->Z) ;
    return v ;
}

cVector3D* cNegativeVector (cVector3D *vl, cVector3D *v)
{
    v->X = -(vl->X) ;
    v->Y = -(vl->Y) ;
    v->Z = -(vl->Z) ;
    return v ;
}
cVector3D* cNormalizeVector (cVector3D *v)
{
    cFloat lenInv = 1.0f / cGetVectorLength (v) ;
    v->X *= lenInv ;
    v->Y *= lenInv ;
    v->Z *= lenInv ;
    return v ;
}

cVector3D* cScalarMulVector (cFloat s, cVector3D *vl, cVector3D *v)
{
    v->X = vl->X * s ;
    v->Y = vl->Y * s ;
    v->Z = vl->Z * s ;
    return v ;
}

cFloat cGetVectorNorm (cVector3D *v)
{
    return (v->X) * (v->X) + (v->Y) * (v->Y) + (v->Z) * (v->Z) ;
}

cFloat cGetVectorLength (cVector3D *v)
{
    return  cSqrt ( (v->X) * (v->X) + (v->Y) * (v->Y) + (v->Z) * (v->Z) ) ;
}

