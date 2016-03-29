#ifndef SERIALPROTOCOL
#define SERIALPROTOCOL


#include <stdint.h>

//struct s_SerialDataPKG ;
//struct s_SerialDataParser ;
typedef struct s_SerialDataPKG SerialDataPKG ;
typedef struct s_SerialDataParser SerialDataParser ;
typedef void (*pSerialPkgHandler) ( uint16_t CmdOrReplyCode, void* param, void* parser ) ;

#define SerialDataPKG_HeaderLen (8)
struct s_SerialDataPKG {

    // Header and Checksum .
    // the length of these two fields ('header'  and  'checksum')
    // should be multiple of 4 bytes
    uint8_t header [ SerialDataPKG_HeaderLen ] ;
    uint32_t checksum ;

    // repeat the whole length of the package .
    uint8_t pkgLen ;
    uint8_t pkgLenRepeat ;

    // Command or Reply code .
    union {
      uint16_t  cmd ;
      uint16_t  reply ;
      uint16_t  CmdOrReply ;
    } ;


    // the parameter of a command or reply .
    // the start address of 'param' should be 4 bytes aligned .
    uint8_t param [100] ;

}  ;



struct s_SerialDataParser {

// public :
    void (*feedByte) (SerialDataParser* parser, uint8_t newByte) ;
    void (*setHandler) ( SerialDataParser* parser, pSerialPkgHandler handler) ;
    void (*setHeader) ( SerialDataParser* parser, const uint8_t*header) ;
    void *aux ; // auxiliary infomation , if needed . usually we don't need this .


// private :
    const uint8_t*  acceptHeader ;
    pSerialPkgHandler pkgHandler ;
    uint32_t currentState ;
    int currentPos ;
    int currentPkgLen ;
    uint8_t pkgBuf[sizeof(SerialDataPKG)];    
} ;



void initSerialDataParser ( SerialDataParser* parser ) ;

void setupSerialDataParser ( SerialDataParser* parser, const uint8_t* header, pSerialPkgHandler handler ) ;

int makeSerialDataPKG ( SerialDataPKG*pkg, const uint8_t* header,
                     uint16_t CmdOrReply, void* param, int paramLen ) ;


/***
 *  general demo :
 *
 *      first define a handler function like this ↓
 *      -----------------------
 *      void handler( uint16_t cmd, void* param, void* parser )
 *      {
 *          switch(cmd)
 *          {
 *          case CMD_GET_SENSOR :
 *                  ... // process different commands or replies .
 *                  ...
 *          default :
 *                  break ;
 *          }
 *      }
 *      ------------------------
 *
 *
 *      then init a Parser with the handler and a header :
 *      ------------------------
 *      SerialDataParser Parser ;
 *      setupSerialDataParser(&Parser, "command", handler);
 *      ......
 *      ------------------------
 *
 *
 *      when received a byte , call the 'feedByte' function to post
 *      the byte to the protocol parser .
 *      ------------------------
 *      Parser.feedByte(&Parser, byte) ;
 *      ------------------------
 **/


#endif // SERIALPROTOCOL

