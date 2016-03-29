#ifndef SERIALCMDANDREPLIES
#define SERIALCMDANDREPLIES


#include <stdint.h>

#define CMD_GET_SENSOR   2
#define REPLY_GET_SENSOR  2
typedef struct
{
    uint8_t SlaveAddr;
} CmdParam_GetSensor ;
typedef struct
{
    uint8_t SensorData[26] ;
    uint8_t SlaveAddr;
} ReplyParam_GetSensor ;




#define CMD_SET_HUB_MODE  0xaa
#define REPLY_SET_HUB_MODE  0xaa
typedef struct
{
    uint8_t hubMODE ;
} CmdParam_SetHubMode ;
typedef CmdParam_SetHubMode ReplyParam_SetHubMode;
#define HubMode_Normal  0x0
#define HubMode_Developer 0x1



#endif // SERIALCMDANDREPLIES

