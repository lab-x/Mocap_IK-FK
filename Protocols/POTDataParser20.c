#include "POTDataParser20.h"



static int findHead ( int start, uint8_t* data, int dataLen )
{
  int i ;
  for ( i=start; i<dataLen; i++ )
  {
      if ( data[i] == 'P' )
        break ;
  }
  return i ;
}


static int feedByte ( POTDataParser20* parser, uint8_t newData )
{
  const int packLen = POTDataParser20_PkgLen ;
  uint8_t* data = parser->buf ;
  int i , j ;
  uint32_t signMask[3] ;
  parser->buf [ parser->curDataLen ] = newData ;
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
  memcpy ( parser->buf, parser->buf+i, parser->curDataLen ) ;
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
      memcpy ( parser->buf, parser->buf+i, parser->curDataLen ) ;
      return 0 ;
    }
  }
#endif

#define PickupInformation
#ifdef PickupInformation
  parser->var.nodeID = ( data[1] & 0xf ) ;
  parser->var.keyStatus =  (( data[1] >> 4 ) & 0xf) ;
  parser->var.LightHouseID = ( (data[24] >> 6) & 0x1 ) + 1 ;
  parser->var.dataValid =  ((data[24] >> 7) & 0x1)  ;

  parser->var.ts = ((uint32_t)(data[20]) << 0)  |
                    ((uint32_t)(data[21]) << 8)  |
                    ((uint32_t)(data[22]) << 16)  |
                    ((uint32_t)(data[23]) << 24)  ;

  parser->var.Qx = ( data[1] & 0xf ) ;
  parser->var.keyStatus =  (( data[1] >> 4 ) & 0xf) ;
  parser->var.LightHouseID = ( (data[24] >> 6) & 0x1 ) + 1 ;

  *(uint16_t*)&(parser->var.Qx)  =  ((uint16_t)(data[ 8]) << 0)  |  ((uint16_t)(data[ 9]) << 8) ;
  *(uint16_t*)&(parser->var.Qy)  =  ((uint16_t)(data[10]) << 0)  |  ((uint16_t)(data[11]) << 8) ;
  *(uint16_t*)&(parser->var.Qz)  =  ((uint16_t)(data[12]) << 0)  |  ((uint16_t)(data[13]) << 8) ;
  *(uint16_t*)&(parser->var.Qgx) =  ((uint16_t)(data[14]) << 0)  |  ((uint16_t)(data[15]) << 8) ;
  *(uint16_t*)&(parser->var.Qgy) =  ((uint16_t)(data[16]) << 0)  |  ((uint16_t)(data[17]) << 8) ;
  *(uint16_t*)&(parser->var.Qgz) =  ((uint16_t)(data[18]) << 0)  |  ((uint16_t)(data[19]) << 8) ;

  signMask[0] = signMask[1] = signMask[2] = 0 ;
  if ( (data[24]>>0)&0x2 )
    signMask[0] = 0xfffc0000 ;
  if ( (data[24]>>2)&0x2 )
    signMask[1] = 0xfffc0000 ;
  if ( (data[24]>>4)&0x2 )
    signMask[2] = 0xfffc0000 ;

  *(uint32_t*)&(parser->var.px)  =  ((uint32_t)(data[8]) << 0)  |  ((uint32_t)(data[9]) << 8)  |  ((uint32_t)((data[18]>>0)&0x3) << 16) |  signMask[0] ;
  *(uint32_t*)&(parser->var.py)  =  ((uint32_t)(data[10]) << 0)  |  ((uint32_t)(data[11]) << 8) |  ((uint32_t)((data[18]>>2)&0x3) << 16) |  signMask[1] ;
  *(uint32_t*)&(parser->var.pz)  =  ((uint32_t)(data[12]) << 0)  |  ((uint32_t)(data[13]) << 8) |  ((uint32_t)((data[18]>>4)&0x3) << 16) |  signMask[2] ;

  if (1)
  {
    float qw, qx, qy, qz, norm ;
    qx = parser->var.Qx / 10000.0 ;
    qy = parser->var.Qy / 10000.0 ;
    qz = parser->var.Qz / 10000.0 ;
    norm = qx*qx + qy*qy + qz*qz ;
    if ( norm > 1 )
      qw = 0 ;
    else
      qw = sqrt ( 1-norm ) ;
    parser->var.fQw = qw ;
    parser->var.fQx = qx ;
    parser->var.fQy = qy ;
    parser->var.fQz = qz ;


    qx = parser->var.Qgx / 10000.0 ;
    qy = parser->var.Qgy / 10000.0 ;
    qz = parser->var.Qgz / 10000.0 ;
    norm = qx*qx + qy*qy + qz*qz ;
    if ( norm > 1 )
      qw = 0 ;
    else
      qw = sqrt ( 1-norm ) ;
    parser->var.fQgw = qw ;
    parser->var.fQgx = qx ;
    parser->var.fQgy = qy ;
    parser->var.fQgz = qz ;


    parser->var.fPx = parser->var.px / 10000.0 ;
    parser->var.fPy = parser->var.py / 10000.0 ;
    parser->var.fPz = parser->var.pz / 10000.0 ;
  }
#endif




  memcpy (parser->output, parser->buf, sizeof(parser->output)) ;
  parser->curDataLen = 0 ; // reset the parser .
  parser->outputReady = 1 ;
  return 1 ;    //  output package ready .
}


static void reset (POTDataParser20* parser)
{
    memset  ( parser->buf, 0, sizeof(parser->buf) );
    memset  ( parser->output, 0, sizeof(parser->output) );
    parser->curDataLen = 0 ;
    parser->outputReady = 0 ;
}


void initPOTDataParser20 ( POTDataParser20* parser )
{
    parser->reset = reset ;
    parser->feedByte = feedByte ;
    parser->reset(parser) ;
}
