
#include "OscilloscopeCore.h"
#include "stdio.h"
#include <QDebug>




OscilloscopeCore::OscilloscopeCore()
{
//    BGColor = qRgb(255,255,255) ;	//背景默认白色
    BGColor = qRgb(0,0,0) ;	//背景默认黑色
    AxisColor = qRgb(30,30,30);	// 坐标轴默认白色，低亮度

    setSelect(false);

    XRange = 2500 ;
    XOffset = 0 ;

    ChannelsCount = MAX_CHANNELS_IN_EACH_SCOPE ;
    ScaleLinesInX = SCALE_LINES_IN_X ;
    ScaleLinesInY = SCALE_LINES_IN_Y ;

	for(int i=0;i<channelsCount();i++)
	{
        channel(i)->setColor ( qRgb ( 255-i*37 , 0 , 0+i*37 ) ) ;
        channel(i)->setShow (false) ;
	}

    TimeStampSyn = false ;
}

OscilloscopeCore::~OscilloscopeCore()
{

}

t_TimeStamp OscilloscopeCore::latestTimeStamp()
{
    t_TimeStamp timeStamp = channel(0)->latestValidTimeStamp() ;
    for (int i=0;i<channelsCount();i++)
    {
        if ( channel(i)->latestValidTimeStamp() > timeStamp )
            timeStamp = channel(i)->latestValidTimeStamp() ;
    }

    return timeStamp ;
}

void OscilloscopeCore::resetAllChannels()
{
    for(int i=0; i<channelsCount();i++)
        channel(i)->reset();
}
void OscilloscopeCore::lockAllChannels()
{
    for(int i=0; i<channelsCount();i++)
        channel(i)->lockSamples();
}
