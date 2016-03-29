#ifndef __POT_DATA_PARSER__
#define __POT_DATA_PARSER__

#include <stdint.h>


#define POT_DATA_PACK_LEN (36)
#define OUTPUT_PACK_LEN (20+6)

typedef struct s_POTDataParser POTDataParser ;
struct s_POTDataParser {

    void (*repackData) ( POTDataParser* parser ) ;
    void (*repackGyroData) ( POTDataParser* parser ) ;
    int  (*feedByte) (POTDataParser* parser, uint8_t newByte) ;
    void (*reset) (POTDataParser* parser) ;

  uint8_t POT_buf [POT_DATA_PACK_LEN] ;
  uint8_t output [OUTPUT_PACK_LEN] ;
  int curDataLen ;
  int outputReady ;
  

  struct {
    int32_t px ;
    int32_t py ;
    int32_t pz ;
    int32_t Qw ;
    int32_t Qx ;
    int32_t Qy ;
    int32_t Qz ;
    uint32_t ts ;
    int LightHouseID ;
    int dataValid ;
  } var ;

  
  
} ;


void initPOTDataParser ( POTDataParser* parser ) ;

/**
* Demo :
*   POTDataParser parser ;
*   initPOTDataParser ( & parser ) ;
*   ......
*   ......
*   uint8_t newData = readUart() ;
*   if ( feedPOTDataParser ( & parser, newData ) )
*   {
*     UartSendN ( parser.output, 20 ) ;
*   }
*/


#endif
