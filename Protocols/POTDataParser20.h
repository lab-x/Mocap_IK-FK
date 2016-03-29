#ifndef __POT_DATA_PARSER20__
#define __POT_DATA_PARSER20__

#include <stdint.h>


#define POTDataParser20_PkgLen (26)

typedef struct s_POTDataParser20 POTDataParser20 ;
struct s_POTDataParser20 {

  void (*reset) (POTDataParser20* parser) ;
  int (*feedByte) (POTDataParser20* parser, uint8_t newByte) ;

  uint8_t buf [POTDataParser20_PkgLen] ;
  uint8_t output[POTDataParser20_PkgLen] ;
  int curDataLen ;
  int outputReady ;

  struct {
    int32_t px ;
    int32_t py ;
    int32_t pz ;
    int16_t Qx ;
    int16_t Qy ;
    int16_t Qz ;
    int16_t Qgx ;
    int16_t Qgy ;
    int16_t Qgz ;
    uint32_t ts ;
    int LightHouseID ;
    int dataValid ;
    uint8_t nodeID ;
    uint8_t keyStatus ;

    float fPx, fPy, fPz ;
    float fQw, fQx, fQy, fQz ;
    float fQgw, fQgx, fQgy, fQgz ;
  } var ;

} ;

void initPOTDataParser20 ( POTDataParser20* parser ) ;


#endif
