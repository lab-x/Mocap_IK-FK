#ifndef LIGHTHOUSEFUSION_H
#define LIGHTHOUSEFUSION_H

#include "stdint.h"
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Matrix.h"

class LightHouseFusionPrivate ;
class LightHousePOTPrivate ;
class LightHouseFusion
{
    friend class IKMonitor ;

public:
    LightHouseFusion ( LightHousePOTPrivate*p=NULL ) ;
    ~LightHouseFusion () ;

    enum FusionOption {
        Fuse_Non = 0x0,
        Fuse_Pos = 0x1,
        Fuse_Ori = 0x2,
        Fuse_PosOri = Fuse_Pos | Fuse_Ori
    };

    bool Fuse ( bool Fuse_Pos, bool Fuse_Ori ) ;
    void reset () ;
    void setPosSmooth(bool smooth) ;
    void setOriSmooth(bool smooth) ;

    void allocResource() ;
    void releaseResource() ;



    ////  FOR MORE DETAIL FEATUREs
    LightHouseFusionPrivate* Priv()  const ;


private:
    LightHouseFusionPrivate* p ;

};

#endif // LightHouseFusion_H
