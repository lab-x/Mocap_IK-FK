
#include <string.h>
#include "POTDataParser.h"


/////////   Functions need Externs  //////////
static inline uint8_t getKeyStatus()
{
//  return KEY_Scan(1) ;
    return 0 ;
}

static inline uint8_t getNodeID()
{
//  return SELF_ID ;//hmd --6,backpack -5
  return 0 ;
}

static inline uint32_t getTimeStamp (POTDataParser* parser )
{
  return parser->var.ts ;
}

///////////////////////////////////////////////



static void repackData ( POTDataParser* parser )
{
  uint8_t *data = parser->output ;
  int index = 0 ;
  int32_t qx, qy, qz ;
  uint8_t checksum ;
  uint32_t ts ;
  int i ;
  const int packLen = OUTPUT_PACK_LEN ;

  if ( parser->var.Qw < 0 )
  {
    qx = -parser->var.Qx ;
    qy = -parser->var.Qy ;
    qz = -parser->var.Qz ;
  }
  else
  {
    qx = parser->var.Qx ;
    qy = parser->var.Qy ;
    qz = parser->var.Qz ;
  }



  data[index++] = 'P' ;
  data[index++] = ( getNodeID() & 0xf ) | ( (getKeyStatus() & 0xf) << 4 ) ;

  // 位置低 16bits
  data[index++] = ((uint32_t)(parser->var.px) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(parser->var.px) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(parser->var.py) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(parser->var.py) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(parser->var.pz) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(parser->var.pz) & 0xff00) >> 8 ;

  // 四元数
  data[index++] = ((uint32_t)(qx) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qx) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qy) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qy) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qz) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qz) & 0xff00) >> 8 ;
    
//  qx = i32GyroIntegrate[0] ;
//  qy = i32GyroIntegrate[1] ;
//  qz = i32GyroIntegrate[2] ;

  data[index++] = ((uint32_t)(qx) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qx) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qy) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qy) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qz) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qz) & 0xff00) >> 8 ;

  // 时间戳
  ts = getTimeStamp(parser) ;
  data[index++] = ((uint32_t)ts) >> 0 ;
  data[index++] = ((uint32_t)ts) >> 8 ;
  data[index++] = ((uint32_t)ts) >> 16 ;
  data[index++] = ((uint32_t)ts) >> 24 ;

  // info
  data[index++] = ((((uint32_t)(parser->var.px) & 0x030000) >> 16) << 0 ) |
                  ((((uint32_t)(parser->var.py) & 0x030000) >> 16) << 2 ) |
                  ((((uint32_t)(parser->var.pz) & 0x030000) >> 16) << 4 ) |
                  (((uint32_t)(parser->var.LightHouseID-1) & 0x1) << 6 ) |
//                  (((uint32_t)(parser->var.dataValid) & 0x1) << 7 )  ;
                  ( 1 << 7 )  ;


  checksum = 0 ;
  for ( i=0; i<packLen-1; i++ )
    checksum += data[i] ;
  data[index++] = checksum ;
}

static void repackGyroData ( POTDataParser* parser )
{
  uint8_t *data = parser->output ;
  int index = 0 ;
  int32_t qx, qy, qz ;
  uint8_t checksum ;
  uint32_t ts ;
  int i ;
  const int packLen = OUTPUT_PACK_LEN ;

  if ( parser->var.Qw < 0 )
  {
    qx = -parser->var.Qx ;
    qy = -parser->var.Qy ;
    qz = -parser->var.Qz ;
  }
  else
  {
    qx = parser->var.Qx ;
    qy = parser->var.Qy ;
    qz = parser->var.Qz ;
  }



  data[index++] = 'P' ;
  data[index++] = ( getNodeID() & 0xf ) | ( (getKeyStatus() & 0xf) << 4 ) ;

  // 位置低 16bits
  data[index++] = ((uint32_t)(parser->var.px) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(parser->var.px) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(parser->var.py) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(parser->var.py) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(parser->var.pz) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(parser->var.pz) & 0xff00) >> 8 ;

  // 四元数
  data[index++] = ((uint32_t)(qx) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qx) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qy) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qy) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qz) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qz) & 0xff00) >> 8 ;
    
//  qx = i32GyroIntegrate[0] ;
//  qy = i32GyroIntegrate[1] ;
//  qz = i32GyroIntegrate[2] ;

  data[index++] = ((uint32_t)(qx) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qx) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qy) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qy) & 0xff00) >> 8 ;

  data[index++] = ((uint32_t)(qz) & 0x00ff) >> 0 ;
  data[index++] = ((uint32_t)(qz) & 0xff00) >> 8 ;

  // 时间戳
  ts = getTimeStamp(parser) ;
  data[index++] = ((uint32_t)ts) >> 0 ;
  data[index++] = ((uint32_t)ts) >> 8 ;
  data[index++] = ((uint32_t)ts) >> 16 ;
  data[index++] = ((uint32_t)ts) >> 24 ;

  // info
  data[index++] = ((((uint32_t)(parser->var.px) & 0x030000) >> 16) << 0 ) |
                  ((((uint32_t)(parser->var.py) & 0x030000) >> 16) << 2 ) |
                  ((((uint32_t)(parser->var.pz) & 0x030000) >> 16) << 4 ) |
                  (((uint32_t)(parser->var.LightHouseID-1) & 0x1) << 6 ) |
                  ( 0 << 7 )  ;


  checksum = 0 ;
  for ( i=0; i<packLen-1; i++ )
    checksum += data[i] ;
  data[index++] = checksum ;
}
  

static int findHead ( int start, uint8_t* data, int dataLen )
{
  int i ;
  for ( i=start; i<dataLen-1; i++ )
  {
      if ( data[i] == 'P' && data[i+1] == 'T' )
        break ;
  }
  return i ;
}






static int feedByte ( POTDataParser* parser, uint8_t newData )
{
  uint8_t info ;
  int32_t D[8] ;
  uint8_t* d ;

  const int packLen = POT_DATA_PACK_LEN ;
  uint8_t* data = parser->POT_buf ;
  int i , j ;

  parser->POT_buf [ parser->curDataLen ] = newData ;
  parser->curDataLen ++ ;
  
  // buf is not long enough , return directly .
  if ( parser->curDataLen < packLen )
  {
    return 0 ;  // output package not ready .
  }

  // find Head and remove bytes before Head . if the rest data length
  // is less than a package, return .
  i = findHead ( 0, data, packLen ) ;
  parser->curDataLen -= i ;
  memcpy ( parser->POT_buf, parser->POT_buf+i, parser->curDataLen ) ;
  if ( parser->curDataLen < packLen )
    return 0 ;




  // package is completely received , then check if the checksum is correct
#define DO_CHECKSUM
#ifdef DO_CHECKSUM
  if (1)
  {
    int k;
    uint8_t checksum=0 ;
    for ( k=0; k<packLen-1; k++ )
      checksum += data[k] ;
    if ( data[packLen-1] != checksum )
    {
      // CHECKSUM ERROR
      // try find next Head-bytes, and remove the front , and return false .
      i = findHead ( 2, data, packLen ) ;
      parser->curDataLen -= i ;
      memcpy ( parser->POT_buf, parser->POT_buf+i, parser->curDataLen ) ;
      return 0 ;
    }
  }
#endif

  /////////////   PACKAGE VALID /////////////////

  //Detect LightHouseID and Accuracy
  info = *(data+2);
  if ( info == 0x40 )
  {
      parser->var.dataValid = 1 ;
      parser->var.LightHouseID = 1 ;
  }
  else if ( info == 0x80 )
  {
      parser->var.dataValid = 1 ;
      parser->var.LightHouseID = 2 ;
  }
  else
  {
      parser->var.dataValid = 0 ;
  }

  if ( parser->var.dataValid )
  {
    data += 3 ;
    d = (uint8_t*)D ;
    for (j=0; j<8; j++)
    {
        d[4 * j]     = data[4 * j] ;
        d[4 * j + 1] = data[4 * j + 1] ;
        d[4 * j + 2] = data[4 * j + 2] ;
        d[4 * j + 3] = data[4 * j + 3] ;
    }

    parser->var.px = D[0] ;
    parser->var.py = D[1] ;
    parser->var.pz = D[2] ;
    parser->var.Qw = D[3] ;
    parser->var.Qx = D[4] ;
    parser->var.Qy = D[5] ;
    parser->var.Qz = D[6] ;
    parser->var.ts = *(uint32_t*)(&D[7]) ;

    repackData ( parser ) ;
    parser->outputReady = 1 ;
  }
  
  parser->curDataLen = 0 ; // reset the parser .
  return parser->var.dataValid ;    //  output package ready .
}

static void reset (POTDataParser* parser)
{
    memset  ( parser->POT_buf, 0, sizeof(parser->POT_buf) );
    memset  ( parser->output, 0, sizeof(parser->output) );
    parser->curDataLen = 0 ;
    parser->outputReady = 0 ;
}

void initPOTDataParser ( POTDataParser* parser )
{
  parser->repackData = repackData ;
  parser->repackGyroData = repackGyroData ;
  parser->feedByte = feedByte ;
  parser->reset = reset ;

  parser->reset(parser) ;
}


