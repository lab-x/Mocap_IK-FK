#ifndef POT_H
#define POT_H

#include "stdint.h"
#include "MathToolsCpp/Quaternion.h"
#include "MathToolsCpp/Vector3D.h"
#include "MathToolsCpp/Matrix.h"

class POTPrivate ;
class LightHousePOTPrivate ;
class LightHouseFusionPrivate ;
class LightHousePOT ;
class LightHouseFusion ;

class POT
{
    friend class IKMonitor ;


public:
    POT();
    ~POT();

    //// MOST OFTEN FUNCTIONs
    void setLHStationNum (int stationNum ) ;
    void setLHTransMat(Float* Mat, int id);
    void setLHTransMat(const Matrix& Mat, int id);
    const Matrix& getLHTransMat(int id);
    bool LoadLHTransMats(const Matrix& Mat);
    bool processRaw ( const Vector3D& rawP, const Quaternion& rawQ,
                      const Quaternion& rawQgyro, int LHStationID,
                      uint32_t ts, bool LHValid ) ;
    bool latestLhValid() ;
    void reset() ;

    void loadRefOri() ;
    void resetRefOri() ;

    Vector3D   getPos() const ;
    Quaternion getOri() const ;


    ////  FOR MORE DETAIL FEATUREs
    POTPrivate* Priv()  const ;
    LightHousePOT* LHPOT()  const ;
    LightHouseFusion* LHFusion()  const ;
    LightHousePOTPrivate* pLHPOT()  const ;
    LightHouseFusionPrivate* pLHFusion()  const ;

private:
    POTPrivate* d ;

};

#endif // POT_H
