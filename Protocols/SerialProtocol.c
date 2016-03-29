#include "string.h"
#include "SerialProtocol.h"


typedef enum {

    WAITING_PKG_HEAD = 0 ,
    WAITING_PKG_LENGTH ,
    WAITING_PKG_TAIL

} SerialDataParserState ;

static inline void resetParser (SerialDataParser* parser)
{
    parser->currentState = WAITING_PKG_HEAD ;
    parser->currentPos = 0 ;
}

static inline int checkPkgHead (SerialDataParser* parser, uint8_t* start , int len)
{
    int i = len - 1 ;
    if (  start[i] == parser->acceptHeader[i]  )
        return 1 ; 	// if match,  return true ;
    else
        return 0 ;  // else return false .
}

static uint32_t getPkgChecksum (SerialDataPKG*pkg)
{
    uint8_t* p = (uint8_t*)&(pkg->pkgLen) ;
    int len = pkg->pkgLen - ( p - (uint8_t*)pkg ) ;
    int i ;
    uint32_t ret = 0 ;
    for ( i=0; i<len; i++ )
    {
        ret += p[i] ;
    }
    return ret ;
}


static void feedByte ( SerialDataParser* parser, uint8_t newData )
{
    SerialDataPKG* pkg = parser->pkgBuf ;
    int bytesBeforeCmdOrReply = (uint8_t*)&(pkg->CmdOrReply) - (uint8_t*)pkg ;
    int bytesBeforeParam = (uint8_t*)(pkg->param) - (uint8_t*)pkg ;


    parser->pkgBuf [parser->currentPos] = newData ;
    parser->currentPos ++ ;

    if (parser->currentState==WAITING_PKG_HEAD)
    {
        while ( parser->currentPos>0 && ! checkPkgHead(parser, parser->pkgBuf,parser->currentPos) )
        {
            // removed the head byte if a wrong header detected , until a probable
            // incomplete header remained or all the bytes removed .
            parser->currentPos -- ;
            memcpy(parser->pkgBuf,parser->pkgBuf+1,parser->currentPos);
        }

        if (parser->currentPos==sizeof(pkg->header))
        {
            // if a integrated header has been received , enter the second
            // stage : wait for pkgLen .
            parser->currentState=WAITING_PKG_LENGTH ;
        }
        return ;
    }

    if(parser->currentPos==bytesBeforeCmdOrReply)
    {
        if ( pkg->pkgLen < bytesBeforeParam  ||
             pkg->pkgLen != pkg->pkgLenRepeat ||
             pkg->pkgLen > sizeof(SerialDataPKG) )
        {
            // if pkgLen is wrong , reset the parser .
            resetParser(parser) ;
            return ;
        }

        // if pkgLen is right , continue parsing .
        parser->currentPkgLen = pkg->pkgLen ;
        parser->currentState = WAITING_PKG_TAIL ;
        return ;
    }

    if(parser->currentPos==parser->currentPkgLen)
    {
        parser->currentPos = 0 ;
        parser->currentPkgLen = 0 ;
        parser->currentState=WAITING_PKG_HEAD ;

        if ( pkg->checksum  ==  getPkgChecksum(pkg) )
            parser->pkgHandler ( pkg->CmdOrReply, pkg->param, parser ) ;
    }
}

static void setHandler ( SerialDataParser* parser, pSerialPkgHandler handler)
{
    parser->pkgHandler = handler ;
}

static void setHeader ( SerialDataParser* parser, const uint8_t*header)
{
    parser->acceptHeader = header ;
}

void initSerialDataParser ( SerialDataParser* parser )
{
    parser->feedByte = feedByte ;
    parser->setHandler = setHandler ;
    parser->setHeader = setHeader ;
    resetParser (parser) ;
}


void setupSerialDataParser ( SerialDataParser* parser, const uint8_t* header, pSerialPkgHandler handler )
{
    initSerialDataParser ( parser ) ;
    parser->setHeader( parser, header ) ;
    parser->setHandler( parser, handler ) ;

}


int makeSerialDataPKG ( SerialDataPKG*pkg, const uint8_t* header,
                     uint16_t CmdOrReply, void* param, int paramLen )
{
    int pkgLen ;
    int bytesBeforeParam = (uint8_t*)(pkg->param) - (uint8_t*)pkg ;
    memcpy (pkg->header,header,sizeof(pkg->header));
    pkg->CmdOrReply = CmdOrReply;
    memcpy(pkg->param,param,paramLen);
    pkgLen = paramLen + bytesBeforeParam ;
    pkg->pkgLen = pkgLen ;
    pkg->pkgLenRepeat = pkg->pkgLen ;
    pkg->checksum = getPkgChecksum (pkg) ;
    return pkgLen ;
}

